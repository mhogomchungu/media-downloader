/*
 *
 *  Copyright (c) 2022
 *  name : Francis Banyikwa
 *  email: mhogomchungu@gmail.com
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QProcess>
#include <QByteArray>

#include <type_traits>

#pragma once

namespace utils
{
	namespace qprocess
	{
		namespace details
		{
			#if __cplusplus >= 201703L
				template<typename Function,typename ... Args>
				using result_of = std::invoke_result_t<Function,Args ...> ;
			#else
				template<typename Function,typename ... Args>
				using result_of = std::result_of_t<Function(Args ...)> ;
			#endif
			template<typename Function,typename ... Args>
			using has_non_void_return_type = std::enable_if_t<!std::is_void<result_of<Function,Args...>>::value,int> ;

			template<typename ReturnType,typename Function,typename ... Args>
			using has_same_return_type = std::enable_if_t<std::is_same<result_of<Function,Args...>,ReturnType>::value,int> ;

			template<typename Function,typename ... Args>
			using has_void_return_type = has_same_return_type<void,Function,Args...> ;
		}

		template< typename WhenCreated,
			  typename WithError,
			  typename WhenStarted,
			  typename WhenDone,
			  typename WithData,
			  details::has_non_void_return_type< WhenCreated,QProcess& > = 0 >
		void run( const QString& cmd,
			  const QStringList& args,
			  WhenCreated whenCreated,
			  WithError withError,
			  WhenStarted whenStarted,
			  WhenDone whenDone,
			  WithData withData )
		{
			class process : public QObject
			{
			public:
				process( const QString& cmd,
					 const QStringList& args,
					 WhenCreated&& whenCreated,
					 WithError&& withError,
					 WhenStarted&& whenStarted,
					 WhenDone&& whenDone,
					 WithData&& withData ) :
					m_withData( std::move( withData ) ),
					m_whenDone( std::move( whenDone ) ),
					m_whenStarted( std::move( whenStarted ) ),
					m_withError( std::move( withError ) ),
					m_data( whenCreated( m_exe ) )
				{
					using cc = void( QProcess::* )( int,QProcess::ExitStatus ) ;

					auto a = &QProcess::errorOccurred ;
					auto b = &QProcess::started ;
					auto c = &QProcess::readyReadStandardOutput ;
					auto d = &QProcess::readyReadStandardError ;
					auto e = static_cast< cc >( &QProcess::finished ) ;

					auto conn = Qt::QueuedConnection ;

					QObject::connect( &m_exe,a,this,&process::withError,conn ) ;
					QObject::connect( &m_exe,b,this,&process::whenStarted,conn ) ;
					QObject::connect( &m_exe,c,this,&process::withStdOut,conn ) ;
					QObject::connect( &m_exe,d,this,&process::withStdError,conn ) ;
					QObject::connect( &m_exe,e,this,&process::whenDone,conn ) ;

					m_exe.start( cmd,args ) ;
				}
			private:
				void withError( QProcess::ProcessError err )
				{
					m_withError( err,m_data ) ;
				}
				void whenStarted()
				{
					m_whenStarted( m_exe,m_data ) ;
				}
				void withStdOut()
				{
					auto a = QProcess::ProcessChannel::StandardOutput ;

					m_withData( a,m_exe.readAllStandardOutput(),m_data ) ;
				}
				void withStdError()
				{
					auto a = QProcess::ProcessChannel::StandardError ;

					m_withData( a,m_exe.readAllStandardError(),m_data ) ;
				}
				void whenDone( int e,QProcess::ExitStatus ss )
				{
					m_whenDone( e,ss,m_data ) ;

					this->deleteLater() ;
				}
				QProcess m_exe ;
				WithData m_withData ;
				WhenDone m_whenDone ;
				WhenStarted m_whenStarted ;
				WithError m_withError ;
				details::result_of< WhenCreated,QProcess& > m_data ;
			};

			new process( cmd,
				     args,
				     std::move( whenCreated ),
				     std::move( withError ),
				     std::move( whenStarted ),
				     std::move( whenDone ),
				     std::move( withData ) ) ;
		}

		template< typename WhenCreated,
			  typename WithError,
			  typename WhenStarted,
			  typename WhenDone,
			  typename WithData,
			  details::has_void_return_type< WhenCreated,QProcess& > = 0 >
		void run( const QString& cmd,
			  const QStringList& args,
			  WhenCreated whenCreated,
			  WithError&& withError,
			  WhenStarted whenStarted,
			  WhenDone whenDone,
			  WithData withData )
		{
			run( cmd,args,[ whenCreated = std::move( whenCreated ) ]( QProcess& exe )mutable{

				   whenCreated( exe ) ;
				   return 0 ;

			},[ withError = std::move( withError ) ]( QProcess::ProcessError e )mutable{

				withError( e ) ;

			},[ whenStarted = std::move( whenStarted ) ]( QProcess& exe,int )mutable{

				whenStarted( exe ) ;

			},[ whenDone = std::move( whenDone ) ]( int e,QProcess::ExitStatus ss,int )mutable{

				whenDone( e,ss ) ;

			},[ withData = std::move( withData ) ]( QProcess::ProcessChannel channel,QByteArray&& data,int )mutable{

				withData( channel,std::move( data ) ) ;
			} ) ;
		}

		struct outPut
		{
			bool success() const
			{
				return exitCode == 0 && exitStatus == outPut::ExitStatus::NormalExit ;
			}
			bool crashed() const
			{
				return exitStatus == outPut::ExitStatus::Crashed ;
			}
			bool failedToStart() const
			{
				return exitStatus == outPut::ExitStatus::FailedToStart ;
			}
			int exitCode ;
			enum class ExitStatus{ FailedToStart,NormalExit,Crashed } exitStatus ;
			QByteArray stdOut ;
			QByteArray stdError ;
		};

		template< typename WhenDone >
		void run( const QString& cmd,
			  const QStringList& args,
			  QProcess::ProcessChannelMode m,
			  WhenDone whenDone )
		{
			run( cmd,args,[ & ]( QProcess& exe )mutable{

				struct context
				{
					context( QProcess::ProcessChannelMode c,WhenDone&& w ) :
						channel( c ),whenDone( std::move( w ) )
					{
					}
					QProcess::ProcessChannelMode channel ;
					QByteArray stdOut ;
					QByteArray stdError ;
					WhenDone whenDone ;
				};

				exe.setProcessChannelMode( m ) ;

				return context( m,std::move( whenDone ) ) ;

			},[]( QProcess::ProcessError err,auto& ctx )mutable{

				if( err == QProcess::ProcessError::FailedToStart ){

					ctx.whenDone( { -1,outPut::ExitStatus::FailedToStart,{},{} } ) ;
				}

			},[]( QProcess&,auto& ){

			},[]( int e,QProcess::ExitStatus ss,auto& ctx )mutable{

				outPut::ExitStatus mm ;

				if( ss == QProcess::ExitStatus::NormalExit ){

					mm = outPut::ExitStatus::NormalExit ;

				}else if( ss == QProcess::ExitStatus::CrashExit ){

					mm = outPut::ExitStatus::Crashed ;
				}else{
					mm = outPut::ExitStatus::FailedToStart ;
				}

				ctx.whenDone( { e,mm,std::move( ctx.stdOut ),std::move( ctx.stdError ) } ) ;

			},[]( QProcess::ProcessChannel c,QByteArray&& data,auto& ctx )mutable{

				if( ctx.channel == QProcess::MergedChannels ){

					ctx.stdOut += std::move( data ) ;
				}else{
					if( c == QProcess::ProcessChannel::StandardOutput ){

						ctx.stdOut += std::move( data ) ;
					}else{
						ctx.stdError += std::move( data ) ;
					}
				}
			} ) ;
		}

		template< typename WhenDone >
		void run( const QString& cmd,const QStringList& args,WhenDone whenDone )
		{
			run( cmd,args,QProcess::SeparateChannels,std::move( whenDone ) ) ;
		}
		
		template< typename FunctionArgs,
			  typename Object,
			  typename Method,
			  typename std::enable_if< std::is_pointer< Object >::value,int >::type = 0,
			  typename std::enable_if< std::is_member_function_pointer< Method >::value,int >::type = 0 >
		void run( const QString& cmd,
			  const QStringList& args,
			  QProcess::ProcessChannelMode mode,
			  FunctionArgs fargs,
			  Object object,
			  Method method )
		{
			run( cmd,args,mode,[ fargs = std::move( fargs ),object,method ]( const utils::qprocess::outPut& s )mutable{
				
				( object->*method )( std::move( fargs ),s ) ;
			} ) ;
		}

		template< typename Object,
			  typename Method,
			  typename std::enable_if< std::is_pointer< Object >::value,int >::type = 0,
			  typename std::enable_if< std::is_member_function_pointer< Method >::value,int >::type = 0 >
		void run( const QString& cmd,
			  const QStringList& args,
			  QProcess::ProcessChannelMode mode,
			  Object object,
			  Method method )
		{
			run( cmd,args,mode,[ object,method ]( const utils::qprocess::outPut& s )mutable{

				( object->*method )( s ) ;
			} ) ;
		}

		template< typename FunctionArgs,
			  typename Object,
			  typename Method,
			  typename std::enable_if< std::is_pointer< Object >::value,int >::type = 0,
			  typename std::enable_if< std::is_member_function_pointer< Method >::value,int >::type = 0 >
		void run( const QString& cmd,
			  const QStringList& args,
			  FunctionArgs fargs,
			  Object object,
			  Method method )
		{
			auto mode = QProcess::SeparateChannels ;

			run( cmd,args,mode,[ fargs = std::move( fargs ),object,method ]( const utils::qprocess::outPut& s )mutable{

				( object->*method )( std::move( fargs ),s ) ;
			} ) ;
		}

		template< typename Object,
			  typename Method,
			  typename std::enable_if< std::is_pointer< Object >::value,int >::type = 0,
			  typename std::enable_if< std::is_member_function_pointer< Method >::value,int >::type = 0 >
		void run( const QString& cmd,
			  const QStringList& args,
			  Object object,
			  Method method )
		{
			auto mode = QProcess::SeparateChannels ;

			run( cmd,args,mode,[ object,method ]( const utils::qprocess::outPut& s )mutable{

				( object->*method )( s ) ;
			} ) ;
		}
	}
}

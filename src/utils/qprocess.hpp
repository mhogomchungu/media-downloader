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
			  typename WhenStarted,
			  typename WhenDone,
			  typename WithData,
			  details::has_non_void_return_type< WhenCreated,QProcess& > = 0 >
		void run( const QString& cmd,
			  const QStringList& args,
			  WhenCreated whenCreated,
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
					 WhenStarted&& whenStarted,
					 WhenDone&& whenDone,
					 WithData&& withData ) :
					m_withData( std::move( withData ) ),
					m_data( whenCreated( m_exe ) )
				{
					QObject::connect( &m_exe,&QProcess::started,
							  [ this,whenStarted = std::move( whenStarted ) ]()mutable{

						whenStarted( m_exe,m_data ) ;
					} ) ;

					QObject::connect( &m_exe,&QProcess::readyReadStandardOutput,[ this ]()mutable{

						m_withData( QProcess::ProcessChannel::StandardOutput,
							    m_exe.readAllStandardOutput(),m_data ) ;
					} ) ;

					QObject::connect( &m_exe,&QProcess::readyReadStandardError,[ this ]()mutable{

						m_withData( QProcess::ProcessChannel::StandardError,
							    m_exe.readAllStandardError(),m_data ) ;
					} ) ;

					using cc = void( QProcess::* )( int,QProcess::ExitStatus ) ;

					auto s = static_cast< cc >( &QProcess::finished ) ;

					QObject::connect( &m_exe,s,[ this,whenDone = std::move( whenDone ) ]
							  ( int e,QProcess::ExitStatus ss )mutable{

						whenDone( e,ss,m_data ) ;

						this->deleteLater() ;
					} ) ;

					m_exe.start( cmd,args ) ;
				}
			private:
				QProcess m_exe ;
				WithData m_withData ;
				details::result_of< WhenCreated,QProcess& > m_data ;
			};

			new process( cmd,
				     args,
				     std::move( whenCreated ),
				     std::move( whenStarted ),
				     std::move( whenDone ),
				     std::move( withData ) ) ;
		}

		template< typename WhenCreated,
			  typename WhenStarted,
			  typename WhenDone,
			  typename WithData,
			  details::has_void_return_type< WhenCreated,QProcess& > = 0 >
		void run( const QString& cmd,
			  const QStringList& args,
			  WhenCreated whenCreated,
			  WhenStarted whenStarted,
			  WhenDone whenDone,
			  WithData withData )
		{
			run( cmd,args,[ whenCreated = std::move( whenCreated ) ]( QProcess& exe )mutable{

				   whenCreated( exe ) ;
				   return 0 ;

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
				return exitCode == 0 && exitStatus == QProcess::ExitStatus::NormalExit ;
			}
			int exitCode ;
			QProcess::ExitStatus exitStatus ;
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
					context( QProcess::ProcessChannelMode c ) :
						channel( c )
					{
					}
					QProcess::ProcessChannelMode channel ;
					QByteArray stdOut ;
					QByteArray stdError ;
				};

				exe.setProcessChannelMode( m ) ;

				return context( m ) ;

			},[]( QProcess&,auto& ){

			},[ whenDone = std::move( whenDone ) ]( int e,QProcess::ExitStatus ss,auto& ctx )mutable{

				whenDone( { e,ss,std::move( ctx.stdOut ),std::move( ctx.stdError ) } ) ;

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
	}
}

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
#include <QString>

#pragma once

namespace utils
{
	namespace qprocess
	{
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
		} ;

		template< typename Events >
		void run( const QString& cmd,const QStringList& args,Events events )
		{
			class process : public QObject
			{
			public:
				process( const QString& cmd,const QStringList& args,Events&& events ) :
					m_events( std::move( events ) )
				{
					m_events.whenCreated( m_exe ) ;

					using cc = void( QProcess::* )( int,QProcess::ExitStatus ) ;					

					auto a = this->errorSignal() ;
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
				auto errorSignal()
				{
				#if QT_VERSION >= QT_VERSION_CHECK( 5,6,0 )
					return &QProcess::errorOccurred ;
				#else
					using aa = void( QProcess::* )( QProcess::ProcessError ) ;
					return static_cast< aa >( &QProcess::error ) ;
				#endif
				}
				void withError( QProcess::ProcessError err )
				{
					m_events.withError( err ) ;

					if( err == QProcess::ProcessError::FailedToStart ){

						this->deleteLater() ;
					}
				}
				void whenStarted()
				{
					m_events.whenStarted( m_exe ) ;
				}
				void withStdOut()
				{
					auto a = QProcess::ProcessChannel::StandardOutput ;

					m_events.withData( a,m_exe.readAllStandardOutput() ) ;
				}
				void withStdError()
				{
					auto a = QProcess::ProcessChannel::StandardError ;

					m_events.withData( a,m_exe.readAllStandardError() ) ;
				}
				void whenDone( int e,QProcess::ExitStatus ss )
				{
					m_events.whenDone( e,ss ) ;

					this->deleteLater() ;
				}
				QProcess m_exe ;
				Events m_events ;
			};

			new process( cmd,args,std::move( events ) ) ;
		}

		template< typename WhenDone >
		void run( const QString& cmd,
			  const QStringList& args,
			  QProcess::ProcessChannelMode m,
			  WhenDone whenDone )
		{
			class events
			{
			public:
				events( WhenDone w,QProcess::ProcessChannelMode m ) :
					m_whenDone( std::move( w ) ),m_mode( m )
				{
				}
				void withError( QProcess::ProcessError err )
				{
					if( err == QProcess::ProcessError::FailedToStart ){

						auto s = outPut::ExitStatus::FailedToStart ;

						m_whenDone( qprocess::outPut{ -1,s,{},{} } ) ;
					}
				}
				void whenStarted( QProcess& )
				{
				}
				void whenCreated( QProcess& )
				{
				}
				void withData( QProcess::ProcessChannel c,const QByteArray& data )
				{
					if( m_mode == QProcess::MergedChannels ){

						m_stdOut += data ;
					}else{
						if( c == QProcess::ProcessChannel::StandardOutput ){

							m_stdOut += data ;
						}else{
							m_stdErr += data ;
						}
					}
				}
				void whenDone( int e,QProcess::ExitStatus ss )
				{
					outPut::ExitStatus mm ;

					if( ss == QProcess::ExitStatus::NormalExit ){

						mm = outPut::ExitStatus::NormalExit ;

					}else if( ss == QProcess::ExitStatus::CrashExit ){

						mm = outPut::ExitStatus::Crashed ;
					}else{
						mm = outPut::ExitStatus::FailedToStart ;
					}

					m_whenDone( this->done( e,mm ) ) ;
				}
			private:
				qprocess::outPut done( int e,outPut::ExitStatus mm )
				{
					return { e,mm,std::move( m_stdOut ),std::move( m_stdErr ) } ;
				}
				QByteArray m_stdOut ;
				QByteArray m_stdErr ;
				WhenDone m_whenDone ;
				QProcess::ProcessChannelMode m_mode ;
			} ;

			run( cmd,args,events( std::move( whenDone ),m ) ) ;
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
			class meaw
			{
			public:
				meaw( FunctionArgs&& args,Object obj,Method method ) :
					m_args( std::move( args ) ),
					m_obj( obj ),
					m_method( method )
				{
				}
				void operator()( const qprocess::outPut& s )
				{
					( m_obj->*m_method )( std::move( m_args ),s ) ;
				}
			private:
				FunctionArgs m_args ;
				Object m_obj ;
				Method m_method ;
			} ;

			run( cmd,args,mode,meaw( std::move( fargs ),object,method ) ) ;
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
			class meaw
			{
			public:
				meaw( Object obj,Method method ) :
					m_obj( obj ),m_method( method )
				{
				}
				void operator()( const qprocess::outPut& s )
				{
					( m_obj->*m_method )( s ) ;
				}
			private:
				Object m_obj ;
				Method m_method ;
			} ;

			run( cmd,args,mode,meaw( object,method ) ) ;
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
			run( cmd,args,QProcess::SeparateChannels,std::move( fargs ),object,method ) ;
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
			run( cmd,args,QProcess::SeparateChannels,object,method ) ;
		}
	}
}

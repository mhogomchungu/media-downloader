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

#include <memory>
#include <iostream>

#include <QTimer>
#include <QApplication>
#include <QByteArray>
#include <QString>
#include <QFile>
#include <QLockFile>
#include <QLocalServer>
#include <QLocalSocket>

namespace utils
{
	namespace app
	{
		namespace details
		{
			class exec : public QObject
			{
				Q_OBJECT
			public:
				template< typename Function >
				exec( Function function ) : m_function( function )
				{
					connect( this,&exec::run,this,&exec::meaw,Qt::QueuedConnection ) ;

					emit this->run() ;
				}
			private:
				void meaw()
				{
					m_function() ;
				}
			signals:
				void run() ;
			private:
				std::function< void() > m_function ;
			} ;
		}
		template< typename Type,typename TypeArgs >
		struct appInfo
		{
			appInfo( TypeArgs t,const QString& s,QApplication& a,QByteArray d = QByteArray() ) :
				args( std::move( t ) ),socketPath( s ),app( a ),data( std::move( d ) )
			{
			}
			using appType = Type ;
			TypeArgs args ;
			QString socketPath ;
			QApplication& app ;
			QByteArray data ;
		} ;

		template< typename AppInfo >
		class multipleInstance
		{
		public:
			multipleInstance( AppInfo info ) :
				m_info( std::move( info ) ),
				m_exec( [ this ](){ this->run() ; } )
			{
			}
			void run()
			{
				m_mainApp = std::make_unique< typename AppInfo::appType >( std::move( m_info.args ) ) ;
				m_mainApp->start( m_info.data ) ;
			}
			int exec()
			{
				return m_info.app.exec() ;
			}
		private:
			AppInfo m_info ;
			details::exec m_exec ;
			std::unique_ptr< typename AppInfo::appType > m_mainApp ;
		} ;

		template< typename AppInfo >
		int runMultiInstances( AppInfo info )
		{
			return multipleInstance< AppInfo >( std::move( info ) ).exec() ;
		}

		template< typename OIR,typename PIC >
		struct instanceArgs
		{
			OIR otherInstanceRunning ;
			PIC otherInstanceCrashed ;
		} ;

		template< typename OIR,typename PIC >
		auto make_oneinstance_args( OIR r,PIC c )
		{
			return instanceArgs< OIR,PIC >{ std::move( r ),std::move( c ) } ;
		}

		template< typename AppInfo,typename InstanceArgs >
		class oneinstance
		{
		public:
			oneinstance( AppInfo info,InstanceArgs iargs ) :
				m_info( std::move( info ) ),
				m_iargs( std::move( iargs ) ),
				m_exec( [ this ](){ this->run() ; } ),
				m_lockFile( m_info.socketPath + ".lock" )
			{
				m_lockFile.lock() ;
			}
			~oneinstance()
			{
				if( m_localServer.isListening() ){

					m_localServer.close() ;
					QFile::remove( m_info.socketPath ) ;
				}
			}
			int exec()
			{
				return m_info.app.exec() ;
			}
		private:
			void run()
			{
				if( QFile::exists( m_info.socketPath ) ){

					QObject::connect( &m_localSocket,&QLocalSocket::connected,[ this ](){

						if( !m_info.data.isEmpty() ){

							m_localSocket.write( m_info.data ) ;
							m_localSocket.waitForBytesWritten() ;
						}

						m_localSocket.close() ;

						m_iargs.otherInstanceRunning() ;

						m_lockFile.unlock() ;

						m_info.app.quit() ;
					} ) ;

				#if QT_VERSION < QT_VERSION_CHECK( 5,15,0 )
					using cs = void( QLocalSocket::* )( QLocalSocket::LocalSocketError ) ;

					QObject::connect( &m_localSocket,static_cast< cs >( &QLocalSocket::error ),[ this ]( QLocalSocket::LocalSocketError ){

						m_iargs.otherInstanceCrashed() ;
						QFile::remove( m_info.socketPath ) ;
						this->start() ;
					} ) ;
				#else
					QObject::connect( &m_localSocket,&QLocalSocket::errorOccurred,[ this ]( QLocalSocket::LocalSocketError ){

						m_iargs.otherInstanceCrashed() ;
						QFile::remove( m_info.socketPath ) ;
						this->start() ;
					} ) ;
				#endif
					m_localSocket.connectToServer( m_info.socketPath ) ;
				}else{
					this->start() ;
				}
			}
			void start()
			{
				m_mainApp = std::make_unique< typename AppInfo::appType >( std::move( m_info.args ) ) ;

				m_mainApp->start( std::move( m_info.data ) ) ;

				QObject::connect( &m_localServer,&QLocalServer::newConnection,[ this ](){

					auto s = m_localServer.nextPendingConnection() ;

					QObject::connect( s,&QLocalSocket::readyRead,[ this,s ]{

						m_mainApp->hasEvent( s->readAll() ) ;
						s->deleteLater() ;
					} ) ;
				} ) ;

				m_localServer.listen( m_info.socketPath ) ;

				m_lockFile.unlock() ;
			}
			QLocalServer m_localServer ;
			QLocalSocket m_localSocket ;
			std::unique_ptr< typename AppInfo::appType > m_mainApp ;
			AppInfo m_info ;
			InstanceArgs m_iargs ;
			details::exec m_exec ;
			QLockFile m_lockFile ;
		} ;

		class AppTypeInterface
		{
		public:
			struct args
			{
				QApplication& app ;
			} ;
			AppTypeInterface( const AppTypeInterface::args& )
			{
			}
			void hasEvent( QByteArray )
			{
				//This method is called with data from another instance that failed
				//to start because this instance prevented it from starting
			}
			void start( QByteArray )
			{
				//This method is called when the first instance is started
			}
		} ;

		template< typename AppInfo,typename Err >
		int runOneInstance( AppInfo info,Err err )
		{
			return oneinstance< AppInfo,Err >( std::move( info ),std::move( err ) ).exec() ;
		}

		template< typename AppInfo >
		int runOneInstance( AppInfo info )
		{
			auto err = make_oneinstance_args( [](){

				std::cout << "There seem to be another instance running,exiting this one" << std::endl ;
			},[](){
				std::cout << "Previous instance seem to have crashed,trying to clean up before starting" << std::endl ;
			} ) ;

			return runOneInstance( std::move( info ),std::move( err ) ) ;
		}
	}
}

/*
 *
 *  Copyright (c) 2021
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

#include "networkAccess.h"

#include "networkAccess.h"
#include "tabmanager.h"
#include "basicdownloader.h"
#include "engines.h"

#include <QtNetwork/QNetworkReply>
#include <QFile>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDir>

#if QT_VERSION >= QT_VERSION_CHECK( 5,4,0 )
static QString _sslLibraryVersionString()
{
	return QSslSocket::sslLibraryBuildVersionString() ;
}
#else
static QString _sslLibraryVersionString()
{
	return {} ;
}
#endif

networkAccess::networkAccess( const Context& ctx ) :
	m_ctx( ctx ),
	m_basicdownloader( m_ctx.TabManager().basicDownloader() ),
	m_tabManager( m_ctx.TabManager() )
{
	if( utility::platformIsWindows() && m_ctx.Settings().showVersionInfoWhenStarting() ){

		auto& e = m_ctx.logger() ;
		auto s = QSslSocket::sslLibraryVersionString() ;

		e.add( QObject::tr( "Checking installed version of" ) + " OpenSSL" ) ;

		if( s.isEmpty() ){

			auto q = _sslLibraryVersionString() ;
			auto m = QObject::tr( "Failed to find version information, make sure \"%1\" is installed and works properly" ).arg( q ) ;
			e.add( m ) ;
		}else{
			e.add( QObject::tr( "Found version" ) + ": " + s ) ;
		}
	}
}

QNetworkRequest networkAccess::networkRequest( const QString& url )
{
	QNetworkRequest networkRequest( url ) ;
#if QT_VERSION >= QT_VERSION_CHECK( 5,9,0 )
	networkRequest.setAttribute( QNetworkRequest::RedirectPolicyAttribute,QNetworkRequest::NoLessSafeRedirectPolicy ) ;
#else
	#if QT_VERSION >= QT_VERSION_CHECK( 5,6,0 )
		networkRequest.setAttribute( QNetworkRequest::FollowRedirectsAttribute,true ) ;
	#endif
#endif
	return networkRequest ;
}

void networkAccess::download( const engines::Iterator& iter,const QString& setDefaultEngine )
{
	const auto& engine = iter.engine() ;

	auto exePath = engine.exePath().realExe() ;

	auto exeFolderPath = [ &exePath ](){

		auto a = QDir::fromNativeSeparators( exePath ) ;

		auto aa = a.lastIndexOf( '/' ) ;

		if( aa != -1 ){

			return a.mid( 0,aa ) ;
		}else{
			return a ;
		}
	}() ;

	auto internalBinPath = QDir::fromNativeSeparators( m_ctx.Engines().engineDirPaths().binPath() ) ;

	if( !exeFolderPath.startsWith( internalBinPath ) ){

		auto m = QDir::fromNativeSeparators( exePath ) ;

		auto a = m.lastIndexOf( '/' ) ;

		if( a == -1 ){

			exePath = internalBinPath + "/" + exePath ;
		}else{
			exePath = internalBinPath + "/" + m.mid(( a + 1 ) ) ;
		}
	}

	QDir dir ;

	auto path = engine.exeFolderPath() ;

	if( !dir.exists( path ) ){

		if( !dir.mkpath( path ) ){

			this->post( engine,QObject::tr( "Failed to download, Following path can not be created: " ) + path ) ;

			return ;
		}
	}

	this->post( engine,QObject::tr( "Start Downloading" ) + " " + engine.name() + " ..." ) ;

	m_tabManager.disableAll() ;

	m_basicdownloader.setAsActive().enableQuit() ;

	QString url( engine.downloadUrl() ) ;

	auto networkReply = m_accessManager.get( this->networkRequest( url ) ) ;

	QObject::connect( networkReply,&QNetworkReply::downloadProgress,[ this,&engine ]( qint64 received,qint64 total ){

		Q_UNUSED( received )
		Q_UNUSED( total )

		this->post( engine,"..." ) ;
	} ) ;

	QObject::connect( networkReply,&QNetworkReply::finished,
			  [ this,networkReply,&engine,iter,exePath,exeFolderPath,setDefaultEngine ](){

		networkReply->deleteLater() ;

		if( networkReply->error() != QNetworkReply::NetworkError::NoError ){

			this->post( engine,QObject::tr( "Download Failed" ) + ": " + networkReply->errorString() ) ;

			m_tabManager.enableAll() ;

			if( iter.hasNext() ){

				m_ctx.versionInfo().check( iter.next() ) ;
			}

			return ;
		}

		networkAccess::metadata metadata ;

		util::Json json( networkReply->readAll() ) ;

		if( !json ){

			this->post( engine,QObject::tr( "Failed to parse json file from github" ) + ": " + json.errorString() ) ;

			m_tabManager.enableAll() ;

			if( iter.hasNext() ){

				m_ctx.versionInfo().check( iter.next() ) ;
			}

			return ;
		}

		auto object = json.doc().object() ;

		auto value = object.value( "assets" ) ;

		const auto array = value.toArray() ;

		for( const auto& it : array ){

			const auto object = it.toObject() ;

			const auto value = object.value( "name" ) ;

			auto entry = value.toString() ;

			if( engine.foundNetworkUrl( entry ) ){

				auto value = object.value( "browser_download_url" ) ;

				metadata.url = value.toString() ;

				value = object.value( "size" ) ;

				metadata.size = value.toInt() ;

				metadata.fileName = entry ;

				break ;
			}
		}

		this->download( { metadata,iter,exePath,exeFolderPath,setDefaultEngine } ) ;
	} ) ;
}

void networkAccess::download( networkAccess::Opts opts )
{
	m_file.setFileName( opts.filePath ) ;

	m_file.remove() ;

	m_file.open( QIODevice::WriteOnly ) ;

	this->post( opts.engine,QObject::tr( "Downloading" ) + ": " + opts.metadata.url ) ;

	this->post( opts.engine,QObject::tr( "Destination" ) + ": " + opts.filePath ) ;

	opts.networkReply = m_accessManager.get( this->networkRequest( opts.metadata.url ) ) ;

	const auto& engine = opts.engine ;

	QObject::connect( opts.networkReply,&QNetworkReply::downloadProgress,
			  [ this,metadata = opts.metadata,networkReply = opts.networkReply,&engine,locale = utility::locale() ]( qint64 received,qint64 total ){

		Q_UNUSED( total )

		m_file.write( networkReply->readAll() ) ;

		auto perc = double( received )  * 100 / double( metadata.size ) ;
		auto totalSize = locale.formattedDataSize( metadata.size ) ;
		auto current   = locale.formattedDataSize( received ) ;
		auto percentage = QString::number( perc,'f',2 ) ;

		auto m = QString( "%1 / %2 (%3%)" ).arg( current,totalSize,percentage ) ;

		this->post( engine,QObject::tr( "Downloading" ) + " " + engine.name() + ": " + m ) ;
	} ) ;

	QObject::connect( opts.networkReply,&QNetworkReply::finished,[ this,opts = std::move( opts ) ]()mutable{

		this->finished( std::move( opts ) ) ;
	} ) ;
}

void networkAccess::finished( networkAccess::Opts str )
{
	str.networkReply->deleteLater() ;

	if( str.networkReply->error() != QNetworkReply::NetworkError::NoError ){

		this->post( str.engine,QObject::tr( "Download Failed" ) + ": " + str.networkReply->errorString() ) ;

		m_tabManager.enableAll() ;

		if( str.iter.hasNext() ){

			m_ctx.versionInfo().check( str.iter.next() ) ;
		}
	}else{
		m_file.close() ;

		this->post( str.engine,QObject::tr( "Download complete" ) ) ;

		if( str.metadata.fileName.endsWith( ".zip" ) || str.metadata.fileName.endsWith( ".tar.gz" ) ){

			this->post( str.engine,QObject::tr( "Extracting archive: " ) + str.filePath ) ;

			QFile::remove( str.exeBinPath ) ;

			QString exe ;
			QStringList args ;

			if( utility::platformIsWindows() ){

				exe = m_ctx.Engines().findExecutable( "7z.exe" ) ;
				args = QStringList{ "x",str.filePath,"-o" + str.exeFolderPath } ;
			}else{
				exe = m_ctx.Engines().findExecutable( "tar" ) ;
				args = QStringList{ "-x","-f",str.filePath,"-C",str.exeFolderPath } ;
			}

			util::run( exe,args,[ this,str = std::move( str ) ]( const util::run_result& s ){

				QFile::remove( str.filePath ) ;

				if( s.success() ){

					QFile f( str.exeBinPath ) ;

					f.setPermissions( f.permissions() | QFileDevice::ExeOwner ) ;

					utility::setDefaultEngine( m_ctx,str.defaultEngine ) ;

					m_ctx.versionInfo().check( str.iter ) ;
				}else{
					this->post( str.engine,s.stdError ) ;

					if( str.iter.hasNext() ){

						m_ctx.versionInfo().check( str.iter.next() ) ;
					}
				}
			} ) ;
		}else{
			this->post( str.engine,QObject::tr( "Renaming file to: " ) + str.exeBinPath ) ;

			QFile::remove( str.exeBinPath ) ;

			m_file.rename( str.exeBinPath ) ;

			m_file.setPermissions( m_file.permissions() | QFileDevice::ExeOwner ) ;

			utility::setDefaultEngine( m_ctx,str.defaultEngine ) ;

			m_ctx.versionInfo().check( str.iter ) ;
		}
	}
}

void networkAccess::post( const engines::engine& engine,const QString& m )
{
	m_ctx.logger().add( [ &engine,&m ]( Logger::Data& s,int id,bool,bool ){

		auto e = m.toUtf8() ;

		Q_UNUSED( id )

		auto p = QObject::tr( "Downloading" ) + " " + engine.name() ;

		auto prefix = p.toUtf8() ;

		if( s.isEmpty() ){

			s.add( "[media-downloader] " + e ) ;

		}else if( e == "..." ){

			s.replaceLast( s.lastText() + " ..." ) ;

		}else if( e.startsWith( prefix ) ){

			if( s.lastText().startsWith( "[media-downloader] " + prefix ) ){

				s.removeLast() ;
			}

			s.add( "[media-downloader] " + e ) ;
		}else{
			s.add( "[media-downloader] " + e ) ;
		}
	},-1 ) ;
}

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
#include "settings.h"

#include "context.hpp"

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
	m_network( m_ctx.Settings().networkTimeOut() ),
	m_basicdownloader( m_ctx.TabManager().basicDownloader() ),
	m_tabManager( m_ctx.TabManager() )
{
	if( utility::platformIsWindows() && m_ctx.Settings().showVersionInfoWhenStarting() ){

		auto& e = m_ctx.logger() ;
		auto s = QSslSocket::sslLibraryVersionString() ;

		auto id = utility::sequentialID() ;

		e.add( QObject::tr( "Checking installed version of" ) + " OpenSSL",id ) ;

		if( s.isEmpty() ){

			auto q = _sslLibraryVersionString() ;
			auto m = QObject::tr( "Failed to find version information, make sure \"%1\" is installed and works properly" ).arg( q ) ;
			e.add( m,id ) ;
		}else{
			e.add( QObject::tr( "Found version" ) + ": " + s,id ) ;
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

QString networkAccess::downloadFailed()
{
	auto m = QString::number( m_ctx.Settings().networkTimeOut() / 1000 ) ;
	return QObject::tr( "Network Failed To Respond Within %1 seconds" ).arg( m ) ;
}

void networkAccess::download( const QByteArray& data,const engines::engine& engine,networkAccess::Opts opts )
{
	networkAccess::metadata metadata ;

	util::Json json( data ) ;

	if( !json ){

		this->post( engine,QObject::tr( "Failed to parse json file from github" ) + ": " + json.errorString(),opts.id ) ;

		m_tabManager.enableAll() ;

		if( opts.iter.hasNext() ){

			m_ctx.getVersionInfo().check( opts.iter.next() ) ;
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

	opts.add( std::move( metadata ) ) ;

	this->download( std::move( opts ) ) ;
}

void networkAccess::download( const engines::Iterator& iter,const QString& setDefaultEngine )
{
	const auto& engine = iter.engine() ;

	auto exeFolderPath = QDir::fromNativeSeparators( m_ctx.Engines().engineDirPaths().binPath() ) ;

	auto m = QDir::fromNativeSeparators( engine.exePath().realExe() ) ;

	auto a = m.lastIndexOf( '/' ) ;

	auto exePath = [ & ](){

		if( a == -1 ){

			return exeFolderPath + "/" + m ;
		}else{
			return exeFolderPath + "/" + m.mid( ( a + 1 ) ) ;
		}
	}() ;

	QDir dir ;

	auto path = engine.exeFolderPath() ;

	int id = utility::sequentialID() ;

	if( !dir.exists( path ) ){

		if( !dir.mkpath( path ) ){

			this->post( engine,QObject::tr( "Failed to download, Following path can not be created: " ) + path,id ) ;

			return ;
		}
	}

	this->post( engine,QObject::tr( "Start Downloading" ) + " " + engine.name() + " ...",id ) ;

	m_tabManager.disableAll() ;

	m_basicdownloader.setAsActive().enableQuit() ;

	networkAccess::Opts opts{ iter,exePath,exeFolderPath,setDefaultEngine,id } ;

	m_network.get( this->networkRequest( engine.downloadUrl() ),[ opts = std::move( opts ),this,&engine ]( const utils::network::progress& p )mutable{

		if( p.finished() ){

			if( p.success() ){

				this->download( p.data(),engine,std::move( opts ) ) ;
			}else{
				auto m = [ & ](){

					if( p.timeOut() ){

						return QObject::tr( "Download Failed" ) + ": " + this->downloadFailed() ;
					}else{
						return QObject::tr( "Download Failed" ) + ": " + p.errorString() ;
					}
				}() ;

				this->post( engine,m,opts.id ) ;

				m_tabManager.enableAll() ;

				if( opts.iter.hasNext() ){

					m_ctx.getVersionInfo().check( opts.iter.next() ) ;
				}
			}
		}else{
			this->post( engine,"...",opts.id ) ;
		}
	} ) ;
}

void networkAccess::download( networkAccess::Opts opts )
{
	const auto& engine = opts.iter.engine() ;

	engine.updateEnginePaths( m_ctx,opts.filePath,opts.exeBinPath,opts.tempPath ) ;

	opts.openFile() ;

	this->post( engine,QObject::tr( "Downloading" ) + ": " + opts.metadata.url,opts.id ) ;

	this->post( engine,QObject::tr( "Destination" ) + ": " + opts.filePath,opts.id ) ;

	auto url = this->networkRequest( opts.metadata.url ) ;

	m_network.get( url,[ this,opts = std::move( opts ),&engine,locale = utility::locale() ]( const utils::network::progress& p )mutable{

		if( p.finished() ){

			if( !p.success() ){

				if( p.timeOut() ){

					opts.networkError = QObject::tr( "Download Failed" ) + ": " + this->downloadFailed() ;  ;
				}else{
					opts.networkError = p.errorString() ;
				}
			}

			this->finished( std::move( opts ) ) ;
		}else{
			opts.file().write( p.data() ) ;

			auto perc = double( p.received() )  * 100 / double( opts.metadata.size ) ;
			auto totalSize = locale.formattedDataSize( opts.metadata.size ) ;
			auto current   = locale.formattedDataSize( p.received() ) ;
			auto percentage = QString::number( perc,'f',2 ) ;

			auto m = QString( "%1 / %2 (%3%)" ).arg( current,totalSize,percentage ) ;

			this->post( engine,QObject::tr( "Downloading" ) + " " + engine.name() + ": " + m,opts.id ) ;
		}
	} ) ;
}

void networkAccess::finished( networkAccess::Opts str )
{
	const auto& engine = str.iter.engine() ;

	if( !str.networkError.isEmpty() ){

		this->post( engine,str.networkError,str.id ) ;

		m_tabManager.enableAll() ;

		if( str.iter.hasNext() ){

			m_ctx.getVersionInfo().check( str.iter.next() ) ;
		}
	}else{
		str.file().close() ;

		this->post( engine,QObject::tr( "Download complete" ),str.id ) ;

		if( str.isArchive ){

			this->extractArchive( engine,std::move( str ) ) ;
		}else{
			this->post( engine,QObject::tr( "Renaming file to: " ) + str.exeBinPath,str.id ) ;

			QFile::remove( str.exeBinPath ) ;

			auto& qfile = str.file() ;

			qfile.rename( str.exeBinPath ) ;

			qfile.setPermissions( qfile.permissions() | QFileDevice::ExeOwner ) ;

			engine.updateCmdPath( str.exeBinPath ) ;

			utility::setDefaultEngine( m_ctx,str.defaultEngine ) ;

			m_ctx.getVersionInfo().check( str.iter ) ;
		}
	}
}

void networkAccess::extractArchive( const engines::engine& engine,networkAccess::Opts str )
{
	this->post( engine,QObject::tr( "Extracting archive: " ) + str.filePath,str.id ) ;

	if( engine.archiveContainsFolder() ){

		auto m = str.tempPath + "/" + engine.name() ;
		//this->post( str.engine,QObject::tr( "Removing Folder: " ) + m,str.id ) ;

		QDir( m ).removeRecursively() ;
	}else{
		QFile::remove( str.exeBinPath ) ;
	}

	auto exe = m_ctx.Engines().findExecutable( utility::platformIsWindows() ? "bsdtar.exe" : "tar" ) ;

	auto args = QStringList{ "-x","-f",str.filePath,"-C",str.tempPath } ;

	utils::qprocess::run( exe,args,[ this,str = std::move( str ) ]( const utils::qprocess::outPut& s ){

		const auto& engine = str.iter.engine() ;

		QFile::remove( str.filePath ) ;

		if( s.success() ){

			if( engine.archiveContainsFolder() ){

				engine.renameArchiveFolder( str.tempPath ) ;

				auto exe = engine.updateCmdPath( str.tempPath ) ;

				QFile f( exe ) ;

				f.setPermissions( f.permissions() | QFileDevice::ExeOwner ) ;
			}else{
				QFile f( str.exeBinPath ) ;

				f.setPermissions( f.permissions() | QFileDevice::ExeOwner ) ;
			}

			utility::setDefaultEngine( m_ctx,str.defaultEngine ) ;

			m_ctx.getVersionInfo().check( str.iter ) ;
		}else{
			this->post( engine,s.stdError,str.id ) ;

			if( str.iter.hasNext() ){

				m_ctx.getVersionInfo().check( str.iter.next() ) ;
			}
		}
	} ) ;
}

void networkAccess::post( const engines::engine& engine,const QString& m,int id )
{	
	m_ctx.logger().add( [ &engine,&m ]( Logger::Data& s,int id,bool ){

		auto e = m.toUtf8() ;

		auto p = QObject::tr( "Downloading" ) + " " + engine.name() ;

		auto prefix = p.toUtf8() ;

		if( s.isEmpty() ){

			s.add( "[media-downloader] " + e,id ) ;

		}else if( e == "..." ){

			auto m = s.getData( id ) ;

			if( m ){

				m.replaceLast( m.lastText() + " ..." ) ;
			}

		}else if( e.startsWith( prefix ) ){

			auto m = s.getData( id ) ;

			if( m ){

				if( m.lastText().startsWith( "[media-downloader] " + prefix ) ){

					m.removeLast() ;
				}
			}

			s.add( "[media-downloader] " + e,id ) ;
		}else{
			s.add( "[media-downloader] " + e,id ) ;
		}
	},id ) ;
}

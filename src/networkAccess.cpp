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
#include "utils/threads.hpp"
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
	m_tabManager( m_ctx.TabManager() ),
	m_appName( m_ctx.appName() )
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

void networkAccess::updateMediaDownloader( int id,networkAccess::Status status ) const
{
	this->post( m_appName,QObject::tr( "Start Downloading" ) + " " + m_ctx.appName() + "...",id ) ;

	auto url = "https://api.github.com/repos/mhogomchungu/media-downloader/releases/latest" ;

	m_tabManager.disableAll() ;

	m_basicdownloader.setAsActive().enableQuit() ;

	m_basicdownloader.setAsActive() ;

	m_network.get( this->networkRequest( url ),[ this,id,status = status.move() ]( const utils::network::progress& p )mutable{

		if( p.finished() ){

			if( p.success() ){

				util::Json json( p.data() ) ;

				if( json ){

					auto object = json.doc().object() ;

					auto value = object.value( "assets" ) ;

					const auto array = value.toArray() ;

					for( const auto& it : array ){

						const auto object = it.toObject() ;

						const auto name = object.value( "name" ).toString() ;

						auto url = object.value( "browser_download_url" ).toString() ;

						if( url.endsWith( ".zip" ) ){

							auto size = object.value( "size" ).toDouble() ;

							updateMDOptions md ;

							md.size = size ;
							md.url  = url ;
							md.id   = id ;
							md.name = name ;
							md.status = status.move() ;

							return this->updateMediaDownloader( md.move() ) ;
						}
					}

					status.done() ;

					this->post( m_appName,QObject::tr( "Failed to parse json file from github" ) + ": " + json.errorString(),id ) ;

					m_tabManager.enableAll() ;
				}
			}else{
				auto m = [ & ](){

					if( p.timeOut() ){

						return QObject::tr( "Download Failed" ) + ": " + this->downloadFailed() ;
					}else{
						return QObject::tr( "Download Failed" ) + ": " + p.errorString() ;
					}
				}() ;

				status.done() ;

				this->post( m_appName,m,id ) ;

				m_tabManager.enableAll() ;
			}
		}else{
			this->post( m_appName,"...",id ) ;
		}
	} ) ;
}

void networkAccess::updateMediaDownloader( networkAccess::updateMDOptions md ) const
{
	md.tmpFile = QDir::fromNativeSeparators( m_ctx.Engines().engineDirPaths().tmp( md.name ) ) ;

	QFile::remove( md.tmpFile ) ;

	this->post( m_appName,QObject::tr( "Downloading" ) + ": " + md.url,md.id ) ;

	this->post( m_appName,QObject::tr( "Destination" ) + ": " + md.tmpFile,md.id ) ;

	auto url = this->networkRequest( md.url ) ;

	auto file = std::make_unique< QFile >( md.tmpFile ) ;

	file->open( QIODevice::WriteOnly ) ;

	m_network.get( url,[ this,file = std::move( file ),locale = utility::locale(),md = md.move() ]( const utils::network::progress& p ){

		if( p.finished() ){

			if( !p.success() ){

				auto m = [ & ](){

					if( p.timeOut() ){

						return QObject::tr( "Download Failed" ) + ": " + this->downloadFailed() ;
					}else{
						return QObject::tr( "Download Failed" ) + ": " + p.errorString() ;
					}
				}() ;

				md.status.done() ;

				this->post( m_appName,m,md.id ) ;

				m_tabManager.enableAll() ;
			}else{
				this->extractMediaDownloader( md.move() ) ;
			}
		}else{
			file->write( p.data() ) ;

			auto perc = double( p.received() )  * 100 / md.size ;
			auto totalSize = locale.formattedDataSize( qint64( md.size ) ) ;
			auto current   = locale.formattedDataSize( p.received() ) ;
			auto percentage = QString::number( perc,'f',2 ) ;

			auto m = QString( "%1 / %2 (%3%)" ).arg( current,totalSize,percentage ) ;

			this->post( m_appName,QObject::tr( "Downloading" ) + " " + m_appName + ": " + m,md.id ) ;
		}
	} ) ;
}

void networkAccess::extractMediaDownloader( networkAccess::updateMDOptions md ) const
{
	this->post( m_appName,QObject::tr( "Extracting archive: " ) + md.tmpFile,md.id ) ;

	const auto& paths = m_ctx.Engines().engineDirPaths() ;

	md.tmpPath = paths.basePath() ;

	md.finalPath = paths.updateNewPath() ;

	auto finalPath = md.finalPath ;

	utils::qthread::run( [ finalPath ](){

		QDir( finalPath ).removeRecursively() ;

	},[ md = md.move(),this ](){

		auto exe = m_ctx.Engines().findExecutable( "bsdtar.exe" ) ;

		auto args = QStringList{ "-x","-f",md.tmpFile,"-C",md.tmpPath } ;

		utils::qprocess::run( exe,args,QProcess::MergedChannels,[ this,md = md.move() ]( const utils::qprocess::outPut& s ){

			QFile::remove( md.tmpFile ) ;

			if( s.success() ){

				QDir dir ;

				auto extractedPath = md.tmpPath + "/" + md.name.mid( 0,md.name.size() - 4 ) ;

				dir.rename( extractedPath,md.finalPath ) ;

				QFile f( md.finalPath + "/media-downloader.exe" ) ;

				f.setPermissions( f.permissions() | QFileDevice::ExeOwner ) ;

				this->removeNotNeededFiles( md.move() ) ;
			}else{
				md.status.done() ;

				auto m = QObject::tr( "Failed To Extract" ) ;

				this->post( m_appName,m + ": " + s.stdOut,md.id ) ;
			}
		} ) ;
	} ) ;
}

QNetworkRequest networkAccess::networkRequest( const QString& url ) const
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

QString networkAccess::downloadFailed() const
{
	auto m = QString::number( m_ctx.Settings().networkTimeOut() / 1000 ) ;
	return QObject::tr( "Network Failed To Respond Within %1 seconds" ).arg( m ) ;
}

void networkAccess::download( const QByteArray& data,
			      const engines::engine& engine,
			      networkAccess::Opts opts ) const
{
	networkAccess::metadata metadata ;

	util::Json json( data ) ;

	if( !json ){

		this->post( engine.name(),QObject::tr( "Failed to parse json file from github" ) + ": " + json.errorString(),opts.id ) ;

		m_tabManager.enableAll() ;

		opts.iter.failed() ;

		if( opts.iter.hasNext() ){

			m_ctx.getVersionInfo().check( opts.iter.next(),opts.showVinfo ) ;
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

			metadata.url = object.value( "browser_download_url" ).toString() ;

			metadata.size = object.value( "size" ).toInt() ;

			metadata.fileName = entry ;

			break ;
		}
	}

	opts.add( std::move( metadata ) ) ;

	this->download( std::move( opts ) ) ;
}

void networkAccess::download( networkAccess::iterator iter,
			      networkAccess::showVersionInfo showVinfo ) const
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

			iter.failed() ;

			this->post( engine.name(),QObject::tr( "Failed to download, Following path can not be created: " ) + path,id ) ;

			return ;
		}
	}

	this->post( engine.name(),QObject::tr( "Start Downloading" ) + " " + engine.name() + " ...",id ) ;

	m_tabManager.disableAll() ;

	m_basicdownloader.setAsActive().enableQuit() ;

	networkAccess::Opts opts{ std::move( iter ),exePath,exeFolderPath,id,showVinfo } ;

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

				this->post( engine.name(),m,opts.id ) ;

				m_tabManager.enableAll() ;

				opts.iter.failed() ;

				if( opts.iter.hasNext() ){

					m_ctx.getVersionInfo().check( opts.iter.next(),opts.showVinfo ) ;
				}else{
					opts.iter.reportDone() ;
				}
			}
		}else{
			this->post( engine.name(),"...",opts.id ) ;
		}
	} ) ;
}

void networkAccess::removeNotNeededFiles( networkAccess::updateMDOptions md ) const
{
	auto folderPath = md.finalPath ;

	utils::qthread::run( [folderPath ](){

		auto entries = directoryManager( folderPath ).readAll() ;

		auto fileIter = entries.fileIter() ;

		while( fileIter.hasNext() ){

			const auto& m = fileIter.valueWithNext() ;

			if( m != "media-downloader.exe" ){

				QFile::remove( folderPath + "/" + m ) ;
			}
		}

		auto folderIter = entries.directoryIter() ;

		while( folderIter.hasNext() ){

			const auto& m = folderIter.valueWithNext() ;

			if( m != "translations" ){

				directoryManager( folderPath + "/" + m ).removeDirectory() ;
			}
		}

	},[ md = md.move(),this ](){

		md.status.done() ;

		auto m = QObject::tr( "Update Complete, Restart To Use New Version" ) ;

		this->post( m_appName,m,md.id ) ;
	} ) ;
}

void networkAccess::download( networkAccess::Opts opts ) const
{
	if( opts.metadata.url.isEmpty() || opts.metadata.fileName.isEmpty() ){

		auto m = QObject::tr( "File Not Found" ) ;

		opts.networkError = QObject::tr( "Download Failed" ) + ": " + m ;

		return this->finished( std::move( opts ) ) ;
	}

	const auto& engine = opts.iter.engine() ;

	engine.updateEnginePaths( m_ctx,opts.filePath,opts.exeBinPath,opts.tempPath ) ;

	opts.openFile() ;

	this->post( engine.name(),QObject::tr( "Downloading" ) + ": " + opts.metadata.url,opts.id ) ;

	this->post( engine.name(),QObject::tr( "Destination" ) + ": " + opts.filePath,opts.id ) ;

	auto url = this->networkRequest( opts.metadata.url ) ;

	m_network.get( url,[ this,opts = std::move( opts ),&engine,locale = utility::locale() ]( const utils::network::progress& p )mutable{

		if( p.finished() ){

			if( !p.success() ){

				opts.iter.failed() ;

				if( p.timeOut() ){

					opts.networkError = QObject::tr( "Download Failed" ) + ": " + this->downloadFailed() ;  ;
				}else{
					opts.networkError = p.errorString() ;
				}
			}

			this->finished( std::move( opts ) ) ;
		}else{
			opts.file().write( p.data() ) ;

			auto total = [ & ](){

				if( opts.metadata.size == 0 ){

					return p.total() ;
				}else{
					return opts.metadata.size ;
				}
			}() ;

			if( total == 0 ){

				auto current = locale.formattedDataSize( p.received() ) ;

				auto m = QString( "%1" ).arg( current ) ;

				this->post( engine.name(),QObject::tr( "Downloading" ) + " " + engine.name() + ": " + m,opts.id ) ;
			}else{
				auto perc = double( p.received() )  * 100 / double( total ) ;
				auto totalSize = locale.formattedDataSize( total ) ;
				auto current   = locale.formattedDataSize( p.received() ) ;
				auto percentage = QString::number( perc,'f',2 ) ;

				auto m = QString( "%1 / %2 (%3%)" ).arg( current,totalSize,percentage ) ;

				this->post( engine.name(),QObject::tr( "Downloading" ) + " " + engine.name() + ": " + m,opts.id ) ;
			}
		}
	} ) ;
}

void networkAccess::finished( networkAccess::Opts str ) const
{
	const auto& engine = str.iter.engine() ;

	if( !str.networkError.isEmpty() ){

		this->post( engine.name(),str.networkError,str.id ) ;

		m_tabManager.enableAll() ;

		if( str.iter.hasNext() ){

			m_ctx.getVersionInfo().check( str.iter.next(),str.showVinfo ) ;
		}else{
			str.iter.reportDone() ;
		}
	}else{
		str.file().close() ;

		this->post( engine.name(),QObject::tr( "Download complete" ),str.id ) ;

		if( str.isArchive ){

			this->extractArchive( engine,std::move( str ) ) ;
		}else{
			this->post( engine.name(),QObject::tr( "Renaming file to: " ) + str.exeBinPath,str.id ) ;

			QFile::remove( str.exeBinPath ) ;

			auto& qfile = str.file() ;

			qfile.rename( str.exeBinPath ) ;

			utility::setPermissions( qfile ) ;

			engine.updateCmdPath( str.exeBinPath ) ;

			auto m = str.showVinfo ;
			m.setAfterDownloading = true ;

			m_ctx.getVersionInfo().check( std::move( str.iter ),m ) ;
		}
	}
}

void networkAccess::extractArchive( const engines::engine& engine,networkAccess::Opts str ) const
{
	this->post( engine.name(),QObject::tr( "Extracting archive: " ) + str.filePath,str.id ) ;

	if( engine.archiveContainsFolder() ){

		auto m = str.tempPath + "/" + engine.name() ;

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

			m_ctx.getVersionInfo().check( str.iter.move(),str.showVinfo ) ;
		}else{
			this->post( engine.name(),s.stdError,str.id ) ;

			if( str.iter.hasNext() ){

				m_ctx.getVersionInfo().check( str.iter.next(),str.showVinfo ) ;
			}else{
				str.iter.reportDone() ;
			}
		}
	} ) ;
}

void networkAccess::post( const QString& engineName,const QString& m,int id ) const
{	
	m_ctx.logger().add( [ engineName,&m ]( Logger::Data& s,int id,bool ){

		auto e = m.toUtf8() ;

		auto p = QObject::tr( "Downloading" ) + " " + engineName ;

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

networkAccess::iter::~iter()
{
}

networkAccess::status::~status()
{
}

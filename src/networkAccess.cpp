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
	auto& settings = m_ctx.Settings() ;

	auto m = settings.showLocalAndLatestVersionInformation() ;
	auto e = settings.showVersionInfoAndAutoDownloadUpdates() ;
	auto s = settings.showLocalVersionInformationOnly() ;

	if( utility::platformIsWindows() && ( m | e | s ) ){

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

static bool _foundMediaDownloader( const QString& url )
{
#if QT_VERSION < QT_VERSION_CHECK( 6,0,0 )
	if( url.contains( "MediaDownloader" ) ){

		if( url.contains( "MediaDownloaderQt6" ) ){

			return false ;
		}else{
			return true ;
		}
	}else{
		return false ;
	}
#else
	return url.contains( "MediaDownloaderQt6" ) ;
#endif
}

void networkAccess::updateMediaDownloader( networkAccess::Status status,const QJsonDocument& json ) const
{
	auto object = json.object() ;

	auto value = object.value( "assets" ) ;

	const auto array = value.toArray() ;

	for( const auto& it : array ){

		const auto object = it.toObject() ;

		const auto name = object.value( "name" ).toString() ;

		auto url = object.value( "browser_download_url" ).toString() ;

		if( _foundMediaDownloader( url ) && url.endsWith( ".zip" ) ){

			auto size = object.value( "size" ).toDouble() ;

			updateMDOptions md ;

			md.size = size ;
			md.url  = url ;
			md.id   = status.id() ;
			md.name = name ;
			md.status = status.move() ;

			return this->updateMediaDownloader( md.move() ) ;
		}
	}

	status.done() ;

	auto m = QObject::tr( "Failed to parse json file from github" ) ;

	this->post( m_appName,m,status.id() ) ;

	m_tabManager.enableAll() ;
}

void networkAccess::updateMediaDownloader( networkAccess::Status status ) const
{
	this->postStartDownloading( m_appName,status.id() ) ;

	m_tabManager.disableAll() ;

	m_basicdownloader.setAsActive().enableQuit() ;

	auto u = this->networkRequest( m_ctx.Settings().gitHubDownloadUrl() ) ;

	this->get( u,status.move(),this,&networkAccess::uMediaDownloaderN ) ;
}

void networkAccess::uMediaDownloaderN( networkAccess::Status& status,
				       const utils::network::progress& p ) const
{
	if( p.finished() ){

		if( p.success() ){

			QJsonParseError err ;

			auto e = QJsonDocument::fromJson( p.data(),&err ) ;

			if( err.error == QJsonParseError::NoError ){

				this->updateMediaDownloader( status.move(),e ) ;
			}else{
				status.done() ;

				auto mm = QObject::tr( "Download Failed" ) ;

				mm += ": " + err.errorString() ;

				this->post( m_appName,mm,status.id() ) ;

				m_tabManager.enableAll() ;
			}
		}else{
			status.done() ;

			this->post( m_appName,this->reportError( p ),status.id() ) ;

			m_tabManager.enableAll() ;
		}
	}else{
		this->post( m_appName,"...",status.id() ) ;
	}
}

void networkAccess::uMediaDownloaderM( networkAccess::updateMDOptions& md,
				       const utils::network::progress& p ) const
{
	if( p.finished() ){

		md.file.close() ;

		if( p.success() ){

			this->extractMediaDownloader( md.move() ) ;
		}else{
			md.status.done() ;

			this->post( m_appName,this->reportError( p ),md.id ) ;

			m_tabManager.enableAll() ;
		}
	}else{
		md.file.write( p.data() ) ;

		auto perc = double( p.received() )  * 100 / md.size ;
		auto totalSize = md.locale.formattedDataSize( qint64( md.size ) ) ;
		auto current   = md.locale.formattedDataSize( p.received() ) ;
		auto percentage = QString::number( perc,'f',2 ) ;

		auto m = QString( "%1 / %2 (%3%)" ).arg( current,totalSize,percentage ) ;

		this->postDownloadingProgress( m_appName,m,md.id ) ;
	}
}

void networkAccess::updateMediaDownloader( networkAccess::updateMDOptions md ) const
{
	auto e = m_ctx.Engines().engineDirPaths().tmp( md.name ) ;

	md.tmpFile = QDir::fromNativeSeparators( e ) ;

	this->postDownloading( m_appName,md.url,md.id ) ;

	this->postDestination( m_appName,md.tmpFile,md.id ) ;

	auto url = this->networkRequest( md.url ) ;

	if( md.file.open( md.tmpFile ) ){

		this->get( url,md.move(),this,&networkAccess::uMediaDownloaderM ) ;
	}else{
		auto m = QObject::tr( "Failed To Open Path For Writing: %1" ).arg( md.tmpFile ) ;

		this->post( m_appName,m,md.id ) ;

		md.status.done() ;

		m_ctx.TabManager().enableAll() ;
	}
}

void networkAccess::emDownloader( networkAccess::updateMDOptions md,
				  const utils::qprocess::outPut& s ) const
{
	QFile::remove( md.tmpFile ) ;

	if( s.success() ){

		QDir dir ;

		auto mm = md.name ;

		auto extractedPath = md.tmpPath + "/" + mm.mid( 0,mm.size() - 4 ) ;

		dir.rename( extractedPath,md.finalPath ) ;

		QFile f( md.finalPath + "/media-downloader.exe" ) ;

		f.setPermissions( f.permissions() | QFileDevice::ExeOwner ) ;

		this->removeNotNeededFiles( md.move() ) ;
	}else{
		md.status.done() ;

		auto m = QObject::tr( "Failed To Extract" ) ;

		this->post( m_appName,m + ": " + s.stdOut,md.id ) ;
	}
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

	},[ md = md.move(),this ]()mutable{

		auto exe = m_ctx.Engines().findExecutable( "bsdtar.exe" ) ;

		auto args = QStringList{ "-x","-f",md.tmpFile,"-C",md.tmpPath } ;

		auto m = QProcess::MergedChannels ;

		utils::qprocess::run( exe,args,m,md.move(),this,&networkAccess::emDownloader ) ;
	} ) ;
}

QNetworkRequest networkAccess::networkRequest( const QString& url ) const
{
	QNetworkRequest networkRequest( url ) ;
#if QT_VERSION >= QT_VERSION_CHECK( 5,9,0 )
	auto a = QNetworkRequest::RedirectPolicyAttribute ;
	auto b = QNetworkRequest::NoLessSafeRedirectPolicy ;

	networkRequest.setAttribute( a,b ) ;
#else
	#if QT_VERSION >= QT_VERSION_CHECK( 5,6,0 )
		auto c = QNetworkRequest::FollowRedirectsAttribute ;
		networkRequest.setAttribute( c,true ) ;
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

		auto m = QObject::tr( "Failed to parse json file from github" ) ;

		this->post( engine.name(),m + ": " + json.errorString(),opts.id ) ;

		m_tabManager.enableAll() ;

		opts.iter.failed() ;

		if( opts.iter.hasNext() ){

			m_ctx.getVersionInfo().check( opts.iter.next(),false ) ;
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

	opts.add( metadata.move() ) ;

	this->download( opts.move() ) ;
}

void networkAccess::download( networkAccess::iterator iter ) const
{
	const auto& engine = iter.engine() ;

	auto ee = m_ctx.Engines().engineDirPaths().binPath() ;

	auto exeFolderPath = QDir::fromNativeSeparators( ee ) ;

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

			auto m = QObject::tr( "Failed to download, Following path can not be created: " ) ;

			this->post( engine.name(),m + path,id ) ;

			return ;
		}
	}

	this->postStartDownloading( engine.name(),id ) ;

	m_tabManager.disableAll() ;

	m_basicdownloader.setAsActive().enableQuit() ;

	networkAccess::Opts opts1{ iter.move(),exePath,exeFolderPath,id } ;

	networkAccess::Opts2 opts2{ engine,opts1.move() } ;

	auto url = this->networkRequest( engine.downloadUrl() ) ;

	this->get( url,opts2.move(),this,&networkAccess::downloadP2 ) ;
}

void networkAccess::downloadP2( networkAccess::Opts2& opts2,
				const utils::network::progress& p ) const
{
	const auto& engine = opts2.engine ;
	auto& opts = opts2.opts ;

	if( p.finished() ){

		if( p.success() ){

			this->download( p.data(),engine,opts.move() ) ;
		}else{
			this->post( engine.name(),this->reportError( p ),opts.id ) ;

			m_tabManager.enableAll() ;

			opts.iter.failed() ;

			if( opts.iter.hasNext() ){

				const auto& g = m_ctx.getVersionInfo() ;
				g.check( opts.iter.next(),false ) ;
			}else{
				opts.iter.reportDone() ;
			}
		}
	}else{
		this->post( engine.name(),"...",opts.id ) ;
	}
}

void networkAccess::removeNotNeededFiles( networkAccess::updateMDOptions md ) const
{
#if 1
	QDir().rmdir( md.finalPath + "/local" ) ;

	md.status.done() ;

	auto m = QObject::tr( "Update Complete, Restart To Use New Version" ) ;

	this->post( m_appName,m,md.id ) ;
#else
	auto folderPath = md.finalPath ;

	utils::qthread::run( [folderPath ](){

		auto entries = directoryManager::readAll( folderPath ) ;

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

				directoryManager::removeDirectory( folderPath + "/" + m ) ;
			}
		}

	},[ md = md.move(),this ](){

		md.status.done() ;

		auto m = QObject::tr( "Update Complete, Restart To Use New Version" ) ;

		this->post( m_appName,m,md.id ) ;
	} ) ;
#endif
}

void networkAccess::download( networkAccess::Opts opts ) const
{
	if( opts.metadata.url.isEmpty() || opts.metadata.fileName.isEmpty() ){

		auto m = QObject::tr( "File Not Found" ) ;

		opts.networkError = QObject::tr( "Download Failed" ) + ": " + m ;

		return this->finished( opts.move() ) ;
	}

	const auto& engine = opts.iter.engine() ;

	engine.updateEnginePaths( m_ctx,opts.filePath,opts.exeBinPath,opts.tempPath ) ;

	if( opts.file.open( opts.filePath ) ){

		this->postDownloading( engine.name(),opts.metadata.url,opts.id ) ;

		this->postDestination( engine.name(),opts.filePath,opts.id ) ;

		auto url = this->networkRequest( opts.metadata.url ) ;

		networkAccess::Opts2 opts2{ engine,opts.move() } ;

		this->get( url,opts2.move(),this,&networkAccess::downloadP ) ;
	}else{
		auto m = QObject::tr( "Failed To Open Path For Writing: %1" ).arg( opts.filePath ) ;

		this->post( engine.name(),m,opts.id ) ;

		opts.iter.reportDone() ;

		m_ctx.TabManager().enableAll() ;
	}
}

void networkAccess::downloadP( networkAccess::Opts2& opts2,
			       const utils::network::progress& p ) const
{
	auto& opts = opts2.opts ;
	const auto& engine = opts2.engine ;

	if( p.finished() ){

		opts.file.close() ;

		if( !p.success() ){

			opts.iter.failed() ;

			opts.networkError = this->reportError( p ) ;
		}

		this->finished( opts.move() ) ;
	}else{
		opts.file.write( p.data() ) ;

		auto total = [ & ](){

			if( opts.metadata.size == 0 ){

				return p.total() ;
			}else{
				return opts.metadata.size ;
			}
		}() ;

		if( total == 0 ){

			auto current = opts.locale.formattedDataSize( p.received() ) ;

			auto m = QString( "%1" ).arg( current ) ;

			this->postDownloading( engine.name(),m,opts.id ) ;
		}else{
			auto perc = double( p.received() )  * 100 / double( total ) ;
			auto totalSize = opts.locale.formattedDataSize( total ) ;
			auto current   = opts.locale.formattedDataSize( p.received() ) ;
			auto percentage = QString::number( perc,'f',2 ) ;

			auto m = QString( "%1 / %2 (%3%)" ).arg( current,totalSize,percentage ) ;

			this->postDownloadingProgress( engine.name(),m,opts.id ) ;
		}
	}
}

void networkAccess::finished( networkAccess::Opts str ) const
{
	const auto& engine = str.iter.engine() ;

	if( !str.networkError.isEmpty() ){

		this->post( engine.name(),str.networkError,str.id ) ;

		m_tabManager.enableAll() ;

		if( str.iter.hasNext() ){

			m_ctx.getVersionInfo().check( str.iter.next(),false ) ;
		}else{
			str.iter.reportDone() ;
		}
	}else{
		this->post( engine.name(),QObject::tr( "Download complete" ),str.id ) ;

		if( str.isArchive ){

			this->extractArchive( engine,str.move() ) ;
		}else{
			auto mm = QObject::tr( "Renaming file to: " ) ;

			this->post( engine.name(),mm + str.exeBinPath,str.id ) ;

			QFile::remove( str.exeBinPath ) ;

			str.file.rename( str.exeBinPath ) ;

			utility::setPermissions( str.file.handle() ) ;

			engine.updateCmdPath( m_ctx.logger(),str.exeBinPath ) ;

			m_ctx.getVersionInfo().check( str.iter.move(),true ) ;
		}
	}
}

void networkAccess::extractArchiveOuput( networkAccess::Opts opts,
					 const utils::qprocess::outPut& s ) const
{
	const auto& engine = opts.iter.engine() ;

	QFile::remove( opts.filePath ) ;

	if( s.success() ){

		if( engine.archiveContainsFolder() ){

			engine.renameArchiveFolder( opts.tempPath ) ;

			auto exe = engine.updateCmdPath( m_ctx.logger(),opts.tempPath ) ;

			QFile f( exe ) ;

			f.setPermissions( f.permissions() | QFileDevice::ExeOwner ) ;
		}else{
			QFile f( opts.exeBinPath ) ;

			f.setPermissions( f.permissions() | QFileDevice::ExeOwner ) ;
		}

		m_ctx.getVersionInfo().check( opts.iter.move(),true ) ;
	}else{
		auto m = QObject::tr( "Failed To Extract" ) ;

		this->post( engine.name(),m + ": " + s.stdError,opts.id ) ;

		if( opts.iter.hasNext() ){

			m_ctx.getVersionInfo().check( opts.iter.next(),true ) ;
		}else{
			opts.iter.reportDone() ;
		}
	}
}

void networkAccess::postStartDownloading( const QString& engineName,int id ) const
{
	auto m = QObject::tr( "Start Downloading" ) + " " + engineName + " ..." ;
	this->post( engineName,m,id ) ;
}

void networkAccess::postDownloading( const QString& engineName,
				     const QString& component,
				     int id ) const
{
	this->post( engineName,QObject::tr( "Downloading" ) + ": " + component,id ) ;
}

void networkAccess::postDestination( const QString& engineName,
				     const QString& component,
				     int id ) const
{
	this->post( engineName,QObject::tr( "Destination" ) + ": " + component,id ) ;
}

void networkAccess::postDownloadingProgress( const QString& name,
					     const QString& cmp,
					     int id ) const
{
	this->post( name,QObject::tr( "Downloading" ) + " " + name + ": " + cmp,id ) ;
}

void networkAccess::extractArchive( const engines::engine& engine,
				    networkAccess::Opts str ) const
{
	auto mm = QObject::tr( "Extracting archive: " ) + str.filePath ;

	this->post( engine.name(),mm,str.id ) ;

	if( engine.archiveContainsFolder() ){

		auto m = str.tempPath + "/" + engine.name() ;

		QDir( m ).removeRecursively() ;
	}else{
		QFile::remove( str.exeBinPath ) ;
	}

	auto cexe = utility::platformIsWindows() ? "bsdtar.exe" : "tar" ;

	auto exe = m_ctx.Engines().findExecutable( cexe ) ;

	if( exe.isEmpty() ){

		auto m = QObject::tr( "Failed To Extract" ) ;

		auto mm = [](){

			if( utility::platformIsWindows() ){

				return QObject::tr( "Failed To Find \"bsdtar.exe\" Executable" ) ;
			}else{
				return QObject::tr( "Failed To Find \"tar\" Executable" ) ;
			}
		}() ;

		this->post( engine.name(),m + ": " + mm,str.id ) ;

		if( str.iter.hasNext() ){

			m_ctx.getVersionInfo().check( str.iter.next(),true ) ;
		}else{
			str.iter.reportDone() ;
		}
	}else{
		auto args = QStringList{ "-x","-f",str.filePath,"-C",str.tempPath } ;

		utils::qprocess::run( exe,args,str.move(),this,&networkAccess::extractArchiveOuput ) ;
	}
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

QString networkAccess::reportError( const utils::network::progress& p ) const
{
	auto mm = QObject::tr( "Download Failed" ) ;

	if( p.timeOut() ){

		return mm + ": " + this->downloadFailed() ;
	}else{
		return mm + ": " + p.errorString() ;
	}
}

networkAccess::iter::~iter()
{
}

networkAccess::status::~status()
{
}

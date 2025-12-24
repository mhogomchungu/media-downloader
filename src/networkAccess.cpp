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

	if( utility::platformIsWindows() && ( m || e || s ) ){

		auto& e = m_ctx.logger() ;
		auto s = QSslSocket::sslLibraryVersionString() ;

		auto id = utility::sequentialID() ;

		auto mm = QObject::tr( "Checking installed version of %1" ) ;

		if( utility::Qt6Version() ){

			e.add( mm.arg( QObject::tr( "Windows' Secure Channel" ) ),id ) ;

			if( !s.isEmpty() ){

				s = util::split( s," " ).last() ;
			}
		}else{
			e.add( mm.arg( "OpenSSL" ),id ) ;
		}

		if( s.isEmpty() ){

			auto q = _sslLibraryVersionString() ;
			auto m = QObject::tr( "Failed to find version information, make sure \"%1\" is installed and works properly" ).arg( q ) ;
			e.add( m,id ) ;
		}else{
			e.add( QObject::tr( "Found version" ) + ": " + s,id ) ;
		}
	}
}

void networkAccess::updateMediaDownloader( networkAccess::Status status,const QJsonDocument& json ) const
{
	class meaw
	{
	public:
		meaw( bool Qt6 ) : m_name( Qt6 ? "MediaDownloaderQt6" : "MediaDownloaderQt5" )
		{
		}
		bool operator()( const QJsonObject& obj )
		{
			auto url = obj.value( "browser_download_url" ).toString() ;

			if( url.contains( "media-downloader-git" ) ){

				return url.contains( m_name + ".git.zip" ) ;
			}else{
				return url.contains( m_name ) && url.endsWith( ".zip" ) ;
			}
		}
	private:
		QString m_name ;
	} ;

	auto obj = utility::parseJsonDataFromGitHub( json,meaw( utility::Qt6Version() ) ) ;

	if( obj.isEmpty() ){

		status.done() ;

		auto m = QObject::tr( "Failed to parse json file from github" ) ;

		this->post( m_appName,m,status.id() ) ;

		m_tabManager.enableAll() ;
	}else{
		this->updateMediaDownloader( networkAccess::updateMDOptions( obj,status.move() ) ) ;
	}
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

			if( md.hash.isEmpty() ){

				auto m = QObject::tr( "Skipping Checking Download Hash" ) ;

				this->post( m_appName,m,md.id ) ;

				this->extractMediaDownloader( md.move() ) ;
			}else{
				auto m = md.hashCalculator->result().toHex().toLower() ;

				if( utility::cliArguments::useFakeMdHash() ){

					m = "bogusHashValue" ;
				}

				if( md.hash == m ){

					this->extractMediaDownloader( md.move() ) ;
				}else{
					this->hashDoNotMatch( md.hash,m,md.id ) ;

					md.status.done() ;

					m_tabManager.enableAll() ;
				}
			}
		}else{
			md.status.done() ;

			this->post( m_appName,this->reportError( p ),md.id ) ;

			m_tabManager.enableAll() ;
		}
	}else{
		auto data = p.data() ;

		md.hashCalculator->addData( data ) ;

		md.file.write( data ) ;

		auto perc       = double( p.received() )  * 100 / md.size ;
		auto totalSize  = md.locale.formattedDataSize( qint64( md.size ) ) ;
		auto current    = md.locale.formattedDataSize( p.received() ) ;
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
	auto mm = utility::removeFile( md.tmpFile ) ;

	if( !mm.isEmpty() ){

		this->failedToRemove( m_appName,md.tmpFile,mm,md.id ) ;
	}

	if( s.success() ){

		auto mm = md.name ;

		auto extractedPath = md.tmpPath + "/" + mm.mid( 0,mm.size() - 4 ) ;

		auto e = utility::rename( extractedPath,md.finalPath ) ;

		if( e.isEmpty() ){

			QFile f( md.finalPath + "/media-downloader.exe" ) ;

			f.setPermissions( f.permissions() | QFileDevice::ExeOwner ) ;

			QDir().rmdir( md.finalPath + "/local" ) ;

			md.status.done() ;

			auto m = QObject::tr( "Update Complete, Restart To Use New Version" ) ;

			this->post( m_appName,m,md.id ) ;
		}else{
			md.status.done() ;

			this->failedToRename( md.name,extractedPath,md.finalPath,e,md.id ) ;
		}
	}else{
		md.status.done() ;

		this->failedToExtract( md.exeArgs,s,md.id ) ;
	}
}

void networkAccess::failedToExtract( const networkAccess::cmdArgs& e,
				     const utils::qprocess::outPut& s,
				     int id ) const
{
	this->post( m_appName,utility::barLine(),id ) ;

	this->post( m_appName,QObject::tr( "Failed To Extract" ),id ) ;
	this->post( m_appName,"Exe Path: " + e.exe(),id ) ;
	this->post( m_appName,"Exe Args: " + e.args(),id ) ;
	this->post( m_appName,"StdOut: "   + s.stdOut,id ) ;
	this->post( m_appName,"StdError: " + s.stdError,id ) ;

	this->post( m_appName,utility::barLine(),id ) ;
}

void networkAccess::failedToRemove( const QString& name,
				    const engines::engine::baseEngine::removeFilesStatus& err,
				    int id ) const
{
	this->post( m_appName,utility::barLine(),id ) ;
	this->post( name,QObject::tr( "Failed To Remove" ),id ) ;

	for( const auto& it : err ){

		this->post( name,"Src: " + it.src(),id ) ;
		this->post( name,"Err: " + it.err(),id ) ;
	}

	this->post( m_appName,utility::barLine(),id ) ;
}

void networkAccess::failedToRemove( const QString& name,
				    const QString& src,
				    const QString& err,
				    int id ) const
{
	this->failedToRemove( name,{ src,err },id ) ;
}

void networkAccess::failedToRename( const QString& name,
				    const QString& src,
				    const QString& dst,
				    const QString& err,
				    int id ) const
{
	this->post( m_appName,utility::barLine(),id ) ;
	this->post( name,QObject::tr( "Failed To Rename" ),id ) ;
	this->post( name,"Src: " + src,id ) ;
	this->post( name,"Dst: " + dst,id ) ;
	this->post( name,"Err: " + err,id ) ;
	this->post( m_appName,utility::barLine(),id ) ;
}

void networkAccess::extractMediaDownloader( networkAccess::updateMDOptions md ) const
{
	this->post( m_appName,QObject::tr( "Extracting archive: " ) + md.tmpFile,md.id ) ;

	const auto& paths = m_ctx.Engines().engineDirPaths() ;

	md.tmpPath = paths.basePath() ;

	md.finalPath = paths.updateNewPath() ;

	class meaw
	{
	public:
		meaw( const networkAccess& na,networkAccess::updateMDOptions md ) :
			m_parent( na ),m_md( md.move() )
		{
		}
		void bg()
		{
			m_err = utility::removeFolder( m_md.finalPath ) ;
		}
		void fg()
		{
			if( !m_err.isEmpty() ){

				m_parent.failedToRemove( m_parent.m_appName,m_md.finalPath,m_err,m_md.id ) ;
			}

			auto exe = m_parent.m_ctx.Engines().findExecutable( "bsdtar.exe" ) ;

			auto args = QStringList{ "-x","-f",m_md.tmpFile,"-C",m_md.tmpPath } ;

			auto m = QProcess::MergedChannels ;

			m_md.exeArgs = { exe,args } ;

			utils::qprocess::run( exe,args,m,m_md.move(),&m_parent,&networkAccess::emDownloader ) ;
		}
	private:
		QString m_err ;
		const networkAccess& m_parent ;
		networkAccess::updateMDOptions m_md ;
	} ;

	utils::qthread::run( meaw( *this,md.move() ) ) ;
}

QNetworkRequest networkAccess::networkRequest( const QString& url,const QByteArray& userAgent ) const
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
	if( !userAgent.isEmpty() ){

		networkRequest.setRawHeader( "User-Agent",userAgent ) ;
	}

	return networkRequest ;
}

QString networkAccess::downloadFailed() const
{
	auto m = QString::number( m_ctx.Settings().networkTimeOut() / 1000 ) ;
	return QObject::tr( "Network Failed To Respond Within %1 seconds" ).arg( m ) ;
}

QByteArray networkAccess::defaultUserAgent() const
{
	return "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/138.0.0.0 Safari/537.36" ;
}

void networkAccess::download( const QByteArray& data,
			      const engines::engine& engine,
			      networkAccess::Opts opts ) const
{
	util::Json json( data ) ;

	if( json ){

		opts.add( engine.parseJsonDataFromGitHub( json.doc() ) ) ;

		this->download( opts.move() ) ;
	}else{
		auto m = QObject::tr( "Failed to parse json file from github" ) ;

		this->post( engine.name(),m + ": " + json.errorString(),opts.id ) ;

		m_tabManager.enableAll() ;

		opts.iter.failed() ;

		if( opts.iter.hasNext() ){

			m_ctx.getVersionInfo().check( opts.iter.next(),false ) ;
		}
	}
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

void networkAccess::download( networkAccess::Opts opts ) const
{
	if( opts.metadata.url().isEmpty() || opts.metadata.fileName().isEmpty() ){

		auto m = QObject::tr( "File Not Found" ) ;

		opts.networkError = QObject::tr( "Download Failed" ) + ": " + m ;

		return this->finished( opts.move() ) ;
	}

	const auto& engine = opts.iter.engine() ;

	engine.updateEnginePaths( m_ctx,opts.filePath,opts.exeBinPath,opts.tempPath ) ;

	if( opts.file.open( opts.filePath ) ){

		this->postDownloading( engine.name(),opts.metadata.url(),opts.id ) ;

		this->postDestination( engine.name(),opts.filePath,opts.id ) ;

		auto url = this->networkRequest( opts.metadata.url() ) ;

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

		if( p.success() ){

			if( opts.metadata.hash().isEmpty() ){

				auto m = QObject::tr( "Skipping Checking Download Hash" ) ;

				this->post( m_appName,m,opts.id ) ;
			}else{
				auto m = opts.hashCalculator->result().toHex().toLower() ;

				if( opts.metadata.hash() != m ){

					this->hashDoNotMatch( opts.metadata.hash(),m,opts.id ) ;

					opts.iter.failed() ;

					opts.networkError = "Bad Download" ;
				}
			}
		}else{
			opts.iter.failed() ;

			opts.networkError = this->reportError( p ) ;
		}

		this->finished( opts.move() ) ;
	}else{
		auto data = p.data() ;

		opts.hashCalculator->addData( data ) ;

		opts.file.write( data ) ;

		auto total = opts.metadata.size() == 0 ? p.total() : opts.metadata.size() ;

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
			auto mm = QObject::tr( "Renaming file to: %1" ).arg( str.exeBinPath ) ;

			this->post( engine.name(),mm,str.id ) ;

			QFileInfo ff( str.exeBinPath ) ;

			if( ff.isDir() ){

				auto m = utility::removeFolder( str.exeBinPath ) ;

				if( !m.isEmpty() ){

					this->failedToRemove( engine.name(),str.exeBinPath,m,str.id ) ;
				}
			}else{
				auto m = utility::removeFile( str.exeBinPath ) ;

				if( !m.isEmpty() ){

					this->failedToRemove( engine.name(),str.exeBinPath,m,str.id ) ;
				}
			}

			auto m = str.file.rename( str.exeBinPath ) ;

			if( m.isEmpty() ){

				utility::setPermissions( str.file.src() ) ;

				engine.updateCmdPath( m_ctx.logger(),str.exeBinPath ) ;

				m_ctx.getVersionInfo().check( str.iter.move(),true ) ;
			}else{
				this->failedToRename( engine.name(),str.file.src(),str.exeBinPath,m,str.id ) ;

				if( str.iter.hasNext() ){

					m_ctx.getVersionInfo().check( str.iter.next(),false ) ;
				}else{
					str.iter.reportDone() ;
				}
			}
		}
	}
}

void networkAccess::extractArchiveOuput( networkAccess::Opts opts,
					 const utils::qprocess::outPut& s ) const
{
	const auto& engine = opts.iter.engine() ;

	if( s.success() ){

		auto err = utility::removeFile( opts.filePath ) ;

		if( !err.isEmpty() ){

			this->failedToRemove( engine.name(),opts.filePath,err,opts.id ) ;
		}

		if( engine.archiveContainsFolder() ){

			auto m = engine.renameArchiveFolder( opts.filePath,opts.tempPath ) ;

			if( m.success() ){

				auto exe = engine.updateCmdPath( m_ctx.logger(),opts.tempPath ) ;

				QFile f( exe ) ;

				f.setPermissions( f.permissions() | QFileDevice::ExeOwner ) ;
			}else{
				this->failedToRename( engine.name(),m.src(),m.dst(),m.err(),opts.id ) ;

				if( opts.iter.hasNext() ){

					m_ctx.getVersionInfo().check( opts.iter.next(),false ) ;
				}else{
					opts.iter.reportDone() ;
				}
			}
		}else{
			QFile f( opts.exeBinPath ) ;

			f.setPermissions( f.permissions() | QFileDevice::ExeOwner ) ;
		}

		m_ctx.getVersionInfo().check( opts.iter.move(),true ) ;
	}else{		
		this->failedToExtract( opts.exeArgs,s,opts.id ) ;

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

void networkAccess::hashDoNotMatch( const QString& hash1,const QString& hash2,int id ) const
{
	this->post( m_appName,utility::barLine(),id ) ;
	this->post( m_appName,QObject::tr( "Ignoring Download Because Hashes Do Not Match" ),id ) ;
	this->post( m_appName,QObject::tr( "Expected \"%1\" but obtained \"%2\"" ).arg( hash1,hash2 ),id ) ;
	this->post( m_appName,utility::barLine(),id ) ;
}

void networkAccess::extractArchive( const engines::engine& engine,
				    networkAccess::Opts str ) const
{
	auto mm = QObject::tr( "Extracting archive: " ) + str.filePath ;

	this->post( engine.name(),mm,str.id ) ;

	if( engine.archiveContainsFolder() ){

		auto m = engine.deleteEngineBinFolder( str.tempPath ) ;

		if( !m.isEmpty() ){

			m = QObject::tr( "Trouble Ahead, Failed To Delete Folder: %1" ).arg( m ) ;

			this->post( engine.name(),m,str.id ) ;
		}
	}else{
		auto m = engine.removeFiles( { str.exeBinPath },QFileInfo( str.exeBinPath ).absolutePath() ) ;

		if( m.size() ){

			this->failedToRemove( engine.name(),m,str.id ) ;
		}
	}

	QStringList extractorArgs ;
	QString extractorExe ;

	if( utility::platformIsWindows() ){

		extractorExe = m_ctx.Engines().findExecutable( "bsdtar.exe" ) ;
		extractorArgs = QStringList{ "-x","-f",str.filePath,"-C",str.tempPath } ;
	}else{
		extractorExe = m_ctx.Engines().findExecutable( "bsdtar" ) ;

		if( extractorExe.isEmpty() ){

			extractorExe = m_ctx.Engines().findExecutable( "unzip" ) ;

			if( !extractorExe.isEmpty() ){

				extractorArgs = QStringList{ str.filePath,"-d",str.tempPath } ;
			}
		}else{
			extractorArgs = QStringList{ "-x","-f",str.filePath,"-C",str.tempPath } ;
		}
	}

	if( extractorExe.isEmpty() ){

		auto m = QObject::tr( "Failed To Extract" ) ;

		auto mm = [](){

			if( utility::platformIsWindows() ){

				return QObject::tr( "Failed To Find \"bsdtar.exe\" Executable" ) ;
			}else{
				return QObject::tr( "Failed To Find \"bsdtar\" or \"unzip\" Executable" ) ;
			}
		}() ;

		this->post( engine.name(),m + ": " + mm,str.id ) ;

		if( str.iter.hasNext() ){

			m_ctx.getVersionInfo().check( str.iter.next(),true ) ;
		}else{
			str.iter.reportDone() ;
		}
	}else{
		const auto& exe = extractorExe ;
		const auto& args = extractorArgs ;

		str.exeArgs = { exe,args } ;

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

QString networkAccess::File::rename( const QString& e )
{
	return utility::rename( m_path,e ) ;
}

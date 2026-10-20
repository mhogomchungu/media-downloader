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
#include <QDateTime>

#include <chrono>

namespace NetworkAccess
{
#if QT_VERSION >= QT_VERSION_CHECK( 5,4,0 )
static QString sslLibraryVersionString()
{
	return QSslSocket::sslLibraryBuildVersionString() ;
}
#else
static QString sslLibraryVersionString()
{
	return {} ;
}
#endif
}

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

		auto id = utility::loggerID() ;

		auto mm = QObject::tr( "Checking installed version of %1" ) ;

		QStringList list ;

		if( utility::Qt6Version() ){

			list.append( mm.arg( QObject::tr( "Windows' Secure Channel" ) ) ) ;

			if( !s.isEmpty() ){

				s = util::split( s," " ).last() ;
			}
		}else{
			list.append( mm.arg( "OpenSSL" ) ) ;
		}

		if( s.isEmpty() ){

			for( const auto& it : list ){

				e.add( it,id ) ;
			}

			auto q = NetworkAccess::sslLibraryVersionString() ;
			auto m = QObject::tr( "Failed to find version information, make sure \"%1\" is installed and works properly" ).arg( q ) ;
			e.add( m,id ) ;
		}else{
			list.append( QObject::tr( "Found version" ) + ": " + s ) ;

			if( utility::cliArguments::debug() ){

				for( const auto& it : list ){

					e.add( it,id ) ;
				}
			}
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

			auto e = utility::jsonDoc( p.data() ) ;

			if( e.valid() ){

				this->updateMediaDownloader( status.move(),e.get() ) ;
			}else{
				status.done() ;

				auto mm = QObject::tr( "Download Failed" ) ;

				mm += ": " + e.errorString() ;

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

		auto speed = md.speed.calculate( p ) ;

		this->postDownloadingProgress( m_appName,speed,md.id ) ;
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

		md.speed.setInitialTimeStamp() ;

		this->get( url,md.move(),this,&networkAccess::uMediaDownloaderM ) ;
	}else{
		auto bar = utility::barLine() ;

		auto m = QObject::tr( "Failed To Open Path For Writing: %1" ).arg( md.tmpFile ) ;

		this->post( m_appName,bar,md.id ) ;
		this->post( m_appName,m,md.id ) ;
		this->post( m_appName,"Err: " + utility::errorMessage(),md.id ) ;
		this->post( m_appName,bar,md.id ) ;

		md.status.done() ;

		m_ctx.TabManager().enableAll() ;
	}
}

void networkAccess::emDownloader1( networkAccess::updateMDOptions md,const utils::qprocess::outPut& s ) const
{
	if( s.success() ){

		class meaw
		{
		public:
			meaw( networkAccess::updateMDOptions m,const networkAccess& n ) :
				m_md( m.move() ),m_parent( n )
			{
				auto mm = m_md.name ;

				m_extractedPath = m_md.tmpPath + "/" + mm.mid( 0,mm.size() - 4 ) ;
			}
			void logError( int )
			{
				m_parent.failedToRename( m_md.name,m_extractedPath,m_md.finalPath,m_status,m_md.id ) ;
			}
			bool bg()
			{
				m_status = utility::rename( m_extractedPath,m_md.finalPath ) ;

				return m_status.isEmpty() ;
			}
			void fg()
			{
				if( m_status.isEmpty() ){

					QFile f( m_md.finalPath + "/media-downloader.exe" ) ;

					f.setPermissions( f.permissions() | QFileDevice::ExeOwner ) ;

					QDir().rmdir( m_md.finalPath + "/local" ) ;

					m_md.status.done() ;

					auto m = QObject::tr( "Update Complete, Restart To Use New Version" ) ;

					m_parent.post( m_parent.m_appName,m,m_md.id ) ;
				}else{
					m_md.status.done() ;
				}
			}
		private:
			QString m_status ;
			networkAccess::updateMDOptions m_md ;
			const networkAccess& m_parent ;
			QString m_extractedPath ;
		} ;

		utility::fileSystemAction( meaw( md.move(),*this ) ) ;
	}else{
		md.status.done() ;

		this->failedToExtract( md.exeArgs,s,md.id ) ;
	}
}

void networkAccess::emDownloader( networkAccess::updateMDOptions md,const utils::qprocess::outPut& s ) const
{
	class meaw
	{
	public:
		meaw( networkAccess::updateMDOptions m,const utils::qprocess::outPut& s,const networkAccess& n ) :
			m_md( m.move() ),m_outPut( s ),m_parent( n )
		{
		}
		bool bg()
		{
			m_status = utility::removeFile( m_md.tmpFile ) ;

			return m_status.isEmpty() ;
		}
		void logError( int )
		{
			m_parent.failedToRemove( m_parent.m_appName,m_md.tmpFile,m_status,m_md.id ) ;
		}
		void fg()
		{
			m_parent.emDownloader1( m_md.move(),m_outPut ) ;
		}
	private:
		QString m_status ;
		networkAccess::updateMDOptions m_md ;
		utils::qprocess::outPut m_outPut ;
		const networkAccess& m_parent ;
	} ;

	utility::fileSystemAction( meaw( md.move(),s,*this ) ) ;
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
		bool bg()
		{
			m_status = utility::removeFolder( m_md.finalPath ) ;

			return m_status.isEmpty() ;
		}
		void logError( int )
		{
			m_parent.failedToRemove( m_parent.m_appName,m_md.finalPath,m_status,m_md.id ) ;
		}
		void fg()
		{
			auto exe = m_parent.m_ctx.Engines().findExecutable( "bsdtar.exe" ) ;

			auto args = QStringList{ "-x","-f",m_md.tmpFile,"-C",m_md.tmpPath } ;

			auto m = QProcess::MergedChannels ;

			m_md.exeArgs = { exe,args } ;

			utils::qprocess::run( exe,args,m,m_md.move(),&m_parent,&networkAccess::emDownloader ) ;
		}
	private:
		QString m_status ;
		const networkAccess& m_parent ;
		networkAccess::updateMDOptions m_md ;
	} ;

	utility::fileSystemAction( meaw( *this,md.move() ) ) ;
}

QNetworkRequest networkAccess::networkRequest( const QString& url,const QByteArray& userAgent,const QByteArray& referer ) const
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

	if( !referer.isEmpty() ){

		networkRequest.setRawHeader( "referer",referer ) ;
	}

	return networkRequest ;
}

QString networkAccess::timeOutErrorString() const
{
	auto m = QString::number( m_ctx.Settings().networkTimeOut() / 1000 ) ;
	return QObject::tr( "Network Failed To Respond Within %1 seconds" ).arg( m ) ;
}

QByteArray networkAccess::defaultUserAgent() const
{
	return "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/138.0.0.0 Safari/537.36" ;
}

void networkAccess::printVersion( networkAccess::Opts opts,bool s ) const
{
	m_ctx.getVersionInfo().check( opts.moveIter(),s ) ;
}

void networkAccess::download( const QByteArray& data,networkAccess::Opts opts ) const
{
	const auto& engine = opts.engine() ;

	util::Json json( data ) ;

	if( engine.parseJsonDataFromGitHub() ){

		if( json ){

			opts.add( engine.parseJsonDataFromGitHub( json.doc() ) ) ;

			return this->download( opts.move() ) ;
		}
	}else{
		auto m = engine.parseJsonDataFromGitHub( data ) ;

		if( m.valid() ){

			opts.add( m ) ;

			return this->download( opts.move() ) ;
		}

	}

	auto m = QObject::tr( "Failed To Parse Data From: %1" ).arg( engine.downloadUrl() ) ;

	this->post( engine.name(),m + ": " + json.errorString(),opts.id ) ;

	m_tabManager.enableAll() ;

	opts.reportFailed() ;

	engine.setBroken() ;

	this->printVersion( opts.move(),false ) ;

}

void networkAccess::download( engines::Iterator e,networkAccess::reportDone rd ) const
{
	networkAccess::iterator iter( e.move(),rd.move() ) ;

	const auto& engine = iter.engine() ;

	auto ee = m_ctx.Engines().engineDirPaths().binPath() ;

	auto exeFolderPath = QDir::fromNativeSeparators( ee ) ;

	auto m = QDir::fromNativeSeparators( engine.exePath().realExe() ) ;

	auto a = m.lastIndexOf( '/' ) ;

	auto exePath = a == -1 ? exeFolderPath + "/" + m : exeFolderPath + "/" + m.mid( a + 1 ) ;

	QDir dir ;

	auto path = engine.exeFolderPath() ;

	int id = utility::loggerID() ;

	if( !dir.exists( path ) ){

		if( !dir.mkpath( path ) ){

			iter.repordFailed() ;

			auto m = QObject::tr( "Failed to download, Following path can not be created: " ) ;

			this->post( engine.name(),m + path,id ) ;

			return ;
		}
	}

	this->postStartDownloading( engine.name(),id ) ;

	m_tabManager.disableAll() ;

	m_basicdownloader.setAsActive().enableQuit() ;

	networkAccess::Opts opts{ iter.move(),exePath,exeFolderPath,id } ;

	auto url = this->networkRequest( engine.downloadUrl() ) ;

	this->get( url,opts.move(),this,&networkAccess::downloadP2 ) ;
}

void networkAccess::downloadP2( networkAccess::Opts& opts,const utils::network::progress& p ) const
{
	const auto& engine = opts.engine() ;

	if( p.finished() ){

		if( p.success() ){

			this->download( p.data(),opts.move() ) ;
		}else{
			this->post( engine.name(),this->reportError( p ),opts.id ) ;

			m_tabManager.enableAll() ;

			opts.reportFailed() ;

			engine.setBroken() ;

			this->printVersion( opts.move(),false ) ;
		}
	}else{
		this->post( engine.name(),"...",opts.id ) ;
	}
}

void networkAccess::download( networkAccess::Opts opts ) const
{
	const auto& engine = opts.engine() ;

	if( opts.metadata.url().isEmpty() || opts.metadata.fileName().isEmpty() ){

		auto a = "Download Failed: Invalid Url or FileName Not Found" ;
		auto b = "Url: " + engine.downloadUrl() ;
		auto c = "Metadata File Name: " + opts.metadata.fileName() ;
		auto d = "Online File Name: " + engine.urlFileName( opts.metadata.version() ) ;
		auto s = utility::barLine() ;

		opts.networkError.add( s,a,b,c,d,s ) ;

		return this->finished( opts.move() ) ;
	}

	engine.updateEnginePaths( m_ctx,opts.filePath,opts.exeBinPath,opts.tempPath ) ;

	if( opts.file.open( opts.filePath ) ){

		this->postDownloading( engine.name(),opts.metadata.url(),opts.id ) ;

		this->postDestination( engine.name(),opts.filePath,opts.id ) ;

		auto url = this->networkRequest( opts.metadata.url() ) ;

		opts.speed.setInitialTimeStamp() ;

		this->get( url,opts.move(),this,&networkAccess::downloadP ) ;
	}else{
		auto m = QObject::tr( "Failed To Open Path For Writing: %1" ).arg( opts.filePath ) ;

		this->post( engine.name(),m,opts.id ) ;

		opts.reportDone() ;

		m_ctx.TabManager().enableAll() ;
	}
}

void networkAccess::downloadP( networkAccess::Opts& opts,const utils::network::progress& p ) const
{
	const auto& engine = opts.engine() ;

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

					opts.reportFailed() ;

					opts.networkError.setbadDownload() ;
				}
			}
		}else{
			opts.reportFailed() ;

			opts.networkError = this->reportError( p ) ;
		}

		this->finished( opts.move() ) ;
	}else{
		auto data = p.data() ;

		opts.hashCalculator->addData( data ) ;

		opts.file.write( data ) ;

		auto speed = opts.speed.calculate( p ) ;

		this->postDownloadingProgress( engine.name(),speed,opts.id ) ;
	}
}

void networkAccess::finished( networkAccess::Opts opts ) const
{
	const auto& engine = opts.engine() ;

	if( opts.networkError.isNotEmpty() ){

		for( const auto& it : opts.networkError ){

			this->post( engine.name(),it,opts.id ) ;
		}

		m_tabManager.enableAll() ;

		engine.setBroken() ;

		this->printVersion( opts.move(),opts.networkError.badDownload() ) ;
	}else{
		this->post( engine.name(),QObject::tr( "Download complete" ),opts.id ) ;

		if( opts.isArchive ){

			this->extractArchive( opts.move() ) ;
		}else{
			auto mm = QObject::tr( "Renaming file to: %1" ).arg( opts.exeBinPath ) ;

			this->post( engine.name(),mm,opts.id ) ;

			QFileInfo ff( opts.exeBinPath ) ;

			if( ff.isDir() ){

				auto m = utility::removeFolder( opts.exeBinPath ) ;

				if( !m.isEmpty() ){

					this->failedToRemove( engine.name(),opts.exeBinPath,m,opts.id ) ;
				}
			}else{
				auto m = utility::removeFile( opts.exeBinPath ) ;

				if( !m.isEmpty() ){

					this->failedToRemove( engine.name(),opts.exeBinPath,m,opts.id ) ;
				}
			}

			auto m = opts.file.rename( opts.exeBinPath ) ;

			if( m.isEmpty() ){

				utility::setPermissions( opts.file.src() ) ;

				engine.updateCmdPath( m_ctx.logger(),opts.exeBinPath ) ;

				this->printVersion( opts.move(),true ) ;
			}else{
				this->failedToRename( engine.name(),opts.file.src(),opts.exeBinPath,m,opts.id ) ;

				engine.setBroken() ;
				this->printVersion( opts.move(),true ) ;
			}
		}
	}
}

void networkAccess::extractArchiveOuput( networkAccess::Opts opts,
					 const utils::qprocess::outPut& s ) const
{
	const auto& engine = opts.engine() ;

	if( s.success() ){

		auto err = utility::removeFile( opts.filePath ) ;

		if( !err.isEmpty() ){

			this->failedToRemove( engine.name(),opts.filePath,err,opts.id ) ;

			engine.setBroken() ;
			this->printVersion( opts.move(),true ) ;

			return ;
		}

		if( engine.archiveContainsFolder() ){

			class meaw
			{
			public:
				meaw( networkAccess::Opts opts,const networkAccess& parent ) :
					m_opts( opts.move() ),
					m_engine( m_opts.engine() ),
					m_parent( parent )
				{
				}
				bool bg()
				{
					m_status = m_engine.renameArchiveFolder( m_opts.filePath,m_opts.tempPath ) ;

					return m_status.success() ;
				}
				void logError( int )
				{
					m_parent.failedToRename( m_engine.name(),m_status.src(),m_status.dst(),m_status.err(),m_opts.id ) ;
				}
				void fg()
				{
					if( m_status.success() ){

						auto exe = m_engine.updateCmdPath( m_parent.m_ctx.logger(),m_opts.tempPath ) ;

						QFile f( exe ) ;

						f.setPermissions( f.permissions() | QFileDevice::ExeOwner ) ;
					}else{
						m_engine.setBroken() ;
					}

					m_parent.printVersion( m_opts.move(),true ) ;
				}
			private:
				networkAccess::Opts m_opts ;
				const engines::engine& m_engine ;
				const networkAccess& m_parent ;
				engines::engine::baseEngine::renameArchiveFolderStatus m_status ;
			} ;

			utility::fileSystemAction( meaw( opts.move(),*this ) ) ;
		}else{
			QFile f( opts.exeBinPath ) ;

			f.setPermissions( f.permissions() | QFileDevice::ExeOwner ) ;

			this->printVersion( opts.move(),true ) ;
		}		
	}else{		
		this->failedToExtract( opts.exeArgs,s,opts.id ) ;

		engine.setBroken() ;
		this->printVersion( opts.move(),true ) ;
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

void networkAccess::extractArchive1( networkAccess::Opts opts ) const
{
	const engines::engine& engine = opts.engine() ;

	QStringList extractorArgs ;
	QString extractorExe ;

	if( utility::platformIsWindows() ){

		extractorExe = m_ctx.Engines().findExecutable( "bsdtar.exe" ) ;
		extractorArgs = QStringList{ "-x","-f",opts.filePath,"-C",opts.tempPath } ;
	}else{
		extractorExe = m_ctx.Engines().findExecutable( "bsdtar" ) ;

		if( extractorExe.isEmpty() ){

			extractorExe = m_ctx.Engines().findExecutable( "unzip" ) ;

			if( !extractorExe.isEmpty() ){

				extractorArgs = QStringList{ opts.filePath,"-d",opts.tempPath } ;
			}
		}else{
			extractorArgs = QStringList{ "-x","-f",opts.filePath,"-C",opts.tempPath } ;
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

		this->post( engine.name(),m + ": " + mm,opts.id ) ;

		engine.setBroken() ;
		this->printVersion( opts.move(),true ) ;
	}else{
		const auto& exe = extractorExe ;
		const auto& args = extractorArgs ;

		opts.exeArgs = { exe,args } ;

		utils::qprocess::run( exe,args,opts.move(),this,&networkAccess::extractArchiveOuput ) ;
	}
}

void networkAccess::extractArchive( networkAccess::Opts opts ) const
{
	const engines::engine& engine = opts.engine() ;

	auto mm = QObject::tr( "Extracting archive: " ) + opts.filePath ;

	this->post( engine.name(),mm,opts.id ) ;

	if( engine.archiveContainsFolder() ){

		class meaw
		{
		public:
			meaw( networkAccess::Opts opts,const networkAccess& n ) :
				m_opts( opts.move() ),
				m_parent( n )
			{
			}
			bool bg()
			{
				m_status = m_opts.engine().deleteEngineBinFolder( m_opts.tempPath ) ;

				return m_status.isEmpty() ;
			}
			void logError( int )
			{
				auto s = QObject::tr( "Trouble Ahead, Failed To Delete Folder: %1" ).arg( m_status ) ;

				m_parent.post( m_opts.engine().name(),s,m_opts.id ) ;
			}
			void fg()
			{
				m_parent.extractArchive1( m_opts.move() ) ;
			}
		private:
			QString m_status ;
			networkAccess::Opts m_opts ;
			const networkAccess& m_parent ;
		} ;

		utility::fileSystemAction( meaw( opts.move(),*this ) ) ;
	}else{
		auto m = engine.removeFiles( { opts.exeBinPath },QFileInfo( opts.exeBinPath ).absolutePath() ) ;

		if( m.size() ){

			this->failedToRemove( engine.name(),m,opts.id ) ;
		}

		this->extractArchive1( opts.move() ) ;
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

		return mm + ": " + this->timeOutErrorString() ;
	}else{
		return mm + ": " + p.errorString() ;
	}
}

networkAccess::status::~status()
{
}

QString networkAccess::File::rename( const QString& e )
{
	return utility::rename( m_path,e ) ;
}

networkAccess::report::~report()
{
}

void networkAccess::downloadSpeed::setInitialTimeStamp()
{
	m_initialTimeStamp = this->currentSecsSinceEpoch() ;
}

qint64 networkAccess::downloadSpeed::currentSecsSinceEpoch()
{
	auto now = std::chrono::system_clock::now() ;
	return static_cast< qint64 >( std::chrono::system_clock::to_time_t( now ) ) ;
}

qint64 networkAccess::downloadSpeed::elapsedTime()
{
	return this->currentSecsSinceEpoch() - m_initialTimeStamp ;
}

QString networkAccess::downloadSpeed::calculate( const utils::network::progress& p )
{
	auto received  = p.received() ;
	auto totalSize = p.total() ;

	auto e = this->elapsedTime() ;

	if( e > 0 ){

		auto m = received / e ;

		m_dataSpeed = m_locale.formattedDataSize( m ) + "/s" ;

		if( e < 60 ){

			m_dataSpeed += " in " + QString::number( e ) + "s" ;

		}else if( e < 60 * 60 ){

			auto a = QString::number( e / 60 ) ;
			auto b = QString::number( e % 60 ) ;

			m_dataSpeed += " in " + a + "m:" + b + "s" ;
		}
	}

	if( totalSize == 0 ){

		auto current = m_locale.formattedDataSize( received ) ;

		return QString( "%1 at %2" ).arg( current,m_dataSpeed ) ;
	}else{
		auto perc       = double( received ) * 100 / double( totalSize ) ;
		auto size       = m_locale.formattedDataSize( totalSize ) ;
		auto current    = m_locale.formattedDataSize( received ) ;
		auto percentage = QString::number( perc,'f',2 ) ;

		if( percentage == "100.00" || percentage == "100,00" ){

			percentage = "100" ;
		}

		return QString( "%1 / %2, %3% at %4" ).arg( current,size,percentage,m_dataSpeed ) ;
	}
}

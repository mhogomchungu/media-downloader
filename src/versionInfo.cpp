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

#include "versionInfo.h"
#include "context.hpp"
#include "tabmanager.h"
#include "mainwindow.h"

versionInfo::versionInfo( Ui::MainWindow&,const Context& ctx ) :
	m_ctx( ctx ),
	m_network( m_ctx.network() )
{
	auto& s = m_ctx.Settings() ;

	m_showLocalAndLatestVersions            = s.showLocalAndLatestVersionInformation() ;
	m_showLocalVersionsAndUpdateIfAvailable = s.showVersionInfoAndAutoDownloadUpdates() ;
	m_showLocalVersionsOnly                 = s.showLocalVersionInformationOnly() ;
}

void versionInfo::checkEnginesUpdates( const std::vector< engines::engine >& engines,bool na ) const
{
	class meaw : public versionInfo::idone
	{
	public:
		meaw( const Context& t ) : m_ctx( t )
		{
		}
		void operator()() override
		{
			m_ctx.TabManager().init_done() ;
		}
	private:
		const Context& m_ctx ;
	} ;

	this->check( { { engines,utility::sequentialID() },
		     { util::types::type_identity< meaw >(),m_ctx },na } ) ;
}

void versionInfo::log( const QString& msg,int id ) const
{
	m_ctx.logger().add( msg,id ) ;
}

void versionInfo::next( printVinfo vinfo ) const
{
	if( vinfo.hasNext() ){

		this->check( vinfo.next() ) ;
	}else{
		m_ctx.TabManager().enableAll() ;

		const auto& e = vinfo.updates() ;

		if( e.size() ){

			auto m = QObject::tr( "There Is An Update For %1" ) ;

			m_ctx.mainWindow().setTitle( m.arg( e.join( ", " ) ) ) ;
		}

		vinfo.reportDone() ;
	}
}

void versionInfo::check( versionInfo::printVinfo vinfo ) const
{
	const auto& engine = vinfo.engine() ;

	if( engine.validDownloadUrl() && networkAccess::hasNetworkSupport() ){

		if( engine.backendExists() ){

			this->printVersion( vinfo.move() ) ;
		}else{
			if( vinfo.networkAvailable() ){

				m_network.download( this->wrap( vinfo.move() ) ) ;
			}else{
				this->next( vinfo.move() ) ;
			}
		}
	}else{
		if( engine.backendExists() ){

			this->printVersion( vinfo.move() ) ;
		}else{
			auto m = QObject::tr( "Failed to find version information, make sure \"%1\" is installed and works properly" ).arg( engine.name() ) ;

			this->log( m,vinfo.iter().id() ) ;
			this->next( vinfo.move() ) ;
		}
	}
}

void versionInfo::checkMediaDownloaderUpdate( int id,
					      const QByteArray& data,
					      const std::vector< engines::engine >& engines,
					      bool hasNetworkAccess ) const
{
	QJsonParseError err ;

	auto e = QJsonDocument::fromJson( data,&err ) ;

	if( err.error == QJsonParseError::NoError ){

		auto lvs = e.object().value( "tag_name" ).toString() ;

		util::version lv = lvs  ;
		util::version iv = utility::runningVersionOfMediaDownloader() ;

		if( lv.valid() && iv < lv ){

			this->log( QObject::tr( "Newest Version Is %1, Updating" ).arg( lvs ),id ) ;

			class meaw : public networkAccess::status
			{
			public:
				meaw( const std::vector< engines::engine >& m,
				      const versionInfo& v,
				      bool hasNetworkAccess,
				      int id ) :
					m_engines( m ),
					m_parent( v ),
					m_hasNetworkAccess( hasNetworkAccess ),
					m_id( id )
				{
				}
				void done()
				{
					m_parent.checkEnginesUpdates( m_engines,m_hasNetworkAccess ) ;
				}
				int id()
				{
					return m_id ;
				}
			private:
				const std::vector< engines::engine >& m_engines ;
				const versionInfo& m_parent ;
				bool m_hasNetworkAccess ;
				int m_id ;
			} ;

			auto tt = util::types::type_identity< meaw >() ;

			networkAccess::Status s{ tt,engines,*this,hasNetworkAccess,id } ;

			m_network.updateMediaDownloader( s.move() ) ;
		}else{
			this->checkEnginesUpdates( engines,hasNetworkAccess ) ;
		}
	}else{
		m_ctx.logger().add( err.errorString(),id ) ;

		this->checkEnginesUpdates( engines,hasNetworkAccess ) ;
	}
}

void versionInfo::checkMediaDownloaderUpdate( const std::vector< engines::engine >& engines ) const
{
	if( !m_showLocalAndLatestVersions ){

		if( !m_showLocalVersionsAndUpdateIfAvailable ){

			if( !m_showLocalVersionsOnly ){

				return m_ctx.TabManager().init_done() ;
			}
		}
	}

	if( utility::platformIsNOTWindows() ){

		return this->checkEnginesUpdates( engines,true ) ;
	}

	m_ctx.TabManager().disableAll() ;

	auto id = utility::sequentialID() ;

	this->log( QObject::tr( "Checking installed version of %1" ).arg( m_ctx.appName() ),id ) ;

	this->log( QObject::tr( "Found version: %1" ).arg( utility::runningVersionOfMediaDownloader() ),id ) ;

	if( m_showLocalVersionsAndUpdateIfAvailable || m_showLocalAndLatestVersions ){

		auto url = "https://api.github.com/repos/mhogomchungu/media-downloader/releases/latest" ;

		m_network.get( url,[ this,id,&engines ]( const utils::network::reply& reply ){

			utility::networkReply nreply( m_ctx,reply ) ;

			if( reply.success() ){

				this->checkMediaDownloaderUpdate( id,nreply.data(),engines,true ) ;
			}else{
				this->checkEnginesUpdates( engines,false ) ;
			}
		} ) ;
	}else{
		this->checkEnginesUpdates( engines,true ) ;
	}
}

networkAccess::iterator versionInfo::wrap( printVinfo m ) const
{
	class meaw : public networkAccess::iter
	{
	public:
		meaw( printVinfo m ) : m_vInfo( m.move() )
		{
		}
		const engines::engine& engine() override
		{
			return m_vInfo.engine() ;
		}
		bool hasNext() override
		{
			return m_vInfo.hasNext() ;
		}
		void moveToNext() override
		{
			m_vInfo = m_vInfo.next() ;
		}
		void reportDone() override
		{
			m_vInfo.reportDone() ;
		}
		void failed() override
		{
			m_vInfo.failed() ;
		}
		const engines::Iterator& itr() override
		{
			return m_vInfo.iter() ;
		}
	private:
		printVinfo m_vInfo ;
	};

	return { util::types::type_identity< meaw >(),m.move() } ;
}

void versionInfo::printVersion( versionInfo::printVinfo vInfo ) const
{
	m_ctx.TabManager().disableAll() ;

	const auto& engine = vInfo.engine() ;

	auto id = utility::sequentialID() ;

	this->log( QObject::tr( "Checking installed version of %1" ).arg( engine.name() ),id ) ;

	if( engine.name().contains( "yt-dlp" ) && engine.name() != "yt-dlp" ){

		const auto& e = m_ctx.Engines().getEngineByName( "yt-dlp" ) ;

		if( e.has_value() ){

			const auto& version = e.value().versionInfo() ;

			if( version.valid() ){

				auto m = QObject::tr( "Found version: %1" ) ;

				this->log( m.arg( version.toString() ),id ) ;

				return this->next( vInfo.move() ) ;
			}
		}
	}

	engines::engine::exeArgs::cmd cmd( engine.exePath(),{ engine.versionArgument() } ) ;

	QString exe ;

	if( m_ctx.debug() ){

		exe = "\"" + cmd.exe() + "\"" ;

		for( const auto& it : cmd.args() ){

			exe += " \"" + it + "\"" ;
		}

		m_ctx.logger().add( "cmd: " + exe,id ) ;
	}

	auto mm = QProcess::ProcessChannelMode::MergedChannels ;

	utility::setPermissions( cmd.exe() ) ;

	versionInfo::pVInfo v{ vInfo.move(),id,exe } ;

	utils::qprocess::run( cmd.exe(),cmd.args(),mm,v.move(),this,&versionInfo::printVersionP ) ;
}

void versionInfo::printVersionP( versionInfo::pVInfo pvInfo,const utils::qprocess::outPut& r ) const
{
	const auto& engine = pvInfo.engine() ;

	if( r.success() ){

		auto m = engine.setVersionString( r.stdOut ) ;

		this->log( QObject::tr( "Found version: %1" ).arg( m ),pvInfo.id() ) ;

		const auto& url = engine.downloadUrl() ;

		const auto& vInfo = pvInfo.printVinfo() ;

		auto a = m_showLocalAndLatestVersions ;
		auto b = m_showLocalVersionsAndUpdateIfAvailable ;

		if( !url.isEmpty() && vInfo.networkAvailable() && ( a || b ) ){

			m_network.get( url,pvInfo.move(),this,&versionInfo::printVersionN ) ;
		}else{
			this->next( pvInfo.movePrintVinfo() ) ;
		}
	}else{
		auto m = QObject::tr( "Failed to find version information, make sure \"%1\" is installed and works properly" ) ;

		this->log( m.arg( engine.name() ),pvInfo.id() ) ;

		engine.setBroken() ;

		auto& debug = m_ctx.debug() ;

		if( debug ){

			auto exitCode   = "Error Code: " + QString::number( r.exitCode ) ;

			QString exitStatus ;

			auto mm = r.exitStatus ;

			if( mm == utils::qprocess::outPut::ExitStatus::NormalExit ){

				exitStatus = "Exit Status: Normal" ;

			}else if( mm == utils::qprocess::outPut::ExitStatus::Crashed ){

				exitStatus = "Exit Status: Crashed" ;
			}else{
				exitStatus = "Exit Status: Failed To Start" ;
			}
			
			QString m = "Cmd:%1\n%2\n%3\nStdOut:\n%4\n-----\nStdError:\n%5" ;

			debug( m.arg( pvInfo.cmd(),exitCode,exitStatus,r.stdOut,r.stdError ).toUtf8() ) ;
		}

		this->next( pvInfo.movePrintVinfo() ) ;
	}	
}

void versionInfo::printVersionN( versionInfo::pVInfo pvInfo,const utils::network::reply& reply ) const
{
	const auto& engine = pvInfo.engine() ;

	auto ss = utility::networkReply( m_ctx,reply ).data() ;

	const auto& versionOnline = engine.versionInfoFromGithub( ss ) ;

	const auto& installedVersion = engine.versionInfo() ;

	const auto& version = versionOnline.version ;

	if( version.valid() && installedVersion.valid() && installedVersion < version ){

		const auto& m = versionOnline.stringVersion ;

		if( m_showLocalVersionsAndUpdateIfAvailable ){

			auto mm = QObject::tr( "Newest Version Is %1, Updating" ).arg( m ) ;

			this->log( mm,pvInfo.id() ) ;

			m_network.download( this->wrap( pvInfo.movePrintVinfo() ) ) ;
		}else{
			pvInfo.updates().append( engine.name() ) ;

			m_ctx.logger().add( [ &m ]( Logger::Data& s,int id,bool ){

				auto d = s.getData( id ) ;

				auto mm = QObject::tr( "Newest Version Is %1" ).arg( m ) ;

				if( d.size() > 1 ){

					auto foundVersion = d.takeLast() ;
					auto engineName = d.takeLast() ;

					auto bar = "[media-downloader] " + utility::barLine() ;

					s.add( id,bar ) ;
					s.add( id,engineName ) ;
					s.add( id,foundVersion ) ;
					s.add( id,"[media-downloader] " + mm.toUtf8() ) ;
					s.add( id,bar ) ;
				}else{
					s.add( id,"[media-downloader] " + mm.toUtf8() ) ;
				}

			},pvInfo.id() ) ;

			this->next( pvInfo.movePrintVinfo() ) ;
		}
	}else{
		m_ctx.TabManager().enableAll() ;

		this->next( pvInfo.movePrintVinfo() ) ;
	}
}

versionInfo::idone::~idone()
{
}

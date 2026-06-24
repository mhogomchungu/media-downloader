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

versionInfo::printVinfo versionInfo::createPrintVinfo( const engines::EnginesList& engines,bool na ) const
{
	return { engines::Iterator( engines,utility::loggerID() ),this->createReportDone(),na } ;
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

	if( engine.forTesting() || engine.broken() ){

		return this->next( vinfo.move() ) ;
	}

	if( engine.validDownloadUrl() ){

		if( engine.backendExists() ){

			this->printVersion( vinfo.move() ) ;

		}else if( vinfo.justFromTheNetwork() ){

			auto id = vinfo.iter().id() ;

			auto m = QObject::tr( "Failed to find version information, make sure \"%1\" is installed and works properly" ) ;

			this->log( m.arg( engine.name() ),id ) ;

			engine.setBroken() ;

			auto exePath = vinfo.iter().engine().exePath().realExe() ;

			this->log( "exePath: " + exePath,id ) ;

			this->next( vinfo.move() ) ;

		}else if( vinfo.networkAvailable() ){

			if( engine.autoUpdate() ){

				m_network.download( vinfo.moveIter(),vinfo.moveRD() ) ;

			}else if( !engine.backendExists() ){

				m_network.download( vinfo.moveIter(),vinfo.moveRD() ) ;
			}else{
				auto m = QObject::tr( "Autoupdate Disabled For %1" ).arg( engine.name() ) ;

				this->log( m,vinfo.iter().id() ) ;

				this->next( vinfo.move() ) ;
			}
		}else{
			this->next( vinfo.move() ) ;
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

static QString _getGitVersion( const QString& e )
{
	auto m = util::split( e,"\n" ) ;

	if( m.size() > 1 ){

		const auto& s = m[ 1 ] ;

		if( s.startsWith( "Build version" ) ){

			m = util::split( s," " ) ;

			if( m.size() > 2 ){

				return m[ 2 ].replace( "\"","" ) ;
			}
		}
	}

	return {} ;
}

void versionInfo::updateMediaDownloader( int id,
					 const QJsonDocument& e,
					 const QString& lvs,
					 const engines::EnginesList& engines,
					 bool hasNetworkAccess ) const
{
	this->log( QObject::tr( "Newest Version Is %1, Updating" ).arg( lvs ),id ) ;

	class meaw : public networkAccess::status
	{
	public:
		meaw( const engines::EnginesList& m,
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
			m_parent.check( m_parent.createPrintVinfo( m_engines,m_hasNetworkAccess ) ) ;
		}
		int id()
		{
			return m_id ;
		}
	private:
		const engines::EnginesList& m_engines ;
		const versionInfo& m_parent ;
		bool m_hasNetworkAccess ;
		int m_id ;
	} ;

	auto tt = util::types::type_identity< meaw >() ;

	networkAccess::Status s{ tt,engines,*this,hasNetworkAccess,id } ;

	m_network.updateMediaDownloader( s.move(),e ) ;
}

void versionInfo::ckMDUpdate( versionInfo::printVinfo vInfo,
			      int id,
			      const QByteArray& data,
			      const engines::EnginesList& engines,
			      bool hasNetworkAccess ) const
{
	auto e = utility::jsonDoc( data ) ;

	if( e.valid() ){

		auto obj = e.toObject() ;

		auto lvs = obj.value( "tag_name" ).toString() ;

		if( lvs == "0.0.0" ){

			lvs = _getGitVersion( obj.value( "body" ).toString() ) ;
		}

		util::version lv = lvs ;
		util::version iv = utility::runningVersionOfMediaDownloader() ;

		if( lv.valid() && iv < lv ){

			if( m_showLocalVersionsAndUpdateIfAvailable ){

				this->updateMediaDownloader( id,e.get(),lvs,engines,hasNetworkAccess ) ;
			}else{
				versionInfo::pVInfo v{ vInfo.move(),id,{} } ;

				this->updateVersion( v,lvs,m_ctx.appName() ) ;

				this->check( v.movePrintVinfo() ) ;
			}
		}else{
			this->check( vInfo.move() ) ;
		}
	}else{
		m_ctx.logger().add( e.errorString(),id ) ;

		this->check( vInfo.move() ) ;
	}
}

void versionInfo::checkMediaDownloaderUpdate( const engines::EnginesList& engines ) const
{
	if( !m_showLocalAndLatestVersions ){

		if( !m_showLocalVersionsAndUpdateIfAvailable ){

			if( !m_showLocalVersionsOnly ){

				if( this->allBackendExists( engines ) ){

					return m_ctx.TabManager().init_done() ;
				}
			}
		}
	}

	if( utility::platformIsNOTWindows() ){

		return this->check( this->createPrintVinfo( engines,true ) ) ;
	}

	m_ctx.TabManager().disableAll() ;

	auto id = utility::loggerID() ;

	this->log( QObject::tr( "Checking installed version of %1" ).arg( m_ctx.appName() ),id ) ;

	this->log( QObject::tr( "Found version: %1" ).arg( utility::runningVersionOfMediaDownloader() ),id ) ;

	if( m_showLocalVersionsAndUpdateIfAvailable || m_showLocalAndLatestVersions ){

		auto url = m_ctx.Settings().gitHubDownloadUrl() ;

		class meaw
		{
		public:
			meaw( const engines::EnginesList& engines,
			      const versionInfo& parent,
			      int id ) :
				m_engines( engines ),m_parent( parent ),m_id( id )
			{
			}
			void operator()( const utils::network::reply& reply )
			{
				if( reply.success() ){

					utility::networkReply m( m_parent.m_ctx,reply ) ;

					this->checkMDUpdate( m.data() ) ;
				}else{
					auto m = m_parent.createPrintVinfo( m_engines,false ) ;

					m_parent.check( m.move() ) ;
				}
			}
		private:
			void checkMDUpdate( const QByteArray& data )
			{
				auto m = m_parent.createPrintVinfo( m_engines,true ) ;

				m_parent.ckMDUpdate( m.move(),m_id,data,m_engines,true ) ;
			}
			const engines::EnginesList& m_engines ;
			const versionInfo& m_parent ;
			int m_id ;
		} ;

		m_network.get( url,meaw( engines,*this,id ) ) ;
	}else{
		this->check( this->createPrintVinfo( engines,true ) ) ;
	}
}

bool versionInfo::likeYtdlpExtra( const engines::engine& engine ) const
{
	const auto& name = engine.name() ;

	return name.contains( "yt-dlp" ) && name != "yt-dlp" && name != "yt-dlp-nightly" ;
}

networkAccess::reportDone versionInfo::createReportDone() const
{
	class meaw : public networkAccess::report
	{
	public:
		meaw( const Context& t ) : m_ctx( t )
		{
		}
		void done() override
		{
			m_ctx.TabManager().init_done() ;
		}
	private:
		const Context& m_ctx ;
	} ;

	return networkAccess::reportDone( util::types::type_identity< meaw >(),m_ctx ) ;
}

bool versionInfo::allBackendExists( const engines::EnginesList& e ) const
{
	for( const auto& it : e ){

		if( !it.backendExists() ){

			return false ;
		}
	}

	return true ;
}

void versionInfo::printVersion( versionInfo::printVinfo vInfo ) const
{
	const auto& engine = vInfo.engine() ;

	if( !utility::cliArguments::debug() ){

		if( engine.supportingEngine() ){

			if( !engine.updatableSupportingEngine() ){

				return this->next( vInfo.move() ) ;
			}
		}
	}

	m_ctx.TabManager().disableAll() ;	

	auto id = utility::loggerID() ;

	this->log( QObject::tr( "Checking installed version of %1" ).arg( engine.name() ),id ) ;

	if( this->likeYtdlpExtra( engine ) ){

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

	const auto& versionArgs = engine.versionArgument() ;

	if( versionArgs.isEmpty() ){

		auto m = QObject::tr( "Skipping Checking Version Info For Engine \"%1\"" ) ;

		auto bar = utility::barLine() ;

		this->log( bar,id ) ;
		this->log( m.arg( engine.name() ),id ) ;
		this->log( bar,id ) ;

		return this->next( vInfo.move() ) ;
	}

	engines::engine::exeArgs::cmd cmd( engine.exePath(),{ versionArgs } ) ;

	QString exe = "\"" + cmd.exe() + "\"" ;

	for( const auto& it : cmd.args() ){

		exe += " \"" + it + "\"" ;
	}

	if( utility::cliArguments::debug() ){

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

	auto versionString = engine.parseVersionInfo( r ) ;

	if( !versionString.isEmpty() ){

		auto m = engine.setVersionString( versionString ) ;

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
		auto bar = utility::barLine() ;

		auto id = pvInfo.id() ;

		this->log( bar,id ) ;

		auto m = QObject::tr( "Failed to find version information, make sure \"%1\" is installed and works properly" ) ;

		this->log( m.arg( engine.name() ),pvInfo.id() ) ;

		engine.setBroken() ;

		this->log( "Cmd: " + pvInfo.cmd(),id ) ;

		this->log( "Exit Code: " + QString::number( r.exitCode ),id ) ;

		auto mm = r.exitStatus ;

		if( mm == utils::qprocess::outPut::ExitStatus::NormalExit ){

			this->log( "Exit Status: Normal Exit",id ) ;

		}else if( mm == utils::qprocess::outPut::ExitStatus::Crashed ){

			this->log( "Exit Status: Crashed",id ) ;
		}else{
			this->log( "Exit Status: Failed To Start",id ) ;
		}

		if( !r.stdOut.isEmpty() ){

			this->log( "Std Out: " + r.stdOut,id ) ;
		}

		if( !r.stdError.isEmpty() ){

			this->log( "Std Error: " + r.stdError,id ) ;
		}

		this->log( bar,id ) ;

		this->next( pvInfo.movePrintVinfo() ) ;
	}	
}

void versionInfo::printVersionN( versionInfo::pVInfo pvInfo,const utils::network::reply& reply ) const
{
	const auto& engine = pvInfo.engine() ;

	auto ss = utility::networkReply( m_ctx,reply ).data() ;

	pvInfo.setNetworkAvailability( !ss.isEmpty() ) ;

	const auto& versionOnline = engine.versionInfoFromGithub( ss ) ;

	const auto& installedVersion = engine.versionInfo() ;

	const auto& version = versionOnline.version ;

	if( version.valid() && installedVersion.valid() && installedVersion < version ){

		const auto& m = versionOnline.stringVersion ;

		if( engine.autoUpdate( versionOnline ) ){

			auto mm = QObject::tr( "Newest Version Is %1, Updating" ).arg( m ) ;

			this->log( mm,pvInfo.id() ) ;

			auto s = pvInfo.movePrintVinfo() ;

			m_network.download( s.moveIter(),s.moveRD() ) ;

		}else if( m_showLocalVersionsAndUpdateIfAvailable ){

			if( engine.autoUpdate() ){

				auto mm = QObject::tr( "Newest Version Is %1, Updating" ).arg( m ) ;

				this->log( mm,pvInfo.id() ) ;

				auto s = pvInfo.movePrintVinfo() ;

				m_network.download( s.moveIter(),s.moveRD() ) ;
			}else{
				auto mm = QObject::tr( "Newest Version Is %1, AutoUpdate Disabled" ).arg( m ) ;

				this->log( mm,pvInfo.id() ) ;

				this->next( pvInfo.movePrintVinfo() ) ;
			}
		}else{
			this->updateVersion( pvInfo,m,engine.name() ) ;

			this->next( pvInfo.movePrintVinfo() ) ;
		}
	}else{
		m_ctx.TabManager().enableAll() ;

		this->next( pvInfo.movePrintVinfo() ) ;
	}
}

void versionInfo::updateVersion( versionInfo::pVInfo& pvInfo,
				 const QString& version,
				 const QString& engineName ) const
{
	pvInfo.updates().append( engineName ) ;

	class meaw
	{
	public:
		meaw( const QString& version ) : m_version( version )
		{
		}
		void operator()( Logger::Data& s,int id,bool ) const
		{
			auto d = s.getData( id ) ;

			auto mm = QObject::tr( "Newest Version Is: %1" ).arg( m_version ) ;

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
		}
	private:
		const QString& m_version ;
	} ;

	m_ctx.logger().add( meaw( version ),pvInfo.id() ) ;
}

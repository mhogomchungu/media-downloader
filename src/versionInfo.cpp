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

versionInfo::printVinfo versionInfo::createPrintVinfo( const std::vector< engines::engine >& engines,bool na ) const
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

	engines::Iterator iter( engines,utility::sequentialID() ) ;
	versionInfo::reportDone rd( util::types::type_identity< meaw >(),m_ctx ) ;

	return { iter.move(),rd.move(),na } ;
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
	bool fromNetwork = vinfo.fromNetwork() ;

	vinfo.resetFromNetwork() ;

	const auto& engine = vinfo.engine() ;

	if( engine.forTesting() ){

		return this->next( vinfo.move() ) ;
	}

	if( engine.validDownloadUrl() && networkAccess::hasNetworkSupport() ){

		if( engine.backendExists() ){

			this->printVersion( vinfo.move() ) ;

		}else if( fromNetwork ){

			auto id = vinfo.iter().id() ;

			auto m = QObject::tr( "Failed to find version information, make sure \"%1\" is installed and works properly" ) ;

			this->log( m.arg( engine.name() ),id ) ;

			engine.setBroken() ;

			auto exePath = vinfo.iter().engine().exePath().realExe() ;

			this->log( "exePath: " + exePath,id ) ;

			this->next( vinfo.move() ) ;

		}else if( vinfo.networkAvailable() ){

			if( engine.autoUpdate() ){

				m_network.download( this->wrap( vinfo.move() ) ) ;
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
					 const std::vector< engines::engine >& engines,
					 bool hasNetworkAccess ) const
{
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
			m_parent.check( m_parent.createPrintVinfo( m_engines,m_hasNetworkAccess ) ) ;
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

	m_network.updateMediaDownloader( s.move(),e ) ;
}

void versionInfo::checkMediaDownloaderUpdate( versionInfo::printVinfo vInfo,
					      int id,
					      const QByteArray& data,
					      const std::vector< engines::engine >& engines,
					      bool hasNetworkAccess ) const
{
	QJsonParseError err ;

	auto e = QJsonDocument::fromJson( data,&err ) ;

	if( err.error == QJsonParseError::NoError ){

		auto obj = e.object() ;

		auto lvs = obj.value( "tag_name" ).toString() ;

		if( lvs == "0.0.0" ){

			lvs = _getGitVersion( obj.value( "body" ).toString() ) ;
		}

		util::version lv = lvs ;
		util::version iv = utility::runningVersionOfMediaDownloader() ;

		if( lv.valid() && iv < lv ){

			if( m_showLocalVersionsAndUpdateIfAvailable ){

				this->updateMediaDownloader( id,e,lvs,engines,hasNetworkAccess ) ;
			}else{
				versionInfo::pVInfo v{ vInfo.move(),id,{} } ;

				this->updateVersion( v,lvs,m_ctx.appName() ) ;

				this->check( v.movePrintVinfo() ) ;
			}
		}else{
			this->check( vInfo.move() ) ;
		}
	}else{
		m_ctx.logger().add( err.errorString(),id ) ;

		this->check( vInfo.move() ) ;
	}
}

void versionInfo::checkMediaDownloaderUpdate( const std::vector< engines::engine >& engines ) const
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

	auto id = utility::sequentialID() ;

	this->log( QObject::tr( "Checking installed version of %1" ).arg( m_ctx.appName() ),id ) ;

	this->log( QObject::tr( "Found version: %1" ).arg( utility::runningVersionOfMediaDownloader() ),id ) ;

	if( m_showLocalVersionsAndUpdateIfAvailable || m_showLocalAndLatestVersions ){

		auto url = m_ctx.Settings().gitHubDownloadUrl() ;

		m_network.get( url,[ this,id,&engines ]( const utils::network::reply& reply ){

			utility::networkReply nreply( m_ctx,reply ) ;

			if( reply.success() ){

				auto m = this->createPrintVinfo( engines,true ) ;
				this->checkMediaDownloaderUpdate( m.move(),id,nreply.data(),engines,true ) ;
			}else{
				this->check( this->createPrintVinfo( engines,false ) ) ;
			}
		} ) ;
	}else{
		this->check( this->createPrintVinfo( engines,true ) ) ;
	}
}

bool versionInfo::allBackendExists( const std::vector<engines::engine>& e ) const
{
	for( const auto& it : e ){

		if( !it.backendExists() ){

			return false ;
		}
	}

	return true ;
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

	if( m_ctx.debug() ){

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

	const auto& versionOnline = engine.versionInfoFromGithub( ss ) ;

	const auto& installedVersion = engine.versionInfo() ;

	const auto& version = versionOnline.version ;

	if( version.valid() && installedVersion.valid() && installedVersion < version ){

		const auto& m = versionOnline.stringVersion ;

		if( m_showLocalVersionsAndUpdateIfAvailable ){

			if( engine.autoUpdate() ){

				auto mm = QObject::tr( "Newest Version Is %1, Updating" ).arg( m ) ;

				this->log( mm,pvInfo.id() ) ;

				m_network.download( this->wrap( pvInfo.movePrintVinfo() ) ) ;
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

	m_ctx.logger().add( [ &version ]( Logger::Data& s,int id,bool ){

		auto d = s.getData( id ) ;

		auto mm = QObject::tr( "Newest Version Is: %1" ).arg( version ) ;

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
}

versionInfo::idone::~idone()
{
}

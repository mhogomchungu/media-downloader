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

void versionInfo::updateMediaDownloader( const engines::Iterator& iter ) const
{
	m_ctx.network().download( iter,{} ) ;
}

void versionInfo::check( const engines::Iterator& iter,
			 versionInfo::reportDone rd,
			 const QString& setDefaultEngine ) const
{
	if( iter.engine().name() =="media-downloader" ){

		return this->printEngineVersionInfo( iter,std::move( rd ) ) ;
	}

	const auto& engine = iter.engine() ;

	if( engine.usingPrivateBackend() && engine.validDownloadUrl() && networkAccess::hasNetworkSupport() ){

		if( engine.backendExists() ){

			this->printEngineVersionInfo( iter,std::move( rd ) ) ;

			utility::setDefaultEngine( m_ctx,setDefaultEngine ) ;

		}else if( !engine.exePath().realExe().isEmpty() ){

			m_ctx.network().download( iter,setDefaultEngine ) ;
		}
	}else{
		if( engine.exePath().isEmpty() ){

			m_ctx.logger().add( QObject::tr( "Failed to find version information, make sure \"%1\" is installed and works properly" ).arg( engine.name() ),iter.id() ) ;
		}else{
			this->printEngineVersionInfo( iter,std::move( rd ) ) ;

			utility::setDefaultEngine( m_ctx,setDefaultEngine ) ;
		}
	}
}

void versionInfo::printEngineVersionInfo( const engines::Iterator& iter,versionInfo::reportDone rd ) const
{
	const auto& engine = iter.engine() ;

	m_ctx.TabManager().disableAll() ;

	engines::engine::exeArgs::cmd cmd( engine.exePath(),{ engine.versionArgument() } ) ;

	auto id = utility::sequentialID() ;

	m_ctx.logger().add( QObject::tr( "Checking installed version of" ) + " " + engine.name(),id ) ;

	if( !m_ctx.debug().isEmpty() ){

		auto exe = "cmd: \"" + cmd.exe() + "\"" ;

		for( const auto& it : cmd.args() ){

			exe += " \"" + it + "\"" ;
		}

		m_ctx.logger().add( exe,id ) ;
	}

	utils::qprocess::run( cmd.exe(),cmd.args(),[ iter,this,id,rd = std::move( rd ) ]( const utils::qprocess::outPut& r ){

		const auto& engine = iter.engine() ;

		if( r.success() ){

			auto& logger = m_ctx.logger() ;

			logger.add( QObject::tr( "Found version" ) + ": " + engine.setVersionString( r.stdOut ),id ) ;

			m_ctx.TabManager().enableAll() ;
		}else{
			m_ctx.logger().add( QObject::tr( "Failed to find version information, make sure \"%1\" is installed and works properly" ).arg( engine.name() ),id ) ;

			m_ctx.TabManager().enableAll() ;

			engine.setBroken() ;
		}

		if( iter.hasNext() ){

			this->check( iter.next(),rd ) ;
		}else{
			rd() ;
		}

	},QProcess::ProcessChannelMode::MergedChannels ) ;
}

versionInfo::doneInterface::~doneInterface()
{
}

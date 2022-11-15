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

versionInfo::versionInfo( Ui::MainWindow& ui,const Context& ctx ) :
	m_ui( ui ),
	m_ctx( ctx ),
	m_checkForEnginesUpdates( m_ctx.Settings().checkForEnginesUpdates() )
{
}

void versionInfo::log( const QString& msg,int id ) const
{
	m_ctx.logger().add( msg,id ) ;
}

void versionInfo::updateMediaDownloader( const engines::Iterator& iter ) const
{
	m_ctx.network().download( iter,{} ) ;
}

void versionInfo::check( versionInfo::printVinfo vinfo ) const
{
	if( vinfo.engine().name() =="media-downloader" ){

		return this->printEngineVersionInfo( std::move( vinfo ) ) ;
	}

	const auto& engine = vinfo.engine() ;

	auto m = vinfo.setAfterDownloading( false ) ;

	if( engine.usingPrivateBackend() && engine.validDownloadUrl() && networkAccess::hasNetworkSupport() ){

		if( engine.backendExists() ){

			if( m || vinfo.show() ){

				utility::setDefaultEngine( m_ctx,vinfo.defaultEngine() ) ;

				this->printEngineVersionInfo( std::move( vinfo ) ) ;
			}else{
				if( vinfo.hasNext() ){

					this->check( vinfo.next() ) ;
				}else{
					vinfo.reportDone() ;
				}
			}
		}else{
			m_ctx.network().download( vinfo.iter(),vinfo.showVersionInfo(),vinfo.defaultEngine() ) ;
		}
	}else{
		if( engine.backendExists() ){

			if( vinfo.show() || m ){

				utility::setDefaultEngine( m_ctx,vinfo.defaultEngine() ) ;

				this->printEngineVersionInfo( std::move( vinfo ) ) ;
			}else{
				if( vinfo.hasNext() ){

					this->check( vinfo.next() ) ;
				}else{
					vinfo.reportDone() ;
				}
			}
		}else{
			if( vinfo.show() ){

				auto m = QObject::tr( "Failed to find version information, make sure \"%1\" is installed and works properly" ).arg( engine.name() ) ;

				this->log( m,vinfo.iter().id() ) ;
			}else{
				if( vinfo.hasNext() ){

					this->check( vinfo.next() ) ;
				}else{
					vinfo.reportDone() ;
				}
			}
		}
	}
}

void versionInfo::printEngineVersionInfo( versionInfo::printVinfo vInfo ) const
{
	m_ctx.TabManager().disableAll() ;

	const auto& engine = vInfo.engine() ;

	auto id = utility::sequentialID() ;

	this->log( QObject::tr( "Checking installed version of" ) + " " + engine.name(),id ) ;

	if( engine.name().contains( "yt-dlp" ) && engine.name() != "yt-dlp" ){

		const auto& e = m_ctx.Engines().getEngineByName( "yt-dlp" ) ;

		if( e.has_value() ){

			const auto& version = e.value().versionInfo() ;

			if( version.valid() ){

				this->log( QObject::tr( "Found version" ) + ": " + version.toString(),id ) ;

				if( vInfo.hasNext() ){

					this->check( vInfo.next() ) ;
				}else{
					vInfo.reportDone() ;
				}

				return ;
			}
		}
	}

	engines::engine::exeArgs::cmd cmd( engine.exePath(),{ engine.versionArgument() } ) ;

	if( !m_ctx.debug().isEmpty() ){

		auto exe = "cmd: \"" + cmd.exe() + "\"" ;

		for( const auto& it : cmd.args() ){

			exe += " \"" + it + "\"" ;
		}

		m_ctx.logger().add( exe,id ) ;
	}

	auto mm = QProcess::ProcessChannelMode::MergedChannels ;

	QFile qfile( cmd.exe() ) ;

	qfile.setPermissions( qfile.permissions() | QFileDevice::ExeOwner ) ;

	utils::qprocess::run( cmd.exe(),cmd.args(),mm,[ this,id,vInfo = std::move( vInfo ) ]( const utils::qprocess::outPut& r ){

		const auto& engine = vInfo.engine() ;

		if( r.success() ){

			this->log( QObject::tr( "Found version" ) + ": " + engine.setVersionString( r.stdOut ),id ) ;

			const auto& url = engine.downloadUrl() ;

			if( !url.isEmpty() && m_checkForEnginesUpdates ){

				m_ctx.network().get( url,[ id,this,vInfo = vInfo.move() ]( const QByteArray& m ){

					const auto& engine = vInfo.engine() ;

					const auto& versionOnline = engine.versionInfoFromGithub( m ) ;
					const auto& installedVersion = engine.versionInfo() ;

					const auto& version = versionOnline.version ;

					if( version.valid() && installedVersion.valid() && installedVersion < version ){

						auto m = versionOnline.stringVersion ;

						this->log( QObject::tr( "Newest Version Is %1, Updating" ).arg( m ) ,id ) ;

						m_ctx.network().download( vInfo.iter(),vInfo.showVersionInfo(),vInfo.defaultEngine() ) ;
					}else{
						m_ctx.TabManager().enableAll() ;

						if( vInfo.hasNext() ){

							this->check( vInfo.next() ) ;
						}else{
							vInfo.reportDone() ;
						}
					}
				} ) ;

				return ;
			}else{
				m_ctx.TabManager().enableAll() ;
			}
		}else{
			this->log( QObject::tr( "Failed to find version information, make sure \"%1\" is installed and works properly" ).arg( engine.name() ),id ) ;

			m_ctx.TabManager().enableAll() ;

			engine.setBroken() ;
		}

		if( vInfo.hasNext() ){

			this->check( vInfo.next() ) ;
		}else{
			vInfo.reportDone() ;
		}
	} ) ;
}

versionInfo::doneInterface::~doneInterface()
{
}

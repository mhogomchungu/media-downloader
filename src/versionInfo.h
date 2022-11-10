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

#ifndef VERSIONINFO_H
#define VERSIONINFO_H

#include <QObject>

#include "utility.h"
#include "engines.h"

class Context ;

namespace Ui
{
	class MainWindow ;
}

class versionInfo : public QObject
{
	Q_OBJECT
public:
	struct doneInterface
	{
		virtual void operator()()
		{
		}
		virtual ~doneInterface() ;
	} ;

	class reportDone
	{
	public:
		template< typename Type,typename ... Args >
		reportDone( Type,Args&& ... args ) :
			m_handle( std::make_shared< typename Type::type >( std::forward< Args >( args ) ... ) )
		{
		}
		reportDone() : m_handle( std::make_shared< doneInterface >() )
		{
		}
		void operator()() const
		{
			( *m_handle )() ;
		}
	private:
		/*
		 * We are using make_shared because old versions of gcc do not work with
		 * unique_ptr when moving the class to lambda capture area
		 */
		std::shared_ptr< doneInterface > m_handle ;
	} ;
	~versionInfo() override
	{
	}
	versionInfo( Ui::MainWindow& ui,const Context& ctx ) : m_ui( ui ),m_ctx( ctx )
	{
	}
	template< typename Then >
	void setVersion( const engines::engine& engine,Then then ) const
	{
		engines::engine::exeArgs::cmd cmd( engine.exePath(),{ engine.versionArgument() } ) ;

		utils::qprocess::run( cmd.exe(),cmd.args(),[ &engine,then = std::move( then ) ]( const utils::qprocess::outPut& r ){

			if( r.success() ){

				engine.setVersionString( r.stdOut ) ;
			}

			then( r.success() ) ;

		},QProcess::ProcessChannelMode::MergedChannels ) ;
	}
	void updateMediaDownloader( const engines::Iterator& iter ) const ;
	void check( const engines::Iterator& iter,const QString& setDefaultEngine ) const
	{
		this->check( iter,{ false,false },versionInfo::reportDone(),setDefaultEngine ) ;
	}
	void check( const engines::Iterator& iter,
		    networkAccess::showVersionInfo showVersionInfo,
		    versionInfo::reportDone = versionInfo::reportDone(),
		    const QString& setDefaultEngine = QString() ) const ;
private:
	void printEngineVersionInfo( const engines::Iterator& iter,
				     networkAccess::showVersionInfo,
				     versionInfo::reportDone ) const ;
	Ui::MainWindow& m_ui ;
	const Context& m_ctx ;
};

#endif

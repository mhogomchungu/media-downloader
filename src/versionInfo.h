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
		virtual bool booting()
		{
			return false ;
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
		bool booting() const
		{
			return m_handle->booting() ;
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
	versionInfo( Ui::MainWindow& ui,const Context& ctx ) ;
	template< typename Then >
	void setVersion( const engines::engine& engine,Then then ) const
	{
		engines::engine::exeArgs::cmd cmd( engine.exePath(),{ engine.versionArgument() } ) ;

		auto mm = QProcess::ProcessChannelMode::MergedChannels ;

		utils::qprocess::run( cmd.exe(),cmd.args(),mm,[ &engine,then = std::move( then ) ]( const utils::qprocess::outPut& r ){

			if( r.success() ){

				engine.setVersionString( r.stdOut ) ;
			}

			then( r.success() ) ;
		} ) ;
	}
	void log( const QString& msg,int id ) const ;
	void updateMediaDownloader( const engines::Iterator& iter ) const ;
	void check( const engines::Iterator& iter,const QString& setDefaultEngine ) const
	{
		this->check( { iter,{ false,false },versionInfo::reportDone(),setDefaultEngine } ) ;
	}
	void check( const engines::Iterator& iter,
		    networkAccess::showVersionInfo showVersionInfo,
		    versionInfo::reportDone rd = versionInfo::reportDone(),
		    const QString& defaultEngine = QString() ) const
	{
		this->check( { iter,showVersionInfo,std::move( rd ),defaultEngine } ) ;
	}
	class printVinfo
	{
	public:
		printVinfo( const engines::Iterator& iter,
			    networkAccess::showVersionInfo showVinfo,
			    versionInfo::reportDone rd,
			    QString setDefaultEngine ) :
			m_iter( iter ),
			m_showVinfo( showVinfo ),
			m_rd( std::move( rd ) ),
			m_setDefaultEngine( std::move( setDefaultEngine ) )
		{
		}
		const engines::Iterator& iter() const
		{
			return m_iter ;
		}
		const engines::engine& engine() const
		{
			return m_iter.engine() ;
		}
		bool hasNext() const
		{
			return m_iter.hasNext() ;
		}
		printVinfo next() const
		{
			auto m = std::move( *const_cast< printVinfo * >( this ) ) ;

			m.m_iter = m.m_iter.next() ;

			return m ;
		}
		printVinfo move() const
		{
			return std::move( *const_cast< printVinfo * >( this ) ) ;
		}
		void reportDone() const
		{
			m_rd() ;
		}
		bool show() const
		{
			return this->showVersionInfo().show ;
		}
		const networkAccess::showVersionInfo& showVersionInfo() const
		{
			return m_showVinfo ;
		}
		const QString& defaultEngine() const
		{
			return m_setDefaultEngine ;
		}
		bool booting() const
		{
			return m_rd.booting() ;
		}
		bool setAfterDownloading( bool e )
		{
			auto m = m_showVinfo.setAfterDownloading ;

			m_showVinfo.setAfterDownloading = e ;

			return m ;
		}
	private:
		engines::Iterator m_iter ;
		networkAccess::showVersionInfo m_showVinfo ;
		versionInfo::reportDone m_rd ;
		QString m_setDefaultEngine ;
	} ;

	class extensionVersionInfo
	{
	public:
		extensionVersionInfo( engines::Iterator iter ) : m_iter( std::move( iter ) )
		{
		}
		void append( const QString& engineName,util::version iv,util::version lv )
		{
			if( iv.valid() && lv.valid() ){

				m_enginesInfo.emplace_back( engineName,std::move( iv ),std::move( lv ) ) ;
			}
		}
		const engines::engine& engine() const
		{
			return m_iter.engine() ;
		}
		bool hasNext() const
		{
			return m_iter.hasNext() ;
		}
		extensionVersionInfo next() const
		{
			auto m = this->move() ;

			m.m_iter = m_iter.next() ;

			return m ;
		}
		extensionVersionInfo move() const
		{
			return std::move( *const_cast< versionInfo::extensionVersionInfo * >( this ) ) ;
		}
		template< typename Function >
		void report( Function function ) const
		{
			for( const auto& m : m_enginesInfo ){

				if( m.installedVersion < m.latestVersion ){

					const auto& a = m.engineName ;
					const auto& b = m.installedVersion.toString() ;
					const auto& c = m.latestVersion.toString() ;

					function( a,b,c ) ;
				}
			}
		}
	private:
		engines::Iterator m_iter ;
		struct engineInfo
		{
			engineInfo( const QString& e,util::version i,util::version l ) :
				engineName( e ),installedVersion( std::move( i ) ),latestVersion( std::move( l ) )
			{
			}
			QString engineName ;
			util::version installedVersion ;
			util::version latestVersion ;
		} ;
		std::vector< engineInfo > m_enginesInfo ;
	} ;

	void checkForEnginesUpdates( versionInfo::extensionVersionInfo ) const ;
	void done( versionInfo::extensionVersionInfo ) const ;
	void done( versionInfo::printVinfo ) const ;
	void check( versionInfo::printVinfo ) const ;
	void checkForUpdates() const ;
private:
	void printEngineVersionInfo( versionInfo::printVinfo ) const ;
	Ui::MainWindow& m_ui ;
	const Context& m_ctx ;
	bool m_checkForEnginesUpdates ;
};

#endif

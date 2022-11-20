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

class versionInfo
{
public:
	struct idone
	{
		virtual void operator()()
		{
		}
		virtual bool booting()
		{
			return false ;
		}
		virtual ~idone() ;
	} ;

	class reportDone
	{
	public:
		template< typename Type,typename ... Args >
		reportDone( Type,Args&& ... args ) :
			m_handle( std::make_unique< typename Type::type >( std::forward< Args >( args ) ... ) )
		{
		}
		reportDone() : m_handle( std::make_unique< idone >() )
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
		std::unique_ptr< idone > m_handle ;
	} ;
	~versionInfo()
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
	class printVinfo
	{
	public:
		printVinfo( engines::Iterator iter,
			    networkAccess::showVersionInfo showVinfo,
			    versionInfo::reportDone rd ) :
			m_iter( std::move( iter ) ),
			m_showVinfo( showVinfo ),
			m_rd( std::move( rd ) ),
			m_fromNetwork( false )
		{
		}
		printVinfo( networkAccess::iterator iter,
			    networkAccess::showVersionInfo showVinfo ) :
			m_networkIter( std::move( iter ) ),
			m_iter( m_networkIter.itr() ),
			m_showVinfo( showVinfo ),
			m_fromNetwork( true )
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
			if( m_fromNetwork ){

				m_networkIter.reportDone() ;
			}else{
				m_rd() ;
			}
		}
		bool show() const
		{
			return this->showVersionInfo().show ;
		}
		const networkAccess::showVersionInfo& showVersionInfo() const
		{
			return m_showVinfo ;
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
		networkAccess::iterator m_networkIter ;
		engines::Iterator m_iter ;
		networkAccess::showVersionInfo m_showVinfo ;
		versionInfo::reportDone m_rd ;
		bool m_fromNetwork ;
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

	void log( const QString& msg,int id ) const ;
	void checkForEnginesUpdates( versionInfo::extensionVersionInfo ) const ;
	void done( versionInfo::extensionVersionInfo ) const ;
	void done( versionInfo::printVinfo ) const ;
	void check( versionInfo::printVinfo ) const ;
	void check( networkAccess::iterator iter,networkAccess::showVersionInfo v ) const
	{
		this->check( { std::move( iter ),std::move( v ) } ) ;
	}
	void check( const engines::Iterator& iter,versionInfo::reportDone rd ) const
	{
		this->check( { iter,{ false,false },std::move( rd ) } ) ;
	}
	void checkForUpdates() const ;
private:
	networkAccess::iterator wrap( versionInfo::printVinfo ) const ;

	void printEngineVersionInfo( versionInfo::printVinfo ) const ;
	const Context& m_ctx ;
	bool m_checkForEnginesUpdates ;
};

#endif

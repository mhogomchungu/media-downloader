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

#include "utils/miscellaneous.hpp"

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
		virtual void failed()
		{
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
		void failed() const
		{
			m_handle->failed() ;
		}
		reportDone move()
		{
			return std::move( *this ) ;
		}
	private:
		utils::misc::unique_ptr< idone > m_handle ;
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

		class meaw
		{
		public:
			meaw( const engines::engine& engine,Then&& then ) :
				m_engine( engine ),m_then( std::move( then ) )
			{
			}
		private:
			void operator()( const utils::qprocess::outPut& r )
			{
				if( r.success() ){

					m_engine.setVersionString( r.stdOut ) ;
				}

				m_then( r.success() ) ;
			}
			const engines::engine& m_engine ;
			Then m_then ;
		} ;

		utils::qprocess::run( cmd.exe(),cmd.args(),mm,meaw( engine,std::move( then ) ) ) ;
	}
	class printVinfo
	{
	public:
		printVinfo( engines::Iterator iter,versionInfo::reportDone rd,bool networkAvailable ) :
			m_iter( std::move( iter ) ),
			m_rd( std::move( rd ) ),
			m_fromNetwork( false ),
			m_networkAvailable( networkAvailable )
		{
		}
		printVinfo( networkAccess::iterator iter,bool networkAvailable ) :
			m_networkIter( std::move( iter ) ),
			m_iter( m_networkIter.itr() ),
			m_fromNetwork( true ),
			m_networkAvailable( networkAvailable )
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
		bool networkAvailable() const
		{
			return m_networkAvailable ;
		}
		bool hasNext() const
		{
			return m_iter.hasNext() ;
		}
		bool fromNetwork() const
		{
			return m_fromNetwork ;
		}
		void resetFromNetwork()
		{
			m_fromNetwork = false ;
		}
		printVinfo next()
		{
			auto m = std::move( *this ) ;

			m.m_iter = m.m_iter.next() ;

			return m ;
		}
		printVinfo move()
		{
			return std::move( *this ) ;
		}
		void reportDone() const
		{
			if( m_fromNetwork ){

				m_networkIter.reportDone() ;
			}else{
				m_rd() ;
			}
		}
		void failed() const
		{
			if( m_fromNetwork ){

				m_networkIter.failed() ;
			}else{
				m_rd.failed() ;
			}
		}
		QStringList& updates()
		{
			return m_updates ;
		}
	private:
		networkAccess::iterator m_networkIter ;
		engines::Iterator m_iter ;
		versionInfo::reportDone m_rd ;
		bool m_fromNetwork ;
		bool m_networkAvailable = true ;		
		QStringList m_updates ;
	} ;

	void log( const QString& msg,int id ) const ;
	void next( versionInfo::printVinfo ) const ;
	void check( versionInfo::printVinfo ) const ;
	void check( networkAccess::iterator iter,bool hn ) const
	{
		this->check( { std::move( iter ),hn } ) ;
	}
	void check( const engines::Iterator& iter,versionInfo::reportDone rd,bool hn ) const
	{
		this->check( { iter,std::move( rd ),hn } ) ;
	}
	void checkMediaDownloaderUpdate( const std::vector< engines::engine >& ) const ;
private:
	bool allBackendExists( const std::vector< engines::engine >& ) const ;
	versionInfo::printVinfo createPrintVinfo( const std::vector< engines::engine >&,bool ) const ;
	void checkMediaDownloaderUpdate( versionInfo::printVinfo,
					 int,
					 const QByteArray&,
					 const std::vector< engines::engine >&,
					 bool ) const ;

	networkAccess::iterator wrap( versionInfo::printVinfo ) const ;

	class pVInfo
	{
	public:
		pVInfo( versionInfo::printVinfo v,int id,const QString& cmd ) :
			m_pvInfo( v.move() ),m_id( id ),m_cmd( cmd )
		{
		}
		const engines::engine& engine()
		{
			return m_pvInfo.engine() ;
		}
		int id()
		{
			return m_id ;
		}
		pVInfo move()
		{
			return std::move( *this ) ;
		}
		versionInfo::printVinfo movePrintVinfo()
		{
			return std::move( m_pvInfo ) ;
		}
		const versionInfo::printVinfo& printVinfo()
		{
			return m_pvInfo ;
		}
		const QString& cmd()
		{
			return m_cmd ;
		}
		QStringList& updates()
		{
			return m_pvInfo.updates() ;
		}
	private:
		versionInfo::printVinfo m_pvInfo ;
		int m_id ;
		QString m_cmd ;
	};
	void printVersion( versionInfo::printVinfo ) const ;
	void printVersionP( versionInfo::pVInfo,const utils::qprocess::outPut& ) const ;
	void printVersionN( versionInfo::pVInfo,const utils::network::reply& ) const ;

	void updateVersion( versionInfo::pVInfo&,const QString&,const QString& ) const ;
	void updateMediaDownloader( int,
				    const QJsonDocument&,
				    const QString&,
				    const std::vector< engines::engine >&,
				    bool ) const ;

	const Context& m_ctx ;
	const networkAccess& m_network ;

	bool m_showLocalAndLatestVersions ;
	bool m_showLocalVersionsAndUpdateIfAvailable ;
	bool m_showLocalVersionsOnly ;
};

#endif

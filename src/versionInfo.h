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
		printVinfo( engines::Iterator iter,networkAccess::reportDone rd,bool networkAvailable ) :
			m_iter( iter.move() ),
			m_rd( rd.move() ),
			m_networkAvailable( networkAvailable ),
			m_justFromTheNetwork( false )
		{
		}
		printVinfo( networkAccess::iterator iter,bool networkAvailable ) :
			m_iter( iter.getItor() ),
			m_rd( iter.getRD() ),
			m_networkAvailable( networkAvailable ),
			m_justFromTheNetwork( true )
		{
		}
		engines::Iterator moveIter()
		{
			return m_iter.move() ;
		}
		networkAccess::reportDone moveRD()
		{
			return m_rd.move() ;
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
		printVinfo next()
		{
			auto m = this->move() ;

			m.m_justFromTheNetwork = false ;

			m.m_iter.setNext() ;

			return m ;
		}
		printVinfo move()
		{
			return std::move( *this ) ;
		}
		void reportDone() const
		{
			m_rd.done() ;
		}
		void failed() const
		{
			m_rd.failed() ;
		}
		QStringList& updates()
		{
			return m_updates ;
		}
		bool justFromTheNetwork()
		{
			return m_justFromTheNetwork ;
		}
	private:
		engines::Iterator m_iter ;
		networkAccess::reportDone m_rd ;
		bool m_networkAvailable = true ;
		bool m_justFromTheNetwork = false ;
		QStringList m_updates ;
	} ;

	void log( const QString& msg,int id ) const ;
	void next( versionInfo::printVinfo ) const ;
	void check( versionInfo::printVinfo ) const ;
	void check( networkAccess::iterator iter,bool hn ) const
	{
		this->check( { iter.move(),hn } ) ;
	}
	void check( const engines::Iterator& iter,networkAccess::reportDone rd,bool hn ) const
	{
		this->check( { iter,rd.move(),hn } ) ;
	}
	void checkMediaDownloaderUpdate( const engines::EnginesList& ) const ;
private:
	networkAccess::reportDone createReportDone() const ;
	bool allBackendExists( const engines::EnginesList& ) const ;
	versionInfo::printVinfo createPrintVinfo( const engines::EnginesList&,bool ) const ;
	void checkMediaDownloaderUpdate( versionInfo::printVinfo,
					 int,
					 const QByteArray&,
					 const engines::EnginesList&,
					 bool ) const ;
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
			return m_pvInfo.move() ;
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
				    const engines::EnginesList&,
				    bool ) const ;

	const Context& m_ctx ;
	const networkAccess& m_network ;

	bool m_showLocalAndLatestVersions ;
	bool m_showLocalVersionsAndUpdateIfAvailable ;
	bool m_showLocalVersionsOnly ;
};

#endif

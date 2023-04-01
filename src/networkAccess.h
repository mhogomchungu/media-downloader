/*
 *
 *  Copyright (c) 2021
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
#ifndef NETWORK_ACCESS_H
#define NETWORK_ACCESS_H

#include <memory>

#include <QFile>
#include <QStringList>

#include "engines.h"
#include "utils/network_access_manager.hpp"
#include "utils/miscellaneous.hpp"
#include "utils/qtimer.hpp"

class basicdownloader ;
class Context ;
class tabManager ;

class networkAccess
{
public:
	struct status
	{
		virtual void done()
		{
		}
		virtual ~status() ;
	} ;

	class Status
	{
	public:
		template< typename Type,typename ... Args >
		Status( Type,Args&& ... args ) :
			m_handle( std::make_unique< typename Type::type >( std::forward< Args >( args ) ... ) )
		{
		}
		Status() : m_handle( std::make_unique< networkAccess::status>() )
		{
		}
		void done() const
		{
			m_handle->done() ;
		}
		Status move() const
		{
			return std::move( const_cast< Status& >( *this ) ) ;
		}
	private:
		utils::misc::unique_ptr< networkAccess::status > m_handle ;
	};

	struct iter
	{
		virtual ~iter() ;
		virtual const engines::engine& engine() = 0 ;
		virtual bool hasNext() = 0 ;
		virtual void moveToNext() = 0 ;
		virtual void reportDone() = 0 ;
		virtual void failed() = 0 ;
		virtual const engines::Iterator& itr() = 0 ;
	} ;

	class iterator
	{
	public:
		template< typename Type,typename ... Args >
		iterator( Type,Args&& ... args ) :
			m_handle( std::make_unique< typename Type::type >( std::forward< Args >( args ) ... ) )
		{
		}
		iterator()
		{
		}
		bool hasNext() const
		{
			return m_handle->hasNext() ;
		}
		networkAccess::iterator next() const
		{
			auto m = this->move() ;

			m.m_handle->moveToNext() ;

			return m ;
		}
		networkAccess::iterator move() const
		{
			return std::move( *const_cast< networkAccess::iterator * >( this ) ) ;
		}
		const engines::engine& engine() const
		{
			return m_handle->engine() ;
		}
		void reportDone() const
		{
			m_handle->reportDone() ;
		}
		void failed() const
		{
			m_handle->failed() ;
		}
		const engines::Iterator& itr() const
		{
			return m_handle->itr() ;
		}
	private:
		utils::misc::unique_ptr< networkAccess::iter > m_handle ;
	} ;

	networkAccess( const Context& ) ;

	static bool hasNetworkSupport()
	{
		#if QT_VERSION >= QT_VERSION_CHECK( 5,6,0 )
			return true ;
		#else
			return false ;
		#endif
	}
	struct showVersionInfo
	{
		bool show ;
		bool setAfterDownloading ;
	};
	void download( engines::Iterator iter,networkAccess::showVersionInfo v ) const
	{
		class meaw : public networkAccess::iter
		{
		public:
			meaw( engines::Iterator m ) : m_iter( std::move( m ) )
			{
			}
			const engines::engine& engine() override
			{
				return m_iter.engine() ;
			}
			bool hasNext() override
			{
				return m_iter.hasNext() ;
			}
			void moveToNext() override
			{
				m_iter = m_iter.next() ;
			}
			void reportDone() override
			{
			}
			void failed() override
			{
			}
			const engines::Iterator& itr() override
			{
				return m_iter ;
			}
		private:
			engines::Iterator m_iter ;
		};

		this->download( { util::types::type_identity< meaw >(),std::move( iter ) },v ) ;
	}

	void updateMediaDownloader( int,networkAccess::Status ) const ;

	void download( networkAccess::iterator,networkAccess::showVersionInfo ) const ;

	template< typename Function >
	void get( const QString& url,Function function ) const
	{
		this->get( this->make_function( std::move( function ),url ) ) ;
	}
private:
	template< typename Function >
	class function
	{
	public:
		function( Function&& f,const QString& url ) :
			m_function( std::move( f ) ),
			m_url( url )
		{
		}
		function< Function > move() const
		{
			return std::move( this->mutableRef() ) ;
		}
		bool retry() const
		{
			return this->mutableRef().m_retry-- ;
		}
		const QString& url() const
		{
			return m_url ;
		}
		void call( const utils::network::reply& reply ) const
		{
			m_function( reply ) ;
		}
	private:
		function< Function >& mutableRef() const
		{
			return const_cast< function< Function >& >( *this ) ;
		}
		Function m_function ;
		QString m_url ;
		int m_retry = 1 ;
	} ;
	template< typename Function >
	function< Function > make_function( Function&& fnt,const QString& url ) const
	{
		return { std::move( fnt ),url } ;
	}
	template< typename Function >
	void get( Function&& function ) const
	{
		auto m = this->networkRequest( function.url() ) ;

		m_network.get( m,[ this,function = function.move() ]( const utils::network::reply& reply ){

			if( !reply.success() && reply.retry() && function.retry() ){

				utils::qtimer::run( 1000,[ this,function = function.move() ](){

					this->get( function.move() ) ;
				} ) ;

				return ;
			}

			function.call( reply ) ;
		} ) ;
	}

	struct metadata
	{
		qint64 size = 0 ;
		QString url ;
		QString fileName ;
	};

	struct Opts
	{
		Opts( networkAccess::iterator itr,
		      const QString& exePath,
		      const QString& efp,
		      int xd,
		      networkAccess::showVersionInfo svf ) :
			iter( std::move( itr ) ),
			exeBinPath( exePath ),
			tempPath( efp ),
			showVinfo( svf ),
			id( xd )
		{
		}
		void add( networkAccess::metadata&& m )
		{
			metadata = std::move( m ) ;

			filePath = tempPath + "/" + metadata.fileName ;

			isArchive = filePath.endsWith( ".zip" ) || filePath.contains( ".tar." ) ;

			if( !isArchive ){

				filePath += ".tmp" ;
			}
		}
		void openFile()
		{
			m_file = std::make_unique< QFile >( this->filePath ) ;
			m_file->remove() ;
			m_file->open( QIODevice::WriteOnly ) ;
		}
		QFile& file()
		{
			return *m_file ;
		}
		networkAccess::iterator iter ;
		QString exeBinPath ;
		networkAccess::metadata metadata ;
		QString filePath ;
		QString tempPath ;
		QString networkError ;
		utils::misc::unique_ptr< QFile > m_file ;
		bool isArchive = false ;
		networkAccess::showVersionInfo showVinfo ;
		int id ;
	};

	struct updateMDOptions
	{
		QString url ;
		QString tmpFile ;
		QString tmpPath ;
		QString name ;
		QString finalPath ;
		int id ;
		double size ;
		networkAccess::Status status ;
		updateMDOptions move() const
		{
			return std::move( const_cast< updateMDOptions& >( *this ) ) ;
		}
	};

	void removeNotNeededFiles( networkAccess::updateMDOptions ) const ;

	void updateMediaDownloader( networkAccess::updateMDOptions ) const ;

	void extractMediaDownloader( networkAccess::updateMDOptions ) const ;

	QString downloadFailed() const ;

	QNetworkRequest networkRequest( const QString& url ) const ;

	void extractArchive( const engines::engine&,networkAccess::Opts ) const ;

	void download( networkAccess::Opts ) const ;

	void download( const QByteArray&,const engines::engine&,networkAccess::Opts ) const ;

	void finished( networkAccess::Opts ) const ;

	void post( const QString&,const QString&,int ) const ;

	const Context& m_ctx ;
	utils::network::manager m_network ;
	basicdownloader& m_basicdownloader ;
	tabManager& m_tabManager ;
	const QString& m_appName ;
};

#endif

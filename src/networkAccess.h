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
		virtual int id()
		{
			//?????
			return 0 ;
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
		int id()
		{
			return m_handle->id() ;
		}
		Status move()
		{
			return std::move( *this ) ;
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
		networkAccess::iterator next()
		{
			auto m = this->move() ;

			m.m_handle->moveToNext() ;

			return m ;
		}
		networkAccess::iterator move()
		{
			return std::move( *this ) ;
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
	void download( engines::Iterator iter ) const
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

		this->download( { util::types::type_identity< meaw >(),std::move( iter ) } ) ;
	}

	void updateMediaDownloader( networkAccess::Status,const QJsonDocument& ) const ;
	void updateMediaDownloader( networkAccess::Status ) const ;

	void download( networkAccess::iterator ) const ;

	template< typename Function >
	void get( const QString& url,Function function ) const
	{
		this->get( this->make_function( std::move( function ),url ) ) ;
	}
	template< typename FunctionArgs,
		  typename Object,
		  typename Method,
		  typename std::enable_if< std::is_pointer< Object >::value,int >::type = 0,
		  typename std::enable_if< std::is_member_function_pointer< Method >::value,int >::type = 0 >
	void get( const QString& url,FunctionArgs args,Object obj,Method method ) const
	{
		this->get( url,[ args = std::move( args ),obj,method ]( const utils::network::reply& r )mutable{

			( obj->*method )( std::move( args ),r ) ;
		} ) ;
	}
	template< typename Object,
		  typename Method,
		  typename std::enable_if< std::is_pointer< Object >::value,int >::type = 0,
		  typename std::enable_if< std::is_member_function_pointer< Method >::value,int >::type = 0 >
	void get( const QString& url,Object obj,Method method ) const
	{
		this->get( url,[ obj,method ]( const utils::network::reply& r )mutable{

			( obj->*method )( r ) ;
		} ) ;
	}
	template< typename FunctionArgs,
		  typename Object,
		  typename Method,
		  typename std::enable_if< std::is_pointer< Object >::value,int >::type = 0,
		  typename std::enable_if< std::is_member_function_pointer< Method >::value,int >::type = 0 >
	void get( const QNetworkRequest& url,FunctionArgs args,Object obj,Method method ) const
	{
		m_network.get( url,[ args = std::move( args ),obj,method ]( const utils::network::progress& p )mutable{

			( obj->*method )( args,p ) ;
		} ) ;
	}
	template< typename Object,
		  typename Method,
		  typename std::enable_if< std::is_pointer< Object >::value,int >::type = 0,
		  typename std::enable_if< std::is_member_function_pointer< Method >::value,int >::type = 0 >
	void get( const QNetworkRequest& url,Object obj,Method method ) const
	{
		m_network.get( url,[ obj,method ]( const utils::network::progress& p )mutable{

			( obj->*method )( p ) ;
		} ) ;
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
		function< Function > move()
		{
			return std::move( *this ) ;
		}
		bool retry()
		{
			return m_retry-- ;
		}
		const QString& url()
		{
			return m_url ;
		}
		void call( const utils::network::reply& reply )
		{
			m_function( reply ) ;
		}
	private:
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
	void get( Function&& f ) const
	{
		auto m = this->networkRequest( f.url() ) ;

		m_network.get( m,[ this,f = f.move() ]( const utils::network::reply& reply )mutable{

			if( !reply.success() && reply.retry() && f.retry() ){

				utils::qtimer::run( 1000,[ this,f = f.move() ]()mutable{

					this->get( f.move() ) ;
				} ) ;

				return ;
			}

			f.call( reply ) ;
		} ) ;
	}

	struct metadata
	{
		qint64 size = 0 ;
		QString url ;
		QString fileName ;
		metadata move()
		{
			return std::move( *this ) ;
		}
	} ;

	class File
	{
	public:
		bool open( const QString& e )
		{
			m_file = std::make_unique< QFile >( e ) ;
			m_file->remove() ;
			return m_file->open( QIODevice::WriteOnly ) ;
		}
		void close()
		{
			m_file->close() ;
		}
		void rename( const QString& e )
		{
			m_file->rename( e ) ;
		}
		void write( const QByteArray& e )
		{
			m_file->write( e ) ;
		}
		QFile& handle()
		{
			return *m_file ;
		}
	private:
		utils::misc::unique_ptr< QFile > m_file ;
	} ;

	struct Opts
	{
		Opts( networkAccess::iterator itr,
		      const QString& exePath,
		      const QString& efp,
		      int xd ) :
			iter( std::move( itr ) ),
			exeBinPath( exePath ),
			tempPath( efp ),
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
		Opts move()
		{
			return std::move( *this ) ;
		}
		networkAccess::iterator iter ;
		QString exeBinPath ;
		networkAccess::metadata metadata ;
		QString filePath ;
		QString tempPath ;
		mutable QString networkError ;
		bool isArchive = false ;
		int id ;
		Logger::locale locale ;
		networkAccess::File file ;
	} ;

	struct Opts2
	{
		const engines::engine& engine ;
		Opts opts ;
		Opts2 move()
		{
			return std::move( *this ) ;
		}
	} ;

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
		Logger::locale locale ;
		networkAccess::File file ;
		updateMDOptions move()
		{
			return std::move( *this ) ;
		}		
	} ;

	void uMediaDownloaderM( networkAccess::updateMDOptions&,const utils::network::progress& ) const ;
	void uMediaDownloaderN( networkAccess::Status& status,const utils::network::progress& p ) const ;

	void downloadP( networkAccess::Opts2&,const utils::network::progress& ) const ;
	void downloadP2( networkAccess::Opts2&,const utils::network::progress& ) const ;

	void updateMediaDownloader( networkAccess::updateMDOptions ) const ;
	void extractMediaDownloader( networkAccess::updateMDOptions ) const ;

	void emDownloader( networkAccess::updateMDOptions,const utils::qprocess::outPut& ) const ;
	void extractArchiveOuput( networkAccess::Opts,const utils::qprocess::outPut& ) const ;

	void postStartDownloading( const QString&,int ) const ;
	void postDownloading( const QString&,const QString&,int ) const ;
	void postDestination( const QString&,const QString&,int ) const ;
	void postDownloadingProgress( const QString&,const QString&,int ) const ;

	QString downloadFailed() const ;

	QNetworkRequest networkRequest( const QString& url ) const ;

	void extractArchive( const engines::engine&,networkAccess::Opts ) const ;

	void download( networkAccess::Opts ) const ;

	void download( const QByteArray&,const engines::engine&,networkAccess::Opts ) const ;

	void finished( networkAccess::Opts ) const ;

	void post( const QString&,const QString&,int ) const ;

	QString reportError( const utils::network::progress& ) const ;

	const Context& m_ctx ;
	utils::network::manager m_network ;
	basicdownloader& m_basicdownloader ;
	tabManager& m_tabManager ;
	const QString& m_appName ;
};

#endif

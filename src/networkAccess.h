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
#include <QCryptographicHash>

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
	struct report
	{
		virtual void done()
		{
		}
		virtual void failed()
		{
		}
		virtual ~report() ;
	} ;

	struct status
	{
		virtual void done() = 0 ;
		virtual int id() = 0 ;
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

	class reportDone
	{
	public:
		template< typename Type,typename ... Args >
		reportDone( Type,Args&& ... args ) :
			m_handle( std::make_unique< typename Type::type >( std::forward< Args >( args ) ... ) )
		{
		}
		void done() const
		{
			m_handle->done() ;
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
		utils::misc::unique_ptr< networkAccess::report > m_handle ;
	} ;

	class iterator
	{
	public:
		iterator( engines::Iterator iter,networkAccess::reportDone rd ) :
			m_rd( rd.move() ),m_iter( iter.move() )
		{
		}
		iterator move()
		{
			return std::move( *this ) ;
		}
		const engines::engine& engine() const
		{
			return m_iter.engine() ;
		}
		engines::Iterator getItor()
		{
			return m_iter.move() ;
		}
		networkAccess::reportDone getRD()
		{
			return m_rd.move() ;
		}
		void reportDone()
		{
			m_rd.done() ;
		}
		void repordFailed()
		{
			m_rd.failed() ;
		}
	private:
		networkAccess::reportDone m_rd ;
		engines::Iterator m_iter ;
	} ;

	networkAccess( const Context& ) ;

	void setProxySettings( const QNetworkProxy& s )
	{
		this->QtNAM().setProxy( s ) ;
	}
	QNetworkAccessManager& QtNAM() const
	{
		return m_network.QtNAM() ;
	}
	void updateMediaDownloader( networkAccess::Status,const QJsonDocument& ) const ;
	void updateMediaDownloader( networkAccess::Status ) const ;

	void download( engines::Iterator,networkAccess::reportDone ) const ;

	QByteArray defaultUserAgent() const ;

	template< typename Function >
	void get( const QString& url,Function function,const QByteArray& userAgent = {} ) const
	{
		this->get( this->make_function( std::move( function ),url ),userAgent ) ;
	}
	template< typename FunctionArgs,
		  typename Object,
		  typename Method,
		  typename std::enable_if< std::is_pointer< Object >::value,int >::type = 0,
		  typename std::enable_if< std::is_member_function_pointer< Method >::value,int >::type = 0 >
	void get( const QString& url,FunctionArgs args,Object obj,Method method,const QByteArray& userAgent = {} ) const
	{
		class meaw
		{
		public:
			meaw( FunctionArgs args,Object obj,Method m ) :
				m_args( std::move( args ) ),m_obj( obj ),m_method( m )
			{
			}
			void operator()( const utils::network::reply& r )
			{
				( m_obj->*m_method )( std::move( m_args ),r ) ;
			}
		private:
			FunctionArgs m_args ;
			Object m_obj ;
			Method m_method ;
		} ;

		this->get( url,meaw( std::move( args ),obj,method ),userAgent ) ;
	}
	template< typename Object,
		  typename Method,
		  typename std::enable_if< std::is_pointer< Object >::value,int >::type = 0,
		  typename std::enable_if< std::is_member_function_pointer< Method >::value,int >::type = 0 >
	void get( const QString& url,Object obj,Method method,const QByteArray& userAgent = {} ) const
	{
		class meaw
		{
		public:
			meaw( Object obj,Method m ) : m_obj( obj ),m_method( m )
			{
			}
			void operator()( const utils::network::reply& r )
			{
				( m_obj->*m_method )( r ) ;
			}
		private:
			Object m_obj ;
			Method m_method ;
		} ;

		this->get( url,meaw( obj,method ),userAgent ) ;
	}
	template< typename FunctionArgs,
		  typename Object,
		  typename Method,
		  typename std::enable_if< std::is_pointer< Object >::value,int >::type = 0,
		  typename std::enable_if< std::is_member_function_pointer< Method >::value,int >::type = 0 >
	void get( const QNetworkRequest& url,FunctionArgs args,Object obj,Method method ) const
	{
		class meaw
		{
		public:
			meaw( FunctionArgs args,Object obj,Method m ) :
				m_args( std::move( args ) ),m_obj( obj ),m_method( m )
			{
			}
			void operator()( const utils::network::progress& r )
			{
				( m_obj->*m_method )( m_args,r ) ;
			}
		private:
			FunctionArgs m_args ;
			Object m_obj ;
			Method m_method ;
		} ;

		m_network.get( url,meaw( std::move( args ),obj,method ) ) ;
	}
	template< typename Object,
		  typename Method,
		  typename std::enable_if< std::is_pointer< Object >::value,int >::type = 0,
		  typename std::enable_if< std::is_member_function_pointer< Method >::value,int >::type = 0 >
	void get( const QNetworkRequest& url,Object obj,Method method ) const
	{
		class meaw
		{
		public:
			meaw( Object obj,Method m ) : m_obj( obj ),m_method( m )
			{
			}
			void operator()( const utils::network::progress& r )
			{
				( m_obj->*m_method )( r ) ;
			}
		private:
			Object m_obj ;
			Method m_method ;
		} ;

		m_network.get( url,meaw( obj,method ) ) ;
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
	void get( Function function,const QByteArray& userAgent = {} ) const
	{
		struct args
		{
			const networkAccess& parent ;
			Function function ;
			QByteArray userAgent ;
		} ;

		class meaw
		{
		public:
			meaw( args a ) : m_args( std::move( a ) )
			{
			}
			void operator()( const utils::network::reply& reply )
			{
				class woof
				{
				public:
					woof( args a ) : m_args( std::move( a ) )
					{
					}
					void operator()()
					{
						m_args.parent.get( m_args.function.move(),m_args.userAgent ) ;
					}
				private:
					args m_args ;
				} ;

				if( !reply.success() && reply.retry() && m_args.function.retry() ){

					utils::qtimer::run( 1000,woof( std::move( m_args ) ) ) ;
				}else{
					m_args.function.call( reply ) ;
				}
			}
		private:
			args m_args ;
		} ;

		auto m = this->networkRequest( function.url(),userAgent ) ;

		m_network.get( m,meaw( { *this,std::move( function ),userAgent } ) ) ;
	}

	class File
	{
	public:
		bool open( const QString& e )
		{
			m_path = e ;
			m_file = std::make_unique< QFile >( e ) ;
			m_file->remove() ;
			return m_file->open( QIODevice::WriteOnly ) ;
		}
		void close()
		{
			m_file->close() ;
		}
		QString rename( const QString& e ) ;
		void write( const QByteArray& e )
		{
			m_file->write( e ) ;
		}
		QFile& handle() const
		{
			return *m_file ;
		}
		const QString& src() const
		{
			return m_path ;
		}
	private:
		QString m_path ;
		utils::misc::unique_ptr< QFile > m_file ;
	} ;

	class cmdArgs
	{
	public:
		cmdArgs()
		{
		}
		cmdArgs( const QString& exe,const QStringList& args ) : m_exe( exe )
		{
			m_args = "\"" + args[ 0 ] + "\"" ;

			for( int s = 1 ; s < args.size() ; s++ ){

				m_args += " \"" + args[ s ] + "\"" ;
			}
		}
		const QString& exe() const
		{
			return m_exe ;
		}
		const QString& args() const
		{
			return m_args ;
		}
	private:
		QString m_exe ;
		QString m_args ;
	} ;
	struct Opts
	{
		Opts( networkAccess::iterator itr,
		      const QString& exePath,
		      const QString& efp,
		      int xd ) :
			iter( itr.move() ),
			exeBinPath( exePath ),
			tempPath( efp ),
			id( xd ),
			hashCalculator( std::make_unique< QCryptographicHash >( QCryptographicHash::Sha256 ) )
		{
		}
		void add( engines::metadata m )
		{
			metadata = m.move() ;

			filePath = tempPath + "/" + metadata.fileName() ;

			isArchive = filePath.endsWith( ".zip" ) || filePath.contains( ".tar." ) ;

			if( !isArchive ){

				filePath += ".tmp" ;
			}
		}
		Opts move()
		{
			return std::move( *this ) ;
		}
		networkAccess::iterator moveIter()
		{
			return iter.move() ;
		}
		const engines::engine& engine()
		{
			return iter.engine() ;
		}
		void reportDone()
		{
			iter.reportDone() ;
		}
		void reportFailed()
		{
			iter.repordFailed() ;
		}
		networkAccess::iterator iter ;
		QString exeBinPath ;
		engines::metadata metadata ;
		QString filePath ;
		QString tempPath ;
		cmdArgs exeArgs ;
		class NetworkError
		{
		public:
			NetworkError()
			{
			}
			NetworkError( const QString& e )
			{
				m_errors.emplace_back( e ) ;
			}
			void operator=( const QString& e )
			{
				m_errors.clear() ;
				m_errors.emplace_back( e ) ;
			}
			template< typename ... R >
			void add( R&& ... args )
			{
				m_errors.clear() ;
				this->addImpl( std::forward< R >( args ) ... ) ;
			}
			auto begin() const
			{
				return m_errors.begin() ;
			}
			auto end() const
			{
				return m_errors.end() ;
			}
			bool isNotEmpty() const
			{
				return m_errors.size() > 0 ;
			}
			void setbadDownload()
			{
				m_errors.clear() ;
				m_errors.emplace_back( "Bad Download" ) ;
			}
			bool badDownload() const
			{
				if( m_errors.size() == 1 ){

					return m_errors[ 0 ] == "Bad Download" ;
				}else{
					return false ;
				}
			}
		private:
			void addImpl()
			{
			}
			template< typename F,typename ... R >
			void addImpl( const F& f,R&& ... r )
			{
				m_errors.emplace_back( f ) ;
				this->addImpl( std::forward< R >( r ) ... ) ;
			}
			std::vector< QString > m_errors ;
		};

		mutable NetworkError networkError ;
		bool isArchive = false ;
		int id ;
		Logger::locale locale ;
		std::unique_ptr< QCryptographicHash > hashCalculator ;
		networkAccess::File file ;
	} ;

	struct updateMDOptions
	{
		updateMDOptions( const QJsonObject& obj,networkAccess::Status st ) :
			url( obj.value( "browser_download_url" ).toString() ),
			name( obj.value( "name" ).toString() ),
			hash( obj.value( "digest" ).toString() ),
			id( st.id() ),
			size( obj.value( "size" ).toDouble() ),
			status( st.move() ),
			hashCalculator( std::make_unique< QCryptographicHash >( QCryptographicHash::Sha256 ) )
		{
		}
		QString url ;
		QString tmpFile ;
		QString tmpPath ;
		QString name ;
		QString finalPath ;
		QString hash ;
		cmdArgs exeArgs ;
		int id ;
		double size ;
		networkAccess::Status status ;
		Logger::locale locale ;
		networkAccess::File file ;
		std::unique_ptr< QCryptographicHash > hashCalculator ;
		updateMDOptions move()
		{
			return std::move( *this ) ;
		}		
	} ;

	void printVersion( networkAccess::Opts,bool ) const ;

	void uMediaDownloaderM( networkAccess::updateMDOptions&,const utils::network::progress& ) const ;
	void uMediaDownloaderN( networkAccess::Status& status,const utils::network::progress& p ) const ;

	void downloadP( networkAccess::Opts&,const utils::network::progress& ) const ;
	void downloadP2( networkAccess::Opts&,const utils::network::progress& ) const ;

	void updateMediaDownloader( networkAccess::updateMDOptions ) const ;
	void extractMediaDownloader( networkAccess::updateMDOptions ) const ;

	void emDownloader( networkAccess::updateMDOptions,const utils::qprocess::outPut& ) const ;
	void extractArchiveOuput( networkAccess::Opts,const utils::qprocess::outPut& ) const ;

	void postStartDownloading( const QString&,int ) const ;
	void postDownloading( const QString&,const QString&,int ) const ;
	void postDestination( const QString&,const QString&,int ) const ;
	void postDownloadingProgress( const QString&,const QString&,int ) const ;

	void hashDoNotMatch( const QString&,const QString&,int ) const ;

	QString downloadFailed() const ;

	QNetworkRequest networkRequest( const QString& url,const QByteArray& userAgent = {} ) const ;

	void extractArchive( networkAccess::Opts ) const ;

	void download( networkAccess::Opts ) const ;

	void download( const QByteArray&,networkAccess::Opts ) const ;

	void finished( networkAccess::Opts ) const ;

	void post( const QString&,const QString&,int ) const ;

	QString reportError( const utils::network::progress& ) const ;

	void failedToExtract( const networkAccess::cmdArgs&,const utils::qprocess::outPut&,int ) const ;
	void failedToRemove( const QString&,
			     const engines::engine::baseEngine::removeFilesStatus&,
			     int ) const ;
	void failedToRemove( const QString&,const QString&,const QString&,int ) const ;
	void failedToRename( const QString& name,
			     const QString& src,
			     const QString& dst,
			     const QString& err,
			     int id ) const ;

	const Context& m_ctx ;
	utils::network::manager m_network ;
	basicdownloader& m_basicdownloader ;
	tabManager& m_tabManager ;
	const QString& m_appName ;
};

#endif

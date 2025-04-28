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

#ifndef CCDOWNLOAD_MG_H
#define CCDOWNLOAD_MG_H

#include <QTableWidget>
#include <QStringList>
#include <QPushButton>

#include <memory>

#include "engines.h"
#include "context.hpp"
#include "utility.h"
#include "tableWidget.h"

class downloadManager
{
public:
	class finishedStatus
	{
	public:
		static QString notStarted()
		{
			return "Not Started" ;
		}
		static QString running()
		{
			return "Running" ;
		}
		static QString finishedCancelled()
		{
			return "FinishedCancelled" ;
		}
		static QString finishedWithError()
		{
			return "FinishedWithError" ;
		}
		static QString finishedWithSuccess()
		{
			return "FinishedWithSuccess" ;
		}
		static bool notStarted( const QString& e )
		{
			return notStarted() == e ;
		}
		static bool running( const QString& e )
		{
			return running() == e ;
		}
		static bool finishedCancelled( const QString& e )
		{
			return finishedCancelled() == e ;
		}
		static bool finishedWithError( const QString& e )
		{
			return finishedWithError() == e ;
		}
		static bool finishedWithSuccess( const QString& e )
		{
			return finishedWithSuccess() == e  ;
		}
		static bool finishedCancelled( const tableWidget& e,int row )
		{
			return finishedCancelled( e.runningState( row ) ) ;
		}
		static bool finishedWithError( const tableWidget& e,int row )
		{
			return finishedWithError( e.runningState( row ) ) ;
		}
		static bool finishedWithSuccess( const tableWidget& e,int row )
		{
			return finishedWithSuccess( e.runningState( row ) ) ;
		}
		QString setState() const
		{
			if( this->exitState().cancelled()){

				return finishedCancelled() ;

			}else if( this->exitState().success() ){

				return finishedWithSuccess() ;
			}else{
				return finishedWithError() ;
			}
		}
		int index() const
		{
			return m_index ;
		}
		int lastIndex() const
		{
			return m_lastIndex ;
		}
		enum class state{ cancelled,done,running } ;
		bool continuing() const
		{
			return m_state == state::running ;
		}
		bool cancelled() const
		{
			return m_state == state::cancelled ;
		}
		bool done() const
		{
			return m_state == state::done ;
		}
		finishedStatus move()
		{
			return std::move( *this ) ;
		}
		const engines::ProcessExitState& exitState() const
		{
			return m_exitState ;
		}
		finishedStatus( int i,int l,state s,engines::ProcessExitState e ) :
			m_index( i ),
			m_lastIndex( l ),
			m_state( s ),
			m_exitState( std::move( e ) )
		{
		}
		finishedStatus( state s,engines::ProcessExitState e ) :
			m_index( 0 ),
			m_lastIndex( 0 ),
			m_state( s ),
			m_exitState( std::move( e ) )
		{
		}
		finishedStatus()
		{
		}
	private:
		int m_index ;
		int m_lastIndex ;
		state m_state ;
		engines::ProcessExitState m_exitState ;
	};

	class index
	{
	public:
		enum class tab{ batch = 0,playlist = 1 } ;

		index()
		{
		}
		index( tableWidget& t,downloadManager::index::tab i ) :
			m_init_position( i ),
			m_table( &t )
		{
		}
		void add( index idx )
		{
			m_init_position = idx.m_init_position ;
			m_table = idx.m_table ;

			for( auto& it : idx.m_entries ){

				m_entries.emplace_back( std::move( it ) ) ;
			}
		}
		int value( int s ) const
		{
			return this->Entry( s ).index ;
		}
		const utility::downLoadOptions& options( int s ) const
		{
			return this->Entry( s ).options ;
		}
		bool forceDownload( int s ) const
		{
			return this->Entry( s ).forceDownload ;
		}
		bool forceDownload() const
		{
			return this->forceDownload( m_index ) ;
		}
		int value() const
		{
			return this->value( m_index ) ;
		}
		size_t count() const
		{
			return m_entries.size() ;
		}
		void next()
		{
			m_index++ ;
		}
		bool hasNext() const
		{
			return static_cast< size_t >( m_index ) < m_entries.size() ;
		}
		tableWidget& table() const
		{
			return *m_table ;
		}
		const tableWidget::entry& tableEntryAtIndex()
		{
			auto s = static_cast< int >( m_init_position ) ;
			return m_table->entryAt( static_cast< size_t >( m_index + s ) ) ;
		}
		void add( int index,utility::downLoadOptions opts,bool forceUpdate = false )
		{
			m_entries.emplace_back( index,opts.move(),forceUpdate ) ;
		}
		int lastIndex() const
		{
			return m_entries.rbegin()->index ;
		}
		index move()
		{
			return std::move( *this ) ;
		}
		bool empty() const
		{
			return m_entries.empty() ;
		}
		const utility::downLoadOptions& options() const
		{
			return this->options( m_index ) ;
		}
		utility::uiIndex uiIndex() const
		{
			if( m_init_position == index::tab::batch ){

				return { this->Entry( m_index ).index + 1,m_table->rowCount() } ;
			}else{
				return { this->Entry( m_index ).index,m_table->rowCount() } ;
			}
		}
	private:
		struct entry
		{
			entry( int i,utility::downLoadOptions o,bool s ) :
				index( i ),
				options( o.move() ),
				forceDownload( s )
			{
			}
			int index ;
			utility::downLoadOptions options ;
			bool forceDownload ;
		} ;
		const entry& Entry( int s ) const
		{
			return m_entries[ static_cast< size_t >( s ) ] ;
		}
		int m_index = 0 ;
		downloadManager::index::tab m_init_position ;
		std::vector< entry > m_entries ;
		tableWidget * m_table ;
	};

	downloadManager( const Context& ctx,QPushButton& cb,settings& s ) :
		m_ctx( ctx ),
		m_cancelButton( cb ),
		m_settings( s )
	{
	}
	void cancelled()
	{
		m_cancelled = true ;
	}
	template< typename Event >
	void monitorForFinished( const engines::engine& engine,
				 int index,
				 engines::ProcessExitState exitState,
				 Event event )
	{
		m_currentlyDownloadingNumber-- ;

		if( m_cancelled ){

			m_cancelButton.setEnabled( false ) ;

			auto a = index ;
			auto b = m_index.lastIndex() ;
			auto c = finishedStatus::state::cancelled ;

			event.finished( { a,b,c,exitState.move() } ) ;
		}else{
			m_counter++ ;

			auto a = index ;
			auto b = m_index.lastIndex() ;

			if( m_counter == m_index.count() ){

				auto c = finishedStatus::state::done ;

				event.finished( { a,b,c,exitState.move() } ) ;

				if( m_index.table().noneAreRunning() ){

					m_cancelButton.setEnabled( false ) ;
				}
			}else{
				auto c = finishedStatus::state::running ;

				event.finished( { a,b,c,exitState.move() } ) ;

				if( m_index.hasNext() ){

					event.next( engine,m_index.value() ) ;
				}
			}
		}
	}
	template< typename ConcurrentDownload >
	void download_add( const engines::engine& engine,
			   downloadManager::index index,
			   size_t maxNumberOfConcurrency,
			   ConcurrentDownload concurrentDownload )
	{
		m_maximumConcurrency = maxNumberOfConcurrency ;

		auto idx = index.value() ;

		m_index.add( index.move() ) ;

		if( m_currentlyDownloadingNumber < m_maximumConcurrency ){

			m_cancelButton.setEnabled( true ) ;

			concurrentDownload( engine,idx ) ;
		}
	}

	template< typename ConcurrentDownload >
	void download( downloadManager::index index,
		       const engines::engine& engine,
		       size_t maxNumberOfConcurrency,
		       ConcurrentDownload concurrentDownload )
	{
		m_index = index.move() ;

		m_maximumConcurrency = maxNumberOfConcurrency ;

		m_counter = 0 ;
		m_cancelled = false ;

		this->uiEnableAll( m_ctx,false ) ;
		m_cancelButton.setEnabled( true ) ;
		m_index.table().setEnabled( true ) ;

		auto min = std::min( m_index.count(),m_maximumConcurrency ) ;

		for( size_t s = 0 ; s < min ; s++ ){

			concurrentDownload( engine,m_index.value( s ) ) ;
		}
	}
	template< typename Options,typename Logger,typename TermSignal >
	void download( QStringList cliOptions,
		       const QString& url,
		       TermSignal conn,
		       Options opts,
		       Logger logger,
		       utility::ProcessOutputChannels channel = utility::ProcessOutputChannels() )
	{
		m_index.next() ;

		cliOptions.append( url ) ;

		auto ctx = utility::make_ctx( opts.move(),logger.move(),conn.move(),channel ) ;

		m_currentlyDownloadingNumber++ ;

		utility::run( cliOptions,QString(),ctx.move() ) ;
	}
	template< typename Options,typename Logger,typename TermSignal,typename OptionUpdater >
	void download( const engines::engine& engine,
		       const OptionUpdater& optsUpdater,
		       const QString& uiDownloadOptions,
		       const QString& url,
		       const Context& cctx,
		       TermSignal term,
		       Options opts,
		       Logger logger,
		       utility::ProcessOutputChannels channel = utility::ProcessOutputChannels() )
	{
		m_currentlyDownloadingNumber++ ;

		const auto& m = m_index.options() ;

		const auto& uiIndex = m_index.uiIndex() ;

		bool fd = m_index.forceDownload() ;

		const auto& e = m_index.tableEntryAtIndex() ;

		m_index.next() ;

		utility::args args( uiDownloadOptions,m.downloadOptions,engine ) ;

		utility::updateOptionsStruct opt{ m,engine,m_settings,args,uiIndex,fd,{ url },e,cctx } ;

		auto ctx = utility::make_ctx( opts.move(),logger.move(),term.move(),channel ) ;

		auto u = optsUpdater( utility::updateOptions( opt ) ) ;

		utility::run( std::move( u ),args.credentials(),ctx.move() ) ;
	}
private:
	template< typename Cxt >
	void uiEnableAll( const Cxt& ctx,bool e )
	{
		if( e ){

			ctx.TabManager().enableAll() ;
		}else{
			ctx.TabManager().disableAll() ;
		}
	}
	size_t m_counter = 0 ;
	downloadManager::index m_index ;
	size_t m_maximumConcurrency ;
	size_t m_currentlyDownloadingNumber = 0 ;
	bool m_cancelled = false ;
	const Context& m_ctx ;
	QPushButton& m_cancelButton ;
	settings& m_settings ;
} ;

class reportFinished
{
public:
	reportFinished()
	{
	}
	reportFinished( const engines::engine& engine,
			downloadManager::finishedStatus status ) :
		m_engine( &engine ),
		m_status( std::move( status ) )
	{
	}
	const engines::engine& engine() const
	{
		return *m_engine ;
	}
	const downloadManager::finishedStatus& finishedStatus() const
	{
		return m_status ;
	}
	reportFinished move()
	{
		return std::move( *this ) ;
	}
private:
	const engines::engine * m_engine = nullptr ;
	downloadManager::finishedStatus m_status ;
};

Q_DECLARE_METATYPE( reportFinished )

#endif

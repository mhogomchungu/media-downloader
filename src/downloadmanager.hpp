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
		static bool finishedCancelled( tableWidget& e,int row )
		{
			return finishedCancelled( e.runningState( row ) ) ;
		}
		static bool finishedWithError( tableWidget& e,int row )
		{
			return finishedWithError( e.runningState( row ) ) ;
		}
		static bool finishedWithSuccess( tableWidget& e,int row )
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
		bool batchDownloading() const
		{
			return m_batchDownloading ;
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
		const utility::ProcessExitState& exitState() const
		{
			return m_exitState ;
		}
		finishedStatus( int i,int l,bool m,state s,utility::ProcessExitState e ) :
			m_index( i ),
			m_lastIndex( l ),
			m_batchDownloading( m ),
			m_state( s ),
			m_exitState( std::move( e ) )
		{
		}
		finishedStatus( state s,utility::ProcessExitState e ) :
			m_index( 0 ),
			m_lastIndex( 0 ),
			m_batchDownloading( false ),
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
		bool m_batchDownloading ;
		state m_state ;
		utility::ProcessExitState m_exitState ;
	};

	class index
	{
	public:
		enum class tab{ batch = 0,playlist = 1 } ;

		index( tableWidget& t,bool n,downloadManager::index::tab i ) :
			m_batchDownloading( n ),
			m_init_position( i ),
			m_table( t )
		{
		}
		int value( int s ) const
		{
			return this->Entry( s ).index ;
		}
		const utility::downLoadOptions& options( int s ) const
		{
			return this->Entry( s ).options ;
		}
		bool batchDownloading() const
		{
			return m_batchDownloading ;
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
			return m_table ;
		}
		const tableWidget::entry& tableEntryAtIndex()
		{
			auto s = static_cast< int >( m_init_position ) ;
			return m_table.entryAt( static_cast< size_t >( m_index + s ) ) ;
		}
		void add( int index,utility::downLoadOptions opts,bool forceUpdate = false )
		{
			m_entries.emplace_back( index,std::move( opts ),forceUpdate ) ;
		}
		int lastIndex() const
		{
			return m_entries.rbegin()->index ;
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

				return { this->Entry( m_index ).index + 1,m_table.rowCount() } ;
			}else{
				return { this->Entry( m_index ).index,m_table.rowCount() } ;
			}
		}
	private:
		struct entry
		{
			entry( int i,utility::downLoadOptions o,bool s ) :
				index( i ),
				options( std::move( o ) ),
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
		bool m_batchDownloading ;
		int m_index = 0 ;
		downloadManager::index::tab m_init_position ;
		std::vector< entry > m_entries ;
		tableWidget& m_table ;
	};

	downloadManager( const Context& ctx,
	                 QPushButton& cancelButton,
	                 settings& s ) :
		m_ctx( ctx ),
		m_cancelButton( cancelButton ),
		m_settings( s )
	{
	}
	void cancelled()
	{
		m_cancelled = true ;
	}
	template< typename Function,typename Finished >
	void monitorForFinished( const engines::engine& engine,
				 int index,
				 utility::ProcessExitState exitState,
				 Function function,
				 Finished finished )
	{
		if( m_cancelled ){

			m_cancelButton.setEnabled( false ) ;

			auto a = index ;
			auto b = m_index->lastIndex() ;
			auto c = m_index->batchDownloading() ;
			auto d = finishedStatus::state::cancelled ;

			finished( { a,b,c,d,std::move( exitState ) } ) ;
		}else{
			m_counter++ ;

			auto a = index ;
			auto b = m_index->lastIndex() ;
			auto c = m_index->batchDownloading() ;

			if( m_counter == m_index->count() ){

				if( m_index->table().noneAreRunning() ){

					m_cancelButton.setEnabled( false ) ;
				}

				auto d = finishedStatus::state::done ;

				finished( { a,b,c,d,std::move( exitState ) } ) ;
			}else{
				auto d = finishedStatus::state::running ;

				finished( { a,b,c,d,std::move( exitState ) } ) ;

				if( m_index->hasNext() ){

					function( engine,m_index->value() ) ;
				}
			}
		}
	}
	template< typename ConcurrentDownload >
	void download( downloadManager::index index,
		       const engines::engine& engine,
		       size_t maxNumberOfConcurrency,
		       ConcurrentDownload concurrentDownload )
	{
		m_index = std::move( index ) ;

		m_counter = 0 ;
		m_cancelled = false ;

		this->uiEnableAll( m_ctx,false ) ;
		m_cancelButton.setEnabled( true ) ;
		m_index->table().setEnabled( true ) ;

		auto min = std::min( m_index->count(),maxNumberOfConcurrency ) ;

		for( size_t s = 0 ; s < min ; s++ ){

			concurrentDownload( engine,m_index->value( s ) ) ;
		}
	}
	template< typename Options,typename Logger,typename TermSignal >
	void download( const engines::engine& engine,
		       QStringList cliOptions,
		       const QString& url,
		       TermSignal conn,
		       Options opts,
		       Logger logger,
		       utility::ProcessOutputChannels channel = utility::ProcessOutputChannels() )
	{
		m_index->next() ;

		cliOptions.append( url ) ;

		auto ctx = utility::make_ctx( engine,
					      std::move( opts ),
					      std::move( logger ),
					      std::move( conn ),
					      channel ) ;

		utility::run( cliOptions,QString(),std::move( ctx ) ) ;
	}
	template< typename Options,typename Logger,typename TermSignal,typename OptionUpdater >
	void download( const engines::engine& engine,
		       const OptionUpdater& optsUpdater,
		       const QString& url,
		       const Context& cctx,
		       TermSignal terminator,
		       Options opts,
		       Logger logger,
		       utility::ProcessOutputChannels channel = utility::ProcessOutputChannels() )
	{
		const auto& m = m_index->options() ;

		const auto& uiIndex = m_index->uiIndex() ;

		bool fd = m_index->forceDownload() ;

		const auto& e = m_index->tableEntryAtIndex() ;

		m_index->next() ;

		utility::args args( m.downloadOptions,engine ) ;

		utility::updateOptionsStruct opt{ m,engine,m_settings,args,uiIndex,fd,{ url },e,cctx } ;

		auto ctx = utility::make_ctx( engine,
					      std::move( opts ),
					      std::move( logger ),
					      std::move( terminator ),
					      channel ) ;

		utility::run( optsUpdater( utility::updateOptions( opt ) ),
			      args.quality(),
			      std::move( ctx ) ) ;
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
	size_t m_counter ;
	util::storage< downloadManager::index > m_index ;
	bool m_cancelled ;
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
	reportFinished( const engines::engine& engine,const downloadManager::finishedStatus& status ) :
		m_engine( &engine ),
		m_status( status )
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
private:
	const engines::engine * m_engine = nullptr ;
	downloadManager::finishedStatus m_status ;
};

Q_DECLARE_METATYPE( reportFinished )

#endif

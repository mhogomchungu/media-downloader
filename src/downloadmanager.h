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
		void setState( QTableWidgetItem& item ) const
		{
			if( this->exitState().cancelled()){

				item.setText( finishedCancelled() ) ;

			}else if( this->exitState().success() ){

				item.setText( finishedWithSuccess() ) ;
			}else{
				item.setText( finishedWithError() ) ;
			}
		}
		int index() const
		{
			return m_index ;
		}
		bool allFinished() const
		{
			return m_allFinished ;
		}
		const utility::ProcessExitState& exitState() const
		{
			return m_exitState ;
		}
		finishedStatus( int i,bool s,utility::ProcessExitState e ) :
			m_index( i ),m_allFinished( s ),m_exitState( std::move( e ) )
		{
		}
	private:
		int m_index ;
		bool m_allFinished ;
		utility::ProcessExitState m_exitState ;
	};

	class index
	{
	public:
		index( tableWidget& t ) :
			m_table( t )
		{
		}
		int value() const
		{
			return m_entries[ m_index ].index ;
		}
		int value( int s ) const
		{
			return m_entries[ static_cast< size_t >( s ) ].index ;
		}
		const QString& options( int s ) const
		{
			return m_entries[ static_cast< size_t >( s ) ].options ;
		}
		int count() const
		{
			return static_cast< int >( m_entries.size() ) ;
		}
		void next()
		{
			m_index++ ;
		}
		bool hasNext() const
		{
			return m_index < m_entries.size() ;
		}
		tableWidget& table() const
		{
			return m_table ;
		}
		void add( int index,const QString& url )
		{
			m_entries.emplace_back( index,url ) ;
		}
		bool empty() const
		{
			return m_entries.empty() ;
		}
		const QString& options() const
		{
			return this->options( static_cast< int >( m_index ) ) ;
		}
		QString indexAsString() const
		{
			return this->indexAsString( m_index ) ;
		}
		template< typename T>
		QString indexAsString( T s ) const
		{
			auto m = QString::number( s + 1 ) ;
			auto r = QString::number( m_table.rowCount() + 1 ) ;

			while( r.size() > m.size() ){

				m = "0" + m ;
			}

			return m ;
		}
	private:
		struct entry
		{
			entry( int i,const QString& o ) :
				index( i ),
				options( o )
			{
			}
			int index ;
			QString options ;
		} ;
		size_t m_index = 0 ;
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

			finished( { index,true,std::move( exitState ) } ) ;
		}else{
			m_counter++ ;

			if( m_counter == m_index->count() ){

				if( m_index->table().noneAreRunning() ){

					m_cancelButton.setEnabled( false ) ;
				}

				finished( { index,true,std::move( exitState ) } ) ;
			}else{
				finished( { index,false,std::move( exitState ) } ) ;

				if( m_index->hasNext() ){

					function( engine,m_index->value() ) ;
				}
			}
		}
	}
	template< typename ConcurrentDownload >
	void download( downloadManager::index index,
		       const engines::engine& engine,
		       int maxNumberOfConcurrency,
		       ConcurrentDownload concurrentDownload )
	{
		m_index = std::move( index ) ;

		m_counter = 0 ;
		m_cancelled = false ;

		this->uiEnableAll( false ) ;
		m_cancelButton.setEnabled( true ) ;
		m_index->table().setEnabled( true ) ;

		auto min = std::min( m_index->count(),maxNumberOfConcurrency ) ;

		for( int s = 0 ; s < min ; s++ ){

			concurrentDownload( engine,m_index->value( s ) ) ;
		}
	}
	template< typename Options,typename Logger,typename TermSignal >
	void download( const engines::engine& engine,
		       const QString& cliOptions,
		       const QString& url,
		       TermSignal conn,
		       Options opts,
		       Logger logger )
	{
		m_index->next() ;

		utility::run( engine,
			      { cliOptions,url },
			      "",
			      std::move( opts ),
			      std::move( logger ),
			      std::move( conn ) ) ;
	}
	template< typename Options,typename Logger,typename TermSignal >
	void download( const engines::engine& engine,
		       QTableWidgetItem& index,
		       const QString& url,
		       TermSignal terminator,
		       Options opts,
		       Logger logger )
	{
		const auto& m = m_index->options() ;

		auto indexAsString = m_index->indexAsString() ;

		m_index->next() ;

		index.setText( finishedStatus::running() ) ;

		utility::args args( m ) ;

		utility::run( engine,
			      utility::updateOptions( engine,m_settings,args,indexAsString,{ url } ),
			      args.quality(),
			      std::move( opts ),
			      std::move( logger ),
			      std::move( terminator ) ) ;
	}
private:
	void uiEnableAll( bool e ) ;
	int m_counter ;
	util::storage< downloadManager::index > m_index ;
	bool m_cancelled ;
	const Context& m_ctx ;
	QPushButton& m_cancelButton ;
	settings& m_settings ;
} ;

#endif

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
		static bool finishedCancelled( QTableWidget& e,int row )
		{
			return finishedCancelled( e.item( row,2 )->text() ) ;
		}
		static bool finishedWithError( QTableWidget& e,int row )
		{
			return finishedWithError( e.item( row,2 )->text() ) ;
		}
		static bool finishedWithSuccess( QTableWidget& e,int row )
		{
			return finishedWithSuccess( e.item( row,2 )->text() ) ;
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
		index( QTableWidget& t,const QString& options ) :
			m_table( t ),
			m_opts( options )
		{
		}
		int value() const
		{
			return m_entries[ m_index ] ;
		}
		int value( int s ) const
		{
			return m_entries[ static_cast< size_t >( s ) ] ;
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
		QTableWidget& table() const
		{
			return m_table ;
		}
		void add( int s )
		{
			m_entries.emplace_back( s ) ;
		}
		bool empty() const
		{
			return m_entries.empty() ;
		}
		const utility::args& options() const
		{
			return m_opts ;
		}
	private:
		size_t m_index = 0 ;
		std::vector< int > m_entries ;
		QTableWidget& m_table ;
		utility::args m_opts ;
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

			this->uiEnableAll( true ) ;
			m_cancelButton.setEnabled( false ) ;

			finished( { index,false,std::move( exitState ) } ) ;
		}else{
			m_counter++ ;

			if( m_counter == m_index->count() ){

				this->uiEnableAll( true ) ;
				m_cancelButton.setEnabled( false ) ;

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
		m_index.create( std::move( index ) ) ;

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
	template< typename Options,typename Logger >
	void download( const engines::engine& engine,
		       int index,
		       const QString& url,
		       Options opts,
		       Logger logger )
	{
		Q_UNUSED( index )

		m_index->next() ;

		auto u = url ;

		if( !u.isEmpty() ){

			u = utility::split( u,'\n',true ).at( 0 ) ;
		}

		const auto& quality = m_index->options().quality ;

		utility::run( engine,
			      utility::updateOptions( engine,m_settings,quality,{ u } ),
			      quality,
			      std::move( opts ),
			      std::move( logger ),
			      utility::make_term_conn( &m_cancelButton,&QPushButton::clicked ) ) ;
	}
private:
	void uiEnableAll( bool e ) ;
	int m_counter ;
	utility::storage< downloadManager::index > m_index ;
	bool m_cancelled ;
	const Context& m_ctx ;
	QPushButton& m_cancelButton ;
	settings& m_settings ;
} ;

#endif

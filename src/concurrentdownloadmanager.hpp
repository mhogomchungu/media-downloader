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

template< typename Index,
	  typename EnableAll >
class concurrentDownloadManager
{
public:
	concurrentDownloadManager( const Context& ctx,
				   Index index,
				   QLineEdit& lineEdit,
				   QPushButton& cancelButton ) :
		m_index( std::move( index ) ),
		m_enableAll( ctx ),
		m_ctx( ctx ),
		m_lineEdit( lineEdit ),
		m_table( m_index.table() ),
		m_cancelButton( cancelButton )
	{
	}
	void cancelled()
	{
		m_cancelled = true ;
	}
	template< typename Function,typename Finished >
	void monitorForFinished( const engines::engine& engine,int index,Function function,Finished finished )
	{
		Q_UNUSED( index )

		if( m_cancelled ){

			for( const auto& it : m_downloadList ){

				m_table.item( it.index,0 )->setText( it.url ) ;
			}

			this->uiEnableAll( true ) ;
			m_cancelButton.setEnabled( false ) ;

			finished() ;
		}else{
			m_counter++ ;

			if( m_counter == m_index.count() ){

				this->uiEnableAll( true ) ;
				m_cancelButton.setEnabled( false ) ;

				finished() ;
			}else{
				if( m_index.hasNext() ){

					function( engine,m_index.value() ) ;
				}
			}
		}
	}
	template< typename ConcurrentDownload >
	void download( const engines::engine& engine,
		       int maxNumberOfConcurrency,
		       ConcurrentDownload concurrentDownload )
	{
		if( m_table.rowCount() ){

			m_counter = 0 ;
			m_cancelled = false ;
			m_index.reset() ;
			m_downloadList.clear() ;

			this->uiEnableAll( false ) ;
			m_cancelButton.setEnabled( true ) ;
			m_table.setEnabled( true ) ;			

			auto max = [ & ](){

				auto count = m_index.count() ;

				if( maxNumberOfConcurrency < count ){

					return maxNumberOfConcurrency ;
				}else{
					return count ;
				}
			}() ;

			for( int s = 0 ; s < max ; s++ ){

				concurrentDownload( engine,m_index.value( s ) ) ;
			}
		}
	}
	template< typename Options,typename Logger >
	void download( const engines::engine& engine,
		       int index,
		       Options opts,
		       Logger logger )
	{
		m_index++ ;

		auto m = m_lineEdit.text() ;

		utility::args args( m_lineEdit.text() ) ;

		auto url = m_table.item( index,0 )->text() ;

		if( !url.isEmpty() ){

			url = utility::split( url,'\n',true ).at( 0 ) ;
		}

		m_downloadList.emplace_back( index,url ) ;

		utility::run( engine,
			      utility::updateOptions( engine,args,{ url } ),
			      args.quality,
			      std::move( opts ),
			      std::move( logger ),
			      utility::make_term_conn( &m_cancelButton,&QPushButton::clicked ) ) ;
	}
private:
	void uiEnableAll( bool e )
	{
		m_enableAll( e ) ;
	}
	int m_counter ;
	Index m_index ;
	EnableAll m_enableAll ;
	bool m_cancelled ;
	const Context& m_ctx ;
	QLineEdit& m_lineEdit ;
	QTableWidget& m_table ;
	QPushButton& m_cancelButton ;
	struct entry
	{
		entry( int i,const QString& s ) :
			index( i ),url( s )
		{
		}
		int index ;
		QString url ;
	} ;

	std::vector< concurrentDownloadManager::entry > m_downloadList ;
} ;

#endif

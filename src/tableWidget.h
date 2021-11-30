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


#ifndef TABLEWIDGET_H
#define TABLEWIDGET_H

#include <QTableWidget>
#include <QObject>
#include <QLineEdit>

#include "engines.h"

#include <vector>

class tableWidget
{
public:
	struct tableWidgetOptions
	{
		QFlags< QAbstractItemView::EditTrigger > editTrigger = QAbstractItemView::NoEditTriggers ;
		Qt::ContextMenuPolicy customContextPolicy = Qt::CustomContextMenu ;
		Qt::FocusPolicy focusPolicy = Qt::NoFocus ;
		QAbstractItemView::SelectionMode selectionMode = QAbstractItemView::NoSelection ;
		bool mouseTracking = true ;
	};
	void setDownloadingOptions( const QString& s,int row )
	{
		this->item( row ).downloadingOptions = s ;
	}
	void setDownloadingOptionsUi( const QString& s,int row )
	{
		this->item( row ).downloadingOptionsUi = s ;
	}
	void setEngineName( const QString& s,int row )
	{
		this->item( row ).engineName = s ;
	}
	void setUiText( const QString& s,int row )
	{
		this->item( row ).uiText = s ;
		m_table.item( row,1 )->setText( s ) ;
	}
	void setRunningState( const QString& s,int row )
	{
		this->item( row ).runningState = s ;
	}
	const QString& downloadingOptions( int row ) const
	{
		return this->item( row ).downloadingOptions ;
	}
	const QString& downloadingOptionsUi( int row ) const
	{
		return this->item( row ).downloadingOptionsUi ;
	}
	const QString& uiText( int row ) const
	{
		return this->item( row ).uiText ;
	}
	const QString& url( int row ) const
	{
		return this->item( row ).url ;
	}
	const QString& engineName( int row ) const
	{
		return this->item( row ).engineName ;
	}
	const QPixmap& thumbnail( int row ) const
	{
		return this->item( row ).thumbnail.image ;
	}
	const QString& runningState( int row ) const
	{
		return this->item( row ).runningState ;
	}
	int startPosition() const
	{
		return m_init ;
	}
	template< typename ... T >
	void hideColumns( T ... t )
	{
		for( auto it : { t ... } ){

			m_table.hideColumn( it ) ;
		}
	}
	struct entry{
		entry( const QString& uiText,
		       const QString& url,
		       const QString& runningState ) :
			url( url ),
			uiText( uiText ),
			runningState( runningState )
		{
		}
		entry( const QPixmap& thumbnail,
		       const QString& uiText,
		       const QString& url,
		       const QString& runningState ) :
			url( url ),
			uiText( uiText ),
			runningState( runningState ),
			thumbnail( thumbnail )
		{
		}
		QString url ;
		QString uiText ;
		QString runningState ;
		QString downloadingOptions ;
		QString downloadingOptionsUi ;
		QString engineName ;
		struct tnail{
			tnail( const QPixmap& p ) : isSet( true ),image( p )
			{
			}
			tnail()
			{
			}
			bool isSet = false ;
			QPixmap image ;
		} thumbnail ;
		int alignment = Qt::AlignCenter ;
	} ;
	template< typename Function >
	void forEach( Function function )
	{
		for( const auto& it : m_items ){

			function( it ) ;
		}
	}
	enum class type{ DownloadOptions,EngineName } ;

	static void selectRow( QTableWidgetItem * current,QTableWidgetItem * previous,int firstColumnNumber = 0 ) ;
	static void setTableWidget( QTableWidget&,const tableWidget::tableWidgetOptions& ) ;
	static QByteArray thumbnailData( const QPixmap& ) ;
	static QString engineName() ;
	void setDownloadingOptions( tableWidget::type,
				    int row,
				    const QString& options,
				    const QString& title = QString() ) ;
	QString thumbnailData( int row ) const ;
	QString completeProgress( int index ) ;
	int addRow() ;
	int addItem( tableWidget::entry ) ;
	int rowCount() const ;
	int currentRow() const ;
	void replace( tableWidget::entry,int row ) ;
	void clear() ;
	void setVisible( bool ) ;
	void selectLast() ;
	void setEnabled( bool ) ;
	void removeRow( int ) ;
	bool isSelected( int ) ;
	bool noneAreRunning() ;

	tableWidget( QTableWidget& t,const QFont& font,int init ) ;

	QTableWidgetItem& item( int row,int column ) const ;

	QTableWidget& get() ;

	template< typename MemberFunction,typename Callback >
	void connect( MemberFunction m,Callback c )
	{
		QObject::connect( &m_table,m,std::move( c ) ) ;
	}
private:
	tableWidget::entry& item( int s )
	{
		return m_items[ static_cast< size_t >( s ) ] ;
	}
	const tableWidget::entry& item( int s ) const
	{
		return m_items[ static_cast< size_t >( s ) ] ;
	}
	QTableWidget& m_table ;
	int m_init ;

	std::vector< tableWidget::entry > m_items ;
} ;

template< typename Stuff >
class tableMiniWidget
{
public:
	tableMiniWidget( QTableWidget& t,const QFont& ) : m_table( t )
	{
		tableWidget::setTableWidget( m_table,tableWidget::tableWidgetOptions() ) ;
	}
	template< typename MemberFunction,typename Callback >
	void connect( MemberFunction m,Callback c )
	{
		QObject::connect( &m_table,m,std::move( c ) ) ;
	}
	void setTableWidget( const tableWidget::tableWidgetOptions& opts )
	{
		tableWidget::setTableWidget( m_table,opts ) ;
	}
	int rowCount()
	{
		return m_table.rowCount() ;
	}
	void selectRow( QTableWidgetItem * current,QTableWidgetItem * previous,int s )
	{
		tableWidget::selectRow( current,previous,s ) ;
	}
	bool isSelected( int row )
	{
		return m_table.item( row,m_table.columnCount() - 1 )->isSelected() ;
	}
	void setVisible( bool e )
	{
		m_table.setVisible( e ) ;
	}
	QTableWidgetItem& item( int row,int column )
	{
		return *m_table.item( row,column ) ;
	}
	void clear()
	{
		int m = m_table.rowCount() ;

		for( int i = 0 ; i < m ; i++ ){

			m_table.removeRow( 0 ) ;
		}
		m_stuff.clear() ;
	}
	int currentRow()
	{
		return m_table.currentRow() ;
	}
	void removeRow( int s )
	{
		m_table.removeRow( s ) ;
		m_stuff.erase( m_stuff.begin() + s ) ;
	}
	void setEnabled( bool e )
	{
		m_table.setEnabled( e ) ;
	}
	QTableWidget& get()
	{
		return m_table ;
	}
	const Stuff& stuffAt( int s )
	{
		return m_stuff[ static_cast< size_t >( s ) ] ;
	}
	int addRow( Stuff stuff = Stuff() )
	{
		auto row = m_table.rowCount() ;

		m_table.insertRow( row ) ;
		m_stuff.emplace_back( std::move( stuff ) ) ;

		for( int i = 0 ; i < m_table.columnCount() ; i++ ){

			auto item = new QTableWidgetItem() ;
			item->setTextAlignment( Qt::AlignCenter ) ;
			m_table.setItem( row,i,item ) ;
		}

		return row ;
	}
	int add( const QStringList& entries,Stuff stuff = Stuff() )
	{
		if( entries.size() == m_table.columnCount() ){

			int row = this->addRow( std::move( stuff ) ) ;

			for( int col = 0 ; col < entries.size() ; col++ ){

				m_table.item( row,col )->setText( entries[ col ] ) ;
			}

			return row ;
		}

		return -1 ;
	}
	void selectMediaOptions( QStringList& optionsList,QTableWidgetItem& item,QLineEdit& opts )
	{
		if( item.isSelected() ){

			auto text = this->item( item.row(),0 ).text() ;

			if( !optionsList.contains( text ) ){

				optionsList.append( text ) ;
			}
		}

		for( int row = 0 ; row < this->rowCount() ; row++ ){

			auto& item = this->item( row,0 ) ;

			if( !item.isSelected() ){

				optionsList.removeAll( item.text() ) ;
			}
		}

		if( optionsList.isEmpty() ){

			opts.clear() ;
		}else{
			opts.setText( optionsList.join( "+" ) ) ;
		}
	}
	void selectLast()
	{
		if( m_table.rowCount() > 0 ){

			m_table.setCurrentCell( m_table.rowCount() - 1,m_table.columnCount() - 1 ) ;
			m_table.scrollToBottom() ;
		}
	}
private:
	QTableWidget& m_table ;
	std::vector< Stuff > m_stuff ;
};

#endif

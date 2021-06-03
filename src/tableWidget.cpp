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

#include "tableWidget.h"
#include "utility.h"

#include <QHeaderView>

void tableWidget::setTableWidget( const tableWidget::tableWidgetOptions& s )
{
	m_table.verticalHeader()->setSectionResizeMode( QHeaderView::ResizeToContents ) ;

	m_table.verticalHeader()->setMinimumSectionSize( 30 ) ;

	m_table.horizontalHeader()->setStretchLastSection( true ) ;

	m_table.setMouseTracking( s.mouseTracking ) ;

	m_table.setContextMenuPolicy( s.customContextPolicy ) ;

	m_table.setEditTriggers( s.editTrigger ) ;
	m_table.setFocusPolicy( s.focusPolicy ) ;
	m_table.setSelectionMode( s.selectionMode ) ;
}

void tableWidget::selectRow( QTableWidgetItem * current,QTableWidgetItem * previous,int firstColumnNumber )
{
	if( current && previous && previous->row() == current->row() ){

		auto table = current->tableWidget() ;

		table->setCurrentCell( current->row(),table->columnCount() - 1 ) ;
	}else{
		auto _update_table_row = [ & ]( QTableWidgetItem * item,bool setSelected ){

			if( item ){

				auto table = item->tableWidget() ;

				auto row = item->row() ;
				auto col = table->columnCount() ;

				for( int i = firstColumnNumber ; i < col ; i++ ){

					table->item( row,i )->setSelected( setSelected ) ;
				}

				if( setSelected ){

					table->setCurrentCell( row,col - 1 ) ;
				}

				table->setFocus() ;
			}
		} ;

		_update_table_row( current,true ) ;
		_update_table_row( previous,false ) ;
	}
}

void tableWidget::addItem( const QString& text,int alignment )
{
	this->addItem( QStringList{ text },alignment ) ;
}

void tableWidget::clear()
{
	int m = m_table.rowCount() ;

	for( int i = 0 ; i < m ; i++ ){

		m_table.removeRow( 0 ) ;
	}
}

void tableWidget::setVisible( bool e )
{
	m_table.setVisible( e ) ;
}

int tableWidget::rowCount() const
{
	return m_table.rowCount() ;
}

void tableWidget::setEnabled( bool e )
{
	m_table.setEnabled( e ) ;
}

int tableWidget::currentRow() const
{
	return m_table.currentRow() ;
}

void tableWidget::removeRow( int s )
{
	m_table.removeRow( s ) ;
}

void tableWidget::selectMediaOptions( QStringList& optionsList,
				      QTableWidgetItem& item,
				      QLineEdit& opts )
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

void tableWidget::showOptions( const engines::engine& engine,const QList<QByteArray>& args )
{
	QStringList m ;

	utility::make_reverseIterator( args ).forEach( [ & ]( const QByteArray& s ){

		auto a = utility::split( s,' ',true ) ;

		if( a.size() > 1 ){

			if( engine.breakShowListIfContains( a ) ){

				return true ;
			}else{
				m.insert( 0,s ) ;
			}
		}

		return false ;
	} ) ;

	for( const auto& it : m ){

		auto a = utility::split( it,' ',true ) ;

		if( a.size() > 3 ){

			auto format     = a.takeAt( 0 ) ;
			auto extension  = a.takeAt( 0 ) ;
			auto resolution = a.takeAt( 0 ) ;
			auto notes      = a.join( " " ) ;

			QStringList args{ format,extension,resolution,notes } ;

			this->addItem( args ) ;
		}
	}
}

tableWidget::tableWidget( QTableWidget& t,const QFont& font ) : m_table( t ),m_font( font )
{
	this->setTableWidget( tableWidget::tableWidgetOptions() ) ;
}

QTableWidgetItem& tableWidget::item( int row,int column ) const
{
	return *m_table.item( row,column ) ;
}

QTableWidget& tableWidget::get()
{
	return m_table ;
}

void tableWidget::addItem( QTableWidget& table,const QStringList& text,int alignment )
{
	auto row = table.rowCount() ;
	auto columns = table.columnCount() ;

	table.insertRow( row ) ;

	for( int it = 0 ; it < columns ; it++ ){

		auto item = new QTableWidgetItem() ;

		item->setText( text.at( it ) ) ;
		item->setTextAlignment( alignment ) ;
		//item->setFont( m_font ) ;

		table.setItem( row,it,item ) ;
	}
}

void tableWidget::addItem( const QStringList& text,int alignment )
{
	tableWidget::addItem( m_table,text,alignment ) ;
}

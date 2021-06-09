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

	static void addItem( QTableWidget&,const QStringList&,int alignment = Qt::AlignCenter ) ;
	static void selectLast( QTableWidget& ) ;

	void setTableWidget( const tableWidget::tableWidgetOptions& ) ;
	void addItem( const QStringList&,int alignment = Qt::AlignCenter ) ;
	void addItem( const QString&,int alignment = Qt::AlignCenter ) ;
	void selectRow( QTableWidgetItem * current,QTableWidgetItem * previous,int firstColumnNumber = 0 ) ;
	void clear() ;
	void setVisible( bool ) ;
	int rowCount() const ;
	void selectLast() ;
	void setEnabled( bool ) ;
	int currentRow() const ;
	void removeRow( int ) ;
	void selectMediaOptions( QStringList& optionsList,QTableWidgetItem& item,QLineEdit& opts ) ;
	void showOptions( const engines::engine& engine,const QList< QByteArray >& args ) ;

	tableWidget( QTableWidget& t,const QFont& font ) ;

	QTableWidgetItem& item( int row,int column ) const ;

	QTableWidget& get() ;

	template< typename MemberFunction,typename Callback >
	void connect( MemberFunction m,Callback c )
	{
		QObject::connect( &m_table,m,std::move( c ) ) ;
	}
private:
	QTableWidget& m_table ;
	const QFont& m_font ;
} ;

#endif

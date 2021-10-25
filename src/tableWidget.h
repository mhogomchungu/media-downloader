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

	QTableWidgetItem& uiTextItem( int row ) const
	{
		return *m_table.item( row,m_init ) ;
	}
	QTableWidgetItem& urlTextItem( int row ) const
	{
		return *m_table.item( row,m_init + 1 ) ;
	}
	QTableWidgetItem& runningStateItem( int row ) const
	{
		return *m_table.item( row,m_init + 2 ) ;
	}	
	QTableWidgetItem& downloadingOptionsItem( int row ) const
	{
		return *m_table.item( row,m_init + 3 ) ;
	}
	QString downloadingOptions( int row ) const
	{
		return this->downloadingOptionsItem( row ).text() ;
	}
	QString uiText( int row ) const
	{
		return this->uiTextItem( row ).text() ;
	}
	QString url( int row ) const
	{
		return this->urlTextItem( row ).text() ;
	}
	QString runningState( int row ) const
	{
		return this->runningStateItem( row ).text() ;
	}
	enum class type{ DownloadOptions,EngineName } ;

	static QString engineName() ;
	void setDownloadingOptions( tableWidget::type,
				    int row,
				    const QString& options,
				    const QString& title = QString() ) ;
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
	void setTableWidget( const tableWidget::tableWidgetOptions& ) ;
	void replace( const QPixmap&,const QStringList&,int row,int alignment = Qt::AlignCenter ) ;
	int addRow() ;
	int addItem( const QPixmap&,const QStringList&,int alignment = Qt::AlignCenter ) ;
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
	bool noneAreRunning() ;
	void selectMediaOptions( QStringList& optionsList,QTableWidgetItem& item,QLineEdit& opts ) ;
	void showOptions( const engines::engine& engine,const QList< QByteArray >& args ) ;
	QString completeProgress( int index ) ;
	tableWidget( QTableWidget& t,const QFont& font,int init ) ;

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
	int m_init ;
} ;

#endif

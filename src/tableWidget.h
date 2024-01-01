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
#include <QJsonArray>
#include <QSize>
#include <QHeaderView>
#include <QMenu>

#include "engines.h"

#include <vector>
#include <array>

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
	void setCurrentItemChanged( int s )
	{
		auto m = &QTableWidget::currentItemChanged ;

		this->connect( m,[ this,s ]( QTableWidgetItem * c,QTableWidgetItem * p ){

			this->selectRow( c,p,s ) ;
		} ) ;
	}
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
	void setSubTitle( const QString& s,int row )
	{
		this->item( row ).subtitle = s ;
	}
	void setTimeInterval( const QString& s,int row )
	{
		this->item( row ).timeInterval = s ;
	}
	void setChapters( const QString& s,int row )
	{
		this->item( row ).chapters = s ;
	}
	void setSplitByChapters( bool s,int row )
	{
		this->item( row ).splitByChapters = s ;
	}
	void setExtraDownloadOptions( const QString& s,int row )
	{
		this->item( row ).extraDownloadingOptions = s ;
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
	const QString& subTitle( int row ) const
	{
		return this->item( row ).subtitle ;
	}
	const QString& extraDownloadOptions( int row ) const
	{
		return this->item( row ).extraDownloadingOptions ;
	}
	const QString& timeInterval( int row ) const
	{
		return this->item( row ).timeInterval ;
	}
	const QString& chapters( int row ) const
	{
		return this->item( row ).chapters ;
	}
	bool splitByChapters( int row ) const
	{
		return this->item( row ).splitByChapters ;
	}
	const QJsonArray& mediaProperties( int row ) const
	{
		return this->item( row ).mediaProperties ;
	}
	const QPixmap& thumbnail( int row ) const
	{
		return this->item( row ).thumbnail ;
	}
	const QString& runningState( int row ) const
	{
		return this->item( row ).runningState ;
	}
	int startPosition() const
	{
		return m_init ;
	}
	struct entry
	{
		entry()
		{
		}
		template< typename BatchDownloaderItem >
		entry( const BatchDownloaderItem& s ) :
			url( s.url ),
			uiText( s.uiText ),
			downloadingOptions( s.downloadOptions ),
			engineName( s.engineName ),
			extraDownloadingOptions( s.downloadExtraOptions )
		{
		}
		template< typename MediaProperties >
		entry( const QPixmap& thumbnail,
		       const QString& rState,
		       const MediaProperties& media ) :
			url( media.url() ),
			uiText( media.uiText() ),
			runningState( rState ),
			playlist( media.playlist() ),
			playlist_count( media.playlist_count() ),
			playlist_id( media.playlist_id() ),
			playlist_title( media.playlist_title() ),
			playlist_uploader( media.playlist_uploader() ),
			playlist_uploader_id( media.playlist_id() ),
			n_entries( media.n_entries() ),
			mediaProperties( media.formats() ),
			uiJson( media.uiJson() ),
			thumbnail( thumbnail )
		{
		}
		entry move()
		{
			return std::move( *this ) ;
		}
		QString url ;
		QString uiText ;
		QString runningState ;
		QString downloadingOptions ;
		QString downloadingOptionsUi ;
		QString engineName ;
		QString subtitle ;
		QString timeInterval ;
		QString chapters ;
		QString extraDownloadingOptions ;
		QString playlist ;
		QString playlist_count ;
		QString playlist_id ;
		QString playlist_title ;
		QString playlist_uploader ;
		QString playlist_uploader_id ;
		QString n_entries ;
		QJsonArray mediaProperties ;
		QJsonObject uiJson ;
		QPixmap thumbnail ;
		bool splitByChapters = false ;
	} ;
	template< typename Function >
	void forEach( Function function )
	{
		for( const auto& it : m_items ){

			function( it ) ;
		}
	}
	const tableWidget::entry& entryAt( size_t s )
	{
		return m_items[ s ] ;
	}
	enum class type{ DownloadOptions,
			 DownloadExtendedOptions,
			 EngineName,
			 subtitleOption,
			 DownloadTimeInterval,
			 DownloadChapters,
			 SplitByChapters } ;

	static void selectRow( QTableWidgetItem * current,QTableWidgetItem * previous,int firstColumnNumber = 0 ) ;
	static void setTableWidget( QTableWidget&,const tableWidget::tableWidgetOptions& ) ;

	void setDownloadingOptions( tableWidget::type,
				    int row,
				    const QString& options,
				    const QString& title = QString() ) ;
	QString completeProgress( int firstRow ) ;
	int addRow() ;
	class sizeHint
	{
	public:
		sizeHint( int h ) : m_size( 0,h ),m_valid( true )
		{
		}
		sizeHint()
		{
		}
		bool valid() const
		{
			return m_valid ;
		}
		const QSize& value() const
		{
			return m_size ;
		}
	private:
		QSize m_size ;
		bool m_valid = false ;
	};

	int addItem( tableWidget::entry,sizeHint = {} ) ;
	int rowCount() const ;
	int currentRow() const ;
	void replace( tableWidget::entry,int row,sizeHint = {} ) ;
	void clear() ;
	void setVisible( bool ) ;
	void selectLast() ;
	void setEnabled( bool ) ;
	void removeRow( int ) ;
	void hideRow( int ) ;
	bool isSelected( int ) ;
	bool noneAreRunning() ;
	bool rowIsVisible( int ) ;
	bool containsHiddenRows() ;
	bool allFinishedWithSuccess() ;

	tableWidget( QTableWidget& t,const QFont& font,int init,int textAlignment ) ;

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
	int m_textAlignment ;
} ;

template< typename Stuff >
struct tableWidgetRow
{
	tableWidgetRow( const Stuff& s,const QStringList& e ) :
		stuff( s ),entries( e )
	{
	}
	Stuff stuff ;
	QStringList entries ;
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
	template< typename Function >
	void each( Function function )
	{
		QStringList m ;

		for( int row = 0 ; row < m_table.rowCount() ; row++ ){

			m.clear() ;

			for( int column = 0 ; column < m_table.columnCount() ; column++ ){

				m.append( this->item( row,column ).text() ) ;
			}

			function( m,m_stuff[ static_cast< size_t >( row ) ] ) ;
		}
	}
	template< typename T >
	void setUpHeaderMenu( T t )
	{
		auto header = m_table.horizontalHeader() ;

		QObject::connect( header,&QHeaderView::sectionClicked,std::move( t ) ) ;
	}
	void setUpHeaderMenu()
	{
		this->setUpHeaderMenu( [ this ]( int column ){

			this->arrangeTable( column ) ;
		} ) ;
	}
	void setCurrentItemChanged( int s )
	{
		auto m = &QTableWidget::currentItemChanged ;

		this->connect( m,[ this,s ]( QTableWidgetItem * c,QTableWidgetItem * p ){

			this->selectRow( c,p,s ) ;
		} ) ;
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
		if( previous ){

			m_columnClicked = previous->column() ;
		}
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
	void hideAll()
	{
		int m = m_table.rowCount() ;

		for( int i = 0 ; i < m ; i++ ){

			m_table.hideRow( i ) ;
		}
	}
	void showAll()
	{
		int m = m_table.rowCount() ;

		for( int i = 0 ; i < m ; i++ ){

			m_table.showRow( i ) ;
		}
	}
	int currentRow()
	{
		return m_table.currentRow() ;
	}
	int columnCount()
	{
		return m_table.columnCount() ;
	}
	int columnClicked()
	{
		return m_columnClicked ;
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
	void replace( const QStringList& entries,int row,Stuff stuff = Stuff() )
	{
		if( entries.size() == m_table.columnCount() ){

			m_stuff[ row ] = std::move( stuff ) ;

			for( int col = 0 ; col < entries.size() ; col++ ){

				m_table.item( row,col )->setText( entries[ col ] ) ;
			}
		}
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
	void arrangeTable( bool ascending,int column )
	{
		std::vector< tableWidgetRow< Stuff > > rows ;

		this->each( [ & ]( const QStringList& e,const Stuff& stuff ){

			rows.emplace_back( stuff,e ) ;
		} ) ;

		class meaw
		{
		public:
			meaw( bool a,int c ) : m_ascending( a ),m_column( c )
			{
			}
			bool operator()( const tableWidgetRow< Stuff >& s,
					 const tableWidgetRow< Stuff >& e )
			{
				const auto& a = s.entries[ m_column ] ;
				const auto& b = e.entries[ m_column ] ;

				bool aa ;
				bool bb ;

				auto aaa = a.toInt( &aa ) ;
				auto bbb = b.toInt( &bb ) ;

				if( m_ascending ){

					if( aa && bb ){

						return aaa < bbb ;
					}else{
						return a < b ;
					}
				}else{
					if( aa && bb ){

						return aaa > bbb ;
					}else{
						return a > b ;
					}
				}
			}
		private:
			bool m_ascending ;
			bool m_column ;
		} ;

		std::sort( rows.begin(),rows.end(),meaw( ascending,column ) ) ;

		this->clear() ;

		for( const auto& it : rows ){

			this->add( it.entries,it.stuff ) ;
		}
	}
	template< typename Rows >
	void filterTable( Rows& rows,int column,QMenu& m )
	{
		QStringList l ;

		if( column == 1 ){

			for( int row = 0 ; row < m_table.rowCount() ; row++ ){

				auto m = m_table.item( row,column )->text() ;

				if( !l.contains( m ) ){

					l.append( m ) ;
				}

				rows.emplace_back( std::move( m ),row ) ;
			}
		}else{
			std::array< const char *,4 >entries{ {
					"storyboard",
					"video only",
					"audio only",
					"audio video" } } ;

			for( int row = 0 ; row < m_table.rowCount() ; row++ ){

				auto e = m_table.item( row,column )->text() ;

				for( const auto& it : entries ){

					if( e.contains( it ) && !l.contains( it ) ){

						l.append( it ) ;
					}
				}

				rows.emplace_back( std::move( e ),row ) ;
			}
		}

		m.addAction( QObject::tr( "Filter" ) )->setObjectName( "Filter" ) ;

		m.addSeparator() ;

		for( const auto& it : l ){

			auto s = it ;
			s[ 0 ] = s[ 0 ].toUpper() ;

			auto ac = m.addAction( s ) ;

			ac->setObjectName( it ) ;
		}

		m.addSeparator() ;

		m.addAction( QObject::tr( "No Filter" ) )->setObjectName( "No Filter" ) ;

		QObject::connect( &m,&QMenu::triggered,[ & ]( QAction * ac ){

			auto m = ac->objectName() ;

			if( m == "No Filter" ){

				this->showAll() ;

			}else if( m != "Filter" ){

				this->hideAll() ;

				for( const auto& it : rows ){

					if( it.text.contains( m ) ){

						m_table.showRow( it.row ) ;
					}
				}
			}
		} ) ;
	}
	void arrangeTable( int column )
	{
		QMenu m ;

		struct entry
		{
			entry( QString&& t,int r ) :
				text( std::move( t ) ),row( r )
			{
			}
			QString text ;
			int row ;
		} ;

		std::vector< entry > rows ;

		if( column == 0 ){

			auto e = QObject::tr( "Arrange In Ascending Order" ) ;

			QObject::connect( m.addAction( e ),&QAction::triggered,[ this,column ](){

				this->arrangeTable( true,column ) ;
			} ) ;

			e = QObject::tr( "Arrange In Descending Order" ) ;

			QObject::connect( m.addAction( e ),&QAction::triggered,[ this,column ](){

				this->arrangeTable( false,column ) ;
			} ) ;

			m.addSeparator() ;

		}else if( column == 1 || column == 2 ){

			this->filterTable( rows,column,m ) ;
		}else{
			return ;
		}

		m.exec( QCursor::pos() ) ;
	}
	int m_columnClicked = -1 ;
	QTableWidget& m_table ;
	std::vector< Stuff > m_stuff ;
};

#endif

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
	void setRunningState( const QString& s,int row ) ;
	void setStateAsRunning( int row,bool runningRecursively ) ;
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
	void setRunningRecursively( int row,bool s )
	{
		this->item( row ).runningRecursively = s ;
	}
	bool runningRecursively( int row ) const
	{
		return this->item( row ).runningRecursively ;
	}
	bool isRunning( int row ) const ;
	int totalRunningRecursively() const
	{
		int m = 0 ;

		for( int row = 0 ; row < this->rowCount() ; row++ ){

			if( this->isRunning( row ) && this->runningRecursively( row ) ){

				m++ ;
			}
		}

		return m ;
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
	bool finishedWithSuccess( int row ) const ;
	bool runningOrFinishedWithSuccess( int row ) const ;
	int nextAvailableEntryToDownload( int ) const ;
	QByteArray mediaEntry( int row ) const
	{
		auto obj = this->entryAt( row ).uiJson ;
		obj.insert( "formats",this->mediaProperties( row ) ) ;
		return QJsonDocument( obj ).toJson() ;
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
		       const MediaProperties& media,
		       const QString& uu = QString() ) :
			url( media.url() ),
			uiText( uu.isEmpty() ? media.uiText() : uu + "\n" + media.uiText() ),
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
			thumbnail( thumbnail ),
			showFirst( media.showFirst() )
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
		std::vector< QByteArray > fileNames ;
		bool splitByChapters = false ;
		bool banner = false ;
		bool showFirst = false ;
		bool runningRecursively = false ;
		int id = -1 ;
	} ;
	template< typename Function >
	void forEach( Function function )
	{
		for( const auto& it : m_items ){

			function( it ) ;
		}
	}
	int rowWithUrl( const QString& url )
	{
		for( size_t m = 0 ; m < m_items.size() ; m++ ){

			if( m_items[ m ].url == url ){

				return static_cast< int >( m ) ;
			}
		}

		return -1 ;
	}
	const tableWidget::entry& entryAt( size_t s ) const
	{
		return m_items[ s ] ;
	}
	void setConcurrentId( size_t s,int id )
	{
		m_items[ s ].id = id ;
	}
	void setFileNames( size_t m,const std::vector< QByteArray >& s )
	{
		m_items[ m ].fileNames = s ;
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
	int columnCount() const ;
	int currentRow() const ;
	void replace( tableWidget::entry,int row,sizeHint = {} ) ;
	void replace( const QJsonArray&,int row ) ;
	void clear() ;
	void setVisible( bool ) ;
	void selectLast() ;
	void selectRow( int ) ;
	void setEnabled( bool ) ;
	void removeRow( int ) ;
	void removeAllSelected() ;
	void hideRow( int ) ;
	bool isSelected( int ) const ;
	std::vector< int > selectedRows() const ;
	bool noneAreRunning() ;
	int numberCurrentlyRunning() ;
	bool rowIsVisible( int ) ;
	bool isRowHidden( int ) ;
	bool rowIsSelected( int ) ;
	bool containsHiddenRows() ;
	bool allFinishedWithSuccess() ;
	int finishWithSuccess() ;

	tableWidget( QTableWidget& t,const QFont& font,int init,Qt::Alignment textAlignment ) ;

	QTableWidgetItem& item( int row,int column ) const ;

	QTableWidget& get() ;

	void setColumnNumbersTo( int ) ;

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
	Qt::Alignment m_textAlignment ;
} ;

class baseRemoveAllSelected
{
public:
	virtual void operator()( int ) = 0 ;
	virtual ~baseRemoveAllSelected() ;
private:
} ;

void tableMiniWidgetRemoveAllSelected( QTableWidget&,std::unique_ptr< baseRemoveAllSelected > ) ;

template< typename Stuff,size_t COLUMN_COUNT >
class tableMiniWidget
{
public:
	tableMiniWidget( QTableWidget& t,int s,const QFont& ) : m_table( t ),m_startPosition( s )
	{
		tableWidget::setTableWidget( m_table,tableWidget::tableWidgetOptions() ) ;
	}
	template< typename MemberFunction,typename Callback >
	void connect( MemberFunction m,Callback c )
	{
		QObject::connect( &m_table,m,std::move( c ) ) ;
	}
	template< typename MemberFunction,typename Obj,typename ObjMemberFunction >
	void connect( MemberFunction m,Obj obj,ObjMemberFunction objm )
	{
		QObject::connect( &m_table,m,obj,objm ) ;
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
	void removeAllSelected()
	{
		class meaw : public baseRemoveAllSelected
		{
		public:
			meaw( tableMiniWidget< Stuff,COLUMN_COUNT >& m ) : m_parent( m )
			{
			}
			void operator()( int row ) override
			{
				m_parent.removeRow( row ) ;
			}
			~meaw()
			{
			}
		private:
			tableMiniWidget< Stuff,COLUMN_COUNT >& m_parent ;
		} ;

		auto s = std::make_unique< meaw >( *this ) ;

		tableMiniWidgetRemoveAllSelected( m_table,std::move( s ) ) ;
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
	int rowCount() const
	{
		return m_table.rowCount() ;
	}
	int startPosition() const
	{
		return m_startPosition ;
	}
	void selectRow( QTableWidgetItem * current,QTableWidgetItem * previous,int s )
	{
		if( previous ){

			m_columnClicked = previous->column() ;
		}
		tableWidget::selectRow( current,previous,s ) ;
	}
	bool isSelected( int row ) const
	{
		return m_table.item( row,m_table.columnCount() - 1 )->isSelected() ;
	}
	std::vector< int > selectedRows() const
	{
		std::vector<int> s ;

		int m = m_table.rowCount() ;

		for( int i = 0 ; i < m ; i++ ){

			if( this->isSelected( i ) ){

				s.emplace_back( i ) ;
			}
		}

		return s ;
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
	void setLastRow()
	{
		if( m_table.rowCount() ){

			m_table.setCurrentCell( m_table.rowCount() - 1,m_table.columnCount() - 1 ) ;
		}
	}
	QTableWidget& get()
	{
		return m_table ;
	}
	const Stuff& stuffAt( int s ) const
	{
		return m_stuff[ static_cast< size_t >( s ) ] ;
	}
	const Stuff& stuffAtLast() const
	{
		return m_stuff.back() ;
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
	template< typename ... Args >
	int add( Stuff stuff,const QString& s,Args&& ... args )
	{
		static_assert( sizeof...( args ) + 1 == COLUMN_COUNT,"Error1" ) ;

		int row = this->addRow( std::move( stuff ) ) ;

		this->updateRow( row,s,std::forward< Args >( args ) ... ) ;

		return row ;
	}
	template< typename ... Args >
	int add( const QString& s,Args&& ... args )
	{
		static_assert( sizeof...( args ) + 1 == COLUMN_COUNT,"Error2" ) ;

		int row = this->addRow( Stuff() ) ;

		this->updateRow( row,s,std::forward< Args >( args ) ... ) ;

		return row ;
	}
	int add( Stuff stuff,const engines::engine::baseEngine::mediaInfo& m )
	{
		int row = this->addRow( std::move( stuff ) ) ;

		const auto& a = m.id() ;
		const auto& b = m.ext() ;
		const auto& c = m.resolution() ;
		const auto& d = m.fileSize() ;
		const auto& e = m.info() ;

		this->updateRow( row,a,b,c,d,e ) ;

		return row ;
	}
	int add( engines::engine::baseEngine::mediaInfo mm )
	{
		int row = this->addRow( std::move( mm ) ) ;

		const auto& m = this->stuffAtLast() ;

		const auto& a = m.id() ;
		const auto& b = m.ext() ;
		const auto& c = m.resolution() ;
		const auto& d = m.fileSize() ;
		const auto& e = m.info() ;

		this->updateRow( row,a,b,c,d,e ) ;

		return row ;
	}
	template< typename ... Args >
	void replace( int row,Stuff stuff,const QString& s,Args&& ... args )
	{
		static_assert( sizeof...( args ) + 1 == COLUMN_COUNT,"Error3" ) ;

		m_stuff[ row ] = std::move( stuff ) ;

		this->updateRow( row,s,std::forward< Args >( args ) ... ) ;
	}
	template< typename ... Args >
	void replace( int row,const QString& s,Args&& ... args )
	{
		static_assert( sizeof...( args ) + 1 == COLUMN_COUNT,"Error4" ) ;

		m_stuff[ row ] = Stuff() ;

		this->updateRow( row,s,std::forward< Args >( args ) ... ) ;
	}
	void selectMediaOptions( QStringList& optionsList,QLineEdit& opts )
	{
		for( int row = 0 ; row < this->rowCount() ; row++ ){

			auto& item = this->item( row,0 ) ;

			if( item.isSelected() ){

				auto text = item.text() ;

				if( !optionsList.contains( text ) ){

					optionsList.append( text ) ;
				}
			}else{
				optionsList.removeAll( item.text() ) ;
			}
		}

		if( optionsList.isEmpty() ){

			opts.clear() ;
		}else{
			opts.setText( optionsList.join( "+" ) ) ;
		}
	}
	void selectRow( int row )
	{
		if( m_table.rowCount() > 0 ){

			m_table.setCurrentCell( row,m_table.columnCount() - 1 ) ;
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
	void updateRow( int,int )
	{
	}
	template< typename ... Args >
	void updateRow( int row,int col,const QString& s,Args&& ... args )
	{
		m_table.item( row,col )->setText( s ) ;

		this->updateRow( row,++col,std::forward< Args >( args ) ... ) ;
	}
	template< typename ... Args >
	void updateRow( int row,const QString& s,Args&& ... args )
	{
		static_assert( sizeof...( args ) + 1 == COLUMN_COUNT,"Error5" ) ;

		this->updateRow( row,0,s,std::forward< Args >( args ) ... ) ;
	}
	template< typename Function >
	void fromStuff( const QJsonObject& e,const Function& function )
	{
		engines::engine::baseEngine::mediaInfo::fromQJobject( e,function ) ;
	}
	template< typename Function >
	void fromStuff( const engines::engine::baseEngine::mediaInfo& e,const Function& function )
	{
		function( e.id(),e.ext(),e.resolution(),e.fileSize(),e.info() ) ;
	}
	class Forwader
	{
	public:
		template< typename Parent >
		Forwader( int row,Parent& e ) : m_row( row ),m_parent( e )
		{
		}
		template< typename ... Args >
		void operator()( Args&& ... args ) const
		{
			m_parent.updateRow( m_row,std::forward< Args >( args ) ... ) ;
		}
	private:
		int m_row ;
		tableMiniWidget< Stuff,COLUMN_COUNT >& m_parent ;
	} ;
	void arrangeTable( bool ascending,int column )
	{
		class meaw
		{
		public:
			meaw( bool a,int c ) : m_ascending( a ),m_column( c )
			{
			}
			QString getSize( const QJsonObject& e )
			{
				return engines::engine::baseEngine::mediaInfo::fileSizeRaw( e ) ;
			}
			const QString& getSize( const engines::engine::baseEngine::mediaInfo& e )
			{
				return e.fileSizeRaw() ;
			}
			QString getId( const QJsonObject& e )
			{
				return engines::engine::baseEngine::mediaInfo::id( e ) ;
			}
			const QString& getId( const engines::engine::baseEngine::mediaInfo& e )
			{
				return e.id() ;
			}
			bool compare( const QString& a,const QString& b )
			{
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
			bool operator()( const Stuff& s,const Stuff& e )
			{
				if( m_column == 0 ){

					const auto& a = this->getId( s ) ;
					const auto& b = this->getId( e ) ;

					return this->compare( a,b ) ;
				}else{
					const auto& a = this->getSize( s ) ;
					const auto& b = this->getSize( e ) ;

					return this->compare( a,b ) ;
				}
			}
		private:
			bool m_ascending ;
			bool m_column ;
		} ;

		auto stuff = std::move( m_stuff ) ;

		std::sort( stuff.begin(),stuff.end(),meaw( ascending,column ) ) ;

		this->clear() ;

		for( auto& it : stuff ){

			int row = this->addRow( std::move( it ) ) ;

			this->fromStuff( this->stuffAtLast(),Forwader( row,*this ) ) ;
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

		if( column == 0 || column == 3 ){

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
	int m_startPosition ;
	std::vector< Stuff > m_stuff ;
};

#endif

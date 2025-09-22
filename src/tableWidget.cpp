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
#include "reportFinished.h"

#include <QHeaderView>
#include <QBuffer>

void tableWidget::setDownloadingOptions( tableWidget::type type,
					 int row,
					 const QString& mm,
					 const QString& title )
{
	auto optionName = [ & ](){

		if( type == tableWidget::type::DownloadOptions ){

			return utility::stringConstants::downloadOptions() + ": " ;

		}else if( type == tableWidget::type::EngineName ){

			return utility::stringConstants::engineName() ;

		}else if( type == tableWidget::type::subtitleOption ){

			return utility::stringConstants::subtitle() + ": " ;

		}else if( type == tableWidget::type::DownloadExtendedOptions ){

			return utility::stringConstants::downloadExtendedOptions() + ": " ;

		}else if( type == tableWidget::type::DownloadTimeInterval ){

			return utility::stringConstants::downloadTimeInterval() + ": " ;

		}else if( type == tableWidget::type::DownloadChapters ){

			return utility::stringConstants::downloadChapters() + ": " ;
		}else{
			return utility::stringConstants::splitByChapters() + ": " ;
		}
	}() ;

	const auto txt = this->uiText( row ) ;

	auto m = [ & ](){

		if( title.isEmpty() ){

			return mm ;
		}else{
			return title ;
		}
	}() ;

	if( txt.contains( optionName ) ){

		auto mm = util::split( txt,'\n',true ) ;

		for( auto& it : mm ){

			if( it.startsWith( optionName ) ){

				it = optionName + m ;

				break ;
			}
		}

		this->setUiText( mm.join( '\n' ),row ) ;
	}else{
		this->setUiText( optionName + m + "\n" + txt,row ) ;
	}

	auto optText = this->downloadingOptionsUi( row ) ;

	if( optText.contains( optionName ) ){

		auto mm = util::split( optText,'\n',true ) ;

		for( auto& it : mm ){

			if( it.startsWith( optionName ) ){

				it = optionName + m ;

				break ;
			}
		}

		this->setDownloadingOptionsUi( mm.join( '\n' ),row ) ;
	}else{
		if( optText.isEmpty() ){

			this->setDownloadingOptionsUi( optionName + m,row ) ;
		}else{
			this->setDownloadingOptionsUi( optionName + m + "\n" + optText,row ) ;
		}
	}

	if( type == tableWidget::type::DownloadOptions ){

		this->setDownloadingOptions( mm,row ) ;

	}else if( type == tableWidget::type::EngineName ){

		this->setEngineName( mm,row ) ;

	}else if( type == tableWidget::type::subtitleOption ){

		this->setSubTitle( mm,row ) ;

	}else if( type == tableWidget::type::DownloadTimeInterval ){

		this->setTimeInterval( mm,row ) ;

	}else if( type == tableWidget::type::DownloadExtendedOptions ){

		this->setExtraDownloadOptions( mm,row ) ;

	}else if( type == tableWidget::type::DownloadChapters ){

		this->setChapters( mm,row ) ;

	}else if( type == tableWidget::type::SplitByChapters ){

		this->setSplitByChapters( mm == "Yes",row ) ;
	}
}

void tableWidget::setTableWidget( QTableWidget& table,const tableWidget::tableWidgetOptions& s )
{
	table.verticalHeader()->setSectionResizeMode( QHeaderView::ResizeToContents ) ;

	//table.verticalHeader()->setMinimumSectionSize( 30 ) ;

	table.horizontalHeader()->setStretchLastSection( true ) ;

	table.setMouseTracking( s.mouseTracking ) ;

	table.setContextMenuPolicy( s.customContextPolicy ) ;

	table.setEditTriggers( s.editTrigger ) ;
	table.setFocusPolicy( s.focusPolicy ) ;
	table.setSelectionMode( s.selectionMode ) ;
}

void tableWidget::replace( tableWidget::entry e,int r,sizeHint s )
{
	auto row = static_cast< size_t >( r ) ;

	m_items[ row ] = e.move() ;

	auto label = new QLabel() ;
	label->setAlignment( Qt::AlignCenter ) ;
	label ->setPixmap( m_items[ row ].thumbnail ) ;

	m_table.setCellWidget( r,0,label ) ;

	auto item = m_table.item( r,1 ) ;

	if( s.valid() ){

		item->setSizeHint( s.value() ) ;
	}

	item->setText( m_items[ row ].uiText ) ;
}

void tableWidget::replace( const QJsonArray& array,int row )
{
	m_items[ static_cast< size_t >( row ) ].mediaProperties = array ;
}

int tableWidget::addRow()
{
	auto row = m_table.rowCount() ;

	m_table.insertRow( row ) ;

	for( int i = 0 ; i < m_table.columnCount() ; i++ ){

		auto item = new QTableWidgetItem() ;
		item->setTextAlignment( Qt::AlignCenter ) ;
		m_table.setItem( row,i,item ) ;
	}

	return row ;
}

int tableWidget::addItem( tableWidget::entry e,tableWidget::sizeHint s )
{
	auto row = m_table.rowCount() ;

	if( e.showFirst && row ){

		auto m = m_items.begin() ;

		if( m->banner ){

			row = 1 ;

			m_items.insert( m + 1,e.move() ) ;
		}else{
			row = 0 ;

			m_items.insert( m,e.move() ) ;
		}
	}else{
		m_items.emplace_back( e.move() ) ;
	}

	m_table.insertRow( row ) ;

	const auto& entry = m_items[ row ] ;

	auto label = new QLabel() ;
	label->setAlignment( Qt::AlignHCenter | Qt::AlignVCenter ) ;
	label->setPixmap( entry.thumbnail ) ;

	m_table.setCellWidget( row,0,label ) ;

	auto item = new QTableWidgetItem( entry.uiText ) ;

	item->setTextAlignment( m_textAlignment ) ;

	if( s.valid() ){

		item->setSizeHint( s.value() ) ;
	}

	m_table.setItem( row,1,item ) ;

	return row ;
}

void tableWidget::setStateAsRunning( int row,bool runningRecursively )
{
	auto& m              = this->item( row ) ;
	m.runningState       = reportFinished::finishedStatus::running() ;
	m.runningRecursively = runningRecursively ;
}

void tableWidget::setRunningState( const QString& s,int row )
{
	auto& m              = this->item( row ) ;
	m.runningState       = s ;
	m.runningRecursively = false ;
}

bool tableWidget::isRunning( int row ) const
{
	return reportFinished::finishedStatus::isRunning( *this,row  ) ;
}

bool tableWidget::finishedWithSuccess( int row ) const
{
	return reportFinished::finishedStatus::finishedWithSuccess( *this,row ) ;
}

bool tableWidget::runningOrFinishedWithSuccess( int row ) const
{
	return this->isRunning( row ) || this->finishedWithSuccess( row ) ;
}

int tableWidget::nextAvailableEntryToDownload( int row ) const
{
	for( ; row < m_table.rowCount() ; row++ ){

		if( !this->runningOrFinishedWithSuccess( row ) ){

			return row ;
		}
	}

	return -1 ;
}

void tableWidget::selectRow( QTableWidgetItem * current,QTableWidgetItem * previous,int firstColumnNumber )
{
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

	if( current && previous ){

		if( previous->row() == current->row() ){

			auto table = current->tableWidget() ;

			table->setCurrentCell( current->row(),table->columnCount() - 1 ) ;
		}else{
			_update_table_row( current,true ) ;

			if( QGuiApplication::keyboardModifiers() != Qt::ControlModifier ){

				auto currentRow = current->row() ;

				auto table = current->tableWidget() ;

				auto lastColumn = table->columnCount() - 1 ;

				for( int row = 0 ; row < table->rowCount() ; row++ ){

					if( row != currentRow ){

						auto item = table->item( row,lastColumn ) ;

						_update_table_row( item,false ) ;
					}
				}
			}
		}

	}else if( current && !previous ){

		auto table = current->tableWidget() ;

		table->setCurrentCell( current->row(),table->columnCount() - 1 ) ;

		_update_table_row( current,true ) ;

	}else if( !current && previous ){

	}else{

	}
}

void tableWidget::clear()
{
	int m = m_table.rowCount() ;

	for( int i = 0 ; i < m ; i++ ){

		m_table.removeRow( 0 ) ;
	}

	m_items.clear() ;
}

void tableWidget::setVisible( bool e )
{
	m_table.setVisible( e ) ;
}

int tableWidget::rowCount() const
{
	return m_table.rowCount() ;
}

int tableWidget::columnCount() const
{
	return m_table.columnCount() ;
}

void tableWidget::selectLast()
{
	if( m_table.rowCount() > 0 ){

		m_table.setCurrentCell( m_table.rowCount() - 1,m_table.columnCount() - 1 ) ;
		m_table.scrollToBottom() ;
	}
}

void tableWidget::selectRow( int row )
{
	m_table.setCurrentCell( row,m_table.columnCount() - 1 ) ;
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
	m_items.erase( m_items.begin() + s ) ;
}

void tableWidget::removeAllSelected()
{
	utility::removeAllSelected( m_table,[ this ]( int row ){

		this->removeRow( row ) ;
	} ) ;
}

void tableWidget::hideRow( int row )
{
	m_table.hideRow( row ) ;

	for( int i = row + 1 ; i < m_table.rowCount() ; i++ ){

		if( this->rowIsVisible( i ) ){

			auto a = m_table.item( i,1 ) ;
			auto b = m_table.item( row,1 ) ;

			this->selectRow( a,b,1 ) ;

			return ;
		}
	}

	for( int i = row - 1 ; i >= 0 ; i-- ){

		if( this->rowIsVisible( i ) ){

			auto a = m_table.item( i,1 ) ;
			auto b = m_table.item( row,1 ) ;

			this->selectRow( a,b,1 ) ;

			return ;
		}
	}
}

bool tableWidget::isSelected( int row ) const
{
	return m_table.item( row,m_init )->isSelected() ;
}

std::vector< int > tableWidget::selectedRows() const
{
	std::vector< int > s ;

	int m = m_table.rowCount() ;

	for( int i = 0 ; i < m ; i++ ){

		if( this->isSelected( i ) ){

			s.emplace_back( i ) ;
		}
	}

	return s ;
}

int tableWidget::numberCurrentlyRunning()
{
	int m = 0 ;

	for( int i = 0 ; i < m_table.rowCount() ; i++ ){

		if( reportFinished::finishedStatus::running( this->runningState( i ) ) ){

			m++ ;
		}
	}

	return m ;
}

bool tableWidget::noneAreRunning()
{
	for( int i = 0 ; i < m_table.rowCount() ; i++ ){

		if( reportFinished::finishedStatus::running( this->runningState( i ) ) ){

			return false ;
		}
	}

	return true ;
}

bool tableWidget::rowIsVisible( int row )
{
	return !m_table.isRowHidden( row ) ;
}

bool tableWidget::isRowHidden( int row )
{
	return m_table.isRowHidden( row ) ;
}

bool tableWidget::rowIsSelected( int row )
{
	return m_table.item( row,m_table.columnCount() - 1 )->isSelected() ;
}

bool tableWidget::containsHiddenRows()
{
	for( int row = 0 ; row < m_table.rowCount() ; row++ ){

		if( m_table.isRowHidden( row ) ){

			return true ;
		}
	}

	return false ;
}

bool tableWidget::allFinishedWithSuccess()
{
	for( int i = 0 ; i < m_table.rowCount() ; i++ ){

		if( !reportFinished::finishedStatus::finishedWithSuccess( this->runningState( i ) ) ){

			return false ;
		}
	}

	return true ;
}

int tableWidget::finishWithSuccess()
{
	int m = 0 ;

	for( int i = 0 ; i < m_table.rowCount() ; i++ ){

		if( reportFinished::finishedStatus::finishedWithSuccess( this->runningState( i ) ) ){

			m++ ;
		}
	}

	return m ;
}

QString tableWidget::completeProgress( int firstRow )
{
	int running = 0 ;
	int completed = 0 ;
	int errored = 0 ;
	int cancelled = 0 ;
	int notStarted = 0 ;
	int rowCount = 0 ;

	for( int i = firstRow ; i < m_table.rowCount() ; i++ ){

		rowCount++ ;

		const auto& s = this->runningState( i ) ;

		if( reportFinished::finishedStatus::running( s ) ){

			running++ ;
		}
		if( reportFinished::finishedStatus::notStarted( s ) ){

			notStarted++ ;
		}
		if( reportFinished::finishedStatus::finishedWithSuccess( s ) ){

			completed++ ;
		}
		if( reportFinished::finishedStatus::finishedWithError( s ) ){

			errored++ ;
		}
		if( reportFinished::finishedStatus::finishedCancelled( s ) ){

			cancelled++ ;
		}
	}

	auto z = completed + errored + cancelled ;

	auto m = QString::number( z ) + "/" + QString::number( rowCount ) ;

	auto a = m + "(" + QString::number( z * 100 / rowCount ) + "%)" ;
	auto b = QString::number( running ) ;
	auto c = QString::number( notStarted ) ;
	auto d = QString::number( completed ) ;
	auto e = QString::number( errored ) ;
	auto f = QString::number( cancelled ) ;

	if( a.startsWith( "100" ) ){

		a = "100" ;
	}

	return QObject::tr( "Completed: %1, Running: %2, Not Started: %3, Succeeded: %4, Failed: %5, Cancelled: %6" ).arg( a,b,c,d,e,f ) ;
}

tableWidget::tableWidget( QTableWidget& t,const QFont&,int init,Qt::Alignment tA ) :
	m_table( t ),
	m_init( init ),
	m_textAlignment( tA )
{
	this->setTableWidget( m_table,tableWidget::tableWidgetOptions() ) ;
}

QTableWidgetItem& tableWidget::item( int row,int column ) const
{
	return *m_table.item( row,column ) ;
}

QTableWidget& tableWidget::get()
{
	return m_table ;
}

void tableWidget::setColumnNumbersTo( int m )
{
	while( m_table.columnCount() ){

		m_table.removeColumn( 0 ) ;
	}

	for( int s = 0 ; s < m ; s++ ){

		m_table.insertColumn( 0 ) ;
	}
}

baseRemoveAllSelected::~baseRemoveAllSelected()
{
}

void tableMiniWidgetRemoveAllSelected( QTableWidget& table,
				       std::unique_ptr< baseRemoveAllSelected > function )
{
	utility::removeAllSelected( table,[ function = std::move( function ) ]( int row ){

		( *function )( row ) ;
	} ) ;
}

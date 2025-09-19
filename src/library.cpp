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

#include "library.h"
#include "basicdownloader.h"
#include "tabmanager.h"
#include "tableWidget.h"
#include "mainwindow.h"

#include "utils/miscellaneous.hpp"

#include <QDir>

library::library( const Context& ctx ) :
	m_ctx( ctx ),
	m_settings( m_ctx.Settings() ),
	m_ui( m_ctx.Ui() ),
	m_table( *m_ui.tableWidgetLibrary,0,m_ctx.mainWidget().font() ),
	m_downloadFolder( QDir::fromNativeSeparators( m_settings.downloadFolder() ) ),
	m_currentPath( m_downloadFolder ),
	m_folderIcon( m_settings.getIcon( "folder" ).pixmap( 30,40 ) ),
	m_videoIcon( m_settings.getIcon( "video" ).pixmap( 30,40 ) )
{
	qRegisterMetaType< directoryEntries::iter >() ;

	this->setRenameUiVisible( false ) ;

	connect( m_ui.pbLibraryCancel,&QPushButton::clicked,[ this ](){

		m_continue = false ;
	} ) ;

	connect( m_ui.pbLibraryCancelRename,&QPushButton::clicked,[ this ](){

		this->setRenameUiVisible( false ) ;
	} ) ;

	connect( m_ui.pbLibrarySetNewFileName,&QPushButton::clicked,[ this ](){

		this->setRenameUiVisible( false ) ;

		auto m = m_ui.pbLibrarySetNewFileName->objectName() ;

		if( m == "Rename" ){

			this->renameFile( m_table.currentRow() ) ;

		}else if( m == "Delete" ){

			this->deleteEntry( m_table.currentRow() ) ;

		}else if( m == "DeleteAll" ){

			this->deleteAll() ;

		}else if( m == "DeleteSelectedItems" ){

			this->disableAll() ;

			m_ui.pbLibraryCancel->setEnabled( true ) ;

			this->deleteEntries( m_table.selectedRows() ) ;
		}
	} ) ;

	connect( this,&library::addEntrySignal,this,&library::addEntrySlot,Qt::QueuedConnection ) ;

	m_table.setUpHeaderMenu( [ this ]( int column ){

		this->arrangeEntries( column ) ;
	} ) ;

	connect( m_ui.cbLibraryTabEnable,&QCheckBox::clicked,[ this ]( bool e ){

		if( e ){

			this->enableAll() ;
			this->showContents( m_currentPath ) ;
		}else{
			m_table.clear() ;
			this->disableAll() ;
			m_ui.pbLibraryQuit->setEnabled( true ) ;
			m_ui.pbLibraryDowloadFolder->setEnabled( true ) ;
		}
	} ) ;

	auto cc = &QTableWidget::currentItemChanged ;

	m_table.connect( cc,[ this ]( QTableWidgetItem * c,QTableWidgetItem * p ){

		m_table.selectRow( c,p,1 ) ;
	} ) ;

	m_table.connect( &QTableWidget::customContextMenuRequested,this,&library::cxMenuRequested ) ;

	connect( m_ui.pbLibraryQuit,&QPushButton::clicked,[ this ](){

		m_ctx.mainWindow().quitApp() ;
	} ) ;

	connect( m_ui.pbLibraryDowloadFolder,&QPushButton::clicked,[ this ](){

		utility::openDownloadFolderPath( m_currentPath ) ;
	} ) ;

	connect( m_ui.pbLibraryHome,&QPushButton::clicked,[ this ](){

		auto m = m_settings.downloadFolder() ;

		m_downloadFolder = QDir::fromNativeSeparators( m ) ;

		if( m_downloadFolder != m_currentPath ){

			m_currentPath = m_downloadFolder ;

			this->showContents( m_currentPath ) ;
		}
	} ) ;

	connect( m_ui.pbLibraryUp,&QPushButton::clicked,[ this ](){

		this->moveUp() ;
	} ) ;

	connect( m_ui.pbLibraryRefresh,&QPushButton::clicked,[ this ](){

		this->showContents( m_currentPath ) ;
	} ) ;

	m_table.connect( &QTableWidget::cellDoubleClicked,[ this ]( int row,int column ){

		Q_UNUSED( column )

		auto s = m_table.item( row,1 ).text() ;

		if( m_table.stuffAt( row ) == directoryEntries::ICON::FOLDER ){

			m_currentPath +=  "/" + s ;

			this->showContents( m_currentPath ) ;
		}else{
			m_ctx.Engines().openUrls( m_currentPath + "/" + s ) ;
		}
	} ) ;
}

void library::moveUp()
{
	if( m_currentPath != m_downloadFolder ){

		auto m = m_currentPath.lastIndexOf( '/' ) ;

		if( m != -1 ){

			m_currentPath.truncate( m ) ;
		}

		this->showContents( m_currentPath ) ;
	}
}

void library::init_done()
{
	if( m_settings.enableLibraryTab() ){

		this->enableAll() ;
	}else{
		this->disableAll() ;
		m_ui.pbLibraryQuit->setEnabled( true ) ;
		m_ui.pbLibraryDowloadFolder->setEnabled( true ) ;
	}
}

void library::resetMenu()
{
}

void library::exiting()
{
}

void library::retranslateUi()
{
}

void library::tabEntered()
{
	if( m_settings.enableLibraryTab() && m_table.rowCount() == 0 ){

		this->showContents( m_currentPath ) ;
	}
}

void library::tabExited()
{
	m_continue = false ;
}

void library::textAlignmentChanged( Qt::LayoutDirection )
{
}

bool library::hasMultipleSelections()
{
	int multipleSelections = 0 ;

	for( int row = 0 ; row < m_table.rowCount() ; row++ ){

		if( m_table.isSelected( row ) ){

			multipleSelections++ ;
		}
	}

	return multipleSelections > 1 ;
}

bool library::deletePath( const QString& m )
{
	QFileInfo mm( m ) ;

	if( mm.isSymLink() ){

		QFile::remove( m ) ;

	}else if( mm.isDir() ){

		directoryManager::removeDirectory( m,m_continue ) ;
	}else{
		QFile::remove( m ) ;
	}

	mm.refresh() ;

	return mm.exists() ;
}

void library::deleteEntries( std::vector< int > items )
{
	if( items.size() == 0 ){

		return this->enableAll() ;
	}

	int row = items.back() ;

	items.pop_back() ;

	class meaw
	{
	public:
		meaw( library& library,std::vector< int > items,int row ) :
			m_parent( library ),
			m_items( std::move( items ) ),
			m_row( row ),
			m_path( this->path() )
		{
		}
		bool bg()
		{
			return m_parent.deletePath( m_path ) ;
		}
		void fg( bool s )
		{
			if( !s ){

				m_parent.m_table.removeRow( m_row ) ;
			}

			m_parent.deleteEntries( std::move( m_items ) ) ;
		}
	private:
		QString path() const
		{
			auto s = m_parent.m_table.item( m_row,1 ).text() ;
			return m_parent.m_currentPath + "/" + s ;
		}
		library& m_parent ;
		std::vector< int > m_items ;
		int m_row ;
		QString m_path ;
	} ;

	utils::qthread::run( meaw( *this,std::move( items ),row ) ) ;
}

void library::setRenameUiVisible( bool e )
{
	m_ui.labelLibrarySetNewFileName->setVisible( e ) ;
	m_ui.pbLibraryCancelRename->setVisible( e ) ;
	m_ui.pbLibrarySetNewFileName->setVisible( e ) ;
	m_ui.plainTextLibrarySetNewName->setVisible( e ) ;
	m_ui.labelLibraryWidgetOverMainTable->setVisible( e ) ;
}

void library::renameFile( int row )
{
	auto nn = m_ui.plainTextLibrarySetNewName->toPlainText() ;

	auto& item = m_table.item( row,1 ) ;

	utility::rename( m_ctx,item,m_currentPath,nn,item.text() ) ;
}

void library::keyPressed( utility::mainWindowKeyCombo m )
{
	if( m == utility::mainWindowKeyCombo::CTRL_D ){

		auto a = tr( "Are You Sure You Want To Delete Selected Items?" ) ;

		m_ui.labelLibrarySetNewFileName->setText( a ) ;

		m_ui.pbLibrarySetNewFileName->setObjectName( "DeleteSelectedItems" ) ;

		m_ui.pbLibrarySetNewFileName->setText( tr( "Yes" ) ) ;

		m_ui.pbLibraryCancelRename->setText( tr( "No" ) ) ;

		this->setRenameUiVisible( true ) ;

		m_ui.plainTextLibrarySetNewName->setVisible( false ) ;
	}else{
		utility::keyPressed( m_table,m ) ;
	}
}

void library::deleteEntry( int row )
{
	if( m_table.isSelected( row ) ){

		this->disableAll() ;

		m_ui.pbLibraryCancel->setEnabled( true ) ;

		std::vector< int > item ;

		item.emplace_back( row ) ;

		this->deleteEntries( std::move( item ) ) ;
	}
}

void library::deleteAll()
{
	this->disableAll() ;

	m_ui.pbLibraryCancel->setEnabled( true ) ;

	class meaw
	{
	public:
		meaw( library& library ) : m_parent( library )
		{
		}
		void bg()
		{
			const auto& a = m_parent.m_currentPath ;
			auto& b = m_parent.m_continue ;

			directoryManager::removeDirectoryContents( a,b ) ;
		}
		void fg()
		{
			m_parent.showContents( m_parent.m_currentPath ) ;
		}
	private:
		library& m_parent ;
	} ;

	utils::qthread::run( meaw( *this ) ) ;
}

void library::enableAll()
{
	m_table.setEnabled( true ) ;
	m_ui.pbLibraryQuit->setEnabled( true ) ;
	m_ui.pbLibraryCancel->setEnabled( true ) ;
	m_ui.pbLibraryHome->setEnabled( true ) ;
	m_ui.pbLibraryDowloadFolder->setEnabled( true ) ;
	m_ui.pbLibraryRefresh->setEnabled( true ) ;
	m_ui.pbLibraryUp->setEnabled( true ) ;
}

void library::disableAll()
{
	m_table.setEnabled( false ) ;
	m_ui.pbLibraryQuit->setEnabled( false ) ;
	m_ui.pbLibraryCancel->setEnabled( false ) ;
	m_ui.pbLibraryHome->setEnabled( false ) ;
	m_ui.pbLibraryDowloadFolder->setEnabled( false ) ;
	m_ui.pbLibraryRefresh->setEnabled( false ) ;
	m_ui.pbLibraryUp->setEnabled( false ) ;
}

void library::addItem( const directoryEntries::iter& s )
{
	auto icon = s.icon() ;

	auto row = m_table.addRow( icon ) ;

	auto label = new QLabel() ;

	if( icon == directoryEntries::ICON::FILE ){

		label->setPixmap( m_videoIcon ) ;
	}else{
		label->setPixmap( m_folderIcon ) ;
	}

	label->setAlignment( Qt::AlignCenter ) ;

	m_table.get().setCellWidget( row,0,label ) ;

	auto& item = m_table.item( row,1 ) ;

	item.setText( s.value() ) ;
	item.setTextAlignment( Qt::AlignCenter ) ;
	item.setFont( m_ctx.mainWidget().font() ) ;
}

void library::addEntrySlot( const directoryEntries::iter& s )
{
	if( s.hasNext() && m_continue ){

		this->addItem( s ) ;

		m_table.setLastRow() ;

		emit this->addEntrySignal( s.next() ) ;
	}else{
		m_table.setLastRow() ;

		this->enableAll() ;
	}
}

void library::cxMenuRequested( QPoint )
{
	auto row = m_table.currentRow() ;

	if( row == -1 ){

		return ;
	}

	QMenu m ;

	auto hasMultipleSelections = this->hasMultipleSelections() ;

	connect( m.addAction( tr( "Delete" ) ),&QAction::triggered,[ this,row,hasMultipleSelections ](){

		if( hasMultipleSelections ){

			m_ui.pbLibrarySetNewFileName->setObjectName( "DeleteSelectedItems" ) ;

			auto a = tr( "Are You Sure You Want To Delete Selected Items?" ) ;

			m_ui.labelLibrarySetNewFileName->setText( a ) ;

			this->setRenameUiVisible( true ) ;

			m_ui.plainTextLibrarySetNewName->setVisible( false ) ;
		}else{
			if( m_table.stuffAt( row ) == directoryEntries::ICON::FILE ){

				auto a = tr( "Are You Sure You Want To Delete Below File?" ) ;

				m_ui.labelLibrarySetNewFileName->setText( a ) ;
			}else{
				auto a = tr( "Are You Sure You Want To Delete Below Folder?" ) ;

				m_ui.labelLibrarySetNewFileName->setText( a ) ;
			}

			m_ui.pbLibrarySetNewFileName->setObjectName( "Delete" ) ;

			auto m = m_table.item( row,1 ).text() ;

			m_ui.plainTextLibrarySetNewName->setPlainText( m ) ;

			m_ui.plainTextLibrarySetNewName->setReadOnly( true ) ;

			this->setRenameUiVisible( true ) ;
		}

		m_ui.pbLibrarySetNewFileName->setText( tr( "Yes" ) ) ;

		m_ui.pbLibraryCancelRename->setText( tr( "No" ) ) ;
	} ) ;

	auto ac = m.addAction( tr( "Delete All" ) ) ;

	ac->setEnabled( !hasMultipleSelections ) ;

	connect( ac,&QAction::triggered,[ this ](){

		auto a = tr( "Are You Sure You Want To Delete All Files And Folders?" ) ;

		m_ui.labelLibrarySetNewFileName->setText( a ) ;

		m_ui.pbLibrarySetNewFileName->setObjectName( "DeleteAll" ) ;

		m_ui.pbLibrarySetNewFileName->setText( tr( "Yes" ) ) ;

		m_ui.pbLibraryCancelRename->setText( tr( "No" ) ) ;

		this->setRenameUiVisible( true ) ;

		m_ui.plainTextLibrarySetNewName->setVisible( false ) ;
	} ) ;

	ac = m.addAction( tr( "Rename" ) ) ;

	ac->setEnabled( !hasMultipleSelections ) ;

	connect( ac,&QAction::triggered,[ this,row ](){

		if( m_table.stuffAt( row ) == directoryEntries::ICON::FILE ){

			auto a = tr( "Rename File To Below Text" ) ;

			m_ui.labelLibrarySetNewFileName->setText( a ) ;
		}else{
			auto a = tr( "Rename Folder To Below Text" ) ;

			m_ui.labelLibrarySetNewFileName->setText( a ) ;
		}

		m_ui.pbLibrarySetNewFileName->setObjectName( "Rename" ) ;

		m_ui.pbLibrarySetNewFileName->setText( tr( "Rename" ) ) ;

		m_ui.pbLibraryCancelRename->setText( tr( "Cancel" ) ) ;

		auto m = m_table.item( row,1 ).text() ;

		m_ui.plainTextLibrarySetNewName->setPlainText( m ) ;

		m_ui.plainTextLibrarySetNewName->moveCursor( QTextCursor::End ) ;

		m_ui.plainTextLibrarySetNewName->setReadOnly( false ) ;

		m_ui.plainTextLibrarySetNewName->setFocus() ;

		this->setRenameUiVisible( true ) ;
	} ) ;

	m.exec( QCursor::pos() ) ;
}

void library::arrangeAndShow()
{
	if( m_settings.libraryArrangeAscending() ){

		if( m_settings.libraryArrangeByDate() ){

			m_directoryEntries.sortByDateAscending() ;
		}else{
			m_directoryEntries.sortByNameAscending() ;
		}
	}else{
		if( m_settings.libraryArrangeByDate() ){

			m_directoryEntries.sortByDateDescending() ;
		}else{
			m_directoryEntries.sortByNameDescending() ;
		}
	}

	m_table.clear() ;

	m_directoryEntries.join( m_settings.libraryShowFolderFirst() ) ;

	this->addEntrySlot( m_directoryEntries.Iter() ) ;
}

static void _set_option( QMenu& m,const QString& tr,const QString& utr,bool o )
{
	auto ac = m.addAction( tr ) ;
	ac->setObjectName( utr ) ;
	ac->setCheckable( true ) ;
	ac->setChecked( o ) ;
	m.addAction( ac ) ;
}

void library::arrangeEntries( int )
{
	QMenu m ;

	auto a = m_settings.libraryShowFolderFirst() ;

	_set_option( m,QObject::tr( "Show Folders First" ),"Show Folders First",a ) ;

	a = m_settings.libraryArrangeAscending() ;

	_set_option( m,QObject::tr( "Arrange In Ascending Order" ),"Arrange In Ascending Order",a ) ;

	a = m_settings.libraryArrangeByDate() ;

	_set_option( m,QObject::tr( "Arrange By Date" ),"Arrange By Date",a ) ;

	_set_option( m,QObject::tr( "Arrange By Name" ),"Arrange By Name",!a ) ;

	QObject::connect( &m,&QMenu::triggered,[ this ]( QAction * ac ){

		auto e = ac->objectName() ;

		auto checked = ac->isChecked() ;

		if( e == "Show Folders First" ){

			m_settings.setLibraryShowFolderFirst( checked ) ;

		}else if( e == "Arrange In Ascending Order" ){

			m_settings.setLibraryArrangeAscending( checked ) ;

		}else if( e == "Arrange By Date" ){

			m_settings.setLibraryArrangeByDate( checked ) ;

		}else if( e == "Arrange By Name" ){

			auto m = m_settings.libraryArrangeByDate() ;

			m_settings.setLibraryArrangeByDate( !m ) ;
		}

		this->disableAll() ;

		m_ui.pbLibraryCancel->setEnabled( true ) ;

		this->arrangeAndShow() ;
	} ) ;

	m.exec( QCursor::pos() ) ;
}

void library::showContents( const QString& path )
{
	m_table.get().setHorizontalHeaderItem( 1,new QTableWidgetItem( m_currentPath ) ) ;

	this->disableAll() ;

	m_ui.pbLibraryCancel->setEnabled( true ) ;

	class meaw
	{
	public:
		meaw( library& library,const QString& path ) :
			m_parent( library ),
			m_path( path )
		{
		}
		void bg()
		{
			auto& m = m_parent.m_continue ;
			m_parent.m_directoryEntries = directoryManager::readAll( m_path,m ) ;
		}
		void fg()
		{
			m_parent.arrangeAndShow() ;
		}
	private:
		library& m_parent ;
		QString m_path ;
	} ;

	utils::qthread::run( meaw( *this,path ) ) ;
}

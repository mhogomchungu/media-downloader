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
	m_enableGlobalUiChanges( false ),
	m_settings( m_ctx.Settings() ),
	m_enabled( m_settings.enableLibraryTab() ),
	m_ui( m_ctx.Ui() ),
	m_table( *m_ui.tableWidgetLibrary,m_ctx.mainWidget().font() ),
	m_downloadFolder( QDir::fromNativeSeparators( m_settings.downloadFolder() ) ),
	m_currentPath( m_downloadFolder ),
	m_folderIcon( m_settings.getIcon( "folder" ).pixmap( 30,40 ) ),
	m_videoIcon( m_settings.getIcon( "video" ).pixmap( 30,40 ) )
{
	qRegisterMetaType< directoryEntries::iter >() ;

	m_ui.cbLibraryTabEnable->setChecked( m_enabled ) ;

	connect( m_ui.pbLibraryCancel,&QPushButton::clicked,[ & ](){

		m_continue = false ;
	} ) ;

	connect( this,&library::addEntrySignal,this,&library::addEntrySlot,Qt::QueuedConnection ) ;

	m_table.setUpHeaderMenu( [ this ]( int column ){

		this->arrangeEntries( column ) ;
	} ) ;

	connect( m_ui.cbLibraryTabEnable,&QCheckBox::clicked,[ this ]( bool e ){

		m_enabled = e ;

		m_settings.setEnableLibraryTab( e ) ;

		if( e ){

			this->enableAll( true ) ;
			this->showContents( m_currentPath,m_ctx.TabManager().uiEnabled() ) ;
		}else{
			m_table.clear() ;
			this->disableAll( true ) ;
			m_ui.pbLibraryQuit->setEnabled( true ) ;
			m_ui.pbLibraryDowloadFolder->setEnabled( true ) ;
			m_ui.cbLibraryTabEnable->setEnabled( true ) ;
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
	if( m_enabled ){

		this->enableAll( true ) ;
	}else{
		this->disableAll( true ) ;
		m_ui.pbLibraryQuit->setEnabled( true ) ;
		m_ui.pbLibraryDowloadFolder->setEnabled( true ) ;
		m_ui.cbLibraryTabEnable->setEnabled( true ) ;
	}
}

void library::enableAll()
{
	m_ui.cbLibraryTabEnable->setEnabled( true ) ;

	if( m_enabled ){

		this->enableAll( m_enableGlobalUiChanges ) ;
	}
}

void library::disableAll()
{
	if( m_enabled ){

		this->disableAll( m_enableGlobalUiChanges ) ;
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
	if( m_enabled && m_table.rowCount() == 0 ){

		this->showContents( m_currentPath,m_ctx.TabManager().uiEnabled() ) ;
	}
}

void library::tabExited()
{
	m_continue = false ;
}

void library::textAlignmentChanged( Qt::LayoutDirection )
{
}

void library::enableAll( bool e )
{
	if( e ){

		m_table.setEnabled( true ) ;
		m_ui.cbLibraryTabEnable->setEnabled( true ) ;
		m_ui.pbLibraryQuit->setEnabled( true ) ;
		m_ui.pbLibraryCancel->setEnabled( true ) ;
		m_ui.pbLibraryHome->setEnabled( true ) ;
		m_ui.pbLibraryDowloadFolder->setEnabled( true ) ;
		m_ui.pbLibraryRefresh->setEnabled( true ) ;
		m_ui.pbLibraryUp->setEnabled( true ) ;
	}
}

void library::disableAll( bool e )
{
	if( e ){

		m_table.setEnabled( false ) ;
		m_ui.cbLibraryTabEnable->setEnabled( false ) ;
		m_ui.pbLibraryQuit->setEnabled( false ) ;
		m_ui.pbLibraryCancel->setEnabled( false ) ;
		m_ui.pbLibraryHome->setEnabled( false ) ;
		m_ui.pbLibraryDowloadFolder->setEnabled( false ) ;
		m_ui.pbLibraryRefresh->setEnabled( false ) ;
		m_ui.pbLibraryUp->setEnabled( false ) ;
	}
}

void library::internalEnableAll()
{
	if( m_enableGlobalUiChanges ){

		m_ctx.TabManager().enableAll() ;
	}else{
		this->enableAll( true ) ;
	}
}

void library::internalDisableAll()
{
	if( m_enableGlobalUiChanges ){

		m_ctx.TabManager().disableAll() ;
	}else{
		this->disableAll( true ) ;
	}
}

void library::addItem( const directoryEntries::iter& s )
{
	auto icon = s.icon() ;

	auto row = m_table.addRow( icon ) ;

	m_table.get().setCellWidget( row,0,[ & ](){

		auto label = new QLabel() ;

		if( icon == directoryEntries::ICON::FILE ){

			label->setPixmap( m_videoIcon ) ;
		}else{
			label->setPixmap( m_folderIcon ) ;
		}

		label->setAlignment( Qt::AlignCenter ) ;

		return label ;
	}() ) ;

	auto& item = m_table.item( row,1 ) ;

	item.setText( s.value() ) ;
	item.setTextAlignment( Qt::AlignCenter ) ;
	item.setFont( m_ctx.mainWidget().font() ) ;
}

void library::addEntrySlot( const directoryEntries::iter& s )
{
	auto& t = m_table.get() ;

	if( s.hasNext() && m_continue ){

		this->addItem( s ) ;

		t.setCurrentCell( m_table.rowCount() - 1,t.columnCount() - 1 ) ;

		emit this->addEntrySignal( s.next() ) ;
	}else{
		if( t.rowCount() > 0 ){

			t.setCurrentCell( 0,t.columnCount() - 1 ) ;
		}

		if( m_disableUi ){

			this->internalEnableAll() ;
		}		
	}
}

void library::cxMenuRequested( QPoint )
{
	QMenu m ;

	connect( m.addAction( tr( "Delete" ) ),&QAction::triggered,[ this ](){

		auto row = m_table.currentRow() ;

		if( row != -1 && m_table.isSelected( row ) ){

			auto m = m_currentPath + "/" + m_table.item( row,1 ).text() ;

			this->internalDisableAll() ;

			m_ui.pbLibraryCancel->setEnabled( true ) ;

			utils::qthread::run( [ this,m ](){

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

			},[ row,this ]( bool s ){

				if( !s ){

					m_table.removeRow( row ) ;
				}

				this->internalEnableAll() ;
			} ) ;
		}
	} ) ;

	connect( m.addAction( tr( "Delete All" ) ),&QAction::triggered,[ this ](){

		this->internalDisableAll() ;

		m_ui.pbLibraryCancel->setEnabled( true ) ;

		utils::qthread::run( [ this ](){

			directoryManager::removeDirectoryContents( m_currentPath,m_continue ) ;

		},[ this ](){

			this->showContents( m_currentPath ) ;

			this->internalEnableAll() ;
		} ) ;
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

	m_ui.pbLibraryCancel->setEnabled( true ) ;

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

		this->internalDisableAll() ;

		this->arrangeAndShow() ;
	} ) ;

	m.exec( QCursor::pos() ) ;
}

void library::showContents( const QString& path,bool disableUi )
{
	m_table.get().setHorizontalHeaderItem( 1,new QTableWidgetItem( m_currentPath ) ) ;

	if( disableUi ){

		this->internalDisableAll() ;
	}

	m_ui.pbLibraryCancel->setEnabled( true ) ;

	utils::qthread::run( [ path,this ](){

		m_directoryEntries = directoryManager::readAll( path,m_continue ) ;

	},[ disableUi,this ](){

		m_disableUi = disableUi ;

		this->arrangeAndShow() ;
	} ) ;
}

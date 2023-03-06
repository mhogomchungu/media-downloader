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
	m_folderIcon( QIcon( ":/folder" ).pixmap( 30,40 ) ),
	m_videoIcon( QIcon( ":/video" ).pixmap( 30,40 ) )
{
	qRegisterMetaType< directoryEntries::iter >() ;

	m_ui.cbLibraryTabEnable->setChecked( m_enabled ) ;

	QObject::connect( m_ui.pbLibraryCancel,&QPushButton::clicked,[ & ](){

		m_continue = false ;
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
			m_ui.labelLibraryWarning->setEnabled( true ) ;
		}
	} ) ;

	m_table.connect( &QTableWidget::currentItemChanged,[ this ]( QTableWidgetItem * c,QTableWidgetItem * p ){

		m_table.selectRow( c,p,1 ) ;
	} ) ;

	m_table.connect( &QTableWidget::customContextMenuRequested,[ this ]( QPoint ){

		QMenu m ;

		connect( m.addAction( tr( "Delete" ) ),&QAction::triggered,[ this ](){

			auto row = m_table.currentRow() ;

			if( row != -1 && m_table.isSelected( row ) ){

				auto m = m_currentPath + "/" + m_table.item( row,1 ).text() ;

				this->internalDisableAll() ;

				utils::qthread::run( [ m ](){

					if( QFileInfo( m ).isFile() ){

						QFile::remove( m ) ;
					}else{
						QDir( m ).removeRecursively() ;
					}

				},[ row,this ](){

					m_table.removeRow( row ) ;

					this->internalEnableAll() ;
				} ) ;
			}
		} ) ;

		connect( m.addAction( tr( "Delete All" ) ),&QAction::triggered,[ this ](){

			this->internalDisableAll() ;

			utils::qthread::run( [ this ](){

				for( const auto& it : QDir( m_currentPath ).entryList( m_dirFilter ) ){

					auto m = m_currentPath + "/" + it ;

					QFileInfo f( m ) ;

					if( f.isFile() ){

						QFile::remove( m ) ;

					}else if( f.isDir() ){

						QDir( m ).removeRecursively() ;
					}
				}

			},[ this ](){

				this->showContents( m_currentPath ) ;

				this->internalEnableAll() ;
			} ) ;
		} ) ;

		m.exec( QCursor::pos() ) ;
	} ) ;

	connect( m_ui.pbLibraryQuit,&QPushButton::clicked,[ this ](){

		m_ctx.mainWindow().quitApp() ;
	} ) ;

	connect( m_ui.pbLibraryDowloadFolder,&QPushButton::clicked,[ this ](){

		utility::openDownloadFolderPath( m_currentPath ) ;
	} ) ;

	connect( m_ui.pbLibraryHome,&QPushButton::clicked,[ this ](){

		m_downloadFolder = QDir::fromNativeSeparators( m_settings.downloadFolder() ) ;

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

		if( m_table.stuffAt( row ) == library::ICON::FOLDER ){

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
		m_ui.labelLibraryWarning->setEnabled( true ) ;
	}
}

void library::enableAll()
{
	m_ui.cbLibraryTabEnable->setEnabled( true ) ;
	m_ui.labelLibraryWarning->setEnabled( true ) ;

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
}

void library::enableAll( bool e )
{
	if( e ){

		m_table.setEnabled( true ) ;
		m_ui.cbLibraryTabEnable->setEnabled( true ) ;
		m_ui.labelLibraryWarning->setEnabled( true ) ;
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
		m_ui.labelLibraryWarning->setEnabled( false ) ;
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

void library::addItem( const QString& text,library::ICON type )
{
	auto row = m_table.addRow( type ) ;

	m_table.get().setCellWidget( row,0,[ & ](){

		auto label = new QLabel() ;

		if( type == library::ICON::FILE ){

			label->setPixmap( m_videoIcon ) ;
		}else{
			label->setPixmap( m_folderIcon ) ;
		}

		label->setAlignment( Qt::AlignCenter ) ;

		return label ;
	}() ) ;

	auto& item = m_table.item( row,1 ) ;

	item.setText( text ) ;
	item.setTextAlignment( Qt::AlignCenter ) ;
	item.setFont( m_ctx.mainWidget().font() ) ;
}

void library::addFolder( const directoryEntries::iter& s )
{
	if( s.hasNext() && m_continue ){

		this->addItem( s.value(),library::ICON::FOLDER ) ;

		auto& t = m_table.get() ;

		t.setCurrentCell( m_table.rowCount() - 1,t.columnCount() - 1 ) ;

		auto a = "addFolder" ;
		auto b = Qt::QueuedConnection ;

		QMetaObject::invokeMethod( this,a,b,Q_ARG( directoryEntries::iter,s.next() ) ) ;
	}else{
		this->addFile( m_directoryEntries.fileIter() ) ;
	}
}

void library::addFile( const directoryEntries::iter& s )
{
	auto& t = m_table.get() ;

	if( s.hasNext() && m_continue ){

		this->addItem( s.value(),library::ICON::FILE ) ;

		t.setCurrentCell( m_table.rowCount() - 1,t.columnCount() - 1 ) ;

		auto a = "addFile" ;
		auto b = Qt::QueuedConnection ;

		QMetaObject::invokeMethod( this,a,b,Q_ARG( directoryEntries::iter,s.next() ) ) ;
	}else{
		if( t.rowCount() > 0 ){

			t.setCurrentCell( 0,t.columnCount() - 1 ) ;
		}

		if( m_disableUi ){

			this->internalEnableAll() ;
		}
	}
}

void library::showContents( const QString& path,bool disableUi )
{
	m_table.clear() ;

	m_table.get().setHorizontalHeaderItem( 1,new QTableWidgetItem( m_currentPath ) ) ;

	if( disableUi ){

		this->internalDisableAll() ;
	}

	m_continue = true ;

	m_ui.pbLibraryCancel->setEnabled( directoryManager::supportsCancel() ) ;

	utils::qthread::run( [ path,this ](){

		directoryManager dm( path,m_dirFilter,m_continue ) ;

		if( dm.valid() ){

			if( dm.readFirst() ){

				while( dm.Continue() ){

					if( dm.read() ){

						break ;
					}
				}
			}
		}

		return dm.entries() ;

	},[ disableUi,this ]( directoryEntries m ){

		m_directoryEntries = std::move( m ) ;

		m_disableUi = disableUi ;

		m_ui.pbLibraryCancel->setEnabled( true ) ;

		this->addFolder( m_directoryEntries.directoryIter() ) ;
	} ) ;
}

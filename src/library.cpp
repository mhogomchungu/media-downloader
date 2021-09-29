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

#include <QDir>

library::library( const Context& ctx ) :
	m_ctx( ctx ),
	m_enableGlobalUiChanges( false ),
	m_settings( m_ctx.Settings() ),
	m_ui( m_ctx.Ui() ),
	m_table( *m_ui.tableWidgetLibrary,m_ctx.mainWidget().font(),1 ),
	m_downloadFolder( QDir::fromNativeSeparators( m_settings.downloadFolder() ) ),
	m_currentPath( m_downloadFolder ),
	m_folderIcon( QIcon( ":/folder" ).pixmap( 30,40 ) ),
	m_videoIcon( QIcon( ":/video" ).pixmap( 30,40 ) )
{
	m_table.get().hideColumn( 2 ) ;

	m_table.connect( &QTableWidget::currentItemChanged,[ this ]( QTableWidgetItem * c,QTableWidgetItem * p ){

		m_table.selectRow( c,p,m_table.startPosition() ) ;
	} ) ;

	m_table.connect( &QTableWidget::customContextMenuRequested,[ this ]( QPoint ){

		QMenu m ;

		connect( m.addAction( tr( "Delete" ) ),&QAction::triggered,[ this ](){

			auto row = m_table.currentRow() ;

			if( row != -1 && m_table.uiTextItem( row ).isSelected() ){

				auto m = m_currentPath + "/" + m_table.uiText( row ) ;

				this->internalDisableAll() ;

				util::runInBgThread( [ m ](){

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

			util::runInBgThread( [ this ](){

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

		m_ctx.TabManager().basicDownloader().appQuit() ;
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

		auto s = m_table.uiText( row ) ;

		if( m_table.url( row ) == "folder" ){

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
}

void library::enableAll()
{
	this->enableAll( m_enableGlobalUiChanges ) ;
}

void library::disableAll()
{
	this->disableAll( m_enableGlobalUiChanges ) ;
}

void library::resetMenu()
{
}

void library::retranslateUi()
{
}

void library::tabEntered()
{
	this->showContents( m_currentPath,m_ctx.TabManager().uiEnabled() ) ;
}

void library::tabExited()
{
}

void library::enableAll( bool e )
{
	if( e ){

		m_table.setEnabled( true ) ;
		m_ui.pbLibraryQuit->setEnabled( true ) ;
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
		m_ui.pbLibraryQuit->setEnabled( false ) ;
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

struct Icon{
	const QString& type ;
	const QPixmap& icon ;
};

static void _addItem( tableWidget& t,const QString& text,const QFont& font,const Icon& icon )
{
	auto& table = t.get() ;

	auto row = table.rowCount() ;

	table.insertRow( row ) ;

	auto item = new QTableWidgetItem() ;

	item->setText( text ) ;
	item->setTextAlignment( Qt::AlignCenter ) ;
	item->setFont( font ) ;

	table.setItem( row,1,item ) ;

	item = new QTableWidgetItem() ;
	auto item1 = new QTableWidgetItem() ;

	item->setTextAlignment( Qt::AlignCenter ) ;

	auto label = new QLabel() ;

	item1->setText( icon.type ) ;
	label ->setPixmap( icon.icon ) ;

	label->setAlignment( Qt::AlignHCenter | Qt::AlignVCenter ) ;

	table.setCellWidget( row,0,label ) ;

	table.setItem( row,2,item1 ) ;
}

static qint64 _created_time( QFileInfo& e )
{
#if QT_VERSION >= QT_VERSION_CHECK( 5,10,0 )
	return e.birthTime().toMSecsSinceEpoch() ;
#else
	return e.created().toMSecsSinceEpoch() ;
#endif
}

void library::showContents( const QString& path,bool disableUi )
{
	m_table.clear() ;

	m_table.get().setHorizontalHeaderItem( 1,new QTableWidgetItem( m_currentPath ) ) ;

	if( disableUi ){

		this->internalDisableAll() ;
	}

	util::runInBgThread( [ path,this ](){

		return QDir( path ).entryList( m_dirFilter ) ;

	},[ path,disableUi,this ]( const QStringList& m ){

		if( disableUi ){

			this->internalEnableAll() ;
		}

		auto& font = m_ctx.mainWidget().font() ;

		struct entry
		{
			entry( bool f,qint64 d,QString p ) :
				file( f ),
				dateCreated( d ),
				path( std::move( p ) )
			{
			}
			bool file ;
			qint64 dateCreated ;
			QString path ;
		};

		std::vector< entry > folders ;
		std::vector< entry > files ;

		for( const auto& it : m ){

			if( it.startsWith( "info_" ) && it.endsWith( ".log" ) ){

				continue ;
			}

			auto q = path + "/" + it ;

			auto w = QDir::fromNativeSeparators( it ) ;

			QFileInfo s( q ) ;

			if( s.isFile() ){

				files.emplace_back( true,_created_time( s ),w ) ;

			}else if( s.isDir() ){

				folders.emplace_back( false,_created_time( s ),w ) ;
			}
		}

		std::sort( folders.begin(),folders.end(),[]( const entry& lhs,const entry& rhs ){

			return lhs.dateCreated < rhs.dateCreated ;
		} ) ;

		std::sort( files.begin(),files.end(),[]( const entry& lhs,const entry& rhs ){

			return lhs.dateCreated < rhs.dateCreated ;
		} ) ;

		for( const auto& it : folders ){

			_addItem( m_table,it.path,font,{ "folder",m_folderIcon } ) ;
		}

		for( const auto& it : files ){

			_addItem( m_table,it.path,font,{ "file",m_videoIcon } ) ;
		}

		if( m_table.rowCount() > 0 ){

			auto& t = m_table.get() ;

			t.setCurrentCell( m_table.rowCount() - 1,t.columnCount() - 1 ) ;
		}
	} ) ;
}

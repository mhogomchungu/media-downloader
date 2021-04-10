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

#include "batchdownloader.h"
#include "tabmanager.h"

batchdownloader::batchdownloader( const Context& ctx ) :
	m_ctx( ctx ),
	m_settings( m_ctx.Settings() ),
	m_ui( m_ctx.Ui() ),
	m_mainWindow( m_ctx.mainWidget() ),
	m_tabManager( m_ctx.TabManager() ),
	m_running( false ),
	m_debug( ctx.debug() ),
	m_ccmd( m_ctx,*m_ui.lineEditBDUrlOptions,*m_ui.tableWidgetBD,*m_ui.pbBDCancel )
{
	qRegisterMetaType< downloadFinished >() ;

	m_ui.tabWidgetBatchDownlader->setCurrentIndex( 0 ) ;

	utility::setTableWidget( *m_ui.tableWidgetBD ) ;

	m_ui.pbBDDownload->setEnabled( false ) ;

	m_ui.pbBDCancel->setEnabled( false ) ;

	this->resetMenu() ;

	connect( m_ui.tabWidgetBatchDownlader,&QTabWidget::currentChanged,[ this ]( int s ){

		if( s == 0 ){

			m_ui.pbBDDownload->setEnabled( m_ui.tableWidgetBD->rowCount() ) ;
		}
	} ) ;

	connect( m_ui.tableWidgetBD,&QTableWidget::customContextMenuRequested,[ this ]( QPoint ){

		if( m_running ){


		}else{
			if( m_ui.tableWidgetBD->rowCount() > 0 ){

				QMenu m ;

				connect( m.addAction( tr( "Remove" ) ),&QAction::triggered,[ this ](){

					auto row = m_ui.tableWidgetBD->currentRow() ;

					if( row != -1 ){

						m_ui.tableWidgetBD->removeRow( row ) ;

						m_ui.pbBDDownload->setEnabled( m_ui.tableWidgetBD->rowCount() ) ;
					}
				} ) ;

				m.exec( QCursor::pos() ) ;
			}
		}
	} ) ;

	connect( m_ui.pbBDQuit,&QPushButton::clicked,[ this ](){

		m_tabManager.basicDownloader().appQuit() ;
	} ) ;

	connect( m_ui.pbBDCancel,&QPushButton::clicked,[ this ](){

		m_ccmd.cancelled() ;
	} ) ;

	connect( m_ui.pbBDAdd,&QPushButton::clicked,[ this ](){

		this->addToList( m_ui.lineEditBDUrl->text() ) ;
	} ) ;

	connect( m_ui.pbBDDownload,&QPushButton::clicked,[ this ](){

		this->download( m_ctx.Engines().defaultEngine() ) ;
	} ) ;
}

void batchdownloader::init_done()
{
}

void batchdownloader::resetMenu()
{
	utility::setMenuOptions( m_ctx,{},true,m_ui.pbBDOptions,[ this ]( QAction * aa ){

		utility::selectedAction ac( aa ) ;

		if( ac.clearOptions() ){

			m_ui.lineEditBDUrlOptions->clear() ;

		}else if( ac.clearScreen() ){

			this->clearScreen() ;

		}else if( ac.openFolderPath() ){

			utility::openDownloadFolderPath( m_settings.downloadFolder() ) ;
		}else{
			m_ui.lineEditBDUrlOptions->setText( ac.objectName() ) ;

			this->download( m_ctx.Engines().defaultEngine() ) ;
		}
	} ) ;
}

void batchdownloader::retranslateUi()
{
	this->resetMenu() ;
}

void batchdownloader::tabEntered()
{
}

void batchdownloader::tabExited()
{
}

void batchdownloader::download( const engines::engine& engine,
				const QString& opts,
				const QStringList& list )
{
	for( int s = 0 ; s < m_ui.tableWidgetBD->rowCount() ; s++ ){

		m_ui.tableWidgetBD->removeRow( 0 ) ;
	}

	for( const auto& it : list ){

		this->addToList( it ) ;
	}

	m_ui.tabWidget->setCurrentIndex( 1 ) ;

	m_ui.tabWidgetBatchDownlader->setCurrentIndex( 0 ) ;

	m_ui.lineEditBDUrlOptions->setText( opts ) ;

	this->download( engine ) ;
}

void batchdownloader::monitorForFinished( downloadFinished f )
{
	m_ccmd.monitorForFinished( std::move( f ),[ this ]( const engines::engine& engine,int index ){

		this->download( engine,index ) ;
	} ) ;
}

void batchdownloader::clearScreen()
{
	auto s = m_ui.tableWidgetBD->rowCount() ;

	for( int i = 0 ; i < s ; i++ ){

		m_ui.tableWidgetBD->removeRow( 0 ) ;
	}

	m_ui.lineEditBDUrlOptions->clear() ;
	m_ui.lineEditBDUrl->clear() ;
}

void batchdownloader::addToList( const QString& a )
{
	if( !a.isEmpty() ){

		auto table = m_ui.tableWidgetBD ;

		auto row = table->rowCount() ;

		table->insertRow( row ) ;

		auto item = new QTableWidgetItem() ;

		item->setText( a ) ;
		item->setTextAlignment( Qt::AlignCenter ) ;
		item->setFont( m_mainWindow.font() ) ;

		table->setItem( row,0,item ) ;

		m_ui.lineEditBDUrl->clear() ;

		m_ui.lineEditBDUrl->setFocus() ;

		m_ui.pbBDDownload->setEnabled( true ) ;
	}
}

void batchdownloader::download( const engines::engine& engine )
{
	this->addToList( m_ui.lineEditBDUrl->text() ) ;

	bool cd = m_settings.concurrentDownloading() ;
	int cdc = m_settings.maxConcurrentDownloads() ;

	m_ccmd.download( engine,cd,cdc,[ this ]( const engines::engine& engine,int index ){

		this->download( engine,index ) ;

	},[ this ]( const engines::engine& engine,const QString& options,const QStringList& urls ){

		this->clearScreen() ;

		m_tabManager.basicDownloader().download( engine,options,urls ) ;
	} ) ;
}

void batchdownloader::download( const engines::engine& engine,int index )
{
	auto aa = batchdownloader::make_options( *m_ui.pbBDCancel,m_ctx,m_debug,[ &engine,index,this ](){

		QMetaObject::invokeMethod( this,
					   "monitorForFinished",
					   Qt::QueuedConnection,
					   Q_ARG( downloadFinished,downloadFinished( engine,index ) ) ) ;
	} ) ;

	auto item = m_ui.tableWidgetBD->item( index,0 ) ;

	m_ccmd.download( engine,
			 item->text(),
			 std::move( aa ),
			 loggerLoggerTableWidgetItem( engine.filter(),engine,*item ) ) ;
}

void batchdownloader::enableAll()
{
	m_running = false ;

	m_ui.tableWidgetBD->setEnabled( true ) ;
	m_ui.pbBDDownload->setEnabled( m_ui.tableWidgetBD->rowCount() ) ;
	m_ui.pbBDAdd->setEnabled( true ) ;
	m_ui.pbBDOptions->setEnabled( true ) ;
	m_ui.labelBDEnterOptions->setEnabled( true ) ;
	m_ui.labelBDEnterUrl->setEnabled( true ) ;
	m_ui.pbBDQuit->setEnabled( true ) ;
	m_ui.lineEditBDUrl->setEnabled( true ) ;
	m_ui.lineEditBDUrlOptions->setEnabled( true ) ;
	m_ui.pbBDCancel->setEnabled( true ) ;
}

void batchdownloader::disableAll()
{
	m_running = true ;

	m_ui.pbBDCancel->setEnabled( false ) ;
	m_ui.tableWidgetBD->setEnabled( false ) ;
	m_ui.pbBDDownload->setEnabled( false ) ;
	m_ui.pbBDAdd->setEnabled( false ) ;
	m_ui.pbBDOptions->setEnabled( false ) ;
	m_ui.labelBDEnterOptions->setEnabled( false ) ;
	m_ui.labelBDEnterUrl->setEnabled( false ) ;
	m_ui.pbBDQuit->setEnabled( false ) ;
	m_ui.lineEditBDUrl->setEnabled( false ) ;
	m_ui.lineEditBDUrlOptions->setEnabled( false ) ;
}

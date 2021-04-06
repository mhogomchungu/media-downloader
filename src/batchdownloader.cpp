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

const engines::engine * batchdownloaderFinished::static_engine ;

batchdownloader::batchdownloader( const Context& ctx ) :
	m_ctx( ctx ),
	m_settings( m_ctx.Settings() ),
	m_ui( m_ctx.Ui() ),
	m_mainWindow( m_ctx.mainWidget() ),
	m_tabManager( m_ctx.TabManager() ),
	m_running( false ),
	m_cancelled( false ),
	m_debug( ctx.debug() )
{
	qRegisterMetaType< batchdownloaderFinished >() ;

	m_ui.tabWidgetBatchDownlader->setCurrentIndex( 0 ) ;

	m_ui.tableWidgetBD->verticalHeader()->setSectionResizeMode( QHeaderView::ResizeToContents ) ;

	m_ui.tableWidgetBD->verticalHeader()->setMinimumSectionSize( 30 ) ;

	m_ui.tableWidgetBD->horizontalHeader()->setStretchLastSection( true ) ;

	m_ui.tableWidgetBD->setMouseTracking( true ) ;

	m_ui.tableWidgetBD->setContextMenuPolicy( Qt::CustomContextMenu ) ;

	m_ui.tableWidgetBD->setEditTriggers( QAbstractItemView::NoEditTriggers ) ;
	m_ui.tableWidgetBD->setFocusPolicy( Qt::NoFocus ) ;
	m_ui.tableWidgetBD->setSelectionMode( QAbstractItemView::NoSelection ) ;

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

		m_cancelled = true ;
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

			auto s = m_ui.tableWidgetBD->rowCount() ;

			for( int i = 0 ; i < s ; i++ ){

				m_ui.tableWidgetBD->removeRow( 0 ) ;
			}

			m_ui.lineEditBDUrlOptions->clear() ;
			m_ui.lineEditBDUrl->clear() ;

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

	m_downloadList.clear() ;

	this->download( engine ) ;
}

void batchdownloader::monitorForFinished( batchdownloaderFinished f )
{
	m_counter++ ;

	if( m_counter == m_ui.tableWidgetBD->rowCount() ){

		m_ctx.TabManager().enableAll() ;
		m_ui.pbBDCancel->setEnabled( false ) ;

		if( m_cancelled ){

			if( m_downloadList.size() == m_ui.tableWidgetBD->rowCount() ){

				for( int s = 0 ; s < m_ui.tableWidgetBD->rowCount() ; s++ ){

					m_ui.tableWidgetBD->item( s,0 )->setText( m_downloadList[ s ] ) ;
				}
			}
		}
	}else{
		if( m_index < m_ui.tableWidgetBD->rowCount() ){

			this->download( f.engine(),m_index ) ;
		}
	}
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

	if( m_ui.tableWidgetBD->rowCount() ){

		if( m_settings.sequentialDownloading() ){

			QStringList urls ;

			for( int s = 0 ; s < m_ui.tableWidgetBD->rowCount() ; s++ ){

				urls.append( m_ui.tableWidgetBD->item( s,0 )->text() ) ;
			}

			auto options = m_ui.lineEditBDUrlOptions->text() ;

			m_tabManager.basicDownloader().download( engine,options,urls ) ;
		}else{
			m_counter = 0 ;
			m_cancelled = false ;
			m_index = 0 ;

			m_ctx.TabManager().disableAll() ;
			m_ui.pbBDCancel->setEnabled( true ) ;
			m_ui.tableWidgetBD->setEnabled( true ) ;

			m_downloadList.clear() ;

			auto m = m_settings.maxConcurrentDownloads() ;

			if( m < m_ui.tableWidgetBD->rowCount() ){

				for( int s = 0 ; s < m ; s++ ){

					this->download( engine,s ) ;
				}
			}else{
				for( int s = 0 ; s < m_ui.tableWidgetBD->rowCount() ; s++ ){

					this->download( engine,s ) ;
				}
			}
		}
	}
}

void batchdownloader::download( const engines::engine& engine,int index )
{
	m_index++ ;

	auto item = m_ui.tableWidgetBD->item( index,0 ) ;

	utility::args args( m_ui.lineEditBDUrlOptions->text() ) ;

	auto l = item->text() ;

	m_downloadList.append( l ) ;

	auto aa = batchdownloader::make_options( *m_ui.pbBDCancel,m_ctx,m_debug,[ &engine,index,this ](){

		batchdownloaderFinished::static_engine = &engine ;

		QMetaObject::invokeMethod( this,
					   "monitorForFinished",
					   Qt::QueuedConnection,
					   Q_ARG( batchdownloaderFinished,batchdownloaderFinished( engine,index ) ) ) ;
	} ) ;

	auto bb = [ &engine ]( const QString& e ){

		return engine.updateProgress( e ) ;
	} ;

	utility::run( engine,
		      utility::updateOptions( engine,args,{ l } ),
		      args.quality,
		      false,
		      std::move( aa ),
		      loggerLoggerTableWidgetItem( std::move( bb ),*item ),
		      utility::make_term_conn( m_ui.pbBDCancel,&QPushButton::clicked ) ) ;
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



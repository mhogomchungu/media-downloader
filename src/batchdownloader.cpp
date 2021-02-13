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

batchdownloader::batchdownloader( Context& ctx) :
	m_ctx( ctx ),
	m_settings( m_ctx.Settings() ),
	m_ui( m_ctx.Ui() ),
	m_mainWindow( m_ctx.mainWidget() ),
	m_tabManager( m_ctx.TabManager() )
{
	m_ui.tabWidgetBatchDownlader->setCurrentIndex( 0 ) ;

	m_ui.tableWidgetBD->verticalHeader()->setSectionResizeMode( QHeaderView::ResizeToContents ) ;

	m_ui.tableWidgetBD->verticalHeader()->setMinimumSectionSize( 30 ) ;

	m_ui.tableWidgetBD->horizontalHeader()->setStretchLastSection( true ) ;

	m_ui.tableWidgetBD->setMouseTracking( true ) ;

	m_ui.tableWidgetBD->setContextMenuPolicy( Qt::CustomContextMenu ) ;

	m_ui.pbBDDownload->setEnabled( false ) ;

	this->resetMenu() ;

	connect( m_ui.tabWidgetBatchDownlader,&QTabWidget::currentChanged,[ this ]( int s ){

		if( s == 0 ){

			m_ui.pbBDDownload->setEnabled( m_ui.tableWidgetBD->rowCount() ) ;
		}
	} ) ;

	connect( m_ui.tableWidgetBD,&QTableWidget::customContextMenuRequested,[ this ]( QPoint ){

		QMenu m ;

		connect( m.addAction( tr( "Remove" ) ),&QAction::triggered,[ this ](){

			auto row = m_ui.tableWidgetBD->currentRow() ;

			if( row != -1 ){

				m_ui.tableWidgetBD->removeRow( row ) ;

				m_ui.pbBDDownload->setEnabled( m_ui.tableWidgetBD->rowCount() ) ;
			}
		} ) ;

		m.exec( QCursor::pos() ) ;
	} ) ;

	connect( m_ui.pbBDQuit,&QPushButton::clicked,[ this ](){

		m_tabManager.basicDownloader().appQuit() ;
	} ) ;

	connect( m_ui.pbBDAdd,&QPushButton::clicked,[ this ](){

		auto a = m_ui.lineEditBDUrl->text() ;

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

			m_ui.pbBDDownload->setEnabled( true ) ;
		}
	} ) ;

	connect( m_ui.pbBDDownload,&QPushButton::clicked,[ this ](){

		this->download() ;
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
		}else{
			if( ac.best() ){

				m_ui.lineEditBDUrlOptions->setText( ac.bestText() ) ;
			}else{
				m_ui.lineEditBDUrlOptions->setText( ac.objectName() ) ;
			}

			this->download() ;
		}
	} ) ;
}

void batchdownloader::retranslateUi()
{
	this->resetMenu() ;
}

void batchdownloader::download()
{
	if( m_ui.tableWidgetBD->rowCount() ){

		QStringList urls ;

		for( int s = 0 ; s < m_ui.tableWidgetBD->rowCount() ; s++ ){

			urls.append( m_ui.tableWidgetBD->item( s,0 )->text() ) ;
		}

		auto options = m_ui.lineEditBDUrlOptions->text() ;

		const auto& engine = m_ctx.Engines().defaultEngine() ;

		m_tabManager.basicDownloader().download( engine,options,urls ) ;
	}
}

void batchdownloader::enableAll()
{
	m_ui.tableWidgetBD->setEnabled( true ) ;
	m_ui.pbBDDownload->setEnabled( m_ui.tableWidgetBD->rowCount() ) ;
	m_ui.pbBDAdd->setEnabled( true ) ;
	m_ui.pbBDOptions->setEnabled( true ) ;
	m_ui.labelBDEnterOptions->setEnabled( true ) ;
	m_ui.labelBDEnterUrl->setEnabled( true ) ;
	m_ui.pbBDQuit->setEnabled( true ) ;
	m_ui.lineEditBDUrl->setEnabled( true ) ;
	m_ui.lineEditBDUrlOptions->setEnabled( true ) ;
}

void batchdownloader::disableAll()
{
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

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

#include "playlistdownloader.h"
#include "tabmanager.h"

#include <QFileDialog>

playlistdownloader::playlistdownloader( Context& ctx ) :
	m_ctx( ctx ),
	m_settings( m_ctx.Settings() ),
	m_ui( m_ctx.Ui() ),
	m_mainWindow( m_ctx.mainWidget() ),
	m_tabManager( m_ctx.TabManager() ),
	m_ccmd( m_ctx,*m_ui.lineEditPLUrlOptions,*m_ui.tableWidgetPl,*m_ui.pbPLCancel ),
	m_running( false )
{
	this->resetMenu() ;

	utility::setTableWidget( *m_ui.tableWidgetPl ) ;

	connect( m_ui.pbPLCancel,&QPushButton::clicked,[ this ](){

		m_ccmd.cancelled() ;
	} ) ;

	connect( m_ui.pbPLGetList,&QPushButton::clicked,[ this ](){

		this->getList() ;
	} ) ;

	connect( m_ui.pbPLDownload,&QPushButton::clicked,[ this ](){

		this->download() ;
	} ) ;

	connect( m_ui.pbPLQuit,&QPushButton::clicked,[ this ](){

		m_tabManager.basicDownloader().appQuit() ;
	} ) ;
}

void playlistdownloader::init_done()
{
	if( !m_ctx.Engines().defaultEngine().canDownloadPlaylist() ){

		this->disableAll() ;
	}
}

void playlistdownloader::enableAll()
{
	if( !m_ctx.Engines().defaultEngine().canDownloadPlaylist() ){

		return ;
	}

	m_ui.lineEditPLUrl->setEnabled( true ) ;
	m_ui.labelPLEnterOptions->setEnabled( true ) ;
	m_ui.labelPLEnterUrlRange->setEnabled( true ) ;
	m_ui.lineEditPLDownloadRange->setEnabled( true ) ;
	m_ui.lineEditPLUrl->setEnabled( true ) ;
	m_ui.lineEditPLUrlOptions->setEnabled( true ) ;
	m_ui.pbPLDownload->setEnabled( true ) ;
	m_ui.pbPLOptions->setEnabled( true ) ;
	m_ui.pbPLQuit->setEnabled( true ) ;
	m_ui.labelPLEnterUrl->setEnabled( true ) ;
	m_ui.pbPLCancel->setEnabled( true ) ;
	m_ui.pbPLGetList->setEnabled( true ) ;
}

void playlistdownloader::disableAll()
{
	m_ui.pbPLGetList->setEnabled( false ) ;
	m_ui.pbPLCancel->setEnabled( false ) ;
	m_ui.lineEditPLUrl->setEnabled( false ) ;
	m_ui.labelPLEnterOptions->setEnabled( false ) ;
	m_ui.labelPLEnterUrlRange->setEnabled( false ) ;
	m_ui.lineEditPLDownloadRange->setEnabled( false ) ;
	m_ui.lineEditPLUrl->setEnabled( false ) ;
	m_ui.lineEditPLUrlOptions->setEnabled( false ) ;
	m_ui.pbPLDownload->setEnabled( false ) ;
	m_ui.pbPLOptions->setEnabled( false ) ;
	m_ui.pbPLQuit->setEnabled( false ) ;
	m_ui.labelPLEnterUrl->setEnabled( false ) ;
}

void playlistdownloader::resetMenu()
{
	utility::setMenuOptions( m_ctx,{},true,m_ui.pbPLOptions,[ this ]( QAction * aa ){

		utility::selectedAction ac( aa ) ;

		if( ac.clearOptions() ){

			m_ui.lineEditPLUrlOptions->clear() ;

		}else if( ac.clearScreen() ){

			this->clearScreen() ;

		}else if( ac.openFolderPath() ){

			utility::openDownloadFolderPath( m_settings.downloadFolder() ) ;
		}else{
			m_ui.lineEditPLUrlOptions->setText( ac.objectName() ) ;

			this->download() ;
		}
	} ) ;
}

void playlistdownloader::retranslateUi()
{
	this->resetMenu() ;
}

void playlistdownloader::tabEntered()
{
	if( !m_running ){

		m_ui.pbPLOptions->setEnabled( m_ui.tableWidgetPl->rowCount() > 0 ) ;
		m_ui.pbPLCancel->setEnabled( false ) ;
		m_ui.pbPLDownload->setEnabled( m_ui.tableWidgetPl->rowCount() > 0 ) ;
	}
}

void playlistdownloader::tabExited()
{
}

void playlistdownloader::monitorForFinished( downloadFinished f )
{
	m_ccmd.monitorForFinished( std::move( f ),[ this ]( const engines::engine& engine,int index ){

		this->download( engine,index ) ;

	},[ this ](){

		m_running = false ;
	} ) ;
}

void playlistdownloader::download()
{
	m_running = true ;

	this->download( m_ctx.Engines().defaultEngine() ) ;
}

void playlistdownloader::download( const engines::engine& engine )
{
	m_ccmd.download( engine,[ this ](){

		if( m_settings.concurrentDownloading() ){

			return m_settings.maxConcurrentDownloads() ;
		}else{
			return 1 ;
		}

	}(),[ this ]( const engines::engine& engine,int index ){

		this->download( engine,index ) ;
	} ) ;
}

void playlistdownloader::download( const engines::engine& engine,int index )
{
	auto aa = playlistdownloader::make_options( *m_ui.pbPLCancel,m_ctx,m_ctx.debug(),[ &engine,index,this ](){

		QMetaObject::invokeMethod( this,
					   "monitorForFinished",
					   Qt::QueuedConnection,
					   Q_ARG( downloadFinished,downloadFinished( engine,index ) ) ) ;
	} ) ;

	auto item = m_ui.tableWidgetPl->item( index,0 ) ;

	m_ccmd.download( engine,
			 item->text(),
			 std::move( aa ),
			 make_loggerBatchDownloader( engine.filter(),engine,m_ctx.logger(),*item ) ) ;
}

void playlistdownloader::getList()
{
	auto url = m_ui.lineEditPLUrl->text() ;

	if( url.isEmpty() ){

		return ;
	}

	m_ctx.TabManager().disableAll() ;

	m_ui.pbPLCancel->setEnabled( true ) ;

	const auto& engine = m_ctx.Engines().defaultEngine() ;

	QStringList opts ;

	opts.append( engine.playListIdArgument() ) ;

	auto range = m_ui.lineEditPLDownloadRange->text() ;

	if( !range.isEmpty() ){

		opts.append( engine.playlistItemsArgument() ) ;
		opts.append( range ) ;
	}

	opts.append( m_ui.lineEditPLUrl->text() ) ;

	utility::args args( m_ui.lineEditPLUrlOptions->text() ) ;

	auto aa = playlistdownloader::make_options( *m_ui.pbPLCancel,m_ctx,m_ctx.debug(),[ this ](){

		m_running = false ;
		m_ctx.TabManager().enableAll() ;
		m_ui.pbPLCancel->setEnabled( false ) ;
	} ) ;

	m_running = true ;

	utility::run( engine,
		      opts,
		      args.quality,
		      false,
		      std::move( aa ),
		      loggerPlaylistDownloader( *m_ui.tableWidgetPl,
						m_ctx.mainWidget().font(),
						m_ctx.logger(),
						engine.playListUrlPrefix() ),
		      utility::make_term_conn( m_ui.pbPLCancel,&QPushButton::clicked ) ) ;
}

void playlistdownloader::clearScreen()
{
	auto s = m_ui.tableWidgetPl->rowCount() ;

	for( int i = 0 ; i < s ; i++ ){

		m_ui.tableWidgetPl->removeRow( 0 ) ;
	}

	m_ui.lineEditPLUrlOptions->clear() ;
	m_ui.lineEditPLDownloadRange->clear() ;
	m_ui.lineEditPLUrl->clear() ;
}

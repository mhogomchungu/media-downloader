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
	m_table( *m_ui.tableWidgetPl ),
	m_running( false ),
	m_ccmd( m_ctx,
		playlistdownloader::Index( m_playlistEntry,m_table ),
		*m_ui.lineEditPLUrlOptions,
		*m_ui.pbPLCancel )
{
	this->resetMenu() ;

	utility::setTableWidget( m_table ) ;

	m_table.hideColumn( 1 ) ;
	m_table.hideColumn( 2 ) ;

	auto s = static_cast< void( QComboBox::* )( int ) >( &QComboBox::activated ) ;

	connect( &m_table,&QTableWidget::cellDoubleClicked,[ this ]( int row,int column ){

		Q_UNUSED( column )

		m_ctx.Engines().openUrls( *m_table.item( row,0 ),
					  m_ui.cbEngineTypePD->currentText() ) ;
	} ) ;

	connect( m_ui.cbEngineTypePD,s,[ & ]( int s ){

		if( s != -1 ){

			m_ui.lineEditPLUrlOptions->clear() ;
			auto m = m_ui.cbEngineTypePD->itemText( s ) ;

			m_settings.setDefaultEngine( m,settings::tabName::playlist ) ;
		}
	} ) ;

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
}

void playlistdownloader::enableAll()
{
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
	m_ui.labelPLEngineName->setEnabled( true ) ;
	m_ui.cbEngineTypePD->setEnabled( true ) ;
}

void playlistdownloader::disableAll()
{
	m_ui.cbEngineTypePD->setEnabled( false ) ;
	m_ui.labelPLEngineName->setEnabled( false ) ;
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

		m_ui.pbPLOptions->setEnabled( m_table.rowCount() > 0 ) ;
		m_ui.pbPLCancel->setEnabled( false ) ;
		m_ui.pbPLDownload->setEnabled( m_table.rowCount() > 0 ) ;
	}
}

void playlistdownloader::tabExited()
{
	m_ui.lineEditPLUrlOptions->setText( m_settings.lastUsedOption( settings::tabName::playlist ) ) ;
}

void playlistdownloader::updateEnginesList( const QStringList& e )
{
	auto& comboBox = *m_ui.cbEngineTypePD ;

	comboBox.clear() ;

	for( const auto& it : e ){

		const auto& engine = m_ctx.Engines().getEngineByName( it ) ;

		if( engine ){

			if( engine->canDownloadPlaylist() ){

				const auto& r = engine->name() ;

				comboBox.addItem( r ) ;
			}
		}
	}

	auto &m = m_ctx.TabManager().batchDownloader() ;

	auto s = settings::tabName::playlist ;

	m.setUpdefaultEngine( comboBox,
			      m_settings.defaultEngine( s ),
			      [ this,s ]( const QString& e ){ m_settings.setDefaultEngine( e,s ) ; } ) ;
}

void playlistdownloader::download()
{
	m_running = true ;

	m_settings.setLastUsedOption( m_ui.lineEditPLUrlOptions->text(),settings::tabName::playlist ) ;

	auto m = m_settings.defaultEngine( settings::tabName::playlist ) ;

	const auto& engine = m_ctx.Engines().defaultEngine( m ) ;

	this->download( engine ) ;
}

void playlistdownloader::download( const engines::engine& engine )
{
	m_playlistEntry.clear() ;

	auto m = m_ui.lineEditPLDownloadRange->text() ;

	auto _add = [ & ]( int s ){

		auto e = m_table.item( s,2 )->text() ;

		if( !concurrentDownloadManagerFinishedStatus::finishedWithSuccess( e ) ){

			m_playlistEntry.emplace_back( s ) ;
		}
	} ;

	if( m.isEmpty() ){

		int count = m_table.rowCount() ;

		for( int i = 0 ; i < count ; i++ ){

			_add( i ) ;
		}
	}else{
		const auto s = utility::split( m,',',true ) ;

		for( const auto& it : s ){

			if( it.contains( "-" ) ){

				const auto ss = utility::split( it,'-',true ) ;

				if( ss.size() == 2 ){

					bool ok ;
					bool ok1 ;
					auto a = ss.at( 0 ).toInt( &ok ) ;
					auto b = ss.at( 1 ).toInt( &ok1 ) ;

					if( ok && ok1 ){

						for(  ; a <= b ; a++ ){

							_add( a - 1 ) ;
						}
					}
				}
			}else{
				bool ok ;
				auto e = it.toInt( &ok ) ;

				if( ok ){

					_add( e - 1 ) ;
				}
			}
		}
	}

	if( m_playlistEntry.empty() ){

		return ;
	}

	for( const auto& it : m_playlistEntry ){

		if( it >= m_table.rowCount() ){

			return ;
		}
	}

	m_ctx.TabManager().basicDownloader().hideTableList() ;

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
	auto aa = playlistdownloader::make_options( *m_ui.pbPLCancel,m_ctx,m_ctx.debug(),[ &engine,index,this ]( utility::ProcessExitState e ){

		m_ccmd.monitorForFinished( engine,index,std::move( e ),[ this ]( const engines::engine& engine,int index ){

			this->download( engine,index ) ;

		},[ &engine,this ]( const concurrentDownloadManagerFinishedStatus& f ){

			m_running = !f.allFinished ;

			utility::updateFinishedState( engine,m_settings,m_table,f ) ;
		} ) ;
	} ) ;

	auto m = m_ui.lineEditPLUrlOptions->text() ;

	m_ccmd.download( engine,
			 index,
			 m_table.item( index,1 )->text(),
			 std::move( aa ),
			 make_loggerBatchDownloader( engine.filter( utility::args( m ).quality ),
						     engine,
						     m_ctx.logger(),
						     *m_table.item( index,0 ),
						     utility::concurrentID() ) ) ;
}

void playlistdownloader::getList()
{
	auto url = m_ui.lineEditPLUrl->text() ;

	if( url.isEmpty() ){

		return ;
	}

	m_ctx.TabManager().disableAll() ;

	m_ui.pbPLCancel->setEnabled( true ) ;

	auto m = m_settings.defaultEngine( settings::tabName::playlist ) ;

	const auto& engine = m_ctx.Engines().defaultEngine( m ) ;

	QStringList opts ;

	opts.append( engine.playListIdArguments() ) ;

	auto range = m_ui.lineEditPLDownloadRange->text() ;

	m_ui.lineEditPLDownloadRange->clear() ;

	if( !range.isEmpty() ){

		opts.append( engine.playlistItemsArgument() ) ;
		opts.append( range ) ;
	}

	opts.append( m_ui.lineEditPLUrl->text() ) ;

	auto aa = playlistdownloader::make_options( *m_ui.pbPLCancel,m_ctx,m_ctx.debug(),[ this ]( utility::ProcessExitState ){

		m_running = false ;
		m_ctx.TabManager().enableAll() ;
		m_ui.pbPLCancel->setEnabled( false ) ;
	} ) ;

	m_running = true ;

	auto bb = []( QTableWidget& table,const QString& txt,const QFont& font ){

		auto s = concurrentDownloadManagerFinishedStatus::notStarted() ;
		utility::addItem( table,{ txt,txt,s },font,Qt::AlignCenter ) ;
	} ;

	utility::run( engine,
		      opts,
		      utility::args( m_ui.lineEditPLUrlOptions->text() ).quality,
		      std::move( aa ),
		      make_loggerPlaylistDownloader( m_table,
						     m_ctx.mainWidget().font(),
						     m_ctx.logger(),
						     engine.playListUrlPrefix(),
						     utility::concurrentID(),
						     std::move( bb ) ),
		      utility::make_term_conn( m_ui.pbPLCancel,&QPushButton::clicked ),
		      QProcess::ProcessChannel::StandardOutput ) ;
}

void playlistdownloader::clearScreen()
{
	utility::clear( m_table ) ;

	m_ui.lineEditPLUrlOptions->clear() ;
	m_ui.lineEditPLDownloadRange->clear() ;
	m_ui.lineEditPLUrl->clear() ;
}

void playlistdownloader::EnableAll::operator()( bool e )
{
	if( e ){

		m_tabManager.enableAll() ;
	}else{
		m_tabManager.disableAll() ;
	}
}

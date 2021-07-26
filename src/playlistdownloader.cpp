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
#include "tableWidget.h"
#include "networkAccess.h"
#include "mainwindow.h"

#include <QFileDialog>

class customOptions
{
public:
	customOptions( const Context& ctx,QStringList& opts,QStringList& eOpts )
	{
		QString downloadArchive ;

		this->takeOption( opts,"--max-media-length",m_maxMediaLength ) ;
		this->takeOption( opts,"--min-media-length",m_minMediaLength ) ;
		this->takeOption( opts,"--download-archive",downloadArchive ) ;

		if( downloadArchive.isEmpty() && !eOpts.isEmpty() ){

			this->takeOption( eOpts,"--download-archive",downloadArchive ) ;
		}

		if( !downloadArchive.isEmpty() ){

			if( utility::isRelativePath( downloadArchive ) ){

				downloadArchive = ctx.Settings().downloadFolder() + "/" + downloadArchive ;
			}

			if( QFile::exists( downloadArchive ) ){

				m_file = std::make_unique< QFile >( downloadArchive ) ;

				m_file->open( QIODevice::ReadOnly ) ;
			}
		}
	}
	int maxMediaLength() const
	{
		return engines::engine::functions::timer::toSeconds( m_maxMediaLength ) ;
	}
	int minMediaLength() const
	{
		return engines::engine::functions::timer::toSeconds( m_minMediaLength ) ;
	}
	bool contains( const QString& e ) const
	{
		if( m_file && m_file->isOpen() ){

			QTextStream txt( m_file.get() ) ;

			while( !txt.atEnd() ){

				auto s = txt.readLine() ;

				if( s.contains( e ) ){

					return true ;
				}
			}
		}

		return false ;
	}
private:
	void takeOption( QStringList& opts,const QString& opt,QString& value )
	{
		for( int i = 0 ; i < opts.size() ; i++ ){

			if( opts[ i ] == opt ){

				if( i + 1 < opts.size() ){

					value = opts[ i + 1 ] ;
					opts.removeAt( i + 1 ) ;
				}

				opts.removeAt( i ) ;

				break ;
			}
		}
	}
	QString m_maxMediaLength ;
	QString m_minMediaLength ;
	std::unique_ptr< QFile > m_file ;
};

playlistdownloader::playlistdownloader( Context& ctx ) :
	m_ctx( ctx ),
	m_settings( m_ctx.Settings() ),
	m_ui( m_ctx.Ui() ),
	m_mainWindow( m_ctx.mainWidget() ),
	m_tabManager( m_ctx.TabManager() ),
	m_table( *m_ui.tableWidgetPl,m_ctx.mainWidget().font(),1 ),
	m_showThumbnails( true ),
	m_ccmd( m_ctx,*m_ui.pbPLCancel,m_settings )
{	
	this->resetMenu() ;

	if( m_showThumbnails ){

		m_table.get().setColumnWidth( 0,m_ctx.Settings().thumbnailWidth( settings::tabName::playlist ) ) ;

		m_table.hideColumns( 2,3,4 ) ;
	}else{
		m_table.hideColumns( 0,2,3,4 ) ;
	}

	m_table.connect( &QTableWidget::currentItemChanged,[ this ]( QTableWidgetItem * c,QTableWidgetItem * p ){

		m_table.selectRow( c,p,m_table.startPosition() ) ;
	} ) ;

	m_table.connect( &QTableWidget::customContextMenuRequested,[ this ]( QPoint ){

		auto row = m_table.currentRow() ;

		auto function = [ this ]( const utility::contextState& c ){

			if( c.showLogWindow() ){

				m_ctx.logger().showLogWindow() ;

			}else if( c.clear() ){

				m_table.clear() ;
			}
		} ;

		if( row == -1 ){

			QMenu m ;

			return utility::appendContextMenu( m,this->enabled(),std::move( function ) ) ;
		}

		auto txt = m_table.runningState( row ) ;

		auto running = downloadManager::finishedStatus::running( txt ) ;
		auto finishSuccess = downloadManager::finishedStatus::finishedWithSuccess( txt ) ;

		QMenu m ;

		auto ac = m.addAction( tr( "Open" ) ) ;

		ac->setEnabled( finishSuccess ) ;

		connect( ac,&QAction::triggered,[ this,row ](){

			auto m = m_ui.cbEngineTypePD->currentText() ;
			m_ctx.Engines().openUrls( m_table,row,m ) ;
		} ) ;

		ac = m.addAction( tr( "Cancel" ) ) ;
		ac->setEnabled( running ) ;

		connect( ac,&QAction::triggered,[ this,row ](){

			m_terminator.terminate( row ) ;
		} ) ;

		ac = m.addAction( tr( "Remove" ) ) ;

		ac->setEnabled( m_table.noneAreRunning() && !m_networkRunning ) ;

		connect( ac,&QAction::triggered,[ this,row ](){

			m_table.removeRow( row ) ;

			m_ui.pbBDDownload->setEnabled( m_table.rowCount() ) ;
		} ) ;

		ac = m.addAction( tr( "Download" ) ) ;
		ac->setEnabled( !running && !finishSuccess ) ;

		connect( ac,&QAction::triggered,[ this,row ](){

			downloadManager::index indexes( m_table ) ;

			auto e = m_table.runningState( row ) ;

			if( !downloadManager::finishedStatus::finishedWithSuccess( e ) ){

				auto u = m_table.downloadingOptions( row ) ;

				if( u.isEmpty() ){

					indexes.add( row,m_ui.lineEditPLUrlOptions->text() ) ;
				}else{
					indexes.add( row,u ) ;
				}
			}

			auto m = m_settings.defaultEngine( settings::tabName::playlist ) ;

			const auto& engine = m_ctx.Engines().defaultEngine( m ) ;

			this->download( engine,std::move( indexes ) ) ;
		} ) ;

		utility::saveDownloadList( m_ctx,m,m_table ) ;

		auto subMenu = utility::setUpMenu( m_ctx,{},false,false,true,&m ) ;

		subMenu->setEnabled( !finishSuccess ) ;

		subMenu->setTitle( QObject::tr( "Preset Options" ) ) ;

		connect( subMenu,&QMenu::triggered,[ this,row ]( QAction * ac ){

			auto m = utility::split( ac->objectName(),'\n',true ) ;

			if( m.size() > 1 ){

				m_table.setDownloadingOptions( row,m[ 0 ],m[ 1 ] ) ;
			}else{
				m_table.setDownloadingOptions( row,m[ 0 ] ) ;
			}
		} ) ;

		m.addMenu( subMenu ) ;

		m.addSeparator() ;

		utility::appendContextMenu( m,{ this->enabled(),finishSuccess },std::move( function ) ) ;
	} ) ;

	auto s = static_cast< void( QComboBox::* )( int ) >( &QComboBox::activated ) ;

	connect( m_ui.pbPLPasteClipboard,&QPushButton::clicked,[ this ](){

		m_ui.lineEditPLUrl->setText( utility::clipboardText() ) ;
	} ) ;

	connect( m_ui.pbPLCancel,&QPushButton::clicked,[ this ](){

		m_networkRunning = 0 ;

		m_terminator.terminateAll( m_table.get() ) ;
	} ) ;

	connect( m_ui.pbPLOptionsHistory,&QPushButton::clicked,[ this ](){

		auto s = utility::showHistory( *m_ui.lineEditPLUrlOptions,
					       m_settings.getOptionsHistory( settings::tabName::playlist ),
					       m_settings,
					       settings::tabName::playlist ) ;

		if( s ){

			this->download() ;
		}
	} ) ;

	connect( m_ui.pbPLDownload,&QPushButton::clicked,[ this ](){

		this->download() ;
	} ) ;

	connect( m_ui.pbPLRangeHistory,&QPushButton::clicked,[ this ](){

		utility::showHistory( *m_ui.lineEditPLDownloadRange,
				      m_settings.playlistRangeHistory(),
				      m_settings,
				      settings::tabName::playlist,
				      utility::PlayListButtonName::DownloadRange ) ;
	} ) ;

	connect( m_ui.pbPLRecentlyUsedUrl,&QPushButton::clicked,[ this ](){

		utility::showHistory( *m_ui.lineEditPLUrl,
				      m_settings.playlistUrlHistory(),
				      m_settings,
				      settings::tabName::playlist,
				      utility::PlayListButtonName::PlaylistUrl ) ;
	} ) ;

	m_table.connect( &QTableWidget::cellDoubleClicked,[ this ]( int row,int column ){

		Q_UNUSED( column )

		m_ctx.Engines().openUrls( m_table,
					  row,
					  m_ui.cbEngineTypePD->currentText() ) ;
	} ) ;

	connect( m_ui.cbEngineTypePD,s,[ & ]( int s ){

		if( s != -1 ){

			m_ui.lineEditPLUrlOptions->clear() ;
			auto m = m_ui.cbEngineTypePD->itemText( s ) ;

			m_settings.setDefaultEngine( m,settings::tabName::playlist ) ;

			m_ui.lineEditPLUrlOptions->setText( m_settings.lastUsedOption( m,settings::tabName::playlist ) ) ;
		}
	} ) ;

	connect( m_ui.pbPLCancel,&QPushButton::clicked,[ this ](){

		m_ccmd.cancelled() ;
	} ) ;

	connect( m_ui.pbPLGetList,&QPushButton::clicked,[ this ](){

		this->getList() ;
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
	m_ui.pbPLRecentlyUsedUrl->setEnabled( true ) ;
	m_ui.pbPLPasteClipboard->setEnabled( true ) ;
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
	m_ui.pbPLRangeHistory->setEnabled( true ) ;
	m_ui.pbPLOptionsHistory->setEnabled( true ) ;
}

void playlistdownloader::disableAll()
{
	m_ui.pbPLRecentlyUsedUrl->setEnabled( false ) ;
	m_ui.pbPLOptionsHistory->setEnabled( false ) ;
	m_ui.pbPLRangeHistory->setEnabled( false ) ;
	m_ui.pbPLPasteClipboard->setEnabled( false ) ;
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
	utility::setMenuOptions( m_ctx,{},false,true,m_ui.pbPLOptions,[ this ]( QAction * aa ){

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
	if( m_table.noneAreRunning() ){

		m_ui.pbPLOptions->setEnabled( m_table.rowCount() > 0 ) ;
		m_ui.pbPLCancel->setEnabled( false ) ;
		m_ui.pbPLDownload->setEnabled( m_table.rowCount() > 0 ) ;
	}

	auto m = m_ui.cbEngineTypePD->currentText() ;

	m_ui.lineEditPLUrlOptions->setText( m_settings.lastUsedOption( m,settings::tabName::playlist ) ) ;

	m_ui.lineEditPLUrl->setFocus() ;
}

void playlistdownloader::tabExited()
{
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
	m_settings.setLastUsedOption( m_ui.cbEngineTypePD->currentText(),
				      m_ui.lineEditPLUrlOptions->text(),
				      settings::tabName::playlist ) ;

	auto m = m_settings.defaultEngine( settings::tabName::playlist ) ;

	const auto& engine = m_ctx.Engines().defaultEngine( m ) ;

	this->download( engine ) ;
}

void playlistdownloader::download( const engines::engine& engine,downloadManager::index indexes )
{
	if( indexes.empty() ){

		return ;
	}

	m_ctx.TabManager().basicDownloader().hideTableList() ;

	m_ctx.mainWindow().setTitle( QString() ) ;

	m_ccmd.download( std::move( indexes ),engine,[ this ](){

		return m_settings.maxConcurrentDownloads() ;

	}(),[ this ]( const engines::engine& engine,int index ){

		this->download( engine,index ) ;
	} ) ;
}

void playlistdownloader::download( const engines::engine& engine )
{
	downloadManager::index indexes( m_table ) ;

	auto m = m_ui.lineEditPLDownloadRange->text() ;
	
	auto _add = [ & ]( int s,const QString& opts ){

		auto e = m_table.runningState( s ) ;

		if( !downloadManager::finishedStatus::finishedWithSuccess( e ) ){

			if( s >= 0 && s < m_table.rowCount() ){

				auto u = m_table.downloadingOptions( s ) ;

				if( u.isEmpty() ){

					indexes.add( s,opts ) ;
				}else{
					indexes.add( s,u ) ;
				}
			}
		}
	} ;

	auto opts = m_ui.lineEditPLUrlOptions->text() ;

	if( m.isEmpty() ){

		int count = m_table.rowCount() ;

		for( int i = 0 ; i < count ; i++ ){

			_add( i,opts ) ;
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

							_add( a - 1,opts ) ;
						}
					}
				}
			}else{
				bool ok ;
				auto e = it.toInt( &ok ) ;

				if( ok ){

					_add( e - 1,opts ) ;
				}
			}
		}
	}

	this->download( engine,std::move( indexes ) ) ;
}

void playlistdownloader::download( const engines::engine& engine,int index )
{
	auto aa = [ &engine,index,this ]( utility::ProcessExitState e,const playlistdownloader::opts& ){

		auto aa = [ this ]( const engines::engine& engine,int index ){

			this->download( engine,index ) ;
		} ;

		auto bb = [ &engine,index,this ]( const downloadManager::finishedStatus& f ){

			utility::updateFinishedState( engine,m_settings,m_table,f ) ;

			if( m_table.noneAreRunning() ){

				m_ctx.TabManager().enableAll() ;
			}

			m_ctx.mainWindow().setTitle( m_table.completeProgress( index ) ) ;
		} ;

		m_ccmd.monitorForFinished( engine,index,std::move( e ),std::move( aa ),std::move( bb ) ) ;
	} ;

	auto functions = utility::OptionsFunctions( []( const playlistdownloader::opts& ){},std::move( aa ) ) ;

	auto m = m_ui.lineEditPLUrlOptions->text() ;

	m_settings.addOptionsHistory( m,settings::tabName::playlist ) ;

	m_ccmd.download( engine,
			 m_table.runningStateItem( index ),
			 m_table.url( index ),
			 m_terminator.setUp(),
			 playlistdownloader::make_options( { m_ctx,m_ctx.debug(),false,index },std::move( functions ) ),
			 make_loggerBatchDownloader( engine.filter( utility::args( m ).quality() ),
						     engine,
						     m_ctx.logger(),
						     m_table.uiTextItem( index ),
						     utility::concurrentID() ) ) ;
}

void playlistdownloader::getList()
{
	auto url = m_ui.lineEditPLUrl->text() ;

	if( url.isEmpty() ){

		return ;
	}

	auto options = utility::split( url,' ',true ) ;

	options.append( utility::split( m_ui.lineEditPLUrlOptions->text(),' ',true ) ) ;

	m_ui.pbPLCancel->setEnabled( true ) ;

	auto m = m_settings.defaultEngine( settings::tabName::playlist ) ;

	const auto& engine = m_ctx.Engines().defaultEngine( m ) ;

	QStringList opts ;

	opts.append( "--dump-json" ) ;

	auto range = m_ui.lineEditPLDownloadRange->text() ;

	m_settings.addToplaylistRangeHistory( range ) ;
	m_settings.addToplaylistUrlHistory( url ) ;

	m_ui.lineEditPLDownloadRange->clear() ;

	if( !range.isEmpty() ){

		if( range.startsWith( "--" ) ){

			opts.append( utility::split( range,' ',true ) ) ;
		}else{
			opts.append( engine.playlistItemsArgument() ) ;

			auto m = utility::split( range,' ',true ) ;

			opts.append( m ) ;
		}
	}

	opts.append( options.first() ) ;

	auto functions = utility::OptionsFunctions( [ this ]( const playlistdownloader::opts& opts ){

			opts.ctx.TabManager().disableAll() ;

			m_ui.pbPLCancel->setEnabled( true ) ;

		},[ this ]( utility::ProcessExitState,const playlistdownloader::opts& ){

			m_ctx.TabManager().enableAll() ;
			m_ui.pbPLCancel->setEnabled( false ) ;
		}
	) ;

	m_networkRunning = 0 ;

	auto bb = [ copts = customOptions( m_ctx,opts,options ),this ]( tableWidget& table,Logger::Data& data ){

		this->parseJson( std::move( copts ),table,data ) ;
	} ;

	utility::run( engine,
		      opts,
		      utility::args( m_ui.lineEditPLUrlOptions->text() ).quality(),
		      playlistdownloader::make_options( { m_ctx,m_ctx.debug(),false,-1 },std::move( functions ) ),
		      make_loggerPlaylistDownloader( m_table,
						     m_ctx.logger(),
						     engine.playListUrlPrefix(),
						     utility::concurrentID(),
						     std::move( bb ) ),
		      m_terminator.setUp( m_ui.pbPLCancel,&QPushButton::clicked,-1 ),
		      QProcess::ProcessChannel::StandardOutput ) ;
}

void playlistdownloader::clearScreen()
{
	m_table.clear() ;

	m_ui.lineEditPLUrlOptions->clear() ;
	m_ui.lineEditPLDownloadRange->clear() ;
	m_ui.lineEditPLUrl->clear() ;
}

bool playlistdownloader::enabled()
{
	return m_ui.lineEditPLUrl->isEnabled() ;
}

void playlistdownloader::parseJson( const customOptions& copts,tableWidget& table,Logger::Data& data )
{
	auto mmm = data.toLine() ;

	auto oo = mmm.indexOf( '{' ) ;

	if( oo == -1 ){

		return ;
	}

	int counter = 0 ;
	int index = 0 ;

	while( true ){

		if( index >= mmm.size() ){

			auto m = QString::number( index + 1 ) + " bytes" ;

			return ;
		}

		auto a = mmm[ index ] ;

		if( a == '{' ){

			counter++ ;

		}else if( a == '}' ){

			counter-- ;

			if( counter == 0 ){

				break ;
			}
		}

		index++ ;
	}

	auto aa = mmm.mid( oo,index + 1 ) ;

	utility::MediaEntry media( aa.toUtf8() ) ;

	if( !media.valid() ){

		return ;
	}else{
		data.clear() ;
		data.add( mmm.mid( index + 1 ) ) ;
	}

	if( copts.contains( media.id() ) ){

		auto m = tr( "Skipping a media with an id of \"%1\" because it is already in the archive file." ).arg( media.id() ) ;
		m_ctx.logger().add( m ) ;
		return ;
	}

	auto max = copts.maxMediaLength() ;

	if( max > 0 && media.intDuration() > max ){

		return ;
	}

	auto min = copts.minMediaLength() ;

	if( min > 0 && media.intDuration() < min ){

		return ;
	}

	settings& ss = m_ctx.Settings() ;

	auto width = ss.thumbnailWidth( settings::tabName::playlist ) ;
	auto height = ss.thumbnailHeight( settings::tabName::playlist ) ;

	auto s = downloadManager::finishedStatus::notStarted() ;

	table.selectLast() ;

	if( !m_showThumbnails ){

		auto pixmap = QIcon( ":/video" ).pixmap( width,height ) ;

		table.addItem( pixmap,{ media.uiText(),media.url(),s },Qt::AlignCenter ) ;

		table.selectLast() ;

		return ;
	}

	if( networkAccess::hasNetworkSupport() ){

		auto& network = m_ctx.TabManager().Configure().network() ;

		m_networkRunning++ ;

		int row = table.addRow() ;

		auto thumbnailUrl = media.thumbnailUrl() ;

		network.getResource( thumbnailUrl,
				     [ this,&table,s,row,width,
				     height,media = std::move( media ) ]( QByteArray data ){

			QPixmap pixmap ;

			if( pixmap.loadFromData( data ) ){

				pixmap = pixmap.scaled( width,height ) ;
			}else{
				pixmap = QIcon( ":/video" ).pixmap( width,height ) ;
			}

			table.replace( pixmap,{ media.uiText(),media.url(),s },row ) ;

			table.selectLast() ;

			m_networkRunning-- ;
		} ) ;
	}else{
		auto pixmap = QIcon( ":/video" ).pixmap( width,height ) ;

		table.addItem( pixmap,{ media.uiText(),media.url(),s },Qt::AlignCenter ) ;

		table.selectLast() ;
	}
}

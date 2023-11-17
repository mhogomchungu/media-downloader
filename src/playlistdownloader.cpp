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
#include <QClipboard>
#include <QMetaObject>

playlistdownloader::playlistdownloader( Context& ctx ) :
	m_ctx( ctx ),
	m_settings( m_ctx.Settings() ),
	m_ui( m_ctx.Ui() ),
	m_mainWindow( m_ctx.mainWidget() ),
	m_tabManager( m_ctx.TabManager() ),
	m_table( *m_ui.tableWidgetPl,m_ctx.mainWidget().font(),1,m_settings.textAlignment() ),
	m_subscriptionTable( *m_ui.tableWidgetPlDownloaderSubscription,m_ctx.mainWidget().font() ),
	m_ccmd( m_ctx,*m_ui.pbPLCancel,m_settings ),
	m_defaultVideoThumbnailIcon( m_settings.defaultVideoThumbnailIcon( settings::tabName::playlist ) ),
	m_banner( m_table ),
	m_subscription( m_ctx,m_subscriptionTable,*m_ui.widgetPlDownloader )
{
	qRegisterMetaType< PlNetworkData >() ;

	m_ui.pbPLPasteClipboard->setIcon( QIcon( ":/clipboard" ) ) ;
	m_ui.pbPLOptionsHistory->setIcon( QIcon( ":/recentlyUsed" ) ) ;
	m_ui.pbPLRangeHistory->setIcon( QIcon( ":/recentlyUsed" ) ) ;
	m_ui.pbPLDownloadOptions->setIcon( QIcon( ":/downloadOptions" ) ) ;
	m_ui.pbPlSubscription->setIcon( QIcon( ":/subscription" ) ) ;
	m_ui.pbClearArchiveFile->setIcon( QIcon( ":/clearFile" ) ) ;

	m_ui.pbPLChangeTableSize->setToolTip( tr( "Hide Controls" ) ) ;

	m_ui.tableWidgetPlDownloaderSubscription->setColumnWidth( 0,180 ) ;

	m_ui.labelPlSubscriptionListOptions->setText( tr( "Get List Options:" ).replace( ":","" ) ) ;

	m_subscription.setVisible( false ) ;

	this->resetMenu() ;

	connect( m_ui.pbClearArchiveFile,&QPushButton::clicked,[ this ](){

		auto m = m_ctx.Engines().engineDirPaths().subscriptionsArchiveFilePath() ;
		QFile f( m ) ;
		f.open( QIODevice::WriteOnly | QIODevice::Truncate ) ;
		f.write( "" ) ;
	} ) ;

	m_ui.cbUseInternalArchiveFile->setChecked( m_ctx.Settings().useInternalArchiveFile() ) ;

	connect( m_ui.cbUseInternalArchiveFile,&QCheckBox::toggled,[ this ]( bool e ){

		m_ctx.Settings().setUseInternalArchiveFile( e ) ;
	} ) ;

	m_ui.lineEditPLDownloadRange->setText( m_settings.playlistRangeHistoryLastUsed() ) ;

	connect( m_ui.pbPlSubscriptionDone,&QPushButton::clicked,[ this ](){

		m_ui.widgetPlDownloader->setVisible( false ) ;
	} ) ;

	connect( m_ui.pbPlSubscriptionAdd,&QPushButton::clicked,[ this ](){

		auto uiText = m_ui.lineEditPlSubscriptionUiName->text() ;
		auto url = m_ui.lineEditPlSubscriptionUrl->text() ;
		auto opts = m_ui.lineEditPlSubscriptionGetListOptions->text() ;

		if( !uiText.isEmpty() && !url.isEmpty() ){

			m_subscription.add( uiText,url,opts ) ;

			m_ui.lineEditPlSubscriptionUiName->clear() ;
			m_ui.lineEditPlSubscriptionUrl->clear() ;
			m_ui.lineEditPlSubscriptionGetListOptions->clear() ;

			m_ui.lineEditPlSubscriptionUiName->setFocus() ;
		}
	} ) ;

	connect( m_ui.pbPLDownloadOptions,&QPushButton::clicked,[ this ](){

		auto& t = m_ctx.TabManager().Configure() ;

		t.engineDefaultDownloadOptions( this->defaultEngineName(),[ this ]( const QString& e ){

			for( int i = 1 ; i < m_table.rowCount() ; i++ ){

				auto u = tableWidget::type::DownloadExtendedOptions ;

				m_table.setDownloadingOptions( u,i,e ) ;
			}
		} ) ;
	} ) ;

	m_table.get().setColumnWidth( 0,m_ctx.Settings().thumbnailWidth( settings::tabName::playlist ) ) ;

	m_table.connect( &QTableWidget::currentItemChanged,[ this ]( QTableWidgetItem * c,QTableWidgetItem * p ){

		m_table.selectRow( c,p,m_table.startPosition() ) ;
	} ) ;

	m_subscriptionTable.connect( &QTableWidget::currentItemChanged,[ this ]( QTableWidgetItem * c,QTableWidgetItem * p ){

		m_subscriptionTable.selectRow( c,p,0 ) ;
	} ) ;

	m_subscriptionTable.connect( &QTableWidget::customContextMenuRequested,[ this ]( QPoint ){

		auto row = m_subscriptionTable.currentRow() ;

		if( row != -1 ){

			QMenu m ;

			connect( m.addAction( tr( "Remove" ) ),&QAction::triggered,[ this,row ](){

				m_subscription.remove( row ) ;
			} ) ;

			m.exec( QCursor::pos() ) ;
		}
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

		QMenu m ;

		if( row == -1 || !m_table.rowIsVisible( row ) ){

			auto ss = this->enabled() ;

			return utility::appendContextMenu( m,ss,function,false,row,m_table ) ;
		}

		auto txt = m_table.runningState( row ) ;

		if( txt.isEmpty() ){

			auto ss = this->enabled() ;

			return utility::appendContextMenu( m,ss,function,false,row,m_table ) ;
		}

		auto running = downloadManager::finishedStatus::running( txt ) ;
		auto finishSuccess = downloadManager::finishedStatus::finishedWithSuccess( txt ) ;

		auto ac = m.addAction( tr( "Open" ) ) ;

		ac->setEnabled( finishSuccess ) ;

		connect( ac,&QAction::triggered,[ this,row ](){

			const auto& engine = utility::resolveEngine( m_table,
								     this->defaultEngine(),
								     m_ctx.Engines(),
								     row ) ;

			m_ctx.Engines().openUrls( m_table,row,engine ) ;
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

		if( m_settings.autoHideDownloadWhenCompleted() ){

			utility::hideUnhideEntries( m,m_table,row,true ) ;
		}

		ac = m.addAction( tr( "Copy Url" ) ) ;

		connect( ac,&QAction::triggered,[ this,row ](){

			auto m = QApplication::clipboard() ;

			if( m ){

				m->setText( m_table.url( row ) ) ;
			}
		} ) ;

		const auto& engine = utility::resolveEngine( m_table,this->defaultEngine(),m_ctx.Engines(),row ) ;

		ac = m.addAction( tr( "Show Comments" ) ) ;
		ac->setEnabled( engine.supportShowingComments() ) ;

		connect( ac,&QAction::triggered,[ this,&engine ](){

			auto row = m_table.currentRow() ;

			if( row != -1 ){

				m_ctx.TabManager().batchDownloader().showComments( engine,m_table.url( row ) ) ;
			}
		} ) ;

		utility::addDownloadContextMenu( running,finishSuccess,m,row,[ this ]( int row ){

			auto m = m_table.uiText( row ) ;

			return m.startsWith( engines::engine::mediaAlreadInArchiveText() + "\n" ) ;

		},[ this,&engine ]( QAction * ac,bool forceDownload,int row ){

			connect( ac,&QAction::triggered,[ &engine,this,row,forceDownload ](){

				auto visible = m_table.rowIsVisible( row ) ;

				downloadManager::index indexes( m_table,downloadManager::index::tab::playlist ) ;

				auto e = m_table.runningState( row ) ;

				if( visible && ( !downloadManager::finishedStatus::finishedWithSuccess( e ) || forceDownload ) ){

					auto u = m_table.downloadingOptions( row ) ;

					if( u.isEmpty() ){

						auto m = m_ui.lineEditPLUrlOptions->text() ;

						auto mm = utility::setDownloadOptions( engine,m_table,row,m ) ;

						indexes.add( row,mm.move() ) ;
					}else{
						auto uu = utility::setDownloadOptions( engine,m_table,row,u ) ;

						indexes.add( row,uu.move() ) ;
					}
				}

				this->download( this->defaultEngine(),indexes.move() ) ;
			} ) ;
		} ) ;

		utility::saveDownloadList( m_ctx,m,m_table,true ) ;

		auto mm = m.addMenu( utility::stringConstants::engineName().replace( ":","" ) ) ;

		mm->setEnabled( !finishSuccess ) ;

		for( const auto& it : m_ctx.Engines().getEngines() ){

			if( it.mainEngine() ){

				const auto& e = it.name() ;

				mm->addAction( e )->setObjectName( e ) ;
			}
		}

		connect( mm,&QMenu::triggered,[ this ]( QAction * ac ){

			auto u = tableWidget::type::EngineName ;

			m_table.setDownloadingOptions( u,m_table.currentRow(),ac->objectName() ) ;
		} ) ;

		auto subMenu = utility::setUpMenu( m_ctx,{},false,false,true,&m ) ;

		subMenu->setEnabled( !finishSuccess ) ;

		subMenu->setTitle( QObject::tr( "Preset Options" ) ) ;

		connect( subMenu,&QMenu::triggered,[ this,row ]( QAction * ac ){

			auto m = util::split( ac->objectName(),'\n',true ) ;

			auto u = tableWidget::type::DownloadOptions ;

			if( m.size() > 1 ){

				m_table.setDownloadingOptions( u,row,m[ 0 ],m[ 1 ] ) ;
			}else{
				m_table.setDownloadingOptions( u,row,m[ 0 ] ) ;
			}
		} ) ;

		m.addMenu( subMenu ) ;

		m.addSeparator() ;

		utility::appendContextMenu( m,{ this->enabled(),finishSuccess },function,true ) ;
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

		m_banner.clear() ;
		this->download() ;
	} ) ;

	connect( m_ui.pbPLChangeTableSize,&QPushButton::clicked,[ this ](){

		this->resizeTable( playlistdownloader::size::toggle ) ;
	} ) ;

	connect( m_ui.pbPLRangeHistory,&QPushButton::clicked,[ this ](){

		utility::showHistory( *m_ui.lineEditPLDownloadRange,
				      m_settings.playlistRangeHistory(),
				      m_settings,
				      settings::tabName::playlist,
				      utility::PlayListButtonName::DownloadRange ) ;
	} ) ;

	connect( m_ui.pbPlSubscription,&QPushButton::clicked,[ this ](){

		m_autoDownload = false ;

		QMenu m ;

		auto entries = m_subscription.entries() ;

		for( auto it = entries.rbegin() ; it != entries.rend() ; it++ ){

			const auto& s = *it ;

			m.addAction( s.uiName )->setObjectName( s.url ) ;
		}

		m.addSeparator() ;

		bool enable = entries.size() > 0 ;

		auto ac = m.addAction( tr( "Show All Updated" ) ) ;

		ac->setObjectName( "Show All Updated" ) ;

		ac->setEnabled( enable ) ;

		ac = m.addAction( tr( "Download All Updated" ) ) ;

		ac->setObjectName( "Download All Updated" ) ;

		ac->setEnabled( enable ) ;

		m.addSeparator() ;

		m.addAction( tr( "Manage Subscriptions" ) )->setObjectName( "Manage Subscriptions" ) ;

		QObject::connect( &m,&QMenu::triggered,[ this,entries = std::move( entries ) ]( QAction * ac )mutable{

			auto s = ac->objectName() ;

			const auto& engine = this->defaultEngine() ;

			if( s == "Download All Updated" ){

				m_autoDownload = true ;

				this->getListing( std::move( entries ),engine ) ;

			}else if( s == "Show All Updated" ){

				this->getListing( std::move( entries ),engine ) ;

			}else if( s == "Manage Subscriptions" ){

				m_ui.lineEditPlSubscriptionUiName->setFocus() ;

				m_subscription.setVisible( true ) ;
			}else{
				for( const auto& it : entries ){

					if( it.url == s ){

						std::vector< subscription::entry > ss ;

						ss.emplace_back( it.uiName,it.url,it.getListOptions ) ;

						this->getListing( std::move( ss ),engine ) ;

						break ;
					}
				}
			}
		} ) ;

		m.exec( QCursor::pos() ) ;
	} ) ;

	m_table.connect( &QTableWidget::cellDoubleClicked,[ this ]( int row,int column ){

		Q_UNUSED( column )

		const auto& engine = utility::resolveEngine( m_table,
							     this->defaultEngine(),
							     m_ctx.Engines(),
							     row ) ;

		m_ctx.Engines().openUrls( m_table,row,engine ) ;
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

		auto m = m_ui.lineEditPLUrl->text() ;

		if( !m.isEmpty() ){

			m_autoDownload = false ;

			std::vector< subscription::entry > ss ;

			for( const auto& it : util::split( m,' ',true ) ){

				ss.emplace_back( it ) ;
			}

			this->getListing( std::move( ss ),this->defaultEngine() ) ;
		}
	} ) ;

	connect( m_ui.pbPLQuit,&QPushButton::clicked,[ this ](){

		m_ctx.mainWindow().quitApp() ;
	} ) ;
}

void playlistdownloader::init_done()
{
	m_banner.setBanner( tr( "This May Take A Very Long Time" ) ) ;
}

void playlistdownloader::enableAll()
{
	m_ui.pbClearArchiveFile->setEnabled( true ) ;
	m_ui.cbUseInternalArchiveFile->setEnabled( true ) ;
	m_ui.pbPlSubscription->setEnabled( true ) ;
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
	m_ui.pbPLDownloadOptions->setEnabled( true ) ;
}

void playlistdownloader::disableAll()
{
	m_ui.pbPLDownloadOptions->setEnabled( false ) ;
	m_ui.pbClearArchiveFile->setEnabled( false ) ;
	m_ui.cbUseInternalArchiveFile->setEnabled( false ) ;
	m_ui.pbPlSubscription->setEnabled( false ) ;
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

			if( m_settings.autoDownload() ){

				this->download() ;
			}
		}
	} ) ;
}

void playlistdownloader::retranslateUi()
{
	this->resetMenu() ;
}

void playlistdownloader::tabEntered()
{
	if( m_table.noneAreRunning() && !m_gettingPlaylist ){

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

void playlistdownloader::exiting()
{
	utility::saveDownloadList( m_ctx,m_table,false ) ;
}

void playlistdownloader::gotEvent( const QJsonObject& )
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

	auto s = settings::tabName::playlist ;

	utility::setUpdefaultEngine( comboBox,
				     this->defaultEngineName(),
				     [ this,s ]( const QString& e ){ m_settings.setDefaultEngine( e,s ) ; } ) ;
}

void playlistdownloader::clipboardData( const QString& )
{
}

QString playlistdownloader::defaultEngineName()
{
	return m_settings.defaultEngine( settings::tabName::playlist,m_ctx.Engines().defaultEngineName() ) ;
}

const engines::engine& playlistdownloader::defaultEngine()
{
	auto id = utility::concurrentID() ;

	return m_ctx.Engines().defaultEngine( this->defaultEngineName(),id ) ;
}

void playlistdownloader::download()
{
	m_settings.setLastUsedOption( m_ui.cbEngineTypePD->currentText(),
				      m_ui.lineEditPLUrlOptions->text(),
				      settings::tabName::playlist ) ;

	this->download( this->defaultEngine() ) ;
}

void playlistdownloader::download( const engines::engine& engine,downloadManager::index indexes )
{
	if( indexes.empty() ){

		return ;
	}

	m_ctx.logger().clear() ;

	m_ctx.logger().setMaxProcessLog( m_table.rowCount() ) ;

	m_ctx.TabManager().basicDownloader().hideTableList() ;

	m_ctx.mainWindow().setTitle( QString() ) ;

	m_ccmd.download( indexes.move(),engine,[ this ](){

		return m_settings.maxConcurrentDownloads() ;

	}(),[ this ]( const engines::engine& engine,int index ){

		this->download( engine,index ) ;
	} ) ;
}

void playlistdownloader::download( const engines::engine& engine )
{
	downloadManager::index indexes( m_table,downloadManager::index::tab::playlist ) ;

	auto _add = [ & ]( int s,const QString& opts ){

		auto validUrl = !m_table.url( s ).isEmpty() ;

		auto e = m_table.runningState( s ) ;

		auto visible = m_table.rowIsVisible( s ) ;

		if( visible && validUrl && !downloadManager::finishedStatus::finishedWithSuccess( e ) ){

			if( s >= 0 && s < m_table.rowCount() ){

				auto u = m_table.downloadingOptions( s ) ;

				if( u.isEmpty() ){

					auto oo = utility::setDownloadOptions( engine,m_table,s,opts ) ;

					indexes.add( s,oo.move() ) ;
				}else{
					auto uu = utility::setDownloadOptions( engine,m_table,s,u ) ;

					indexes.add( s,uu.move() ) ;
				}
			}
		}
	} ;

	auto opts = m_ui.lineEditPLUrlOptions->text() ;

	int count = m_table.rowCount() ;

	for( int i = 0 ; i < count ; i++ ){

		_add( i,opts ) ;
	}

	this->download( engine,indexes.move() ) ;
}

void playlistdownloader::download( const engines::engine& eng,int index )
{
	class events
	{
	public:
		events( playlistdownloader& p,const engines::engine& engine,int index ) :
			m_parent( p ),m_engine( engine ),m_index( index )
		{
		}
		void done( engines::ProcessExitState e,const playlistdownloader::opts& )
		{
			m_parent.m_ccmd.monitorForFinished( m_engine,m_index,e,event( m_parent,m_engine ) ) ;
		}
		void disableAll( const playlistdownloader::opts& )
		{
		}
		void list( const engines::ProcessExitState&,const QByteArray& )
		{
		}
		events move()
		{
			return std::move( *this ) ;
		}
	private:
		class event
		{
		public:
			event( playlistdownloader& p,const engines::engine& engine ) :
				m_parent( p ),m_engine( engine )
			{
			}
			void next( const engines::engine& engine,int index )
			{
				m_parent.m_banner.updateTimer() ;
				m_parent.download( engine,index ) ;
			}
			void finished( const downloadManager::finishedStatus& f )
			{
				reportFinished r( m_engine,f ) ;

				auto a = "reportFinishedStatus" ;
				auto b = Qt::QueuedConnection ;

				QMetaObject::invokeMethod( &m_parent,a,b,Q_ARG( reportFinished,r.move() ) ) ;
			}
		private:
			playlistdownloader& m_parent ;
			const engines::engine& m_engine ;
		} ;

		playlistdownloader& m_parent ;
		const engines::engine& m_engine ;
		int m_index ;
	} ;

	auto m = m_ui.lineEditPLUrlOptions->text() ;

	m_settings.addOptionsHistory( m,settings::tabName::playlist ) ;

	auto updater = [ this,index ]( const QByteArray& e ){

		QMetaObject::invokeMethod( this,
					   "addTextToUi",
					   Qt::QueuedConnection,
					   Q_ARG( QByteArray,e ),
					   Q_ARG( int,index ) ) ;
	} ;

	auto error = []( const QByteArray& ){} ;

	const auto& engine = utility::resolveEngine( m_table,eng,m_ctx.Engines(),index ) ;

	auto id = utility::concurrentID() ;
	playlistdownloader::opts opts{ m_ctx,m_ctx.debug(),false,index } ;
	auto oopts  = playlistdownloader::make_options( engine,opts,events( *this,engine,index ) ) ;
	auto logger = make_loggerBatchDownloader( engine.filter( id ),m_ctx.logger(),updater,error,id ) ;

	m_table.setRunningState( downloadManager::finishedStatus::running(),index ) ;

	auto optsUpdater = [ this ]( QStringList opts ){

		if( m_ctx.Settings().useInternalArchiveFile() ){

			opts.append( "--download-archive" ) ;
			opts.append( m_subscription.archivePath() ) ;
		}

		return opts ;
	} ;

	m_ccmd.download( engine,
			 optsUpdater,
			 m_ui.lineEditPLUrlOptions->text(),
			 m_table.url( index ),
			 m_ctx,
			 m_terminator.setUp(),
			 oopts.move(),
			 logger.move() ) ;
}

void playlistdownloader::showBanner()
{
	m_table.clear() ;
	m_banner.clear() ;

	QIcon icon( ":/media-downloader" ) ;

	auto w = m_settings.thumbnailWidth( settings::tabName::playlist ) ;
	auto h = m_settings.thumbnailHeight( settings::tabName::playlist ) ;

	tableWidget::entry entry ;

	entry.uiText    = m_banner.txt() ;
	entry.thumbnail = icon.pixmap( w,h ) ;

	m_table.addItem( entry.move() ) ;
	m_table.selectLast() ;

	m_ctx.TabManager().disableAll() ;
}

void playlistdownloader::getListing( playlistdownloader::listIterator e,const engines::engine& engine )
{
	this->resizeTable( playlistdownloader::size::large ) ;

	this->showBanner() ;

	engine.updateVersionInfo( m_ctx,[ this,&engine,e = e.move() ]()mutable{

		this->getList( e.move(),engine ) ;
	} ) ;
}

void playlistdownloader::getList( playlistdownloader::listIterator iter,
				  const engines::engine& engine )
{
	m_dataReceived = false ;
	m_stoppedOnExisting = false ;
	m_downloaderId++ ;

	auto url = iter.url() ;

	url = util::split( url,' ',true ).first() ;

	m_ui.lineEditPLUrl->setText( url ) ;

	m_ui.pbPLCancel->setEnabled( true ) ;

	auto opts = engine.dumpJsonArguments( engines::engine::tab::playlist ) ;

	if( !opts.isEmpty() ){

		opts.last() += m_jsonEndMarker ;
	}

	auto configListOpts = iter.listOptions() ;
	auto listOptions    = m_ui.lineEditPLDownloadRange->text() ;

	if( !listOptions.isEmpty() ){

		m_settings.addToplaylistRangeHistory( listOptions ) ;

		m_settings.setPlaylistRangeHistoryLastUsed( listOptions ) ;

		opts.append( util::split( listOptions,' ',true ) ) ;
	}

	if( !configListOpts.isEmpty() ){

		opts.append( util::split( configListOpts,' ',true ) ) ;
	}

	engine.setTextEncondig( opts ) ;
	engine.updateGetPlaylistCmdOptions( opts ) ;

	opts.append( "-v" ) ;

	auto m = m_ui.lineEditPLUrlOptions->text() ;

	utility::addToListOptionsFromsDownload( opts,m,m_ctx,engine ) ;

	opts.append( url ) ;

	m_networkRunning = 0 ;

	utils::qthread::run( [ &engine,this,opts = std::move( opts ) ]()mutable{

		return customOptions( std::move( opts ),
				      m_subscription.archivePath(),
				      engine,
				      m_ctx ) ;

	},[ this,&engine,iter = iter.move() ]( customOptions c )mutable{

		this->getList( c.move(),engine,iter.move() ) ;
	} ) ;
}

void playlistdownloader::getList( customOptions&& c,
				  const engines::engine& engine,
				  playlistdownloader::listIterator iter )
{
	class events
	{
	public:
		events( playlistdownloader& p,
			const engines::engine& e,
			playlistdownloader::listIterator i ) :
			m_parent( p ),m_engine( e ),m_iter( i.move() )
		{
		}
		void done( engines::ProcessExitState st,const playlistdownloader::opts& )
		{
			if( st.cancelled() ){

				if( m_parent.m_stoppedOnExisting && m_iter.hasNext() ){

					m_parent.getList( m_iter.next(),m_engine ) ;
				}else{
					m_parent.m_ctx.TabManager().enableAll() ;
					m_parent.m_gettingPlaylist = false ;
					m_parent.m_ui.pbPLCancel->setEnabled( false ) ;
				}

			}else if( m_iter.hasNext() ){

				m_parent.getList( m_iter.next(),m_engine ) ;
			}else{
				if( m_parent.m_autoDownload ){

					m_parent.download() ;
				}else{
					m_parent.m_ctx.TabManager().enableAll() ;
					m_parent.m_gettingPlaylist = false ;
					m_parent.m_ui.pbPLCancel->setEnabled( false ) ;
				}
			}
		}
		void disableAll( const playlistdownloader::opts& opts )
		{
			opts.ctx.TabManager().disableAll() ;
			m_parent.m_gettingPlaylist = true ;
			m_parent.m_ui.pbPLCancel->setEnabled( true ) ;
		}
		void list( const engines::ProcessExitState&,const QByteArray& )
		{
		}
		events move()
		{
			return std::move( *this ) ;
		}
	private:
		playlistdownloader& m_parent ;
		const engines::engine& m_engine ;
		playlistdownloader::listIterator m_iter ;
	} ;

	playlistdownloader::opts mmm{ m_ctx,m_ctx.debug(),false,-1 } ;

	auto opts = c.options() ;

	stdOut sOut( *this,c.move() ) ;
	stdError sErr( m_banner ) ;

	events ev( *this,engine,iter.move() ) ;

	auto& ll = m_ctx.logger() ;

	auto id     = utility::concurrentID() ;
	auto oopts  = playlistdownloader::make_options( engine,mmm.move(),ev.move() ) ;
	auto logger = make_loggerPlaylistDownloader( m_table,ll,id,sOut.move(),sErr.move() ) ;
	auto term   = m_terminator.setUp( m_ui.pbPLCancel,&QPushButton::clicked,-1 ) ;
	auto ch     = QProcess::ProcessChannel::StandardOutput ;

	if( !m_gettingPlaylist ){

		logger.clear() ;

		this->showBanner() ;

		m_ctx.logger().setMaxProcessLog( m_table.rowCount() ) ;
	}

	m_table.selectLast() ;

	auto ctx = utility::make_ctx( engine,oopts.move(),logger.move(),term.move(),ch ) ;

	utility::run( opts,{},ctx.move() ) ;
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

bool playlistdownloader::parseJson( const customOptions& copts,
				    tableWidget& table,
				    utility::MediaEntry media )
{
	if( copts.contains( media.id() ) ){

		if( copts.breakOnExisting() ){

			m_stoppedOnExisting = true ;
			m_ui.pbPLCancel->click() ;

			return true ;

		}else if( copts.skipOnExisting() ){

			auto s = downloadManager::finishedStatus::finishedWithSuccess() ;

			const auto& img = m_defaultVideoThumbnailIcon ;

			tableWidget::entry entry{ img,s,media } ;

			entry.uiText = QObject::tr( "Media Already In Archive" ) + "\n" + media.uiText() ;

			this->showEntry( table,entry.move() ) ;

			table.selectLast() ;

			return false ;
		}
	}

	auto max = copts.maxMediaLength() ;

	if( max > 0 && media.intDuration() > max ){

		return false ;
	}

	auto min = copts.minMediaLength() ;

	if( min > 0 && media.intDuration() < min ){

		return false ;
	}

	table.selectLast() ;

	if( networkAccess::hasNetworkSupport() ){

		auto& network = m_ctx.network() ;

		m_networkRunning++ ;

		auto thumbnailUrl = media.thumbnailUrl() ;

		networkCtx m{ media.move(),m_downloaderId,table } ;

		network.get( thumbnailUrl,m.move(),this,&playlistdownloader::networkResult ) ;
	}else{
		utility::networkReply( this,"networkData",&table,m_downloaderId,media.move() ) ;
	}

	return false ;
}

void playlistdownloader::networkResult( networkCtx d,const utils::network::reply& reply )
{
	utility::networkReply( this,"networkData",m_ctx,reply,&d.table,d.id,d.media.move() ) ;
}

void playlistdownloader::networkData( utility::networkReply m )
{
	if( m.id() < m_downloaderId ){

		/*
		 * Network responce took too long and we are now running
		 * a newer request to get playlist entries
		 */
		//return ;
	}

	auto& table = m.table() ;
	auto& media = m.media() ;

	auto s = downloadManager::finishedStatus::notStarted() ;

	if( networkAccess::hasNetworkSupport() ){

		QPixmap pixmap ;

		if( m.success() && pixmap.loadFromData( m.data() ) ){

			auto width = m_settings.thumbnailWidth( settings::tabName::playlist ) ;
			auto height = m_settings.thumbnailHeight( settings::tabName::playlist ) ;

			auto img = pixmap.scaled( width,height ) ;

			this->showEntry( table,{ img,s,media } ) ;
		}else{
			const auto& img = m_defaultVideoThumbnailIcon ;

			this->showEntry( table,{ img,s,media } ) ;
		}
	}else{
		const auto& img = m_defaultVideoThumbnailIcon ;

		this->showEntry( table,{ img,s,media } ) ;
	}

	table.selectLast() ;

	m_networkRunning-- ;
}

void playlistdownloader::addTextToUi( const QByteArray& data,int index )
{
	if( downloadManager::finishedStatus::running( m_table.runningState( index ) ) ){

		m_banner.updateTimer() ;

		m_table.setUiText( data,index ) ;
	}
}

void playlistdownloader::reportFinishedStatus( const reportFinished& f )
{
	m_banner.updateTimer() ;

	utility::updateFinishedState( f.engine(),m_settings,m_table,f.finishedStatus() ) ;

	if( m_table.noneAreRunning() ){

		m_ctx.TabManager().enableAll() ;

		if( m_table.allFinishedWithSuccess() ){

			this->resizeTable( playlistdownloader::size::small ) ;
		}
	}

	if( m_ctx.Settings().autoHideDownloadWhenCompleted() ){

		auto index = f.finishedStatus().index() ;

		const auto& r = f.finishedStatus().finishedWithSuccess() ;

		if( m_table.runningState( index ) == r ){

			m_table.hideRow( index ) ;
		}
	}

	m_ctx.mainWindow().setTitle( m_table.completeProgress( 1 ) ) ;
}

void playlistdownloader::resizeTable( playlistdownloader::size s )
{
	bool e = [ & ](){

		if( s == playlistdownloader::size::toggle ){

			return m_ui.pbPLPasteClipboard->isVisible() ;
		}else{
			return s == playlistdownloader::size::large ;
		}
	}() ;

	m_ui.pbPLPasteClipboard->setVisible( !e ) ;
	m_ui.pbPlSubscription->setVisible( !e ) ;
	m_ui.pbPLRangeHistory->setVisible( !e ) ;
	m_ui.pbClearArchiveFile->setVisible( !e ) ;
	m_ui.cbUseInternalArchiveFile->setVisible( !e ) ;
	m_ui.pbPLOptionsHistory->setVisible( !e ) ;
	m_ui.pbPLDownloadOptions->setVisible( !e ) ;
	m_ui.cbEngineTypePD->setVisible( !e ) ;
	m_ui.labelPLEngineName->setVisible( !e ) ;

	if( e ){

		m_ui.pbPLChangeTableSize->setToolTip( tr( "Show Controls" ) ) ;
		m_ui.pbPLChangeTableSize->setText( tr( "Show" ) ) ;

		m_ui.tableWidgetPl->resize( 771,431 ) ;
	}else{
		m_ui.pbPLChangeTableSize->setToolTip( tr( "Hide Controls" ) ) ;
		m_ui.pbPLChangeTableSize->setText( tr( "Hide" ) ) ;

		m_ui.tableWidgetPl->resize( 771,271 ) ;
	}
}

void playlistdownloader::showEntry( tableWidget& table,tableWidget::entry e )
{
	auto row = table.addItem( e.move() ) ;

	m_ctx.logger().setMaxProcessLog( m_table.rowCount() ) ;

	m_ctx.TabManager().Configure().setDownloadOptions( row,table ) ;

	if( !m_ui.pbPLCancel->isEnabled() ){

		if( m_autoDownload ){

			this->download() ;
		}
	}
}

playlistdownloader::subscription::subscription( const Context& e,
						tableMiniWidget< int >& t,
						QWidget& w ) :
	m_path( e.Engines().engineDirPaths().dataPath( "subscriptions.json" ) ),
	m_archivePath( e.Engines().engineDirPaths().subscriptionsArchiveFilePath() ),
	m_table( t ),
	m_ui( w )
{
}

void playlistdownloader::subscription::add( const QString& uiName,const QString& url,const QString& Opts )
{
	for( const auto& it : util::asConst( m_array ) ){

		auto m = it.toObject() ;

		auto a = m.value( "uiName" ).toString() ;
		auto b = m.value( "url" ).toString() ;
		auto c = m.value( "getListOptions" ).toString() ;

		if( a == uiName && b == url && c == Opts ){

			return ;
		}
	}

	QJsonObject obj ;
	obj.insert( "uiName",uiName ) ;
	obj.insert( "url",url ) ;
	obj.insert( "getListOptions",Opts ) ;

	m_array.append( obj ) ;

	m_table.add( { uiName,url } ) ;

	m_table.selectLast() ;

	this->save() ;
}

void playlistdownloader::subscription::remove( int s )
{
	m_array.removeAt( s ) ;
	m_table.removeRow( s ) ;

	this->save() ;
}

void playlistdownloader::subscription::setVisible( bool e )
{
	if( e ){

		m_table.clear() ;

		for( const auto& it : util::asConst( m_array ) ){

			auto m = it.toObject() ;

			auto a = m.value( "uiName" ).toString() ;
			auto b = m.value( "url" ).toString() ;
			auto c = tr( "Get List Options:" ) + " " + m.value( "getListOptions" ).toString() ;

			int row = m_table.add( { a,b } ) ;

			m_table.item( row,0 ).setToolTip( c ) ;
			m_table.item( row,1 ).setToolTip( c ) ;
		}

		m_table.selectLast() ;

		m_ui.setVisible( e ) ;
	}else{
		m_ui.setVisible( e ) ;
		m_table.clear() ;
	}
}

const QString& playlistdownloader::subscription::archivePath() const
{
	return m_archivePath ;
}

std::vector< playlistdownloader::subscription::entry > playlistdownloader::subscription::entries()
{
	if( m_array.isEmpty() && QFile::exists( m_path ) ){

		QFile f( m_path ) ;

		f.open( QIODevice::ReadOnly ) ;

		auto m = f.readAll() ;

		if( !m.isEmpty() ){

			QJsonParseError err ;

			auto e = QJsonDocument::fromJson( m,&err ) ;

			if( err.error == QJsonParseError::NoError ){

				m_array = e.array() ;
			}
		}
	}

	std::vector< subscription::entry > e ;

	for( int i = m_array.size() - 1 ; i >= 0 ; i-- ){

		auto m = m_array[ i ].toObject() ;

		auto a = m.value( "uiName" ).toString() ;
		auto b = m.value( "url" ).toString() ;
		auto c = m.value( "getListOptions" ).toString() ;

		e.emplace_back( std::move( a ),std::move( b ),std::move( c ) ) ;
	}

	return e ;
}

void playlistdownloader::subscription::save()
{
	QFile f( m_path ) ;

	f.open( QIODevice::WriteOnly | QIODevice::Truncate ) ;

	f.write( QJsonDocument( m_array ).toJson( QJsonDocument::Indented ) ) ;
}

void playlistdownloader::banner::updateProgress( const QString& progress )
{
	m_time = m_timer.elapsedTime() ;
	auto duration = engines::engine::functions::timer::stringElapsedTime( m_time ) ;
	m_progress = duration + ", " + progress ;
	m_table.setUiText( m_txt + "\n" + m_progress,0 ) ;
}

void playlistdownloader::banner::reportError( const QString& e )
{
	m_table.setUiText( e,0 ) ;
}

void playlistdownloader::banner::updateTimer()
{
	auto m = m_timer.elapsedTime() ;
	auto duration = engines::engine::functions::timer::stringElapsedTime( m_time + m ) ;
	m_table.setUiText( m_txt + "\n" + duration,0 ) ;
}

bool playlistdownloader::stdError::operator()( const QByteArray& e )
{
	auto s = e.indexOf( "page" ) ;

	if( s != -1 && e.contains( "Downloading API JSON" ) ){

		auto m = e.mid( s ) ;

		s = m.indexOf( ':' ) ;

		if( s != -1 ){

			m = m.mid( 0,s ).mid( 4 ) ;

			m_banner.updateProgress( tr( "Number of Pages Downloaded" ) + ": " + m ) ;
		}

		return false ;

	}else if( utils::misc::startsWithAny( e,"[download] Downloading video ",
					      "[download] Downloading item " ) ){

		auto s = [ & ](){

			if( e.startsWith( "[download] Downloading video " ) ){

				return 29 ;
			}else{
				return 28 ;
			}
		}() ;

		auto m = tr( "Downloading video info" ) + " " + e.mid( s ) ;

		s = m.indexOf( '\n' ) ;

		if( s != -1 ){

			m = m.mid( 0,s ) ;

			m.replace( " of N/A","" ) ;
		}

		m_banner.updateProgress( m ) ;

		return false ;
	}

	if( e.startsWith( "ERROR: " ) ){

		if( e.contains( "Temporary failure in name resolution" ) ){

			m_banner.reportError( "ERROR\nNetwork Not Reachable" ) ;
		}else{
			auto m = util::split( e,'\n',true ).at( 0 ) ;
			m_banner.reportError( "ERROR\n" + m.mid( 7 ) ) ;
		}

		return true ;
	}

	return e.startsWith( "WARNING" ) ;
}

void playlistdownloader::stdOut::operator()( tableWidget& table,Logger::Data& data )
{
	m_parent.m_dataReceived = true ;

	int position = 0 ;

	const auto line = data.toLine() ;

	const auto& jsonMarker = m_parent.m_jsonEndMarker ;

	if( line.contains( jsonMarker ) ){

		while( true ){

			auto m = line.indexOf( jsonMarker,position ) ;

			if( m == -1 ){

				break ;
			}else{
				utility::MediaEntry media( line.mid( position,m ) ) ;

				if( media.valid() ){

					if( m_parent.parseJson( m_customOptions,table,media.move() ) ){

						break ;
					}
				}

				position = position + m + jsonMarker.size() ;
			}
		}

		data.clear() ;
		data.add( line.mid( position ),utility::concurrentID() ) ;
	}
}

playlistdownloader::customOptions::customOptions( QStringList&& opts,
						  const QString& downloadArchivePath,
						  const engines::engine& engine,
						  const Context& ctx ) :
	m_options( std::move( opts ) )
{
	utility::arguments Opts( m_options ) ;

	m_maxMediaLength = Opts.hasValue( "--max-media-length",true ) ;
	m_minMediaLength = Opts.hasValue( "--min-media-length",true ) ;

	m_breakOnExisting = Opts.hasOption( "--break-on-existing",true ) ;
	m_skipOnExisting  = Opts.hasOption( "--skip-on-existing",true ) ;

	if( m_breakOnExisting || m_skipOnExisting ){

		auto s = ctx.TabManager().Configure().engineDefaultDownloadOptions( engine.name() ) ;

		if( ctx.Settings().useInternalArchiveFile() ){

			QFile file( downloadArchivePath ) ;

			if( file.open( QIODevice::ReadOnly ) ){

				m_archiveFileData = file.readAll() ;
			}
		}

		auto ss = util::splitPreserveQuotes( s ) ;

		auto mm = utility::arguments( ss ).hasValue( "--download-archive" ) ;

		if( !mm.isEmpty() && QFile::exists( mm ) ){

			QFile file( mm ) ;

			if( file.open( QIODevice::ReadOnly ) ){

				m_archiveFileData.append( "\n" + file.readAll() ) ;
			}
		}
	}
}

bool playlistdownloader::customOptions::contains( const QString& e ) const
{
	if( m_archiveFileData.isEmpty() ){

		return false ;
	}else{
		return m_archiveFileData.contains( e.toUtf8() ) ;
	}
}

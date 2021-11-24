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

class playlistdownloader::customOptions
{
public:
	customOptions( QStringList&& opts,
		       const QString& downloadArchivePath,
		       settings& settings,
		       const engines::engine& engine ) :
		m_options( std::move( opts ) )
	{
		utility::arguments Opts( m_options ) ;

		m_maxMediaLength = Opts.hasValue( "--max-media-length",true ) ;
		m_minMediaLength = Opts.hasValue( "--min-media-length",true ) ;

		m_breakOnExisting = Opts.hasOption( "--break-on-existing",true ) ;
		m_skipOnExisting  = Opts.hasOption( "--skip-on-existing",true ) ;

		if( m_breakOnExisting || m_skipOnExisting ){

			auto s = settings.engineDefaultDownloadOptions( engine.name() ) ;
			auto ss = util::splitPreserveQuotes( s ) ;

			auto mm = utility::arguments( ss ).hasValue( "--download-archive" ) ;

			if( mm.isEmpty() ){

				mm = downloadArchivePath ;
			}

			if( !mm.isEmpty() ){

				if( utility::isRelativePath( mm ) ){

					mm = settings.downloadFolder() + "/" + mm ;
				}
			}

			if( !mm.isEmpty() && QFile::exists( mm ) ){

				QFile file( mm ) ;

				if( file.open( QIODevice::ReadOnly ) ){

					m_archiveFileData = file.readAll() ;
				}
			}
		}
	}
	const QStringList& options() const
	{
		return m_options ;
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
		if( m_archiveFileData.isEmpty() ){

			return false ;
		}else{
			return m_archiveFileData.contains( e.toUtf8() + "\n" ) ;
		}
	}
	bool breakOnExisting() const
	{
		return m_breakOnExisting ;
	}
	bool skipOnExisting() const
	{
		return m_skipOnExisting ;
	}
private:
	bool m_breakOnExisting = false ;
	bool m_skipOnExisting = false ;
	QStringList m_options ;
	QString m_maxMediaLength ;
	QString m_minMediaLength ;
	QByteArray m_archiveFileData ;
};

playlistdownloader::playlistdownloader( Context& ctx ) :
	m_ctx( ctx ),
	m_settings( m_ctx.Settings() ),
	m_ui( m_ctx.Ui() ),
	m_mainWindow( m_ctx.mainWidget() ),
	m_tabManager( m_ctx.TabManager() ),
	m_table( *m_ui.tableWidgetPl,m_ctx.mainWidget().font(),1 ),
	m_subscriptionTable( *m_ui.tableWidgetPlDownloaderSubscription,m_ctx.mainWidget().font() ),
	m_ccmd( m_ctx,*m_ui.pbPLCancel,m_settings ),
	m_defaultVideoThumbnailIcon( m_settings.defaultVideoThumbnailIcon( settings::tabName::playlist ) ),
	m_subscription( m_ctx,m_subscriptionTable,*m_ui.widgetPlDownloader )
{	
	m_ui.labelPlSubscriptionListOptions->setText( tr( "Get List Options:" ).replace( ":","" ) ) ;

	m_subscription.setVisible( false ) ;

	this->resetMenu() ;

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

		if( row == -1 ){

			return utility::appendContextMenu( m,this->enabled(),function,false ) ;
		}

		auto txt = m_table.runningState( row ) ;

		if( txt.isEmpty() ){

			return utility::appendContextMenu( m,this->enabled(),function,false ) ;
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

		ac = m.addAction( tr( "Copy Url" ) ) ;

		connect( ac,&QAction::triggered,[ this,row ](){

			auto m = QApplication::clipboard() ;

			if( m ){

				m->setText( m_table.url( row ) ) ;
			}
		} ) ;

		utility::addDownloadContextMenu( running,finishSuccess,m,row,[ this ]( int row ){

			auto m = m_table.uiText( row ) ;

			return m.startsWith( engines::engine::mediaAlreadInArchiveText() + "\n" ) ;

		},[ this ]( QAction * ac,bool forceDownload,int row ){

			connect( ac,&QAction::triggered,[ this,row,forceDownload ](){

				downloadManager::index indexes( m_table ) ;

				auto e = m_table.runningState( row ) ;

				if( !downloadManager::finishedStatus::finishedWithSuccess( e ) || forceDownload ){

					auto u = m_table.downloadingOptions( row ) ;

					if( u.isEmpty() ){

						indexes.add( row,m_ui.lineEditPLUrlOptions->text() ) ;
					}else{
						indexes.add( row,u ) ;
					}
				}

				this->download( this->defaultEngine(),std::move( indexes ) ) ;
			} ) ;
		} ) ;

		utility::saveDownloadList( m_ctx,m,m_table ) ;

		auto mm = m.addMenu( tableWidget::engineName().replace( ":","" ) ) ;

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

		utility::appendContextMenu( m,{ this->enabled(),finishSuccess },function ) ;
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

			if( s == "Download All Updated" ){

				m_autoDownload = true ;

				this->getList( std::move( entries ) ) ;

			}else if( s == "Show All Updated" ){

				this->getList( std::move( entries ) ) ;

			}else if( s == "Manage Subscriptions" ){

				m_ui.lineEditPlSubscriptionUiName->setFocus() ;

				m_subscription.setVisible( true ) ;
			}else{
				for( const auto& it : entries ){

					if( it.url == s ){

						std::vector< subscription::entry > ss ;

						ss.emplace_back( it.uiName,it.url,it.getListOptions ) ;

						this->getList( std::move( ss ) ) ;

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

			this->getList( m ) ;
		}
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
}

void playlistdownloader::disableAll()
{
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

void playlistdownloader::gotEvent( const QByteArray& )
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

QString playlistdownloader::defaultEngineName()
{
	return m_settings.defaultEngine( settings::tabName::playlist,m_ctx.Engines().defaultEngineName() ) ;
}

const engines::engine& playlistdownloader::defaultEngine()
{
	return m_ctx.Engines().defaultEngine( this->defaultEngineName() ) ;
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

	auto _add = [ & ]( int s,const QString& opts ){

		auto validUrl = !m_table.url( s ).isEmpty() ;

		auto e = m_table.runningState( s ) ;

		if( validUrl && !downloadManager::finishedStatus::finishedWithSuccess( e ) ){

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

	int count = m_table.rowCount() ;

	for( int i = 0 ; i < count ; i++ ){

		_add( i,opts ) ;
	}

	this->download( engine,std::move( indexes ) ) ;
}

void playlistdownloader::download( const engines::engine& eng,int index )
{
	const auto& engine = utility::resolveEngine( m_table,eng,m_ctx.Engines(),index ) ;

	auto aa = [ &engine,index,this ]( utility::ProcessExitState e,const playlistdownloader::opts& ){

		auto aa = [ this ]( const engines::engine& engine,int index ){

			this->download( engine,index ) ;
		} ;

		auto bb = [ &engine,index,this ]( const downloadManager::finishedStatus& f ){

			utility::updateFinishedState( engine,m_settings,m_table,f ) ;

			if( m_table.noneAreRunning() ){

				m_ctx.TabManager().enableAll() ;

				m_showTimer = false ;
			}

			m_ctx.mainWindow().setTitle( m_table.completeProgress( index ) ) ;
		} ;

		m_ccmd.monitorForFinished( engine,index,std::move( e ),std::move( aa ),std::move( bb ) ) ;
	} ;

	auto functions = utility::OptionsFunctions( []( const playlistdownloader::opts& ){},std::move( aa ) ) ;

	auto m = m_ui.lineEditPLUrlOptions->text() ;

	m_settings.addOptionsHistory( m,settings::tabName::playlist ) ;

	auto updater = [ this,index ]( const QString& e ){

		m_table.setUiText( e,index ) ;
	} ;

	auto oopts  = playlistdownloader::make_options( { m_ctx,m_ctx.debug(),false,index },std::move( functions ) ) ;
	auto logger = make_loggerBatchDownloader( engine.filter( utility::args( m ).quality() ),
						  m_ctx.logger(),
						  std::move( updater ),
						  utility::concurrentID() ) ;

	m_table.setRunningState( downloadManager::finishedStatus::running(),index ) ;

	auto optsUpdater = [ this ]( QStringList opts ){

		const auto& archivePath = m_subscription.archivePath() ;

		if( !opts.contains( "--download-archive" ) && !archivePath.isEmpty() ){

			opts.append( "--download-archive" ) ;
			opts.append( archivePath ) ;
		}

		return opts ;
	} ;

	m_ccmd.download( engine,
			 optsUpdater,
			 m_ctx.Engines().engineDirPaths(),
			 m_table.url( index ),
			 m_terminator.setUp(),
			 std::move( oopts ),
			 std::move( logger ) ) ;
}

void playlistdownloader::getList( playlistdownloader::listIterator iter )
{
	m_dataReceived = false ;
	m_stoppedOnExisting = false ;
	m_meaw = false ;

	auto url = iter.url() ;

	url = util::split( url,' ',true ).first() ;

	m_ui.lineEditPLUrl->setText( url ) ;

	m_ui.pbPLCancel->setEnabled( true ) ;

	const auto& engine = this->defaultEngine() ;

	auto opts = engine.dumpJsonArguments() ;

	auto listOpts = iter.listOptions() ;

	if( listOpts.isEmpty() ){

		listOpts = m_ui.lineEditPLDownloadRange->text() ;

		m_settings.addToplaylistRangeHistory( listOpts ) ;

		m_settings.setPlaylistRangeHistoryLastUsed( listOpts ) ;
	}else{
		m_ui.lineEditPLDownloadRange->setText( listOpts ) ;
	}

	if( !listOpts.isEmpty() ){

		if( listOpts.startsWith( "--" ) ){

			opts.append( util::split( listOpts,' ',true ) ) ;
		}else{
			opts.append( engine.playlistItemsArgument() ) ;

			auto m = util::split( listOpts,' ',true ) ;

			opts.append( m ) ;
		}
	}

	opts.append( url ) ;

	m_networkRunning = 0 ;

	util::runInBgThread( [ &engine,this,opts = std::move( opts ) ]()mutable{

		return customOptions( std::move( opts ),
				      m_subscription.archivePath(),
				      m_settings,
				      engine ) ;

	},[ this,&engine,iter = std::move( iter ) ]( customOptions&& c )mutable{

		this->getList( std::move( c ),engine,std::move( iter ) ) ;
	} ) ;
}

void playlistdownloader::getList( customOptions&& c,
				  const engines::engine& engine,
				  playlistdownloader::listIterator iter )
{
	auto functions = utility::OptionsFunctions( [ this ]( const playlistdownloader::opts& opts ){

			opts.ctx.TabManager().disableAll() ;
			m_gettingPlaylist = true ;
			m_ui.pbPLCancel->setEnabled( true ) ;

		},[ this,iter = std::move( iter ) ]( utility::ProcessExitState st,const playlistdownloader::opts& ){

			if( m_meaw ){

				if( iter.hasNext() ){

					this->getList( iter.next() ) ;
				}else{
					if( m_autoDownload ){

						this->download() ;
					}else{
						m_showTimer = false ;
						m_ctx.TabManager().enableAll() ;
						m_gettingPlaylist = false ;
						m_ui.pbPLCancel->setEnabled( false ) ;
					}
				}

			}else if( st.cancelled() ){

				if( m_stoppedOnExisting && iter.hasNext() ){

					this->getList( iter.next() ) ;
				}else{
					m_showTimer = false ;
					m_ctx.TabManager().enableAll() ;
					m_gettingPlaylist = false ;
					m_ui.pbPLCancel->setEnabled( false ) ;
				}

			}else if( iter.hasNext() ){

				this->getList( iter.next() ) ;
			}else{
				m_ctx.TabManager().enableAll() ;
				m_gettingPlaylist = false ;
				m_ui.pbPLCancel->setEnabled( false ) ;

				if( !m_dataReceived ){

					m_showTimer = false ;
				}
			}
		}
	) ;

	auto opts = c.options() ;

	auto bb = [ copts = std::move( c ),this ]( tableWidget& table,Logger::Data& data ){

		m_dataReceived = true ;

		this->parseJson( copts,table,data ) ;
	} ;

	auto id     = utility::concurrentID() ;
	auto oopts  = playlistdownloader::make_options( { m_ctx,m_ctx.debug(),false,-1 },std::move( functions ) ) ;
	auto logger = make_loggerPlaylistDownloader( m_table,m_ctx.logger(),id,std::move( bb ) ) ;
	auto term   = m_terminator.setUp( m_ui.pbPLCancel,&QPushButton::clicked,-1 ) ;
	auto ch     = QProcess::ProcessChannel::StandardOutput ;
	auto argsq  = utility::args( m_ui.lineEditPLUrlOptions->text() ).quality() ;

	if( !m_gettingPlaylist ){

		logger.clear() ;

		auto s = tr( "This May Take A Very Long Time" ) ;
		auto d = engines::engine::functions::timer::stringElapsedTime( 0 ) ;

		QIcon icon( ":/media-downloader" ) ;

		auto w = m_settings.thumbnailWidth( settings::tabName::playlist ) ;
		auto h = m_settings.thumbnailHeight( settings::tabName::playlist ) ;

		m_table.addItem( { icon.pixmap( w,h ),d + "\n" + s,"","" } ) ;

		m_showTimer = true ;

		util::Timer( 1000,[ this,s ]( int counter ){

			if( m_showTimer ){

				using tt = engines::engine::functions ;

				auto duration = tt::timer::stringElapsedTime( counter * 1000 ) ;

				m_table.setUiText( duration + "\n" + s,0 ) ;

				return false ;
			}else{
				return true ;
			}
		} ) ;
	}

	m_table.selectLast() ;

	auto ctx = utility::make_ctx( engine,
				      std::move( oopts ),
				      std::move( logger ),
				      std::move( term ),
				      ch ) ;

	utility::run( opts,argsq,std::move( ctx ) ) ;

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

void playlistdownloader::parseJson( const customOptions& copts,
				    tableWidget& table,
				    Logger::Data& data )
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

	m_meaw = false ;

	auto aa = mmm.mid( oo,index + 1 ) ;

	utility::MediaEntry media( aa.toUtf8() ) ;

	if( !media.valid() ){

		return ;
	}else{
		data.clear() ;
		data.add( mmm.mid( index + 1 ) ) ;
	}	

	auto _show = [ this,&table ]( const tableWidget::entry& e )
	{
		auto row = table.addItem( e ) ;

		m_ctx.TabManager().Configure().setDownloadOptions( row,table ) ;

		if( !m_ui.pbPLCancel->isEnabled() ){

			if( m_autoDownload ){

				this->download() ;
			}else{
				m_showTimer = false ;
			}
		}
	} ;

	if( copts.contains( media.id() ) ){

		if( copts.breakOnExisting() ){

			m_stoppedOnExisting = true ;
			m_ui.pbPLCancel->click() ;
			return ;

		}else if( copts.skipOnExisting() ){

			auto s = downloadManager::finishedStatus::finishedWithSuccess() ;

			auto a = QObject::tr( "Media Already In Archive" ) + "\n" + media.uiText() ;

			_show( { m_defaultVideoThumbnailIcon,a,media.url(),s } ) ;

			table.selectLast() ;

			m_meaw = true ;

			return ;
		}
	}

	auto max = copts.maxMediaLength() ;

	if( max > 0 && media.intDuration() > max ){

		return ;
	}

	auto min = copts.minMediaLength() ;

	if( min > 0 && media.intDuration() < min ){

		return ;
	}

	auto s = downloadManager::finishedStatus::notStarted() ;

	table.selectLast() ;

	if( networkAccess::hasNetworkSupport() ){

		auto& network = m_ctx.versionInfo().network() ;

		m_networkRunning++ ;

		auto thumbnailUrl = media.thumbnailUrl() ;

		network.getResource( thumbnailUrl,
				     [ this,&table,s,_show = std::move( _show ),
				     media = std::move( media ) ]( const QByteArray& data ){

			QPixmap pixmap ;

			if( pixmap.loadFromData( data ) ){

				auto width = m_settings.thumbnailWidth( settings::tabName::playlist ) ;
				auto height = m_settings.thumbnailHeight( settings::tabName::playlist ) ;

				_show( { pixmap.scaled( width,height ),media.uiText(),media.url(),s } ) ;
			}else{
				_show( { m_defaultVideoThumbnailIcon,media.uiText(),media.url(),s } ) ;
			}

			table.selectLast() ;

			m_networkRunning-- ;
		} ) ;
	}else{
		_show( { m_defaultVideoThumbnailIcon,media.uiText(),media.url(),s } ) ;

		table.selectLast() ;

		m_meaw = true ;
	}
}

playlistdownloader::subscription::subscription( const Context& e,
						tableMiniWidget< int >& t,
						QWidget& w ) :
	m_path( e.Engines().engineDirPaths().dataPath( "subscriptions.json" ) ),
	m_archivePath( e.Engines().engineDirPaths().dataPath( "subscriptions_archive_file.txt" ) ),
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

			m_table.add( { a,b } ) ;
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

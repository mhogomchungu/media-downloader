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
	m_subscriptionTable( *m_ui.tableWidgetPlDownloaderSubscription,0,m_ctx.mainWidget().font() ),
	m_defaultVideoThumbnailIcon( m_settings.defaultVideoThumbnailIcon( settings::tabName::playlist ) ),
	m_banner( m_table ),
	m_subscription( m_ctx,m_subscriptionTable,*m_ui.widgetPlDownloader )
{
	qRegisterMetaType< PlNetworkData >() ;

	m_ui.pbPLPasteClipboard->setIcon( m_settings.getIcon( "clipboard" ) ) ;
	m_ui.pbPLOptionsHistory->setIcon( m_settings.getIcon( "recentlyUsed" ) ) ;
	m_ui.pbPLRangeHistory->setIcon( m_settings.getIcon( "recentlyUsed" ) ) ;
	m_ui.pbPLDownloadOptions->setIcon( m_settings.getIcon( "downloadOptions" ) ) ;
	m_ui.pbPlSubscription->setIcon( m_settings.getIcon( "subscription" ) ) ;
	m_ui.pbClearArchiveFile->setIcon( m_settings.getIcon( "clearFile" ) ) ;

	m_ui.pbPLChangeTableSize->setToolTip( tr( "Hide Controls" ) ) ;

	m_ui.tableWidgetPlDownloaderSubscription->setColumnWidth( 0,180 ) ;

	m_ui.labelPlSubscriptionListOptions->setText( tr( "Get List Options:" ).replace( ":","" ) ) ;

	m_subscription.setVisible( false ) ;

	this->resetMenu() ;

	connect( this,
		 &playlistdownloader::networkDataSignal,
		 this,
		 &playlistdownloader::networkData,
		 Qt::QueuedConnection ) ;

	connect( this,
		 &playlistdownloader::addTextToUiSignal,
		 this,
		 &playlistdownloader::addTextToUi,
		 Qt::QueuedConnection ) ;

	connect( this,
		 &playlistdownloader::reportFinishedStatusSignal,
		 this,
		 &playlistdownloader::reportFinishedStatus,
		 Qt::QueuedConnection ) ;

	connect( m_ui.pbClearArchiveFile,&QPushButton::clicked,[ this ](){

		auto m = m_ctx.Engines().engineDirPaths().subscriptionsArchiveFilePath() ;
		QFile f( m ) ;

		if( f.open( QIODevice::WriteOnly | QIODevice::Truncate ) ){

			f.write( "" ) ;
		}
	} ) ;

	m_ui.cbUseInternalArchiveFile->setChecked( m_ctx.Settings().useInternalArchiveFile() ) ;

	connect( m_ui.cbUseInternalArchiveFile,&QCheckBox::toggled,[ this ]( bool e ){

		m_ctx.Settings().setUseInternalArchiveFile( e ) ;
	} ) ;

	m_ui.lineEditPLDownloadRange->setText( m_settings.playlistRangeHistoryLastUsed( this->defaultEngineName()) ) ;

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

		t.engineSetDefaultDownloadOptions( this->defaultEngine() ) ;
	} ) ;

	auto ww = m_ctx.Settings().thumbnailWidth( settings::tabName::playlist ) ;

	m_table.get().setColumnWidth( 0,ww ) ;

	m_table.setCurrentItemChanged( m_table.startPosition() ) ;

	m_subscriptionTable.setCurrentItemChanged( 0 ) ;

	auto ccc = &QTableWidget::customContextMenuRequested ;

	m_subscriptionTable.connect( ccc,[ this ]( const QPoint& ){

		auto row = m_subscriptionTable.currentRow() ;

		if( row != -1 ){

			QMenu m ;

			auto s = &QAction::triggered ;

			connect( m.addAction( tr( "Remove" ) ),s,[ this,row ](){

				m_subscription.remove( row ) ;
			} ) ;

			m.exec( QCursor::pos() ) ;
		}
	} ) ;

	m_table.connect( ccc,[ this ]( const QPoint& ){

		this->customContextMenuRequested() ;
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

		auto engineName = this->defaultEngineName() ;

		auto mm = m_settings.getOptionsHistory( settings::tabName::playlist,engineName ) ;

		auto& m = *m_ui.lineEditPLUrlOptions ;

		if( utility::showHistory( m,mm,m_settings,engineName,settings::tabName::playlist ) ){

			if( m_settings.autoDownload() ){

				this->download() ;
			}
		}
	} ) ;

	connect( m_ui.pbPLDownload,&QPushButton::clicked,[ this ](){

		m_banner.clear() ;

		for( int s = m_table.rowCount() - 1 ; s >= 0 ; s-- ){

			if( m_table.isRowHidden( s ) ){

				m_table.removeRow( s ) ;
			}
		}

		this->download() ;
	} ) ;

	connect( m_ui.pbPLChangeTableSize,&QPushButton::clicked,[ this ](){

		this->resizeTable( playlistdownloader::size::toggle ) ;
	} ) ;

	connect( m_ui.pbPLRangeHistory,&QPushButton::clicked,[ this ](){

		auto& a = *m_ui.lineEditPLDownloadRange ;
		auto b = m_settings.playlistRangeHistory( this->defaultEngineName() ) ;
		auto c = settings::tabName::playlist ;
		auto d = utility::PlayListButtonName::DownloadRange ;

		utility::showHistory( a,b,m_settings,this->defaultEngineName(),c,d ) ;
	} ) ;

	connect( m_ui.pbPlSubscription,&QPushButton::clicked,[ this ](){

		this->plSubscription() ;
	} ) ;

	m_table.connect( &QTableWidget::cellDoubleClicked,[ this ]( int row,int column ){

		Q_UNUSED( column )

		const auto& ee = this->defaultEngine() ;
		const auto& e = utility::resolveEngine( m_table,ee,m_ctx.Engines(),row ) ;

		m_ctx.Engines().openUrls( m_table,row,e ) ;
	} ) ;

	connect( m_ui.cbEngineTypePD,s,[ & ]( int s ){

		if( s != -1 ){

			m_ui.lineEditPLUrlOptions->clear() ;
			auto m = m_ui.cbEngineTypePD->itemText( s ) ;

			m_settings.setDefaultEngine( m,settings::tabName::playlist ) ;

			auto mm = m_settings.lastUsedOption( m,settings::tabName::playlist ) ;

			m_ui.lineEditPLUrlOptions->setText( mm ) ;

			auto e = m_settings.playlistRangeHistoryLastUsed( this->defaultEngineName() ) ;

			m_ui.lineEditPLDownloadRange->setText( e ) ;
		}
	} ) ;

	connect( m_ui.pbPLGetList,&QPushButton::clicked,[ this ](){		

		auto m = m_ui.lineEditPLUrl->text() ;

		if( !m.isEmpty() ){

			utility::vector< subscription::entry > ss ;

			if( utils::misc::containsAny( m,'\n',' ' ) ){

				for( const auto& it : util::split( m,' ',true ) ){

					for( const auto& xt : util::split( it,'\n',true ) ){

						ss.emplace_back( xt ) ;
					}
				}
			}else{
				ss.emplace_back( m ) ;
			}

			this->getListing( ss.move(),this->defaultEngine(),false ) ;
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

void playlistdownloader::keyPressed( utility::mainWindowKeyCombo m )
{
	if( m == utility::mainWindowKeyCombo::ENTER ){

		if( m_ui.pbPLDownload->isEnabled() ){

			m_ui.pbPLDownload->click() ;
		}
	}else{
		utility::keyPressed( m_table,m ) ;
	}
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

	auto mm = m_settings.lastUsedOption( m,settings::tabName::playlist ) ;

	m_ui.lineEditPLUrlOptions->setText( mm ) ;

	m_ui.lineEditPLUrl->setFocus() ;
}

void playlistdownloader::tabExited()
{
}

void playlistdownloader::exiting()
{
}

void playlistdownloader::saveData()
{
	utility::saveDownloadList( m_ctx,m_table,true ) ;
}

void playlistdownloader::textAlignmentChanged( Qt::LayoutDirection m )
{
	auto a = m_ui.labelPLEngineName ;
	auto b = m_ui.labelPLEnterOptions ;
	auto c = m_ui.labelPLEnterUrlRange ;
	auto d = m_ui.labelPLEnterUrl ;

	utility::alignText( m,a,b,c,d ) ;
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

	auto m = this->defaultEngineName() ;
	auto s = settings::tabName::playlist ;

	utility::setUpdefaultEngine( comboBox,m,m_settings,s ) ;
}

void playlistdownloader::clipboardData( const QString& )
{
}

QString playlistdownloader::defaultEngineName()
{
	auto nn = m_ctx.Engines().defaultEngineName() ;
	return m_settings.defaultEngine( settings::tabName::playlist,nn ) ;
}

const engines::engine& playlistdownloader::defaultEngine()
{
	auto id = utility::concurrentID() ;

	return m_ctx.Engines().defaultEngine( this->defaultEngineName(),id ) ;
}

void playlistdownloader::customContextMenuRequested()
{
	auto row = m_table.currentRow() ;

	auto function = [ this,row ]( const utility::contextState& c ){

		if( c.showLogWindow() ){

			if( row == -1 ){

				m_ctx.logger().showLogWindow( row ) ;
			}else{
				m_ctx.logger().showLogWindow( m_table.entryAt( row ).id ) ;
			}

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

	auto running = reportFinished::finishedStatus::running( txt ) ;
	auto finishSuccess = reportFinished::finishedStatus::finishedWithSuccess( txt ) ;

	auto ac = m.addAction( tr( "Open" ) ) ;

	ac->setEnabled( finishSuccess ) ;

	connect( ac,&QAction::triggered,[ this,row ](){

		const auto& ee = this->defaultEngine() ;

		const auto& e = utility::resolveEngine( m_table,ee,m_ctx.Engines(),row ) ;

		m_ctx.Engines().openUrls( m_table,row,e ) ;
	} ) ;

	ac = m.addAction( tr( "Cancel" ) ) ;
	ac->setEnabled( running ) ;

	connect( ac,&QAction::triggered,[ this,row ](){

		m_terminator.terminate( row ) ;
	} ) ;

	ac = m.addAction( tr( "Remove" ) ) ;

	ac->setEnabled( m_table.noneAreRunning() && !m_networkRunning ) ;

	connect( ac,&QAction::triggered,[ this,row ](){

		Q_UNUSED( row ) ;

		m_table.removeAllSelected() ;

		m_ui.pbBDDownload->setEnabled( m_table.rowCount() ) ;
	} ) ;

	if( m_settings.autoHideDownloadWhenCompleted() ){

		utility::hideUnhideEntries( m,m_table,row,true ) ;
	}

	ac = m.addAction( tr( "Copy Url" ) ) ;

	connect( ac,&QAction::triggered,[ this ](){

		utility::copyToClipboardUrls( m_table ) ;
	} ) ;

	const auto& rr = this->defaultEngine() ;

	const auto& engine = utility::resolveEngine( m_table,rr,m_ctx.Engines(),row ) ;

	ac = m.addAction( tr( "Show Comments" ) ) ;
	ac->setEnabled( engine.supportShowingComments() ) ;

	connect( ac,&QAction::triggered,[ this,&engine ](){

		auto row = m_table.currentRow() ;

		if( row != -1 ){

			auto& m = m_ctx.TabManager().batchDownloader() ;

			m.showComments( engine,m_table.url( row ) ) ;
		}
	} ) ;

	utility::addDownloadContextMenu( running,finishSuccess,m,row,[ this ]( int row ){

		auto m = m_table.uiText( row ) ;

		return m.startsWith( engines::engine::mediaAlreadInArchiveText() + "\n" ) ;

	},[ this,&engine ]( QAction * ac,bool forceDownload,int row ){

		Q_UNUSED( row )

		connect( ac,&QAction::triggered,[ &engine,this,forceDownload ](){

			std::vector< int > v ;

			for( int row = 1 ; row < m_table.rowCount() ; row++ ){

				const auto& e    = m_table.runningState( row ) ;
				auto visible     = m_table.rowIsVisible( row ) ;
				auto highlighted = m_table.rowIsSelected( row ) ;

				auto m = reportFinished::finishedStatus::finishedWithSuccess( e ) ;

				if( visible && highlighted && ( !m || forceDownload ) ){

					v.emplace_back( row ) ;
				}
			}

			auto mm = m_settings.maxConcurrentDownloads() ;

			auto s = std::min( mm,v.size() ) ;

			for( size_t m = 0 ; m < s ; m++ ){

				this->downloadRecursively( engine,v[ m ],false ) ;
			}
		} ) ;
	} ) ;

	utility::saveDownloadList( m_ctx,m,m_table,true ) ;

	auto mm = m.addMenu( utility::stringConstants::engineName().replace( ":","" ) ) ;

	mm->setEnabled( !finishSuccess ) ;

	for( const auto& it : m_ctx.Engines().getEngines() ){

		if( !it.supportingEngine() ){

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
}

void playlistdownloader::plSubscription()
{
	QMenu m ;

	auto ee = m_subscription.entries() ;

	ee.rEach( [ & ]( const subscription::entry& s ){

		m.addAction( s.UiName() )->setObjectName( s.url() ) ;
	} ) ;

	m.addSeparator() ;

	bool enable = ee.size() > 0 ;

	auto ac = m.addAction( tr( "Show All Updated" ) ) ;

	ac->setObjectName( "Show All Updated" ) ;

	ac->setEnabled( enable ) ;

	ac = m.addAction( tr( "Download All Updated" ) ) ;

	ac->setObjectName( "Download All Updated" ) ;

	ac->setEnabled( enable ) ;

	m.addSeparator() ;

	m.addAction( tr( "Manage Subscriptions" ) )->setObjectName( "Manage Subscriptions" ) ;

	class meaw
	{
	public:
		meaw( utility::vector< playlistdownloader::subscription::entry > e,playlistdownloader& p ) :
			m_vector( e.move() ),m_parent( p )
		{
		}
		void operator()( QAction * ac )
		{
			auto s = ac->objectName() ;

			const auto& engine = m_parent.defaultEngine() ;

			if( s == "Download All Updated" ){

				m_parent.getListing( m_vector.move(),engine,true ) ;

			}else if( s == "Show All Updated" ){

				m_parent.getListing( m_vector.move(),engine,false ) ;

			}else if( s == "Manage Subscriptions" ){

				m_parent.m_ui.lineEditPlSubscriptionUiName->setFocus() ;

				m_parent.m_subscription.setVisible( true ) ;
			}else{
				m_vector.each( [ & ]( const subscription::entry& e ){

					if( e.url() == s ){

						utility::vector< subscription::entry > ss ;

						ss.emplace_back( e ) ;

						m_parent.getListing( ss.move(),engine,false ) ;

						return true ;
					}else{
						return false ;
					}
				} ) ;
			}
		}
	private:
		utility::vector< playlistdownloader::subscription::entry > m_vector ;
		playlistdownloader& m_parent ;
	} ;

	QObject::connect( &m,&QMenu::triggered,meaw( ee.move(),*this ) ) ;

	m.exec( QCursor::pos() ) ;
}

void playlistdownloader::download()
{
	auto a = m_ui.cbEngineTypePD->currentText() ;
	auto b = m_ui.lineEditPLUrlOptions->text() ;
	auto c = settings::tabName::playlist ;

	m_settings.setLastUsedOption( a,b,c ) ;

	this->download( this->defaultEngine() ) ;
}

void playlistdownloader::download( const engines::engine& engine )
{
	class meaw
	{
	public:
		meaw( playlistdownloader& parent,const engines::engine& engine ) :
			m_parent( parent ),m_engine( engine )
		{
		}
		void operator()()
		{
			std::vector< int > v ;

			for( int s = 1 ; s < m_parent.m_table.rowCount() ; s++ ){

				if( !m_parent.m_table.finishedWithSuccess( s ) ){

					v.emplace_back( s ) ;
				}
			}

			auto mm = m_parent.m_settings.maxConcurrentDownloads() ;

			auto s = std::min( mm,v.size() ) ;

			for( size_t m = 0 ; m < s ; m++ ){

				m_parent.downloadRecursively( m_engine,v[ m ],true ) ;
			}
		}
	private:
		playlistdownloader& m_parent ;
		const engines::engine& m_engine ;
	} ;

	m_ctx.logger().clear() ;

	m_ctx.logger().setMaxProcessLog( m_table.rowCount() ) ;

	m_ctx.TabManager().basicDownloader().hideTableList() ;

	engine.updateVersionInfo( m_ctx,meaw( *this,engine ) ) ;
}

void playlistdownloader::downloadRecursively( const engines::engine& eng,int index,bool downloadRecursively )
{	
	class events
	{
	public:
		events( playlistdownloader& p,const engines::engine& engine,int index,bool dr ) :
			m_parent( p ),
			m_engine( engine ),
			m_index( index ),
			m_downloadRecursively( dr )
		{
		}
		bool addData( const QByteArray& e )
		{
			if( utility::containsLinkerWarning( e ) ){

				return false ;
			}else{
				return true ;
			}
		}
		const engines::engine& engine()
		{
			return m_engine ;
		}
		void done( engines::ProcessExitState st,const std::vector< QByteArray >& fileNames )
		{
			m_parent.m_ctx.mainWindow().setTitle( m_parent.m_table.completeProgress( 1 ) ) ;

			m_parent.m_banner.updateTimer() ;

			reportFinished::finishedStatus::state m ;

			if( st.cancelled() ){

				m = reportFinished::finishedStatus::state::cancelled ;
			}else{
				m = reportFinished::finishedStatus::state::done ;
			}

			if( m_downloadRecursively && !st.cancelled() ){

				this->startNext() ;
			}

			reportFinished::finishedStatus s{ m_index,m,st } ;

			emit m_parent.reportFinishedStatusSignal( { m_engine,s.move() },fileNames ) ;

			if( fileNames.size() && st.success() && m_engine.isGalleryDl() ){

				m_parent.setThumbnail( fileNames,m_engine,m_index ) ;
			}
		}
		void disableAll()
		{
			m_parent.disableAll() ;
			m_parent.m_ui.pbPLCancel->setEnabled( true ) ;
		}
		int index()
		{
			return m_index ;
		}
		void printOutPut( const QByteArray& e )
		{
			m_parent.m_ctx.debug( m_index,e ) ;
		}
		QString downloadFolder()
		{
			return m_parent.m_ctx.Settings().downloadFolder() ;
		}
		events move()
		{
			return std::move( *this ) ;
		}
	private:
		void startNext()
		{
			auto m = m_parent.m_table.nextAvailableEntryToDownload( m_index + 1 ) ;

			if( m != -1 ){

				m_parent.downloadRecursively( m_engine,m,m_downloadRecursively ) ;
			}
		}
		playlistdownloader& m_parent ;
		const engines::engine& m_engine ;
		int m_index ;
		bool m_downloadRecursively ;
	} ;

	auto m = m_ui.lineEditPLUrlOptions->text() ;

	m_settings.addOptionsHistory( this->defaultEngineName(),m,settings::tabName::playlist ) ;

	auto updater = [ this,index ]( const QByteArray& e ){

		emit this->addTextToUiSignal( e,index ) ;
	} ;

	auto error = []( const QByteArray& ){} ;

	const auto& engine = utility::resolveEngine( m_table,eng,m_ctx.Engines(),index ) ;

	auto logs   = m_settings.getLogsLimits() ;
	auto id     = utility::concurrentID() ;
	auto ff     = engine.filter( id ) ;
	auto logger = make_loggerBatchDownloader( ff.move(),m_ctx.logger(),updater,error,id,logs ) ;

	m_table.setStateAsRunning( index,downloadRecursively ) ;
	m_table.setConcurrentId( index,id ) ;

	auto optsUpdater = [ this ]( QStringList opts ){

		if( m_ctx.Settings().useInternalArchiveFile() ){

			opts.append( "--download-archive" ) ;
			opts.append( m_subscription.archivePath() ) ;
		}

		return opts ;
	} ;

	m_ctx.mainWindow().setTitle( m_table.completeProgress( 1 ) ) ;

	auto dopt = utility::setDownloadOptions( engine,m_table,index ) ;
	const auto& ent = m_table.entryAt( index ) ;

	auto urlOpts = m_ui.lineEditPLUrlOptions->text() ;

	utility::download( engine,
			  std::move( optsUpdater ),
			  engine.name() == this->defaultEngineName() ? urlOpts : QString(),
			  m_table.url( index ),
			  m_ctx,
			  { dopt,{ index,m_table.rowCount() },true,ent },
			  m_terminator.setUp(),
			  events( *this,engine,index,downloadRecursively ),
			  logger.move() ) ;
}

void playlistdownloader::showBanner()
{
	m_table.clear() ;
	m_banner.clear() ;

	QIcon icon( m_settings.getIcon( "media-downloader" ) ) ;

	auto w = m_settings.thumbnailWidth( settings::tabName::playlist ) ;
	auto h = m_settings.thumbnailHeight( settings::tabName::playlist ) ;

	tableWidget::entry entry ;

	entry.banner    = true ;
	entry.uiText    = m_banner.txt() ;
	entry.thumbnail = icon.pixmap( w,h ) ;

	m_table.addItem( entry.move() ) ;

	m_table.selectLast() ;

	this->disableAll() ;
}

void playlistdownloader::getListing( playlistdownloader::listIterator e,
				    const engines::engine& engine,
				    bool autoDownload )
{
	this->resizeTable( playlistdownloader::size::large ) ;

	this->showBanner() ;

	class meaw
	{
	public:
		meaw( playlistdownloader& p,
		      playlistdownloader::listIterator it,
		      const engines::engine& engine,
		      bool autoDownload  ) :
			m_parent( p ),m_it( it.move() ),m_engine( engine ),m_autoDownload( autoDownload )
		{
		}
		void operator()()
		{
			m_parent.getList( m_it.move(),m_engine,m_autoDownload ) ;
		}
	private:
		playlistdownloader& m_parent ;
		playlistdownloader::listIterator m_it ;
		const engines::engine& m_engine ;
		bool m_autoDownload ;
	} ;

	engine.updateVersionInfo( m_ctx,meaw( *this,e.move(),engine,autoDownload ) ) ;
}

void playlistdownloader::getList( playlistdownloader::listIterator iter,
				 const engines::engine& engine,
				 bool autoDownload )
{
	m_stoppedOnExisting = false ;

	auto url = iter.url() ;

	url = util::split( url,' ',true ).first() ;

	m_ui.lineEditPLUrl->setText( url ) ;

	m_ui.pbPLCancel->setEnabled( true ) ;

	auto opts = engine.dumpJsonArguments( engines::engine::tab::playlist ) ;

	if( !opts.isEmpty() && engine.likeYtDlp() ){

		opts.last() += m_jsonEndMarker ;
	}

	auto configListOpts = iter.listOptions() ;
	auto listOptions    = m_ui.lineEditPLDownloadRange->text() ;

	if( !listOptions.isEmpty() ){

		m_settings.addToplaylistRangeHistory( engine.name(),listOptions ) ;

		m_settings.setPlaylistRangeHistoryLastUsed( engine.name(),listOptions ) ;

		opts.append( util::splitPreserveQuotes( listOptions ) ) ;
	}

	if( !configListOpts.isEmpty() ){

		opts.append( util::splitPreserveQuotes( configListOpts ) ) ;
	}

	engine.setTextEncondig( opts ) ;
	engine.updateGetPlaylistCmdOptions( opts ) ;

	auto m = m_ui.lineEditPLUrlOptions->text() ;

	utility::addToListOptionsFromsDownload( opts,m,m_ctx,engine ) ;

	utility::setCookieOption( opts,m_settings,engine ) ;

	opts.append( url ) ;

	m_networkRunning = 0 ;

	class meaw
	{
	public:
		meaw(	const QString& url,
			const engines::engine& engine,
			QStringList opts,
			playlistdownloader& parent,
			playlistdownloader::listIterator iter,
			bool ad ) :
			m_engine( engine ),
			m_opts( std::move( opts ) ),
			m_parent( parent ),
			m_iter( std::move( iter ) ),
			m_url( url ),
			m_autoDownload( ad )
		{
		}
		customOptions bg()
		{
			const auto& m = m_parent.m_subscription.archivePath() ;

			return { std::move( m_opts ),m,m_engine,m_parent.m_ctx } ;
		}
		void fg( customOptions o )
		{
			m_parent.getList( m_url,o.move(),m_engine,m_iter.move(),m_autoDownload ) ;
		}
	private:
		const engines::engine& m_engine ;
		QStringList m_opts ;
		playlistdownloader& m_parent ;
		playlistdownloader::listIterator m_iter ;
		QString m_url ;
		bool m_autoDownload ;
	} ;

	utils::qthread::run( meaw( url,engine,std::move( opts ),*this,iter.move(),autoDownload ) ) ;
}

void playlistdownloader::getList(  const QString& url,
				  customOptions&& c,
				  const engines::engine& engine,
				  playlistdownloader::listIterator iter,
				  bool autoDownload )
{
	class events
	{
	public:
		events( playlistdownloader& p,
			const engines::engine& e,
			playlistdownloader::listIterator i,
			bool ad ) :
			m_parent( p ),m_engine( e ),m_iter( i.move() ),m_autoDownload( ad )
		{
		}
		const engines::engine& engine()
		{
			return m_engine ;
		}
		bool addData( const QByteArray& e )
		{
			return utility::addData( e ) ;
		}
		void done( engines::ProcessExitState st,const std::vector< QByteArray >& )
		{
			if( st.cancelled() ){

				if( m_parent.m_stoppedOnExisting && m_iter.hasNext() ){

					m_parent.getList( m_iter.next(),m_engine,m_autoDownload ) ;
				}else{
					m_parent.enableAll() ;
					m_parent.m_gettingPlaylist = false ;
					m_parent.m_ui.pbPLCancel->setEnabled( false ) ;
				}

			}else if( m_iter.hasNext() ){

				m_parent.getList( m_iter.next(),m_engine,m_autoDownload ) ;
			}else{
				if( m_autoDownload ){

					m_parent.download() ;
				}else{
					m_parent.enableAll() ;
					m_parent.m_gettingPlaylist = false ;
					m_parent.m_ui.pbPLCancel->setEnabled( false ) ;
				}
			}
		}
		void disableAll()
		{
			m_parent.disableAll() ;
			m_parent.m_gettingPlaylist = true ;
			m_parent.m_ui.pbPLCancel->setEnabled( true ) ;
			m_parent.m_ui.pbPLDownload->setEnabled( true ) ;
		}
		int index()
		{
			return -1 ;
		}
		void printOutPut( const QByteArray& e )
		{
			m_parent.m_ctx.debug( -1,e ) ;
		}
		QString downloadFolder()
		{
			return m_parent.m_settings.downloadFolder() ;
		}
		events move()
		{
			return std::move( *this ) ;
		}
	private:
		playlistdownloader& m_parent ;
		const engines::engine& m_engine ;
		playlistdownloader::listIterator m_iter ;
		bool m_autoDownload ;
	} ;

	auto opts = c.options() ;

	stdOut sOut( *this,c.move(),engine,url ) ;
	stdError sErr( *this,m_banner,engine ) ;

	events ev( *this,engine,iter.move(),autoDownload ) ;

	auto& ll = m_ctx.logger() ;

	auto logs   = m_ctx.Settings().getLogsLimits() ;
	auto id     = utility::concurrentID() ;
	auto logger = make_loggerPlaylistDownloader( m_table,ll,id,sOut.move(),sErr.move(),logs ) ;
	auto term   = m_terminator.setUp( m_ui.pbPLCancel,&QPushButton::clicked,-1 ) ;
	auto ch     = QProcess::ProcessChannel::StandardOutput ;

	if( !m_gettingPlaylist ){

		logger.clear() ;

		this->showBanner() ;

		m_ctx.logger().setMaxProcessLog( m_table.rowCount() ) ;
	}

	m_table.selectLast() ;

	auto ctx = utility::make_ctx( m_ctx,ev.move(),logger.move(),term.move(),ch ) ;

	opts.removeAll( "--playlist-reverse" ) ;

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

bool playlistdownloader::parseJson( const engines::engine& engine,
				   const customOptions& copts,
				   utility::MediaEntry media )
{
	if( copts.contains( media.id() ) ){

		if( copts.breakOnExisting() ){

			m_stoppedOnExisting = true ;
			m_ui.pbPLCancel->click() ;

			auto s = reportFinished::finishedStatus::finishedWithSuccess() ;

			auto mm = QObject::tr( "Stopping Because Media Is Already In Archive File" ) ;

			this->showEntry( { m_defaultVideoThumbnailIcon,s,media,mm },false ) ;

			return true ;

		}else if( copts.skipOnExisting() ){

			auto s = reportFinished::finishedStatus::finishedWithSuccess() ;

			auto mm = QObject::tr( "Media Already In Archive" ) ;

			this->showEntry( { m_defaultVideoThumbnailIcon,s,media,mm },false ) ;

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

	auto thumbnailUrl = media.thumbnailUrl() ;

	if( networkAccess::hasNetworkSupport() && !thumbnailUrl.isEmpty() ){

		auto& network = m_ctx.network() ;

		m_networkRunning++ ;

		auto ua = engine.isGalleryDl() ? network.defaultUserAgent() : QByteArray() ;

		network.get( thumbnailUrl,media.move(),this,&playlistdownloader::networkResult,ua ) ;
	}else{
		emit this->networkDataSignal( { -1,media.move() } ) ;
	}

	return false ;
}

void playlistdownloader::networkResult( utility::MediaEntry media,const utils::network::reply& reply )
{
	emit this->networkDataSignal( { m_ctx,reply,-1,media.move() } ) ;
}

void playlistdownloader::networkData( utility::networkReply m )
{
	auto s = reportFinished::finishedStatus::notStarted() ;

	if( networkAccess::hasNetworkSupport() ){

		QPixmap pixmap ;

		if( m.success() && pixmap.loadFromData( m.data() ) ){

			auto width = m_settings.thumbnailWidth( settings::tabName::playlist ) ;
			auto height = m_settings.thumbnailHeight( settings::tabName::playlist ) ;

			auto img = pixmap.scaled( width,height ) ;

			this->showEntry( { img,s,m.media() },true ) ;
		}else{
			const auto& img = m_defaultVideoThumbnailIcon ;

			this->showEntry( { img,s,m.media() },true ) ;
		}
	}else{
		const auto& img = m_defaultVideoThumbnailIcon ;

		this->showEntry( { img,s,m.media() },true ) ;
	}

	m_networkRunning-- ;
}

void playlistdownloader::addTextToUi( const QByteArray& data,int index )
{
	if( reportFinished::finishedStatus::running( m_table.runningState( index ) ) ){

		m_banner.updateTimer() ;

		m_table.setUiText( data,index ) ;
	}
}

void playlistdownloader::setThumbnail( const std::vector< QByteArray >& fileNames,
				      const engines::engine& engine,
				      int row )
{
	auto m = m_settings.downloadFolder() ;
	auto downloadFolder = engine.downloadFolder( m ) ;

	class meaw
	{
	public:
		meaw( playlistdownloader& p,int row ) : m_parent( p ),m_row( row )
		{
		}
		void setPath( const QString& e )
		{
			m_filePath = e ;
		}
		bool invalidEntry( const QByteArray& e ) const
		{
			return utility::fileIsInvalidForGettingThumbnail( e ) ;
		}
		QPixmap bg()
		{
			QFile f( m_filePath ) ;

			QPixmap pixmap ;

			if( f.open( QIODevice::ReadOnly ) && pixmap.loadFromData( f.readAll() ) ){

				auto a = settings::tabName::batch ;

				auto w = m_parent.m_settings.thumbnailWidth( a ) ;
				auto h = m_parent.m_settings.thumbnailHeight( a ) ;

				return pixmap.scaled( w,h ) ;
			}

			return pixmap ;
		}
		void fg( const QPixmap& pixmap )
		{
			if( !pixmap.isNull() ){

				auto label = new QLabel() ;

				label->setAlignment( Qt::AlignCenter ) ;
				label->setPixmap( pixmap ) ;

				m_parent.m_table.get().setCellWidget( m_row,0,label ) ;
			}
		}
		meaw move()
		{
			return *this ;
		}
	private:
		playlistdownloader& m_parent ;
		int m_row ;
		QString m_filePath ;
	} ;

	utility::setThumbNail( fileNames,downloadFolder,meaw( *this,row ) ) ;
}

void playlistdownloader::reportFinishedStatus( const reportFinished& f,
					      const std::vector< QByteArray >& fileNames )
{
	m_banner.updateTimer() ;

	utility::updateFinishedState( f.engine(),m_settings,m_table,"playlist",f.status(),fileNames ) ;

	auto index = f.status().index() ;

	if( m_settings.desktopNotifyOnDownloadComplete() ){

		if( f.status().exitState().success() ){

			const auto& ss = m_table.entryAt( index ).uiText ;

			auto m = util::split( ss,"\n" ) ;

			if( m.size() > 1 ){

				m_ctx.mainWindow().notifyOnDownloadComplete( m[ 1 ] ) ;
			}else{
				m_ctx.mainWindow().notifyOnDownloadComplete( m[ 0 ] ) ;
			}
		}
	}

	if( m_table.noneAreRunning() ){

		if( m_settings.desktopNotifyOnAllDownloadComplete() ){

			auto m = m_table.finishWithSuccess() ;

			if( m == 1 ){

				m_ctx.mainWindow().notifyOnAllDownloadComplete( "1 Download Complete" ) ;

			}else if( m > 1 ){

				auto s = QString::number( m ) ;

				m_ctx.mainWindow().notifyOnAllDownloadComplete( s + " Downloads Complete" ) ;
			}
		}

		this->enableAll() ;

		if( m_table.allFinishedWithSuccess() ){

			this->resizeTable( playlistdownloader::size::small ) ;
		}
	}

	if( m_ctx.Settings().autoHideDownloadWhenCompleted() ){

		if( m_table.finishedWithSuccess( index ) ){

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

		m_ui.tableWidgetPl->resize( 771,446 ) ;
	}else{
		m_ui.pbPLChangeTableSize->setToolTip( tr( "Hide Controls" ) ) ;
		m_ui.pbPLChangeTableSize->setText( tr( "Hide" ) ) ;

		m_ui.tableWidgetPl->resize( 771,281 ) ;
	}
}

void playlistdownloader::showEntry( tableWidget::entry e,bool downloadable )
{
	auto row = m_table.addItem( e.move() ) ;

	m_ctx.logger().setMaxProcessLog( m_table.rowCount() ) ;

	m_ctx.TabManager().Configure().setDownloadOptions( row,m_table ) ;

	m_table.selectRow( row ) ;

	if( !m_ui.pbPLCancel->isEnabled() && downloadable ){

		auto e = static_cast< int >( m_settings.maxConcurrentDownloads() ) ;

		auto m = m_table.totalRunningRecursively() ;

		if( m > 0 && m < e ){

			const auto& eng = this->defaultEngine() ;

			const auto& engines = m_ctx.Engines() ;

			const auto& engine = utility::resolveEngine( m_table,eng,engines,row ) ;

			this->downloadRecursively( engine,row,true ) ;
		}
	}
}

playlistdownloader::subscription::subscription( const Context& e,
						tableMiniWidget< int,2 >& t,
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

		subscription::entry m( it )  ;

		if( m.UiName() == uiName && m.url() == url && m.options() == Opts ){

			return ;
		}
	}

	m_array.append( subscription::entry::toObject( uiName,url,Opts ) ) ;

	m_table.add( uiName,url ) ;

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

			subscription::entry m( it ) ;

			auto c = tr( "Get List Options:" ) + " " + m.options() ;

			int row = m_table.add( m.UiName(),m.url() ) ;

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

utility::vector< playlistdownloader::subscription::entry > playlistdownloader::subscription::entries()
{
	if( m_array.isEmpty() && QFile::exists( m_path ) ){

		QFile f( m_path ) ;

		if( f.open( QIODevice::ReadOnly ) ){

			auto m = f.readAll() ;

			if( !m.isEmpty() ){

				QJsonParseError err ;

				auto e = QJsonDocument::fromJson( m,&err ) ;

				if( err.error == QJsonParseError::NoError ){

					m_array = e.array() ;
				}
			}
		}
	}

	utility::vector< subscription::entry > e ;

	for( int i = m_array.size() - 1 ; i >= 0 ; i-- ){

		e.emplace_back( m_array,i ) ;
	}

	return e ;
}

void playlistdownloader::subscription::save()
{
	QFile f( m_path ) ;

	if( f.open( QIODevice::WriteOnly | QIODevice::Truncate ) ){

		f.write( QJsonDocument( m_array ).toJson( QJsonDocument::Indented ) ) ;
	}
}

void playlistdownloader::banner::updateProgress( const QString& progress )
{
	m_time = m_timer.elapsedTime() ;

	auto duration = engines::engine::baseEngine::timer::stringElapsedTime( m_time ) ;

	if( progress.isEmpty() ){

		m_progress = duration ;
	}else{
		m_progress = duration + ", " + progress ;
	}

	m_table.setUiText( m_txt + "\n" + m_progress,0 ) ;
}

void playlistdownloader::banner::reportError( const QString& e )
{
	m_table.setUiText( e,0 ) ;
}

void playlistdownloader::banner::updateTimer()
{
	auto m = m_timer.elapsedTime() ;
	auto duration = engines::engine::baseEngine::timer::stringElapsedTime( m_time + m ) ;
	m_table.setUiText( m_txt + "\n" + duration,0 ) ;
}

bool playlistdownloader::stdError::operator()( const QByteArray& e )
{
	if( e.startsWith( "[gallery-dl][error] Unsupported URL " ) || e.contains( "is not a valid UR" ) ){

		m_parent.m_table.setUiText( QObject::tr( "Url Is Not Supported" ),0 ) ;

		return true ;
	}
	if( utility::containsLinkerWarning( e ) ){

		return true ;
	}

	auto s = e.indexOf( "page" ) ;

	if( s != -1 && e.contains( "Downloading API JSON" ) ){

		auto m = e.mid( s ) ;

		s = m.indexOf( ':' ) ;

		if( s != -1 ){

			m = m.mid( 0,s ).mid( 4 ) ;

			auto mm = tr( "Number of Pages Downloaded" ) ;
			m_banner.updateProgress( mm + ": " + m ) ;
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

	auto m = m_engine.parseError( e ) ;

	if( !m.isEmpty() ){

		m_banner.reportError( m ) ;

		return true ;
	}

	if( e.startsWith( "ERROR: " )  || e.contains( "error:" ) ){

		auto a = "Temporary failure in name resolution" ;
		auto b = "Connection refused" ;

		if( e.contains( a ) || e.contains( b ) ){

			m_banner.reportError( QObject::tr( "Download Failed, Network Issue" ) ) ;

		}else{
			m_banner.reportError( QObject::tr( "Download Failed, Unknown Reason" ) ) ;
		}

		return true ;
	}

	return e.startsWith( "WARNING" ) ;
}

void playlistdownloader::stdOut::operator()( Logger::Data& data )
{
	if( m_engine.likeYtDlp() ){

		this->parseYtDlpData( data ) ;
	}else{
		auto m = data.toLine() ;

		data.clear() ;

		if( m.contains( "Download Failed(ErrorCode=1)" ) ){

			m_parent.m_table.setUiText( QObject::tr( "Download Failed, Unknown Reason" ),0 ) ;

		}else if( m.contains( "Download Failed(ErrorCode=4)" ) ){

			m_parent.m_table.setUiText( QObject::tr( "Download Failed, Network Issue" ),0 ) ;

		}else if( !m.startsWith( "[media-downloader]" ) ){

			m_parent.m_banner.updateProgress( "" ) ;

			m_data += m ;

			for( const auto& it : m_engine.parseJsonData( m_data ) ){

				auto mm = m_engine.parseJson( m_url,it ) ;

				auto url = mm.value( "direct_url" ).toString() ;

				mm.remove( "direct_url" ) ;

				mm.insert( "webpage_url",url ) ;

				m_parent.parseJson( m_engine,m_customOptions,QJsonDocument( mm ) ) ;
			}
		}
	}
}

void playlistdownloader::stdOut::parseYtDlpData( Logger::Data& data )
{
	int position = 0 ;

	const auto line = data.toLine() ;

	const auto& jsonMarker = m_parent.m_jsonEndMarker ;

	if( line.contains( jsonMarker ) ){

		while( true ){

			auto m = line.indexOf( jsonMarker,position ) ;

			if( m == -1 ){

				break ;
			}else{
				utility::MediaEntry media( {},m_engine,line.mid( position,m ) ) ;

				position = position + m + jsonMarker.size() ;

				if( media.valid() ){

					const auto& p = m_customOptions ;

					if( p.options().contains( "--playlist-reverse" ) ){

						media.setShowFirst() ;
					}

					if( m_parent.parseJson( m_engine,p,media.move() ) ){

						break ;
					}
				}
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

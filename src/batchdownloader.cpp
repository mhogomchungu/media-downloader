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
#include "mainwindow.h"
#include "utils/miscellaneous.hpp"

#include <QMetaObject>
#include <QClipboard>
#include <QFileDialog>
#include <QJsonArray>
#include <QJsonDocument>
#include <QFile>

batchdownloader::batchdownloader( const Context& ctx ) :
	m_ctx( ctx ),
	m_settings( m_ctx.Settings() ),
	m_ui( m_ctx.Ui() ),
	m_mainWindow( m_ctx.mainWidget() ),
	m_tabManager( m_ctx.TabManager() ),
	m_table( *m_ui.tableWidgetBD,m_ctx.mainWidget().font(),1,m_settings.textAlignment() ),
	m_tableWidgetBDList( *m_ui.TableWidgetBatchDownloaderList,0,m_ctx.mainWidget().font() ),
	m_defaultVideoThumbnail( m_settings.defaultVideoThumbnailIcon( settings::tabName::batch ) ),
	m_downloadingComments( tr( "Downloading comments" ).toUtf8() ),
	m_subtitlesTimer( m_tableWidgetBDList )
{
	qRegisterMetaType< ItemEntry >() ;
	qRegisterMetaType< ItemEntries >() ;

	this->setShowMetaData( m_settings.showMetaDataInBatchDownloader() ) ;

	connect( this,
		&batchdownloader::showMetaDataSignal,
		this,
		&batchdownloader::showMetaDataSlot,
		Qt::QueuedConnection ) ;

	connect( this,
		 &batchdownloader::downloadAddItemsSignal,
		 this,
		 &batchdownloader::downloadAddItems,
		 Qt::QueuedConnection ) ;

	connect( this,
		 &batchdownloader::networkDataSignal,
		 this,
		 &batchdownloader::networkData,
		 Qt::QueuedConnection ) ;

	connect( this,&batchdownloader::reportFStatus,
		 this,
		 &batchdownloader::reportFinishedStatus,
		 Qt::QueuedConnection ) ;

	connect( this,
		 &batchdownloader::addItemUiSignal,
		 this,
		 &batchdownloader::addItemUiSlot,
		 Qt::QueuedConnection ) ;

	connect( this,
		 &batchdownloader::addTextToUiSignal,
		 this,
		 &batchdownloader::addTextToUi,
		 Qt::QueuedConnection ) ;

	connect( this,
		 &batchdownloader::addClipboardSignal,
		 this,
		 &batchdownloader::addClipboardSlot,
		 Qt::QueuedConnection ) ;

	m_ui.pbBDPasteClipboard->setIcon( m_settings.getIcon( "clipboard" ) ) ;
	m_ui.pbBDOptionsHistory->setIcon( m_settings.getIcon( "recentlyUsed" ) ) ;
	m_ui.pbBDOptionsDownload->setIcon( m_settings.getIcon( "downloadOptions" ) ) ;

	m_tableWidgetBDList.setTableWidget( [](){

		tableWidget::tableWidgetOptions opts ;

		opts.selectionMode = QAbstractItemView::ExtendedSelection ;

		return opts ;
	}() ) ;

	m_table.setCurrentItemChanged( m_table.startPosition() ) ;

	this->setVisibleWidgetOverMainTable( false ) ;

	m_ui.pbBDDownload->setEnabled( false ) ;

	m_ui.pbBDCancel->setEnabled( false ) ;

	m_ui.BDFrame->setVisible( false ) ;

	this->resetMenu() ;

	auto cm = &QTableWidget::customContextMenuRequested ;

	m_tableWidgetBDList.connect( cm,[ this ]( const QPoint& ){

		if( m_listType == batchdownloader::listType::MEDIA_OPTIONS ){

			class meaw
			{
			public:
				meaw( tableMiniWidget< QJsonObject,5 >& m ) : m_table( m )
				{
				}
				const QJsonObject& stuffAt( int s ) const
				{
					return m_table.stuffAt( s ) ;
				}
				std::vector< int > selectedRows() const
				{
					return m_table.selectedRows() ;
				}
			private:
				const tableMiniWidget< QJsonObject,5 >& m_table ;
			} ;

			QMenu m ;

			utility::setContextMenuForDirectUrl( meaw( m_tableWidgetBDList ),m,m_ctx ) ;

			m.exec( QCursor::pos() ) ;

		}else if( m_listType == batchdownloader::listType::SUBTITLES ){

			this->saveSubtitles() ;
		}else{
			this->sortComments() ;
		}
	} ) ;

	connect( m_ui.pbBDOptionsDownload,&QPushButton::clicked,[ this ](){

		auto& t = m_ctx.TabManager().Configure() ;

		t.engineSetDefaultDownloadOptions( this->defaultEngine() ) ;
	} ) ;

	m_tableWidgetBDList.setUpHeaderMenu() ;

	m_tableWidgetBDList.connect( &QTableWidget::itemSelectionChanged,[ this ](){

		if( m_listType == batchdownloader::listType::MEDIA_OPTIONS ){

			auto& a = m_optionsList ;

			m_tableWidgetBDList.selectMediaOptions( a,m_lineEdit ) ;
		}
	} ) ;

	auto idc = &QTableWidget::itemDoubleClicked ;

	m_tableWidgetBDList.connect( idc,[ this ]( QTableWidgetItem * item ){

		if( item && m_listType != batchdownloader::listType::COMMENTS ){

			this->tableItemDoubleClicked( *item ) ;
		}
	} ) ;

	connect( m_ui.pbBatchDownloaderSet,&QPushButton::clicked,[ this ](){

		this->batchDownloaderSet() ;
	} ) ;

	connect( m_ui.pbCancelBatchDownloder,&QPushButton::clicked,[ this ](){

		m_ui.BDFrame->hide() ;
	} ) ;

	connect( m_ui.pbBDOptionsHistory,&QPushButton::clicked,[ this ](){

		const auto& engine = this->defaultEngine() ;

		auto m = m_settings.getOptionsHistory( settings::tabName::batch,engine.name() ) ;
		auto e = settings::tabName::batch ;

		if( utility::showHistory( *m_ui.lineEditBDUrlOptions,m,m_settings,engine.name(),e ) ){

			if( m_settings.autoDownload() ){

				this->download( engine ) ;
			}
		}
	} ) ;

	connect( m_ui.pbBDDownload,&QPushButton::clicked,[ this ](){

		for( int s = m_table.rowCount() - 1 ; s >= 0 ; s-- ){

			if( m_table.isRowHidden( s ) ){

				m_table.removeRow( s ) ;
			}
		}

		this->download( this->defaultEngine() ) ;
	} ) ;

	connect( m_ui.pbBDCancel,&QPushButton::clicked,[ this ](){

		m_terminator.terminateAll( m_table.get() ) ;
	} ) ;

	connect( m_ui.pbSetTimeIntervals,&QPushButton::clicked,[ this ](){

		if( m_widgetOverMainTable.showRenameUi() ){

			this->renameFile( m_widgetOverMainTable.row() ) ;
		}else{
			this->setTimeIntervals( m_widgetOverMainTable.row() ) ;
		}

		this->setVisibleWidgetOverMainTable( false ) ;
	} ) ;

	connect( m_ui.pbCancelSetTimeInterval,&QPushButton::clicked,[ this ](){

		this->setVisibleWidgetOverMainTable( false ) ;
	} ) ;

	connect( m_ui.pbBDPasteClipboard,&QPushButton::clicked,[ this ](){

		auto m = utility::clipboardText() ;

		if( m.startsWith( "http" ) ){

			this->addToList( m,{ this->showMetaData(),this->autoDownloadWhenAdded() } ) ;

		}else if( m.startsWith( "yt-dlp " ) ){

			this->addToList( m,{ false,this->autoDownloadWhenAdded() } ) ;
		}
	} ) ;

	auto cb = m_settings.monitorClipboardUrl( settings::tabName::batch ) ;

	m_ui.cbBDMonitorClipboardContent->setChecked( cb ) ;

	utility::connectQCheckBox( m_ui.cbBDMonitorClipboardContent,[ this ]( bool s ){

		m_settings.setMonitorClipboardUrl( s,settings::tabName::batch ) ;
	} ) ;

	m_table.connect( &QTableWidget::cellDoubleClicked,[ this ]( int row,int column ){

		Q_UNUSED( column )

		const auto& e = this->defaultEngine() ;
		const auto& engine = utility::resolveEngine( m_table,e,m_ctx.Engines(),row ) ;

		m_ctx.Engines().openUrls( m_table,row,engine ) ;
	} ) ;

	auto s = static_cast< void( QComboBox::* )( int ) >( &QComboBox::activated ) ;

	connect( m_ui.cbEngineTypeBD,s,[ & ]( int s ){

		if( s != -1 ){

			auto m = m_ui.cbEngineTypeBD->itemText( s ) ;

			auto e = m_settings.lastUsedOption( m,settings::tabName::batch ) ;

			m_ui.lineEditBDUrlOptions->setText( e ) ;

			m_settings.setDefaultEngine( m,settings::tabName::batch ) ;
		}
	} ) ;

	m_table.connect( &QTableWidget::customContextMenuRequested,[ this ]( QPoint ){

		this->showCustomContext() ;
	} ) ;

	connect( m_ui.pbBDQuit,&QPushButton::clicked,[ this ](){

		m_ctx.mainWindow().quitApp() ;
	} ) ;

	connect( m_ui.pbBDAdd,&QPushButton::clicked,[ this ](){

		auto m = m_ui.lineEditBDUrl->text() ;

		if( !m.isEmpty() ){

			m_ui.lineEditBDUrl->clear() ;

			if( m.startsWith( "yt-dlp " ) ){

				this->addToList( m,{ false,this->autoDownloadWhenAdded() } ) ;
			}else{
				this->addToList( m,{ this->showMetaData(),this->autoDownloadWhenAdded() } ) ;
			}
		}
	} ) ;
}

void batchdownloader::keyPressed( utility::mainWindowKeyCombo m )
{
	if( m == utility::mainWindowKeyCombo::ENTER ){

		auto m = m_ui.lineEditBDUrl->text() ;

		if( !m.isEmpty() ){

			this->addToList( m,{ false,false } ) ;
		}

		if( m_ui.pbBDDownload->isEnabled() ){

			m_ui.pbBDDownload->click() ;
		}
	}else{
		utility::keyPressed( m_table,m ) ;
	}
}

void batchdownloader::showCustomContext()
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

			m_ctx.mainWindow().resetTitle() ;

			m_table.clear() ;

		}else if( c.batchDownloaderShowHide() ){

			this->showHideControls() ;
		}
	} ;

	QMenu m ;

	if( row == -1 ){

		this->getListFromFile( m ) ;

		utility::contextState ss = m_table.noneAreRunning() ;

		ss.setBatchDownloader() ;

		return utility::appendContextMenu( m,ss,function,true,row,m_table ) ;
	}

	if( !m_table.rowIsVisible( row ) ){

		utility::contextState ss = m_table.noneAreRunning() ;

		ss.setBatchDownloader() ;

		return utility::appendContextMenu( m,ss,function,true,row,m_table ) ;
	}

	auto txt = m_table.runningState( row ) ;

	auto running = reportFinished::finishedStatus::running( txt ) ;
	auto finishSuccess = reportFinished::finishedStatus::finishedWithSuccess( txt ) ;

	auto ac = m.addAction( tr( "Open" ) ) ;

	ac->setEnabled( finishSuccess ) ;

	connect( ac,&QAction::triggered,[ this,row ](){

		const auto& e = this->defaultEngine() ;

		const auto& ee = utility::resolveEngine( m_table,e,m_ctx.Engines(),row ) ;

		m_ctx.Engines().openUrls( m_table,row,ee ) ;
	} ) ;

	ac = m.addAction( tr( "Cancel" ) ) ;

	ac->setEnabled( running ) ;

	connect( ac,&QAction::triggered,[ this,row ](){

		m_terminator.terminate( row ) ;
	} ) ;

	ac = m.addAction( tr( "Copy Url" ) ) ;

	connect( ac,&QAction::triggered,[ this ](){

		utility::copyToClipboardUrls( m_table ) ;
	} ) ;

	ac = m.addAction( tr( "Rename" ) ) ;

	auto canRename = m_table.entryAt( row ).fileNames.size() == 1 ;

	ac->setEnabled( m_table.finishedWithSuccess( row ) && canRename ) ;

	connect( ac,&QAction::triggered,[ this,row ](){

		m_widgetOverMainTable = { true,row } ;

		this->setVisibleWidgetOverMainTable( true ) ;
	} ) ;

	ac = m.addAction( tr( "Remove" ) ) ;

	ac->setEnabled( m_table.noneAreRunning() ) ;

	connect( ac,&QAction::triggered,[ this,row ](){

		Q_UNUSED( row )

		//m_table.removeRow( row ) ;

		m_table.removeAllSelected() ;

		m_ui.pbBDDownload->setEnabled( m_table.rowCount() ) ;
	} ) ;

	if( m_settings.autoHideDownloadWhenCompleted() ){

		utility::hideUnhideEntries( m,m_table,row,true ) ;
	}

	const auto& ee = this->defaultEngine() ;

	const auto& engine = utility::resolveEngine( m_table,ee,m_ctx.Engines(),row ) ;

	ac = m.addAction( tr( "Show Subtitles" ) ) ;
	ac->setEnabled( !engine.defaultSubstitlesCmdOptions().isEmpty() ) ;

	using ltty = batchdownloader::listType ;

	connect( ac,&QAction::triggered,[ this,&engine ](){

		auto row = m_table.currentRow() ;

		if( row != -1 ){

			this->showList( ltty::SUBTITLES,engine,m_table.url( row ),row ) ;
		}
	} ) ;

	ac = m.addAction( tr( "Show Comments" ) ) ;
	ac->setEnabled( engine.supportShowingComments() ) ;

	connect( ac,&QAction::triggered,[ this,&engine ](){

		auto row = m_table.currentRow() ;

		if( row != -1 ){

			this->showList( ltty::COMMENTS,engine,m_table.url( row ),row ) ;
		}
	} ) ;

	ac = m.addAction( tr( "Show Media Options" ) ) ;
	ac->setEnabled( !running && !engine.defaultListCmdOptions().isEmpty() ) ;

	connect( ac,&QAction::triggered,[ this,&engine ](){

		auto row = m_table.currentRow() ;

		if( row != -1 ){

			this->showList( ltty::MEDIA_OPTIONS,engine,m_table.url( row ),row ) ;
		}
	} ) ;

	ac = m.addAction( tr( "Download Media Part" ) ) ;
	ac->setEnabled( !finishSuccess && engine.canDownloadMediaPart() ) ;

	connect( ac,&QAction::triggered,[ this,row ](){

		m_widgetOverMainTable = { false,row } ;

		this->setVisibleWidgetOverMainTable( true ) ;
	} ) ;

	utility::addDownloadContextMenu( running,finishSuccess,m,row,[ this ]( int row ){

		auto m = m_table.uiText( row ) ;

		return m.endsWith( "\n" + engines::engine::mediaAlreadInArchiveText() ) ;

	},[ this,&engine ]( QAction * ac,bool forceDownload,int row ){

		Q_UNUSED( row )

		connect( ac,&QAction::triggered,[ &engine,this,forceDownload ](){

			for( int row = 0 ; row < m_table.rowCount() ; row++ ){

				const auto& e = m_table.runningState( row ) ;

				auto visible     = m_table.rowIsVisible( row ) ;
				auto highlighted = m_table.rowIsSelected( row ) ;

				auto m = reportFinished::finishedStatus::finishedWithSuccess( e ) ;

				if( visible && highlighted && ( !m || forceDownload ) ){

					this->downloadSingle( engine,row ) ;
				}
			}			
		} ) ;
	} ) ;

	this->getListFromFile( m ) ;

	utility::saveDownloadList( m_ctx,m,m_table,false ) ;

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

	subMenu->setTitle( QObject::tr( "Preset Options" ) ) ;

	subMenu->setEnabled( !finishSuccess ) ;

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

	utility::contextState ss = { m_table.noneAreRunning(),finishSuccess } ;

	ss.setBatchDownloader() ;

	utility::appendContextMenu( m,ss,function,true ) ;
}

void batchdownloader::init_done()
{
	auto m = m_ctx.Engines().engineDirPaths().dataPath( "autoSavedList.json" ) ;

	if( QFile::exists( m ) ){

		this->getListFromFile( m,true ) ;
	}

	m_initDone = true ;
}

void batchdownloader::resetMenu()
{
	utility::setMenuOptions( m_ctx,{},false,true,m_ui.pbBDOptions,[ this ]( QAction * aa ){

		utility::selectedAction ac( aa ) ;

		if( ac.clearOptions() ){

			m_ui.lineEditBDUrlOptions->clear() ;

		}else if( ac.clearScreen() ){

			this->clearScreen() ;

		}else if( ac.openFolderPath() ){

			utility::openDownloadFolderPath( m_settings.downloadFolder() ) ;
		}else{
			m_ui.lineEditBDUrlOptions->setText( ac.objectName() ) ;

			if( m_settings.autoDownload() ){

				this->download( this->defaultEngine() ) ;
			}
		}
	} ) ;
}

void batchdownloader::retranslateUi()
{
	this->resetMenu() ;
}

void batchdownloader::tabEntered()
{
	auto m = m_ui.cbEngineTypeBD->currentText() ;
	auto s = m_settings.lastUsedOption( m,settings::tabName::batch ) ;
	m_ui.lineEditBDUrlOptions->setText( s ) ;
	m_ui.lineEditBDUrl->setFocus() ;
}

void batchdownloader::tabExited()
{
}

void batchdownloader::exiting()
{
}

void batchdownloader::saveData()
{
	utility::saveDownloadList( m_ctx,m_table,false ) ;
}

void batchdownloader::gotEvent( const QJsonObject& jsonArgs )
{
	auto url = jsonArgs.value( "-u" ).toString() ;

	if( !url.isEmpty() ){

		m_ui.tabWidget->setCurrentIndex( 1 ) ;

		auto autoDownload = jsonArgs.value( "-a" ).toBool() ;

		if( !autoDownload ){

			autoDownload = m_settings.autoDownloadWhenAddedInBatchDownloader() ;
		}

		auto v = jsonArgs.value( "-e" ) ;

		if( v.isUndefined() ){

			this->addToList( url,{ this->showMetaData(),autoDownload } ) ;
		}else{
			this->addToList( url,{ this->showMetaData() ? v.toBool() : false,autoDownload } ) ;
		}
	}
}

void batchdownloader::updateEnginesList( const QStringList& e )
{
	auto& comboBox = *m_ui.cbEngineTypeBD ;

	comboBox.clear() ;

	for( const auto& it : e ){

		comboBox.addItem( it ) ;
	}

	auto m = this->defaultEngineName() ;
	auto s = settings::tabName::batch ;

	utility::setUpdefaultEngine( comboBox,m,m_settings,s ) ;
}

void batchdownloader::downloadAddItems( ItemEntries s )
{
	if( s.hasNext() ){

		auto e = s.next() ;

		auto row = this->addItemUi( m_defaultVideoThumbnail,-1,false,e.url ) ;

		this->setDownloadingOptions( row,m_table ) ;

		const auto& ee = m_table.entryAt( row ) ;

		const auto& eng = m_ctx.Engines().getEngineByName( ee.engineName ) ;

		if( eng ){

			this->downloadRecursively( eng.value(),row ) ;
		}else{
			this->downloadRecursively( s.engine(),row ) ;
		}

		emit this->downloadAddItemsSignal( s ) ;

	}else if( s.hasMore() ){

		s.setMore() ;

		this->addItemUiSlot( s ) ;
	}
}

void batchdownloader::downloadOrShowThumbnail( ItemEntries entries,const downloadOpts& opts )
{
	if( entries.hasMore() ){

		if( !m_initDone ){

			this->addItemUiSlot( entries.move() ) ;

		}else if( opts.autoDownload() ){

			this->downloadAddItems( entries.move() ) ;

		}else if( opts.showMetaData() && entries.engine().canShowMetaData() ){

			if( m_table.totalRunningRecursively() > 0 ){

				this->addItemUiSlot( entries.move() ) ;
			}else{
				this->showMetaDataSlot( entries.move() ) ;
			}
		}else{
			this->addItemUiSlot( entries.move() ) ;
		}
	}
}

void batchdownloader::showMetaDataSlot( ItemEntries e )
{
	if( e.hasNext() ){

		auto m = e.next() ;

		auto& c = m_ctx.TabManager().Configure() ;

		auto s = c.getEngineNameFromUrlManager( m.url ) ;

		if( s.isEmpty() ){

			this->getMetaData( e.engine(),m ) ;
		}else{
			class meaw
			{
			public:
				meaw( const QString& engineName,
				      batchdownloader& parent,
				      const Context& ctx,
				      const engines::engine& engine,
				      const Items::entry& entry ) :
				      m_parent( parent ),m_entry( entry )
				{
					auto ss = ctx.Engines().getEngineByName( engineName ) ;

					if( ss ){

						this->exec( ss.value() ) ;
					}else{
						this->exec( engine ) ;
					}
				}
			private:
				void exec( const engines::engine& engine )
				{
					if( engine.canShowMetaData() ){

						m_parent.getMetaData( engine,m_entry ) ;
					}else{
						m_parent.addItemToUi( engine,m_entry ) ;
					}
				}
				batchdownloader& m_parent ;
				const Items::entry& m_entry ;
			} ;

			meaw( s,*this,m_ctx,e.engine(),m ) ;
		}

		emit this->showMetaDataSignal( e ) ;

	}else if( e.hasMore() ){

		e.setMore() ;

		this->addItemUiSlot( e ) ;
	}
}

void batchdownloader::setThumbnail( const std::vector< QByteArray >& fileNames,
				   const engines::engine& engine,
				   int row )
{
	auto m = m_settings.downloadFolder() ;
	auto downloadFolder = engine.downloadFolder( m ) ;

	class meaw
	{
	public:
		meaw( batchdownloader& p,int row ) : m_parent( p ),m_row( row )
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
		batchdownloader& m_parent ;
		int m_row ;
		QString m_filePath ;
	} ;

	utility::setThumbNail( fileNames,downloadFolder,meaw( *this,row ) ) ;
}

void batchdownloader::getMetaData( const engines::engine& eng,const Items::entry& it )
{
	tableWidget::entry entry( it ) ;

	auto uiText        = it.uiText ;
	entry.uiText       = "...\n" + uiText ;
	entry.thumbnail    = m_defaultVideoThumbnail ;
	entry.runningState = reportFinished::finishedStatus::running() ;

	auto h = m_settings.thumbnailHeight( settings::tabName::batch ) ;

	int row = m_table.addItem( entry.move(),h ) ;

	util::Timer( 1000,[ this,row,uiText ]( int counter ){

		using ff = reportFinished::finishedStatus ;

		if( ff::running( m_table.runningState( row ) ) ){

			QString m = "..." ;

			int max = counter % 4 ;

			for( int s = 0 ; s < max ; s++ ){

				m += " ..." ;
			}

			m_table.setUiText( m + "\n" + uiText,row ) ;

			return false ;
		}else{
			return true ;
		}
	} ) ;

	m_table.selectLast() ;

	this->showThumbnail( eng,row,it.url ) ;
}

void batchdownloader::setDefaultEngineAndOptions( Items::entry& s )
{
	if( !m_settings.autoSetDefaultEngineAndOptions() ){

		return ;
	}

	auto engineName = m_ui.cbEngineTypeBD->currentText() ;
	auto opts       = m_ui.lineEditBDUrlOptions->text() ;

	s.engineName = engineName ;

	auto mm = util::split( s.uiText,"\n" ) ;

	bool engineFound = false ;

	for( auto& e : mm ){

		if( e.startsWith( "Engine Name: " ) ){

			e = "Engine Name: " + engineName ;

			engineFound = true ;

			break ;
		}
	}

	if( !engineFound ){

		mm.insert( 0,"Engine Name: " + engineName ) ;
	}

	if( s.downloadOptions.isEmpty() && !opts.isEmpty() ){

		s.downloadOptions = opts ;

		bool optionsFound = false ;

		for( auto& e : mm ){

			if( e.startsWith( "Download Options: " ) ){

				e = "Download Options: " + opts ;

				optionsFound = true ;

				break ;
			}
		}

		if( !optionsFound ){

			mm.insert( 1,"Download Options: " + opts ) ;
		}
	}

	s.uiText = mm.join( "\n" ) ;
}

void batchdownloader::addItemToUi( const engines::engine& engine,Items::entry s )
{
	auto row = this->addItemUi( m_defaultVideoThumbnail,-1,false,s.toJsonDoc() ) ;

	auto ss = m_ctx.TabManager().Configure().getEngineNameFromUrlManager( s.url ) ;

	if( !ss.isEmpty() ){

		s.engineName = ss ;
	}

	if( s.engineName.isEmpty() ){

		this->setDefaultEngineAndOptions( s ) ;
	}

	m_table.setUiText( s.uiText,row ) ;
	m_table.setEngineName( s.engineName,row ) ;
	m_table.setExtraDownloadOptions( s.downloadExtraOptions,row ) ;

	if( s.downloadOptions.isEmpty() ){

		m_ctx.TabManager().Configure().setDownloadOptions( row,m_table ) ;
	}else{
		m_table.setDownloadingOptions( s.downloadOptions,row ) ;
	}

	auto e = static_cast< int >( m_settings.maxConcurrentDownloads() ) ;

	auto m = m_table.totalRunningRecursively() ;

	if( m > 0 && m < e ){

		this->downloadRecursively( engine,row ) ;
	}
}

void batchdownloader::addItemUiSlot( ItemEntries m )
{
	if( m.hasNext() ){

		this->addItemToUi( m.engine(),m.next() ) ;

		emit this->addItemUiSignal( m ) ;

	}else if( m.hasMore() ){

		m.setMore() ;

		emit this->addItemUiSignal( m ) ;
	}
}

static QJsonArray _saveComments( const QJsonArray& arr )
{
	class comments
	{
	public:
		void add( const QJsonObject& obj )
		{
			QJsonObject oo ;

			oo.insert( "id",obj.value( "id" ) ) ;
			oo.insert( "parent",obj.value( "parent" ) ) ;
			oo.insert( "author",obj.value( "author" ) ) ;
			oo.insert( "text",obj.value( "text" ) ) ;
			oo.insert( "date",obj.value( "date" ) ) ;
			oo.insert( "text replies",obj.value( "text replies" ) ) ;

			m_objs.append( oo ) ;
		}
		void add( const QJsonObject& obj,const QString& parent )
		{
			for( int i = 0 ; i < m_objs.size() ; i++ ){

				auto m = m_objs.at( i ).toObject() ;

				if( m.value( "id" ).toString() == parent ){

					this->add( obj,m,i ) ;
				}
			}
		}
		const QJsonArray& data() const
		{
			return m_objs ;
		}
	private:
		void add( const QJsonObject& obj,QJsonObject& ss,int i )
		{
			auto replies = this->replies( ss ) ;

			QJsonObject oo ;

			oo.insert( "author",obj.value( "author" ) ) ;
			oo.insert( "text",obj.value( "text" ) ) ;
			oo.insert( "date",obj.value( "date" ) ) ;

			replies.append( oo ) ;

			ss.insert( "text replies",replies ) ;

			m_objs.replace( i,ss ) ;
		}
		QJsonArray replies( const QJsonObject& obj ) const
		{
			auto arr = obj.value( "text replies" ) ;

			if( arr.isUndefined() ){

				return QJsonArray() ;
			}else{
				return arr.toArray() ;
			}
		}
		QJsonArray m_objs ;
	} ;

	comments mm ;

	for( const auto& it : arr ){

		auto obj = it.toObject() ;

		auto parent = obj.value( "parent" ).toString() ;

		if( parent == "root" ){

			mm.add( obj ) ;
		}else{
			mm.add( obj,parent ) ;
		}
	}

	QJsonArray e ;

	for( const auto& it : mm.data() ){

		auto obj = it.toObject() ;

		obj.remove( "parent" ) ;
		obj.remove( "id" ) ;

		e.append( obj ) ;
	}

	return e ;
}

template< typename Array,typename Table >
void _add_comments( const Array& arr,Table& table )
{
	for( const auto& it : arr ){

		auto obj = it.toObject() ;

		auto parent    = obj.value( "parent" ).toString() ;
		auto txt       = obj.value( "text" ).toString() ;
		auto author    = obj.value( "author" ).toString() ;
		auto comment   = QObject::tr( "Author: %1" ).arg( author ) ;
		auto likeCount = QString::number( obj.value( "like_count" ).toInt() ) ;
		auto timestamp = obj.value( "timestamp" ) ;

		comment += "\n" + QObject::tr( "Like Count: %1" ).arg( likeCount ) ;

		if( !timestamp.isUndefined() ){

			auto a = timestamp.toInt() ;
			auto b = utility::fromSecsSinceEpoch( a ) ;

			if( !b.isEmpty() ){

				comment += "\n" + QObject::tr( "Date" ) + ": " + b ;

				obj.insert( "date",b ) ;
			}
		}

		if( parent != "root" ){

			for( const auto& xt : arr ){

				auto xobj = xt.toObject() ; ;

				auto xd = xobj.value( "id" ).toString() ;

				if( xd == parent ){

					auto xauthor = xobj.value( "author" ).toString() ;

					auto nn = QObject::tr( "Replies to" ) ;
					comment += "\n" + nn + ": " + xauthor ;

					break ;
				}
			}
		}

		comment += "\n" + txt ;

		table.add( std::move( obj ),"","","","",comment ) ;
	}
}

void batchdownloader::saveComments( const QJsonArray& arr,const QString& filePath )
{
	auto data = QJsonDocument( _saveComments( arr ) ).toJson( QJsonDocument::Indented ) ;

	engines::file( filePath,m_ctx.logger() ).write( data ) ;
}

void batchdownloader::showComments( const QByteArray& e )
{
	QJsonParseError err ;

	auto doc = QJsonDocument::fromJson( e,&err ) ;

	if( err.error == QJsonParseError::NoError ){

		auto obj = doc.object() ;

		auto f = obj.value( "title" ).toString() ;

		auto hh = utility::homePath() ;

		if( f.isEmpty() ){

			m_commentsFileName = hh + "/MediaDowloaderComments.json" ;
		}else{
			this->normalizeFilePath( f ) ;

			m_commentsFileName = hh + "/" + f.mid( 0,200 ) + ".json" ;
		}

		_add_comments( obj.value( "comments" ).toArray(),m_tableWidgetBDList ) ;
	}else{
		m_ctx.logger().setMaxProcessLog( 2 ) ;

		auto id = utility::concurrentID() ;

		m_ctx.logger().add( "Failed To Parse JSON Data: " + err.errorString(),id ) ;
	}
}

void batchdownloader::normalizeFilePath( QString& e )
{
	if( utility::platformIsLikeWindows() ){

		e.replace( '<','_' ) ;
		e.replace( '>','_' ) ;
		e.replace( ':','_' ) ;
		e.replace( '"','_' ) ;
		e.replace( '/','_' ) ;
		e.replace( '\\','_' ) ;
		e.replace( '|','_' ) ;
		e.replace( '?','_' ) ;
		e.replace( '*','_' ) ;
	}
}

void batchdownloader::setVisibleWidgetOverMainTable( bool e )
{
	if( e ){

		this->disableAll() ;
	}else{
		this->enableAll() ;
	}

	m_ui.labelWidgetOverMainTable->setVisible( e ) ;

	m_ui.pbCancelSetTimeInterval->setVisible( e ) ;
	m_ui.pbSetTimeIntervals->setVisible( e ) ;

	if( m_widgetOverMainTable.showRenameUi() ){

		m_ui.labelBDSetNewFileName->setVisible( e ) ;
		m_ui.plainTextEditBD->setVisible( e ) ;

		if( e ){

			const auto& m = m_table.entryAt( m_widgetOverMainTable.row() ).fileNames ;

			if( m.size() ){

				m_ui.plainTextEditBD->clear() ;

				m_ui.plainTextEditBD->appendPlainText( m[ 0 ] ) ;

				m_ui.plainTextEditBD->moveCursor( QTextCursor::End ) ;
			}

			m_ui.plainTextEditBD->setFocus() ;
		}

		m_ui.lineEditChapters->setVisible( false ) ;
		m_ui.lineEditStartTimeInterval->setVisible( false ) ;
		m_ui.lineEditEndTimeInterval->setVisible( false ) ;
		m_ui.cbSplitByChapters->setVisible( false ) ;
		m_ui.label_7->setVisible( false ) ;
		m_ui.label_8->setVisible( false ) ;
		m_ui.label_9->setVisible( false ) ;
	}else{
		if( e ){

			m_ui.lineEditStartTimeInterval->setFocus() ;
		}else{
			m_ui.lineEditBDUrl->setFocus() ;
		}

		m_ui.lineEditChapters->setVisible( e ) ;
		m_ui.lineEditStartTimeInterval->setVisible( e ) ;
		m_ui.lineEditEndTimeInterval->setVisible( e ) ;
		m_ui.cbSplitByChapters->setVisible( e ) ;
		m_ui.label_7->setVisible( e ) ;
		m_ui.label_8->setVisible( e ) ;
		m_ui.label_9->setVisible( e ) ;
		m_ui.labelBDSetNewFileName->setVisible( false ) ;
		m_ui.plainTextEditBD->setVisible( false ) ;
	}
}

void batchdownloader::renameFile( int row )
{
	const auto& fn = m_table.entryAt( row ).fileNames ;

	if( fn.size() ){

		auto nn = m_ui.plainTextEditBD->toPlainText() ;
		auto df = m_ctx.Settings().downloadFolder() ;

		auto& item = m_table.item( row,m_table.startPosition() ) ;

		auto m = utility::rename( m_ctx,item,df,nn,fn[ 0 ] ) ;

		if( !m.isEmpty() ){

			std::vector< QByteArray > s ;
			s.emplace_back( m.toUtf8() ) ;

			m_table.setFileNames( row,std::move( s ) ) ;
		}
	}
}

void batchdownloader::setTimeIntervals( int row )
{
	if( row != -1 ){

		auto a = m_ui.lineEditStartTimeInterval->text() ;
		auto b = m_ui.lineEditEndTimeInterval->text() ;
		auto c = m_ui.lineEditChapters->text() ;

		if( !a.isEmpty() && !b.isEmpty() ){

			auto u = tableWidget::type::DownloadTimeInterval ;

			m_table.setDownloadingOptions( u,row,a + "-" + b ) ;

		}else if( a.isEmpty() && b.isEmpty() ){

			//Left empty on purpose
		}else{
			return ;
		}

		if( !c.isEmpty() ){

			auto u = tableWidget::type::DownloadChapters ;

			m_table.setDownloadingOptions( u,row,c ) ;
		}

		if( m_ui.cbSplitByChapters->isChecked() ){

			auto u = tableWidget::type::SplitByChapters ;

			m_table.setDownloadingOptions( u,row,"Yes" ) ;
		}else{
			auto u = tableWidget::type::SplitByChapters ;

			m_table.setDownloadingOptions( u,row,"No" ) ;
		}
	}
}

void batchdownloader::showSubtitles( const QByteArray& e )
{
	class language
	{
	public:
		language( const QJsonObject::ConstIterator& it ) :
			m_name( it.key() ),
			m_formats( it.value().toArray() )
		{
		}
		const QString& name() const
		{
			return m_name ;
		}
		QString notes() const
		{
			if( m_formats.isEmpty() ){

				return {} ;
			}else{
				auto iter = utility::reverse( m_formats ) ;

				auto obj = iter.nextAsValue().toObject() ;

				auto name = "Name: " + obj.value( "name" ).toString() ;

				auto formats = "Formats: " + obj.value( "ext" ).toString() ;

				iter.forEach( [ & ]( const QJsonValue& v ){

					formats += ", " + v.toObject().value( "ext" ).toString() ;
				} ) ;

				return name + "\n" + formats ;
			}
		}
		const QJsonArray& subtitles() const
		{
			return m_formats ;
		}
	private:
		QString m_name ;
		QJsonArray m_formats ;
	} ;

	QJsonParseError err ;

	auto doc = QJsonDocument::fromJson( e,&err ) ;

	if( err.error == QJsonParseError::NoError ){

		auto _parse = [ & ]( const QJsonValue& j ){

			utility::vector< language > ll ;

			ll.addAsIterator( j.toObject() ) ;

			ll.sort( []( const language& l,const language& r ){

				return l.name() < r.name() ;
			} ) ;

			return ll ;
		} ;

		auto obj = doc.object() ;

		auto title = obj.value( "title" ).toString() ;

		auto _add = [ & ]( const language& l,const QString& m ){

			QJsonObject obj ;

			obj.insert( "id",l.name() ) ;
			obj.insert( "extension",m ) ;
			obj.insert( "resolution",l.subtitles() ) ;
			obj.insert( "filesize",title ) ;
			obj.insert( "info",l.notes() ) ;

			return obj ;
		} ;

		_parse( obj.value( "subtitles" ) ).each( [ & ]( const language& it ){

			auto obj = _add( it,"subtitles" ) ;

			auto e = "subtitle" ;

			m_tableWidgetBDList.add( std::move( obj ),it.name(),e,"","",it.notes() ) ;
		} ) ;

		_parse( obj.value( "automatic_captions" ) ).each( [ & ]( const language& it ){

			auto obj = _add( it,"automatic_captions" ) ;

			auto e = "automatic\ncaption" ;

			m_tableWidgetBDList.add( std::move( obj ),it.name(),e,"","",it.notes() ) ;
		} ) ;
	}
}

void batchdownloader::saveSubtitles()
{
	auto row = m_tableWidgetBDList.currentRow() ;

	if( row == -1 ){

		return ;
	}

	const auto& s = m_tableWidgetBDList.stuffAt( row ) ;

	auto title = s.value( "filesize" ).toString() ;
	auto subtitles = s.value( "resolution" ).toArray() ;

	if( subtitles.isEmpty() ){

		return ;
	}

	this->normalizeFilePath( title ) ;

	struct entry
	{
		entry( const QJsonObject& obj ) :
			name( obj.value( "name" ).toString() ),
			ext( obj.value( "ext" ).toString() ),
			url( obj.value( "url" ).toString() )
		{
		}
		QString name ;
		QString ext ;
		QString url ;
	};

	utility::vector< entry > ee ;

	QMenu m ;

	utility::reverse( subtitles ).forEach( [ & ]( const QJsonValue& v ){

		ee.emplace_back( v.toObject() ) ;

		const auto& ext = ee.back().ext ;

		m.addAction( tr( "Download" ) + " " + ext )->setObjectName( ext ) ;
	} ) ;

	class meaw
	{
	public:
		meaw( batchdownloader& p,utility::vector< entry > e,const QString& t ) :
			m_parent( p ),m_entries( std::move( e ) ),m_title( t )
		{
		}
		void operator()( QAction * ac )
		{
			auto ext = ac->objectName() ;

			m_entries.each( [ & ]( const entry& it ){

				if( it.ext == ext ){

					return m_parent.saveSubtitles( it.url,it.ext,m_title ) ;
				}else{
					return false ;
				}
			} ) ;
		}
	private:
		batchdownloader& m_parent ;
		utility::vector< entry > m_entries ;
		QString m_title ;
	} ;

	connect( &m,&QMenu::triggered,meaw( *this,std::move( ee ),title ) ) ;

	m.exec( QCursor::pos() ) ;
}

template< typename Table,typename Cmp >
auto _make_sort( const char * key,Table& table,Cmp cmp )
{
	class sort
	{
	public:
		sort( const char * key,Table& table,Cmp cmp ) :
			m_key( key ),m_table( table ),m_cmp( std::move( cmp ) )
		{
		}
		void operator()()
		{
			class obj
			{
			public:
				obj( const char * key,QJsonObject obj ) :
					m_key( key ),m_obj( std::move( obj ) )
				{
				}
				operator int() const
				{
					return m_obj.value( m_key ).toInt() ;
				}
				QJsonObject toObject() const
				{
					return m_obj ;
				}
			private:
				const char * m_key ;
				QJsonObject m_obj ;
			} ;

			std::vector< obj > m ;

			for( int i = 0 ; i < m_table.rowCount() ; i++ ){

				m.emplace_back( m_key,m_table.stuffAt( i ) ) ;
			}

			std::sort( m.begin(),m.end(),std::move( m_cmp ) ) ;

			m_table.clear() ;

			_add_comments( m,m_table ) ;
		}
	private:
		const char * m_key ;
		Table& m_table ;
		Cmp m_cmp ;
	} ;

	return sort( key,table,std::move( cmp ) ) ;
}

void batchdownloader::sortComments()
{
	QMenu m ;

	connect( m.addAction( tr( "Sort By Date Ascending" ) ),
		 &QAction::triggered,
		 _make_sort( "timestamp",m_tableWidgetBDList,std::less<int>() ) ) ;

	connect( m.addAction( tr( "Sort By Date Descending" ) ),
		 &QAction::triggered,
		 _make_sort( "timestamp",m_tableWidgetBDList,std::greater<int>() ) ) ;

	connect( m.addAction( tr( "Sort By Likes" ) ),
		 &QAction::triggered,
		 _make_sort( "like_count",m_tableWidgetBDList,std::greater<int>() ) ) ;

	m.exec( QCursor::pos() ) ;
}

bool batchdownloader::saveSubtitles( const QString& url,const QString& ext,const QString& title )
{
	auto flatpak = utility::platformisFlatPak() ;

	auto df = flatpak ? utility::homePath() : m_ctx.Settings().downloadFolder() ;
	auto m = df + "/" + title + "." + ext ;

	auto s = QObject::tr( "Save Subtitle To File" ) ;
	auto e = QFileDialog::getSaveFileName( &m_ctx.mainWidget(),s,m ) ;

	if( !e.isEmpty() ){

		m_ctx.network().get( url,[ e,this ]( const utils::network::reply& reply ){

			auto s = utility::networkReply( m_ctx,reply ).data() ;

			QFile f( e ) ;

			if( f.open( QIODevice::WriteOnly | QIODevice::Truncate ) ){

				f.write( s ) ;
			}else{
				auto x = QObject::tr( "Failed To Open Path For Writing: %1" ).arg( e ) ;
				m_ctx.logger().add( x,utility::concurrentID() ) ;
			}
		} ) ;
	}

	return true ;
}

QString batchdownloader::setSubtitleString( const QJsonObject& obj,const QString& m )
{
	auto type = obj.value( "type" ).toString() ;

	if( type == "automatic_captions" ){

		return "ac: " + m ;
	}else{
		return "su: " + m ;
	}
}

void batchdownloader::showBDFrame( batchdownloader::listType m )
{
	m_listType = m ;

	auto& table = m_tableWidgetBDList.get() ;

	table.showColumn( 0 ) ;
	table.showColumn( 1 ) ;
	table.showColumn( 2 ) ;
	table.showColumn( 3 ) ;

	if( m == batchdownloader::listType::MEDIA_OPTIONS ){

		m_ui.pbBatchDownloaderSet->setText( tr( "Set" ) ) ;

	}else if( m == batchdownloader::listType::COMMENTS ){

		table.hideColumn( 0 ) ;
		table.hideColumn( 1 ) ;
		table.hideColumn( 2 ) ;
		table.hideColumn( 3 ) ;

		m_ui.pbBatchDownloaderSet->setText( tr( "Save" ) ) ;

	}else if( m == batchdownloader::listType::SUBTITLES ){

		table.hideColumn( 2 ) ;
		table.hideColumn( 3 ) ;
		m_ui.pbBatchDownloaderSet->setText( tr( "Set" ) ) ;
	}

	m_optionsList.clear() ;
	m_tableWidgetBDList.clear() ;
	m_ui.BDFrame->show() ;
	m_ui.pbCancelBatchDownloder->setFocus() ;
}

void batchdownloader::parseDataFromFile( Items& items,const QByteArray& data )
{
	QJsonParseError err ;

	auto json = QJsonDocument::fromJson( data,&err ) ;

	if( err.error == QJsonParseError::NoError ){

		this->parseDataFromObject( items,json.object(),json.array() ) ;
	}
}

void batchdownloader::parseItems( Items items,const batchdownloader::downloadOpts& opts )
{
	const auto& engine    = this->defaultEngine() ;
	auto currentlyRunning = m_table.numberCurrentlyRunning() ;
	auto maxRunning       = static_cast< int >( m_settings.maxConcurrentDownloads() ) ;
	auto size             = static_cast< int >( items.size() ) ;
	auto availableSlots   = maxRunning - currentlyRunning ;

	Items itms ;

	for( int i = 0 ; i < size && i < availableSlots ; i++ ){

		itms.add( items.takeFirst() ) ;
	}

	this->downloadOrShowThumbnail( { { engine,itms.move() },{ engine,items.move() } },opts ) ;
}

void batchdownloader::parseDataFromObject( Items& items,const QJsonObject& obj,const QJsonArray& array )
{
	if( obj.isEmpty() ){

		/*
		 * File created by us
		 */
		auto function = []( const QJsonValue& e ){

			return e.toString() ;
		} ;

		auto a = "url" ;
		auto b = "uploadDate" ;

		this->dataFromFile( items,{ array,a,b },function ) ;
	}else{
		/*
		 * File created with yt-dlp
		 */
		auto array = obj.value( "entries" ).toArray() ;

		auto function = []( const QJsonValue& e ){

			using tt = engines::engine::baseEngine::timer ;
			return tt::duration( e.toInt() * 1000 ) ;
		} ;

		if( !array.isEmpty() ){

			auto a = "webpage_url" ;
			auto b = "upload_date" ;

			this->dataFromFile( items,{ array,a,b },function ) ;
		}
	}
}

void batchdownloader::getListFromFile( const QString& e,bool deleteFile )
{
	engines::file::readAll( e,m_ctx.logger(),[ this,deleteFile,e ]( bool,QByteArray list ){

		if( deleteFile ){

			QFile::remove( e ) ;
		}

		if( !list.isEmpty() ){

			Items items ;

			if( list.startsWith( '[' ) || list.startsWith( '{' ) ){

				this->parseDataFromFile( items,list ) ;
			}else{
				list.replace( "\r","" ) ;

				for( const auto& it : util::split( list,'\n',true ) ){

					if( it.startsWith( "http" ) ){

						items.add( it ) ;
					}
				}
			}

			if( items.size() ){

				m_ui.tabWidget->setCurrentIndex( 1 ) ;
				this->parseItems( items.move(),{ false,false } ) ;
			}
		}
	} ) ;
}

void batchdownloader::getListFromFile( QMenu& m )
{
	auto ac = m.addAction( QObject::tr( "Get List From File" ) ) ;

	QObject::connect( ac,&QAction::triggered,[ this ](){

		auto a = &m_ctx.mainWindow() ;
		auto b = tr( "Set Batch File" ) ;
		auto c = utility::homePath() ;

		auto e = QFileDialog::getOpenFileName( a,b,c ) ;

		if( !e.isEmpty() ){

			this->getListFromFile( e,false ) ;
		}		
	} ) ;
}

QString batchdownloader::defaultEngineName()
{
	auto mm = m_ctx.Engines().defaultEngineName() ;
	return m_settings.defaultEngine( settings::tabName::batch,mm ) ;
}

const engines::engine& batchdownloader::defaultEngine()
{
	auto id = utility::concurrentID() ;

	return m_ctx.Engines().defaultEngine( this->defaultEngineName(),id ) ;
}

void batchdownloader::tableItemDoubleClicked( QTableWidgetItem& item )
{
	auto row = item.row() ;

	auto m = m_tableWidgetBDList.item( row,0 ).text() ;

	if( !m.isEmpty() ){

		auto crow = m_table.currentRow() ;

		if( m_listType == batchdownloader::listType::SUBTITLES ){

			auto u = tableWidget::type::subtitleOption ;

			auto obj = m_tableWidgetBDList.stuffAt( row ) ;

			m = this->setSubtitleString( obj,m ) ;

			m_table.setDownloadingOptions( u,crow,m ) ;
		}else{
			auto u = tableWidget::type::DownloadOptions ;

			m_table.setDownloadingOptions( u,crow,m ) ;
		}
	}

	m_ui.BDFrame->hide() ;
}

void batchdownloader::batchDownloaderSet()
{
	if( m_listType == batchdownloader::listType::COMMENTS ){

		auto a = &m_ctx.mainWidget() ;

		auto b = QObject::tr( "Save List To File" ) ;
		auto e = QFileDialog::getSaveFileName( a,b,m_commentsFileName ) ;

		if( e.isEmpty() ){

			return ;
		}

		int rowCount = m_tableWidgetBDList.rowCount() ;

		QJsonArray arr ;

		for( int i = 0 ; i < rowCount ; i++ ){

			arr.append( m_tableWidgetBDList.stuffAt( i ) ) ;
		}

		this->saveComments( arr,e ) ;
	}else{
		auto crow = m_table.currentRow() ;

		if( m_listType == batchdownloader::listType::SUBTITLES ){

			auto row = m_tableWidgetBDList.currentRow() ;

			auto m = m_tableWidgetBDList.item( row,0 ).text() ;

			auto obj = m_tableWidgetBDList.stuffAt( row ) ;

			m = this->setSubtitleString( obj,m ) ;

			auto u = tableWidget::type::subtitleOption ;

			m_table.setDownloadingOptions( u,crow,m ) ;
		}else{
			auto m = m_lineEdit.text() ;

			auto u = tableWidget::type::DownloadOptions ;

			m_table.setDownloadingOptions( u,crow,m ) ;
		}

		m_ui.BDFrame->hide() ;
	}
}

void batchdownloader::updateTitleBar()
{
	m_ctx.mainWindow().setTitle( m_table.completeProgress( 0 ) ) ;
}

void batchdownloader::showThumbnail( const engines::engine& engine,
				    int index,
				    const QString& url )
{			
	class events
	{
	public:
		events( batchdownloader& p,
			const engines::engine& engine,
			int index,
			const QString& url,
			BatchLoggerWrapper< batchdownloader::defaultLogger > logger ) :
			m_parent( p ),
			m_engine( engine ),
			m_index( index ),
			m_url( url ),
			m_logger( logger )
		{
		}
		bool addData( const QByteArray& e )
		{
			return utility::addData( e ) ;
		}
		void done( engines::ProcessExitState,const std::vector< QByteArray >& )
		{
			auto enableAll = false ;

			auto data = m_logger.data() ;

			auto s = data.indexOf( "[media-downloader]" ) ;

			if( s != -1 ){

				data.truncate( s ) ;
			}

			utility::MediaEntry m( m_url,m_engine,data ) ;

			if( m.valid() ){

				if( m.url().isEmpty() ){

					m.setUrl( m_url ) ;
				}

				m_parent.addItem( this->index(),enableAll,m.move() ) ;
			}else{
				m_parent.addItem( this->index(),enableAll,m_url ) ;
			}
		}
		void disableAll()
		{
			m_parent.disableAll() ;

			m_parent.m_ui.pbBDPasteClipboard->setEnabled( true ) ;

			m_parent.m_ui.pbBDAdd->setEnabled( true ) ;

			m_parent.m_ui.lineEditBDUrl->setEnabled( true ) ;

			m_parent.m_ui.pbBDCancel->setEnabled( true ) ;

			m_parent.m_table.setEnabled( true ) ;
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
		const engines::engine& engine()
		{
			return m_engine ;
		}
	private:
		batchdownloader& m_parent ;
		const engines::engine& m_engine ;
		int m_index ;
		QString m_url ;
		BatchLoggerWrapper< batchdownloader::defaultLogger > m_logger ;
	} ;

	auto args = engine.dumpJsonArguments( engines::engine::tab::batch ) ;

	utility::setCookieOption( args,m_settings,engine ) ;

	auto m = m_ui.lineEditBDUrlOptions->text() ;

	utility::addToListOptionsFromsDownload( args,m,m_ctx,engine ) ;

	engine.setTextEncondig( args ) ;

	engine.updateCmdOptions( args ) ;

	m_ctx.logger().setMaxProcessLog( m_table.rowCount() + 1 ) ;

	batchdownloader::defaultLogger ll ;

	auto logs = m_ctx.Settings().getLogsLimits() ;

	BatchLoggerWrapper< batchdownloader::defaultLogger > wrapper( m_ctx.logger(),logs,ll.move() ) ;

	args.append( m_table.url( index ) ) ;

	auto ctx = utility::make_ctx( m_ctx,
				      events( *this,engine,index,url,wrapper ),
				      wrapper,
				      m_terminator.setUp(),
				      QProcess::ProcessChannel::StandardOutput ) ;

	utility::run( args,QString(),ctx.move() ) ;
}

int batchdownloader::addItemUi( const QPixmap& pixmap,
				int index,
				tableWidget& table,
				Ui::MainWindow& ui,
				const utility::MediaEntry& media )
{
	auto state = reportFinished::finishedStatus::notStarted() ;

	int row ;

	auto h = m_settings.thumbnailHeight( settings::tabName::batch ) ;

	if( index == -1 ){

		row = table.addItem( { pixmap,state,media },h ) ;

		table.selectLast() ;
	}else{
		row = index ;

		table.replace( { pixmap,state,media },index,h ) ;
	}

	ui.lineEditBDUrl->clear() ;

	ui.lineEditBDUrl->setFocus() ;

	return row ;
}

int batchdownloader::addItemUi( const QPixmap& pixmap,
				int index,
				bool enableAll,
				const utility::MediaEntry& media )
{
	Q_UNUSED( enableAll )

	auto row = this->addItemUi( pixmap,index,m_table,m_ui,media ) ;

	this->setDownloadingOptions( row,m_table ) ;

	if( m_table.noneAreRunning() ){

		m_ui.pbBDDownload->setEnabled( true ) ;

		this->enableAll() ;
		m_ui.pbBDCancel->setEnabled( false ) ;
	}else{
		m_ui.pbBDCancel->setEnabled( true ) ;
	}

	return row ;
}

void batchdownloader::setShowMetaData( bool e )
{
	if( e ){

		m_table.get().showColumn( 0 ) ;

		auto m = m_settings.thumbnailWidth( settings::tabName::batch ) ;
		m_table.get().setColumnWidth( 0,m ) ;
	}else{
		m_table.get().hideColumn( 0 ) ;
	}
}

void batchdownloader::showComments( const engines::engine& engine,const QString& url )
{
	m_ctx.Ui().tabWidget->setCurrentIndex( 1 ) ;

	this->showList( batchdownloader::listType::COMMENTS,engine,url,-1 ) ;
}

void batchdownloader::textAlignmentChanged( Qt::LayoutDirection m )
{
	auto a = m_ui.labelBDEnterOptions ;
	auto b = m_ui.labelBDEnterUrl ;
	auto c = m_ui.labelBDEngineName ;

	utility::alignText( m,a,b,c ) ;
}

void batchdownloader::dataFromFile( Items& items,
				   const batchdownloader::dataFromFileOpts& dFileopts,
				   QJsonObject& obj,
				   const QString& drion )
{
	auto url = obj.value( dFileopts.urlKey ).toString() ;

	obj.insert( "webpage_url",url ) ;

	auto d = utility::stringConstants::duration() + " " ;
	auto u = utility::stringConstants::uploadDate() + " " ;

	auto title    = obj.value( "title" ).toString() ;
	auto date     = obj.value( dFileopts.uploadDate ).toString() ;

	auto engineName      = obj.value( "engineName" ).toString() ;
	auto downloadOpts    = obj.value( "downloadOptions" ).toString() ;
	auto downloadExtOpts = obj.value( "downloadExtraOptions" ).toString() ;

	auto duration = drion ;

	if( !duration.isEmpty() ){

		duration = d + duration ;
	}

	if( !date.isEmpty() ){

		date = u + date ;
	}

	QString durationAndDate ;

	if( !duration.isEmpty() && !date.isEmpty() ){

		durationAndDate = duration + ", " + date ;

	}else if( duration.isEmpty() ){

		if( date.isEmpty() ){

		}else{
			durationAndDate = date ;
		}

	}else if( date.isEmpty() ){

		if( duration.isEmpty() ){

		}else{
			durationAndDate = duration ;
		}
	}

	QString opts ;

	if( !engineName.isEmpty() ){

		opts = utility::stringConstants::engineName() + engineName ;
	}

	if( !downloadOpts.isEmpty() ){

		auto& cc = m_ctx.TabManager().Configure() ;
		downloadOpts = cc.optionsTranslated( downloadOpts ) ;

		auto mm = utility::stringConstants::downloadOptions() ;
		auto dopts = mm + ": " + downloadOpts ;

		if( opts.isEmpty() ){

			opts = dopts ;
		}else{
			opts += "\n" + dopts ;
		}
	}

	if( !downloadExtOpts.isEmpty() ){

		auto mm = utility::stringConstants::downloadExtendedOptions() ;

		auto dopts = mm + ": " + downloadExtOpts ;

		if( opts.isEmpty() ){

			opts = dopts ;
		}else{
			opts += "\n" + dopts ;
		}
	}

	auto _set_title = []( const QString& e ){

		if( e.isEmpty() ){

			return QString() ;
		}else{
			return "\n" + e ;
		}
	} ;

	if( opts.isEmpty() ){

		if( durationAndDate.isEmpty() ){

			obj.insert( "uiText",title ) ;
		}else{
			auto txt = durationAndDate + _set_title( title ) ;
			obj.insert( "uiText",txt ) ;
		}
	}else{
		if( durationAndDate.isEmpty() ){

			obj.insert( "uiText",opts + _set_title( title ) ) ;
		}else{
			auto txt = opts + "\n" + durationAndDate + _set_title( title ) ;

			obj.insert( "uiText",txt ) ;
		}
	}

	items.add( std::move( obj ) ) ;
}

bool batchdownloader::showMetaData()
{
	return m_settings.showMetaDataInBatchDownloader() ;
}

bool batchdownloader::autoDownloadWhenAdded()
{
	return m_settings.autoDownloadWhenAddedInBatchDownloader() ;
}

void batchdownloader::clipboardData( const QString& url,bool s )
{
	if( s ){

		emit this->addClipboardSignal( url ) ;
	}else{
		m_ctx.logger().add( url,utility::concurrentID() ) ;
	}
}

void batchdownloader::addClipboardSlot( QString url )
{
	if( m_settings.monitorClipboardUrl( settings::tabName::batch ) ){

		if( m_table.rowWithUrl( url ) == -1 ){

			m_ui.tabWidget->setCurrentIndex( 1 ) ;

			if( url.startsWith( "yt-dlp " ) ){

				this->addToList( url,{ false,this->autoDownloadWhenAdded() } ) ;
			}else{
				this->addToList( url,{ this->showMetaData(),this->autoDownloadWhenAdded() } ) ;
			}
		}
	}
}

void batchdownloader::clearScreen()
{
	m_table.clear() ;
	m_ui.lineEditBDUrlOptions->clear() ;
	m_ui.lineEditBDUrl->clear() ;
}

void batchdownloader::hideBasicDownloaderTableList()
{
	m_ctx.TabManager().basicDownloader().hideTableList() ;
}

void batchdownloader::showList( batchdownloader::listType listType,
				const engines::engine& engine,
				const QString& url,
				int row )
{
	QStringList args ;

	if( listType == batchdownloader::listType::SUBTITLES ){

		args = engine.defaultSubstitlesCmdOptions() ;

		this->showBDFrame( listType ) ;

		m_tableWidgetBDList.add( "","","","","\n" + tr( "Downloading subtitles" ) + "\n" ) ;

		m_subtitlesTimer.start() ;

	}else if( listType == batchdownloader::listType::COMMENTS ){

		args = engine.defaultCommentsCmdOptions() ;
		args.append( "--verbose" ) ;

		this->showBDFrame( listType ) ;

		m_tableWidgetBDList.add( "","","","","\n" + m_downloadingComments + "\n" ) ;
	}else{
		auto& table = m_tableWidgetBDList.get() ;

		table.setHorizontalHeaderLabels( engine.horizontalHeaderLabels() ) ;

		if( row != -1 ){

			this->showBDFrame( listType ) ;

			args = engine.defaultListCmdOptions() ;

			const auto& mp = m_table.mediaProperties( row ) ;

			if( !mp.isEmpty() ){

				auto data = m_table.mediaEntry( row ) ;

				const auto ss = engine.mediaProperties( m_ctx.logger(),data ) ;

				if( !ss.empty() ){

					for( const auto& m : ss ){

						m_tableWidgetBDList.add( m.toqJsonObject(),m ) ;
					}

					return ;
				}
			}
		}
	}

	utility::setCookieOption( args,m_settings,engine ) ;

	auto m = m_ui.lineEditBDUrlOptions->text() ;

	utility::addToListOptionsFromsDownload( args,m,m_ctx,engine ) ;

	engine.setTextEncondig( args ) ;

	engine.updateCmdOptions( args ) ;

	args.append( url ) ;

	this->disableAll() ;

	class outPut
	{
	public:
		outPut( batchdownloader& b,batchdownloader::listType l ) :
			m_parent( b ),m_listType( l )
		{
		}
		outPut move()
		{
			return std::move( *this ) ;
		}
		bool operator()( const QByteArray& data )
		{
			if( m_listType == batchdownloader::listType::SUBTITLES ){

				return true ;

			}else if( m_listType == batchdownloader::listType::COMMENTS ){

				if( utils::misc::containsAny( data,"WARNING","ERROR" ) ){

					return true ;
				}

				if( data.contains( "Downloading comment API JSON reply " ) ){

					this->downloadComments( data ) ;
				}

				return false ;
			}else{
				return true ;
			}
		}
		void downloadComments( const QByteArray& data )
		{
			auto m = data.indexOf( '(' ) ;

			if( m != -1 ){

				auto w = data.mid( m + 1 ) ;

				m = w.indexOf( '\n' ) ;

				if( m != -1 ){

					w.truncate( m - 1 ) ;
				}

				w = "\n" + m_parent.m_downloadingComments + ": " + w + "\n" ;

				m_parent.m_tableWidgetBDList.replace( 0,"","","","",w ) ;
			}else{
				auto m = data.indexOf( "Downloading" ) ;

				auto w = "\n" + data.mid( m ).trimmed() + "\n" ;

				m_parent.m_tableWidgetBDList.replace( 0,"","","","",w ) ;
			}
		}
	private:
		batchdownloader& m_parent ;
		batchdownloader::listType m_listType ;
	} ;

	class events
	{
	public:
		events( batchdownloader& p,
			batchdownloader::listType l,
			const engines::engine& engine,
			int row ) :
			m_parent( p ),m_listType( l ),m_engine( engine ),m_row( row )
		{
		}
		const engines::engine& engine()
		{
			return m_engine ;
		}
		bool addData( const QByteArray& e )
		{
			if( utility::addData( e ) ){

				m_listData += e ;

				return true ;
			}else{
				return false ;
			}
		}
		void done( engines::ProcessExitState st,const std::vector< QByteArray >& )
		{
			m_parent.enableAll() ;

			m_parent.m_ui.lineEditBDUrl->setFocus() ;

			this->list( st,m_listData ) ;
		}
		void disableAll()
		{
			m_parent.disableAll() ;

			m_parent.m_ui.pbCancelBatchDownloder->setEnabled( true ) ;
			m_parent.m_ui.pbCancelBatchDownloder->setFocus() ;
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
		void processData( const QByteArray& a )
		{
			using lt = batchdownloader::listType ;

			if( m_listType == lt::SUBTITLES ){

				m_parent.showSubtitles( a ) ;

			}else if( m_listType == lt::COMMENTS ){

				m_parent.showComments( a ) ;
			}else{
				auto& logger = m_parent.m_ctx.logger() ;

				auto ee = m_engine.mediaProperties( logger,a ) ;

				auto& t = m_parent.m_tableWidgetBDList ;

				for( const auto& m : ee ){

					t.add( m.toqJsonObject(),m ) ;
				}

				auto m = QJsonDocument::fromJson( a ) ;

				auto array = m.object().value( "formats" ).toArray() ;

				m_parent.m_table.replace( array,m_row ) ;
			}
		}
		void list( const engines::ProcessExitState&,const QByteArray& a )
		{
			using lt = batchdownloader::listType ;

			if( m_listType != lt::MEDIA_OPTIONS ){

				if( m_listType == lt::SUBTITLES ){

					m_parent.m_subtitlesTimer.stop() ;
				}

				m_parent.m_tableWidgetBDList.removeRow( 0 ) ;
			}

			if( !a.isEmpty() ){

				this->processData( a ) ;
			}
		}
		batchdownloader& m_parent ;
		batchdownloader::listType m_listType ;
		const engines::engine& m_engine ;
		QByteArray m_listData ;
		int m_row ;
	} ;

	auto term = m_terminator.setUp( m_ui.pbCancelBatchDownloder,&QPushButton::clicked,-1 ) ;
	auto ch   = QProcess::ProcessChannel::StandardOutput ;

	engine.updateOutPutChannel( ch ) ;

	m_ctx.logger().setMaxProcessLog( 1 ) ;

	auto logs = m_ctx.Settings().getLogsLimits() ;

	BatchLoggerWrapper< outPut > logger( m_ctx.logger(),logs,outPut( *this,listType ) ) ;

	events ev( *this,listType,engine,row ) ;

	auto ctx = utility::make_ctx( m_ctx,ev.move(),logger.move(),term.move(),ch ) ;

	utility::run( args,QString(),ctx.move() ) ;
}

void batchdownloader::setDownloadingOptions( int row,tableWidget& table )
{
	m_ctx.TabManager().Configure().setDownloadOptions( row,table ) ;

	if( m_table.engineName( row ).isEmpty() ){

		Items::entry s( m_table.uiText( row ),m_table.url( row ) ) ;

		s.downloadOptions = m_table.downloadingOptions( row ) ;

		this->setDefaultEngineAndOptions( s ) ;

		m_table.setUiText( s.uiText,row ) ;
		m_table.setDownloadingOptions( s.downloadOptions,row ) ;
		m_table.setEngineName( s.engineName,row ) ;
	}
}

void batchdownloader::addItemUi( int index,bool enableAll,const utility::MediaEntry& media )
{
	this->addItemUi( m_defaultVideoThumbnail,index,enableAll,media ) ;
}

void batchdownloader::networkData( utility::networkReply m )
{
	if( networkAccess::hasNetworkSupport() ){

		QPixmap pixmap ;

		if( m.success() && pixmap.loadFromData( m.data() ) ){

			auto a = settings::tabName::batch ;

			auto w = m_settings.thumbnailWidth( a ) ;
			auto h = m_settings.thumbnailHeight( a ) ;

			auto p = pixmap.scaled( w,h ) ;

			this->addItemUi( p,m.index(),m_table,m_ui,m.media() ) ;
		}else{
			auto& p = m_defaultVideoThumbnail ;

			this->addItemUi( p,m.index(),m_table,m_ui,m.media() ) ;
		}
	}else{
		this->addItemUi( m_defaultVideoThumbnail,m.index(),m_table,m_ui,m.media() ) ;
	}

	this->setDownloadingOptions( m.index(),m_table ) ;

	if( m_table.noneAreRunning() ){

		m_ui.pbBDDownload->setEnabled( true ) ;

		this->enableAll() ;

		m_ui.pbBDCancel->setEnabled( false ) ;
	}
}

void batchdownloader::addItem( int index,bool enableAll,const utility::MediaEntry& media )
{
	if( media.thumbnailUrl().isEmpty() ){

		this->addItemUi( index,enableAll,media ) ;

	}else if( this->showMetaData() && networkAccess::hasNetworkSupport() ){

		auto u = media.thumbnailUrl() ;

		networkCtx n{ media,index } ;

		auto m = &batchdownloader::networkResult ;

		m_ctx.network().get( u,n.move(),this,m ) ;
	}else{
		this->addItemUi( index,enableAll,media ) ;
	}
}

void batchdownloader::networkResult( networkCtx d,const utils::network::reply& reply )
{
	emit this->networkDataSignal( { m_ctx,reply,d.index,d.media.move() } ) ;
}

void batchdownloader::addToList( const QString& u,const batchdownloader::downloadOpts& opts )
{
	class meaw
	{
	public:
		meaw( batchdownloader& p,const QString& url,const batchdownloader::downloadOpts& opts ) :
			m_parent( p ),
			m_url( url ),
			m_opts( opts )
		{
		}
		void operator()()
		{
			Items items ;

			for( const auto& it : util::split( m_url,'\n',true ) ){

				if( it.startsWith( "#" ) ){

					continue ;

				}else if( it.startsWith( "yt-dlp" ) ){

					this->parseYtdlp( it,items ) ;
				}else{
					this->parseUrl( it,items ) ;
				}
			}

			m_parent.parseItems( items.move(),m_opts ) ;
		}
	private:
		void parseUrl( const QString& it,Items& items )
		{
			auto url = it ;

			url.replace( "\r","" ) ;

			for( const auto& xt : util::split( url,' ',true ) ){

				auto row = m_parent.m_table.rowWithUrl( xt ) ;

				if( row == -1 ){

					items.add( xt ) ;
				}else{
					m_parent.m_table.selectRow( row ) ;
				}
			}
		}
		void parseYtdlp( const QString& it,Items& items )
		{
			/*
			 * Entry looks like yt-dlp ${YTDLP_OPTIONS} URL
			 * Stream detector has ability to create such entries
			 * https://github.com/54ac/stream-detector
			 */

			auto m = util::split( it,' ',true ) ;

			if( m.size() < 3 ){

				return ;
			}

			QJsonObject obj ;

			auto url = m.takeLast() ;

			if( url.startsWith( "\"" ) ){

				url.remove( 0,1 ) ;
			}

			if( url.endsWith( "\"" ) ){

				url.remove( url.size() - 1,1 ) ;
			}

			url.replace( "\r","" ) ;

			obj.insert( "engineName",m.takeFirst() ) ;
			obj.insert( "downloadOptions",m.join( " " ) ) ;
			obj.insert( "url",url ) ;
			obj.insert( "uiText",url ) ;

			QJsonArray arr ;

			arr.append( obj ) ;

			m_parent.parseDataFromObject( items,QJsonObject(),arr ) ;
		}
		batchdownloader& m_parent ;
		QString m_url ;
		batchdownloader::downloadOpts m_opts ;
	} ;

	this->defaultEngine().updateVersionInfo( m_ctx,meaw( *this,u,opts ) ) ;
}

void batchdownloader::download( const engines::engine& engine )
{
	m_ctx.TabManager().basicDownloader().hideTableList() ;

	class meaw
	{
	public:
		meaw( batchdownloader& parent,const engines::engine& engine ) :
			m_parent( parent ),m_engine( engine )
		{
		}
		void operator()()
		{
			std::vector< int > v ;

			for( int s = 0 ; s < m_parent.m_table.rowCount() ; s++ ){

				if( !m_parent.m_table.finishedWithSuccess( s ) ){

					v.emplace_back( s ) ;
				}
			}

			auto mm = m_parent.m_settings.maxConcurrentDownloads() ;

			auto s = std::min( mm,v.size() ) ;

			for( size_t m = 0 ; m < s ; m++ ){

				m_parent.downloadRecursively( m_engine,v[ m ] ) ;
			}
		}
	private:
		batchdownloader& m_parent ;
		const engines::engine& m_engine ;
	} ;

	engine.updateVersionInfo( m_ctx,meaw( *this,engine ) ) ;
}

void batchdownloader::reportFinishedStatus( const reportFinished& f,
					   const std::vector< QByteArray >& fileNames )
{
	auto finishedStatus = f.status() ;

	utility::updateFinishedState( f.engine(),m_settings,m_table,"batch",finishedStatus,fileNames ) ;

	auto index = finishedStatus.index() ;

	auto success = finishedStatus.exitState().success() ;

	if( m_ctx.Settings().autoHideDownloadWhenCompleted() ){

		if( success ){

			m_table.hideRow( index ) ;
		}
	}

	if( m_settings.desktopNotifyOnDownloadComplete() && success ){

		const auto& ss = m_table.entryAt( index ).uiText ;

		auto m = util::split( ss,"\n" ) ;

		if( m.size() > 1 ){

			m_ctx.mainWindow().notifyOnDownloadComplete( m[ 1 ] ) ;
		}else{
			m_ctx.mainWindow().notifyOnDownloadComplete( m[ 0 ] ) ;
		}
	}

	if( m_table.noneAreRunning() ){

		if( m_settings.desktopNotifyOnAllDownloadComplete() ){

			auto m = m_table.finishWithSuccess() ;

			if( m == 1 ){

				auto m = "1 Download Complete" ;
				m_ctx.mainWindow().notifyOnAllDownloadComplete( m ) ;

			}else if( m > 1 ){

				auto s = QString::number( m ) + " Downloads Complete" ;

				m_ctx.mainWindow().notifyOnAllDownloadComplete( s ) ;
			}
		}

		this->enableAll() ;

		m_ui.pbBDCancel->setEnabled( false ) ;
	}

	this->updateTitleBar() ;
}

void batchdownloader::disableWhileDownloading()
{
	this->disableAll() ;

	m_ui.pbBDPasteClipboard->setEnabled( true ) ;

	m_ui.pbBDAdd->setEnabled( true ) ;

	m_ui.lineEditBDUrl->setEnabled( true ) ;

	m_ui.pbBDCancel->setEnabled( true ) ;

	m_table.setEnabled( true ) ;
}

void batchdownloader::downloadSingle( const engines::engine& eng,int row )
{
	class meaw
	{
	public:
		meaw( batchdownloader& p,const engines::engine& engine,int index ) :
			m_parent( p ),m_engine( engine ),m_index( index )
		{
		}
		void whenCreated()
		{
		}
		void whenDone( const engines::ProcessExitState& st,const std::vector< QByteArray >& fileNames )
		{
			auto b = m_parent.showMetaData() ;

			if( fileNames.size() && b && st.success() && m_engine.isGalleryDl() ){

				m_parent.setThumbnail( fileNames,m_engine,m_index ) ;
			}
		}
		meaw move()
		{
			return std::move( *this ) ;
		}
	private:
		batchdownloader& m_parent ;
		const engines::engine& m_engine ;
		int m_index ;
	} ;

	const auto& engine = utility::resolveEngine( m_table,eng,m_ctx.Engines(),row ) ;

	this->downloadEvent( meaw( *this,engine,row ),engine,row,false ) ;
}

void batchdownloader::downloadRecursively( const engines::engine& eng,int index )
{
	class meaw
	{
	public:
		meaw( batchdownloader& p,const engines::engine& engine,int index ) :
			m_parent( p ),m_engine( engine ),m_index( index )
		{
		}
		void whenCreated()
		{
		}
		void whenDone( const engines::ProcessExitState& st,const std::vector< QByteArray >& fileNames )
		{
			if( st.success() ){

				if( m_parent.showMetaData() && m_engine.isGalleryDl() ){

					if( fileNames.size() ){

						m_parent.setThumbnail( fileNames,m_engine,m_index ) ;
					}
				}
			}

			if( !st.cancelled() ){

				this->startNext() ;
			}
		}
		meaw move()
		{
			return std::move( *this ) ;
		}
	private:
		void startNext()
		{
			auto m = m_parent.m_table.nextAvailableEntryToDownload( m_index + 1 ) ;

			if( m != -1 ){

				m_parent.downloadRecursively( m_engine,m ) ;
			}
		}
		batchdownloader& m_parent ;
		const engines::engine& m_engine ;
		int m_index ;
	} ;

	const auto& engine = utility::resolveEngine( m_table,eng,m_ctx.Engines(),index ) ;

	this->downloadEvent( meaw( *this,engine,index ),engine,index,true ) ;
}

void batchdownloader::addTextToUi( const QByteArray& data,int index )
{
	m_table.setUiText( data,index ) ;
}

void batchdownloader::showHideControls()
{
	auto m = m_ui.tableWidgetBD->height() ;

	bool show = m != 321 ;

	m_ui.lineEditBDUrl->setVisible( show ) ;
	m_ui.pbBDPasteClipboard->setVisible( show ) ;
	m_ui.cbBDMonitorClipboardContent->setVisible( show ) ;
	m_ui.labelBDEnterUrl->setVisible( show ) ;
	m_ui.labelBDEnterOptions->setVisible( show ) ;
	m_ui.lineEditBDUrlOptions->setVisible( show ) ;
	m_ui.pbBDOptionsHistory->setVisible( show ) ;
	m_ui.pbBDOptionsDownload->setVisible( show ) ;
	m_ui.labelBDEngineName->setVisible( show ) ;
	m_ui.cbEngineTypeBD->setVisible( show ) ;

	if( m == 321 ){

		m_ui.tableWidgetBD->resize( 771,441 ) ;
	}else{
		m_ui.tableWidgetBD->resize( 771,321 ) ;
	}
}

void batchdownloader::enableAll()
{
	m_table.setEnabled( true ) ;
	m_ui.pbBDOptionsDownload->setEnabled( true ) ;
	m_ui.cbBDMonitorClipboardContent->setEnabled( true ) ;
	m_ui.pbBDPasteClipboard->setEnabled( true ) ;
	m_ui.pbBDDownload->setEnabled( m_table.rowCount() ) ;
	m_ui.pbBDAdd->setEnabled( true ) ;
	m_ui.pbBDOptions->setEnabled( true ) ;
	m_ui.labelBDEnterOptions->setEnabled( true ) ;
	m_ui.labelBDEnterUrl->setEnabled( true ) ;
	m_ui.pbBDQuit->setEnabled( true ) ;
	m_ui.lineEditBDUrl->setEnabled( true ) ;
	m_ui.lineEditBDUrlOptions->setEnabled( true ) ;
	m_ui.labelBDEngineName->setEnabled( true ) ;
	m_ui.cbEngineTypeBD->setEnabled( true ) ;
	m_ui.pbBatchDownloaderSet->setEnabled( true ) ;
	m_ui.pbCancelBatchDownloder->setEnabled( true ) ;
	m_ui.TableWidgetBatchDownloaderList->setEnabled( true ) ;
	m_ui.pbBDOptionsHistory->setEnabled( true ) ;
}

void batchdownloader::disableAll()
{
	m_table.setEnabled( false ) ;
	m_ui.pbBDOptionsDownload->setEnabled( false ) ;
	m_ui.pbBDOptionsHistory->setEnabled( false ) ;
	m_ui.cbEngineTypeBD->setEnabled( false ) ;
	m_ui.labelBDEngineName->setEnabled( false ) ;
	m_ui.pbBDDownload->setEnabled( false ) ;
	m_ui.pbBDOptions->setEnabled( false ) ;
	m_ui.labelBDEnterOptions->setEnabled( false ) ;
	m_ui.pbBDQuit->setEnabled( false ) ;
	m_ui.lineEditBDUrlOptions->setEnabled( false ) ;
	m_ui.pbBatchDownloaderSet->setEnabled( false ) ;
	m_ui.TableWidgetBatchDownloaderList->setEnabled( false ) ;

	m_ui.pbCancelBatchDownloder->setEnabled( true ) ;

	//m_ui.cbBDMonitorClipboardContent->setEnabled( false ) ;
	//m_ui.pbBDPasteClipboard->setEnabled( false ) ;
	//m_ui.pbBDAdd->setEnabled( false ) ;
	//m_ui.lineEditBDUrl->setEnabled( false ) ;
	//m_ui.labelBDEnterUrl->setEnabled( false ) ;
}

batchdownloader::subtitlesTimer::subtitlesTimer( tableMiniWidget< QJsonObject,5 >& table ) :
	m_banner( tr( "Downloading subtitles" ).toUtf8() + "\n",8 ),
	m_table( table )
{
	connect( &m_timer,&QTimer::timeout,[ this ](){

		auto m = util::split( m_banner.text(),'\n' ) ;

		if( m.size() == 2 ){

			auto s = m[ 1 ] + "\n" + m[ 0 ] ;

			m_table.item( 0,3 ).setText( s + "\n" ) ;
		}else{
			m_table.item( 0,3 ).setText( "\n" + m_banner.text() ) ;
		}
	} ) ;
}

void batchdownloader::subtitlesTimer::start()
{
	m_banner.reset() ;
	m_timer.start( 1000 ) ;
}

void batchdownloader::subtitlesTimer::stop()
{
	m_timer.stop() ;
}

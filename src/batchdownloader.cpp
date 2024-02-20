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

batchdownloader::batchdownloader( const Context& ctx ) :
	m_ctx( ctx ),
	m_settings( m_ctx.Settings() ),
	m_ui( m_ctx.Ui() ),
	m_mainWindow( m_ctx.mainWidget() ),
	m_tabManager( m_ctx.TabManager() ),
	m_table( *m_ui.tableWidgetBD,m_ctx.mainWidget().font(),1,m_settings.textAlignment() ),
	m_tableWidgetBDList( *m_ui.TableWidgetBatchDownloaderList,m_ctx.mainWidget().font() ),
	m_defaultVideoThumbnail( m_settings.defaultVideoThumbnailIcon( settings::tabName::batch ) ),
	m_ccmd( m_ctx,*m_ui.pbBDCancel,m_settings ),
	m_ccmd_metadata( m_ctx,*m_ui.pbBDCancel,m_settings ),
	m_downloadingComments( tr( "Downloading comments" ).toUtf8() ),
	m_startAutoDownload( m_settings.autoDownloadWhenAddedInBatchDownloader() ),
	m_subtitlesTimer( m_tableWidgetBDList )
{
	qRegisterMetaType< ItemEntry >() ;

	this->setShowMetaData( m_settings.showMetaDataInBatchDownloader() ) ;

	m_ui.pbBDPasteClipboard->setIcon( QIcon( ":/clipboard" ) ) ;
	m_ui.pbBDOptionsHistory->setIcon( QIcon( ":/recentlyUsed" ) ) ;
	m_ui.pbBDOptionsDownload->setIcon( QIcon( ":/downloadOptions" ) ) ;

	m_tableWidgetBDList.setTableWidget( [](){

		tableWidget::tableWidgetOptions opts ;

		opts.selectionMode = QAbstractItemView::ExtendedSelection ;

		return opts ;
	}() ) ;

	m_table.setCurrentItemChanged( m_table.startPosition() ) ;

	this->setVisibleMediaSectionCut( false ) ;

	m_ui.pbBDDownload->setEnabled( false ) ;

	m_ui.pbBDCancel->setEnabled( false ) ;

	m_ui.BDFrame->setVisible( false ) ;

	this->resetMenu() ;

	auto cm = &QTableWidget::customContextMenuRequested ;

	m_tableWidgetBDList.connect( cm,[ this ]( const QPoint& ){

		if( m_listType == batchdownloader::listType::MEDIA_OPTIONS ){

			auto row = m_tableWidgetBDList.currentRow() ;

			if( row != -1 ){

				const auto& obj = m_tableWidgetBDList.stuffAt( row ) ;

				auto arr = obj.value( "urls" ).toArray() ;

				utility::contextMenuForDirectUrl( arr,m_ctx ) ;
			}

		}else if( m_listType == batchdownloader::listType::SUBTITLES ){

			this->saveSubtitles() ;
		}
	} ) ;

	connect( m_ui.pbBDOptionsDownload,&QPushButton::clicked,[ this ](){

		auto& t = m_ctx.TabManager().Configure() ;

		auto m = this->defaultEngineName() ;

		t.engineDefaultDownloadOptions( m,[ this ]( const QString& e ){

			for( int i = 0 ; i < m_table.rowCount() ; i++ ){

				auto u = tableWidget::type::DownloadExtendedOptions ;

				m_table.setDownloadingOptions( u,i,e ) ;
			}
		} ) ;
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

			auto row = item->row() ;

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
	} ) ;

	connect( m_ui.pbBatchDownloaderSet,&QPushButton::clicked,[ this ](){

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
	} ) ;

	connect( m_ui.pbCancelBatchDownloder,&QPushButton::clicked,[ this ](){

		m_ui.BDFrame->hide() ;
	} ) ;

	connect( m_ui.pbBDOptionsHistory,&QPushButton::clicked,[ this ](){

		auto m = m_settings.getOptionsHistory( settings::tabName::batch ) ;
		auto e = settings::tabName::batch ;

		if( utility::showHistory( *m_ui.lineEditBDUrlOptions,m,m_settings,e ) ){

			if( m_settings.autoDownload() ){

				this->download( this->defaultEngine() ) ;
			}
		}
	} ) ;

	connect( m_ui.pbBDDownload,&QPushButton::clicked,[ this ](){

		this->download( this->defaultEngine() ) ;
	} ) ;

	connect( m_ui.pbBDCancel,&QPushButton::clicked,[ this ](){

		m_terminator.terminateAll( m_table.get() ) ;
	} ) ;

	connect( m_ui.pbSetTimeIntervals,&QPushButton::clicked,[ this ](){

		auto row = m_table.currentRow() ;

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

		this->setVisibleMediaSectionCut( false ) ;
	} ) ;

	connect( m_ui.pbCancelSetTimeInterval,&QPushButton::clicked,[ this ](){

		this->setVisibleMediaSectionCut( false ) ;
	} ) ;

	connect( m_ui.pbBDPasteClipboard,&QPushButton::clicked,[ this ](){

		auto m = utility::clipboardText() ;

		if( m.startsWith( "http" ) || m.startsWith( "yt-dlp" ) ){

			this->addToList( m ) ;
		}
	} ) ;

	auto cb = m_settings.monitorClipboardUrl( settings::tabName::batch ) ;

	m_ui.cbBDMonitorClipboardContent->setChecked( cb ) ;

	connect( m_ui.cbBDMonitorClipboardContent,&QCheckBox::stateChanged,[ this ]( int s ){

		auto m = s == Qt::CheckState::Checked ;

		m_settings.setMonitorClipboardUrl( m,settings::tabName::batch ) ;
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

	connect( m_ui.pbBDCancel,&QPushButton::clicked,[ this ](){

		m_ccmd.cancelled() ;
		m_ccmd_metadata.cancelled() ;
	} ) ;

	connect( m_ui.pbBDAdd,&QPushButton::clicked,[ this ](){

		auto m = m_ui.lineEditBDUrl->text() ;

		if( !m.isEmpty() ){

			m_ui.lineEditBDUrl->clear() ;

			this->addToList( m ) ;
		}
	} ) ;
}

void batchdownloader::showCustomContext()
{
	auto row = m_table.currentRow() ;

	auto function = [ this ]( const utility::contextState& c ){

		if( c.showRowLogWindow() ){

			m_ctx.logger().showDebugLogWindow() ;

		}else if( c.showLogWindow() ){

			m_ctx.logger().showLogWindow() ;

		}else if( c.clear() ){

			m_ctx.mainWindow().resetTitle() ;

			m_table.clear() ;
		}
	} ;

	QMenu m ;

	if( row == -1 ){

		this->getListFromFile( m ) ;

		auto ss = m_table.noneAreRunning() ;

		return utility::appendContextMenu( m,ss,function,true,row,m_table ) ;
	}

	if( !m_table.rowIsVisible( row ) ){

		auto ss = m_table.noneAreRunning() ;

		return utility::appendContextMenu( m,ss,function,true,row,m_table ) ;
	}

	auto txt = m_table.runningState( row ) ;

	auto running = downloadManager::finishedStatus::running( txt ) ;
	auto finishSuccess = downloadManager::finishedStatus::finishedWithSuccess( txt ) ;

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

	connect( ac,&QAction::triggered,[ this,row ](){

		auto m = QApplication::clipboard() ;

		if( m ){

			m->setText( m_table.url( row ) ) ;
		}
	} ) ;

	ac = m.addAction( tr( "Remove" ) ) ;

	ac->setEnabled( m_table.noneAreRunning() ) ;

	connect( ac,&QAction::triggered,[ this,row ](){

		m_table.removeRow( row ) ;

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

	connect( ac,&QAction::triggered,[ this ](){

		this->setVisibleMediaSectionCut( true ) ;
	} ) ;

	utility::addDownloadContextMenu( running,finishSuccess,m,row,[ this ]( int row ){

		auto m = m_table.uiText( row ) ;

		return m.endsWith( "\n" + engines::engine::mediaAlreadInArchiveText() ) ;

	},[ this,&engine ]( QAction * ac,bool forceDownload,int row ){

		connect( ac,&QAction::triggered,[ &engine,this,row,forceDownload ](){

			auto hh = downloadManager::index::tab::batch ;

			downloadManager::index indexes( m_table,hh ) ;

			auto e = m_table.runningState( row ) ;

			auto visible = m_table.rowIsVisible( row ) ;

			auto m = downloadManager::finishedStatus::finishedWithSuccess( e ) ;

			if( visible && ( !m || forceDownload ) ){

				auto m = m_ui.lineEditBDUrlOptions->text() ;

				auto u = utility::setDownloadOptions( engine,m_table,row,m ) ;

				indexes.add( row,u.move(),forceDownload ) ;
			}

			this->download( engine,indexes.move() ) ;
		} ) ;
	} ) ;

	this->getListFromFile( m ) ;

	utility::saveDownloadList( m_ctx,m,m_table,false ) ;

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

	utility::appendContextMenu( m,{ m_table.noneAreRunning(),finishSuccess },function,true ) ;
}

void batchdownloader::init_done()
{
	auto m = m_ctx.Engines().engineDirPaths().dataPath( "autoSavedList.json" ) ;

	if( QFile::exists( m ) ){

		this->getListFromFile( m,true ) ;
	}else{
		m_done = true ;
	}
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
	utility::saveDownloadList( m_ctx,m_table,false ) ;
}

void batchdownloader::gotEvent( const QJsonObject& jsonArgs )
{
	auto url = jsonArgs.value( "-u" ).toString() ;

	if( !url.isEmpty() ){

		m_ui.tabWidget->setCurrentIndex( 1 ) ;

		auto autoDownload = jsonArgs.value( "-a" ).toBool() ;
		auto showThumbnail = jsonArgs.value( "-e" ).toBool() ;

		this->addToList( url,autoDownload,showThumbnail ) ;
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

void batchdownloader::download( const engines::engine& engine,Items list )
{
	auto row = this->addItemUi( m_defaultVideoThumbnail,-1,false,list.first().url ) ;

	m_ctx.TabManager().Configure().setDownloadOptions( row,m_table ) ;

	const auto& ee = m_table.entryAt( row ) ;

	downloadManager::index index( m_table,downloadManager::index::tab::batch ) ;

	index.add( row,ee.downloadingOptions ) ;

	auto mm = m_ctx.Settings().maxConcurrentDownloads() ;

	const auto& eng = m_ctx.Engines().getEngineByName( ee.engineName ) ;

	if( eng ){

		m_ccmd.download_add( eng.value(),index.move(),mm,batchdownloader::de( *this ) ) ;
	}else{
		m_ccmd.download_add( engine,index.move(),mm,batchdownloader::de( *this ) ) ;
	}
}

void batchdownloader::showThumbnail( const engines::engine& engine,
				     Items list,
				     bool autoDownload,
				     bool showThumbnails )
{
	if( list.isEmpty() ){

		return ;
	}

	if( m_done && m_startAutoDownload && list.hasOneEntry() ){

		this->download( engine,list.move() ) ;

	}else if( autoDownload && !showThumbnails && list.hasOneEntry() ){

		this->download( engine,list.move() ) ;

	}else if( m_showMetaData && engine.likeYoutubeDl() ){

		for( const auto& it : list ){

			auto bb = downloadManager::index::tab::batch ;

			downloadManager::index indexes( m_table,bb ) ;

			tableWidget::entry entry( it ) ;

			auto uiText        = it.uiText ;
			entry.uiText       = "...\n" + uiText ;
			entry.thumbnail    = m_defaultVideoThumbnail ;
			entry.runningState = downloadManager::finishedStatus::running() ;

			auto h = m_settings.thumbnailHeight( settings::tabName::batch ) ;

			int row = m_table.addItem( entry.move(),h ) ;

			util::Timer( 1000,[ this,row,uiText ]( int counter ){

				using ff = downloadManager::finishedStatus ;

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

			indexes.add( row,m_ui.lineEditBDUrlOptions->text() ) ;

			auto url = it.url ;

			m_ccmd_metadata.download( indexes.move(),engine,[ this ](){

				return m_settings.maxConcurrentDownloads() ;

			}(),[ this,url,autoDownload ]( const engines::engine& engine,int index ){

				this->showThumbnail( engine,index,url,autoDownload ) ;
			} ) ;
		}
	}else{
		this->addItemUiSlot( { engine,list.move() } ) ;
	}
}

void batchdownloader::addItemUiSlot( ItemEntry m )
{
	if( m.hasNext() ){

		auto s = m.next() ;

		auto row = this->addItemUi( m_defaultVideoThumbnail,-1,false,s.toJson() ) ;

		m_table.setUiText( s.uiText,row ) ;
		m_table.setEngineName( s.engineName,row ) ;
		m_table.setExtraDownloadOptions( s.downloadExtraOptions,row ) ;

		if( s.downloadOptions.isEmpty() ){

			m_ctx.TabManager().Configure().setDownloadOptions( row,m_table ) ;
		}else{
			m_table.setDownloadingOptions( s.downloadOptions,row ) ;
		}

		auto a = "addItemUiSlot" ;
		auto b = Qt::QueuedConnection ;

		QMetaObject::invokeMethod( this,a,b,Q_ARG( ItemEntry,m ) ) ;
	}
}

static QJsonArray _saveComments( const QJsonArray& arr )
{
	QJsonArray objs ;

	auto _add_object = [ & ]( const QJsonObject& obj,int position ){

		QJsonObject oo ;

		oo.insert( "id",obj.value( "id" ) ) ;
		oo.insert( "parent",obj.value( "parent" ) ) ;
		oo.insert( "author",obj.value( "author" ) ) ;
		oo.insert( "text",obj.value( "text" ) ) ;
		oo.insert( "date",obj.value( "date" ) ) ;
		oo.insert( "text replies",obj.value( "text replies" ) ) ;

		if( position == -1 ){

			objs.append( oo ) ;
		}else{
			objs.removeAt( position ) ;
			objs.insert( position,oo ) ;
		}
	} ;

	for( const auto& it : arr ){

		auto obj = it.toObject() ;

		auto parent = obj.value( "parent" ).toString() ;

		if( parent == "root" ){

			_add_object( obj,-1 ) ;
		}else{
			for( int i = 0 ; i < objs.size() ; i++ ){

				auto xobj = objs.at( i ).toObject() ;

				if( xobj.value( "id" ).toString() == parent ){

					auto replies = [ & ](){

						auto arr = xobj.value( "text replies" ) ;

						if( arr.isUndefined() ){

							return QJsonArray() ;
						}else{
							return arr.toArray() ;
						}
					}() ;

					QJsonObject oo ;

					oo.insert( "author",obj.value( "author" ) ) ;
					oo.insert( "text",obj.value( "text" ) ) ;
					oo.insert( "date",obj.value( "date" ) ) ;

					replies.append( oo ) ;
					xobj.insert( "text replies",replies ) ;

					_add_object( xobj,i ) ;

					break ;
				}
			}
		}
	}

	QJsonArray finalArr ;

	for( const auto& it : util::asConst( objs ) ){

		auto obj = it.toObject() ;

		obj.remove( "parent" ) ;
		obj.remove( "id" ) ;

		finalArr.append( obj ) ;
	}

	return finalArr ;
}

template< typename Function >
static void _getComments( const QJsonArray& arr,Function function )
{
	for( const auto& it : arr ){

		auto obj = it.toObject() ;

		auto id        = obj.value( "id" ).toString() ;
		auto parent    = obj.value( "parent" ).toString() ;
		auto txt       = obj.value( "text" ).toString() ;
		auto author    = obj.value( "author" ).toString() ;
		auto comment   = QObject::tr( "Author" ) + ": " + author ;
		auto timestamp = obj.value( "timestamp" ) ;

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

				auto xobj = xt.toObject() ;

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

		function( comment,std::move( obj ) ) ;
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

		auto arr = obj.value( "comments" ).toArray() ;

		_getComments( arr,[ this ]( const QString& comment,QJsonObject obj ){

			m_tableWidgetBDList.add( { "","","","",comment },std::move( obj ) ) ;
		} ) ;
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

void batchdownloader::setVisibleMediaSectionCut( bool e )
{
	if( e ){

		m_ui.lineEditStartTimeInterval->setFocus() ;
	}else{
		m_ui.lineEditBDUrl->setFocus() ;
	}

	m_ui.labelSetTimeIntervals->setVisible( e ) ;
	m_ui.pbCancelSetTimeInterval->setVisible( e ) ;
	m_ui.pbSetTimeIntervals->setVisible( e ) ;
	m_ui.lineEditChapters->setVisible( e ) ;
	m_ui.lineEditStartTimeInterval->setVisible( e ) ;
	m_ui.lineEditEndTimeInterval->setVisible( e ) ;
	m_ui.cbSplitByChapters->setVisible( e ) ;
	m_ui.label_7->setVisible( e ) ;
	m_ui.label_8->setVisible( e ) ;
	m_ui.label_9->setVisible( e ) ;
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

		auto _parse = [ &]( const QJsonValue& j ){

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

			obj.insert( "title",title ) ;
			obj.insert( "subtitles",l.subtitles() ) ;
			obj.insert( "type",m ) ;

			return obj ;
		} ;

		_parse( obj.value( "subtitles" ) ).each( [ & ]( const language& it ){

			auto obj = _add( it,"subtitles" ) ;

			QStringList s{ it.name(),"subtitle","","",it.notes() } ;

			m_tableWidgetBDList.add( s,std::move( obj ) ) ;
		} ) ;

		_parse( obj.value( "automatic_captions" ) ).each( [ & ]( const language& it ){

			auto obj = _add( it,"automatic_captions" ) ;

			QStringList s{ it.name(),"automatic\ncaption","","",it.notes() } ;

			m_tableWidgetBDList.add( s,std::move( obj ) ) ;
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

	auto title = s.value( "title" ).toString() ;
	auto subtitles = s.value( "subtitles" ).toArray() ;

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

	connect( &m,&QMenu::triggered,[ this,title,ee = ee.move() ]( QAction * ac ){

		auto ext = ac->objectName() ;

		ee.each( [ & ]( const entry& it ){

			if( it.ext == ext ){

				return this->saveSubtitles( it.url,it.ext,title ) ;
			}else{
				return false ;
			}
		} ) ;
	} ) ;

	m.exec( QCursor::pos() ) ;
}

bool batchdownloader::saveSubtitles( const QString& url,const QString& ext,const QString& title )
{
	auto m = utility::homePath() + "/" + title + "." + ext ;

	auto s = QObject::tr( "Save Subtitle To File" ) ;
	auto e = QFileDialog::getSaveFileName( &m_ctx.mainWidget(),s,m ) ;

	if( !e.isEmpty() ){

		auto& n = m_ctx.network() ;

		n.get( url,[ m,this ]( const utils::network::reply& reply ){

			auto s = utility::networkReply( m_ctx,reply ).data() ;

			QFile f( m ) ;
			f.open( QIODevice::WriteOnly ) ;

			f.write( s ) ;
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

template< typename Function >
static void _dataFromFile( Items& items,
			   const Context& ctx,
			   const QJsonArray& array,
			   const QString& urlKey,
			   const QString& uploadDate,
			   const Function& converter )
{
	for( int i = 0 ; i < array.size() ; i++ ){

		const auto& it = array[ i ] ;

		auto obj = it.toObject() ;

		if( obj.isEmpty() ){

			continue ;
		}

		auto url = obj.value( urlKey ).toString() ;

		obj.insert( "webpage_url",url ) ;

		auto d = utility::stringConstants::duration() + " " ;
		auto u = utility::stringConstants::uploadDate() + " " ;

		auto title    = obj.value( "title" ).toString() ;
		auto duration = converter( obj.value( "duration" ) ) ;
		auto date     = obj.value( uploadDate ).toString() ;

		auto engineName   = obj.value( "engineName" ).toString() ;
		auto downloadOpts = obj.value( "downloadOptions" ).toString() ;
		auto downloadExtOpts = obj.value( "downloadExtraOptions" ).toString() ;

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

			auto& cc = ctx.TabManager().Configure() ;
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
}

void batchdownloader::parseDataFromFile( const QByteArray& data )
{
	QJsonParseError err ;

	auto json = QJsonDocument::fromJson( data,&err ) ;

	if( err.error == QJsonParseError::NoError ){

		this->parseDataFromObject( json.object(),json.array() ) ;
	}
}

void batchdownloader::parseDataFromObject( const QJsonObject& obj,const QJsonArray& array )
{
	Items items ;

	if( obj.isEmpty() ){

		/*
		 * File created by us
		 */
		auto function = []( const QJsonValue& e ){

			return e.toString() ;
		} ;

		auto a = "url" ;
		auto b = "uploadDate" ;

		_dataFromFile( items,m_ctx,array,a,b,function ) ;
	}else{
		/*
		 * File created with yt-dlp
		 */
		auto array = obj.value( "entries" ).toArray() ;

		auto function = []( const QJsonValue& e ){

			using tt = engines::engine::functions::timer ;
			return tt::duration( e.toInt() * 1000 ) ;
		} ;

		if( !array.isEmpty() ){

			auto a = "webpage_url" ;
			auto b = "upload_date" ;

			_dataFromFile( items,m_ctx,array,a,b,function ) ;
		}
	}

	const auto& engine = this->defaultEngine() ;

	auto m = m_showMetaData ;
	m_showMetaData = false ;
	this->showThumbnail( engine,items.move() ) ;
	m_showMetaData = m ;
}

void batchdownloader::getListFromFile( const QString& e,bool deleteFile )
{
	engines::file::readAll( e,m_ctx.logger(),[ this,deleteFile,e ]( bool,QByteArray list ){

		if( deleteFile ){

			QFile::remove( e ) ;

			m_ui.tabWidget->setCurrentIndex( 1 ) ;
		}

		if( !list.isEmpty() ){

			if( list.startsWith( '[' ) || list.startsWith( '{' ) ){

				this->parseDataFromFile( list ) ;
			}else{
				list.replace( "\r","" ) ;

				Items items ;

				for( const auto& it : util::split( list,'\n',true ) ){

					items.add( it ) ;
				}

				const auto& engine = this->defaultEngine() ;

				if( items.size() <= m_settings.maxConcurrentDownloads() ){

					this->showThumbnail( engine,items.move() ) ;
				}else{
					auto m = m_showMetaData ;
					m_showMetaData = false ;
					this->showThumbnail( engine,items.move() ) ;
					m_showMetaData = m ;
				}
			}
		}

		m_done = true ;
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

void batchdownloader::showThumbnail( const engines::engine& engine,
				     int index,
				     const QString& url,
				     bool autoDownload )
{			
	class finishedStatus
	{
	public:
		finishedStatus( batchdownloader& parent,
				const engines::engine& engine,
				bool autoDownload,
				QString url,
				BatchLoggerWrapper< batchdownloader::defaultLogger >& l ) :
			m_parent( parent ),
			m_engine( engine ),
			m_autoDownload( autoDownload ),
			m_url( std::move( url ) ),
			m_logger( l )
		{
		}
		void next( const engines::engine& engine,int index )
		{
			m_parent.showThumbnail( engine,index,{},m_autoDownload ) ;
		}
		void finished( const downloadManager::finishedStatus& f )
		{
			auto enableAll = f.done() || f.cancelled() ;

			if( f.exitState().cancelled() ){

				m_parent.addItem( f.index(),enableAll,m_url ) ;
			}else{
				utility::MediaEntry m( m_logger.data() ) ;

				if( m.valid() ){

					m_parent.addItem( f.index(),enableAll,m.move() ) ;
				}else{
					m_parent.addItem( f.index(),enableAll,m_url ) ;
				}

				if( f.done() && m_autoDownload ){

					m_parent.download( m_engine ) ;
				}
			}
		}
		finishedStatus move()
		{
			return std::move( *this ) ;
		}
	private:
		batchdownloader& m_parent ;
		const engines::engine& m_engine ;
		bool m_autoDownload ;
		QString m_url ;
		BatchLoggerWrapper< batchdownloader::defaultLogger >& m_logger ;
	} ;

	class events
	{
	public:
		events( batchdownloader& p,
			const engines::engine& engine,
			int index,
			bool autoDownload,
			const QString& url,
			BatchLoggerWrapper< batchdownloader::defaultLogger > logger ) :
			m_parent( p ),
			m_engine( engine ),
			m_index( index ),
			m_autoDownload( autoDownload ),
			m_url( url ),
			m_logger( logger )
		{
		}
		bool addData( const QByteArray& e )
		{
			return utility::addData( e ) ;
		}
		void done( engines::ProcessExitState e )
		{
			auto& a = m_parent ;
			auto& b = m_engine ;
			auto& c = m_autoDownload ;
			auto& d = m_logger ;

			finishedStatus st( a,b,c,std::move( m_url ),d ) ;

			auto& h = m_parent.m_ccmd_metadata ;

			h.monitorForFinished( m_engine,m_index,e.move(),st.move() ) ;
		}
		void disableAll()
		{
			m_parent.m_ui.pbBDPasteClipboard->setEnabled( true ) ;

			m_parent.m_ui.pbBDAdd->setEnabled( true ) ;

			m_parent.m_ui.lineEditBDUrl->setEnabled( true ) ;
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
		bool m_autoDownload ;
		QString m_url ;
		BatchLoggerWrapper< batchdownloader::defaultLogger > m_logger ;
	} ;

	auto args = engine.dumpJsonArguments( engines::engine::tab::batch ) ;

	auto cookiePath = m_settings.cookieFilePath( engine.name() ) ;
	const auto& ca = engine.cookieArgument() ;

	if( !cookiePath.isEmpty() && !ca.isEmpty() ){

		args.append( ca ) ;
		args.append( cookiePath ) ;
	}

	auto m = m_ui.lineEditBDUrlOptions->text() ;

	utility::addToListOptionsFromsDownload( args,m,m_ctx,engine ) ;

	engine.setTextEncondig( args ) ;

	engine.updateCmdOptions( args ) ;

	m_ctx.logger().setMaxProcessLog( m_table.rowCount() + 1 ) ;

	batchdownloader::defaultLogger ll ;

	BatchLoggerWrapper< batchdownloader::defaultLogger > wrapper( m_ctx.logger(),ll.move() ) ;

	m_ccmd_metadata.download( args,
				  index == -1 ? url : m_table.url( index ),
				  m_terminator.setUp( m_ui.pbBDCancel,&QPushButton::clicked,index ),
				  events( *this,engine,index,autoDownload,url,wrapper ),
				  wrapper,
				  QProcess::ProcessChannel::StandardOutput ) ;
}

int batchdownloader::addItemUi( const QPixmap& pixmap,
				int index,
				tableWidget& table,
				Ui::MainWindow& ui,
				const utility::MediaEntry& media )
{
	auto state = downloadManager::finishedStatus::notStarted() ;

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
	auto row = this->addItemUi( pixmap,index,m_table,m_ui,media ) ;

	m_ctx.TabManager().Configure().setDownloadOptions( row,m_table ) ;

	m_ui.pbBDDownload->setEnabled( true ) ;

	if( enableAll ){

		if( m_table.noneAreRunning() ){

			m_ctx.TabManager().enableAll() ;
			m_ui.pbBDCancel->setEnabled( false ) ;
		}else{
			m_ui.pbBDCancel->setEnabled( true ) ;
		}
	}

	return row ;
}

void batchdownloader::setShowMetaData( bool e )
{
	m_showMetaData = e ;

	if( m_showMetaData ){

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

void batchdownloader::clipboardData( const QString& url )
{
	if( m_settings.monitorClipboardUrl( settings::tabName::batch ) ){

		if( m_table.rowWithUrl( url ) == -1 ){

			m_ui.tabWidget->setCurrentIndex( 1 ) ;
			this->addToList( url,false,m_showMetaData ) ;
		}
	}
}

void batchdownloader::clearScreen()
{
	m_table.clear() ;
	m_ui.lineEditBDUrlOptions->clear() ;
	m_ui.lineEditBDUrl->clear() ;
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

		m_tableWidgetBDList.add( { "","","","","\n" + tr( "Downloading subtitles" ) + "\n" } ) ;

		m_subtitlesTimer.start() ;

	}else if( listType == batchdownloader::listType::COMMENTS ){

		args = engine.defaultCommentsCmdOptions() ;
		args.append( "--verbose" ) ;

		this->showBDFrame( listType ) ;

		m_tableWidgetBDList.add( { "","","","","\n" + m_downloadingComments + "\n" } ) ;
	}else{
		auto& table = m_tableWidgetBDList.get() ;

		table.setHorizontalHeaderLabels( engine.horizontalHeaderLabels() ) ;

		if( row != -1 ){

			this->showBDFrame( listType ) ;

			args = engine.defaultListCmdOptions() ;

			const auto& mp = m_table.mediaProperties( row ) ;

			if( !mp.isEmpty() ){

				const auto ss = engine.mediaProperties( m_ctx.logger(),mp ) ;

				if( !ss.empty() ){

					for( const auto& m : ss ){

						auto e = m.toStringList() ;
						auto s = m.toqJsonObject() ;

						m_tableWidgetBDList.add( e,s ) ;
					}

					return ;
				}
			}
		}
	}

	auto cookiePath = m_settings.cookieFilePath( engine.name() ) ;
	const auto& ca = engine.cookieArgument() ;

	if( !cookiePath.isEmpty() && !ca.isEmpty() ){

		args.append( ca ) ;
		args.append( cookiePath ) ;
	}

	auto m = m_ui.lineEditBDUrlOptions->text() ;

	utility::addToListOptionsFromsDownload( args,m,m_ctx,engine ) ;

	engine.setTextEncondig( args ) ;

	engine.updateCmdOptions( args ) ;

	args.append( url ) ;

	m_ctx.TabManager().disableAll() ;

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

				m_parent.m_tableWidgetBDList.replace( { "","","","",w },0 ) ;
			}else{
				auto m = data.indexOf( "Downloading" ) ;

				auto w = "\n" + data.mid( m ).trimmed() + "\n" ;

				m_parent.m_tableWidgetBDList.replace( { "","","","",w },0 ) ;
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
		void done( engines::ProcessExitState st )
		{
			m_parent.m_ctx.TabManager().enableAll() ;

			m_parent.m_ui.lineEditBDUrl->setFocus() ;

			this->list( st,m_listData ) ;
		}
		void disableAll()
		{
			m_parent.m_ctx.TabManager().disableAll() ;

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
		void list( const engines::ProcessExitState& s,const QByteArray& a )
		{
			using lt = batchdownloader::listType ;

			if( m_listType != lt::MEDIA_OPTIONS ){

				if( m_listType == lt::SUBTITLES ){

					m_parent.m_subtitlesTimer.stop() ;
				}

				m_parent.m_tableWidgetBDList.removeRow( 0 ) ;
			}

			if( s.success() && !a.isEmpty() ){

				if( m_listType == lt::SUBTITLES ){

					m_parent.showSubtitles( a ) ;

				}else if( m_listType == lt::COMMENTS ){

					m_parent.showComments( a ) ;
				}else{
					auto& logger = m_parent.m_ctx.logger() ;

					auto ee = m_engine.mediaProperties( logger,a ) ;

					for( const auto& m : ee ){

						auto s = m.toStringList() ;

						auto e = m.toqJsonObject() ;

						m_parent.m_tableWidgetBDList.add( s,e ) ;
					}

					auto m = QJsonDocument::fromJson( a ) ;

					m_parent.m_table.replace( m.array(),m_row ) ;
				}
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

	BatchLoggerWrapper< outPut > logger( m_ctx.logger(),outPut( *this,listType ) ) ;

	events ev( *this,listType,engine,row ) ;

	auto ctx = utility::make_ctx( ev.move(),logger.move(),term.move(),ch ) ;

	utility::run( args,QString(),ctx.move() ) ;
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

	m_ctx.TabManager().Configure().setDownloadOptions( m.index(),m_table ) ;

	if( m_table.noneAreRunning() ){

		m_ui.pbBDDownload->setEnabled( true ) ;

		m_ctx.TabManager().enableAll() ;

		m_ui.pbBDCancel->setEnabled( false ) ;
	}
}

void batchdownloader::addItem( int index,bool enableAll,const utility::MediaEntry& media )
{
	if( media.thumbnailUrl().isEmpty() ){

		this->addItemUi( index,enableAll,media ) ;
	}else{
		if( networkAccess::hasNetworkSupport() ){

			auto u = media.thumbnailUrl() ;

			networkCtx n{ media,index } ;

			m_ctx.network().get( u,n.move(),this,&batchdownloader::networkResult ) ;
		}else{
			this->addItemUi( index,enableAll,media ) ;
		}
	}
}

void batchdownloader::networkResult( networkCtx d,const utils::network::reply& reply )
{
	utility::networkReply( this,"networkData",m_ctx,reply,nullptr,d.index,d.media.move() ) ;
}

void batchdownloader::addToList( const QString& u,bool autoDownload,bool showThumbnails )
{
	const auto& ee = this->defaultEngine() ;

	ee.updateVersionInfo( m_ctx,[ this,&ee,u,autoDownload,showThumbnails ](){

		for( const auto& it : util::split( u,'\n',true ) ){

			if( it.startsWith( "yt-dlp" ) ){

				/*
				 * Entry looks like yt-dlp ${YTDLP_OPTIONS} URL
				 * Stream detector has ability to create such entries
				 * https://github.com/54ac/stream-detector
				 */

				auto m = util::split( it,' ',true ) ;

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

				this->parseDataFromObject( QJsonObject(),arr ) ;
			}else{
				auto url = it ;

				url.replace( "\r","" ) ;

				for( const auto& xt : util::split( url,' ',true ) ){

					auto row = m_table.rowWithUrl( xt ) ;

					if( row == -1 ){

						this->showThumbnail( ee,xt,autoDownload,showThumbnails ) ;
					}else{
						m_table.selectRow( row ) ;
					}
				}
			}
		}
	} ) ;
}

void batchdownloader::download( const engines::engine& engine,downloadManager::index indexes )
{
	if( indexes.empty() ){

		return ;
	}

	m_settings.setLastUsedOption( m_ui.cbEngineTypeBD->currentText(),
				      m_ui.lineEditBDUrlOptions->text(),
				      settings::tabName::batch ) ;

	m_ctx.TabManager().basicDownloader().hideTableList() ;

	m_ctx.mainWindow().setTitle( QString() ) ;

	m_ctx.TabManager().disableAll() ;

	engine.updateVersionInfo( m_ctx,[ this,&engine,indexes = indexes.move() ]()mutable{

		auto mm = m_settings.maxConcurrentDownloads() ;

		batchdownloader::de de( *this ) ;

		m_ccmd.download( indexes.move(),engine,mm,de.move() ) ;
	} ) ;
}

void batchdownloader::download( const engines::engine& engine,int init )
{
	downloadManager::index indexes( m_table,downloadManager::index::tab::batch ) ;

	for( int s = init ; s < m_table.rowCount() ; s++ ){

		auto e = m_table.runningState( s ) ;

		auto visible = m_table.rowIsVisible( s ) ;

		if( visible && !downloadManager::finishedStatus::finishedWithSuccess( e ) ){

			auto u = utility::setDownloadOptions( engine,m_table,s ) ;

			indexes.add( s,u ) ;
		}
	}

	this->download( engine,indexes.move() ) ;
}

void batchdownloader::reportFinishedStatus( const reportFinished& f )
{
	utility::updateFinishedState( f.engine(),m_settings,m_table,f.finishedStatus() ) ;

	if( m_ctx.Settings().autoHideDownloadWhenCompleted() ){

		auto index = f.finishedStatus().index() ;

		const auto& r = f.finishedStatus().finishedWithSuccess() ;

		if( m_table.runningState( index ) == r ){

			m_table.hideRow( index ) ;
		}
	}

	if( m_table.noneAreRunning() ){

		m_ctx.TabManager().enableAll() ;

		m_ui.pbBDCancel->setEnabled( false ) ;

		m_ctx.mainWindow().setTitle( m_table.completeProgress( 0 ) ) ;
	}
}

void batchdownloader::downloadEntry( const engines::engine& eng,int index )
{
	class events
	{
	public:
		events( batchdownloader& p,const engines::engine& engine,int index ) :
			m_parent( p ),m_engine( engine ),m_index( index )
		{
		}
		const engines::engine& engine()
		{
			return m_engine ;
		}
		bool addData( const QByteArray& )
		{
			return true ;
		}
		void done( engines::ProcessExitState e )
		{
			event ev( m_parent,m_engine ) ;
			m_parent.m_ccmd.monitorForFinished( m_engine,m_index,e.move(),ev ) ;
		}
		void disableAll()
		{
		}
		int index()
		{
			return m_index ;
		}
		void printOutPut( const QByteArray& e )
		{
			m_parent.m_ctx.logger().addRawData( m_index,e ) ;

			m_parent.m_ctx.debug( m_index,e ) ;
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
		class event
		{
		public:
			event( batchdownloader& p,const engines::engine& engine ) :
				m_parent( p ),m_engine( engine )
			{
			}
			void next( const engines::engine& engine,int index )
			{
				m_parent.downloadEntry( engine,index ) ;
			}
			void finished( downloadManager::finishedStatus f )
			{
				reportFinished r( m_engine,f.move() ) ;

				auto a = "reportFinishedStatus" ;
				auto b = Qt::QueuedConnection ;

				auto& m = m_parent ;

				QMetaObject::invokeMethod( &m,a,b,Q_ARG( reportFinished,r.move() ) ) ;
			}
		private:
			batchdownloader& m_parent ;
			const engines::engine& m_engine ;
		} ;

		batchdownloader& m_parent ;
		const engines::engine& m_engine ;
		int m_index ;
	} ;

	auto m = m_ui.lineEditBDUrlOptions->text() ;

	m_settings.addOptionsHistory( m,settings::tabName::batch ) ;

	const auto& engine = utility::resolveEngine( m_table,eng,m_ctx.Engines(),index ) ;

	auto updater = [ this,index ]( const QByteArray& e ){

		auto a = this ;
		auto b = "addTextToUi" ;
		auto c = Qt::QueuedConnection ;

		QMetaObject::invokeMethod( a,b,c,Q_ARG( QByteArray,e ),Q_ARG( int,index ) ) ;
	} ;

	auto error = []( const QByteArray& ){} ;

	int id = utility::concurrentID() ;

	auto& ll = m_ctx.logger() ;

	auto logger = make_loggerBatchDownloader( engine.filter( id ),ll,updater,error,id ) ;

	m_table.setRunningState( downloadManager::finishedStatus::running(),index ) ;

	m_ctx.logger().setMaxProcessLog( m_table.rowCount() + 1 ) ;

	auto updateOpts = [ &engine ]( QStringList opts ){

		engine.updateLocalOptions( opts ) ;

		return opts ;
	} ;

	m_ccmd.download( engine,
			 std::move( updateOpts ),
			 m_ui.lineEditBDUrlOptions->text(),
			 m_table.url( index ),
			 m_ctx,
			 m_terminator.setUp(),
			 events( *this,engine,index ),
			 logger.move() ) ;
}

void batchdownloader::addTextToUi( const QByteArray& data,int index )
{
	m_table.setUiText( data,index ) ;
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
	m_ui.pbCancelBatchDownloder->setEnabled( false ) ;
	m_ui.TableWidgetBatchDownloaderList->setEnabled( false ) ;

	//m_ui.cbBDMonitorClipboardContent->setEnabled( false ) ;
	//m_ui.pbBDPasteClipboard->setEnabled( false ) ;
	//m_ui.pbBDAdd->setEnabled( false ) ;
	//m_ui.lineEditBDUrl->setEnabled( false ) ;
	//m_ui.labelBDEnterUrl->setEnabled( false ) ;
}

batchdownloader::subtitlesTimer::subtitlesTimer( tableMiniWidget< QJsonObject >& table ) :
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

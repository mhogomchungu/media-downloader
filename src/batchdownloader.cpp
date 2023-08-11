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
	m_subtitlesTimer( m_tableWidgetBDList )
{
	qRegisterMetaType< ItemEntry >() ;

	this->setShowMetaData( m_settings.showMetaDataInBatchDownloader() ) ;

	m_tableWidgetBDList.setTableWidget( [](){

		tableWidget::tableWidgetOptions opts ;

		opts.selectionMode = QAbstractItemView::ExtendedSelection ;

		return opts ;
	}() ) ;

	m_table.connect( &QTableWidget::currentItemChanged,[ this ]( QTableWidgetItem * c,QTableWidgetItem * p ){

		m_table.selectRow( c,p,m_table.startPosition() ) ;
	} ) ;

	this->setVisibleMediaSectionCut( false ) ;

	m_ui.pbBDDownload->setEnabled( false ) ;

	m_ui.pbBDCancel->setEnabled( false ) ;

	m_ui.BDFrame->setVisible( false ) ;

	this->resetMenu() ;

	m_tableWidgetBDList.connect( &QTableWidget::customContextMenuRequested,[ this ]( QPoint ){

		if( m_listType == batchdownloader::listType::MEDIA_OPTIONS ){

			auto row = m_tableWidgetBDList.currentRow() ;

			if( row != -1 ){

				const auto arr = m_tableWidgetBDList.stuffAt( row ).value( "urls" ).toArray() ;

				QMenu m ;

				auto clipBoard = QApplication::clipboard() ;

				if( arr.size() == 0 || !clipBoard ){

					m.addAction( tr( "Copy Url" ) )->setEnabled( false ) ;
				}else{
					if( arr.size() == 1 ){

						auto url = arr[ 0 ].toString() ;

						connect( m.addAction( tr( "Copy Url" ) ),&QAction::triggered,[ clipBoard,url ](){

							clipBoard->setText( url ) ;
						} ) ;
					}else{
						for( int i = 0 ; i < arr.size() ; i++ ){

							auto e = QString::number( i + 1 ) ;

							auto s = tr( "Copy Url %1" ).arg( e ) ;

							auto url = arr[ i ].toString() ;

							connect( m.addAction( s ),&QAction::triggered,[ clipBoard,url ](){

								clipBoard->setText( url ) ;
							} ) ;
						}
					}
				}

				m.exec( QCursor::pos() ) ;
			}

		}else if( m_listType == batchdownloader::listType::SUBTITLES ){

			this->saveSubtitles() ;
		}
	} ) ;

	connect( m_ui.pbBDOptionsDownload,&QPushButton::clicked,[ this ](){

		auto& t = m_ctx.TabManager().Configure() ;

		t.engineDefaultDownloadOptions( this->defaultEngineName(),[ this ]( const QString& e ){

			for( int i = 0 ; i < m_table.rowCount() ; i++ ){

				auto u = tableWidget::type::DownloadExtendedOptions ;

				m_table.setDownloadingOptions( u,i,e ) ;
			}
		} ) ;
	} ) ;

	m_tableWidgetBDList.connect( &QTableWidget::itemClicked,[ this ]( QTableWidgetItem * item ){

		if( item && m_listType == batchdownloader::listType::MEDIA_OPTIONS ){

			m_tableWidgetBDList.selectMediaOptions( m_optionsList,*item,m_lineEdit ) ;
		}
	} ) ;

	m_tableWidgetBDList.connect( &QTableWidget::itemDoubleClicked,[ this ]( QTableWidgetItem * item ){

		if( item && m_listType != batchdownloader::listType::COMMENTS ){

			auto row = item->row() ;

			auto m = m_tableWidgetBDList.item( row,0 ).text() ;

			if( !m.isEmpty() ){

				if( m_listType == batchdownloader::listType::SUBTITLES ){

					auto u = tableWidget::type::subtitleOption ;

					auto obj = m_tableWidgetBDList.stuffAt( row ) ;

					m = this->setSubtitleString( obj,m ) ;

					m_table.setDownloadingOptions( u,m_table.currentRow(),m ) ;
				}else{
					auto u = tableWidget::type::DownloadOptions ;

					m_table.setDownloadingOptions( u,m_table.currentRow(),m ) ;
				}
			}

			m_ui.BDFrame->hide() ;
		}
	} ) ;

	connect( m_ui.pbBatchDownloaderSet,&QPushButton::clicked,[ this ](){

		if( m_listType == batchdownloader::listType::COMMENTS ){

			auto e = QFileDialog::getSaveFileName( &m_ctx.mainWidget(),
							       QObject::tr( "Save List To File" ),
							       m_commentsFileName ) ;

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
			if( m_listType == batchdownloader::listType::SUBTITLES ){

				auto row = m_tableWidgetBDList.currentRow() ;

				auto m = m_tableWidgetBDList.item( row,0 ).text() ;

				auto obj = m_tableWidgetBDList.stuffAt( row ) ;

				m = this->setSubtitleString( obj,m ) ;

				auto u = tableWidget::type::subtitleOption ;

				m_table.setDownloadingOptions( u,m_table.currentRow(),m ) ;
			}else{
				auto m = m_lineEdit.text() ;

				auto u = tableWidget::type::DownloadOptions ;

				m_table.setDownloadingOptions( u,m_table.currentRow(),m ) ;
			}

			m_ui.BDFrame->hide() ;			
		}
	} ) ;

	connect( m_ui.pbCancelBatchDownloder,&QPushButton::clicked,[ this ](){

		m_ui.BDFrame->hide() ;
	} ) ;

	connect( m_ui.pbBDOptionsHistory,&QPushButton::clicked,[ this ](){

		auto s = utility::showHistory( *m_ui.lineEditBDUrlOptions,
					       m_settings.getOptionsHistory( settings::tabName::batch ),
					       m_settings,settings::tabName::batch ) ;

		if( s ){

			this->download( this->defaultEngine() ) ;
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

		if( m.startsWith( "http" ) ){

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

		const auto& engine = utility::resolveEngine( m_table,
							     this->defaultEngine(),
							     m_ctx.Engines(),
							     row ) ;

		m_ctx.Engines().openUrls( m_table,row,engine ) ;
	} ) ;

	auto s = static_cast< void( QComboBox::* )( int ) >( &QComboBox::activated ) ;

	connect( m_ui.cbEngineTypeBD,s,[ & ]( int s ){

		if( s != -1 ){

			auto m = m_ui.cbEngineTypeBD->itemText( s ) ;

			m_ui.lineEditBDUrlOptions->setText( m_settings.lastUsedOption( m,settings::tabName::batch ) ) ;

			m_settings.setDefaultEngine( m,settings::tabName::batch ) ;
		}
	} ) ;

	m_table.connect( &QTableWidget::customContextMenuRequested,[ this ]( QPoint ){

		auto row = m_table.currentRow() ;

		auto function = [ this ]( const utility::contextState& c ){

			if( c.showLogWindow() ){

				m_ctx.logger().showLogWindow() ;

			}else if( c.clear() ){

				m_ctx.mainWindow().resetTitle() ;

				m_table.clear() ;
			}
		} ;

		QMenu m ;

		if( row == -1 ){

			this->getListFromFile( m ) ;

			return utility::appendContextMenu( m,m_table.noneAreRunning(),function ) ;
		}

		auto txt = m_table.runningState( row ) ;

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

		const auto& engine = utility::resolveEngine( m_table,this->defaultEngine(),m_ctx.Engines(),row ) ;

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

				downloadManager::index indexes( m_table,false,downloadManager::index::tab::batch ) ;

				auto e = m_table.runningState( row ) ;

				if( !downloadManager::finishedStatus::finishedWithSuccess( e ) || forceDownload ){

					auto m = m_ui.lineEditBDUrlOptions->text() ;

					auto u = utility::setDownloadOptions( engine,m_table,row,m ) ;

					indexes.add( row,std::move( u ),forceDownload ) ;
				}

				this->download( engine,std::move( indexes ) ) ;
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

		utility::appendContextMenu( m,{ m_table.noneAreRunning(),finishSuccess },function ) ;
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

void batchdownloader::init_done()
{
	auto m = m_ctx.Engines().engineDirPaths().dataPath( "autoSavedList.json" ) ;

	if( QFile::exists( m ) ){

		this->getListFromFile( m,true ) ;
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

	m_ui.lineEditBDUrlOptions->setText( m_settings.lastUsedOption( m,settings::tabName::batch ) ) ;
	m_ui.lineEditBDUrl->setFocus() ;
}

void batchdownloader::tabExited()
{
}

void batchdownloader::exiting()
{
	utility::saveDownloadList( m_ctx,m_table,false ) ;
}

void batchdownloader::gotEvent( const QByteArray& m )
{
	QJsonParseError err ;
	auto jsonDoc = QJsonDocument::fromJson( m,&err ) ;

	if( err.error == QJsonParseError::NoError ){

		auto jsonArgs = jsonDoc.object() ;

		auto url = jsonArgs.value( "-u" ).toString() ;

		if( !url.isEmpty() ){

			m_ui.tabWidget->setCurrentIndex( 1 ) ;

			auto autoDownload = jsonArgs.value( "-a" ).toBool( false ) ;
			auto showThumbnail = jsonArgs.value( "-e" ).toBool( false ) ;

			this->addToList( url,autoDownload,showThumbnail ) ;
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

	auto s = settings::tabName::batch ;

	utility::setUpdefaultEngine( comboBox,
				     this->defaultEngineName(),
				     [ this,s ]( const QString& e ){ m_settings.setDefaultEngine( e,s ) ; } ) ;
}

void batchdownloader::showThumbnail( const engines::engine& engine,
				     Items list,
				     bool autoDownload,
				     bool showThumbnails )
{
	if( list.isEmpty() ){

		return ;
	}

	if( autoDownload && !showThumbnails && list.hasOneEntry() ){

		const auto& s = list.first() ;

		tableWidget::entry entry( list.first() ) ;

		entry.thumbnail = m_defaultVideoThumbnail ;

		entry.runningState = downloadManager::finishedStatus::running() ;

		auto row = this->addItemUi( m_defaultVideoThumbnail,-1,m_table,m_ui,s.toJson() ) ;

		m_ctx.TabManager().Configure().setDownloadOptions( row,m_table ) ;

		m_table.selectLast() ;

		downloadManager::index index( m_table,true,downloadManager::index::tab::batch ) ;

		index.add( row,m_ui.lineEditBDUrlOptions->text() ) ;

		this->download( engine,std::move( index ) ) ;

	}else if( m_showMetaData && engine.likeYoutubeDl() ){

		for( const auto& it : list ){

			downloadManager::index indexes( m_table,true,downloadManager::index::tab::batch ) ;

			tableWidget::entry entry( it ) ;

			auto uiText        = it.uiText ;
			entry.uiText       = "...\n" + uiText ;
			entry.thumbnail    = m_defaultVideoThumbnail ;
			entry.runningState = downloadManager::finishedStatus::running() ;

			auto h = m_settings.thumbnailHeight( settings::tabName::batch ) ;

			int row = m_table.addItem( std::move( entry ),h ) ;

			util::Timer( 1000,[ this,row,uiText ]( int counter ){

				if( downloadManager::finishedStatus::running( m_table.runningState( row ) ) ){

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

			m_ccmd_metadata.download( std::move( indexes ),engine,[ this ](){

				return m_settings.maxConcurrentDownloads() ;

			}(),[ this,url,autoDownload ]( const engines::engine& engine,int index ){

				this->showThumbnail( engine,index,url,autoDownload ) ;
			} ) ;
		}
	}else{
		this->addItemUiSlot( { engine,std::move( list ) } ) ;
	}
}

void batchdownloader::addItemUiSlot( ItemEntry m )
{
	if( m.hasNext() ){

		auto s = m.next() ;

		auto row = this->addItemUi( m_defaultVideoThumbnail,-1,false,s.toJson() ) ;

		m_table.setUiText( s.uiText,row ) ;
		m_table.setEngineName( s.engineName,row ) ;
		m_table.setDownloadingOptions( s.downloadOptions,row ) ;
		m_table.setExtraDownloadOptions( s.downloadExtraOptions,row ) ;

		m_ctx.TabManager().Configure().setDownloadOptions( row,m_table ) ;

		QMetaObject::invokeMethod( this,"addItemUiSlot",Qt::QueuedConnection,Q_ARG( ItemEntry,m ) ) ;
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

					comment += "\n" + QObject::tr( "Replies to" ) + ": " + xauthor ;

					break ;
				}
			}
		}

		comment += "\n" + QObject::tr( "Text" ) + ": " + txt ;

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

		if( f.isEmpty() ){

			m_commentsFileName = utility::homePath() + "/MediaDowloaderComments.json" ;
		}else{
			this->normalizeFilePath( f ) ;

			m_commentsFileName = utility::homePath() + "/" + f.mid( 0,200 ) + ".json" ;
		}

		auto arr = obj.value( "comments" ).toArray() ;

		_getComments( arr,[ this ]( const QString& comment,QJsonObject obj ){

			m_tableWidgetBDList.add( { "","","",comment },std::move( obj ) ) ;
		} ) ;
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
				auto iter = utility::make_reverseIterator( m_formats ) ;

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

			std::vector< language > languages ;

			auto obj = j.toObject() ;

			for( auto it = obj.begin() ; it != obj.end() ; it++ ){

				languages.emplace_back( it ) ;
			}

			std::sort( languages.begin(),languages.end(),[]( const language& l,const language& r ){

				return l.name() < r.name() ;
			} ) ;

			return languages ;
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

		for( const auto& it : _parse( obj.value( "subtitles" ) ) ){

			auto obj = _add( it,"subtitles" ) ;

			m_tableWidgetBDList.add( { it.name(),"subtitle","",it.notes() },std::move( obj ) ) ;
		}

		for( const auto& it : _parse( obj.value( "automatic_captions" ) ) ){

			auto obj = _add( it,"automatic_captions" ) ;

			m_tableWidgetBDList.add( { it.name(),"automatic\ncaption","",it.notes() },std::move( obj ) ) ;
		}
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

	std::vector< entry > entries ;

	QMenu m ;

	utility::make_reverseIterator( subtitles ).forEach( [ & ]( const QJsonValue& v ){

		entries.emplace_back( v.toObject() ) ;

		const auto& ext = entries.back().ext ;

		m.addAction( tr( "Download" ) + " " + ext )->setObjectName( ext ) ;
	} ) ;

	connect( &m,&QMenu::triggered,[ this,title,entries = std::move( entries ) ]( QAction * ac ){

		auto ext = ac->objectName() ;

		for( const auto& it : entries ){

			if( it.ext == ext ){

				auto m = utility::homePath() + "/" + title + "." + it.ext ;

				auto s = QObject::tr( "Save Subtitle To File" ) ;
				auto e = QFileDialog::getSaveFileName( &m_ctx.mainWidget(),s,m ) ;

				if( !e.isEmpty() ){

					m_ctx.network().get( it.url,[ m,this ]( const utils::network::reply& reply ){

						QFile f( m ) ;
						f.open( QIODevice::WriteOnly ) ;
						f.write( utility::networkReply( m_ctx,reply ).data() ) ;
					} ) ;
				}

				break ;
			}
		}
	} ) ;

	m.exec( QCursor::pos() ) ;
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

		m_ui.pbBatchDownloaderSet->setText( tr( "Save" ) ) ;

	}else if( m == batchdownloader::listType::SUBTITLES ){

		table.hideColumn( 2 ) ;

		m_ui.pbBatchDownloaderSet->setText( tr( "Set" ) ) ;
	}

	m_optionsList.clear() ;
	m_tableWidgetBDList.clear() ;
	m_ui.BDFrame->show() ;
	m_ui.pbCancelBatchDownloder->setFocus() ;
}

template< typename Function >
static void _parseDataFromFile( Items& items,
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

			downloadOpts = ctx.TabManager().Configure().optionsTranslated( downloadOpts ) ;

			auto dopts = utility::stringConstants::downloadOptions() + ": " + downloadOpts ;

			if( opts.isEmpty() ){

				opts = dopts ;
			}else{
				opts += "\n" + dopts ;
			}
		}

		if( !downloadExtOpts.isEmpty() ){

			auto dopts = utility::stringConstants::downloadExtendedOptions() + ": " + downloadExtOpts ;

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

		auto obj = json.object() ;
		Items items ;

		if( obj.isEmpty() ){

			/*
			 * File created by us
			 */
			auto function = []( const QJsonValue& e ){

				return e.toString() ;
			} ;

			_parseDataFromFile( items,m_ctx,json.array(),"url","uploadDate",function ) ;
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

				_parseDataFromFile( items,m_ctx,array,"webpage_url","upload_date",function ) ;
			}
		}

		const auto& engine = this->defaultEngine() ;

		auto m = m_showMetaData ;
		m_showMetaData = false ;
		this->showThumbnail( engine,std::move( items ) ) ;
		m_showMetaData = m ;
	}
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

				this->showThumbnail( engine,std::move( items ) ) ;
			}
		}
	} ) ;
}

void batchdownloader::getListFromFile( QMenu& m )
{
	auto ac = m.addAction( QObject::tr( "Get List From File" ) ) ;

	QObject::connect( ac,&QAction::triggered,[ this ](){

		auto e = QFileDialog::getOpenFileName( &m_ctx.mainWindow(),
						       tr( "Set Batch File" ),
						       utility::homePath() ) ;

		if( !e.isEmpty() ){

			this->getListFromFile( e,false ) ;
		}		
	} ) ;
}

QString batchdownloader::defaultEngineName()
{
	return m_settings.defaultEngine( settings::tabName::batch,m_ctx.Engines().defaultEngineName() ) ;
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
	auto aa = [ &engine,index,this,url,autoDownload ]( utility::ProcessExitState e,const auto& opts ){

		auto aa = [ this,autoDownload ]( const engines::engine& engine,int index ){

			this->showThumbnail( engine,index,{},autoDownload ) ;
		} ;

		auto bb = [ this,&engine,&opts,url,autoDownload ]( const downloadManager::finishedStatus& f ){

			auto enableAll = f.done() || f.cancelled() ;

			if( f.exitState().cancelled() ){

				this->addItem( f.index(),enableAll,url ) ;
			}else{
				utility::MediaEntry m( opts.batchLogger.data() ) ;

				if( m.valid() ){

					this->addItem( f.index(),enableAll,std::move( m ) ) ;
				}else{
					this->addItem( f.index(),enableAll,url ) ;
				}

				if( f.done() && autoDownload ){

					this->download( engine ) ;
				}
			}
		} ;

		m_ccmd_metadata.monitorForFinished( engine,index,std::move( e ),std::move( aa ),std::move( bb ) ) ;
	} ;

	auto functions = utility::OptionsFunctions( [ this ]( const auto& ){

		m_ui.pbBDPasteClipboard->setEnabled( true ) ;

		m_ui.pbBDAdd->setEnabled( true ) ;

		m_ui.lineEditBDUrl->setEnabled( true ) ;

	},std::move( aa ) ) ;

	auto wrapper = batchdownloader::make_logger( m_ctx.logger(),[]( const QByteArray& ){

		return true ;
	} ) ;

	auto args = engine.dumpJsonArguments( engines::engine::tab::batch ) ;

	auto cookiePath = m_settings.cookieFilePath( engine.name() ) ;
	const auto& ca = engine.cookieArgument() ;

	if( !cookiePath.isEmpty() && !ca.isEmpty() ){

		args.append( ca ) ;
		args.append( cookiePath ) ;
	}

	engine.setTextEncondig( args ) ;

	engine.updateCmdOptions( args ) ;

	m_ctx.logger().setMaxProcessLog( m_table.rowCount() + 1 ) ;

	auto mmm = batchdownloader::make_options( m_ctx,engine,m_ctx.debug(),false,index,wrapper,std::move( functions ) ) ;

	m_ccmd_metadata.download( engine,
				  args,
				  index == -1 ? url : m_table.url( index ),
				  m_terminator.setUp( m_ui.pbBDCancel,&QPushButton::clicked,index ),
				  std::move( mmm ),
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

		m_ctx.TabManager().enableAll() ;
		m_ui.pbBDCancel->setEnabled( false ) ;
	}

	return row ;
}

void batchdownloader::setShowMetaData( bool e )
{
	m_showMetaData = e ;

	if( m_showMetaData ){

		m_table.get().showColumn( 0 ) ;

		m_table.get().setColumnWidth( 0,m_settings.thumbnailWidth( settings::tabName::batch ) ) ;
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

		bool found = false ;

		m_table.forEach( [ & ]( const tableWidget::entry& it ){

			if( it.url == url ){

				found = true ;
			}
		} ) ;

		if( !found ){

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

		m_tableWidgetBDList.add( { "","","","\n" + tr( "Downloading subtitles" ) + "\n" } ) ;

		m_subtitlesTimer.start() ;

	}else if( listType == batchdownloader::listType::COMMENTS ){

		args = engine.defaultCommentsCmdOptions() ;
		args.append( "--verbose" ) ;

		this->showBDFrame( listType ) ;

		m_tableWidgetBDList.add( { "","","","\n" + m_downloadingComments + "\n" } ) ;
	}else{
		auto& table = m_tableWidgetBDList.get() ;

		table.setHorizontalHeaderLabels( engine.horizontalHeaderLabels() ) ;

		if( row != -1 ){

			this->showBDFrame( listType ) ;

			args = engine.defaultListCmdOptions() ;

			const auto& mp = m_table.mediaProperties( row ) ;

			if( !mp.isEmpty() ){

				const auto ss = engine.mediaProperties( mp ) ;

				if( !ss.empty() ){

					for( const auto& m : ss ){

						m_tableWidgetBDList.add( m.toStringList(),m.toqJsonObject() ) ;
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

	engine.setTextEncondig( args ) ;

	engine.updateCmdOptions( args ) ;

	args.append( url ) ;

	m_ctx.TabManager().disableAll() ;

	auto functions = utility::OptionsFunctions( [ this,&engine,listType ]( const utility::ProcessExitState& s,const QByteArray& a ){

			if( listType != batchdownloader::listType::MEDIA_OPTIONS ){

				if( listType == batchdownloader::listType::SUBTITLES ){

					m_subtitlesTimer.stop() ;
				}

				m_tableWidgetBDList.removeRow( 0 ) ;
			}

			if( s.success() && !a.isEmpty() ){

				if( listType == batchdownloader::listType::SUBTITLES ){

					this->showSubtitles( a ) ;

				}else if( listType == batchdownloader::listType::COMMENTS ){

					this->showComments( a ) ;
				}else{
					for( const auto& m : engine.mediaProperties( a ) ){

						m_tableWidgetBDList.add( m.toStringList(),m.toqJsonObject() ) ;
					}
				}
			}

		},[ this ]( const auto& opts ){

			opts.ctx.TabManager().disableAll() ;

			m_ui.pbCancelBatchDownloder->setEnabled( true ) ;
			m_ui.pbCancelBatchDownloder->setFocus() ;

		},[ this ]( utility::ProcessExitState,const auto& opts ){

			opts.ctx.TabManager().enableAll() ;

			m_ui.lineEditBDUrl->setFocus() ;
		}
	) ;

	auto logger = batchdownloader::make_logger( m_ctx.logger(),[ this,listType ]( const QByteArray& data ){

		if( listType == batchdownloader::listType::SUBTITLES ){

			return true ;

		}else if( listType == batchdownloader::listType::COMMENTS ){

			if( utils::misc::containsAny( data,"WARNING","ERROR" ) ){

				return true ;
			}

			if( data.contains( "Downloading comment API JSON reply " ) ){

				auto m = data.indexOf( '(' ) ;

				if( m != -1 ){

					auto w = data.mid( m + 1 ) ;

					m = w.indexOf( '\n' ) ;

					if( m != -1 ){

						w.truncate( m - 1 ) ;
					}

					w = "\n" + m_downloadingComments + ": " + w + "\n" ;

					m_tableWidgetBDList.replace( { "","","",w },0 ) ;
				}else{
					auto m = data.indexOf( "Downloading" ) ;

					auto w = "\n" + data.mid( m ).trimmed() + "\n" ;

					m_tableWidgetBDList.replace( { "","","",w },0 ) ;
				}
			}

			return false ;
		}else{
			return true ;
		}
	} ) ;

	auto oopts  = batchdownloader::make_options( m_ctx,engine,m_ctx.debug(),true,-1,logger,std::move( functions ) ) ;
	auto term   = m_terminator.setUp( m_ui.pbCancelBatchDownloder,&QPushButton::clicked,-1 ) ;
	auto ch     = QProcess::ProcessChannel::StandardOutput ;

	engine.updateOutPutChannel( ch ) ;

	auto ctx    = utility::make_ctx( engine,std::move( oopts ),logger,std::move( term ),ch ) ;

	m_ctx.logger().setMaxProcessLog( 1 ) ;

	utility::run( args,QString(),std::move( ctx ) ) ;
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

			auto w = m_settings.thumbnailWidth( settings::tabName::batch ) ;
			auto h = m_settings.thumbnailHeight( settings::tabName::batch ) ;

			this->addItemUi( pixmap.scaled( w,h ),m.index(),m_table,m_ui,m.media() ) ;
		}else{
			this->addItemUi( m_defaultVideoThumbnail,m.index(),m_table,m_ui,m.media() ) ;
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

	m_networkRunning-- ;
}

void batchdownloader::addItem( int index,bool enableAll,const utility::MediaEntry& media )
{
	if( media.thumbnailUrl().isEmpty() ){

		this->addItemUi( index,enableAll,media ) ;
	}else{
		if( networkAccess::hasNetworkSupport() ){

			m_networkRunning++ ;

			const auto& u = media.thumbnailUrl() ;

			m_ctx.network().get( u,[ this,media,index ]( const utils::network::reply& reply ){

				utility::networkReply( this,"networkData",m_ctx,reply,nullptr,index,media.move() ) ;
			} ) ;
		}else{
			this->addItemUi( index,enableAll,media ) ;
		}
	}
}

void batchdownloader::addToList( const QString& u,bool autoDownload,bool showThumbnails )
{
	const auto& engine = this->defaultEngine() ;

	engine.updateVersionInfo( m_ctx,[ this,&engine,u,autoDownload,showThumbnails ](){

		auto url = u ;

		url.replace( "\r","" ) ;

		if( utils::misc::containsAny( url,'\n',' ' ) ){

			for( const auto& it : util::split( url,' ',true ) ){

				for( const auto& xt : util::split( it,'\n',true ) ){

					this->showThumbnail( engine,xt,autoDownload,showThumbnails ) ;
				}
			}
		}else{
			this->showThumbnail( engine,url,autoDownload,showThumbnails ) ;
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

	engine.updateVersionInfo( m_ctx,[ this,&engine,indexes = indexes.move() ](){

		m_ccmd.download( indexes.move(),engine,[ this ](){

			return m_settings.maxConcurrentDownloads() ;

		}(),[ this ]( const engines::engine& engine,int index ){

			this->downloadEntry( engine,index ) ;
		} ) ;
	} ) ;
}

void batchdownloader::download( const engines::engine& engine,int init )
{
	downloadManager::index indexes( m_table,true,downloadManager::index::tab::batch ) ;

	for( int s = init ; s < m_table.rowCount() ; s++ ){

		auto e = m_table.runningState( s ) ;

		if( !downloadManager::finishedStatus::finishedWithSuccess( e ) ){

			auto u = utility::setDownloadOptions( engine,m_table,s ) ;

			indexes.add( s,u ) ;
		}
	}

	this->download( engine,std::move( indexes ) ) ;
}

void batchdownloader::reportFinishedStatus( const reportFinished& f )
{
	utility::updateFinishedState( f.engine(),m_settings,m_table,f.finishedStatus() ) ;

	const auto& e = f.finishedStatus() ;

	auto lastIndex = e.lastIndex() ;

	bool moreItemsRemaining = lastIndex < m_table.rowCount() - 1 ;

	if( e.done() && e.batchDownloading() && moreItemsRemaining ){
		/*
		 * We restart downloading because more entries were added after we
		 * started downloading
		 */
		this->download( f.engine(),lastIndex + 1 ) ;
	}else{
		if( m_table.noneAreRunning() ){

			m_ctx.TabManager().enableAll() ;

			m_ui.pbBDCancel->setEnabled( false ) ;

			m_ctx.mainWindow().setTitle( m_table.completeProgress( 0 ) ) ;
		}
	}
}

void batchdownloader::downloadEntry( const engines::engine& eng,int index )
{
	const auto& engine = utility::resolveEngine( m_table,eng,m_ctx.Engines(),index ) ;

	auto aa = [ &engine,index,this ]( utility::ProcessExitState e,const auto& ){

		auto aa = [ this ]( const engines::engine& engine,int index ){

			this->downloadEntry( engine,index ) ;
		} ;

		auto bb = [ &engine,this ]( downloadManager::finishedStatus f ){

			QMetaObject::invokeMethod( this,
						   "reportFinishedStatus",
						   Qt::QueuedConnection,
						   Q_ARG( reportFinished,reportFinished( engine,std::move( f ) ) ) ) ;
		} ;

		m_ccmd.monitorForFinished( engine,index,std::move( e ),std::move( aa ),std::move( bb ) ) ;
	} ;

	auto functions = utility::OptionsFunctions( []( const auto& ){},std::move( aa ) ) ;

	auto m = m_ui.lineEditBDUrlOptions->text() ;

	m_settings.addOptionsHistory( m,settings::tabName::batch ) ;

	auto loog = batchdownloader::make_logger( m_ctx.logger(),[]( const QByteArray& ){

		return true ;
	} ) ;

	auto oopts = batchdownloader::make_options( m_ctx,engine,m_ctx.debug(),false,index,loog,std::move( functions ) ) ;

	auto updater = [ this,index ]( const QByteArray& e ){

		QMetaObject::invokeMethod( this,
					   "addTextToUi",
					   Qt::QueuedConnection,
					   Q_ARG( QByteArray,e ),
					   Q_ARG( int,index ) ) ;
	} ;

	auto error = []( const QByteArray& ){} ;

	int id = utility::concurrentID() ;

	auto logger = make_loggerBatchDownloader( engine.filter( id ),
						  m_ctx.logger(),
						  std::move( updater ),
						  std::move( error ),
						  id ) ;

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
			 std::move( oopts ),
			 std::move( logger ) ) ;
}

void batchdownloader::addTextToUi( const QByteArray& data,int index )
{
	if( downloadManager::finishedStatus::running( m_table.runningState( index ) ) ){

	//	if( data.count( '\n' ) == 0 && data.endsWith( "..." ) ){

	//		m_table.setUiText( "\n" + data + "\n",index ) ;
	//	}else{
			m_table.setUiText( data,index ) ;
	//	}
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

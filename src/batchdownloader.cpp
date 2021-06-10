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

batchdownloader::batchdownloader( const Context& ctx ) :
	m_ctx( ctx ),
	m_settings( m_ctx.Settings() ),
	m_ui( m_ctx.Ui() ),
	m_mainWindow( m_ctx.mainWidget() ),
	m_tabManager( m_ctx.TabManager() ),
	m_table( *m_ui.tableWidgetBD,m_ctx.mainWidget().font() ),
	m_tableWidgetBDList( *m_ui.TableWidgetBatchDownloaderList,m_ctx.mainWidget().font() ),
	m_running( false ),
	m_debug( ctx.debug() ),
	m_ccmd( m_ctx,*m_ui.pbBDCancel,m_settings )
{
	m_tableWidgetBDList.setTableWidget( [](){

		tableWidget::tableWidgetOptions opts ;

		opts.customContextPolicy = Qt::NoContextMenu ;
		opts.selectionMode       = QAbstractItemView::ExtendedSelection ;

		return opts ;
	}() ) ;

	m_table.connect( &QTableWidget::currentItemChanged,[ this ]( QTableWidgetItem * c,QTableWidgetItem * p ){

		m_table.selectRow( c,p,0 ) ;
	} ) ;

	m_ui.tabWidgetBatchDownlader->setCurrentIndex( 0 ) ;

	auto& t = m_table.get() ;

	t.hideColumn( 1 ) ;
	t.hideColumn( 2 ) ;

	m_ui.pbBDDownload->setEnabled( false ) ;

	m_ui.pbBDCancel->setEnabled( false ) ;

	m_ui.BDFrame->hide() ;

	this->resetMenu() ;

	m_tableWidgetBDList.connect( &QTableWidget::itemClicked,[ this ]( QTableWidgetItem * item ){

		if( item ){

			m_tableWidgetBDList.selectMediaOptions( m_optionsList,*item,m_lineEdit ) ;
		}
	} ) ;

	m_tableWidgetBDList.connect( &QTableWidget::itemDoubleClicked,[ this ]( QTableWidgetItem * item ){

		if( item ){

			auto m = m_tableWidgetBDList.item( item->row(),0 ).text() ;

			if( !m.isEmpty() ){

				m_ui.lineEditBDUrlOptions->setText( m ) ;
			}

			m_ui.BDFrame->hide() ;
		}
	} ) ;

	connect( m_ui.pbBatchDownloaderSet,&QPushButton::clicked,[ this ](){

		auto m = m_lineEdit.text() ;

		if( !m.isEmpty() ){

			m_ui.lineEditBDUrlOptions->setText( m ) ;
		}

		m_ui.BDFrame->hide() ;
	} ) ;

	connect( m_ui.pbCancelBatchDownloder,&QPushButton::clicked,[ this ](){

		m_ui.BDFrame->hide() ;
	} ) ;

	connect( m_ui.pbBDCancel,&QPushButton::clicked,[ this ](){

		m_terminator.terminateAll( m_table.get() ) ;
	} ) ;

	connect( m_ui.pbBDPasteClipboard,&QPushButton::clicked,[ this ](){

		auto m = utility::clipboardText() ;

		if( !m.isEmpty() ){

			this->addToList( m,m_settings.doNotGetUrlTitle() ) ;
		}
	} ) ;

	m_table.connect( &QTableWidget::cellDoubleClicked,[ this ]( int row,int column ){

		Q_UNUSED( column )

		m_ctx.Engines().openUrls( m_table.item( row,0 ),
					  m_ui.cbEngineTypeBD->currentText() ) ;
	} ) ;

	auto s = static_cast< void( QComboBox::* )( int ) >( &QComboBox::activated ) ;

	connect( m_ui.cbEngineTypeBD,s,[ & ]( int s ){

		if( s != -1 ){

			auto m = m_ui.cbEngineTypeBD->itemText( s ) ;

			m_ui.lineEditBDUrlOptions->setText( m_settings.lastUsedOption( m,settings::tabName::batch ) ) ;

			m_settings.setDefaultEngine( m,settings::tabName::batch ) ;
		}
	} ) ;

	connect( m_ui.tabWidgetBatchDownlader,&QTabWidget::currentChanged,[ this ]( int s ){

		if( s == 0 ){

			m_ui.pbBDDownload->setEnabled( m_table.rowCount() ) ;
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

		if( row == -1 ){

			QMenu m ;

			return utility::appendContextMenu( m,m_running,function ) ;
		}

		auto txt = m_table.item( row,2 ).text() ;

		auto running = downloadManager::finishedStatus::running( txt ) ;
		auto finishSuccess = downloadManager::finishedStatus::finishedWithSuccess( txt ) ;

		QMenu m ;

		auto ac = m.addAction( tr( "Open" ) ) ;

		ac->setEnabled( finishSuccess ) ;

		connect( ac,&QAction::triggered,[ this,row ](){

			auto m = m_ui.cbEngineTypeBD->currentText() ;
			m_ctx.Engines().openUrls( m_table.item( row,0 ),m ) ;
		} ) ;

		ac = m.addAction( tr( "Cancel" ) ) ;

		ac->setEnabled( running ) ;

		connect( ac,&QAction::triggered,[ this,row ](){

			m_terminator.terminate( row ) ;
		} ) ;

		ac = m.addAction( tr( "Remove" ) ) ;

		ac->setEnabled( !m_running ) ;

		connect( ac,&QAction::triggered,[ this,row ](){

			m_table.removeRow( row ) ;

			m_ui.pbBDDownload->setEnabled( m_table.rowCount() ) ;
		} ) ;

		ac = m.addAction( tr( "Get List" ) ) ;
		ac->setEnabled( !running ) ;

		connect( ac,&QAction::triggered,[ this ](){

			this->showList() ;
		} ) ;

		ac = m.addAction( tr( "Download" ) ) ;
		ac->setEnabled( !running && !finishSuccess ) ;

		connect( ac,&QAction::triggered,[ this,row ](){

			auto m = m_settings.defaultEngine( settings::tabName::batch ) ;

			const auto& engine = m_ctx.Engines().defaultEngine( m ) ;

			downloadManager::index indexes( m_table.get(),m_ui.lineEditBDUrlOptions->text() ) ;

			auto e = m_table.item( row,2 ).text() ;

			if( !downloadManager::finishedStatus::finishedWithSuccess( e ) ){

				indexes.add( row ) ;
			}

			this->download( engine,std::move( indexes ) ) ;
		} ) ;

		m.addSeparator() ;

		utility::appendContextMenu( m,{ m_running,finishSuccess },function ) ;
	} ) ;

	connect( m_ui.pbBDQuit,&QPushButton::clicked,[ this ](){

		m_tabManager.basicDownloader().appQuit() ;
	} ) ;

	connect( m_ui.pbBDCancel,&QPushButton::clicked,[ this ](){

		m_ccmd.cancelled() ;
	} ) ;

	connect( m_ui.pbBDAdd,&QPushButton::clicked,[ this ](){

		this->addToList( m_ui.lineEditBDUrl->text(),m_settings.doNotGetUrlTitle() ) ;
	} ) ;

	connect( m_ui.pbBDDownload,&QPushButton::clicked,[ this ](){

		auto m = m_settings.defaultEngine( settings::tabName::batch ) ;

		this->download( m_ctx.Engines().defaultEngine( m ) ) ;
	} ) ;
}

void batchdownloader::init_done()
{
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

			auto m = m_settings.defaultEngine( settings::tabName::batch ) ;

			this->download( m_ctx.Engines().defaultEngine( m ) ) ;
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

void batchdownloader::updateEnginesList( const QStringList& e )
{
	auto& comboBox = *m_ui.cbEngineTypeBD ;

	comboBox.clear() ;

	for( const auto& it : e ){

		comboBox.addItem( it ) ;
	}

	auto s = settings::tabName::batch ;

	this->setUpdefaultEngine( comboBox,
				  m_settings.defaultEngine( s ),
				  [ this,s ]( const QString& e ){ m_settings.setDefaultEngine( e,s ) ; } ) ;
}

void batchdownloader::download( const engines::engine& engine,
				const QString& opts,
				const QStringList& list,
				bool doNotGetTitle )
{
	for( const auto& it : list ){

		this->addToList( it,doNotGetTitle ) ;
	}

	m_ui.tabWidget->setCurrentIndex( 1 ) ;

	m_ui.tabWidgetBatchDownlader->setCurrentIndex( 0 ) ;

	m_ui.lineEditBDUrlOptions->setText( opts ) ;

	this->download( engine ) ;
}

void batchdownloader::clearScreen()
{
	m_table.clear() ;
	m_ui.lineEditBDUrlOptions->clear() ;
	m_ui.lineEditBDUrl->clear() ;
}

void batchdownloader::showList()
{
	auto& table = m_table.get() ;

	auto row = table.currentRow() ;

	if( row == -1 ){

		return ;
	}

	auto m = m_settings.defaultEngine( settings::tabName::batch ) ;

	const auto& engine = m_ctx.Engines().defaultEngine( m ) ;

	auto args = engine.defaultListCmdOptions() ;

	if( args.isEmpty() ){

		return ;
	}

	m_optionsList.clear() ;

	m_tableWidgetBDList.clear() ;

	m_ui.BDFrame->show() ;

	args.append( table.item( row,1 )->text() ) ;

	m_ctx.TabManager().disableAll() ;

	auto functions = utility::OptionsFunctions( [ this,&engine ]( const QList< QByteArray >& args ){

			m_tableWidgetBDList.showOptions( engine,args ) ;

		},[ this ]( const batchdownloader::opts& opts ){

			opts.ctx.TabManager().disableAll() ;

			m_ui.pbCancelBatchDownloder->setEnabled( true ) ;

		},[]( utility::ProcessExitState,const batchdownloader::opts& opts ){

			opts.ctx.TabManager().enableAll() ;
		}
	) ;

	batchdownloader::opts opts{ m_ctx,m_debug,true,-1 } ;

	utility::run( engine,
		      args,
		      QString(),
		      batchdownloader::make_options( std::move( opts ),std::move( functions ) ),
		      LoggerWrapper( m_ctx.logger(),utility::concurrentID() ),
		      utility::Terminator::setUp( m_ui.pbCancelBatchDownloder,&QPushButton::clicked,-1 ) ) ;
}

static void _set_variables( Ui::MainWindow& ui,
			    tableWidget& table,
			    const QString& url,
			    const QString& state )
{
	table.addItem( { url,url,state } ) ;

	table.selectLast() ;

	ui.lineEditBDUrl->clear() ;

	ui.lineEditBDUrl->setFocus() ;

	ui.pbBDDownload->setEnabled( true ) ;
}

template< typename Function >
static void _getUrlTitle( const QString& exe,const QStringList& args,Function function )
{
	utility::run( exe,args,[]( QProcess& exe ){

		exe.setProcessChannelMode( QProcess::ProcessChannelMode::SeparateChannels ) ;

		return QString() ;

	},[]( QProcess& ){},[ function = std::move( function ) ]( int s,QProcess::ExitStatus e,QString& c ){

		if( s == 0 && e == QProcess::ExitStatus::NormalExit ){

			function( c ) ;
		}else{
			function( QString() ) ;
		}

	},[]( QProcess::ProcessChannel channel,QByteArray data,QString& c ){

		if( channel == QProcess::ProcessChannel::StandardOutput ){

			c = data ;
		}
	} ) ;
}

void batchdownloader::addToList( const QString& a,bool doNotGetTitle )
{
	if( !a.isEmpty() ){

		const auto& engine = m_ctx.Engines().defaultEngine( m_settings.defaultEngine( settings::tabName::batch ) ) ;

		if( doNotGetTitle || !engine.likeYoutubeDl() ){

			auto s = downloadManager::finishedStatus::notStarted() ;

			_set_variables( m_ui,m_table,a,s ) ;
		}else{
			m_ctx.TabManager().disableAll() ;

			const auto& exe = engine.exePath().realExe() ;
			QStringList args{ "--get-title",a } ;

			engines::engine::exeArgs::cmd cmd( exe,args ) ;

			m_ctx.logger().add( "cmd: " + engine.commandString( cmd ) ) ;

			_getUrlTitle( exe,args,[ a,this ]( const QString& title ){

				auto state = downloadManager::finishedStatus::notStarted() ;

				if( title.isEmpty() || title == "\n" ){

					_set_variables( m_ui,m_table,a,state ) ;
				}else{
					m_ctx.logger().add( title ) ;
					_set_variables( m_ui,m_table,a + "\n" + title,state ) ;
				}

				m_ctx.TabManager().enableAll() ;
			} ) ;
		}
	}
}

void batchdownloader::download( const engines::engine& engine,downloadManager::index indexes )
{
	if( indexes.empty() ){

		return ;
	}

	m_settings.setLastUsedOption( m_ui.cbEngineTypeBD->currentText(),
				      m_ui.lineEditBDUrlOptions->text(),
				      settings::tabName::batch ) ;

	this->addToList( m_ui.lineEditBDUrl->text(),true ) ;

	m_ctx.TabManager().basicDownloader().hideTableList() ;

	m_ccmd.download( std::move( indexes ),engine,[ this ](){

		if( m_settings.concurrentDownloading() ){

			return m_settings.maxConcurrentDownloads() ;
		}else{
			return 1 ;
		}

	}(),[ this ]( const engines::engine& engine,int index ){

		this->download( engine,index ) ;
	} ) ;
}

void batchdownloader::download( const engines::engine& engine )
{
	downloadManager::index indexes( m_table.get(),m_ui.lineEditBDUrlOptions->text() ) ;

	for( int s = 0 ; s < m_table.rowCount() ; s++ ){

		auto e = m_table.item( s,2 ).text() ;

		if( !downloadManager::finishedStatus::finishedWithSuccess( e ) ){

			indexes.add( s ) ;
		}
	}

	this->download( engine,std::move( indexes ) ) ;
}

void batchdownloader::download( const engines::engine& engine,int index )
{
	auto aa = [ &engine,index,this ]( utility::ProcessExitState e,const batchdownloader::opts& ){

		auto aa = [ this ]( const engines::engine& engine,int index ){

			this->download( engine,index ) ;
		} ;

		auto bb = [ &engine,this ]( const downloadManager::finishedStatus& f ){

			utility::updateFinishedState( engine,m_settings,m_table.get(),f ) ;
		} ;

		m_ccmd.monitorForFinished( engine,index,std::move( e ),std::move( aa ),std::move( bb ) ) ;
	} ;

	auto functions = utility::OptionsFunctions( []( const batchdownloader::opts& ){},std::move( aa ) ) ;

	auto m = m_ui.lineEditBDUrlOptions->text() ;

	m_ccmd.download( engine,
			 index,
			 m_table.item( index,1 ).text(),
			 m_terminator,
			 batchdownloader::make_options( { m_ctx,m_debug,false,index },std::move( functions ) ),
			 make_loggerBatchDownloader( engine.filter( utility::args( m ).quality ),
						     engine,
						     m_ctx.logger(),
						     m_table.item( index,0 ),
						     utility::concurrentID() ) ) ;
}

void batchdownloader::enableAll()
{
	m_running = false ;

	m_table.setEnabled( true ) ;
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
}

void batchdownloader::disableAll()
{
	m_running = true ;
	m_table.setEnabled( false ) ;
	m_ui.pbBDPasteClipboard->setEnabled( false ) ;
	m_ui.cbEngineTypeBD->setEnabled( false ) ;
	m_ui.labelBDEngineName->setEnabled( false ) ;
	m_ui.pbBDDownload->setEnabled( false ) ;
	m_ui.pbBDAdd->setEnabled( false ) ;
	m_ui.pbBDOptions->setEnabled( false ) ;
	m_ui.labelBDEnterOptions->setEnabled( false ) ;
	m_ui.labelBDEnterUrl->setEnabled( false ) ;
	m_ui.pbBDQuit->setEnabled( false ) ;
	m_ui.lineEditBDUrl->setEnabled( false ) ;
	m_ui.lineEditBDUrlOptions->setEnabled( false ) ;
	m_ui.pbBatchDownloaderSet->setEnabled( false ) ;
	m_ui.pbCancelBatchDownloder->setEnabled( false ) ;
	m_ui.TableWidgetBatchDownloaderList->setEnabled( false ) ;
}

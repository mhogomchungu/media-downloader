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

#include "basicdownloader.h"

#include <QFile>
#include <QDir>
#include <QMenu>
#include <QFileDialog>

#include "tabmanager.h"
#include "mainwindow.h"
#include "utils/threads.hpp"

basicdownloader::basicdownloader( const Context& ctx ) :
	m_ctx( ctx ),
	m_settings( m_ctx.Settings() ),
	m_ui( m_ctx.Ui() ),
	m_tabManager( m_ctx.TabManager() ),
	m_tableList( *m_ui.bdTableWidgetList,0,m_ctx.mainWidget().font() ),
	m_hiddenTable( m_bogusTableOriginal,m_ctx.mainWidget().font(),0,m_settings.textAlignment() )
{
	m_ui.pbPasteClipboard->setIcon( m_settings.getIcon( "clipboard" ) ) ;
	m_ui.pbOptionsHistory->setIcon( m_settings.getIcon( "recentlyUsed" ) ) ;
	m_ui.pbOptionsDownloadOptions->setIcon( m_settings.getIcon( "downloadOptions" ) ) ;	
	m_ui.pbBasicDownloaderPlay->setIcon( m_settings.getIcon( "video" ) ) ;

	m_hiddenTable.setColumnNumbersTo( 3 ) ;

	this->setAsActive() ;

	m_ui.pbCancel->setEnabled( false ) ;

	m_tableList.setVisible( false ) ;

	m_tableList.setUpHeaderMenu() ;

	tableWidget::tableWidgetOptions opts ;

	opts.selectionMode = QAbstractItemView::ExtendedSelection ;

	m_tableList.setTableWidget( opts ) ;

	connect( m_ui.pbPasteClipboard,&QPushButton::clicked,[ this ](){

		m_ui.lineEditURL->setText( utility::clipboardText() ) ;
	} ) ;

	connect( m_ui.pbBasicDownloaderPlay,&QPushButton::clicked,[ this ](){

		if( m_hiddenTable.rowCount() ){

			int row = 0 ;

			const auto& e = this->defaultEngine().engine ;

			const auto& engines = m_ctx.Engines() ;

			const auto& engine = utility::resolveEngine( m_hiddenTable,e,engines,row ) ;

			engines.openUrls( m_hiddenTable,row,engine ) ;
		}
	} ) ;

	m_tableList.connect( &QTableWidget::itemSelectionChanged,[ this ](){

		auto& a = *m_ui.lineEditOptions ;
		m_tableList.selectMediaOptions( m_optionsList,a ) ;
	} ) ;

	auto cm = &QTableWidget::customContextMenuRequested ;

	m_tableList.connect( cm,[ this ]( const QPoint& ){

		auto row = m_tableList.currentRow() ;

		QMenu m ;

		if( row != -1 ){

			class meaw
			{
			public:
				meaw( tableMiniWidget< engines::engine::baseEngine::mediaInfo,5 >& m ) :
					m_table( m )
				{
				}
				const QJsonObject& stuffAt( int s ) const
				{
					m_obj = m_table.stuffAt( s ).toqJsonObject() ;

					return m_obj ;
				}
				std::vector< int > selectedRows() const
				{
					return m_table.selectedRows() ;
				}
			private:
				mutable QJsonObject m_obj ;
				const tableMiniWidget< engines::engine::baseEngine::mediaInfo,5 >& m_table ;
			} ;

			utility::setContextMenuForDirectUrl( meaw( m_tableList ),m,m_ctx ) ;

			m.addSeparator() ;

			connect( m.addAction( tr( "Hide List" ) ),&QAction::triggered,[ this ](){

				m_tableList.setVisible( false ) ;
			} ) ;

			m.exec( QCursor::pos() ) ;
		}else{
			connect( m.addAction( tr( "Hide List" ) ),&QAction::triggered,[ this ](){

				m_tableList.setVisible( false ) ;
			} ) ;

			m.exec( QCursor::pos() ) ;
		}
	} ) ;

	connect( m_ui.pbOptionsDownloadOptions,&QPushButton::clicked,[ this ](){

		auto& t = m_ctx.TabManager().Configure() ;

		t.engineSetDefaultDownloadOptions( this->defaultEngine().engine ) ;
	} ) ;

	connect( m_ui.pbList,&QPushButton::clicked,[ this ](){

		m_tableList.setVisible( false ) ;

		this->list() ;
	} ) ;

	this->resetMenu() ;

	connect( m_ui.pbDownload,&QPushButton::clicked,[ this ](){

		m_tableList.setVisible( false ) ;

		this->download( m_ui.lineEditURL->text() ) ;
	} ) ;

	connect( m_ui.pbOptionsHistory,&QPushButton::clicked,[ this ](){

		auto engineName = this->defaultEngineName() ;

		auto& a = *m_ui.lineEditOptions ;
		auto b = m_settings.getOptionsHistory( settings::tabName::basic,engineName ) ;
		auto c = settings::tabName::basic ;

		if( utility::showHistory( a,b,m_settings,engineName,c ) ){

			if( m_settings.autoDownload() ){

				m_tableList.setVisible( false ) ;

				this->download( m_ui.lineEditURL->text() ) ;
			}
		}
	} ) ;

	connect( m_ui.pbQuit,&QPushButton::clicked,[ this ](){

		m_ctx.mainWindow().quitApp() ;
	} ) ;

	auto s = static_cast< void( QComboBox::* )( int ) >( &QComboBox::activated ) ;

	connect( m_ui.cbEngineType,s,[ & ]( int s ){

		if( s != -1 ){

			m_ui.lineEditOptions->clear() ;
			this->changeDefaultEngine( s ) ;

			auto e = m_ui.cbEngineType->currentText() ;
			auto m = m_settings.lastUsedOption( e,settings::tabName::basic ) ;

			m_ui.lineEditOptions->setText( m ) ;
		}
	} ) ;	
}

void basicdownloader::keyPressed( utility::mainWindowKeyCombo e )
{
	if( e == utility::mainWindowKeyCombo::ENTER ){

		if( m_ui.pbDownload->isEnabled() ){

			m_ui.pbDownload->click() ;
		}
	}
}

void basicdownloader::init_done()
{
}

void basicdownloader::changeDefaultEngine( int s )
{
	auto& engines = m_ctx.Engines() ;

	const auto& engine = engines.getEngineByName( m_ui.cbEngineType->itemText( s ) ) ;

	if( engine ){

		m_settings.setDefaultEngine( engine.value().name(),settings::tabName::basic ) ;

		m_ctx.TabManager().setDefaultEngines() ;
	}else{
		auto id = utility::concurrentID() ;
		auto m = "Error: basicdownloader::basicdownloader: Unknown Engine:" ;

		m_ctx.logger().add( m + m_ui.cbEngineType->itemText( s ),id ) ;
	}
}

QStringList basicdownloader::enginesList()
{
	QStringList m ;

	for( int s = 0 ; s < m_ui.cbEngineType->count() ; s++ ){

		m.append( m_ui.cbEngineType->itemText( s ) ) ;
	}

	return m ;
}

void basicdownloader::resetMenu( const QStringList& args )
{
	utility::setMenuOptions( m_ctx,args,false,true,m_ui.pbEntries,[ this ]( QAction * aa ){

		utility::selectedAction ac( aa ) ;

		if( ac.clearOptions() ){

			m_ui.lineEditOptions->clear() ;

		}else if( ac.clearScreen() ){

			m_ctx.logger().clear() ;

		}else if( ac.openFolderPath() ){

			utility::openDownloadFolderPath( m_settings.downloadFolder() ) ;
		}else{
			m_ui.lineEditOptions->setText( ac.objectName() ) ;

			if( m_settings.autoDownload() ){

				this->download( m_ui.lineEditURL->text() ) ;
			}
		}
	} ) ;
}

basicdownloader& basicdownloader::setAsActive()
{
	m_ui.tabWidget->setCurrentIndex( 0 ) ;
	return *this ;
}

basicdownloader& basicdownloader::hideTableList()
{
	m_tableList.setVisible( false ) ;
	return *this ;
}

QString basicdownloader::defaultEngineName()
{
	const auto& m = m_ctx.Engines().defaultEngineName() ;
	return m_settings.defaultEngine( settings::tabName::basic,m ) ;
}

basicdownloader::engine basicdownloader::defaultEngine()
{
	auto id = utility::concurrentID() ;
	return { m_ctx.Engines().defaultEngine( this->defaultEngineName(),id ),id } ;
}

void basicdownloader::updateEnginesList( const QStringList& e )
{
	auto& comboBox = *m_ui.cbEngineType ;

	comboBox.clear() ;

	for( const auto& it : e ){

		comboBox.addItem( it ) ;
	}

	auto m = this->defaultEngineName() ;
	auto s = settings::tabName::basic ;

	utility::setUpdefaultEngine( comboBox,m,m_settings,s ) ;
}

void basicdownloader::clipboardData( const QString& )
{
}

void basicdownloader::retranslateUi()
{
	this->resetMenu() ;
}

void basicdownloader::listRequested( const QByteArray& a,int id )
{
	if( a.isEmpty() ){

		m_tableList.setVisible( false ) ;
	}else{
		auto m = m_ui.cbEngineType->currentText() ;

		const auto& engine = m_ctx.Engines().defaultEngine( m,id ) ;

		auto ee = engine.mediaProperties( m_ctx.logger(),a ) ;

		if( ee.size() ){

			for( auto& m : ee ){

				m_tableList.add( std::move( m ) ) ;
			}

			m_tableList.setEnabled( true ) ;
		}else{
			m_tableList.setVisible( false ) ;
		}
	}
}

void basicdownloader::list()
{	
	m_tableList.setEnabled( false ) ;

	m_tableList.setVisible( true ) ;

	m_tableList.clear() ;

	m_optionsList.clear() ;

	auto url = m_ui.lineEditURL->text() ;

	const auto& backend = this->defaultEngine() ;

	const auto& engine = backend.engine ;

	auto args = engine.defaultListCmdOptions() ;

	engine.setTextEncondig( args ) ;

	engine.updateCmdOptions( args ) ;

	args.append( url.split( ' ' ) ) ;

	auto mm = m_ui.lineEditOptions->text() ;

	utility::addToListOptionsFromsDownload( args,mm,m_ctx,engine ) ;

	utility::setCookieOption( args,m_settings,engine ) ;

	this->run( backend,args,"",true ) ;
}

void basicdownloader::download( const QString& url )
{
	if( url.isEmpty() ){

		return ;
	}

	m_settings.setLastUsedOption( m_ui.cbEngineType->currentText(),
				      m_ui.lineEditOptions->text(),
				      settings::tabName::basic ) ;

	auto m = util::splitPreserveQuotes( url ) ;

	if( m.size() ){

		if( m[ 0 ] == "yt-dlp" ){

			m.removeAt( 0 ) ;
		}
	}

	const auto& engine = this->defaultEngine() ;

	m_hiddenTable.clear() ;

	auto uiText = m.last() ;
	auto state = reportFinished::finishedStatus::notStarted() ;

	tableWidget::entry entry ;

	entry.uiText = uiText ;
	entry.url    = uiText ;
	entry.runningState = state ;

	m_hiddenTable.addItem( entry.move() ) ;

	auto e = m_extraOptions.downloadOptions ;

	m_hiddenTable.setDownloadingOptions( tableWidget::type::DownloadOptions,0,e ) ;

	m_ctx.TabManager().Configure().setDownloadOptions( 0,m_hiddenTable ) ;

	auto s = utility::setDownloadOptions( engine.engine,m_hiddenTable,0 ).downloadOptions ;

	auto mm = m_ui.lineEditOptions->text() ;

	m_settings.addOptionsHistory( engine.engine.name(),mm,settings::tabName::basic ) ;

	this->download( engine,{ mm,s,engine.engine },m,false ) ;
}

void basicdownloader::download( const basicdownloader::engine& engine,
				const utility::args& args,
				const QString& url,
				bool s )
{
	this->download( engine,args,QStringList( url ),s ) ;
}

void basicdownloader::download( const basicdownloader::engine& eng,
				const utility::args& args,
				const QStringList& urls,
				bool update )
{	
	eng.engine.updateVersionInfo( m_ctx,[ this,eng,args,urls,update ](){

		const auto& engine = eng.engine ;

		m_tableList.setVisible( false ) ;

		m_ctx.logger().setMaxProcessLog( 1 ) ;

		if( update ){

			m_ui.lineEditOptions->setText( args.options().join( ' ' ) ) ;

			m_ui.lineEditURL->setText( urls.join( ' ' ) ) ;
		}

		m_ui.tabWidget->setCurrentIndex( 0 ) ;

		m_ui.pbCancel->setEnabled( true ) ;

		auto& mm = m_extraOptions ;
		auto& ss = m_settings ;

		utility::updateOptionsStruct str{ mm,engine,ss,args,{},false,urls,{},m_ctx } ;

		auto opts = utility::updateOptions( str ) ;

		opts.append( engine.extraArguments() ) ;

		this->run( eng,opts,args.credentials(),false ) ;
	} ) ;
}

void basicdownloader::run( const basicdownloader::engine& eng,
			   const QStringList& args,
			   const QString& credentials,
			   bool getList )
{
	class events
	{
	public:
		events( basicdownloader& p,int id,bool l,const engines::engine& engine ) :
			m_parent( p ),m_engine( engine ),m_id( id ),m_getList( l )
		{
		}
		void done( engines::ProcessExitState m,const std::vector< QByteArray >& fileNames )
		{
			m_parent.enableAll() ;

			m_parent.m_ui.pbCancel->setEnabled( false ) ;

			if( m_getList ){

				m_parent.listRequested( m_listData,m_id ) ;
			}else{
				auto e = reportFinished::finishedStatus::state::done ;

				auto a = reportFinished::finishedStatus( e,m.move() ) ;

				auto& s = m_parent.m_ctx.Settings() ;

				auto& t = m_parent.m_hiddenTable ;

				utility::updateFinishedState( m_engine,s,t,"basic",a.move(),fileNames ) ;

				if( m.success() ){

					const auto& s = m_parent.m_ctx ;

					if( s.Settings().desktopNotifyOnDownloadComplete() ){

						s.mainWindow().notifyOnDownloadComplete() ;

					}else if( s.Settings().desktopNotifyOnAllDownloadComplete() ){

						s.mainWindow().notifyOnDownloadComplete() ;
					}
				}
			}
		}
		void disableAll()
		{
			m_parent.disableAll() ;

			m_parent.m_ui.pbCancel->setEnabled( true ) ;
		}
		bool addData( const QByteArray& e )
		{
			if( m_getList ){

				m_listData += e ;

				return utility::addData( e ) ;
			}else{
				if( utility::containsLinkerWarning( e ) ){

					return false ;
				}else{
					return true ;
				}
			}
		}
		const engines::engine& engine()
		{
			return m_engine ;
		}
		utility::ProcessOutputChannels outPutChannel( const engines::engine& engine )
		{
			if( m_getList ){

				auto m = QProcess::ProcessChannel::StandardOutput ;

				engine.updateOutPutChannel( m ) ;

				return utility::ProcessOutputChannels( m ) ;
			}else{
				return utility::ProcessOutputChannels() ;
			}
		}
		int index()
		{
			return -1 ;
		}
		void printOutPut( const QByteArray& e )
		{
			m_parent.m_ctx.debug( m_id,e ) ;
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
		basicdownloader& m_parent ;
		const engines::engine& m_engine ;
		int m_id ;
		bool m_getList ;
		QByteArray m_listData ;
	} ;

	events ev( *this,eng.id,getList,eng.engine ) ;

	auto logs   = m_settings.getLogsLimits() ;
	auto ch     = ev.outPutChannel( eng.engine ) ;
	auto& ll    = m_ctx.logger() ;
	auto update = []( const QByteArray& ){} ;
	auto logger = make_loggerBasicDownloader( eng.engine.filter( eng.id ),ll,update,eng.id,logs ) ;
	auto term   = m_terminator.setUp( m_ui.pbCancel,&QPushButton::clicked,-1 ) ;
	auto ctx    = utility::make_ctx( m_ctx,ev.move(),logger.move(),term.move(),ch ) ;

	utility::run( args,credentials,ctx.move() ) ;
}

void basicdownloader::tabEntered()
{
	auto e = m_ui.cbEngineType->currentText() ;
	auto m = m_settings.lastUsedOption( e,settings::tabName::basic ) ;

	m_ui.lineEditOptions->setText( m ) ;
	m_ui.lineEditURL->setFocus() ;
	m_ctx.logger().updateView( true ) ;
}

void basicdownloader::tabExited()
{
	m_ctx.logger().updateView( false ) ;
}

void basicdownloader::enableQuit()
{
	m_ui.pbQuit->setEnabled( true ) ;
}

void basicdownloader::enableAll()
{
	m_ui.pbOptionsDownloadOptions->setEnabled( true ) ;
	m_ui.pbOptionsHistory->setEnabled( true ) ;
	m_ui.pbPasteClipboard->setEnabled( true ) ;
	m_ui.cbEngineType->setEnabled( true ) ;
	m_ui.pbEntries->setEnabled( true ) ;
	m_ui.label_2->setEnabled( true ) ;
	m_ui.label->setEnabled( true ) ;
	m_ui.pbList->setEnabled( true ) ;
	m_ui.pbDownload->setEnabled( true ) ;
	m_ui.lineEditURL->setEnabled( true ) ;
	m_ui.lineEditOptions->setEnabled( true ) ;
	m_ui.pbQuit->setEnabled( true ) ;
	m_ui.labelEngineName->setEnabled( true ) ;
	m_ui.pbBasicDownloaderPlay->setEnabled( true ) ;
}

void basicdownloader::disableAll()
{
	m_ui.pbBasicDownloaderPlay->setEnabled( false ) ;
	m_ui.pbOptionsDownloadOptions->setEnabled( false ) ;
	m_ui.pbOptionsHistory->setEnabled( false ) ;
	m_ui.pbPasteClipboard->setEnabled( false ) ;
	m_ui.cbEngineType->setEnabled( false ) ;
	m_ui.pbQuit->setEnabled( false ) ;
	m_ui.pbEntries->setEnabled( false ) ;
	m_ui.labelEngineName->setEnabled( false ) ;
	m_ui.label_2->setEnabled( false ) ;
	m_ui.label->setEnabled( false ) ;
	m_ui.pbList->setEnabled( false ) ;
	m_ui.pbDownload->setEnabled( false ) ;
	m_ui.lineEditURL->setEnabled( false ) ;
	m_ui.lineEditOptions->setEnabled( false ) ;
}

void basicdownloader::exiting()
{
}

void basicdownloader::textAlignmentChanged( Qt::LayoutDirection m )
{
	utility::alignText( m,m_ui.label,m_ui.label_2,m_ui.labelEngineName ) ;
}

void basicdownloader::gotEvent( const QJsonObject& )
{
}

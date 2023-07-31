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

basicdownloader::basicdownloader( const Context& ctx ) :
	m_ctx( ctx ),
	m_settings( m_ctx.Settings() ),
	m_debug( ctx.debug() ),
	m_ui( m_ctx.Ui() ),
	m_tabManager( m_ctx.TabManager() ),
	m_tableList( *m_ui.bdTableWidgetList,m_ctx.mainWidget().font() ),
	m_bogusTable( m_bogusTableOriginal,m_ctx.mainWidget().font(),0,m_settings.textAlignment() )
{
	this->setAsActive() ;

	m_ui.pbCancel->setEnabled( false ) ;

	m_tableList.setVisible( false ) ;

	tableWidget::tableWidgetOptions opts ;

	opts.customContextPolicy = Qt::NoContextMenu ;
	opts.selectionMode       = QAbstractItemView::ExtendedSelection ;

	m_tableList.setTableWidget( opts ) ;

	connect( m_ui.pbPasteClipboard,&QPushButton::clicked,[ this ](){

		m_ui.lineEditURL->setText( utility::clipboardText() ) ;
	} ) ;

	m_tableList.connect( &QTableWidget::itemClicked,[ this ]( QTableWidgetItem * item ){

		if( item ){

			m_tableList.selectMediaOptions( m_optionsList,*item,*m_ui.lineEditOptions ) ;
		}
	} ) ;

	connect( m_ui.pbOptionsDownloadOptions,&QPushButton::clicked,[ this ](){

		auto& t = m_ctx.TabManager().Configure() ;

		t.engineDefaultDownloadOptions( this->defaultEngineName(),[ this ]( const QString& e ){

			m_extraOptions.hasExtraOptions = true ;
			m_extraOptions.downloadOptions = e ;
		} ) ;
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

		auto s = utility::showHistory( *m_ui.lineEditOptions,
					       m_settings.getOptionsHistory( settings::tabName::basic ),
					       m_settings,settings::tabName::basic ) ;

		if( s ){

			m_tableList.setVisible( false ) ;

			this->download( m_ui.lineEditURL->text() ) ;
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
		}
	} ) ;

	auto& table = m_bogusTable.get() ;

	table.insertRow( 0 ) ;

	for( int s = 0 ; s < 3 ; s++ ){

		table.insertColumn( s ) ;
		table.setItem( 0,s,new QTableWidgetItem ) ;
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
		m_ctx.logger().add( "Error: basicdownloader::basicdownloader: Unknown Engine:" + m_ui.cbEngineType->itemText( s ),id ) ;
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
	utility::setMenuOptions( m_ctx,args,true,true,m_ui.pbEntries,[ this ]( QAction * aa ){

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
	return m_settings.defaultEngine( settings::tabName::basic,m_ctx.Engines().defaultEngineName() ) ;
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

	auto s = settings::tabName::basic ;

	utility::setUpdefaultEngine( comboBox,
				     this->defaultEngineName(),
				     [ this,s ]( const QString& e ){ m_settings.setDefaultEngine( e,s ) ; } ) ;
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

		auto ee = engine.mediaProperties( a ) ;

		if( ee.size() ){

			for( const auto& m : ee ){

				m_tableList.add( m.toStringList(),m ) ;
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

	m_ui.lineEditOptions->clear() ;

	auto url = m_ui.lineEditURL->text() ;

	const auto& backend = this->defaultEngine() ;

	auto args = backend.engine.defaultListCmdOptions() ;
	args.append( url.split( ' ' ) ) ;

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

	auto m = util::split( url,' ',true ) ;

	const auto& engine = this->defaultEngine() ;

	m_bogusTable.clear() ;

	auto uiText = m.at( 0 ) ;
	auto state = downloadManager::finishedStatus::notStarted() ;

	tableWidget::entry entry ;

	entry.uiText = uiText ;
	entry.url    = uiText ;
	entry.runningState = state ;

	m_bogusTable.addItem( std::move( entry ) ) ;

	auto e = m_extraOptions.downloadOptions ;

	m_bogusTable.setDownloadingOptions( tableWidget::type::DownloadOptions,0,e ) ;

	m_ctx.TabManager().Configure().setDownloadOptions( 0,m_bogusTable ) ;

	auto s = utility::setDownloadOptions( engine.engine,m_bogusTable,0 ).downloadOptions ;

	auto mm = m_ui.lineEditOptions->text() ;

	qDebug() << mm ;

	m_settings.addOptionsHistory( mm,settings::tabName::basic ) ;

	this->download( engine,{ mm,s,engine.engine },m,false ) ;
}

void basicdownloader::download( const basicdownloader::engine& engine,
				const utility::args& args,
				const QString& url,
				bool s )
{
	this->download( engine,args,QStringList( url ),s ) ;
}

void basicdownloader::download( const basicdownloader::engine& engine,
				const utility::args& args,
				const QStringList& urls,
				bool update )
{	
	engine.engine.updateVersionInfo( m_ctx,[ this,engine,args,urls,update ](){

		m_tableList.setVisible( false ) ;

		m_ctx.logger().setMaxProcessLog( 1 ) ;

		if( update ){

			m_ui.lineEditOptions->setText( args.options().join( ' ' ) ) ;

			m_ui.lineEditURL->setText( urls.join( ' ' ) ) ;
		}

		m_ui.tabWidget->setCurrentIndex( 0 ) ;

		m_ui.pbCancel->setEnabled( true ) ;

		auto opts = utility::updateOptions( { m_extraOptions,
						      engine.engine,
						      m_settings,
						      args,
						      {},
						      false,
						      urls,
						      {},
						      m_ctx } ) ;

		this->run( engine,opts,args.credentials(),false ) ;
	} ) ;
}

void basicdownloader::run( const basicdownloader::engine& eng,
			   const QStringList& args,
			   const QString& credentials,
			   bool list_requested )
{
	auto id = eng.id ;
	const auto& engine = eng.engine ;

	auto functions = utility::OptionsFunctions( [ this,id ]( const utility::ProcessExitState&,const QByteArray& args ){

			this->listRequested( args,id ) ;

		},[ this ]( const basicdownloader::opts& opts ){

			opts.ctx.TabManager().disableAll() ;

			m_ui.pbCancel->setEnabled( true ) ;

		},[ this ]( utility::ProcessExitState m,const basicdownloader::opts& opts ){

			opts.ctx.TabManager().enableAll() ;

			m_ui.pbCancel->setEnabled( false ) ;

			if( !opts.listRequested ){

				auto e = downloadManager::finishedStatus::state::done ;

				auto a = downloadManager::finishedStatus( e,std::move( m ) ) ;

				auto& s = opts.ctx.Settings() ;

				utility::updateFinishedState( opts.engine,s,opts.table,std::move( a ) ) ;
			}
		}
	 ) ;

	basicdownloader::opts opts{ engine,m_bogusTable,m_ctx,m_debug,list_requested,-1 } ;

	auto oopts  = basicdownloader::make_options( engine,std::move( opts ),std::move( functions ) ) ;
	auto logger = LoggerWrapper( m_ctx.logger(),id ) ;
	auto term   = m_terminator.setUp( m_ui.pbCancel,&QPushButton::clicked,-1 ) ;

	auto ctx = utility::make_ctx( engine,std::move( oopts ),std::move( logger ),std::move( term ),[ & ](){

		if( list_requested ){

			auto m = QProcess::ProcessChannel::StandardOutput ;

			engine.updateOutPutChannel( m ) ;

			return utility::ProcessOutputChannels( m ) ;
		}else{
			return utility::ProcessOutputChannels() ;
		}
	}() ) ;

	utility::run( args,credentials,std::move( ctx ) ) ;
}

void basicdownloader::tabEntered()
{
	auto m = m_settings.lastUsedOption( m_ui.cbEngineType->currentText(),settings::tabName::basic ) ;
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
}

void basicdownloader::disableAll()
{
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

void basicdownloader::gotEvent( const QByteArray& )
{
}

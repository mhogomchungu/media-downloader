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

basicdownloader::basicdownloader( const Context& ctx ) :
	m_ctx( ctx ),
	m_settings( m_ctx.Settings() ),
	m_debug( ctx.debug() ),
	m_ui( m_ctx.Ui() ),
	m_tabManager( m_ctx.TabManager() ),
	m_tableList( *m_ui.bdTableWidgetList,m_ctx.mainWidget().font(),0 ),
	m_bogusTable( m_bogusTableOriginal,m_ctx.mainWidget().font(),0 )
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

		utility::showHistory( *m_ui.lineEditOptions,
				      m_settings.getOptionsHistory( settings::tabName::basic ),
				      m_settings,
				      settings::tabName::basic ) ;
	} ) ;

	connect( m_ui.pbQuit,&QPushButton::clicked,[ this ](){

		this->appQuit() ;
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
	const auto& engines = m_ctx.Engines().getEngines() ;

	if( engines.size() > 0 ){

		for( const auto& engine : engines ){

			if( engine.mainEngine() ){

				m_ui.cbEngineType->addItem( engine.name() ) ;
			}
		}

		this->setDefaultEngine() ;

		if( m_settings.showVersionInfoWhenStarting() ){

			this->printEngineVersionInfo() ;
		}else{
			m_counter = static_cast< size_t >( -1 ) ;
		}
	}else{
		m_tabManager.disableAll() ;

		m_ui.pbQuit->setEnabled( true ) ;
	}
}

void basicdownloader::printEngineVersionInfo()
{
	const auto& engines = m_ctx.Engines().getEngines() ;

	if( m_counter < engines.size() ){

		const auto& engine = engines[ m_counter ] ;

		m_counter++ ;

		this->printEngineVersionInfo( engine ) ;
	}
}

void basicdownloader::changeDefaultEngine( int s )
{
	auto& engines = m_ctx.Engines() ;

	const auto& engine = engines.getEngineByName( m_ui.cbEngineType->itemText( s ) ) ;

	if( engine ){

		m_settings.setDefaultEngine( engine.value().name(),settings::tabName::basic ) ;

		this->setDefaultEngine() ;
	}else{
		m_ctx.logger().add( "Error: basicdownloader::basicdownloader: Unknown Engine:" + m_ui.cbEngineType->itemText( s ) ) ;
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

void basicdownloader::printEngineVersionInfo( const engines::engine& engine )
{
	if( engine.usingPrivateBackend() && !engine.downloadUrl().isEmpty() && networkAccess::hasNetworkSupport() ){

		if( engine.backendExists() ){

			this->checkAndPrintInstalledVersion( engine ) ;

		}else if( !engine.exePath().realExe().isEmpty() ){

			m_ctx.TabManager().Configure().downloadFromGitHub( engine ) ;
		}
	}else{
		if( engine.exePath().isEmpty() ){

			m_ctx.logger().add( tr( "Failed to find version information, make sure \"%1\" is installed and works properly" ).arg( engine.name() ) ) ;
		}else{
			this->checkAndPrintInstalledVersion( engine ) ;
		}
	}
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

void basicdownloader::setDefaultEngine()
{
	auto m = m_settings.defaultEngine( settings::tabName::basic ) ;

	for( int i = 0 ; i < m_ui.cbEngineType->count() ; i++ ){

		if( m_ui.cbEngineType->itemText( i ) == m ){

			m_ui.cbEngineType->setCurrentIndex( i ) ;

			auto e = this->enginesList() ;

			auto& t = m_ctx.TabManager() ;

			t.batchDownloader().updateEnginesList( e ) ;
			t.playlistDownloader().updateEnginesList( e ) ;
			t.Configure().updateEnginesList( e ) ;

			const auto& s = m_ctx.Engines().getEngineByName( m ) ;

			if( s ){

				m_ui.pbList->setEnabled( !s->defaultListCmdOptions().isEmpty() ) ;
			}

			return ;
		}
	}

	m_settings.setDefaultEngine( m_ui.cbEngineType->itemText( 0 ),settings::tabName::basic ) ;
	m_ui.cbEngineType->setCurrentIndex( 0 ) ;
	m_ctx.TabManager().batchDownloader().updateEnginesList( this->enginesList() ) ;
	m_ctx.TabManager().playlistDownloader().updateEnginesList( this->enginesList() ) ;
}

void basicdownloader::retranslateUi()
{
	this->resetMenu() ;
}

void basicdownloader::checkAndPrintInstalledVersion( const engines::engine& engine )
{
	m_tabManager.disableAll() ;

	struct ctx
	{
		ctx( const engines::engine& e,const Context& c ) :
			engine( e ),context( c )
		{
		}
		QByteArray data ;
		const engines::engine& engine ;
		const Context& context ;
	} ;

	engines::engine::exeArgs::cmd cmd( engine.exePath(),{ engine.versionArgument() } ) ;

	utility::run( cmd.exe(),cmd.args(),[ this,&engine ]( QProcess& exe ){

		exe.setProcessChannelMode( QProcess::ProcessChannelMode::MergedChannels ) ;

		m_ctx.logger().add( tr( "Checking installed version of" ) + " " + engine.name() ) ;

		return ctx( engine,m_ctx ) ;

	},[]( QProcess& ){},[ this ]( int exitCode,QProcess::ExitStatus exitStatus,ctx& ctx ){

		if( exitStatus == QProcess::ExitStatus::CrashExit || exitCode != 0 ){

			for( int i = 0 ; i < m_ui.cbEngineType->count() ; i++ ){

				if( m_ui.cbEngineType->itemText( i ) == ctx.engine.name() ){

					m_ui.cbEngineType->removeItem( i ) ;

					this->setDefaultEngine() ;

					break ;
				}
			}

			ctx.context.logger().add( tr( "Failed to find version information, make sure \"%1\" is installed and works properly" ).arg( ctx.engine.name() ) ) ;

			m_tabManager.enableAll() ;
		}else{
			auto& logger = ctx.context.logger() ;

			logger.add( tr( "Found version" ) + ": " + ctx.engine.versionString( ctx.data ) ) ;

			if( ctx.context.debug() ){

				logger.add( tr( "Executable Path" ) + ": " + ctx.engine.exePath().realExe() ) ;
			}

			m_tabManager.enableAll() ;
		}

		this->printEngineVersionInfo() ;

	},[]( QProcess::ProcessChannel,const QByteArray& data,ctx& ctx ){

		ctx.data += data ;
	} ) ;
}

void basicdownloader::listRequested( const QList< QByteArray >& args )
{
	for( const auto& it : args ){

		if( it.contains( "ERROR:" ) ){

			m_tableList.setVisible( false ) ;
			return ;
		}
	}

	const auto& engine = m_ctx.Engines().defaultEngine( m_ui.cbEngineType->currentText() ) ;

	m_tableList.showOptions( engine,args ) ;

	m_tableList.setEnabled( true ) ;
}

void basicdownloader::list()
{	
	m_tableList.setEnabled( false ) ;

	m_tableList.setVisible( true ) ;

	m_tableList.clear() ;

	m_optionsList.clear() ;

	m_ui.lineEditOptions->clear() ;

	auto url = m_ui.lineEditURL->text() ;

	auto m = m_settings.defaultEngine( settings::tabName::basic ) ;

	const auto& backend = m_ctx.Engines().defaultEngine( m ) ;

	auto args = backend.defaultListCmdOptions() ;
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

	auto m = utility::split( url,' ',true ) ;

	const auto& engine = m_ctx.Engines().defaultEngine( m_settings.defaultEngine( settings::tabName::basic ) ) ;

	m_bogusTable.clear() ;

	QStringList args{ m.at( 0 ),m.at( 0 ),downloadManager::finishedStatus::notStarted() } ;

	m_bogusTable.addItem( args ) ;

	auto s = m_ui.lineEditOptions->text() ;

	m_settings.addOptionsHistory( s,settings::tabName::basic ) ;

	this->download( engine,s,m,false ) ;
}

void basicdownloader::download( const engines::engine& engine,
				const utility::args& args,
				const QString& url,
				bool s )
{
	this->download( engine,args,QStringList( url ),s ) ;
}

void basicdownloader::download( const engines::engine& engine,
				const utility::args& args,
				const QStringList& urls,
				bool update )
{
	m_tableList.setVisible( false ) ;

	if( update ){

		m_ui.lineEditOptions->setText( args.quality() + " " + args.otherOptions().join( ' ' ) ) ;

		m_ui.lineEditURL->setText( urls.join( ' ' ) ) ;
	}

	m_ui.tabWidget->setCurrentIndex( 0 ) ;

	m_ui.pbCancel->setEnabled( true ) ;

	auto opts = utility::updateOptions( engine,m_settings,args,urls ) ;

	this->run( engine,opts,args.quality(),false ) ;
}

void basicdownloader::run( const engines::engine& engine,
			   const QStringList& args,
			   const QString& quality,
			   bool list_requested )
{
	auto functions = utility::OptionsFunctions( [ this ]( const QList< QByteArray >& args ){

			this->listRequested( args ) ;

		},[]( const basicdownloader::opts& opts ){

			opts.ctx.TabManager().disableAll() ;

		},[ this ]( utility::ProcessExitState m,const basicdownloader::opts& opts ){

			m_ctx.TabManager().enableAll() ;

			m_ui.pbCancel->setEnabled( false ) ;

			if( !opts.listRequested ){

				auto a = downloadManager::finishedStatus( 0,true,std::move( m ) ) ;

				auto& s = opts.ctx.Settings() ;

				utility::updateFinishedState( opts.engine,s,opts.table,std::move( a ) ) ;
			}
		}
	 ) ;

	basicdownloader::opts opts{ engine,m_bogusTable,m_ctx,m_debug,list_requested,-1 } ;

	utility::run( engine,
		      args,
		      quality,
		      basicdownloader::make_options( std::move( opts ),std::move( functions ) ),
		      LoggerWrapper( m_ctx.logger(),utility::concurrentID() ),
		      m_terminator.setUp( m_ui.pbCancel,&QPushButton::clicked,-1 ) ) ;
}

void basicdownloader::updateEngines()
{
	const auto& engines = m_ctx.Engines().getEngines() ;

	m_ui.cbEngineType->clear() ;

	if( engines.size() > 0 ){

		for( const auto& engine : engines ){

			m_ui.cbEngineType->addItem( engine.name() ) ;
		}

		this->setDefaultEngine() ;
	}
}

void basicdownloader::downloadDefaultEngine()
{
	this->updateEngines() ;

	m_counter = static_cast< size_t >( -1 ) ;

	auto m = m_settings.defaultEngine( settings::tabName::basic ) ;

	this->printEngineVersionInfo( m_ctx.Engines().defaultEngine( m ) ) ;
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

void basicdownloader::appQuit()
{
	m_settings.setTabNumber( m_ui.tabWidget->currentIndex() ) ;

	QCoreApplication::quit() ;
}

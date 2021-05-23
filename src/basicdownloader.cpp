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
	m_tableList( *m_ui.bdTableWidgetList )
{
	this->setAsActive() ;

	m_ui.pbCancel->setEnabled( false ) ;

	m_tableList.setVisible( false ) ;

	utility::tableWidgetOptions opts ;

	opts.customContextPolicy = Qt::NoContextMenu ;
	opts.selectionMode       = QAbstractItemView::ExtendedSelection ;

	utility::setTableWidget( m_tableList,opts ) ;

	connect( &m_tableList,&QTableWidget::itemClicked,[ this ]( QTableWidgetItem * item ){

		if( item->isSelected() ){

			auto text = m_tableList.item( item->row(),0 )->text() ;

			if( !m_optionsList.contains( text ) ){

				m_optionsList.append( text ) ;
			}
		}

		for( int row = 0 ; row < m_tableList.rowCount() ; row++ ){

			auto item = m_tableList.item( row,0 ) ;

			if( !item->isSelected() ){

				m_optionsList.removeAll( item->text() ) ;
			}
		}

		if( m_optionsList.isEmpty() ){

			m_ui.lineEditOptions->clear() ;
		}else{
			m_ui.lineEditOptions->setText( m_optionsList.join( "+" ) ) ;
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

	m_bogusTable.insertRow( 0 ) ;

	for( int s = 0 ; s < 3 ; s++ ){

		m_bogusTable.insertColumn( s ) ;
		m_bogusTable.setItem( 0,s,new QTableWidgetItem ) ;
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
	utility::setMenuOptions( m_ctx,args,true,m_ui.pbEntries,[ this ]( QAction * aa ){

		utility::selectedAction ac( aa ) ;

		if( ac.clearOptions() ){

			m_ui.lineEditOptions->clear() ;

		}else if( ac.clearScreen() ){

			m_ctx.logger().clear() ;
			m_ui.lineEditURL->clear() ;
			m_ui.lineEditOptions->clear() ;

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

			m_ctx.TabManager().batchDownloader().updateEnginesList( this->enginesList() ) ;
			m_ctx.TabManager().playlistDownloader().updateEnginesList( this->enginesList() ) ;

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

void basicdownloader::tabManagerEnableAll( bool e )
{
	if( e ){

		m_tabManager.enableAll() ;
	}else{
		m_tabManager.disableAll() ;
	}
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

	QStringList m ;

	utility::make_reverseIterator( args ).forEach( [ & ]( const QByteArray& s ){

		auto a = utility::split( s,' ',true ) ;

		auto _digits_only = []( const QString& e ){

			for( const auto& it : e ){

				if( !( it >= '0' && it <= '9' ) ){

					return false ;
				}
			}

			return true ;
		} ;

		if( a.size() > 1 ){

			if( _digits_only( a.at( 0 ) ) ){

				m.insert( 0,s ) ;
			}else{
				return true ;
			}
		}

		return false ;
	} ) ;

	for( const auto& it : m ){

		auto a = utility::split( it,' ',true ) ;

		if( a.size() > 3 ){

			auto format     = a.takeAt( 0 ) ;
			auto extension  = a.takeAt( 0 ) ;
			auto resolution = a.takeAt( 0 ) ;
			auto notes      = a.join( " " ) ;

			QStringList args{ format,extension,resolution,notes } ;

			utility::addItem( m_tableList,args,m_ctx.mainWidget().font() ) ;
		}
	}

	m_tableList.setEnabled( true ) ;
}

void basicdownloader::list()
{	
	m_tableList.setEnabled( false ) ;

	m_tableList.setVisible( true ) ;

	utility::clear( m_tableList ) ;

	m_optionsList.clear() ;

	m_ui.lineEditOptions->clear() ;

	m_ui.pbCancel->setEnabled( true ) ;

	auto url = m_ui.lineEditURL->text() ;

	const auto& backend = m_ctx.Engines().defaultEngine( m_settings.defaultEngine( settings::tabName::basic ) ) ;

	auto args = backend.defaultListCmdOptions() ;
	args.append( url.split( ' ' ) ) ;

	this->run( backend,args,"",true ) ;
}

void basicdownloader::download( const QString& url )
{
	if( url.isEmpty() ){

		return ;
	}

	m_settings.setLastUsedOption( m_ui.lineEditOptions->text(),settings::tabName::basic ) ;

	auto m = utility::split( url,' ',true ) ;

	const auto& engine = m_ctx.Engines().defaultEngine( m_settings.defaultEngine( settings::tabName::basic ) ) ;

	utility::clear( m_bogusTable ) ;

	QStringList args{ m.at( 0 ),m.at( 0 ),concurrentDownloadManagerFinishedStatus::notStarted() } ;

	utility::addItem( m_bogusTable,args,m_ctx.mainWidget().font() ) ;

	this->download( engine,m_ui.lineEditOptions->text(),m,false ) ;
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

		m_ui.lineEditOptions->setText( args.quality + " " + args.otherOptions.join( ' ' ) ) ;

		m_ui.lineEditURL->setText( urls.join( ' ' ) ) ;
	}

	m_ui.tabWidget->setCurrentIndex( 0 ) ;

	m_ui.pbCancel->setEnabled( true ) ;

	auto opts = utility::updateOptions( engine,args,urls ) ;

	this->run( engine,opts,args.quality,false ) ;
}

void basicdownloader::run( const engines::engine& engine,
			   const QStringList& args,
			   const QString& quality,
			   bool list_requested )
{
	utility::run( engine,
		      args,
		      quality,
		      basicdownloader::options( *m_ui.pbCancel,m_ctx,engine,m_bogusTable,m_debug,list_requested ),
		      LoggerWrapper( m_ctx.logger(),utility::concurrentID() ),
		      utility::make_term_conn( m_ui.pbCancel,&QPushButton::clicked ) ) ;
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

	this->printEngineVersionInfo( m_ctx.Engines().defaultEngine( m_settings.defaultEngine( settings::tabName::basic ) ) ) ;
}

void basicdownloader::tabEntered()
{
	m_ui.lineEditOptions->setText( m_settings.lastUsedOption( settings::tabName::basic ) ) ;
}

void basicdownloader::tabExited()
{
}

void basicdownloader::enableQuit()
{
	m_ui.pbQuit->setEnabled( true ) ;
}

void basicdownloader::enableAll()
{
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

void basicdownloader::options::done( utility::ProcessExitState m )
{
	this->tabManagerEnableAll( true ).enableCancel( false ) ;

	if( !m_listRequested ){

		utility::updateFinishedState( m_engine,m_ctx.Settings(),m_table,{ 0,true,std::move( m ) } ) ;
	}
}

basicdownloader::options& basicdownloader::options::tabManagerEnableAll( bool e )
{
	if( e ){

		m_ctx.TabManager().enableAll() ;
	}else{
		m_ctx.TabManager().disableAll() ;
	}

	return *this ;
}

basicdownloader::options& basicdownloader::options::listRequested( const QList< QByteArray >& e )
{
	m_ctx.TabManager().basicDownloader().listRequested( e ) ;

	return *this ;
}

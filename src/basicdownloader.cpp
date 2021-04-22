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
	m_tabManager( m_ctx.TabManager() )
{
	this->setAsActive() ;

	m_ui.pbCancel->setEnabled( false ) ;

	m_ui.bdTableWidgetList->setVisible( false ) ;

	utility::tableWidgetOptions opts ;

	opts.customContextPolicy = Qt::NoContextMenu ;
	opts.selectionMode       = QAbstractItemView::ExtendedSelection ;
	opts.customContextPolicy = Qt::NoContextMenu ;

	utility::setTableWidget( *m_ui.bdTableWidgetList,opts ) ;

	connect( m_ui.bdTableWidgetList,&QTableWidget::itemClicked,[ this ]( QTableWidgetItem * ){

		auto rowCount = m_ui.bdTableWidgetList->rowCount() ;

		QStringList txt ;

		for( int row = 0 ; row < rowCount ; row++ ){

			auto item = m_ui.bdTableWidgetList->item( row,0 ) ;

			auto m = item->text() ;

			if( item->isSelected() ){

				if( !txt.contains( m ) ){

					txt.append( m ) ;
				}
			}else{
				txt.removeAll( m ) ;
			}
		}

		if( txt.isEmpty() ){

			m_ui.lineEditOptions->clear() ;
		}else{
			m_ui.lineEditOptions->setText( txt.join( "+" ) ) ;
		}
	} ) ;

	connect( m_ui.pbList,&QPushButton::clicked,[ this ](){

		m_ui.bdTableWidgetList->setVisible( false ) ;

		this->list() ;
	} ) ;

	this->resetMenu() ;

	connect( m_ui.pbDownload,&QPushButton::clicked,[ this ](){

		m_ui.bdTableWidgetList->setVisible( false ) ;

		this->download() ;
	} ) ;

	connect( m_ui.pbQuit,&QPushButton::clicked,[ this ](){

		this->appQuit() ;
	} ) ;

	auto s = static_cast< void( QComboBox::* )( int ) >( &QComboBox::activated ) ;

	connect( m_ui.cbEngineType,s,[ & ]( int s ){

		auto& engines = m_ctx.Engines() ;

		const auto& engine = engines.getEngineByName( m_ui.cbEngineType->itemText( s ) ) ;

		if( engine ){

			engines.setDefaultEngine( engine.value() ) ;

			this->setDefaultEngine() ;

			if( engine->canDownloadPlaylist() ){

				m_ctx.TabManager().playlistDownloader().enableAll() ;
			}else{
				m_ctx.TabManager().playlistDownloader().disableAll() ;
			}
		}else{
			m_ctx.logger().add( "Error: basicdownloader::basicdownloader: Unknown Engine:" + m_ui.cbEngineType->itemText( s ) ) ;
		}
	} ) ;
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

				if( !m_ui.lineEditURL->text().isEmpty() ){

					this->download() ;
				}
			}
		}
	} ) ;
}

basicdownloader& basicdownloader::setAsActive()
{
	m_ui.tabWidget->setCurrentIndex( 0 ) ;
	return *this ;
}

void basicdownloader::setDefaultEngine()
{
	auto m = m_settings.defaultEngine() ;

	for( int i = 0 ; i < m_ui.cbEngineType->count() ; i++ ){

		if( m_ui.cbEngineType->itemText( i ) == m ){

			m_ui.cbEngineType->setCurrentIndex( i ) ;

			const auto& s = m_ctx.Engines().getEngineByName( m ) ;

			if( s ){

				m_ui.pbList->setEnabled( !s->defaultListCmdOptions().isEmpty() ) ;
			}

			return ;
		}
	}

	m_settings.setDefaultEngine( m_ui.cbEngineType->itemText( 0 ) ) ;
	m_ui.cbEngineType->setCurrentIndex( 0 ) ;
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
		ctx( const engines::engine& e,Logger& l ) :
			engine( e ),logger( l )
		{
		}
		QByteArray data ;
		const engines::engine& engine ;
		Logger& logger ;
	} ;

	engines::engine::exeArgs::cmd cmd( engine.exePath(),{ engine.versionArgument() } ) ;

	utility::run( cmd.exe(),cmd.args(),[ this,&engine ]( QProcess& exe ){

		exe.setProcessChannelMode( QProcess::ProcessChannelMode::MergedChannels ) ;

		m_ctx.logger().add( tr( "Checking installed version of" ) + " " + engine.name() ) ;

		return ctx( engine,m_ctx.logger() ) ;

	},[]( QProcess& ){},[ this ]( int exitCode,QProcess::ExitStatus exitStatus,ctx& ctx ){

		if( exitStatus == QProcess::ExitStatus::CrashExit || exitCode != 0 ){

			for( int i = 0 ; i < m_ui.cbEngineType->count() ; i++ ){

				if( m_ui.cbEngineType->itemText( i ) == ctx.engine.name() ){

					m_ui.cbEngineType->removeItem( i ) ;

					this->setDefaultEngine() ;
					break ;
				}
			}

			ctx.logger.add( tr( "Failed to find version information, make sure \"%1\" is installed and works properly" ).arg( ctx.engine.name() ) ) ;

			m_tabManager.enableAll() ;
		}else{
			ctx.logger.add( tr( "Found version" ) + ": " + ctx.engine.versionString( ctx.data ) ) ;

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

			return ;
		}
	}

	utility::clear( *m_ui.bdTableWidgetList ) ;

	m_ui.bdTableWidgetList->setVisible( true ) ;

	QStringList opts ;

	QStringList m ;

	for( auto it = args.rbegin() ; it != args.rend() ; it++ ){

		auto a = utility::split( *it,' ',true ) ;

		if( a.size() > 1 ){

			const auto& e = a.at( 0 ) ;

			if( e == "format" && a.at( 1 ) == "code" ){

				break ;
			}else{
				opts.insert( 0,e ) ;
				m.insert( 0,*it ) ;
			}
		}
	}

	for( const auto& it : m ){

		auto a          = utility::split( it,' ',true ) ;
		auto format     = a.takeAt( 0 ) ;
		auto extension  = a.takeAt( 0 ) ;
		auto resolution = a.takeAt( 0 ) ;
		auto notes      = a.join( " " ) ;

		QStringList args{ format,extension,resolution,notes } ;

		utility::addItem( *m_ui.bdTableWidgetList,args,m_ctx.mainWidget().font() ) ;
	}

	//this->resetMenu( opts ) ;
}

void basicdownloader::list()
{
	m_ui.pbCancel->setEnabled( true ) ;

	auto url = m_ui.lineEditURL->text() ;

	const auto& backend = m_ctx.Engines().defaultEngine() ;

	auto args = backend.defaultListCmdOptions() ;
	args.append( url.split( ' ' ) ) ;

	this->run( backend,args,"",true ) ;
}

void basicdownloader::download()
{
	QString url = m_ui.lineEditURL->text() ;

	auto m = utility::split( url,' ',true ) ;

	const auto& engine = m_ctx.Engines().defaultEngine() ;

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
	m_ui.bdTableWidgetList->setVisible( false ) ;

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
		      basicdownloader::options( *m_ui.pbCancel,m_ctx,m_debug,list_requested ),
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

	this->printEngineVersionInfo( m_ctx.Engines().defaultEngine() ) ;
}

void basicdownloader::tabEntered()
{
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
}

void basicdownloader::disableAll()
{
	m_ui.cbEngineType->setEnabled( false ) ;
	m_ui.pbQuit->setEnabled( false ) ;
	m_ui.pbEntries->setEnabled( false ) ;
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

void basicdownloader::options::done()
{
	this->tabManagerEnableAll( true ).enableCancel( false ) ;
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

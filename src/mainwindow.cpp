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

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "utility.h"
#include "tabmanager.h"

#include "context.hpp"
#include "settings.h"
#include "translator.h"

static std::unique_ptr< Ui::MainWindow > _init_ui( QMainWindow& mw )
{
	auto m = std::make_unique< Ui::MainWindow >() ;
	m->setupUi( &mw ) ;
	return m ;
}

static QString _debug( const QStringList& e )
{
	if( e.contains( "--debug" ) ){

		return "--debug" ;

	}else if( e.contains( "--qdebug" ) ){

		return "--qdebug" ;
	}else{
		return "" ;
	}
}

MainWindow::MainWindow( QApplication& app,
			settings& s,
			translator& t,
			const engines::enginePaths& paths,
			const QStringList& args ) :
	m_trayIcon( QIcon::fromTheme( "media-downloader",QIcon( ":media-downloader" ) ) ),
	m_qApp( app ),
	m_appName( "Media Downloader" ),
	m_ui( _init_ui( *this ) ),
	m_logger( *m_ui->plainTextEditLogger,this,s ),
	m_engines( m_logger,paths,s,utility::sequentialID() ),
	m_tabManager( s,t,m_engines,m_logger,*m_ui,*this,*this,m_appName,_debug( args ) ),
	m_settings( s ),
	m_showTrayIcon( s.showTrayIcon() )
{
	this->setTitle( m_appName ) ;

	qRegisterMetaType< utility::networkReply >() ;
	qRegisterMetaType< reportFinished >() ;

	this->window()->setFixedSize( this->window()->size() ) ;

	this->window()->setWindowIcon( m_trayIcon.icon() ) ;

	m_trayIcon.setContextMenu( [ this,&t ](){

		auto m = new QMenu( this ) ;

		auto ac = t.addAction( m,{ tr( "Quit" ),"Quit","Quit" },true ) ;

		connect( ac,&QAction::triggered,[ this ](){

			this->quitApp() ;
		} ) ;

		return m ;
	}() ) ;

	connect( &m_trayIcon,&QSystemTrayIcon::activated,[ this ]( QSystemTrayIcon::ActivationReason ){

		if( this->isVisible() ){

			this->hide() ;
		}else{
			this->show() ;
		}
	} ) ;

	if( m_showTrayIcon ){

		if( QSystemTrayIcon::isSystemTrayAvailable() ){

			m_trayIcon.show() ;
		}else{
			util::Timer( 1000,[ this ]( int counter ){

				if( QSystemTrayIcon::isSystemTrayAvailable() ){

					m_trayIcon.show() ;

					return true ;
				}else{
					if( counter == 5 ){

						/*
						 * We have waited for system tray to become
						 * available and we can wait no longer, display
						 * it and hope for the best.
						 */
						m_trayIcon.show() ;

						return true ;
					}else{
						return false ;
					}
				}
			} ) ;
		}

		m_trayIcon.show() ;
	}

	paths.confirmPaths( m_logger ) ;
}

void MainWindow::showTrayIcon( bool e )
{
	m_showTrayIcon = e ;

	if( e ){

		m_trayIcon.show() ;
	}else{
		m_trayIcon.hide() ;
	}
}

void MainWindow::retranslateUi()
{
	m_ui->retranslateUi( this ) ;
}

void MainWindow::setTitle( const QString& m )
{
	if( m.isEmpty() ){

		this->resetTitle() ;
	}else{
		this->window()->setWindowTitle( m ) ;
	}
}

void MainWindow::resetTitle()
{
	this->setTitle( m_appName ) ;
}

void MainWindow::Show()
{
	this->show() ;
}

void MainWindow::processEvent( const QByteArray& e )
{
	m_tabManager.gotEvent( e ) ;
}

void MainWindow::quitApp()
{
	m_settings.setTabNumber( m_ui->tabWidget->currentIndex() ) ;

	m_tabManager.exiting() ;

	QCoreApplication::quit() ;
}

void MainWindow::log( const QByteArray& e )
{
	m_logger.add( e,utility::sequentialID() ) ;
}

MainWindow::~MainWindow()
{
}

void MainWindow::closeEvent( QCloseEvent * e )
{
	e->ignore() ;

	this->hide() ;

	if( !m_showTrayIcon ){

		this->quitApp() ;
	}
}

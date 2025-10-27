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

#include <csignal>

MainWindow::MainWindow( QApplication& app,
			settings& s,
			translator& t,
			const engines::enginePaths& paths,
			const utility::cliArguments& args ) :
	m_trayIcon( s.getIcon( "media-downloader" ) ),
	m_qApp( app ),
	m_appName( "Media Downloader" ),
	m_ui( this ),
	m_logger( m_ui.plainTextEditLogger(),this,s ),
	m_engines( m_logger,paths,s,utility::sequentialID() ),
	m_printOutPut( args ),
	m_tabManager( s,t,m_engines,m_logger,m_ui.get(),*this,*this,m_appName,m_printOutPut ),
	m_settings( s ),
	m_showTrayIcon( s.showTrayIcon() ),
	m_shortcut( this )
{
	MainWindow::setUpSignals( this ) ;

	this->setTitle( m_appName ) ;

	qRegisterMetaType< utility::networkReply >() ;
	qRegisterMetaType< reportFinished >() ;

	m_settings.setMainWindowDimensions( this->window() ) ;

	this->window()->setWindowIcon( m_trayIcon.icon() ) ;

	m_trayIcon.setContextMenu( [ this,&t ](){

		auto m = new QMenu( this ) ;

		auto ac = t.addAction( m,{ tr( "Quit" ),"Quit","Quit" },true ) ;

		connect( ac,&QAction::triggered,[ this ](){

			this->quitApp() ;
		} ) ;

		return m ;
	}() ) ;

	auto qe = Qt::QueuedConnection ;

	connect( this,&MainWindow::processEventSignal,this,&MainWindow::processEventSlot,qe ) ;

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

void MainWindow::keyPressEvent( QKeyEvent * e )
{
	auto key = static_cast< Qt::Key >( e->key() ) ;

	if( e->modifiers() & Qt::CTRL ){

		if( key == Qt::Key_D ){

			m_tabManager.keyPressed( utility::mainWindowKeyCombo::CTRL_D ) ;

		}else if( key == Qt::Key_A ){

			m_tabManager.keyPressed( utility::mainWindowKeyCombo::CTRL_A ) ;
		}
	}else{
		if( key == Qt::Key_Enter || key == Qt::Key_Return ){

			m_tabManager.keyPressed( utility::mainWindowKeyCombo::ENTER ) ;
		}
	}

	QWidget::keyPressEvent( e ) ;
}

void MainWindow::retranslateUi()
{
	m_ui.retranslateUi( this ) ;
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

void MainWindow::processEvent( const QByteArray& m )
{
	emit this->processEventSignal( m ) ;
}

void MainWindow::processEventSlot( const QByteArray& e )
{
	m_tabManager.gotEvent( e ) ;
}

void MainWindow::quitApp()
{
	m_settings.setTabNumber( m_ui.currentIndex() ) ;

	m_tabManager.exiting() ;

	if( m_dataNotSaved ){

		m_dataNotSaved = false ;
		this->saveData() ;
	}

	QCoreApplication::quit() ;
}

void MainWindow::saveData()
{
	m_tabManager.batchDownloader().saveData() ;
	m_tabManager.playlistDownloader().saveData() ;
}

void MainWindow::notifyOnDownloadComplete( const QString& e )
{
	auto m = QSystemTrayIcon::Information ;
	auto s = m_settings.desktopNotificationTimeOut() ;

	m_trayIcon.showMessage( "Download Complete",e,m,s ) ;
}

void MainWindow::notifyOnAllDownloadComplete( const QString& e )
{
	auto m = QSystemTrayIcon::Information ;
	auto s = m_settings.desktopNotificationTimeOut() ;

	m_trayIcon.showMessage( "All Downloads Complete",e,m,s ) ;
}

MainWindow::~MainWindow()
{
	m_settings.saveMainWindowDimensions( this->window()->geometry() ) ;
}

MainWindow * MainWindow::m_mainWindow ;

void MainWindow::setUpSignals( MainWindow * m )
{
	m_mainWindow = m ;
	MainWindow::setUpSignal( SIGTERM,SIGSEGV,SIGINT,SIGABRT ) ;
}

void MainWindow::signalHandler( int )
{
	m_mainWindow->quitApp() ;
}

void MainWindow::setUpSignal( int sig )
{
	std::signal( sig,MainWindow::signalHandler ) ;
}

void MainWindow::closeEvent( QCloseEvent * )
{
	if( m_showTrayIcon ){

		this->hide() ;
	}else{
		this->quitApp() ;
	}
}

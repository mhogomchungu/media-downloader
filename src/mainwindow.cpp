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

MainWindow::MainWindow( settings& settings ) :
	m_ui( new Ui::MainWindow )
{
	m_ui->setupUi( this ) ;

	tabManager::instance().init( &settings,m_ui,this ) ;

	this->window()->setFixedSize( this->window()->size() ) ;

	QIcon icon = QIcon::fromTheme( "media-downloader",QIcon( ":media-downloader" ) ) ;

	this->window()->setWindowIcon( icon ) ;

	m_trayIcon.setIcon( icon ) ;

	auto m = new QMenu( this ) ;

	connect( m->addAction( "Quit" ),&QAction::triggered,[](){

		tabManager::instance().basicDownloader().appQuit() ;
	} ) ;

	m_trayIcon.setContextMenu( m ) ;

	m_trayIcon.show() ;
}

MainWindow::~MainWindow()
{
	delete m_ui ;
}

void MainWindow::closeEvent( QCloseEvent * e )
{
	e->ignore() ;

	this->hide() ;

	QCoreApplication::exit() ;
}

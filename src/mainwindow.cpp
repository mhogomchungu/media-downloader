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

MainWindow::MainWindow( QApplication& app,settings& s,translator& t ) :
	m_qApp( app ),
	m_ui( std::make_unique< Ui::MainWindow >() ),
	m_initUi( *m_ui,*this ),
	m_tabManager( s,t,*m_ui,*this,*this )
{
	this->window()->setFixedSize( this->window()->size() ) ;

	QIcon icon = QIcon::fromTheme( "media-downloader",QIcon( ":media-downloader" ) ) ;

	this->window()->setWindowIcon( icon ) ;

	if( s.showTrayIcon() ){

		m_trayIcon.setIcon( icon ) ;

		m_trayIcon.setContextMenu( [ this,&t ](){

			auto m = new QMenu( this ) ;

			auto ac = t.addAction( m,{ tr( "Quit" ),"Quit","Quit" },true ) ;

			connect( ac,&QAction::triggered,[ this ](){

				m_tabManager.basicDownloader().appQuit() ;
			} ) ;

			return m ;
		}() ) ;

		m_trayIcon.show() ;
	}
}

void MainWindow::retranslateUi()
{
	m_ui->retranslateUi( this ) ;
}

int MainWindow::exec()
{
	this->show() ;
	return m_qApp.exec() ;
}

MainWindow::~MainWindow()
{
}

void MainWindow::closeEvent( QCloseEvent * e )
{
	e->ignore() ;

	this->hide() ;

	m_tabManager.basicDownloader().appQuit() ;
}

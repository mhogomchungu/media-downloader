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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCloseEvent>
#include <QSystemTrayIcon>
#include <QString>
#include <QStringList>
#include <QMenu>

#include "settings.h"
#include "translator.h"
#include "tabmanager.h"
#include "context.hpp"

namespace Ui
{
	class MainWindow ;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT
public:
	MainWindow( settings&,translator& ) ;
	~MainWindow() ;
private:	
	QSystemTrayIcon m_trayIcon ;
	Ui::MainWindow * m_ui ;
	class unitUi{
	public:
		unitUi( Ui::MainWindow * m_ui,QMainWindow * mainWindow )
		{
			m_ui->setupUi( mainWindow ) ;
		}
	} m_initUi ;
	Context m_args ;
	tabManager m_tabManager ;
	void closeEvent( QCloseEvent * ) ;
};

#endif // MAINWINDOW_H

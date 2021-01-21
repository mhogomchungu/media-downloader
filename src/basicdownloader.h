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

#ifndef BASIC_DOWNLOADER_H
#define BASIC_DOWNLOADER_H

#include <QString>
#include <QStringList>
#include <QMenu>

#include "settings.h"
#include "utility.h"

namespace Ui {
class MainWindow;
}

class basicdownloader : public QObject
{
	Q_OBJECT
public:
	basicdownloader( settings& ) ;
	void init( Ui::MainWindow *,QWidget * ) ;
private:
	Ui::MainWindow * m_ui ;
	settings& m_settings ;
	QMenu * m_menu ;
	QWidget * m_mainWindow ;
	QStringList m_tmp ;
	void run( const QString& cmd,const QStringList& args ) ;
	void list() ;
	void download() ;
	void exit() ;
	void enableAll() ;
	void disableAll() ;
	QMenu * setMenu() ;
} ;

#endif

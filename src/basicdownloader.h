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
#include "context.hpp"

class tabManager ;

class basicdownloader : public QObject
{
	Q_OBJECT
public:
	basicdownloader( Context&,tabManager& ) ;
	void init_done() ;
	void download( const utility::args&,
		       const QString& urls,
		       bool = true ) ;
	void download( const utility::args&,
		       const QStringList& urls,
		       bool = true ) ;
	void enableAll() ;
	void disableAll() ;
	void appQuit() ;
	void resetMenu( const QStringList& = {} ) ;
	void setAsActive() ;
private:
	Context& m_args ;
	settings& m_settings ;
	Ui::MainWindow& m_ui ;
	tabManager& m_tabManager ;

	QString m_exe ;
	void run( const QString& cmd,const QStringList& args,bool list_requested ) ;
	void listRequested( const QStringList& ) ;
	void failedToFindExe( const QStringList& ) ;
	void list() ;
	void download() ;
	void exit() ;
} ;

#endif

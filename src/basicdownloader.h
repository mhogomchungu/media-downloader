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
	basicdownloader( Context& ) ;
	void init_done() ;
	void download( const engines::engine&,
		       const utility::args&,
		       const QString& urls,
		       bool = true ) ;
	void download( const engines::engine&,
		       const utility::args&,
		       const QStringList& urls,
		       bool = true ) ;
	void post( const QString& ) ;
	void enableQuit() ;
	void enableAll() ;
	void disableAll() ;
	void appQuit() ;
	void resetMenu( const QStringList& = {} ) ;
	void retranslateUi() ;
	void checkAndPrintInstalledVersion( const engines::engine&,const QStringList& = {} ) ;
	basicdownloader& setAsActive() ;
	const QStringList& currentVersionData() ;
private:
	size_t m_counter = 0 ;
	Context& m_ctx ;
	settings& m_settings ;
	Ui::MainWindow& m_ui ;
	tabManager& m_tabManager ;
	QStringList m_output ;

	void disableEngine( const engines::engine& ) ;
	void enableEngine( const engines::engine& ) ;

	void run( const engines::engine& engine,
		  const QStringList& args,
		  bool list_requested ) ;
	void listRequested( const QStringList& ) ;
	void list() ;
	void download() ;
	void exit() ;
	void printDefaultBkVersionInfo() ;
} ;

#endif

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

#ifndef ARGS_H
#define ARGS_H

#include "engines.h"

#include <QCoreApplication>

class translator ;
class QMainWindow ;
class tabManager ;
class settings ;
class MainWindow ;
class MainWindowUi ;
class Logger ;

class QWidget ;

namespace Ui
{
	class MainWindow ;
}

class Context{
public:
	Context( settings& s,
		 translator& t,
		 Ui::MainWindow& m,
		 QWidget& w,
		 MainWindow& mw,
		 Logger& l,
		 engines& e,
		 tabManager& tm ) :
		m_settings( s ),
		m_translator( t ),
		m_mainUi( m ),
		m_mainWidget( w ),
		m_mainWindow( mw ),
		m_logger( l ),
		m_engines( e ),
		m_tabManager( tm ),
		m_debug( QCoreApplication::arguments().contains( "--debug" ) )
	{
	}
	engines& Engines() const
	{
		return m_engines ;
	}
	settings& Settings() const
	{
		return m_settings ;
	}
	translator& Translator() const
	{
		 return m_translator ;
	}
	Ui::MainWindow& Ui() const
	{
		return m_mainUi ;
	}
	QWidget& mainWidget() const
	{
		return m_mainWidget ;
	}
	MainWindow& mainWindow() const
	{
		return m_mainWindow ;
	}
	tabManager& TabManager() const
	{
		return m_tabManager ;
	}
	Logger& logger() const
	{
		return m_logger ;
	}
	bool debug() const
	{
		return m_debug ;
	}
private:
	settings& m_settings ;
	translator& m_translator ;
	Ui::MainWindow& m_mainUi ;
	QWidget& m_mainWidget ;
	MainWindow& m_mainWindow ;
	Logger& m_logger ;
	engines& m_engines ;
	tabManager& m_tabManager ;
	bool m_debug ;
};

#endif

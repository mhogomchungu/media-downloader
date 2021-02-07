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

#include "settings.h"

#include <QWidget>

class translator ;
class QMainWindow ;
class tabManager ;

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
		 QMainWindow& mw ) :
		m_settings( s ),
		m_translator( t ),
		m_mainUi( m ),
		m_mainWidget( w ),
		m_mainWindow( mw )
	{
	}
	settings& Settings()
	{
		return m_settings ;
	}
	translator& Translator()
	{
		 return m_translator ;
	}
	Ui::MainWindow& Ui()
	{
		return m_mainUi ;
	}
	QWidget& mainWidget()
	{
		return m_mainWidget ;
	}
	QMainWindow& mainWindows()
	{
		return m_mainWindow ;
	}
private:
	settings& m_settings ;
	translator& m_translator ;
	Ui::MainWindow& m_mainUi ;
	QWidget& m_mainWidget ;
	QMainWindow& m_mainWindow ;
};

#endif

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

#ifndef TABMANAGER_H
#define TABMANAGER_H

#include "about.h"
#include "basicdownloader.h"
#include "batchdownloader.h"
#include "batchfiledownloader.h"
#include "configure.h"
#include "playlistdownloader.h"

class tabManager
{
public:
	static tabManager& instance()
	{
		static tabManager m ;
		return m ;
	}
	tabManager& init( settings * s,Ui::MainWindow * m,QWidget * w )
	{
		m_about.init( s,m,w ) ;
		m_configure.init( s,m,w ) ;
		m_basicdownloader.init( s,m,w ) ;
		m_batchdownloader.init( s,m,w ) ;
		m_batchfiledownloader.init( s,m,w ) ;
		m_playlistdownloader.init( s,m,w ) ;

		m_about.init_done() ;
		m_configure.init_done() ;
		m_basicdownloader.init_done() ;
		m_batchdownloader.init_done() ;
		m_batchfiledownloader.init_done() ;
		m_playlistdownloader.init_done() ;

		return *this ;
	}
	tabManager& enableAll()
	{
		m_about.enableAll() ;
		m_configure.enableAll() ;
		m_basicdownloader.enableAll() ;
		m_batchdownloader.enableAll() ;
		m_batchfiledownloader.enableAll() ;
		m_playlistdownloader.enableAll() ;

		return *this ;
	}
	tabManager& disableAll()
	{
		m_about.disableAll() ;
		m_configure.disableAll() ;
		m_basicdownloader.disableAll() ;
		m_batchdownloader.disableAll() ;
		m_batchfiledownloader.disableAll() ;
		m_playlistdownloader.disableAll() ;

		return *this ;
	}
	tabManager& resetMenu()
	{
		m_about.resetMenu() ;
		m_configure.resetMenu() ;
		m_basicdownloader.resetMenu() ;
		m_batchdownloader.resetMenu() ;
		m_batchfiledownloader.resetMenu() ;
		m_playlistdownloader.resetMenu() ;

		return *this ;
	}
	basicdownloader& basicDownloader()
	{
		return m_basicdownloader ;
	}
private:
	about m_about ;
	configure m_configure ;
	basicdownloader m_basicdownloader ;
	batchdownloader m_batchdownloader ;
	batchfiledownloader m_batchfiledownloader ;
	playlistdownloader m_playlistdownloader ;
} ;

#endif

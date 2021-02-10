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
#include "context.hpp"

class tabManager
{
public:
	tabManager( settings& s,
		    translator& t,
		    Ui::MainWindow& m,
		    QWidget& w,
		    MainWindow& mw ) :
		m_ctx( s,t,m,w,mw,*this ),
		m_about( m_ctx ),
		m_configure( m_ctx ),
		m_basicdownloader( m_ctx ),
		m_batchdownloader( m_ctx ),
		m_batchfiledownloader( m_ctx ),
		m_playlistdownloader( m_ctx )
	{
		m_about.init_done() ;
		m_configure.init_done() ;
		m_basicdownloader.init_done() ;
		m_batchdownloader.init_done() ;
		m_batchfiledownloader.init_done() ;
		m_playlistdownloader.init_done() ;
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
	tabManager& reTranslateUi()
	{		
		m_about.retranslateUi() ;
		m_configure.retranslateUi() ;
		m_basicdownloader.retranslateUi() ;
		m_batchdownloader.retranslateUi() ;
		m_batchfiledownloader.retranslateUi() ;
		m_playlistdownloader.retranslateUi() ;

		return *this ;
	}
	basicdownloader& basicDownloader()
	{
		return m_basicdownloader ;
	}
	configure& Configure()
	{
		return m_configure ;
	}
private:
	Context m_ctx ;
	about m_about ;
	configure m_configure ;
	basicdownloader m_basicdownloader ;
	batchdownloader m_batchdownloader ;
	batchfiledownloader m_batchfiledownloader ;
	playlistdownloader m_playlistdownloader ;
} ;

#endif

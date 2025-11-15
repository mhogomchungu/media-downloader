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
#include "configure.h"
#include "playlistdownloader.h"
#include "context.hpp"
#include "library.h"
#include "networkAccess.h"

#include <QClipboard>

Q_DECLARE_METATYPE( QClipboard::Mode )

class tabManager : public QObject
{
public:
	tabManager( settings& s,
		    translator& t,
		    engines& e,
		    Logger& l,
		    Ui::MainWindow& ui,
		    QWidget& w,
		    MainWindow& mw,
		    const QString& appName,
		    utility::printOutPut& ) ;
	void setDefaultEngines() ;
	tabManager& gotEvent( const QByteArray& e ) ;
	tabManager& enableAll() ;
	tabManager& disableAll() ;
	tabManager& resetMenu() ;
	tabManager& reTranslateUi() ;
	tabManager& exiting() ;
	void keyPressed( utility::mainWindowKeyCombo ) ;
	basicdownloader& basicDownloader()
	{
		return m_basicdownloader ;
	}
	batchdownloader& batchDownloader()
	{
		return m_batchdownloader ;
	}
	playlistdownloader& playlistDownloader()
	{
		return m_playlistdownloader ;
	}
	configure& Configure()
	{
		return m_configure ;
	}
	bool uiEnabled()
	{
		return m_uiEnabled ;
	}
	void textAlignmentChanged( Qt::LayoutDirection ) ;
	void init_done() ;
	void setProxy( const settings::proxySettings&,const settings::proxySettings::type& ) ;
private:
	void clipboardEvent( QClipboard::Mode ) ;
	void mainThreadClipboardHandler() ;
	void bgThreadClipboardHandler() ;
	QClipboard * m_clipboard ;
	bool m_firstTimeSettingProxy = true ;
	bool m_firstTime = true ;
	int m_currentTab ;
	bool m_uiEnabled = true ;
	Context m_ctx ;
	about m_about ;
	configure m_configure ;
	basicdownloader m_basicdownloader ;
	batchdownloader m_batchdownloader ;
	playlistdownloader m_playlistdownloader ;
	library m_library ;
} ;

#endif

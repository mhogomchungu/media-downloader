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
#ifndef PLAYLIST_DOWNLOADER
#define PLAYLIST_DOWNLOADER

#include "utility.h"

#include "settings.h"
#include "context.hpp"
#include "downloadmanager.h"
#include "tableWidget.h"

class tabManager ;

class playlistdownloader : public QObject
{
        Q_OBJECT
public:
	playlistdownloader( Context& ) ;
	void init_done() ;
	void enableAll() ;
	void disableAll() ;
	void resetMenu() ;
	void retranslateUi() ;
	void tabEntered() ;
	void tabExited() ;
	void updateEnginesList( const QStringList& ) ;
private:
	void download() ;	
	void download( const engines::engine&,downloadManager::index ) ;
	void download( const engines::engine& ) ;
	void download( const engines::engine&,int ) ;
	void getList() ;
	void clearScreen() ;

	Context& m_ctx ;
	settings& m_settings ;
	Ui::MainWindow& m_ui ;
	QWidget& m_mainWindow ;
	tabManager& m_tabManager ;
	tableWidget m_table ;

	bool m_showThumbnails ;
	bool m_running ;
	int m_networkRunning = 0 ;

	downloadManager m_ccmd ;

	utility::Terminator m_terminator ;

	struct opts
	{
		const Context& ctx ;
		bool debug ;
		bool listRequested ;
		int index ;
	} ;

	template< typename Functions >
	auto make_options( playlistdownloader::opts opts,Functions f )
	{
		return utility::options< playlistdownloader::opts,Functions >( std::move( opts ),std::move( f ) ) ;
	}
};

#endif

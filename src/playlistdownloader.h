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

class customOptions ;

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
	void gotEvent( const QByteArray& ) ;
	void updateEnginesList( const QStringList& ) ;
private:
	QString defaultEngineName() ;
	const engines::engine& defaultEngine() ;

	void download() ;	
	void download( const engines::engine&,downloadManager::index ) ;
	void download( const engines::engine& ) ;
	void download( const engines::engine&,int ) ;

	void getList() ;
	void clearScreen() ;
	bool enabled() ;
	Context& m_ctx ;
	settings& m_settings ;
	Ui::MainWindow& m_ui ;
	QWidget& m_mainWindow ;
	tabManager& m_tabManager ;
	tableWidget m_table ;

	bool m_gettingPlaylist = false ;
	bool m_showThumbnails ;
	int m_networkRunning = 0 ;

	downloadManager m_ccmd ;

	utility::Terminator m_terminator ;

	QPixmap m_defaultVideoThumbnailIcon ;

	class Monitor
	{
	public:
		struct Replace
		{
			bool replace ;
			int row ;
		};
		Monitor( tableWidget& t ) :
			m_table( t )
		{
		}
		void stop()
		{
			if( !m_doingNetworking ){

				m_table.runningStateItem( 0 ).setText( "blabla" ) ;
			}
		}
		Monitor::Replace replace() const
		{
			return { m_table.runningState( 0 ).isEmpty(),0 } ;
		}
		bool stillProcessing() const
		{
			return m_table.runningState( 0 ).isEmpty() ;
		}
		void doingNetworking()
		{
			m_doingNetworking = true ;
		}
		~Monitor()
		{
			if( m_table.rowCount() > 0 && m_table.url( 0 ).isEmpty() ){

				m_table.removeRow( 0 ) ;
			}
		}
	private:
		bool m_doingNetworking = false ;
		tableWidget& m_table ;
	};

	void parseJson( const customOptions&,
			const std::shared_ptr< playlistdownloader::Monitor >&,
			tableWidget& table,
			Logger::Data& data ) ;

	struct opts
	{
		const Context& ctx ;
		QString debug ;
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

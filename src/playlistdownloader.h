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
	void gotEvent( const QByteArray& ) ;
	void updateEnginesList( const QStringList& ) ;
private:
	QString defaultEngineName() ;
	const engines::engine& defaultEngine() ;

	void download() ;	
	void download( const engines::engine&,downloadManager::index ) ;
	void download( const engines::engine& ) ;
	void download( const engines::engine&,int ) ;

	void clearScreen() ;
	bool enabled() ;
	Context& m_ctx ;
	settings& m_settings ;
	Ui::MainWindow& m_ui ;
	QWidget& m_mainWindow ;
	tabManager& m_tabManager ;
	tableWidget m_table ;
	tableMiniWidget< int > m_subscriptionTable ;
	bool m_gettingPlaylist = false ;
	bool m_showThumbnails ;
	bool m_showTimer ;
	bool m_autoDownload ;
	bool m_stoppedOnExisting ;
	bool m_dataReceived ;

	int m_networkRunning = 0 ;

	downloadManager m_ccmd ;

	utility::Terminator m_terminator ;

	QPixmap m_defaultVideoThumbnailIcon ;

	class customOptions ;

	void parseJson( const playlistdownloader::customOptions&,
			bool,
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

	class archiveFileManager
	{
	public:
		void setPath( const QString& ) ;
		const QByteArray& data() const ;
		void clear() ;
	private:
		QString m_path ;
		QByteArray m_data ;
	};

	archiveFileManager m_archiveFileManager ;

	class subscription
	{
	public:
		subscription( const Context&,tableMiniWidget< int >&,QWidget& ) ;
		void add( const QString& uiName,const QString& url,const QString& Opts ) ;
		void remove( int ) ;
		void setVisible( bool ) ;
		const QString& archivePath() const ;
		struct entry
		{
			entry()
			{
			}
			entry( const QString& u ) : url( u )
			{
			}
			entry( QString u,QString l,QString o ) :
				uiName( std::move( u ) ),
				url( std::move( l ) ),
				getListOptions( std::move( o ) )
			{
			}
			QString uiName ;
			QString url ;
			QString getListOptions ;
		};
		std::vector< subscription::entry > entries() ;
	private:
		void save() ;
		QString m_path ;
		QString m_archivePath ;
		tableMiniWidget< int >& m_table ;
		QWidget& m_ui ;
		QJsonArray m_array ;
	};

	class listIterator
	{
	public:
		listIterator( std::vector< subscription::entry >&& s ) :
			m_list( std::move( s ) )
		{
		}
		listIterator( const QString& s )
		{
			m_list.emplace_back( s ) ;
		}
		bool hasNext() const
		{
			return m_list.size() > 1 ;
		}
		const QString& url() const
		{
			return m_list.back().url ;
		}
		const QString& uiName() const
		{
			return m_list.back().uiName ;
		}
		const QString& listOptions() const
		{
			return m_list.back().getListOptions ;
		}
		listIterator next() const
		{
			m_list.pop_back() ;
			return std::move( m_list ) ;
		}
	private:
		mutable std::vector< subscription::entry > m_list ;
	};

	void getList( playlistdownloader::listIterator ) ;
	void getList( customOptions&&,const engines::engine&,listIterator ) ;

	subscription m_subscription ;
};

#endif

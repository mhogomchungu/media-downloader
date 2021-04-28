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
#include "concurrentdownloadmanager.h"

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
private:
	void download() ;
	void download( const engines::engine& ) ;
	void download( const engines::engine&,int ) ;
	void getList() ;
	void clearScreen() ;

	Context& m_ctx ;
	settings& m_settings ;
	Ui::MainWindow& m_ui ;
	QWidget& m_mainWindow ;
	tabManager& m_tabManager ;
	bool m_running ;
	std::vector< int > m_playlistEntry ;
	class Index{
	public:
		Index( std::vector< int >& e,QTableWidget& t ) :
			m_entries( e ),m_table( t )
		{
		}
		int value() const
		{
			return m_entries[ m_index ] ;
		}
		int value( int s ) const
		{
			return m_entries[ static_cast< size_t >( s ) ] ;
		}
		int count() const
		{
			return static_cast< int >( m_entries.size() ) ;
		}
		void operator++( int )
		{
			m_index++ ;
		}
		bool notAtEnd() const
		{
			return m_index < m_entries.size() ;
		}
		QTableWidget& table() const
		{
			return m_table ;
		}
		void reset()
		{
			m_index = 0 ;
		}
	private:
		size_t m_index = 0 ;
		std::vector< int >& m_entries ;
		QTableWidget& m_table ;
	};

	class EnableAll
	{
	public:
		EnableAll( const Context ctx ) : m_tabManager( ctx.TabManager() )
		{
		}
		void operator()( bool e ) ;
	private:
		tabManager& m_tabManager ;
	} ;

	concurrentDownloadManager< Index,EnableAll > m_ccmd ;

	template< typename Function >
	class options
	{
	public:
		options( QPushButton& p,const Context& ctx,bool d,Function function ) :
			m_button( p ),
			m_ctx( ctx ),
			m_debug( d ),
			m_done( std::move( function ) )
		{
		}
		void done()
		{
			m_done() ;
		}
		options& tabManagerEnableAll( bool )
		{
			return *this ;
		}
		options& listRequested( const QList< QByteArray >& )
		{
			return *this ;
		}
		bool listRequested()
		{
			return false ;
		}
		options& enableCancel( bool e )
		{
			Q_UNUSED( e )
			//m_button.setEnabled( e ) ;

			return *this ;
		}
		bool debug()
		{
			return m_debug ;
		}
		QString downloadFolder() const
		{
			return m_ctx.Settings().downloadFolder() ;
		}
		const QProcessEnvironment& processEnvironment() const
		{
			return m_ctx.Engines().processEnvironment() ;
		}
	private:
		QPushButton& m_button ;
		const Context& m_ctx ;
		bool m_debug ;
		Function m_done ;
	} ;

	template< typename Function >
	auto make_options( QPushButton& p,const Context& ctx,bool d,Function function )
	{
		return playlistdownloader::options< Function >( p,ctx,d,std::move( function ) ) ;
	}
};

#endif

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
#ifndef BATCH_DOWNLOADER_URL_H
#define BATCH_DOWNLOADER_URL_H

#include <QString>
#include <QStringList>
#include <QMenu>

#include "settings.h"
#include "utility.h"
#include "context.hpp"
#include "downloadmanager.h"
#include "tableWidget.h"

class tabManager ;

class Items{
public:
	struct entry
	{
		entry( const QString& uiText,const QString& url ) :
			uiText( uiText ),url( url )
		{
		}
		QString uiText ;
		QString url ;
	} ;
	Items() = default ;
	Items( const QString& url )
	{
		m_entries.emplace_back( url,url ) ;
	}
	Items( const QString& uiText,const QString& url )
	{
		m_entries.emplace_back( uiText,url ) ;
	}
	void add( const QString& uiText,const QString& url )
	{
		m_entries.emplace_back( uiText,url ) ;
	}
	void add( const QString& url )
	{
		m_entries.emplace_back( url,url ) ;
	}
	const Items::entry& at( size_t s ) const
	{
		return m_entries[ s ] ;
	}
	const Items::entry& first() const
	{
		return m_entries[ 0 ] ;
	}
	size_t size() const
	{
		return m_entries.size() ;
	}
	bool hasOneEntry() const
	{
		return m_entries.size() == 1 ;
	}
	Items::entry takeFirst()
	{
		auto m = m_entries[ 0 ] ;

		m_entries.erase( m_entries.begin() ) ;

		return m ;
	}
	bool isEmpty() const
	{
		return m_entries.size() == 0 ;
	}
	auto begin()
	{
		return m_entries.begin() ;
	}
	auto end()
	{
		return m_entries.end() ;
	}
	auto begin() const
	{
		return m_entries.begin() ;
	}
	auto end() const
	{
		return m_entries.end() ;
	}
private:
	std::vector< entry > m_entries ;
};

class ItemEntry
{
public:
	ItemEntry() = default;
	ItemEntry( const engines::engine& engine,Items list ) :
		m_engine( &engine ),
		m_list( std::move( list ) )
	{
	}
	Items::entry next()
	{
		return m_list.takeFirst() ;
	}
	bool hasNext() const
	{
		return !m_list.isEmpty() ;
	}
	const engines::engine& engine()
	{
		return *m_engine ;
	}
private:
	const engines::engine * m_engine ;
	Items m_list ;
};

Q_DECLARE_METATYPE( ItemEntry )

class batchdownloader : public QObject
{
	Q_OBJECT
public:
	batchdownloader( const Context& ) ;
	void init_done() ;
	void enableAll() ;
	void disableAll() ;
	void resetMenu() ;
	void retranslateUi() ;
	void tabEntered() ;
	void tabExited() ;
	void gotEvent( const QByteArray& ) ;
	void updateEnginesList( const QStringList& ) ;
	void setThumbnailColumnSize( bool ) ;
	void showComments( const engines::engine&,const QString& ) ;
	void clipboardData( const QString& ) ;
private slots:
	void addItemUiSlot( ItemEntry ) ;
private:
	void saveComments( const QJsonArray&,const QString& filePath ) ;
	void showComments( const QByteArray& ) ;
	void showBDFrame( bool ) ;
	void getListFromFile( QMenu& ) ;
	QString defaultEngineName() ;
	const engines::engine& defaultEngine() ;
	void clearScreen() ;
	void showList( bool,const engines::engine&,const QString&,int ) ;
	void addToList( const QString&,bool autoDownload = false,bool showThumbnails = true ) ;
	void download( const engines::engine&,downloadManager::index ) ;
	void download( const engines::engine& ) ;
	void download( const engines::engine&,int ) ;
	void addItem( int,bool,const utility::MediaEntry& ) ;
	void addItemUi( int,bool,const utility::MediaEntry& ) ;
	void addItemUi( const QPixmap& pixmap,int,bool,const utility::MediaEntry& ) ;
	void showThumbnail( const engines::engine&,int,const QString& url,bool ) ;

	void showThumbnail( const engines::engine&,Items,bool = false,bool = false ) ;

	const Context& m_ctx ;
	settings& m_settings ;
	Ui::MainWindow& m_ui ;
	QWidget& m_mainWindow ;
	tabManager& m_tabManager ;
	bool m_showThumbnails ;
	tableWidget m_table ;
	tableMiniWidget< QJsonObject > m_tableWidgetBDList ;
	QString m_debug ;
	QString m_commentsFileName ;
	int m_networkRunning = false ;
	QStringList m_optionsList ;
	QLineEdit m_lineEdit ;
	QPixmap m_defaultVideoThumbnail ;

	utility::Terminator m_terminator ;

	downloadManager m_ccmd ;

	class BatchLogger
	{
	public:
		BatchLogger( Logger& l ) :
			m_logger( l ),
			m_id( utility::concurrentID() )
		{
		}
		void add( const QString& e )
		{
			this->add( e.toUtf8() ) ;
		}
		void add( const QByteArray& e )
		{
			m_logger.add( e,m_id ) ;
		}
		void clear()
		{
		}
		template< typename Function >
		void add( const Function& function )
		{
			m_logger.add( function,m_id ) ;
			function( m_lines,m_id,false ) ;
		}
		void logError( const QByteArray& data )
		{
			m_logger.logError( data,m_id ) ;
		}
		QByteArray data() const
		{
			return m_lines.toLine() ;
		}
	private:
		Logger::Data m_lines ;
		Logger& m_logger ;
		int m_id ;
	};

	class BatchLoggerWrapper
	{
	public:
		BatchLoggerWrapper( Logger& l ) :
			m_logger( std::make_shared< BatchLogger >( l ) )
		{
		}
		void add( const QByteArray& e )
		{
			m_logger->add( e ) ;
		}
		void clear()
		{
			m_logger->clear() ;
		}
		template< typename Function >
		void add( const Function& function )
		{
			m_logger->add( function ) ;
		}
		QByteArray data() const
		{
			return m_logger->data() ;
		}
		void logError( const QByteArray& data )
		{
			m_logger->logError( data ) ;
		}
	private:
		std::shared_ptr< BatchLogger > m_logger ;
	};

	struct opts
	{
		const Context& ctx ;
		QString debug ;
		bool listRequested ;
		int index ;
		BatchLoggerWrapper batchLogger ;
	} ;

	template< typename Functions >

	auto make_options( batchdownloader::opts opts,Functions f )
	{
		return utility::options< batchdownloader::opts,Functions >( std::move( opts ),std::move( f ) ) ;
	}
};

#endif

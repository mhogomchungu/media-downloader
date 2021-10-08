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

class ItemEntry
{
public:
	ItemEntry() = default;
	ItemEntry( const engines::engine& engine,const QStringList& list ) :
		m_engine( &engine ),
		m_list( list )
	{
	}
	QString next()
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
	QStringList m_list ;
};

Q_DECLARE_METATYPE( ItemEntry )

class batchdownloader : public QObject
{
	Q_OBJECT
public:
	template< typename Function >
	void setUpdefaultEngine( QComboBox& comboBox,
				 const QString& defaultEngine,
				 Function function )
	{
		for( int s = 0 ; s < comboBox.count() ; s++ ){

			if( comboBox.itemText( s ) == defaultEngine ){

				comboBox.setCurrentIndex( s ) ;

				return ;
			}
		}

		if( comboBox.count() > 0 ){

			comboBox.setCurrentIndex( 0 ) ;
			function( comboBox.itemText( 0 ) ) ;
		}
	}

	batchdownloader( const Context& ) ;
	void init_done() ;
	void enableAll() ;
	void disableAll() ;
	void resetMenu() ;
	void retranslateUi() ;
	void tabEntered() ;
	void tabExited() ;
	void updateEnginesList( const QStringList& ) ;
	void download( const engines::engine&,const QString& opts,const QStringList& ) ;
	void setThumbnailColumnSize( bool ) ;
private slots:
	void addItemUiSlot( ItemEntry ) ;
private:
	void clearScreen() ;
	void showList() ;
	void addToList( const QString& ) ;
	void download( const engines::engine&,downloadManager::index ) ;
	void download( const engines::engine& ) ;
	void download( const engines::engine&,int ) ;
	void addItem( int,bool,const utility::MediaEntry& ) ;
	void addItemUi( int,bool,const utility::MediaEntry& ) ;
	void addItemUi( const QPixmap& pixmap,int,bool,const utility::MediaEntry& ) ;
	void showThumbnail( const engines::engine&,int,const QString& url,bool ) ;

	const Context& m_ctx ;
	settings& m_settings ;
	Ui::MainWindow& m_ui ;
	QWidget& m_mainWindow ;
	tabManager& m_tabManager ;
	bool m_showThumbnails ;
	tableWidget m_table ;
	tableWidget m_tableWidgetBDList ;
	bool m_debug ;
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
			m_logger.add( e ) ;
			//m_lines.add( e ) ;
		}
		void clear()
		{
		}
		template< typename Function >
		void add( const Function& function )
		{
			m_logger.add( function,m_id ) ;
			function( m_lines,m_id ) ;
		}
		QByteArray data() const
		{
			return m_lines.toLine().toUtf8() ;
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
		void add( const QString& e )
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
	private:
		std::shared_ptr< BatchLogger > m_logger ;
	};

	struct opts
	{
		const Context& ctx ;
		bool debug ;
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

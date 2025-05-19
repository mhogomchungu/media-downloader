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
#include "downloadmanager.hpp"
#include "tableWidget.h"

class tabManager ;

class Items{
public:
	struct entry
	{
		entry( const QString& u,const QString& l ) : url( l ),uiText( u ),title( u )
		{
			obj.insert( "webpage_url",url ) ;
			obj.insert( "uiText",uiText ) ;
			obj.insert( "title",title ) ;
			obj.insert( "engineName","" ) ;
			obj.insert( "downloadOptions","" ) ;
			obj.insert( "downloadExtraOptions","" ) ;
			obj.insert( "uploader","" ) ;
			obj.insert( "runningState","" ) ;
			obj.insert( "uploadDate","" ) ;
			obj.insert( "duration","" ) ;
		}
		entry( QJsonObject o ) : obj( std::move( o ) ),
			url( obj.value( "webpage_url" ).toString() ),
			uiText( obj.value( "uiText" ).toString() + "\n" + url ),
			title( obj.value( "title" ).toString() ),
			engineName( obj.value( "engineName" ).toString() ),
			downloadOptions( obj.value( "downloadOptions" ).toString() ),
			downloadExtraOptions( obj.value( "downloadExtraOptions" ).toString() )
		{
			auto m = obj.value( "duration" ).toString() ;

			if( !m.isEmpty() ){

				auto mm = util::split( m,':',true ) ;

				if( mm.size() > 2 ){

					auto a = mm[ 0 ].toInt() ;
					auto b = mm[ 1 ].toInt() ;
					auto c = mm[ 2 ].toInt() ;

					auto d = a * 3600 + b * 60 + c ;

					obj.insert( "duration",d ) ;

				}else if( mm.size() == 2 ){

					auto a = mm[ 0 ].toInt() ;
					auto b = mm[ 1 ].toInt() ;

					auto d = a * 60 + b ;

					obj.insert( "duration",d ) ;

				}else if( mm.size() == 1 ){

					obj.insert( "duration",mm[ 0 ].toInt() ) ;
				}else{
					obj.insert( "duration",0 ) ;
				}
			}
		}
		QJsonDocument toJsonDoc() const
		{
			return QJsonDocument( obj ) ;
		}
		QJsonObject obj ;
		QString url ;
		QString uiText ;
		QString title ;
		QString engineName ;
		QString downloadOptions ;
		QString downloadExtraOptions ;
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
	Items move()
	{
		return std::move( *this ) ;
	}
	void add( QJsonObject obj )
	{
		m_entries.emplace_back( std::move( obj ) ) ;
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
	void keyPressed( utility::mainWindowKeyCombo ) ;
	void init_done() ;
	void enableAll() ;
	void disableAll() ;
	void resetMenu() ;
	void retranslateUi() ;
	void tabEntered() ;
	void tabExited() ;
	void exiting() ;
	void saveData() ;
	void gotEvent( const QJsonObject& ) ;
	void updateEnginesList( const QStringList& ) ;
	void setShowMetaData( bool ) ;
	void showComments( const engines::engine&,const QString& ) ;
	void clipboardData( const QString&,bool ) ;
	void textAlignmentChanged( Qt::LayoutDirection ) ;
signals:
	void reportFStatus( const reportFinished&,const std::vector< QByteArray >& ) ;
	void addItemUiSignal( ItemEntry ) ;
	void addTextToUiSignal( const QByteArray&,int ) ;
	void addClipboardSignal( QString,bool ) ;
private:
	void disableWhileDownloading() ;
	void addClipboardSlot( QString,bool ) ;
	void addTextToUi( const QByteArray&,int ) ;
	void getMetaData( const Items::entry&,const engines::engine&,bool ) ;
	void showHideControls() ;
	void networkData( const utility::networkReply& ) ;
	void addItemUiSlot( ItemEntry ) ;
	void reportFinishedStatus( const reportFinished&,const std::vector< QByteArray >& ) ;
	enum class listType{ COMMENTS,SUBTITLES,MEDIA_OPTIONS } ;
	void setDefaultEngineAndOptions( Items::entry& ) ;
	void showList( batchdownloader::listType,const engines::engine&,const QString&,int ) ;
	void setDownloadingOptions( int,tableWidget& ) ;
	void showBDFrame( batchdownloader::listType ) ;
	void saveComments( const QJsonArray&,const QString& filePath ) ;
	void showComments( const QByteArray& ) ;
	void showSubtitles( const QByteArray& ) ;
	void saveSubtitles() ;
	void sortComments() ;
	bool saveSubtitles( const QString& url,const QString& ext,const QString& title ) ;
	void normalizeFilePath( QString& ) ;
	void setVisibleWidgetOverMainTable( bool ) ;
	void renameFile( int ) ;
	void setTimeIntervals( int ) ;
	QString setSubtitleString( const QJsonObject&,const QString& ) ;
	void parseDataFromFile( Items&,const QByteArray& ) ;
	void parseItems( Items ) ;
	void parseDataFromObject( Items&,const QJsonObject&,const QJsonArray& ) ;
	void getListFromFile( QMenu& ) ;
	void getListFromFile( const QString&,bool ) ;
	QString defaultEngineName() ;
	const engines::engine& defaultEngine() ;
	bool downloadable( int ) ;
	void clearScreen() ;
	void showCustomContext() ;
	void addToList( const QString& ) ;
	void downloadAddItems( const engines::engine&,Items ) ;
	void download( const engines::engine& ) ;
	void downloadSingle( const engines::engine&,int ) ;
	void downloadRecursively( const engines::engine&,int ) ;
	void addItem( int,bool,const utility::MediaEntry& ) ;
	void addItemUi( int,bool,const utility::MediaEntry& ) ;
	void showThumbnail( const engines::engine&,int,const QString& url,bool ) ;
	int addItemUi( const QPixmap& pixmap,int,bool,const utility::MediaEntry& ) ;
	int addItemUi( const QPixmap& pixmap,
		       int index,
		       tableWidget& table,
		       Ui::MainWindow& ui,
		       const utility::MediaEntry& media ) ;
	void showThumbnail( const engines::engine&,
			   Items,
			   bool autoDownload = false,
			   bool showThumbnails= false ) ;
	struct networkCtx
	{
		utility::MediaEntry media ;
		int index ;
		networkCtx move()
		{
			return std::move( *this ) ;
		}
	} ;

	template< typename Event >
	void downloadEvent( Event event )
	{
		const engines::engine& engine = event.engine() ;

		int index = event.index() ;

		auto updater = [ this,index ]( const QByteArray& e ){

			emit this->addTextToUiSignal( e,index ) ;
		} ;

		auto error  = []( const QByteArray& ){} ;

		int id      = utility::concurrentID() ;

		auto& ll    = m_ctx.logger() ;

		auto logs   = m_settings.getLogsLimits() ;

		auto logger = make_loggerBatchDownloader( engine.filter( id ),ll,updater,error,id,logs ) ;

		m_table.setRunningState( downloadManager::finishedStatus::running(),index ) ;

		m_ctx.logger().setMaxProcessLog( m_table.rowCount() + 1 ) ;

		auto updateOpts = [ &engine ]( QStringList opts ){

			engine.updateLocalOptions( opts ) ;

			return opts ;
		} ;

		auto dopt = utility::setDownloadOptions( engine,m_table,index ) ;
		const auto& ent = m_table.entryAt( index ) ;

		downloadManager::download_exec( engine,
					       std::move( updateOpts ),
					       m_ui.lineEditBDUrlOptions->text(),
					       m_table.url( index ),
					       m_ctx,
					       { dopt,{ index,m_table.rowCount() },true,ent },
					       m_terminator.setUp(),
					       event.move(),
					       logger.move() ) ;
	}
	void networkResult( networkCtx,const utils::network::reply& ) ;
	const Context& m_ctx ;
	settings& m_settings ;
	Ui::MainWindow& m_ui ;
	QWidget& m_mainWindow ;
	tabManager& m_tabManager ;
	bool m_showMetaData ;
	int m_topDownloadingIndex = 0 ;
	int m_downloadingInstances = 0 ;
	tableWidget m_table ;
	tableMiniWidget< QJsonObject,5 > m_tableWidgetBDList ;
	QString m_commentsFileName ;
	QStringList m_optionsList ;
	QLineEdit m_lineEdit ;
	QPixmap m_defaultVideoThumbnail ;
	batchdownloader::listType m_listType ;
	utility::Terminator m_terminator ;

	class widgetOverMainTable
	{
	public:
		widgetOverMainTable()
		{
		}
		widgetOverMainTable( bool b,int r ) : m_row( r ),m_show( b )
		{
		}
		int row() const
		{
			return m_row ;
		}
		bool showRenameUi() const
		{
			return m_show ;
		}
	private:
		int m_row ;
		bool m_show ;
	} ;

	widgetOverMainTable m_widgetOverMainTable ;

	downloadManager m_ccmd ;

	QByteArray m_downloadingComments ;

	bool m_initDone = false ;
	bool m_startAutoDownload ;

	class tmpChangeOptions
	{
	public:
		tmpChangeOptions( batchdownloader& b ) :
			m_parent( b ),
			_autoStartDownload( m_parent.m_startAutoDownload ),
			_showMetaData( m_parent.m_showMetaData )
		{
		}
		tmpChangeOptions( batchdownloader& b,bool a,bool s ) :
			m_parent( b ),
			_autoStartDownload( m_parent.m_startAutoDownload ),
			_showMetaData( m_parent.m_showMetaData )
		{
			m_parent.m_startAutoDownload = a ;
			m_parent.m_showMetaData      = s ;
		}
		void set() const
		{
			//needs to be called to remove "unused" warning.
		}
		~tmpChangeOptions()
		{
			m_parent.m_startAutoDownload = _autoStartDownload ;
			m_parent.m_showMetaData      = _showMetaData ;
		}
	private:
		batchdownloader& m_parent ;
		bool _autoStartDownload ;
		bool _showMetaData ;
	} ;

	class BatchLogger
	{
	public:
		BatchLogger( Logger& l,const settings::LogsLimits& s ) :
			m_localLogger( false,s ),
			m_logger( l ),
			m_id( utility::concurrentID() )
		{
		}
		BatchLogger move()
		{
			return std::move( *this ) ;
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
		void registerDone()
		{
			m_logger.registerDone( m_id ) ;
		}
		template< typename Function >
		void add( const Function& function )
		{
			m_logger.add( function,m_id ) ;
			function( m_localLogger,m_id,false ) ;
		}
		void logError( const QByteArray& data )
		{
			m_logger.logError( data,m_id ) ;
		}
		QByteArray data() const
		{
			return m_localLogger.toLine() ;
		}
		const std::vector< QByteArray >& fileNames() const
		{
			return m_localLogger.fileNames() ;
		}
	private:
		Logger::Data m_localLogger ;
		Logger& m_logger ;
		int m_id ;
	};

	class subtitlesTimer
	{
	public:
		subtitlesTimer( tableMiniWidget< QJsonObject,5 >& table ) ;
		void start() ;
		void stop() ;
	private:
		engines::engine::baseEngine::preProcessing m_banner ;
		QTimer m_timer ;
		tableMiniWidget< QJsonObject,5 >& m_table ;
	} m_subtitlesTimer ;

	template< typename LogFilter >
	class BatchLoggerWrapper
	{
	public:
		BatchLoggerWrapper( Logger& l,const settings::LogsLimits& s,LogFilter f ) :
			m_logger( std::make_shared< BatchLogger >( l,s ) ),
			m_logFilter( std::move( f ) )
		{
		}
		BatchLoggerWrapper< LogFilter > move()
		{
			return std::move( *this ) ;
		}
		void add( const QByteArray& e )
		{
			m_logger->add( e ) ;
		}
		void clear()
		{
			m_logger->clear() ;
		}
		void registerDone()
		{
			m_logger->registerDone() ;
		}
		template< typename Function >
		void add( const Function& function )
		{
			m_logger->add( function ) ;
		}
		QByteArray data() const
		{
			auto data = m_logger->data() ;

			data.replace( "[media-downloader] Download Completed Successfully","" ) ;

			return data ;
		}
		const std::vector< QByteArray >& fileNames() const
		{
			return m_logger->fileNames() ;
		}
		void logError( const QByteArray& data )
		{
			if( m_logFilter( data ) ){

				m_logger->logError( data ) ;
			}
		}
	private:
		std::shared_ptr< BatchLogger > m_logger ;
		LogFilter m_logFilter ;
	} ;

	class defaultLogger
	{
	public:
		defaultLogger()
		{
		}
		defaultLogger move()
		{
			return std::move( *this ) ;
		}
		bool operator()( const QByteArray& )
		{
			return true ;
		}
	} ;	
};

#endif

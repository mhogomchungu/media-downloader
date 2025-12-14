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

#ifndef LOGGER_H
#define LOGGER_H

#include <QPlainTextEdit>
#include <QString>
#include <QStringList>
#include <QTableWidgetItem>
#include <QDebug>
#include <QTime>

#include "logwindow.h"
#include "util.hpp"

#include <vector>

class Logger
{
public:
	class locale
	{
	public:
		QString formattedDataSize( qint64 ) const ;
		static QString secondsToString( int s )
		{
			if( s < 3600 ){

				return QTime( 0,0,0,0 ).addSecs( s ).toString( "mm:ss" ) ;
			}else{
				return QTime( 0,0,0,0 ).addSecs( s ).toString( "hh:mm:ss" ) ;
			}
		}
	private:
		QLocale m_locale ;
	};

	class Data
	{
	private:
		class processOutput
		{
		public:
			class outputEntry
			{
			public:
				outputEntry( QByteArray text,bool p = false ) :
					m_text( std::move( text ) ),
					m_progressLine( p )
				{
				}
				const QByteArray& text() const
				{
					return m_text ;
				}
				bool progressLine() const
				{
					return m_progressLine ;
				}
				void replace( const QByteArray& text )
				{
					m_progressLine = true ;
					m_text = text ;
				}
			private:
				QByteArray m_text ;
				bool m_progressLine ;
			} ;
			processOutput( int processId,QByteArray d ) : m_processId( processId )
			{
				m_data.emplace_back( std::move( d ) ) ;
			}
			int processId() const
			{
				return m_processId ;
			}
			struct IdLessThanZero
			{
				bool operator()( int id ) const
				{
					return id < 0 ;
				}
			} ;
			bool operator==( const IdLessThanZero& id ) const
			{
				return id( m_processId ) ;
			}
			const std::vector< Logger::Data::processOutput::outputEntry >& entries() const
			{
				return m_data ;
			}
			std::vector< Logger::Data::processOutput::outputEntry >& entries()
			{
				return m_data ;
			}
			bool doneDownloading() const
			{
				return m_doneDownloading ;
			}
			void setDoneDownloading()
			{
				m_doneDownloading = true ;
			}
			void setProcessAsFinished()
			{
				m_processFinished = true ;
			}
			bool processFinished() const
			{
				return m_processFinished ;
			}
		private:
			bool m_processFinished = false ;
			bool m_doneDownloading = false ;
			int m_processId ;
			std::vector< Logger::Data::processOutput::outputEntry > m_data ;
		};
	public:
		template< typename LogsLimits >
		Data( bool s,const LogsLimits& l ) :
			m_mainLogger( s ),
			m_maxLogEntries( l.maxLogEntries() ),
			m_longEntriesToRemove( l.removeEntriesCount() )
		{
		}
		bool mainLogger() const
		{
			return m_mainLogger ;
		}
		bool isEmpty() const
		{
			return m_processOutputs.empty() ;
		}
		bool isNotEmpty() const
		{
			return !this->isEmpty() ;
		}
		bool registerDone( int ) ;
		void add( int id,QByteArray d )
		{
			for( auto& it : m_processOutputs ){

				if( it.processId() == id ){

					it.entries().emplace_back( std::move( d ) ) ;

					return ;
				}
			}

			m_processOutputs.emplace_back( id,std::move( d ) ) ;
		}
		size_t size() const
		{
			return m_processOutputs.size() ;
		}
		template< typename Function >
		void forEach( Function function ) const
		{
			for( const auto& it : m_processOutputs ){

				auto id = it.processId() ;

				for( const auto& xt : it.entries() ){

					if( function( id,xt.text() ) ){

						return ;
					}
				}
			}
		}
		template< typename Function >
		void reverseForEach( Function function ) const
		{
			for( auto it = m_processOutputs.rbegin() ; it != m_processOutputs.rend() ; it++ ){

				auto id = it->processId() ;

				const auto& e = it->entries() ;

				for( auto xt = e.rbegin() ; xt != e.rend() ; xt++ ){

					if( function( id,xt->text() ) ){

						return ;
					}
				}
			}
		}
		void clear()
		{
			m_processOutputs.clear() ;
		}
		void clear( int id )
		{
			for( auto it = m_processOutputs.begin() ; it != m_processOutputs.end() ; it++ ){

				if( it->processId() == id ){

					it->entries().clear() ;
					break ;
				}
			}
		}
		class QByteArrayList
		{
		public:
			QByteArrayList()
			{
			}
			auto begin() const
			{
				return m_array.begin() ;
			}
			auto end() const
			{
				return m_array.end() ;
			}
			auto rbegin() const
			{
				return m_array.rbegin() ;
			}
			auto rend() const
			{
				return m_array.rend() ;
			}
			void append( const QByteArray& s )
			{
				m_array.emplace_back( s ) ;
			}
			const QByteArray& last() const
			{
				return *m_array.rbegin() ;
			}
			size_t size() const
			{
				return m_array.size() ;
			}
			const QByteArray& operator[]( size_t m ) const
			{
				return m_array[ m ] ;
			}
			template< typename Function,
				  typename std::enable_if< std::is_void< util::types::result_of< Function,QByteArray > >::value,int >::type = 0 >
			void forEach( Function function ) const
			{
				for( const auto& it : m_array ){

					function( it ) ;
				}
			}
			template< typename Function,
				  typename std::enable_if< std::is_same< util::types::result_of< Function,QByteArray >,bool >::value,int >::type = 0 >
			void forEach( Function function ) const
			{
				for( const auto& it : m_array ){

					if( function( it ) ){

						break ;
					}
				}
			}
		private:
			class ByteArray
			{
			public:
				ByteArray( const QByteArray& s ) : m_array( &s )
				{
				}
				operator const QByteArray&() const
				{
					return *m_array ;
				}
				operator QString() const
				{
					return *m_array ;
				}
			private:
				const QByteArray * m_array ;
			} ;

			std::vector< ByteArray > m_array ;
		};

		QByteArrayList toStringList() const
		{
			QByteArrayList s ;

			for( const auto& it : m_processOutputs ){

				for( const auto& xt : it.entries() ){

					s.append( xt.text() ) ;
				}
			}

			return s ;
		}
		const QByteArray& lastText() const
		{
			return m_processOutputs.rbegin()->entries().rbegin()->text() ;
		}
		bool lastLineIsProgressLine() const
		{
			return m_processOutputs.rbegin()->entries().rbegin()->progressLine() ;
		}
		QByteArray debugOutPut() const ;
		QByteArray join( const QByteArray& joiner ) const ;
		QByteArray toLine() const
		{
			return this->join( "" ) ;
		}
		QByteArray toLines() const
		{
			return this->join( "\n" ) ;
		}
		QByteArray join( const QByteArray& joiner,int id ) const ;
		QByteArray toLine( int id ) const
		{
			return this->join( "",id ) ;
		}
		QByteArray toLines( int id ) const
		{
			return this->join( "\n",id ) ;
		}
		void removeExtraLogs() ;
		bool removeFirstFinished() ;
		class ProcessData
		{
		public:
			ProcessData()
			{
			}
			ProcessData( std::vector< Logger::Data::processOutput::outputEntry >& e ) :
				m_entries( &e )
			{
			}
			void replaceLast( const QByteArray& e )
			{
				m_entries->rbegin()->replace( e ) ;
			}
			void removeLast()
			{
				m_entries->pop_back() ;
			}
			QByteArray takeLast()
			{
				auto m = this->lastText() ;
				this->removeLast() ;
				return m ;
			}
			size_t size() const
			{
				return m_entries->size() ;
			}
			operator bool() const
			{
				return m_entries != nullptr ;
			}
			const QByteArray& lastText() const
			{
				return m_entries->rbegin()->text() ;
			}
		private:
			std::vector< Logger::Data::processOutput::outputEntry > * m_entries = nullptr ;
		};
		Logger::Data::ProcessData getData( int id )
		{
			for( auto it = m_processOutputs.rbegin() ; it != m_processOutputs.rend() ; it++ ){

				if( it->processId() == id ){

					return it->entries() ;
				}
			}

			return {} ;
		}
		bool doneDownloading( int id ) const
		{
			for( const auto& it : m_processOutputs ){

				if( it.processId() == id ){

					return it.doneDownloading() ;
				}
			}

			return false ;
		}
		bool doneDownloading() const
		{
			return m_processOutputs.rbegin()->doneDownloading() ;
		}
		template< typename Filter >
		void replaceOrAdd( const Filter& filter )
		{
			_replaceOrAdd( filter ) ;
		}
		void add( const QByteArray& text,int id )
		{
			class Filter
			{
			public:
				Filter( int id,const QByteArray& arr ) :
					m_id( id ),m_text( arr )
				{
				}
				int id() const
				{
					return m_id ;
				}
				const QByteArray& text() const
				{
					return m_text ;
				}
				bool skip( const QByteArray& ) const
				{
					return false ;
				}
				bool replace( const QByteArray& ) const
				{
					return false ;
				}
			private:
				int m_id ;
				const QByteArray& m_text ;
			} ;

			_replaceOrAdd( Filter( id,text ) ) ;
		}
		class YtDlpData
		{
		public:
			const QByteArray& filePath() const
			{
				return m_filePath ;
			}
			void setFilePath( const QByteArray& m )
			{
				if( !m.isEmpty() && m != m_filePath ){

					m_filePath = m ;
				}
			}
		private:
			QByteArray m_filePath ;
		};

		YtDlpData& ytDlpData()
		{
			return m_ytDlpData ;
		}
		const YtDlpData& ytDlpData() const
		{
			return m_ytDlpData ;
		}
		class SvtData
		{
		public:
			SvtData()
			{
			}
			qint64 size() const
			{
				return m_size ;
			}
			void addToSize( qint64 s )
			{
				m_size += s ;
			}
			void reset()
			{
				m_size = 0 ;
			}
			const QByteArray& fileName() const
			{
				return m_fileName ;
			}
			void setFileName( const QByteArray& m )
			{
				if( !m.isEmpty() && m != m_fileName ){

					m_fileName = m ;
				}
			}
		private:
			qint64 m_size = 0 ;
			QByteArray m_fileName ;
		};

		SvtData& svtData()
		{
			return m_svtData ;
		}
		const SvtData& svtData() const
		{
			return m_svtData ;
		}
		void addFileName( const QByteArray& e )
		{
			for( const auto& it : m_fileNames ){

				if( it == e ){

					return ;
				}
			}

			m_fileNames.emplace_back( e ) ;
		}
		const std::vector< QByteArray >& fileNames() const
		{
			return m_fileNames ;
		}
	private:
		void manageLogSize( std::vector< Logger::Data::processOutput::outputEntry >& ) ;
		bool doneDownloadingText( const QByteArray& data ) ;
		template< typename Filter >
		void _replaceOrAdd( const Filter& filter )
		{
			for( auto it = m_processOutputs.rbegin() ; it != m_processOutputs.rend() ; it++ ){

				if( it->processId() == filter.id() ){

					if( this->doneDownloadingText( filter.text() ) ){

						it->setDoneDownloading() ;
					}

					auto& ee = it->entries() ;

					if( ee.empty() ){

						ee.emplace_back( filter.text() ) ;
					}else{
						auto iter = ee.rbegin() ;

						const auto& s = iter->text() ;

						if( filter.replace( s ) ){

							iter->replace( filter.text() ) ;
						}else{
							this->manageLogSize( ee ) ;

							if( !filter.skip( filter.text() ) ){

								ee.emplace_back( filter.text() ) ;
							}
						}
					}

					return ;
				}
			}

			if( !filter.skip( filter.text() ) ){

				m_processOutputs.emplace_back( filter.id(),filter.text() ) ;
			}
		}
		std::list< Logger::Data::processOutput > m_processOutputs ;
		bool m_mainLogger ;
		YtDlpData m_ytDlpData ;
		SvtData m_svtData ;
		std::vector< QByteArray > m_fileNames ;
		size_t m_maxLogEntries ;
		size_t m_longEntriesToRemove ;
	} ;

	Logger( QPlainTextEdit&,QWidget * parent,settings& ) ;
	void add( const QString& s,int id )
	{
		this->add( s.toUtf8(),id ) ;
	}
	void registerDone( int ) ;
	void add( const QByteArray&,int id ) ;
	void clear() ;
	template< typename Function >
	void add( const Function& function,int id )
	{
		function( m_processOutPuts,id,true ) ;

		this->update() ;
	}
	void logError( const QByteArray& data,int id )
	{
		class Filter
		{
		public:
			Filter( int id,const QByteArray& data ) :
				m_id( id ),m_text( "[media-downloader][std error] " + data )
			{
			}
			int id() const
			{
				return m_id ;
			}
			const QByteArray& text() const
			{
				return m_text ;
			}
			bool skip( const QByteArray& ) const
			{
				return false ;
			}
			bool replace( const QByteArray& ) const
			{
				return false ;
			}
		private:
			int m_id ;
			QByteArray m_text ;
		} ;

		m_processOutPuts.replaceOrAdd( Filter( id,data ) ) ;

		this->update() ;
	}
	void setMaxProcessLog( size_t s )
	{
		this->setMaxProcessLog( static_cast< int >( s ) ) ;
	}
	void setMaxProcessLog( int s ) ;
	void showLogWindow( int ) ;
	void showAllLogs() ;
	void reTranslateLogWindow() ;
	void updateView( bool e ) ;
	const std::vector< QByteArray >& fileNames()
	{
		return m_processOutPuts.fileNames() ;
	}
	Logger( const Logger& ) = delete ;
	Logger& operator=( const Logger& ) = delete ;
	Logger( Logger&& ) = delete ;
	Logger& operator=( Logger&& ) = delete ;
private:	
	void update() ;
	logWindow m_logWindow ;
	QPlainTextEdit& m_textEdit ;
	Logger::Data m_processOutPuts ;
	bool m_updateView = false ;
	settings& m_settings ;
	int m_maxProcessLog ;
	int m_id = -1 ;

	class meaw : public QObject
	{
	public:
		meaw( Logger& p ) : m_parent( p )
		{
		}
		bool eventFilter( QObject *,QEvent * ) override ;
	private:
		Logger& m_parent ;
	} ;

	meaw m_qobj ;
} ;

class LoggerWrapper
{
public:
	LoggerWrapper() = delete ;
	LoggerWrapper( Logger& logger,int id ) :
		m_logger( &logger ),
		m_id( id )
	{
	}
	void add( const QString& e )
	{
		this->add( e.toUtf8() ) ;
	}
	void add( const QByteArray& e )
	{
		m_logger->add( e,m_id ) ;
	}
	void logError( const QByteArray& data )
	{
		m_logger->logError( data,m_id ) ;
	}
	void registerDone()
	{
		m_logger->registerDone( m_id ) ;
	}
	void clear()
	{
		m_logger->clear() ;
	}
	template< typename Function >
	void add( const Function& function )
	{
		m_logger->add( function,m_id ) ;
	}
	LoggerWrapper move()
	{
		return std::move( *this ) ;
	}
	const std::vector< QByteArray >& fileNames()
	{
		return m_logger->fileNames() ;
	}
private:
	Logger * m_logger ;
	int m_id ;
};

template< typename F,typename U,typename L >
class LoggerBasicDownloader
{
public:
	LoggerBasicDownloader() = delete ;
	LoggerBasicDownloader( F function,U ff,Logger& logger,int id,const L& logs ) :
		m_function( std::move( function ) ),
		m_functionUpdate( std::move( ff ) ),
		m_logger( logger ),
		m_localLogger( false,logs ),
		m_id( id )
	{
	}
	void add( const QString& e )
	{
		this->add( e.toUtf8() ) ;
	}
	void add( const QByteArray& e )
	{
		m_logger.add( e,m_id ) ;

		if( e.startsWith( "[media-downloader]" ) ){

			m_localLogger.add( e,m_id ) ;
		}else{
			m_localLogger.add( "[media-downloader] " + e,m_id ) ;
		}

		this->update() ;
	}
	void logError( const QByteArray& data )
	{
		m_logger.logError( data,m_id ) ;
	}
	void registerDone()
	{
		m_logger.registerDone( m_id ) ;
	}
	void clear()
	{
		m_logger.clear() ;
		m_localLogger.clear() ;
	}
	template< typename Function >
	void add( const Function& function )
	{
		m_logger.add( function,m_id ) ;
		function( m_localLogger,m_id,false ) ;
		this->update() ;
	}
	LoggerBasicDownloader move()
	{
		return std::move( *this ) ;
	}
	const std::vector< QByteArray >& fileNames()
	{
		return m_localLogger.fileNames() ;
	}
private:
	void update()
	{
		if( m_localLogger.isNotEmpty() ){

			m_functionUpdate( m_function( m_localLogger ) ) ;
		}
	}
	F m_function ;
	U m_functionUpdate ;
	Logger& m_logger ;
	Logger::Data m_localLogger ;
	int m_id ;
} ;

template< typename F,typename U,typename L >
auto make_loggerBasicDownloader( F function,Logger& logger,U fu,int id,const L& logs )
{
	using lbd = LoggerBasicDownloader< F,U,L > ;

	return lbd( std::move( function ),std::move( fu ),logger,id,logs ) ;
}

template< typename F,typename U,typename E,typename L >
class loggerBatchDownloader
{
public:
	loggerBatchDownloader( F function,Logger& logger,U ff,E err,int id,const L& logs ) :
		m_function( std::move( function ) ),
		m_functionUpdate( std::move( ff ) ),
		m_error( std::move( err ) ),
		m_logger( logger ),
		m_localLogger( false,logs ),
		m_id( id )
	{
	}
	loggerBatchDownloader< F,U,E,L > move()
	{
		return std::move( *this ) ;
	}
	void add( const QString& e )
	{
		this->add( e.toUtf8() ) ;
	}
	void add( const QByteArray& s )
	{
		m_logger.add( s,m_id ) ;

		if( s.startsWith( "[media-downloader]" ) ){

			m_localLogger.add( s,m_id ) ;
		}else{
			m_localLogger.add( "[media-downloader] " + s,m_id ) ;
		}

		this->update() ;
	}
	void clear()
	{
		m_functionUpdate( "" ) ;
		m_localLogger.clear() ;
	}
	template< typename G >
	void add( const G& function )
	{
		m_logger.add( function,m_id ) ;
		function( m_localLogger,m_id,false ) ;
		this->update() ;
	}
	void registerDone()
	{
		m_logger.registerDone( m_id ) ;
	}
	void logError( const QByteArray& data )
	{
		m_error( data ) ;
		m_logger.logError( data,m_id ) ;
	}
	const std::vector< QByteArray >& fileNames()
	{
		return m_localLogger.fileNames() ;
	}
private:
	void update()
	{
		if( m_localLogger.isNotEmpty() ){

			m_functionUpdate( m_function( m_localLogger ) ) ;
		}
	}
	F m_function ;
	U m_functionUpdate ;
	E m_error ;
	Logger& m_logger ;
	Logger::Data m_localLogger ;
	int m_id ;
} ;

template< typename F,typename U,typename E,typename L >
auto make_loggerBatchDownloader( F function,Logger& logger,U fu,E err,int id,const L& logs )
{
	using lbd = loggerBatchDownloader< F,U,E,L > ;

	return lbd( std::move( function ),logger,std::move( fu ),std::move( err ),id,logs ) ;
}

template< typename AddToTable,typename TableWidget,typename Error,typename L >
class loggerPlaylistDownloader
{
public:
	loggerPlaylistDownloader( TableWidget& t,
				  Logger& logger,
				  int id,
				  AddToTable add,
				  Error error,
				  const L& logs ) :
		m_table( t ),
		m_logger( logger ),
		m_localLogger( false,logs ),
		m_id( id ),
		m_addToTable( std::move( add ) ),
		m_error( std::move( error ) )
	{
	}
	loggerPlaylistDownloader< AddToTable,TableWidget,Error,L > move()
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
		auto s = m_table.rowCount() ;

		for( int i = 0 ; i < s ; i++ ){

			m_table.removeRow( 0 ) ;
		}

		m_localLogger.clear() ;
	}
	template< typename Function >
	void add( const Function& function )
	{
		m_logger.add( function,m_id ) ;
		function( m_localLogger,m_id,false ) ;
		m_addToTable( m_localLogger ) ;
	}
	void registerDone()
	{
		m_logger.registerDone( m_id ) ;
	}
	void logError( const QByteArray& data )
	{
		if( m_error( data ) ){

			m_logger.logError( data,m_id ) ;
		}
	}
	const std::vector< QByteArray >& fileNames()
	{
		return m_localLogger.fileNames() ;
	}
private:
	TableWidget& m_table ;
	Logger& m_logger ;
	Logger::Data m_localLogger ;
	int m_id ;
	AddToTable m_addToTable ;
	Error m_error ;
};

template< typename AddToTable,typename TableWidget,typename Error,typename L >
auto make_loggerPlaylistDownloader( TableWidget& t,
				    Logger& logger,
				    int id,
				    AddToTable add,
				    Error err,
				    const L& logs )
{
	using lpd = loggerPlaylistDownloader< AddToTable,TableWidget,Error,L > ;

	return lpd( t,logger,id,std::move( add ),std::move( err ),logs ) ;
}
#endif

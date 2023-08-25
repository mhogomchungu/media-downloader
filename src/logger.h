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
		Data( bool s ) : m_mainLogger( s )
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
		QByteArray toString() const
		{
			if( this->isNotEmpty() ){

				QByteArray m ;

				for( const auto& it : m_processOutputs ){

					for( const auto& xt : it.entries() ){

						m += xt.text() + "\n" ;
					}
				}

				m.truncate( m.size() - 1 ) ;

				return m ;
			}else{
				return {} ;
			}
		}
		QByteArray toLine() const
		{
			if( this->isNotEmpty() ){

				QByteArray m ;

				for( const auto& it : m_processOutputs ){

					for( const auto& xt : it.entries() ){

						m += xt.text() ;
					}
				}

				return m ;
			}else{
				return {} ;
			}
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
		template< typename Function >
		void replaceOrAdd( const QByteArray& text,int id,const Function& function )
		{
			_replaceOrAdd( text,id,function ) ;
		}
		void add( const QByteArray& text,int id )
		{
			auto _false = []( const QByteArray& ){ return false ; } ;

			_replaceOrAdd( text,id,_false ) ;
		}
		class YtDlpData
		{
		public:
			const QByteArray& filePath() const
			{
				return m_filePath ;
			}
			void setFilePath( const QString& m )
			{
				if( !m.isEmpty() && m != m_filePath ){

					m_filePath = m.toUtf8() ;
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
			void size( qint64 s )
			{
				m_size += s ;
			}
			const QByteArray& fileName() const
			{
				return m_fileName ;
			}
			void setFileName( const QString& m )
			{
				if( !m.isEmpty() && m != m_fileName ){

					m_fileName = m.toUtf8() ;
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
	private:
		bool doneDownloadingText( const QByteArray& data ) ;
		template< typename Function >
		void _replaceOrAdd( const QByteArray& text,int id,const Function& function )
		{
			for( auto it = m_processOutputs.rbegin() ; it != m_processOutputs.rend() ; it++ ){

				if( it->processId() == id ){

					if( this->doneDownloadingText( text ) ){

						it->setDoneDownloading() ;
					}

					auto& ee = it->entries() ;

					if( ee.empty() ){

						ee.emplace_back( text ) ;
					}else{
						auto iter = ee.rbegin() ;

						const auto& s = iter->text() ;

						if( function( s ) ){

							iter->replace( text ) ;
						}else{
							ee.emplace_back( text ) ;
						}
					}

					return ;
				}
			}

			m_processOutputs.emplace_back( id,text ) ;
		}
		std::list< Logger::Data::processOutput > m_processOutputs ;
		bool m_mainLogger ;
		YtDlpData m_ytDlpData ;
		SvtData m_svtData ;
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
		auto function = []( const QByteArray& ){ return false ; } ;

		m_processOutPuts.replaceOrAdd( "[media-downloader][std error] " + data,id,function ) ;

		this->update() ;
	}
	void setMaxProcessLog( size_t s )
	{
		this->setMaxProcessLog( static_cast< int >( s ) ) ;
	}
	void setMaxProcessLog( int s ) ;
	void showLogWindow() ;
	void reTranslateLogWindow() ;
	void updateView( bool e ) ;
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
private:
	Logger * m_logger ;
	int m_id ;
};

template< typename F,typename U,typename E >
class loggerBatchDownloader
{
public:
	loggerBatchDownloader( F function,Logger& logger,U ff,E err,int id ) :
		m_function( std::move( function ) ),
		m_functionUpdate( std::move( ff ) ),
		m_error( std::move( err ) ),
		m_logger( logger ),
		m_localLogger( false ),
		m_id( id )
	{
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

template< typename F,typename U,typename E >
auto make_loggerBatchDownloader( F function,Logger& logger,U fu,E err,int id )
{
	using lbd = loggerBatchDownloader< F,U,E > ;

	return lbd( std::move( function ),logger,std::move( fu ),std::move( err ),id ) ;
}

template< typename AddToTable,typename TableWidget,typename Error >
class loggerPlaylistDownloader
{
public:
	loggerPlaylistDownloader( TableWidget& t,Logger& logger,int id,AddToTable add,Error error ) :
		m_table( t ),
		m_logger( logger ),
		m_localLogger( false ),
		m_id( id ),
		m_addToTable( std::move( add ) ),
		m_error( std::move( error ) )
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
		m_addToTable( m_table,m_localLogger ) ;
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
private:
	TableWidget& m_table ;
	Logger& m_logger ;
	Logger::Data m_localLogger ;
	int m_id ;
	AddToTable m_addToTable ;
	Error m_error ;
};

template< typename AddToTable,typename TableWidget,typename Error >
auto make_loggerPlaylistDownloader( TableWidget& t,Logger& logger,int id,AddToTable add,Error err )
{
	using lpd = loggerPlaylistDownloader< AddToTable,TableWidget,Error > ;

	return lpd( t,logger,id,std::move( add ),std::move( err ) ) ;
}
#endif

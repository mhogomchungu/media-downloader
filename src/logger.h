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

#include "logwindow.h"
#include "util.hpp"

class Logger
{
public:
	class Data
	{
	public:
		bool isEmpty() const
		{
			return m_lines.empty() ;
		}
		bool isNotEmpty() const
		{
			return !this->isEmpty() ;
		}
		size_t size() const
		{
			return m_lines.size() ;
		}
		const QByteArray& operator[]( size_t s ) const
		{
			return m_lines[ s ].text() ;
		}
		template< typename Function >
		void forEach( Function function ) const
		{
			for( const auto& it : m_lines ){

				function( it.id(),it.text() ) ;
			}
		}
		const QByteArray& lastText() const
		{
			return m_lines[ m_lines.size() - 1 ].text() ;
		}
		bool lastLineIsProgressLine() const
		{
			return m_lines[ m_lines.size() - 1 ].progressLine() ;
		}
		bool doneDownloading() const
		{
			return m_doneDownloading ;
		}
		void clear()
		{
			m_lines.clear() ;
		}

		QList< QByteArray > toStringList() const ;

		QByteArray toString() const
		{
			if( this->isNotEmpty() ){

				auto it = m_lines.begin() ;

				auto m = it->text() ;

				it++ ;

				for( ; it != m_lines.end() ; it++ ){

					m += "\n" + it->text() ;
				}

				return m ;
			}else{
				return {} ;
			}
		}
		QByteArray toLine() const
		{
			if( this->isNotEmpty() ){

				auto it = m_lines.begin() ;

				auto m = it->text() ;

				it++ ;

				for( ; it != m_lines.end() ; it++ ){

					m += it->text() ;
				}

				return m ;
			}else{
				return {} ;
			}
		}
		void removeLast()
		{
			m_lines.pop_back() ;
		}
		void replaceLast( const QByteArray& e )
		{
			m_lines.rbegin()->replace( e ) ;
		}
		template< typename Function,typename Add >
		void replaceOrAdd( const QByteArray& text,int id,Function function,Add add )
		{
			_replaceOrAdd( text,id,std::move( function ),std::move( add ) ) ;
		}
		void add( const QByteArray& text,int id = -1 )
		{
			auto _false = []( const QByteArray& ){ return false ; } ;

			_replaceOrAdd( text,id,_false,_false ) ;
		}
	private:
		bool postProcessText( const QByteArray& data ) ;

		template< typename Function,typename Add >
		void _replaceOrAdd( const QByteArray& text,int id,Function function,Add add )
		{
			if( this->postProcessText( text ) ){

				m_doneDownloading = true ;

				return ;
			}

			if( id != -1 ){

				for( auto it = m_lines.rbegin() ; it != m_lines.rend() ; it++ ){

					if( it->id() == id ){

						if( function( it->text() ) ){

							if( add( it->text() ) ){

								this->add( it,text,id ) ;
							}else{
								it->replace( text ) ;
							}
						}else{
							this->add( it,text,id ) ;
						}

						return ;
					}
				}
			}

			m_lines.emplace_back( text,id ) ;
		}
		template< typename It >
		void add( const It& it,const QByteArray& text,int id )
		{
			if( it != m_lines.rbegin() ){

				m_lines.emplace( it.base(),text,id ) ;
			}else{
				m_lines.emplace_back( text,id ) ;
			}
		}
		class line
		{
		public:
			line( const QByteArray& text,int id,bool p = false ) :
				m_text( text ),
				m_id( id ),
				m_progressLine( p )
			{
			}
			line( const QByteArray& text ) :
				m_text( text ),
				m_id( -1 )
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
			int id() const
			{
				return m_id ;
			}
			void replace( const QByteArray& text )
			{
				m_progressLine = true ;
				m_text = text ;
			}
		private:
			QByteArray m_text ;
			int m_id ;
			bool m_progressLine ;
		} ;
		std::vector< Logger::Data::line > m_lines ;
		bool m_doneDownloading = false ;
	} ;

	Logger( QPlainTextEdit&,QWidget * parent,settings& ) ;
	void add( const QString& s )
	{
		this->add( s.toUtf8() ) ;
	}
	void add( const QByteArray&,int id = -1 ) ;
	void clear() ;
	template< typename Function >
	void add( const Function& function,int id )
	{
		function( m_lines,id,true ) ;

		this->update() ;
	}
	void logError( const QByteArray& data,int id )
	{
		auto function = []( const QByteArray& ){ return true ; } ;

		m_lines.replaceOrAdd( "[media-downloader][std error] " + data,id,function,function ) ;

		this->update() ;
	}
	void showLogWindow() ;
	void updateView( bool e ) ;
	Logger( const Logger& ) = delete ;
	Logger& operator=( const Logger& ) = delete ;
	Logger( Logger&& ) = delete ;
	Logger& operator=( Logger&& ) = delete ;

	class updateLogger
	{
	public:
		struct args
		{
			template< typename Engine >
			args( const Engine& engine ) :
				controlStructure( engine.controlStructure() ),
				skipLinesWithText( engine.skiptLineWithText() ),
				splitLinesBy( engine.splitLinesBy() ),
				likeYoutubeDl( engine.likeYoutubeDl() )
			{
			}
			const QJsonObject& controlStructure ;
			const QStringList& skipLinesWithText ;
			const QStringList& splitLinesBy ;
			const bool likeYoutubeDl ;
		} ;
		template< typename Engine >
		updateLogger( const QByteArray& data,
			      const Engine& engine,
			      Logger::Data& outPut,
			      int id,
			      bool humanReadableJson ) :
			m_args( engine ),m_outPut( outPut ),m_id( id )
		{
			this->run( humanReadableJson,data ) ;
		}
	private:
		void run( bool humanReadableJson,const QByteArray& data ) ;
		bool meetCondition( const QByteArray& line,const QJsonObject& obj ) const ;
		bool meetCondition( const QByteArray& line ) const ;
		bool skipLine( const QByteArray& line ) const ;
		void add( const QByteArray& data,QChar token ) const ;
		updateLogger::args m_args ;
		Logger::Data& m_outPut ;
		int m_id ;
	};
private:
	void update() ;
	logWindow m_logWindow ;
	QPlainTextEdit& m_textEdit ;
	Logger::Data m_lines ;
	bool m_updateView = false ;
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

template< typename Function,typename FunctionUpdate,typename Error >
class loggerBatchDownloader
{
public:
	loggerBatchDownloader( Function function,Logger& logger,FunctionUpdate ff,Error err,int id ) :
		m_functionUpdate( std::move( ff ) ),
		m_function( std::move( function ) ),
		m_error( std::move( err ) ),
		m_logger( logger ),
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

			m_lines.add( s ) ;
		}else{
			m_lines.add( "[media-downloader] " + s ) ;
		}

		this->update() ;
	}
	void clear()
	{
		m_functionUpdate( "" ) ;
		m_lines.clear() ;
	}
	template< typename F >
	void add( const F& function )
	{
		m_logger.add( function,m_id ) ;
		function( m_lines,-1,false ) ;
		this->update() ;
	}
	void logError( const QByteArray& data )
	{
		m_error( data ) ;
		m_logger.logError( data,m_id ) ;
	}
private:
	void update()
	{
		if( m_lines.isNotEmpty() ){

			m_functionUpdate( m_function( m_lines ) ) ;
		}
	}
	FunctionUpdate m_functionUpdate ;
	Function m_function ;
	Error m_error ;
	Logger& m_logger ;
	Logger::Data m_lines ;
	int m_id ;
} ;

template< typename Function,typename FunctionUpdate,typename Error >
auto make_loggerBatchDownloader( Function function,Logger& logger,FunctionUpdate fu,Error err,int id )
{
	return loggerBatchDownloader< Function,FunctionUpdate,Error >( std::move( function ),
								 logger,
								 std::move( fu ),
								 std::move( err ),
								 id ) ;
}

template< typename AddToTable,typename TableWidget >
class loggerPlaylistDownloader
{
public:
	loggerPlaylistDownloader( TableWidget& t,Logger& logger,int id,AddToTable add ) :
		m_table( t ),
		m_logger( logger ),
		m_id( id ),
		m_addToTable( std::move( add ) )
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

		m_lines.clear() ;
	}
	template< typename Function >
	void add( const Function& function )
	{
		m_logger.add( function,m_id ) ;
		function( m_lines,-1,false ) ;
		m_addToTable( m_table,m_lines ) ;
	}
	void logError( const QByteArray& data )
	{
		m_logger.logError( data,m_id ) ;
	}
private:
	TableWidget& m_table ;
	Logger& m_logger ;
	Logger::Data m_lines ;
	int m_id ;
	AddToTable m_addToTable ;
};

template< typename AddToTable,typename TableWidget >
auto make_loggerPlaylistDownloader( TableWidget& t,Logger& logger,int id,AddToTable add )
{
	return loggerPlaylistDownloader< AddToTable,TableWidget >( t,logger,id,std::move( add ) ) ;
}
#endif

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
		const QString& operator[]( size_t s ) const
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
		const QString& secondFromLast() const
		{
			return m_lines[ m_lines.size() - 2 ].text() ;
		}
		const QString& lastText() const
		{
			return m_lines[ m_lines.size() - 1 ].text() ;
		}
		void clear()
		{
			m_lines.clear() ;
		}

		QStringList toStringList() const ;

		QString toString() const
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
		void removeLast()
		{
			m_lines.pop_back() ;
		}
		void replaceLast( const QString& e )
		{
			m_lines.rbegin()->replace( e ) ;
		}
		template< typename Function,
			  typename Add,
			  typename Arg >
		void replaceOrAdd( const Arg& arg,const QString& text,int id,Function function,Add add )
		{
			_replaceOrAdd( arg,text,id,std::move( function ),std::move( add ) ) ;
		}
		void add( const QString& text,int id = -1 )
		{
			_replaceOrAdd( 0,
				       text,
				       id,
				       []( const int&,const QString& ){ return false ; },
				       []( const QString& ){ return false ; } ) ;
		}
	private:
		template< typename Function,
			  typename Add,
			  typename Arg >
		void _replaceOrAdd( const Arg& arg,const QString& text,int id,Function function,Add add )
		{
			if( id != -1 ){

				for( auto it = m_lines.rbegin() ; it != m_lines.rend() ; it++ ){

					if( it->id() == id ){

						if( function( arg,it->text() ) ){

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
		void add( const It& it,const QString& text,int id )
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
			line( const QString& text,int id ) :
				m_text( text ),
				m_id( id )
			{
			}
			line( const QString& text ) :
				m_text( text ),
				m_id( -1 )
			{
			}
			const QString& text() const
			{
				return m_text ;
			}
			const QString& text()
			{
				return m_text ;
			}
			int id()
			{
				return m_id ;
			}
			int id() const
			{
				return m_id ;
			}
			void replace( const QString& text )
			{
				m_text = text ;
			}
		private:
			QString m_text ;
			int m_id ;
		} ;
		std::vector< Logger::Data::line > m_lines ;
	} ;

	Logger( QPlainTextEdit& ) ;
	void add( const QString&,int id = -1 ) ;
	void clear() ;
	template< typename Function >
	void add( const Function& function,int id )
	{
		function( m_lines,id ) ;
		this->update() ;
	}
	Logger( const Logger& ) = delete ;
	Logger& operator=( const Logger& ) = delete ;
	Logger( Logger&& ) = delete ;
	Logger& operator=( Logger&& ) = delete ;
private:
	void update() ;
	QPlainTextEdit& m_textEdit ;
	Logger::Data m_lines ;
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
		m_logger->add( e,m_id ) ;
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

template< typename Function,
	  typename Engine >
class loggerBatchDownloader
{
public:
	loggerBatchDownloader( Function function,
			       Engine& engine,
			       Logger& logger,
			       QTableWidgetItem& item,
			       int id ) :
		m_tableWidgetItem( item ),
		m_function( std::move( function ) ),
		m_engine( engine ),
		m_logger( logger ),
		m_id( id )
	{
	}
	void add( const QString& s )
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
		m_tableWidgetItem.setText( "" ) ;
		m_lines.clear() ;
	}
	template< typename F >
	void add( const F& function )
	{
		m_logger.add( function,m_id ) ;
		function( m_lines,-1 ) ;
		this->update() ;
	}
private:
	void update()
	{
		if( m_lines.isNotEmpty() ){

			auto& function = *m_function ;
			m_tableWidgetItem.setText( function( m_engine,m_lines ) ) ;
		}
	}
	QTableWidgetItem& m_tableWidgetItem ;
	Function m_function ;
	Engine& m_engine ;
	Logger& m_logger ;
	Logger::Data m_lines ;
	int m_id ;
} ;

template< typename Function,typename Engine >
static auto make_loggerBatchDownloader( Function function,
					Engine& engine,
					Logger& logger,
					QTableWidgetItem& item,
					int id )
{
	return loggerBatchDownloader< Function,Engine >( std::move( function ),engine,logger,item,id ) ;
}

template< typename AddToTable >
class loggerPlaylistDownloader
{
public:
	loggerPlaylistDownloader( QTableWidget& t,
				  Logger& logger,
				  const QString& u,
				  int id,
				  AddToTable add ) :
		m_table( t ),
		m_logger( logger ),
		m_urlPrefix( u ),
		m_id( id ),
		m_addToTable( std::move( add ) )
	{
		this->clear() ;
	}
	void add( const QString& e )
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
		function( m_lines,-1 ) ;

		this->update() ;
	}
private:
	void update()
	{
		auto s = m_lines.size() ;

		if( s > 0 && s % 2 == 0 ){

			auto a = m_urlPrefix + m_lines.lastText() ;
			auto b = m_lines.secondFromLast() ;
			auto c = a + "\n" + b ;

			m_addToTable( m_table,c ) ;
		}
	}
private:
	QTableWidget& m_table ;
	Logger& m_logger ;
	const QString& m_urlPrefix ;
	Logger::Data m_lines ;
	int m_id ;
	AddToTable m_addToTable ;
};

template< typename AddToTable >
auto make_loggerPlaylistDownloader( QTableWidget& t,
				    Logger& logger,
				    const QString& u,
				    int id,
				    AddToTable add )
{
	return loggerPlaylistDownloader< AddToTable >( t,logger,u,id,std::move( add ) ) ;
}
#endif

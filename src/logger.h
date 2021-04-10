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
	Logger( QPlainTextEdit& ) ;
	void add( const QString& ) ;
	void clear() ;
	template< typename Function >
	void add( Function function )
	{
		function( m_text ) ;
		this->update() ;
	}
	Logger( const Logger& ) = delete ;
	Logger& operator=( const Logger& ) = delete ;
	Logger( Logger&& ) = delete ;
	Logger& operator=( Logger&& ) = delete ;
private:
	void update() ;
	QPlainTextEdit& m_textEdit ;
	QStringList m_text ;
} ;

class LoggerWrapper
{
public:
	LoggerWrapper() = delete ;
	LoggerWrapper( Logger& logger ) : m_logger( &logger )
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
	void add( Function function )
	{
		m_logger->add( std::move( function ) ) ;
	}
private:
	Logger * m_logger ;
};

template< typename Function,
	  typename Engine >
class loggerBatchDownloader
{
public:
	loggerBatchDownloader( Function function,Engine& engine,QTableWidgetItem& item ) :
		m_tableWidgetItem( item ),
		m_function( std::move( function ) ),
		m_engine( engine )
	{
	}
	void add( const QString& s )
	{
		if( s.startsWith( "[media-downloader]" ) ){

			m_text.append( s ) ;
		}else{
			m_text.append( "[media-downloader] " + s ) ;
		}

		this->update() ;
	}
	void clear()
	{
		m_tableWidgetItem.setText( "" ) ;
		m_text.clear() ;
	}
	template< typename F >
	void add( F function )
	{
		function( m_text ) ;
		this->update() ;
	}
private:
	void update()
	{
		if( m_text.size() > 0 ){

			auto& function = *m_function ;
			m_tableWidgetItem.setText( function( m_engine,m_text.last() ) ) ;
		}
	}
	QTableWidgetItem& m_tableWidgetItem ;
	QStringList m_text ;
	Function m_function ;
	Engine& m_engine ;
} ;

template< typename Function,typename Engine >
static auto make_loggerBatchDownloader( Function function,Engine& engine,QTableWidgetItem& item )
{
	return loggerBatchDownloader< Function,Engine >( std::move( function ),engine,item ) ;
}

class loggerPlaylistDownloader
{
public:
	loggerPlaylistDownloader( QTableWidget& t,const QFont& f,bool debug ) :
		m_table( t ),
		m_font( f ),
		m_debug( debug )
	{
		this->clear() ;
	}
	void add( const QString& s )
	{
		if( m_debug ){

			qDebug() << s ;
		}
	}
	void clear()
	{
		auto s = m_table.rowCount() ;

		for( int i = 0 ; i < s ; i++ ){

			m_table.removeRow( 0 ) ;
		}

		m_text.clear() ;
	}
	template< typename F >
	void add( F function )
	{
		function( m_text ) ;
		this->update() ;
	}
private:
	void update()
	{
		if( m_text.size() > 0 ){

			auto row = m_table.rowCount() ;

			m_table.insertRow( row ) ;

			auto item = new QTableWidgetItem() ;

			item->setText( "https://youtube.com/watch?v=" + m_text.last() ) ;
			item->setTextAlignment( Qt::AlignCenter ) ;
			item->setFont( m_font ) ;
			m_table.setItem( row,0,item ) ;
		}
	}
private:
	QTableWidget& m_table ;
	QStringList m_text ;
	const QFont& m_font ;
	bool m_debug ;
};

#endif

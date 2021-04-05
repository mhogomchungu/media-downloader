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

template< typename Function >
class LoggerTableWidgetItem
{
public:
	LoggerTableWidgetItem( Function function,QTableWidgetItem& item ) :
		m_tableWidgetItem( item ),
		m_function( std::move( function ) )
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
#if 1
		if( m_text.size() > 0 ){

			m_tableWidgetItem.setText( m_function( m_text.last() ) ) ;
		}
#else
		QString m ;

		int s = m_text.size() ;

		if( s == 0 ){


		}else if( s == 1 ){

			m = m_text.at( s - 1 ) ;

		}else if( s == 2 ){

			m = m_text.at( s - 2 ) + "\n" + m_text.at( s - 1 ) ;
		}else{
			m = m_text.at( s - 3 ) + "\n" + m_text.at( s - 2 ) + "\n" + m_text.at( s - 1 ) ;
		}

		m_tableWidgetItem.setText( m ) ;
#endif
	}
	QTableWidgetItem& m_tableWidgetItem ;
	QStringList m_text ;
	Function m_function ;
} ;

template< typename Function >
static auto loggerLoggerTableWidgetItem( Function function,QTableWidgetItem& item )
{
	return LoggerTableWidgetItem< Function >( std::move( function ),item ) ;
}

#endif

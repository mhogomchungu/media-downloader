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

class LoggerTableWidgetItem
{
public:
	LoggerTableWidgetItem( QTableWidgetItem& ) ;
	void add( const QString& ) ;
	void clear() ;
	template< typename Function >
	void add( Function function )
	{
		function( m_text ) ;
		this->update() ;
	}
private:
	void update() ;
	QTableWidgetItem& m_tableWidgetItem ;
	QStringList m_text ;
} ;

#endif

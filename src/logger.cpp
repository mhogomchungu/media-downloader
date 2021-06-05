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

#include "logger.h"

#include "engines.h"

#include "utility.h"

Logger::Logger( QPlainTextEdit& e,const QString& s ) :
	m_textEdit( e ),
	m_lines( s )
{
	m_textEdit.setReadOnly( true ) ;
}

void Logger::add( const QString& s,int id )
{
	if( s.startsWith( "[media-downloader]" ) ){

		m_lines.add( s,id ) ;
	}else{
		m_lines.add( "[media-downloader] " + s,id ) ;
	}

	this->update() ;
}

void Logger::clear()
{
	m_lines.clear() ;
	m_textEdit.clear() ;
}

void Logger::update()
{
	m_textEdit.setPlainText( m_lines.toString() ) ;

	m_textEdit.moveCursor( QTextCursor::End ) ;
}

const QString& _set_nothing()
{
	static QString m ;
	return m ;
}

Logger::Data::Data() : m_skipData( _set_nothing() )
{
}

QStringList Logger::Data::toStringList() const
{
	return utility::split( this->toString(),'\n',true ) ;
}

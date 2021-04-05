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

Logger::Logger( QPlainTextEdit& e ) : m_textEdit( e )
{
	m_textEdit.setReadOnly( true ) ;
}

void Logger::add( const QString& s )
{
	if( s.startsWith( "[media-downloader]" ) ){

		m_text.append( s ) ;
	}else{
		m_text.append( "[media-downloader] " + s ) ;
	}

	this->update() ;
}

void Logger::clear()
{
	m_text.clear() ;
	m_textEdit.clear() ;
}

void Logger::update()
{
	m_textEdit.setPlainText( m_text.join( '\n' ) ) ;
	m_textEdit.moveCursor( QTextCursor::End ) ;
}

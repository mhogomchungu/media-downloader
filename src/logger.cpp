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

#include "engines/yt-dlp.h"

Logger::Logger( QPlainTextEdit& e,QWidget *,settings& s ) :
	m_logWindow( nullptr,s,*this ),
	m_textEdit( e ),
	m_processOutPuts( true ),
	m_debugProcessOutPuts( true ),
	m_settings( s )
{
	m_textEdit.setReadOnly( true ) ;
}

void Logger::registerDone( int id )
{
	if( m_processOutPuts.registerDone( id ) ){

		this->update() ;
	}
}

void Logger::add( const QByteArray& s,int id )
{
	if( s.startsWith( "[media-downloader]" ) ){

		m_processOutPuts.add( s,id ) ;
	}else{
		m_processOutPuts.add( "[media-downloader] " + s,id ) ;
	}

	this->update() ;
}

void Logger::clear()
{
	m_processOutPuts.clear() ;
	m_debugProcessOutPuts.clear() ;
	m_textEdit.clear() ;
	m_logWindow.clear() ;
}

void Logger::setMaxProcessLog( int s )
{
	m_processOutPuts.removeExtraLogs() ;
	m_maxProcessLog = s ;
}

void Logger::showLogWindow()
{
	m_logWindow.setText( m_processOutPuts.toLines() ) ;
	m_logWindow.Show() ;
}

void Logger::showDebugLogWindow()
{
	m_logWindow.setText( m_debugProcessOutPuts.debugOutPut() ) ;
	m_logWindow.Show() ;
}

void Logger::reTranslateLogWindow()
{
	m_logWindow.retranslateUi() ;
}

void Logger::updateView( bool e )
{
	m_updateView = e ;
	this->update() ;
}

void Logger::update()
{
	auto s = m_settings.maxLoggerProcesses() ;

	auto e = m_maxProcessLog > s ? m_maxProcessLog : s ;

	while( true ){

		int current_amount = static_cast< int >( m_processOutPuts.size() ) ;

		if( current_amount > e ){

			if( !m_processOutPuts.removeFirstFinished() ){

				break ;
			}
		}else{
			break ;
		}
	}

	auto m = m_processOutPuts.toLines() ;

	if( m_updateView ){

		m_textEdit.setPlainText( m ) ;
		m_textEdit.moveCursor( QTextCursor::End ) ;
	}

	m_logWindow.update( m ) ;
}

bool Logger::Data::registerDone( int id )
{
	for( auto& it : m_processOutputs ){

		if( it.processId() == id ){

			it.setProcessAsFinished() ;

			return true ;
		}
	}

	return false ;
}

QByteArray Logger::Data::debugOutPut() const
{
	QByteArray m ;

	for( const auto& it : m_processOutputs ){

		auto s = "Process ID: " + QString::number( it.processId() ).toUtf8() ;

		const auto& ee = it.entries() ;

		if( ee.empty() ){

			s += "\n<EMPTY>" ;
		}else{
			for( const auto& it : ee ){

				s += "\n" + it.text() + "\n----------" ;
			}
		}

		s += "\n*******************************\n" ;

		m += s ;
	}

	return m ;
}

QByteArray Logger::Data::join( const QByteArray& joiner ) const
{
	QByteArray m ;

	for( const auto& it : m_processOutputs ){

		const auto& ee = it.entries() ;

		if( !ee.empty() ){

			if( m.isEmpty() ){

				m = ee[ 0 ].text() ;
			}else{
				m += joiner + ee[ 0 ].text() ;
			}

			for( size_t i = 1 ; i < ee.size() ; i++ ){

				m += joiner + ee[ i ].text() ;
			}
		}
	}

	return m ;
}

void Logger::Data::removeExtraLogs()
{
	auto& v = m_processOutputs ;
	auto m = Logger::Data::processOutput::IdLessThanZero() ;
	v.erase( std::remove( v.begin(),v.end(),m ),v.end() ) ;
}

bool Logger::Data::removeFirstFinished()
{
	for( auto it = m_processOutputs.begin() ; it != m_processOutputs.end() ; it++ ){

		if( it->processFinished() ){

			m_processOutputs.erase( it ) ;

			return true ;
		}
	}

	return false ;
}

bool Logger::Data::doneDownloadingText( const QByteArray& data )
{
	return utility::stringConstants::doneDownloadingText( data ) ;
}

QString Logger::locale::locale::formattedDataSize( qint64 s ) const
{
#if QT_VERSION >= QT_VERSION_CHECK( 5,14,0 )
	return m_locale.formattedDataSize( s ) ;
#else
	std::array< const char *,7 > sizes = { "EiB", "PiB", "TiB", "GiB", "MiB", "KiB", "B" } ;

	qint64  multiplier = 1024ULL * 1024ULL * 1024ULL * 1024ULL * 1024ULL * 1024ULL ;

	QString result ;

	for( size_t i = 0 ; i < sizes.size() ; i++,multiplier /= 1024 ){

		if( s < multiplier ){

			continue ;
		}

		if( s % multiplier == 0 ){

			auto a = QString::number( s / multiplier ) ;
			auto b = sizes[ i ] ;

			result = QString( "%1 %2" ).arg( a,b ) ;
		}else{
			auto a = static_cast< double >( s ) / static_cast< double >( multiplier ) ;
			auto b = sizes[ i ] ;
			auto c = QString::number( a,'f',2 ) ;

			result = QString( "%1 %2" ).arg( c,b ) ;
		}

		return result ;
	}

	return {} ;
#endif
}

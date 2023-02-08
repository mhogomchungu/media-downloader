/*
 *
 *  Copyright (c) 2022
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

#include "svtplay-dl.h"
#include "../settings.h"
#include "../util.hpp"

svtplay_dl::svtplay_dl( const engines& engines,const engines::engine& engine,QJsonObject& ) :
	engines::engine::functions( engines.Settings(),engine,engines.processEnvironment() ),
	m_processEnvironment( engines::engine::functions::processEnvironment() )
{
	m_processEnvironment.insert( "PYTHONUNBUFFERED","true" ) ;
}

void svtplay_dl::updateOutPutChannel( QProcess::ProcessChannel& s ) const
{
	s = QProcess::ProcessChannel::StandardError ;
}

QStringList svtplay_dl::horizontalHeaderLabels() const
{
	auto m = engines::engine::functions::horizontalHeaderLabels() ;

	m[ 1 ] = QObject::tr( "Method" ) ;

	return m ;
}

std::vector<engines::engine::functions::mediaInfo> svtplay_dl::mediaProperties( const QByteArray& e )
{
	auto mm = util::split( e,'\n',true ) ;

	if( mm.size() <= 1 ){

		return {} ;
	}

	mm.removeAt( 0 ) ;

	std::vector< engines::engine::functions::mediaInfo > s ;

	for( const auto& it : mm ){

		auto a = util::split( it,' ',true ) ;

		auto n = a.size() ;

		if( n > 4 ){

			a.takeAt( 0 ) ;
			auto format     = a.takeAt( 0 ) ;
			auto method     = "Method: " + a.takeAt( 0 ) ;
			auto codec      = a.takeAt( 0 ) ;
			auto resolution = a.takeAt( 0 ) ;
			auto notes      = method + "\n" + a.join( ", " ) ;

			s.emplace_back( format,codec,resolution,notes ) ;

		}else if( n == 3 ){

			a.takeAt( 0 ) ;
			auto format     = a.takeAt( 0 ) ;
			auto method     = "Method: " + a.takeAt( 0 ) ;
			auto codec      = a.takeAt( 0 ) ;
			auto resolution = "N/A" ;
			auto notes = method ;

			s.emplace_back( format,codec,resolution,notes ) ;
		}
	}

	return s ;
}

const QProcessEnvironment& svtplay_dl::processEnvironment() const
{
	return m_processEnvironment ;
}

svtplay_dl::~svtplay_dl()
{
}

engines::engine::functions::DataFilter svtplay_dl::Filter( int id,const QString& e )
{
	auto& s = engines::engine::functions::Settings() ;
	const auto& engine = engines::engine::functions::engine() ;

	return { util::types::type_identity< svtplay_dl::svtplay_dlFilter >(),e,s,engine,id } ;
}

QString svtplay_dl::updateTextOnCompleteDownlod( const QString& uiText,
						 const QString& bkText,
						 const QString& dopts,
						 const engines::engine::functions::finishedState& f )
{
	if( f.success() ){

		return engines::engine::functions::updateTextOnCompleteDownlod( uiText,dopts,f ) ;

	}else if( f.cancelled() ){

		return engines::engine::functions::updateTextOnCompleteDownlod( bkText,dopts,f ) ;
	}else{
		using functions = engines::engine::functions ;

		if( uiText.startsWith( "ERROR:" ) ){

			auto m = engines::engine::functions::updateTextOnCompleteDownlod( uiText.mid( 6 ),dopts,f ) ;

			return m + "\n" + bkText ;

		}else if( uiText.contains( "Temporary failure in name resolution" ) ){

			return functions::errorString( f,functions::errors::noNetwork,bkText ) ;

		}else if( uiText.contains( "Name or service not known" ) ){

			return functions::errorString( f,functions::errors::unknownUrl,bkText ) ;
		}else{
			auto m = engines::engine::functions::processCompleteStateText( f ) ;
			return m + "\n" + bkText ;
		}
	}
}

svtplay_dl::svtplay_dlFilter::svtplay_dlFilter( const QString& e,settings&,const engines::engine& engine,int id ) :
	engines::engine::functions::filter( e,engine,id ),
	m_processId( id )
{
	Q_UNUSED( m_processId )
}

const QByteArray& svtplay_dl::svtplay_dlFilter::operator()( const Logger::Data& s )
{
	if( s.doneDownloading() ){

		auto m = s.toLine() ;

		if( m.contains( "Temporary failure in name resolution" ) ){

			m_tmp = "Temporary failure in name resolution" ;

		}else if( m.contains( "Name or service not known" ) ){

			m_tmp = "Name or service not known" ;
		}

		return m_tmp ;

	}else if( s.lastLineIsProgressLine() ){

		return s.lastText() ;
	}else{
		const auto& m = s.lastText() ;

		if( m.startsWith( "WARNING" ) && m.contains( "already exists" ) ){

			auto a = m.indexOf( '(' ) ;
			auto b = m.lastIndexOf( ')' ) ;

			if( a != -1 && b != -1 && b > a ){

				m_tmp = "\"" + m.mid( a + 1,b - a - 1 ) + "\" already exists" ;
			}else{
				m_tmp = "media already exists" ;
			}

			return m_tmp ;

		}else if( m.startsWith( "Merge audio, video and subtitle into " ) ){

			m_tmp = m.mid( 37 ) ;
			return m_tmp ;

		}else if( m.startsWith( "INFO: Merge audio and video into " ) ){

			m_tmp = m.mid( 33 ) ;
			return m_tmp ;

		}else if( m.startsWith( "INFO: " ) ){

			return m_tmp ;

		}else if( m.startsWith( "ERROR:" ) ){

			m_tmp = m ;

			return m_tmp ;
		}else{
			return m_preProcessing.text() ;
		}
	}
}

svtplay_dl::svtplay_dlFilter::~svtplay_dlFilter()
{
}

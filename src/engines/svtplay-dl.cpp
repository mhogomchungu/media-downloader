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
#include "../downloadmanager.h"
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

std::vector<QStringList> svtplay_dl::mediaProperties( const QByteArray& e )
{
	auto m = e ;

	m.replace( "INFO: ","" ) ;

	auto mm = util::split( m,'\n',true ) ;

	if( mm.size() > 0 ){

		mm.removeAt( 0 ) ;
	}

	if( mm.size() == 0 ){

		return {} ;
	}

	std::vector< QStringList > s ;

	for( const auto& it : mm ){

		auto a = util::split( it,' ',true ) ;

		if( a.size() > 5 ){

			auto format     = a.takeAt( 0 ) ;
			auto method     = "Method: " + a.takeAt( 0 ) ;
			auto codec      = a.takeAt( 0 ) ;
			auto resolution = a.takeAt( 0 ) ;
			auto language   = "Language: " + a.takeAt( 0 ) ;
			auto role       = "Role: " + a.takeAt( 0 ) ;
			auto notes      = method + ", " + language + "\n" + role ;

			s.emplace_back( QStringList{ format,codec,resolution,notes } ) ;
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

void svtplay_dl::updateDownLoadCmdOptions( const engines::engine::functions::updateOpts& s )
{
	const auto& engine = engines::engine::functions::engine() ;

	if( !( s.quality == "default" || s.quality == "Default" ) ){

		s.ourOptions.append( engine.optionsArgument() ) ;
		s.ourOptions.append( s.quality ) ;
	}
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
	}else{
		return engines::engine::functions::updateTextOnCompleteDownlod( bkText,dopts,f ) ;
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
	if( s.lastLineIsProgressLine() ){

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

			return m ;
		}else{
			return m_preProcessing.text() ;
		}
	}
}

svtplay_dl::svtplay_dlFilter::~svtplay_dlFilter()
{
}

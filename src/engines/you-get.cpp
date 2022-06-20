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

#include "you-get.h"
#include "../settings.h"
#include "../downloadmanager.h"
#include "../util.hpp"

#include <cstring>

you_get::you_get( const engines& engines,const engines::engine& engine,QJsonObject& ) :
	engines::engine::functions( engines.Settings(),engine,engines.processEnvironment() )
{
}

std::vector<QStringList> you_get::mediaProperties( const QByteArray& e )
{
	QJsonParseError err ;

	auto json = QJsonDocument::fromJson( e,&err ) ;

	std::vector<QStringList> s ;

	if( err.error == QJsonParseError::NoError ){

		const auto obj = json.object().value( "streams" ).toObject() ;

		utility::locale locale ;

		for( auto it = obj.begin() ; it != obj.end() ; it++ ){

			QStringList m ;

			auto oo = it.value().toObject() ;

			m.append( oo.value( "itag" ).toString() ) ;
			m.append( oo.value( "container" ).toString() ) ;
			m.append( oo.value( "quality" ).toString().replace( " ","\n" ) ) ;

			auto mm = "size: " + locale.formattedDataSize( oo.value( "size" ).toInt() ) ;
			mm += "\ntype: " + oo.value( "type" ).toString() ;

			m.append( mm ) ;

			s.emplace_back( std::move( m ) ) ;
		}
	}

	return s ;
}

you_get::~you_get()
{
}

void you_get::updateDownLoadCmdOptions( const engines::engine::functions::updateOpts& s )
{
	const auto& engine = engines::engine::functions::engine() ;

	if( !s.quality.isEmpty() && !( s.quality == "default" || s.quality == "Default" ) ){

		s.ourOptions.append( engine.optionsArgument() ) ;
		s.ourOptions.append( s.quality ) ;
	}
}

engines::engine::functions::DataFilter you_get::Filter( int id,const QString& e )
{
	auto& s = engines::engine::functions::Settings() ;
	const auto& engine = engines::engine::functions::engine() ;

	return { util::types::type_identity< you_get::you_getFilter >(),e,s,engine,id } ;
}

QString you_get::updateTextOnCompleteDownlod( const QString& uiText,
					      const QString& bkText,
					      const QString& dopts,
					      const engines::engine::functions::finishedState& f )
{
	if( f.success() ){

		auto m = util::split( uiText,'\n',true ) ;

		if( m.size() == 2 ){

			m.removeAt( 1 ) ;
		}

		return engines::engine::functions::updateTextOnCompleteDownlod( m.join( "\n" ),dopts,f ) ;
	}else{
		return engines::engine::functions::updateTextOnCompleteDownlod( bkText,dopts,f ) ;
	}
}

you_get::you_getFilter::you_getFilter( const QString& e,settings&,const engines::engine& engine,int id ) :
	engines::engine::functions::filter( e,engine,id ),
	m_processId( id )
{
	Q_UNUSED( m_processId )
}

const QByteArray& you_get::you_getFilter::operator()( const Logger::Data& s )
{
	if( s.lastLineIsProgressLine() ){

		if( m_title.isEmpty() ){

			return s.lastText() ;
		}else{
			m_tmp = m_title + "\n" + s.lastText() ;
			return m_tmp ;
		}
	}else{
		auto m = s.toLine() ;

		if( m_title.isEmpty() ){

			auto a = m.indexOf( "title:               " ) ;
			auto b = m.indexOf( "stream:" ) ;

			if( a != -1 && b != -1 ){

				int s = static_cast< int >( std::strlen( "title:               " ) ) ;
				m_title = m.mid( a + s,b - ( a + s ) ) ;
			}
		}

		auto a = m.indexOf( "Skipping ./" ) ;
		auto b = m.indexOf( ": file already exists" ) ;

		if( a != -1 && b != -1 ){

			int s = static_cast< int >( std::strlen( "Skipping ./" ) ) ;

			m_title = m.mid( a + s, b - ( a + s ) ) ;

			return m_title ;
		}else{
			auto a = m.indexOf( "Merged into " ) ;

			if( a != -1 ){

				auto b = m.indexOf( "Saving" ) ;

				if( b == -1 ){

					b = m.indexOf( "Skipping " ) ;
				}

				if( a != -1 && b != -1 ){

					int s = static_cast< int >( std::strlen( "Merged into " ) ) ;
					m_title = m.mid( a + s, b - ( a + s ) ) ;

					return m_title ;
				}
			}

			if( m_title.isEmpty() ){

				return m_preProcessing.text() ;
			}else{
				m_tmp = m_title + "\n" + m_preProcessing.text() ;
				return m_tmp ;
			}
		}
	}
}

you_get::you_getFilter::~you_getFilter()
{
}

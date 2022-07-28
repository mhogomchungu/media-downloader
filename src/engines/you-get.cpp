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

bool you_get::foundNetworkUrl( const QString& url )
{
	return url.startsWith( "you-get" ) && url.endsWith( ".tar.gz" ) ;
}

void you_get::renameArchiveFolder( const QString& e )
{
	const auto m = QDir( e ).entryList( QDir::Filter::Dirs | QDir::Filter::NoDotAndDotDot ) ;

	const auto& name = engines::engine::functions::engine().name() ;

	QDir dir ;

	for( const auto& it : m ){

		QFileInfo s( e + "/" + it ) ;

		if( s.isDir() && it.startsWith( name ) ){

			if( it != name ){

				dir.rename( e + "/" + it,e + "/" + name ) ;
				break ;
			}
		}
	}
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

		return engines::engine::functions::updateTextOnCompleteDownlod( uiText,dopts,f ) ;
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
	if( s.doneDownloading() ){

		return m_title ;

	}else if( s.lastLineIsProgressLine() ){

		if( m_title.isEmpty() ){

			return s.lastText() ;
		}else{
			m_tmp = m_title + "\n" + s.lastText() ;
			return m_tmp ;
		}
	}else{
		auto m = s.toLine() ;

		auto strLen = []( const char * s ){

			return static_cast< int >( std::strlen( s ) ) ;
		} ;

		if( m_title.isEmpty() ){

			auto a = m.indexOf( "title:               " ) ;
			auto b = m.indexOf( "stream:" ) ;

			if( a != -1 && b != -1 ){

				auto s = strLen( "title:               " ) ;
				m_title = m.mid( a + s,b - ( a + s ) ) ;
			}
		}

		auto a = m.indexOf( "Skipping ./" ) ;
		auto b = m.indexOf( ": file already exists" ) ;

		if( a != -1 && b != -1 ){

			auto s = strLen( "Skipping ./" ) ;

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

					auto s = strLen( "Merged into " ) ;
					m_title = m.mid( a + s, b - ( a + s ) ) ;

					return m_title ;
				}
			}

			return m_preProcessing.text() ;
		}
	}
}

you_get::you_getFilter::~you_getFilter()
{
}

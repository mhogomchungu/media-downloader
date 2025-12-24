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
#include "../util.hpp"
#include "../utility.h"

#include <cstring>

you_get::you_get( const engines& engines,const engines::engine& engine,QJsonObject& ) :
	engines::engine::baseEngine( engines.Settings(),engine,engines.processEnvironment() )
{
}

QString you_get::updateCmdPath( const QString& e )
{
	const auto& name = engines::engine::baseEngine::engine().name() ;

	return e + "/" + name + "/" + name ;
}

bool you_get::foundNetworkUrl( const QString& url )
{
	if( url.startsWith( "you_get" ) || url.startsWith( "you-get" ) ){

		return url.endsWith( this->archiveExtension() ) ;
	}else{
		return false ;
	}
}

engines::engine::baseEngine::optionsEnvironment you_get::setProxySetting( QStringList& e,const QString& s )
{
	e.append( "--http-proxy" ) ;
	e.append( s ) ;

	return {} ;
}

engines::engine::baseEngine::renameArchiveFolderStatus
you_get::renameArchiveFolder( const QString& archivePath,const QString& binPath )
{
	auto m = this->archiveExtension() ;

	const auto& name = engines::engine::baseEngine::engine().name() ;

	auto oldPath = binPath + "/" + QFileInfo( archivePath ).fileName().replace( m,"" ) ;
	auto newPath = binPath + "/" + name ;

	auto s = utility::rename( oldPath,newPath ) ;

	if( s.isEmpty() ){

		return {} ;
	}else{
		return { oldPath,newPath,s } ;
	}
}

std::vector<engines::engine::baseEngine::mediaInfo> you_get::mediaProperties( Logger& l,const QByteArray& e )
{
	QJsonParseError err ;

	auto json = QJsonDocument::fromJson( e,&err ) ;

	std::vector<engines::engine::baseEngine::mediaInfo> s ;

	if( err.error != QJsonParseError::NoError ){

		utility::failedToParseJsonData( l,err ) ;
	}else{
		const auto obj = json.object().value( "streams" ).toObject() ;

		Logger::locale locale ;

		for( auto it = obj.begin() ; it != obj.end() ; it++ ){

			QStringList l ;

			auto oo = it.value().toObject() ;

			auto url = oo.value( "url" ) ;

			if( url.isUndefined() ){

				const auto arr = oo.value( "src" ).toArray() ;

				for( const auto& it : arr ){

					const auto xrr = it.toArray() ;

					for( const auto& xt : xrr ){

						l.append( xt.toString() ) ;
					}
				}
			}else{
				l.append( url.toString() ) ;
			}

			auto a  = oo.value( "itag" ).toString() ;
			auto b  = oo.value( "container" ).toString() ;
			auto c  = oo.value( "quality" ).toString().replace( " ","\n" ) ;
			auto d  = oo.value( "size" ).toInt() ;
			auto e  = locale.formattedDataSize( d ) ;
			auto f  = QString::number( d ) ;
			auto g = "type: " + oo.value( "type" ).toString() ;

			s.emplace_back( l,a,b,c,e,f,g,"","" ) ;
		}
	}

	return s ;
}

you_get::~you_get()
{
}

engines::engine::baseEngine::DataFilter you_get::Filter( int id )
{
	auto& s = engines::engine::baseEngine::Settings() ;
	const auto& engine = engines::engine::baseEngine::engine() ;

	return { util::types::type_identity< you_get::you_getFilter >(),s,engine,id } ;
}

QString you_get::updateTextOnCompleteDownlod( const QString& uiText,
					      const QString& bkText,
					      const QString& dopts,
					      const QString& tabName,
					      const engines::engine::baseEngine::finishedState& f )
{
	if( f.success() ){

		return engines::engine::baseEngine::updateTextOnCompleteDownlod( uiText,dopts,tabName,f ) ;

	}else if( f.cancelled() ){

		return engines::engine::baseEngine::updateTextOnCompleteDownlod( bkText,dopts,tabName,f ) ;

	}else if( uiText.startsWith( "you-get: [Error]" ) ){

		using functions = engines::engine::baseEngine ;

		if( uiText.contains( "Invalid video format" ) ){

			return functions::errorString( f,functions::errors::unknownFormat,bkText ) ;
		}else{
			auto s = uiText.mid( 16 ) ;
			auto m = engines::engine::baseEngine::updateTextOnCompleteDownlod( s,dopts,tabName,f ) ;

			return m + "\n" + bkText ;
		}
	}else{
		auto m = engines::engine::baseEngine::processCompleteStateText( f ) ;
		return m + "\n" + bkText ;
	}
}

you_get::you_getFilter::you_getFilter( settings&,const engines::engine& engine,int id ) :
	engines::engine::baseEngine::filter( engine,id ),
	m_processId( id )
{
	Q_UNUSED( m_processId )
}

const QByteArray& you_get::you_getFilter::operator()( Logger::Data& s )
{
	if( s.doneDownloading() ){

		if( m_title.isEmpty() ){

			s.toStringList().forEach( [ & ]( const QByteArray& e ){

				if( e.startsWith( "you-get: [Error]" ) ){

					m_title = e ;

					if( m_title.endsWith( '.' ) ){

						m_title.truncate( m_title.size() - 1 ) ;
					}

					return true ;
				}else{
					return false ;
				}
			} ) ;
		}

		s.addFileName( m_title ) ;

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

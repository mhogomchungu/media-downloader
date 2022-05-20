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

#include "lux.h"
#include "../utility.h"

lux::~lux()
{
}

lux::lux( const engines& engines,const engines::engine& engine,QJsonObject& ) :
	engines::engine::functions( engines.Settings(),engine ),
	m_engine( engine )
{
}

engines::engine::functions::DataFilter lux::Filter( int id,const QString& e )
{
	return { util::types::type_identity< lux::lux_dlFilter >(),e,m_engine,id } ;
}

std::vector<QStringList> lux::mediaProperties( const QByteArray& e )
{
	QJsonParseError err ;

	auto json = QJsonDocument::fromJson( e,&err ) ;

	if( err.error == QJsonParseError::NoError ){

		return this->mediaProperties( json.array() ) ;
	}else{
		return {} ;
	}
}

std::vector<QStringList> lux::mediaProperties( const QJsonArray& arr )
{
	std::vector<QStringList> ent ;

	utility::locale locale ;

	for( const auto& it : arr ){

		const auto obj = it.toObject().value( "streams" ).toObject() ;

		for( auto it = obj.begin() ; it != obj.end() ; it++ ){

			auto obj = it.value().toObject() ;

			auto quality = obj.value( "quality" ).toString() ;

			auto m = util::split( quality,' ',true ) ;

			QString resolution ;

			if( m.size() > 1 && m[ 1 ].startsWith( "video" ) ){

				resolution = m.takeFirst() ;
			}

			auto id = obj.value( "id" ).toString() ;
			auto size = locale.formattedDataSize( obj.value( "size" ).toInt() ) ;
			auto notes = "Size: " + size + "\n" + m.join( " " ) ;
			auto extension = obj.value( "ext" ).toString() ;

			ent.emplace_back( QStringList{ id,extension,resolution,notes } ) ;
		}
	}

	return ent ;
}

bool lux::parseOutput( Logger::Data& outPut,const QByteArray& data,int id,bool )
{
	outPut.luxHack( id,data ) ;

	return false ;
}

void lux::runCommandOnDownloadedFile( const QString&,const QString& )
{
}

bool lux::foundNetworkUrl( const QString& s )
{
	if( utility::platformIsWindows() ){

		if( utility::platformIs32Bit() ){

			return s.contains( "Windows_32-bit" ) ;
		}else{
			return s.contains( "Windows_64-bit" ) ;
		}

	}else if( utility::platformIsLinux() ){

		if( utility::platformIs32Bit() ){

			return s.contains( "Linux_32-bit" ) ;
		}else{
			return s.contains( "Linux_64-bit" ) ;
		}
	}else{
		return s.contains( "macOS_64-bit" ) ;
	}
}

QString lux::updateTextOnCompleteDownlod( const QString& uiText,
					  const QString& bkText,
					  const QString& dopts,
					  const engines::engine::functions::finishedState& f )
{
	if( f.cancelled() ){

		return engines::engine::functions::updateTextOnCompleteDownlod( bkText,dopts,f ) ;

	}else if( f.success() ){

		auto a = util::split( uiText,'\n',true ) ;

		a.removeLast() ;

		return engines::engine::functions::updateTextOnCompleteDownlod( a.join( '\n' ),dopts,f ) ;
	}else{
		return engines::engine::functions::updateTextOnCompleteDownlod( uiText,dopts,f ) ;
	}
}

void lux::updateDownLoadCmdOptions( const engines::engine::functions::updateOpts& s )
{
	if( !s.quality.isEmpty() && s.quality.compare( "Default",Qt::CaseInsensitive ) ){

		s.ourOptions.append( m_engine.optionsArgument() ) ;

		s.ourOptions.append( s.quality ) ;
	}
}

lux::lux_dlFilter::lux_dlFilter( const QString& e,const engines::engine& engine,int id ) :
	engines::engine::functions::filter( e,engine,id ),
	m_processId( id )
{
	if( m_processId ){}
}

const QByteArray& lux::lux_dlFilter::operator()( const Logger::Data& e )
{
	const auto& s = e.lastText() ;

	auto line = e.toLine() ;

	if( s.startsWith( "[media-downloader]" ) ){

		return m_progress.text() ;

	}else if( s.startsWith( "Elapsed" ) ){

		return m_progress.text() ;
	}

	if( m_title == "Title: Unknown" ){

		auto a = line.indexOf( "Title:" ) ;
		auto b = line.indexOf( "Type:" ) ;

		if( a != -1 && b != -1 && b > a ){

			m_title = line.mid( a,b - a ).trimmed().mid( 7 ) ;

			m_title.replace( "\n","" ) ;

			while( m_title.size() > 7 && m_title[ 8 ] == ' ' ){

				m_title.remove( 8,1 ) ;
			}
		}
	}

	auto m = line.indexOf( "Merging video parts into " ) ;
	auto mm = line.indexOf( " file already exists, skipping" ) ;

	if( mm != -1 ){

		m_tmp = m_title + "\nblabla" ;

	}else if( m != -1 ){

		m_tmp = line.mid( m + 25 ) + "\n" + m_title ;
	}else{
		auto a = line.indexOf( "Downloading " ) ;

		if( a != -1 ){

			m_tmp = line.mid( a + 12 ) ;

			auto b = m_tmp.indexOf( " error:" ) ;

			if( b != -1 ){

				m_tmp.truncate( b ) ;
			}

			return m_tmp ;
		}

		e.reverseForEach( [ & ]( int,const QByteArray& data ){

			if( data.isEmpty() || data == "\n" ){

				return false ;
			}else{
				auto e = util::split( data,'\n' ) ;

				if( !e.isEmpty() ){

					m_tmp = m_title + "\n" + e.last() ;
				}else{
					m_tmp = m_title + "\n" + s ;
				}

				return true ;
			}
		} ) ;

		return m_tmp ;
	}

	return m_tmp ;
}

lux::lux_dlFilter::~lux_dlFilter()
{
}

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

engines::engine::functions::DataFilter lux::Filter( const QString& e )
{
	const auto& engine = engines::engine::functions::engine() ;

	return { util::types::type_identity< lux::lux_dlFilter >(),e,engine } ;
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

		a.removeFirst() ;
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

lux::lux_dlFilter::lux_dlFilter( const QString& e,const engines::engine& engine ) :
	engines::engine::functions::filter( e,engine )
{
}

const QByteArray& lux::lux_dlFilter::operator()( const Logger::Data& e )
{
	const auto& s = e.lastText() ;

	if( s.startsWith( "[media-downloader]" ) ){

		return m_tmp ;

	}else if( s.startsWith( "Elapsed" ) ){

		return s ;
	}

	if( m_title == "Title: Unknown" ){

		auto m = e.toLine() ;

		auto a = m.indexOf( "Title:" ) ;
		auto b = m.indexOf( "Type:" ) ;

		if( a != -1 && b != -1 && b > a ){

			m_title = m.mid( a,b - a ).trimmed() ;

			m_title.replace( "\n","" ) ;

			while( m_title.size() > 7 && m_title[ 8 ] == ' ' ){

				m_title.remove( 8,1 ) ;
			}
		}
	}

	if( m_size == "Size: Unknown" ){

		auto m = e.toLine() ;

		auto a = m.indexOf( "Size:" ) ;
		auto b = m.indexOf( "# download with:" ) ;

		if( a != -1 && b != -1  && b > a ){

			m_size = m.mid( a,b - a ).trimmed() ;

			m_size.replace( "\n","" ) ;

			while( m_size.size() > 6 && m_size[ 7 ] == ' ' ){

				m_size.remove( 7,1 ) ;
			}
		}
	}

	auto m = s.indexOf( "Merging video parts into " ) ;

	if( m != -1 ){

		m_tmp = m_size + "\n" + s.mid( m + 25 ) + "\n" + m_title ;
	}else{
		if( s.contains( '\n' ) ){

			auto e = util::split( s,'\n' ) ;

			e.removeAll( "" ) ;

			if( !e.isEmpty() ){

				m_tmp1 = m_size + "\n" + m_title + "\n" + e.last() ;
			}else{
				m_tmp1 = m_size + "\n" + m_title + "\n" + s ;
			}
		}else{
			m_tmp1 = m_size + "\n" + m_title + "\n" + s ;
		}

		return m_tmp1 ;
	}

	return m_tmp ;
}

lux::lux_dlFilter::~lux_dlFilter()
{
}

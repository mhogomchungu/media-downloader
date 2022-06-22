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
	engines::engine::functions( engines.Settings(),engine,engines.processEnvironment() ),
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
	outPut.luxHack( id,data,[ this,&outPut ]( const QByteArray& allData,const QByteArray& lastData ){

		if( lastData.startsWith( "[media-downloader]" ) ){

			return Logger::Data::luxResult{ false,lastData } ;
		}

		auto ee = allData.indexOf( "\n\n" ) ;

		QByteArray header ;

		if( ee != -1 ){

			header = allData.mid( 0,ee ) ;
		}

		auto& lux = outPut.luxHackStuff() ;

		if( lux.fileSizeString.isEmpty() ){

			auto s = header ;

			s.replace( " ","" ) ;
			s.replace( "\n","" ) ;

			auto a = s.indexOf( "Size:" ) ;
			auto b = s.indexOf( "Bytes)" ) ;

			if( a != -1 && b != -1 ){

				lux.fileSizeString = s.mid( a + 5,a + 5 - b ) ;
				lux.fileSizeString.replace( "Bytes","" ) ;

				auto mm = util::split( lux.fileSizeString,"(" ) ;

				if( mm.size() == 2 ){

					lux.fileSizeString = mm.at( 0 ) ;

					auto mmm = util::split( mm.at( 1 ),")" ).at( 0 ) ;

					lux.fileSizeInt = mmm.toLongLong() ;
				}
			}
		}

		auto ss = allData.indexOf( "-]" ) ;

		if( ss == -1 ){

			ss = allData.indexOf( ">]" ) ;
		}

		if( ss == -1 ){

			ss = allData.indexOf( "=]" ) ;

			if( ss != -1 ){

				return Logger::Data::luxResult{ true,lastData } ;
			}
		}

		if( ss != -1 ){

			auto s = util::split( allData.mid( ss + 1 ),' ' ) ;

			if( s.size() > 1 ){

				auto a = s.at( s.size() - 1 ) ;
				auto b = s.at( s.size() - 2 ) ;
				auto c = b ;

				b.replace( "%","" ) ;

				bool ok ;
				auto bb = qint64( b.toDouble( &ok ) * double( lux.fileSizeInt ) / 100 ) ;

				if( ok ){

					auto bbb = m_locale.formattedDataSize( bb ) ;

					auto aa = "Time left: " + a ;
					auto bb = "Downloaded: " + bbb + " / " + lux.fileSizeString ;
					auto cc = "(" + c + ")" ;

					auto ggg = aa + ", " + bb + " " + cc ;

					return Logger::Data::luxResult{ true,header + "\n" + ggg.toUtf8() } ;
				}else{
					return Logger::Data::luxResult{ true,header + "\n" + lastData } ;
				}
			}else{
				return Logger::Data::luxResult{ false,header + "\n" + lastData } ;
			}
		}else{
			return Logger::Data::luxResult{ false,header } ;
		}
	} ) ;

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

		return engines::engine::functions::updateTextOnCompleteDownlod( uiText,dopts,f ) ;
	}else{
		return engines::engine::functions::updateTextOnCompleteDownlod( uiText,dopts,f ) ;
	}
}

lux::lux_dlFilter::lux_dlFilter( const QString& e,const engines::engine& engine,int id ) :
	engines::engine::functions::filter( e,engine,id ),
	m_processId( id )
{
	Q_UNUSED( m_processId )
}

const QByteArray& lux::lux_dlFilter::operator()( const Logger::Data& e )
{
	const auto& s = e.lastText() ;

	auto line = e.allData() ;

	auto ss = s.indexOf( "Time left:" ) ;

	if( ss != -1 && m_title != "Title: Unknown" ){

		m_tmp = m_title + "\n" + s.mid( ss ) ;

		return m_tmp ;
	}
	if( s.startsWith( "[media-downloader]" ) ){

		if( s.startsWith( "[media-downloader] Done Processing And Shutting Down ..." ) ){

			return m_title ;
		}else{
			return m_progress.text() ;
		}

	}else if( s.startsWith( "Elapsed" ) ){

		return m_progress.text() ;
	}

	auto m = util::split( line,'\n' ) ;

	for( int i = 0 ; i < m.size() ; i++ ){

		const auto& it = m[ i ] ;

		if( m_title == "Title: Unknown" ){

			if( it.contains( "Title:" ) ){

				m_title = it.mid( 12 ) ;
			}
		}

		if( it.contains( ": file already exists, skipping" ) ){

			auto s = util::split( it,'\n' ) ;

			for( const auto& ss : s ){

				auto m = ss.indexOf( ": file already exists, skipping" ) ;

				if( m != -1 ){

					m_title = ss.mid( 0,m ) ;

					return m_title ;
				}
			}
		}else{
			auto m = it.indexOf( "Merging video parts into " ) ;

			if( m != -1 ){

				m_title = it.mid( m + 25 ) ;
			}
		}
	}

	return m_title ;
}

lux::lux_dlFilter::~lux_dlFilter()
{
}

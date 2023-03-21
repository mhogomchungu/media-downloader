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
#include "../utils/threads.hpp"

#include <QCryptographicHash>

#include <ctime>

lux::~lux()
{
}

lux::lux( const engines& engines,const engines::engine& engine,QJsonObject&,const QString& df ) :
	engines::engine::functions( engines.Settings(),engine,engines.processEnvironment() ),
	m_engine( engine ),
	m_downloadFolder( df + "/" )
{
}

engines::engine::functions::DataFilter lux::Filter( int id,const QString& e )
{
	return { util::types::type_identity< lux::lux_dlFilter >(),e,m_engine,id,m_downloadFolder.toUtf8() } ;
}

std::vector<engines::engine::functions::mediaInfo> lux::mediaProperties( const QByteArray& e )
{
	QJsonParseError err ;

	auto json = QJsonDocument::fromJson( e,&err ) ;

	if( err.error == QJsonParseError::NoError ){

		return this->mediaProperties( json.array() ) ;
	}else{
		return {} ;
	}
}

std::vector<engines::engine::functions::mediaInfo> lux::mediaProperties( const QJsonArray& arr )
{
	std::vector<engines::engine::functions::mediaInfo> ent ;

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

			QStringList urls ;

			const auto parts = obj.value( "parts" ).toArray() ;

			for( const auto& it : parts ){

				urls.append( it.toObject().value( "url" ).toString() ) ;
			}

			ent.emplace_back( urls,id,extension,resolution,notes ) ;
		}
	}

	return ent ;
}

Logger::Data::luxResult lux::parseOutput::operator()( int processId,
						      Logger::Data& outPut,
						      const QByteArray& allData,
						      const QByteArray& lastData ) const
{
	if( lastData.startsWith( "[media-downloader]" ) ){

		return { Logger::Data::luxResult::ac::add,lastData } ;
	}

	const auto& lHeader = outPut.LuxHeader( processId ) ;

	if( !lHeader.has_value() ){

		//Should not get here

		return { Logger::Data::luxResult::ac::nothing,QByteArray() } ;
	}

	auto& luxHeader = lHeader.value() ;

	if( luxHeader.data.isEmpty() ){

		auto ee = allData.indexOf( "...\n\n" ) ;

		if( ee != -1 ){

			luxHeader.timeLeft = QObject::tr( "Time Left" ).toUtf8() ;

			auto mm = allData.lastIndexOf( "Site:" ) ;

			if( mm != -1 ){

				luxHeader.data = allData.mid( mm,ee ) ;
			}else{
				luxHeader.data = allData.mid( 0,ee ) ;
			}

			auto aa = luxHeader.data.indexOf( "Title:" ) ;
			auto bb = luxHeader.data.indexOf( "Type:" ) ;

			if( aa != -1 && bb != -1 ){

				luxHeader.title = luxHeader.data.mid( aa + 6,bb - aa - 6 ).trimmed() ;
			}

			auto s = luxHeader.data.simplified() ;

			s.replace( " ","" ) ;
			s.replace( "\n","" ) ;

			auto a = s.indexOf( "Size:" ) ;
			auto b = s.indexOf( "Bytes)" ) ;

			if( a != -1 && b != -1 ){

				auto j = s.mid( a + 5,a + 5 - b ) ;
				j.replace( "Bytes","" ) ;

				auto mm = util::split( j,'(' ) ;

				if( mm.size() == 2 ){

					j = mm.at( 0 ) ;

					auto mmm = util::split( mm.at( 1 ),")" ).at( 0 ) ;

					luxHeader.fileSizeInt = mmm.toLongLong() ;
					luxHeader.fileSizeString = m_locale.formattedDataSize( luxHeader.fileSizeInt ).toUtf8() ;

					return { Logger::Data::luxResult::ac::replace,luxHeader.data + "\n" + lastData } ;
				}
			}
		}else{
			if( allData.contains( "Site:" ) ){

				return { Logger::Data::luxResult::ac::nothing,QByteArray() } ;
			}
		}
	}

	auto ss = allData.lastIndexOf( "=]" ) ;

	if( ss != -1 ){

		auto mm = allData.indexOf( "Merging video parts into " ) ;

		if( mm != -1 ){

			return { Logger::Data::luxResult::ac::replace,luxHeader.data + "\n" + allData.mid( mm ) } ;
		}
	}

	ss = allData.lastIndexOf( "-]" ) ;

	if( ss == -1 ){

		ss = allData.lastIndexOf( ">]" ) ;
	}

	if( ss != -1 ){

		auto s = util::split( allData.mid( ss + 1 ),' ' ) ;

		if( s.size() > 1 ){

			auto a = s.at( s.size() - 1 ) ;
			auto b = s.at( s.size() - 2 ) ;
			auto c = b ;

			b.replace( "%","" ) ;

			bool ok ;
			auto bb = qint64( b.toDouble( &ok ) * double( luxHeader.fileSizeInt ) / 100 ) ;

			if( ok ){

				auto bbb = m_locale.formattedDataSize( bb ) ;

				auto aa = luxHeader.timeLeft + ": " + a ;
				auto bb = QObject::tr( "Downloaded" ) + ": " + bbb + " / " + luxHeader.fileSizeString ;
				auto cc = "(" + c + ")" ;

				auto ggg = aa + ", " + bb + " " + cc ;

				return { Logger::Data::luxResult::ac::replace,luxHeader.data + "\n" + ggg.toUtf8() } ;
			}else{
				return { Logger::Data::luxResult::ac::replace,luxHeader.data + "\n" + lastData } ;
			}
		}else{
			return { Logger::Data::luxResult::ac::add,luxHeader.data + "\n" + lastData } ;
		}
	}else{
		return { Logger::Data::luxResult::ac::add,lastData } ;
	}
}

bool lux::parseOutput( Logger::Data& outPut,const QByteArray& data,int id,bool )
{
	outPut.luxHack( id,data,outPut,m_parseOutput ) ;

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

void lux::updateDownLoadCmdOptions( const engines::engine::functions::updateOpts& s )
{
	engines::engine::functions::updateDownLoadCmdOptions( s ) ;

	for( int m = 0 ; m < s.ourOptions.size() ; m++ ){

		if( s.ourOptions[ m ] == "-O" ){

			if( m + 1 < s.ourOptions.size() ){

				auto& e = s.ourOptions[ m + 1 ] ;

				auto r = s.uiIndex.toString( true,s.ourOptions ) ;

				e.replace( "%(autonumber)s",r ) ;

				if( e.contains( "%(id)s" ) ){

					e.replace( "%(id)s",[ &r ](){

						auto m = std::time( nullptr ) ;
						auto e = QString::number( m ) + r ;

						QCryptographicHash hash( QCryptographicHash::Sha256 ) ;

						hash.addData( e.toUtf8() ) ;

						return hash.result().toHex().mid( 0,8 ) ;
					}() ) ;
				}

				break ;
			}
		}
	}
}

QString lux::updateTextOnCompleteDownlod( const QString& uiText,
					  const QString& bkText,
					  const QString& dopts,
					  const engines::engine::functions::finishedState& f )
{
	using functions = engines::engine::functions ;

	if( f.cancelled() ){

		return functions::updateTextOnCompleteDownlod( bkText,dopts,f ) ;

	}else if( f.success() ){

		return functions::updateTextOnCompleteDownlod( uiText,dopts,f ) ;

	}else if( uiText == "invalid URI for request" ){

		return functions::errorString( f,functions::errors::unknownUrl,bkText ) ;

	}else if( uiText == "connect: cannot assign requested address" ){

		return functions::errorString( f,functions::errors::noNetwork,bkText ) ;

	}else if( uiText == "no stream named " ){

		return functions::errorString( f,functions::errors::unknownFormat,bkText ) ;
	}else{
		auto m = engines::engine::functions::processCompleteStateText( f ) ;
		return m + "\n" + bkText ;
	}
}

lux::lux_dlFilter::lux_dlFilter( const QString& e,const engines::engine& engine,int id,QByteArray df ) :
	engines::engine::functions::filter( e,engine,id ),
	m_banner( ".. " + QObject::tr( "This May Take A Very Long Time" ).toUtf8() + " .." ),
	m_processId( id ),
	m_downloadFolder( std::move( df ) )
{
}

const QByteArray& lux::lux_dlFilter::operator()( const Logger::Data& e )
{
	const auto& eee = e.LuxHeader( m_processId ) ;

	if( !eee.has_value() ){

		//Should not get here
		return e.lastText() ;
	}

	const auto& s = e.lastText() ;

	if( s.startsWith( "[media-downloader] cmd: " ) ){

		m_cmd = s ;

		return m_banner ;
	}

	const auto& luksHeader = eee.value() ;
	const auto& allData = e.allData( m_processId ) ;

	if( luksHeader.data.isEmpty() ){

		if( allData.has_value() ){

			const auto& e = allData.value() ;

			if( e.contains( "invalid URI for request" ) ){

				m_tmp = "invalid URI for request" ;
				return m_tmp ;

			}else if( e.contains( "connect: cannot assign requested address" ) ){

				m_tmp = "connect: cannot assign requested address" ;
				return m_tmp ;

			}else if( e.contains( "no stream named " ) ){

				m_tmp = "no stream named " ;
				return m_tmp ;
			}
		}

		m_tmp1 = m_banner + "\n" + s ;

		return m_tmp1 ;
	}

	auto ss = s.indexOf( luksHeader.timeLeft ) ;

	if( ss != -1 ){

		m_tmp1 = luksHeader.title + "\n" + s.mid( ss ) ;

		return m_tmp1 ;
	}

	if( s.startsWith( "[media-downloader]" ) ){

		if( utility::stringConstants::doneDownloadingText( s ) ){

			return this->renameTitle( luksHeader.title ) ;
		}else{
			return m_progress.text() ;
		}

	}else if( s.startsWith( "Elapsed" ) ){

		return m_progress.text() ;
	}

	if( !allData.has_value() ){

		//Should not get here

		return luksHeader.title ;
	}

	for( const auto& it : util::split( allData.value(),'\n' ) ){

		if( it.contains( ": file already exists, skipping" ) ){

			const auto s = util::split( it,'\n' ) ;

			for( const auto& ss : s ){

				auto m = ss.indexOf( ": file already exists, skipping" ) ;

				if( m != -1 ){

					m_tmp = ss.mid( 0,m ) ;

					return m_tmp ;
				}
			}
		}else if( it.contains( "status: ERROR, reason:" ) ){

			m_tmp = it ;

			return m_tmp ;
		}else{
			auto m = it.indexOf( "Merging video parts into " ) ;

			if( m != -1 ){

				m_fileName = it.mid( m + 25 ) ;

				return m_fileName ;
			}
		}
	}

	m_tmp = luksHeader.title + "\n" + m_progress.text() ;

	return m_tmp ;
}

lux::lux_dlFilter::~lux_dlFilter()
{
}

static void _replaceChars( QByteArray& )
{
}
template< typename ... T >
static void _replaceChars( QByteArray& e,const char * a,const char * b,T&& ... t )
{
	e.replace( a,b ) ;
	_replaceChars( e,std::forward< T >( t ) ... ) ;
}

static QByteArray _title( QByteArray title )
{
	// Got these substitions from lux source code
	// https://github.com/iawia002/lux/blob/c97baa8c5325c48618a6e0b243f3e614e7980f43/utils/utils.go#L89

	_replaceChars( title,"\n"," ","/"," ","|","-",": ","：",":","：","'","’" ) ;

	if( utility::platformIsWindows() ){

		_replaceChars( title,"\""," ","?"," ","*"," ","\\"," ","<"," ",">"," " ) ;
	}

	return title ;
}

const QByteArray& lux::lux_dlFilter::renameTitle( const QByteArray& title )
{
	if( m_fileName.isEmpty() ){

		/*
		 * We will get here is no merging took place and hence we have no
		 * file name.
		 */

		auto fileName = [ this,&title ](){

			auto m = util::splitPreserveQuotes( m_cmd ) ;

			for( int i = 0 ; i < m.size() ; i++ ){

				if( m[ i ] == "-O" ){

					if( i + 1 < m.size() ){

						return m[ i + 1 ].toUtf8() ;
					}
				}
			}

			return _title( title ) ;
		}() ;

		if( QFile::exists( m_downloadFolder + fileName + ".webm" ) ){

			m_fileName = fileName + ".webm" ;

		}else if( QFile::exists( m_downloadFolder + fileName + ".mp4" ) ){

			m_fileName = fileName + ".mp4" ;
		}else{
			m_fileName = fileName ;
		}
	}

	if( m_fileName.contains( "%(title)s" ) ){

		auto originalFileName = m_downloadFolder + m_fileName ;
		auto newFileName = originalFileName ;

		newFileName.replace( "%(title)s",_title( title ) ) ;

		if( QFile::exists( newFileName ) ){

			auto a = newFileName.lastIndexOf( '.' ) ;

			if( a != -1 ){

				auto fn = newFileName.mid( 0,a - 1 ) + "-1" ;
				auto ext = newFileName.mid( a ) ;

				newFileName = fn + ext ;
			}
		}

		m_fileName = newFileName.mid( m_downloadFolder.size() ) ;

		utils::qthread::run( [ originalFileName,newFileName ](){

			QThread::currentThread()->msleep( 500 ) ;

			for( int i = 0 ; i < 4 ; i++ ){

				if( QFile::rename( originalFileName,newFileName ) ){

					break ;
				}else{
					QThread::currentThread()->msleep( 500 ) ;
				}
			}
		} ) ;
	}

	m_tmp = m_fileName ;

	return m_tmp ;
}

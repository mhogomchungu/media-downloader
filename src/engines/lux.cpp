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

const char * lux::testData()
{
	return R"R( Site:      YouTube youtube.com
 Title:     TRIPLE PLAY‼ Braves capitalize on shoddy baserunning by Red Sox | MLB on ESPN
 Type:      video
 Stream:
     [133]  -------------------
     Quality:         240p video/mp4; codecs="avc1.4d4015"
     Size:            0.84 MiB (885406 Bytes)
     # download with: lux -f 133 ...

0 B / 864.65 KiB [--------------------------------------------------------------------------] ? p/s 0.00% ?
0 B / 864.65 KiB [--------------------------------------------------------------------------] ? p/s 0.00% ?
0 B / 864.65 KiB [--------------------------------------------------------------------------] ? p/s 0.00% ?
32.00 KiB / 864.65 KiB [==>-------------------------------------------------------] 53.38 KiB p/s 3.70% 15s
64.00 KiB / 864.65 KiB [====>-----------------------------------------------------] 53.38 KiB p/s 7.40% 14s
80.00 KiB / 864.65 KiB [=====>----------------------------------------------------] 53.38 KiB p/s 9.25% 14s
112.00 KiB / 864.65 KiB [=======>------------------------------------------------] 58.54 KiB p/s 12.95% 12s
144.00 KiB / 864.65 KiB [=========>----------------------------------------------] 58.54 KiB p/s 16.65% 12s
176.00 KiB / 864.65 KiB [===========>--------------------------------------------] 58.54 KiB p/s 20.35% 11s
208.00 KiB / 864.65 KiB [=============>------------------------------------------] 65.07 KiB p/s 24.06% 10s
240.00 KiB / 864.65 KiB [===============>-----------------------------------------] 65.07 KiB p/s 27.76% 9s
272.00 KiB / 864.65 KiB [=================>---------------------------------------] 65.07 KiB p/s 31.46% 9s
304.00 KiB / 864.65 KiB [====================>------------------------------------] 71.20 KiB p/s 35.16% 7s
336.00 KiB / 864.65 KiB [======================>----------------------------------] 71.20 KiB p/s 38.86% 7s
368.00 KiB / 864.65 KiB [========================>--------------------------------] 71.20 KiB p/s 42.56% 6s
400.00 KiB / 864.65 KiB [==========================>------------------------------] 76.93 KiB p/s 46.26% 6s
432.00 KiB / 864.65 KiB [============================>----------------------------] 76.93 KiB p/s 49.96% 5s
464.00 KiB / 864.65 KiB [==============================>--------------------------] 76.93 KiB p/s 53.66% 5s
496.00 KiB / 864.65 KiB [================================>------------------------] 82.30 KiB p/s 57.36% 4s
528.00 KiB / 864.65 KiB [==================================>----------------------] 82.30 KiB p/s 61.06% 4s
560.00 KiB / 864.65 KiB [====================================>--------------------] 82.30 KiB p/s 64.77% 3s
592.00 KiB / 864.65 KiB [=======================================>-----------------] 87.31 KiB p/s 68.47% 3s
624.00 KiB / 864.65 KiB [=========================================>---------------] 87.31 KiB p/s 72.17% 2s
656.00 KiB / 864.65 KiB [===========================================>-------------] 87.31 KiB p/s 75.87% 2s
688.00 KiB / 864.65 KiB [=============================================>-----------] 91.99 KiB p/s 79.57% 1s
720.00 KiB / 864.65 KiB [===============================================>---------] 91.99 KiB p/s 83.27% 1s
752.00 KiB / 864.65 KiB [=================================================>-------] 91.99 KiB p/s 86.97% 1s
768.00 KiB / 864.65 KiB [==================================================>------] 94.66 KiB p/s 88.82% 1s
796.86 KiB / 864.65 KiB [====================================================>----] 94.66 KiB p/s 92.16% 0s
828.86 KiB / 864.65 KiB [======================================================>--] 94.66 KiB p/s 95.86% 0s
828.86 KiB / 864.65 KiB [======================================================>--] 95.09 KiB p/s 95.86% 0s
844.86 KiB / 864.65 KiB [=======================================================>-] 95.09 KiB p/s 97.71% 0s
864.65 KiB / 864.65 KiB [=====================================================] 139.03 KiB p/s 100.00% 6.4s
Merging video parts into TRIPLE PLAY‼ Braves capitalize on shoddy baserunning by Red Sox - MLB on ESPN.mp4
)R" ;
}

lux::~lux()
{
}

lux::lux( const engines& engines,const engines::engine& engine,QJsonObject&,const QString& df ) :
	engines::engine::functions( engines.Settings(),engine,engines.processEnvironment() ),
	m_engine( engine ),
	m_downloadFolder( df + "/" )
{
}

engines::engine::functions::DataFilter lux::Filter( int id )
{
	return { util::types::type_identity< lux::lux_dlFilter >(),m_engine,id,m_downloadFolder.toUtf8() } ;
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

	Logger::locale locale ;

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

void lux::runCommandOnDownloadedFile( const QString&,const QString& )
{
}

bool lux::foundNetworkUrl( const QString& s )
{
	if( utility::platformIsWindows() ){

		if( utility::platformIs32Bit() ){

			return s.contains( "Windows_i386" ) ;
		}else{
			return s.contains( "Windows_x86_64" ) ;
		}

	}else if( utility::platformIsLinux() ){

		if( utility::platformIs32Bit() ){

			return s.contains( "Linux_i386" ) ;
		}else{
			return s.contains( "Linux_x86_64" ) ;
		}
	}else{
		return false ;
	}
}

void lux::updateDownLoadCmdOptions( const engines::engine::functions::updateOpts& s )
{
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

	engines::engine::functions::updateDownLoadCmdOptions( s ) ;
}

static bool _meetCondition( const engines::engine&,const QByteArray& e )
{
	return e.contains( "] " ) && e.contains( " p/s " ) ;
}

static bool _meetLocalCondition( const engines::engine&,const QByteArray& e )
{
	return e.contains( ", ETA: " ) ;
}

class luxFilter : public engines::engine::functions::filterOutPut
{
public:
	luxFilter( const engines::engine& engine ) : m_engine( engine )
	{
	}
	engines::engine::functions::filterOutPut::result
	formatOutput( const engines::engine::functions::filterOutPut::args& args ) const override
	{
		const auto& locale = args.locale ;
		const auto& outPut = args.data ;
		const auto& e      = args.outPut ;

		const auto& luxHeader = outPut.luxHeader() ;

		const auto& m = luxHeader.allData() ;

		auto mm = m.lastIndexOf( "-]" ) ;

		if( mm == -1 ){

			mm = m.lastIndexOf( "=]" ) ;
		}

		if( mm == - 1 ){

			return { e,m_engine,_meetLocalCondition } ;
		}

		auto a = util::split( m.mid( mm + 2 ),' ' ) ;

		QString pgr = "%1 / %2 (%3) at %4, ETA: %5" ;

		if( a.size() > 4 ){

			auto speed = a[ 0 ] + " " + a[ 1 ] + "/s" ;

			auto perc = a[ 3 ] ;

			auto ee = perc ;
			ee.replace( "%","" ) ;

			auto percentage = ee.toDouble() / 100 ;

			auto totalSize = luxHeader.fileSizeInt() ;

			auto sizeString = locale.formattedDataSize( totalSize * percentage ) ;

			m_tmp = pgr.arg( sizeString,luxHeader.fileSize(),perc,speed,a[ 4 ] ).toUtf8() ;

			return { m_tmp,m_engine,_meetLocalCondition } ;

		}else if( a.size() == 4 ){

			QString s = "?" ;

			m_tmp = pgr.arg( s,s,s,s,s ).toUtf8() ;

			return { m_tmp,m_engine,_meetLocalCondition } ;
		}else{
			return { e,m_engine,_meetLocalCondition } ;
		}
	}
	bool meetCondition( const engines::engine::functions::filterOutPut::args& args ) const override
	{
		const auto& e = args.outPut ;
		auto& outPut  = args.data ;

		outPut.luxHeaderUpdateData( e ) ;

		const auto& luxHeader = outPut.luxHeader() ;

		if( luxHeader.invalid() && luxHeader.allData().contains( "..." ) ){

			this->setHeader( args ) ;
		}

		return _meetCondition( m_engine,e ) ;
	}
	const engines::engine& engine() const override
	{
		return m_engine ;
	}
	void setHeader( const engines::engine::functions::filterOutPut::args& args ) const
	{
		auto& outPut       = args.data ;
		const auto& locale = args.locale ;

		QByteArray webSite ;
		QByteArray title ;
		QByteArray fsizeS ;
		qint64 fsize = 0 ;

		for( const auto& it : outPut.toStringList() ){

			const QByteArray& e = it ;

			if( e.contains( "Site: " ) ){

				webSite = util::join( util::split( e,' ' ),1," " ) ;

			}else if( e.contains( "Title: " ) ){

				title = util::join( util::split( e,' ' ),1," " ) ;

			}else if( e.contains( "Size: " ) ){

				auto mm = util::split( e,' ' ) ;

				if( mm.size() > 1 ){

					auto ss = mm[ mm.size() - 2 ].mid( 1 ) ;

					fsize = ss.toLongLong() ;
					fsizeS = locale.formattedDataSize( fsize ).toUtf8() ;
				}
			}
		}

		outPut.setLuxHeader( { webSite,title,fsizeS,fsize } ) ;
	}
private:
	const engines::engine& m_engine ;
	mutable QByteArray m_tmp ;
} ;

engines::engine::functions::FilterOutPut lux::filterOutput()
{
	const engines::engine& engine = engines::engine::functions::engine() ;

	return { util::types::type_identity< luxFilter >(),engine } ;
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

lux::lux_dlFilter::lux_dlFilter( const engines::engine& engine,int id,QByteArray df ) :
	engines::engine::functions::filter( engine,id ),
	m_banner( ".. " + QObject::tr( "This May Take A Very Long Time" ).toUtf8() + " .." ),
	m_downloadFolder( std::move( df ) )
{
}

const QByteArray& lux::lux_dlFilter::operator()( const Logger::Data& e )
{	
	const auto& luxHeader = e.luxHeader() ;

	if( e.doneDownloading() ){

		const auto& allData = luxHeader.allData() ;

		if( allData.contains( ": file already exists, skipping" ) ){

			const auto s = util::split( allData,'\n' ) ;

			for( const auto& ss : s ){

				auto m = ss.indexOf( ": file already exists, skipping" ) ;

				if( m != -1 ){

					m_tmp = ss.mid( 0,m ) ;

					return m_tmp ;
				}
			}

		}else if( allData.contains( "status: ERROR, reason:" ) ){

			auto m = allData.indexOf( "status: ERROR, reason:" ) ;

			m_tmp = allData.mid( m + 22 ) ;

			return m_tmp ;

		}else if( allData.contains( "invalid URI for request" ) ){

			m_tmp = "invalid URI for request" ;
			return m_tmp ;

		}else if( allData.contains( "connect: cannot assign requested address" ) ){

			m_tmp = "connect: cannot assign requested address" ;
			return m_tmp ;

		}else if( allData.contains( "no stream named " ) ){

			m_tmp = "no stream named " ;
			return m_tmp ;
		}else{
			auto m = allData.indexOf( "Merging video parts into " ) ;

			if( m != -1 ){

				m_fileName = allData.mid( m + 25 ) ;

				m = m_fileName.indexOf( "[media-downloader]" ) ;

				if( m != -1 ){

					m_fileName = m_fileName.mid( 0,m ) ;
				}

				return m_fileName ;
			}
		}

		if( m_fileName.isEmpty() ){

			if( luxHeader.title().isEmpty() ){

				//???
				m_tmp.clear() ;

				return m_tmp ;
			}else{
				const auto& m = luxHeader.title() ;

				if( QFile::exists( m_downloadFolder + m + ".webm" ) ){

					m_fileName = m + ".webm" ;

					return m_fileName ;

				}else if( QFile::exists( m_downloadFolder + m + ".mp4" ) ){

					m_fileName = m + ".mp4" ;

					return m_fileName ;
				}else{
					return m ;
				}
			}
		}else{
			return m_fileName ;
		}
	}

	if( luxHeader.title().isEmpty() ){

		const auto& s = e.lastText() ;

		if( s.startsWith( "Elapsed Time:" ) ){

			m_tmp = m_banner + "\n" + s ;

			return m_tmp ;
		}else{
			return m_banner ;
		}

	}else if( e.lastLineIsProgressLine() ){

		m_tmp = luxHeader.title() + "\n" + e.lastText() ;

		return m_tmp ;
	}else{
		m_tmp = luxHeader.title() + "\n" + m_progress.text() ;

		return m_tmp ;
	}
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

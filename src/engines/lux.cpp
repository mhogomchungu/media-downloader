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
Merging video parts into TRIPLE PLAY‼ Braves capitalize on shoddy baserunning by Red Sox - MLB on ESPN.mp4)R" ;
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

lux::~lux()
{
}

lux::lux( const engines& engines,const engines::engine& engine,QJsonObject& ) :
	engines::engine::baseEngine( engines.Settings(),engine,engines.processEnvironment() ),
	m_engine( engine ),
	m_downloadFolder( engines.Settings().downloadFolder() + "/" )
{
}

engines::engine::baseEngine::DataFilter lux::Filter( int id )
{
	return { util::types::type_identity< lux::lux_dlFilter >(),m_engine,id,m_downloadFolder.toUtf8() } ;
}

engines::engine::baseEngine::optionsEnvironment lux::setProxySetting( QStringList&,const QString& e )
{
	return { "HTTPS_PROXY",e } ;
}

std::vector<engines::engine::baseEngine::mediaInfo> lux::mediaProperties( Logger& l,const QByteArray& e )
{
	QJsonParseError err ;

	auto json = QJsonDocument::fromJson( e,&err ) ;

	if( err.error == QJsonParseError::NoError ){

		return this->mediaProperties( l,json.array() ) ;
	}else{
		utility::failedToParseJsonData( l,err ) ;

		return {} ;
	}
}

std::vector<engines::engine::baseEngine::mediaInfo> lux::mediaProperties( Logger&,const QJsonArray& arr )
{
	std::vector<engines::engine::baseEngine::mediaInfo> ent ;

	Logger::locale locale ;

	QString title ;

	if( arr.size() ){

		title = arr[ 0 ].toObject().value( "title" ).toString() ;
	}

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

			auto id        = obj.value( "id" ).toString() ;
			auto sizeRaw   = obj.value( "size" ).toInt() ;
			auto size      = locale.formattedDataSize( sizeRaw ) ;
			auto notes     = m.join( " " ) ;
			auto extension = obj.value( "ext" ).toString() ;

			QStringList urls ;

			const auto parts = obj.value( "parts" ).toArray() ;

			for( const auto& it : parts ){

				urls.append( it.toObject().value( "url" ).toString() ) ;
			}

			auto sizeRawInt = QString::number( sizeRaw ) ;

			ent.emplace_back( urls,id,extension,resolution,size,sizeRawInt,notes,"",title ) ;
		}
	}

	return ent ;
}

bool lux::foundNetworkUrl( const QString& s )
{
	if( utility::platformIsWindows() ){

		if( utility::CPU().x86_32() ){

			return s.contains( "Windows_i386" ) ;
		}else{
			return s.contains( "Windows_x86_64" ) ;
		}

	}else if( utility::platformIsLinux() ){

		if( utility::CPU().x86_32() ){

			return s.contains( "Linux_i386" ) ;
		}else{
			return s.contains( "Linux_x86_64" ) ;
		}

	}else if( utility::platformIsOSX() ){

		return s.contains( "Darwin_x86_64.tar.gz" ) ;
	}else{
		return false ;
	}
}

static QByteArray _hash( const QString& r )
{
	auto m = utility::simpleRandomNumber() ;
	auto e = QString::number( m ) + r ;

	QCryptographicHash hash( QCryptographicHash::Sha256 ) ;

	hash.addData( e.toUtf8() ) ;

	return hash.result().toHex().mid( 0,8 ) ;
}

void lux::updateDownLoadCmdOptions( const engines::engine::baseEngine::updateOpts& s,
				    bool e,
				    const QStringList& extraOpts )
{
	engines::engine::baseEngine::updateDownLoadCmdOptions( s,e,extraOpts ) ;

	for( int m = 0 ; m < s.ourOptions.size() ; m++ ){

		if( s.ourOptions[ m ] == "-O" ){

			if( m + 1 < s.ourOptions.size() ){

				auto& e = s.ourOptions[ m + 1 ] ;

				auto r = s.uiIndex.toString( true,s.ourOptions ) ;

				e.replace( "%(autonumber)s",r ) ;

				if( e.contains( "%(id)s" ) ){

					e.replace( "%(id)s",_hash( r ) ) ;
				}

				break ;
			}
		}
	}
}

class LuxHeader
{
public:
	static const char * marker()
	{
		return "..." ;
	}
	LuxHeader( const Logger::locale& locale,const QByteArray& allData,int m )
	{
		this->parse( locale,allData,m ) ;
	}
	LuxHeader( const Logger::locale& locale,const QByteArray& allData )
	{
		this->parse( locale,allData,allData.indexOf( LuxHeader::marker() ) ) ;
	}
	const QByteArray& title() const
	{
		return m_title ;
	}
	const QByteArray& fileSize() const
	{
		return m_fileSizeString ;
	}
	QByteArray extension() const
	{
		const auto mm = util::split( m_quality,' ' ) ;

		for( const auto& it : mm ){

			if( it.endsWith( ";" ) ){

				auto e = it ;

				auto s = e.replace( ";","" ) ;

				auto ss = s.indexOf( "/" ) ;

				if( ss != -1 ){

					return "." + s.mid( ss + 1 ) ;
				}
			}
		}

		return {} ;
	}
	qint64 fileSizeInt() const
	{
		return m_fileSizeInt ;
	}
private:
	void parse( const Logger::locale& locale,const QByteArray& allData,int m )
	{
		auto data = allData.mid( 0,m ) ;

		m_title = this->getEntry( "Title:","Type:",data ) ;

		m_quality = this->getEntry( "Quality:","Size:",data ) ;

		auto size = this->getEntry( "Size:","#",data ) ;

		if( !size.isEmpty() ){

			auto s = util::split( size,' ' ) ;

			if( s.size() > 2 ){

				auto e = s[ s.size() - 2 ].mid( 1 ) ;

				m_fileSizeInt    = e.toLongLong() ;
				m_fileSizeString = locale.formattedDataSize( m_fileSizeInt ).toUtf8() ;
			}
		}
	}
	QByteArray getEntry( const QByteArray& start,const QByteArray& end,const QByteArray& s ) const
	{
		auto m = s.indexOf( end ) ;

		if( m != -1 ){

			auto mm = s.mid( 0,m ) ;

			m = mm.indexOf( start ) ;

			if( m != -1 ){

				mm = mm.mid( m + start.size() ) ;

				return mm.trimmed() ;
			}
		}

		return {} ;
	}
	QByteArray m_title ;
	QByteArray m_fileSizeString ;
	QByteArray m_quality ;
	qint64 m_fileSizeInt = 0 ;
};

using Output = engines::engine::baseEngine::filterOutPut ;

class luxFilter : public engines::engine::baseEngine::filterOutPut
{
public:
	luxFilter( const engines::engine& engine ) :
		m_engine( engine ),
		m_callables( luxFilter::meetLocalCondition,luxFilter::skipCondition )
	{
	}
	Output::result formatOutput( const Output::args& args ) const override
	{
		auto data = args.data.toLine() + args.outPut ;

		auto m = data.indexOf( LuxHeader::marker() ) ;

		if( m != -1 ){

			return this->formatOutput( args,data,m ) ;
		}else{
			return { args.outPut,m_engine,m_callables } ;
		}
	}
	Output::result formatOutput( int mm,
				     int m,
				     const QByteArray& ss,
				     const QString& pgr,
				     const Output::args& args,
				     const QByteArray& allData ) const
	{
		const auto& locale = args.locale ;
		const auto& e      = args.outPut ;

		auto a = util::split( ss.mid( 0,mm + 1 ),' ' ) ;

		if( a.size() == 4 ){

			LuxHeader luxHeader( args.locale,allData,m ) ;

			auto eta = a[ 3 ] ;

			auto speed = a[ 0 ] + " " + a[ 1 ] + "/s" ;

			auto perc = a[ 2 ] ;

			auto totalSize = luxHeader.fileSizeInt() ;

			if( totalSize == 0 ){

				m_tmp = pgr.arg( "?","?",perc,speed,eta ).toUtf8() ;
			}else{
				auto ee = QString( perc ).replace( "%","" ) ;

				auto percentage = ee.toDouble() / 100 ;

				auto sizeString = locale.formattedDataSize( totalSize * percentage ) ;

				auto fs = luxHeader.fileSize() ;

				m_tmp = pgr.arg( sizeString,fs,perc,speed,eta ).toUtf8() ;
			}

			mm = e.indexOf( "Merging video parts into " ) ;

			if( mm != -1 ){

				m_tmp = m_tmp + "\n" + e.mid( mm ) ;
			}

			return { m_tmp,m_engine,m_callables } ;
		}else{
			QString s = "?" ;

			m_tmp = pgr.arg( s,s,s,s,s ).toUtf8() ;

			return { m_tmp,m_engine,m_callables } ;
		}
	}
	Output::result formatOutput( const Output::args& args,const QByteArray& allData,int m ) const
	{
		auto mm = allData.lastIndexOf( "-]" ) ;

		if( mm == -1 ){

			mm = allData.lastIndexOf( "=]" ) ;
		}

		if( mm == -1 ){

			mm = allData.lastIndexOf( ">]" ) ;
		}

		if( mm == -1 ){

			return { args.outPut,m_engine,m_callables } ;
		}

		auto ss = allData.mid( mm + 2 ).replace( "p/s","" ) ;

		mm = ss.indexOf( 's' ) ;

		QString pgr = "%1 / %2 (%3) at %4, ETA: %5" ;

		if( mm != -1 ){

			return this->formatOutput( mm,m,ss,pgr,args,allData ) ;

		}else if( ss.startsWith( " ? " ) ){

			QString s = "?" ;

			m_tmp = pgr.arg( s,s,s,s,s ).toUtf8() ;

			return { m_tmp,m_engine,m_callables } ;
		}else{
			return { args.outPut,m_engine,m_callables } ;
		}
	}
	bool meetCondition( const engines::engine::baseEngine::filterOutPut::args& args ) const override
	{
		const auto& e = args.outPut ;
		return e.contains( "] " ) && e.contains( " p/s " ) ;
	}
	const engines::engine& engine() const override
	{
		return m_engine ;
	}
private:	
	static bool meetLocalCondition( const engines::engine&,const QByteArray& e )
	{
		return e.contains( ", ETA: " ) ;
	}
	static bool skipCondition( const engines::engine&,const QByteArray& )
	{
		return false ;
	}
	const engines::engine& m_engine ;
	mutable QByteArray m_tmp ;
	engines::engine::baseEngine::filterOutPut::result::callables m_callables ;
} ;

engines::engine::baseEngine::FilterOutPut lux::filterOutput( int )
{
	const engines::engine& engine = engines::engine::baseEngine::engine() ;

	return { util::types::type_identity< luxFilter >(),engine } ;
}

QString lux::updateTextOnCompleteDownlod( const QString& uiText,
					  const QString& bkText,
					  const QString& dopts,
					  const QString& tabName,
					  const engines::engine::baseEngine::finishedState& f )
{
	using functions = engines::engine::baseEngine ;

	if( f.cancelled() ){

		return functions::updateTextOnCompleteDownlod( bkText,dopts,tabName,f ) ;

	}else if( f.success() ){

		return functions::updateTextOnCompleteDownlod( uiText,dopts,tabName,f ) ;

	}else if( uiText == "invalid URI for request" ){

		return functions::errorString( f,functions::errors::unknownUrl,bkText ) ;

	}else if( uiText == "connect: cannot assign requested address" ){

		return functions::errorString( f,functions::errors::noNetwork,bkText ) ;

	}else if( uiText == "no stream named " ){

		return functions::errorString( f,functions::errors::unknownFormat,bkText ) ;
	}else{
		auto m = engines::engine::baseEngine::processCompleteStateText( f ) ;
		return m + "\n" + bkText ;
	}
}

lux::lux_dlFilter::lux_dlFilter( const engines::engine& engine,int id,QByteArray df ) :
	engines::engine::baseEngine::filter( engine,id ),
	m_banner( ".. " + QObject::tr( "This May Take A Very Long Time" ).toUtf8() + " .." ),
	m_downloadFolder( std::move( df ) )
{
}

const QByteArray& lux::lux_dlFilter::operator()( Logger::Data& e )
{	
	auto allData = e.toLines() ;

	if( e.doneDownloading() ){

		const auto& m = this->doneDownloading( allData ) ;

		if( m.isEmpty() ){

			return this->setFileName( e,allData ) ;
		}else{
			return m ;
		}
	}else{
		const auto& s = e.lastText() ;

		if( s.startsWith( "Elapsed Time:" ) ){

			m_tmp = m_banner + "\n" + s ;
		}else{
			if( m_title.isEmpty() ){

				m_title = LuxHeader( m_locale,allData ).title() ;

				if( m_title.isEmpty() ){

					return m_banner ;
				}
			}

			if( s.contains( "Merging video parts into " ) ){

				m_tmp = m_title + "\n...Merging Video Parts..." ;

			}else if( e.lastLineIsProgressLine() ){

				m_tmp = m_title + "\n" + e.lastText() ;
			}else{
				m_tmp = m_title + "\n" + m_progress.text() ;
			}
		}

		return m_tmp ;
	}
}

lux::lux_dlFilter::~lux_dlFilter()
{
}

const QByteArray& lux::lux_dlFilter::doneDownloading( const QByteArray& allData )
{
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

			m = m_fileName.indexOf( "\n[media-downloader]" ) ;

			if( m != -1 ){

				m_fileName = m_fileName.mid( 0,m ) ;
			}
		}
	}

	m_tmp.clear() ;

	return m_tmp ;
}

QByteArray lux::lux_dlFilter::fileName( const QByteArray& m,const QByteArray& extension )
{
	if( m.endsWith( extension ) ){

		return m ;
	}else{
		return m + extension ;
	}
}

const QByteArray& lux::lux_dlFilter::setFileName( Logger::Data& e,const QByteArray& allData )
{
	auto extension = LuxHeader( m_locale,allData ).extension() ;

	auto fileNameCmd = this->fileNameFromCmd( e ) ;

	if( fileNameCmd.contains( "%(title)s" ) ){

		auto fileName = this->fileName( fileNameCmd,extension ) ;

		if( m_fileName.isEmpty() || m_fileName.contains( "%(title)s" ) ){

			fileNameCmd.replace( "%(title)s",_title( m_title ) ) ;
		}else{
			fileNameCmd.replace( "%(title)s",m_fileName ) ;
		}

		fileNameCmd = this->fileName( fileNameCmd,extension ) ;

		auto old = m_downloadFolder + fileName ;
		auto New = m_downloadFolder + fileNameCmd ;

		if( QFile::exists( New ) ){

			m_tmp = "ERROR: Failed To Rename, Destination Path Already Taken" ;
			m_tmp += "\n" + fileName ;

			e.addFileName( fileName ) ;
		}else{
			utils::qthread::run( [ = ](){ QFile::rename( old,New ) ; } ) ;

			e.addFileName( fileNameCmd ) ;

			m_tmp = fileNameCmd ;
		}

		return m_tmp ;
	}else{
		if( m_fileName.isEmpty() ){

			m_tmp = this->fileName( _title( m_title ),extension ) ;
		}else{
			m_tmp = this->fileName( m_fileName,extension ) ;
		}

		e.addFileName( m_tmp ) ;

		return m_tmp ;
	}
}

QByteArray lux::lux_dlFilter::fileNameFromCmd( const Logger::Data& e )
{	
	class lux
	{
	public:
		lux( QByteArray& m ) : m_title( m )
		{
		}
		bool operator()( int,const QByteArray& s )
		{
			if( this->valid( s ) ){

				return this->setTitle( s ) ;
			}else{
				return false ;
			}
		}
	private:
		bool valid( const QByteArray& s )
		{
			if( s.startsWith( "[media-downloader] cmd: " ) ){

				return s.contains( "-O" ) ;
			}else{
				return false ;
			}
		}
		bool setTitle( const QByteArray& s )
		{
			const auto& mm = util::splitPreserveQuotes( s ) ;

			for( auto it = mm.begin() ; it != mm.end() ; it++ ){

				if( *it == "-O" ){

					it++ ;

					if( it != mm.end() ){

						m_title = it->toUtf8() ;
					}
				}
			}

			return true ;
		}
		QByteArray& m_title ;
	} ;

	QByteArray m ;

	e.forEach( lux( m ) ) ;

	return m ;
}

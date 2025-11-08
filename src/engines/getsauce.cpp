/*
 *
 *  Copyright (c) 2025
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

#include "getsauce.h"
#include "../utility.h"

const char * getsauce::testData()
{
	return R"R(
 Site:      https://hentaimama.io/
 Title:     Cute cat sleeping animation episode 1
 Type:      video
 Stream:

     [0]  -------------------
     Type:            video
     Info:            Mirror 1
     Quality:         unknown
     Size:            186.3 MB
     # download with: get-sauce -s 0 ...


Downloading Cute cat sleeping animation episode 1.mp4 ...   0% |                                        |  [0s:0s]


Downloading Cute cat sleeping animation episode 1.mp4 ...   1% |                                        | (209 kB/s) [9s:14m43s]


Downloading Cute cat sleeping animation episode 1.mp4 ...   2% |                                        | (307 kB/s) [16s:9m54s]


Downloading Cute cat sleeping animation episode 1.mp4 ...   3% |█                                       | (247 kB/s) [27s:12m11s]


Downloading Cute cat sleeping animation episode 1.mp4 ...   4% |█                                       | (275 kB/s) [35s:10m50s]


Downloading Cute cat sleeping animation episode 1.mp4 ...   5% |██                                      | (266 kB/s) [43s:11m6s]


Downloading Cute cat sleeping animation episode 1.mp4 ...   6% |██                                      | (182 kB/s) [55s:16m3s]


Downloading Cute cat sleeping animation episode 1.mp4 ...   7% |██                                      | (295 kB/s) [1m1s:9m47s]


Downloading Cute cat sleeping animation episode 1.mp4 ...   8% |███                                     | (274 kB/s) [1m9s:10m24s]


Downloading Cute cat sleeping animation episode 1.mp4 ...   9% |███                                     | (326 kB/s) [1m15s:8m39s]


Downloading Cute cat sleeping animation episode 1.mp4 ...  10% |████                                    | (285 kB/s) [1m22s:9m47s]


Downloading Cute cat sleeping animation episode 1.mp4 ...  11% |████                                    | (322 kB/s) [1m29s:8m34s]


Downloading Cute cat sleeping animation episode 1.mp4 ...  12% |████                                    | (260 kB/s) [1m38s:10m30s]
)R" ;
}

static QByteArray _getFileName( const QByteArray& allData )
{
	auto mm = allData.lastIndexOf( "Downloading " ) ;

	if( mm == -1 ){

		return {} ;
	}

	const auto data = allData.mid( mm + 12 ) ;

	mm = data.indexOf( " ..." ) ;

	if( mm == -1 ){

		return {} ;
	}

	return data.mid( 0,mm ) ;
}

getsauce::~getsauce()
{
}

getsauce::getsauce( const engines& engines,const engines::engine& engine,QJsonObject&,const QString& df ) :
	engines::engine::baseEngine( engines.Settings(),engine,engines.processEnvironment() ),
	m_engine( engine ),
	m_downloadFolder( df + "/" )
{
}

engines::engine::baseEngine::optionsEnvironment getsauce::setProxySetting( QStringList&,const QString& e )
{
	return { "HTTPS_PROXY",e } ;
}

static bool _meetCondition( const engines::engine&,const QByteArray& e )
{
	return e.contains( "Downloading " ) && e.contains( "% |" ) ;
}

static bool _meetLocalCondition( const engines::engine&,const QByteArray& e )
{
	return e.contains( ") [" ) ;
}

class getsauceHeader
{
public:
	static const char * marker()
	{
		return "download with: " ;
	}
	getsauceHeader( const QByteArray& allData,int m )
	{
		this->parse( allData,m ) ;
	}
	getsauceHeader( const QByteArray& allData )
	{
		this->parse( allData,allData.indexOf( getsauceHeader::marker() ) ) ;
	}
	const QByteArray& title() const
	{
		return m_title ;
	}
	const QByteArray& site() const
	{
		return m_site ;
	}
	const QByteArray& fileSize() const
	{
		return m_fileSizeString ;
	}
private:
	void parse( const QByteArray& allData,int m )
	{
		auto data = allData.mid( 0,m ) ;

		m_title = this->getEntry( "Title:","Type:",data ) ;

		m_quality = this->getEntry( "Quality:","Size:",data ) ;

		m_fileSizeString = this->getEntry( "Size:","#",data ) ;

		m_site = this->getEntry( "Site:","Title:",data ) ;

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
	QByteArray m_site ;
};

using Output = engines::engine::baseEngine::filterOutPut ;

class getsauceFilter : public engines::engine::baseEngine::filterOutPut
{
public:
	getsauceFilter( const engines::engine& engine ) : m_engine( engine )
	{
	}
	Output::result formatOutput( const Output::args& args ) const override
	{
		auto data = args.data.toLine() + args.outPut ;

		auto m = data.indexOf( getsauceHeader::marker() ) ;

		if( m != -1 ){

			return this->formatOutput( args,data,m ) ;
		}else{
			return { args.outPut,m_engine,_meetLocalCondition } ;
		}
	}
	Output::result formatOutput( const Output::args& args,const QByteArray& allData,int ) const
	{
		auto fileName = _getFileName( allData ) ;

		auto title = getsauceHeader( allData ).title() ;

		if( fileName.isEmpty() ){

			if( title.isEmpty() ){

				return { args.outPut,m_engine,_meetLocalCondition } ;
			}else{
				m_tmp = title ;

				return { m_tmp,m_engine,_meetLocalCondition } ;
			}
		}

		auto mm = allData.lastIndexOf( "Downloading " ) ;

		if( mm == -1 ){

			if( title.isEmpty() ){

				return { args.outPut,m_engine,_meetLocalCondition } ;
			}else{
				m_tmp = title ;

				return { m_tmp,m_engine,_meetLocalCondition } ;
			}
		}

		const auto data = allData.mid( mm + 12 ) ;

		mm = data.indexOf( "..." ) ;

		if( mm == -1 ){

			if( title.isEmpty() ){

				return { args.outPut,m_engine,_meetLocalCondition } ;
			}else{
				m_tmp = title ;

				return { m_tmp,m_engine,_meetLocalCondition } ;
			}
		}

		auto percentage = data.mid( mm + 3 ) ;

		mm = percentage.indexOf( "%" ) ;

		if( mm == -1 ){

			if( title.isEmpty() ){

				return { args.outPut,m_engine,_meetLocalCondition } ;
			}else{
				m_tmp = title ;

				return { m_tmp,m_engine,_meetLocalCondition } ;
			}
		}

		percentage = percentage.mid( 0,mm + 1 ).trimmed() ;

		mm = data.indexOf( "(" ) ;

		if( mm == -1 ){

			if( title.isEmpty() ){

				return { args.outPut,m_engine,_meetLocalCondition } ;
			}else{
				m_tmp = title ;

				return { m_tmp,m_engine,_meetLocalCondition } ;
			}
		}

		auto speedAndETA = data.mid( mm ) ;

		mm = speedAndETA.indexOf( "]" ) ;

		if( mm == -1 ){

			if( title.isEmpty() ){

				return { args.outPut,m_engine,_meetLocalCondition } ;
			}else{
				m_tmp = title ;

				return { m_tmp,m_engine,_meetLocalCondition } ;
			}
		}

		speedAndETA = speedAndETA.mid( 0,mm + 1 ) ;

		QByteArray name ;

		if( fileName.isEmpty() ){

			if( !title.isEmpty() ){

				name = title ;
			}
		}else{
			name = fileName ;
		}

		m_tmp = name + "\n" + percentage + " " + speedAndETA ;

		return { m_tmp,m_engine,_meetLocalCondition } ;
	}
	bool meetCondition( const engines::engine::baseEngine::filterOutPut::args& args ) const override
	{
		return _meetCondition( m_engine,args.outPut ) ;
	}
	const engines::engine& engine() const override
	{
		return m_engine ;
	}
private:
	const engines::engine& m_engine ;
	mutable QByteArray m_tmp ;
} ;

engines::engine::baseEngine::DataFilter getsauce::Filter( int id )
{
	return { util::types::type_identity< getsauce::getsauce_dlFilter >(),m_engine,id,m_downloadFolder.toUtf8() } ;
}

engines::engine::baseEngine::FilterOutPut getsauce::filterOutput()
{
	const auto& engine = engines::engine::baseEngine::engine() ;

	return { util::types::type_identity< getsauceFilter >(),engine } ;
}

std::vector<engines::engine::baseEngine::mediaInfo> getsauce::mediaProperties( Logger& l,const QByteArray& e )
{
	QJsonParseError err ;

	auto json = QJsonDocument::fromJson( e,&err ) ;

	if( err.error == QJsonParseError::NoError ){

		auto obj = json.object() ;

		auto streams = obj.value( "streams" ).toObject() ;
		auto site    = obj.value( "site" ).toString() ;
		auto title   = obj.value( "title" ).toString() ;
		auto type    = obj.value( "type" ).toString() ;

		QJsonArray arr ;

		QString id ;

		for( int i = 0 ; ; i++ ){

			id = QString::number( i ) ;

			auto oo = streams.value( id ).toObject() ;

			if( oo.isEmpty() ){

				break ;
			}else{
				oo.insert( "id",id ) ;
				oo.insert( "site",site ) ;
				oo.insert( "main_title",title ) ;
				oo.insert( "main_type",type ) ;

				arr.append( oo ) ;
			}
		}

		return this->mediaProperties( l,arr ) ;
	}else{
		utility::failedToParseJsonData( l,err ) ;

		return {} ;
	}
}

std::vector<engines::engine::baseEngine::mediaInfo> getsauce::mediaProperties( Logger&,const QJsonArray& arr )
{
	std::vector<engines::engine::baseEngine::mediaInfo> ent ;

	Logger::locale locale ;

	for( const auto& it : arr ){

		auto obj = it.toObject() ;

		auto resolution = obj.value( "quality" ).toString() ;
		auto id         = obj.value( "id" ).toString() ;
		auto sizeRaw    = obj.value( "size" ).toInt() ;
		auto size       = locale.formattedDataSize( sizeRaw ) ;
		auto notes      = obj.value( "info" ).toString() ;
		auto extension  = obj.value( "ext" ).toString() ;
		auto title      = obj.value( "title" ).toString() ;

		QString duration ;

		if( title.isEmpty() ){

			title = obj.value( "main_title" ).toString() ;
		}

		QStringList urls ;

		const auto parts = obj.value( "urls" ).toArray() ;

		int count = 0 ;

		for( const auto& it : parts ){

			count++ ;
			urls.append( it.toObject().value( "url" ).toString() ) ;
		}

		if( parts.size() == 1 && extension.isEmpty() ){

			extension = parts[ 0 ].toObject().value( "ext" ).toString() ;
		}

		if( !notes.isEmpty() ){

			notes += "\n" ;
		}

		notes += "Urls Count: " + QString::number( count ) ;

		auto sizeRawInt = QString::number( sizeRaw ) ;

		ent.emplace_back( urls,id,extension,resolution,size,sizeRawInt,notes,duration,title ) ;
	}

	return ent ;
}

bool getsauce::foundNetworkUrl( const QString& s )
{
	utility::CPU cpu ;

	if( utility::platformIsWindows() ){

		if( cpu.x86_64() ){

			return s.endsWith( "Windows_x86_64.zip" ) ;

		}else if( cpu.aarch64() ){

			return s.endsWith( "Windows_arm64.zip" ) ;

		}else if( cpu.x86_32() ){

			return s.endsWith( "Windows_i386.zip" ) ;
		}

	}else if( utility::platformIsLinux() ){

		if( cpu.x86_64() ){

			return s.endsWith( "Linux_x86_64.tar.gz" ) ;

		}else if( cpu.aarch64() ){

			return s.endsWith( "Linux_arm64.tar.gz" ) ;

		}else if( cpu.x86_32() ){

			return s.endsWith( "Linux_i386.tar.gz" ) ;
		}

	}else if( utility::platformIsOSX() ){

		if( cpu.x86_64() ){

			return s.endsWith( "Darwin_x86_64.tar.gz" ) ;

		}else if( cpu.aarch64() ){

			return s.endsWith( "Darwin_arm64.tar.gz" ) ;
		}
	}

	return false ;
}

getsauce::getsauce_dlFilter::getsauce_dlFilter( const engines::engine& engine,int id,QByteArray df ) :
	engines::engine::baseEngine::filter( engine,id ),
	m_banner( ".. " + QObject::tr( "This May Take A Very Long Time" ).toUtf8() + " .." ),
	m_downloadFolder( std::move( df ) )
{
}

const QByteArray& getsauce::getsauce_dlFilter::operator()( Logger::Data& e )
{
	if( e.doneDownloading() ){

		return e.lastText() ;
	}else{
		if( e.lastLineIsProgressLine() ){

			return e.lastText() ;
		}else{
			return m_progress.text() ;
		}
	}
}

getsauce::getsauce_dlFilter::~getsauce_dlFilter()
{
}

const QByteArray& getsauce::getsauce_dlFilter::doneDownloading( const QByteArray& )
{
	m_tmp.clear() ;

	return m_tmp ;
}

const QByteArray& getsauce::getsauce_dlFilter::setFileName( Logger::Data& e,const QByteArray& allData )
{
	m_tmp = _getFileName( allData ) ;

	e.addFileName( m_tmp ) ;

	return m_tmp ;
}

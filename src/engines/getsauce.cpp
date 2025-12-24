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
 Site:      https://blablabla.com
 Title:     Cute Cats Videos
 Type:      video
 Stream:

     [0]  -------------------
     Type:            video
     Info:            Mirror 1
     Quality:         1280x720
     Parts:           562
     Size:            ~ 427.6 MB
     # download with: get-sauce -s 0 ...


Downloading CuteCatsVideos/0.ts ...   0% |                                        |  [0s:0s]


Downloading CuteCatsVideos/0.ts ...   1% |                                        | (1.4 MB/s) [0s:0s]


Downloading CuteCatsVideos/0.ts ...   2% |                                        | (350 kB/s) [0s:3s]


Downloading CuteCatsVideos/0.ts ...   3% |█                                       | (266 kB/s) [0s:4s]


Downloading CuteCatsVideos/0.ts ...   4% |█                                       | (247 kB/s) [0s:5s]


Downloading CuteCatsVideos/0.ts ...   5% |██                                      | (239 kB/s) [0s:5s]


Downloading CuteCatsVideos/0.ts ...   7% |██                                      | (224 kB/s) [0s:5s]


Downloading CuteCatsVideos/0.ts ...   8% |███                                     | (223 kB/s) [0s:5s]


Downloading CuteCatsVideos/0.ts ...   9% |███                                     | (220 kB/s) [0s:5s]


Downloading CuteCatsVideos/0.ts ...  10% |████                                    | (220 kB/s) [0s:5s]


Downloading CuteCatsVideos/0.ts ...  11% |████                                    | (220 kB/s) [0s:5s]


Downloading CuteCatsVideos/0.ts ...  13% |█████                                   | (220 kB/s) [0s:5s]


Downloading CuteCatsVideos/0.ts ...  14% |█████                                   | (220 kB/s) [0s:5s]


Downloading CuteCatsVideos/0.ts ...  15% |██████                                  | (220 kB/s) [0s:5s]


Downloading CuteCatsVideos/0.ts ...  16% |██████                                  | (220 kB/s) [0s:5s]


Downloading CuteCatsVideos/0.ts ...  17% |███████                                 | (220 kB/s) [0s:5s]


Downloading CuteCatsVideos/0.ts ...  19% |███████                                 | (220 kB/s) [1s:5s]


Downloading CuteCatsVideos/0.ts ...  20% |████████                                | (220 kB/s) [1s:4s]


Downloading CuteCatsVideos/0.ts ...  21% |████████                                | (220 kB/s) [1s:4s]


Downloading CuteCatsVideos/0.ts ...  22% |█████████                               | (220 kB/s) [1s:4s]


Downloading CuteCatsVideos/0.ts ...  23% |█████████                               | (220 kB/s) [1s:4s]


Downloading CuteCatsVideos/0.ts ...  25% |██████████                              | (313 kB/s) [1s:3s]


Downloading CuteCatsVideos/0.ts ...  26% |██████████                              | (313 kB/s) [1s:3s]


Downloading CuteCatsVideos/0.ts ...  27% |███████████                             | (313 kB/s) [1s:3s]


Downloading CuteCatsVideos/0.ts ...  28% |███████████                             | (313 kB/s) [1s:3s]


Downloading CuteCatsVideos/0.ts ...  29% |███████████                             | (313 kB/s) [1s:3s]


Downloading CuteCatsVideos/0.ts ...  31% |████████████                            | (313 kB/s) [1s:3s]


Downloading CuteCatsVideos/0.ts ...  32% |████████████                            | (313 kB/s) [1s:2s]


Downloading CuteCatsVideos/0.ts ...  33% |█████████████                           | (313 kB/s) [1s:2s]


Downloading CuteCatsVideos/0.ts ...  34% |█████████████                           | (313 kB/s) [1s:2s]


Downloading CuteCatsVideos/0.ts ...  35% |██████████████                          | (313 kB/s) [1s:2s]


Downloading CuteCatsVideos/0.ts ...  37% |██████████████                          | (313 kB/s) [1s:2s]


Downloading CuteCatsVideos/0.ts ...  38% |███████████████                         | (313 kB/s) [1s:2s]


Downloading CuteCatsVideos/0.ts ...  39% |███████████████                         | (313 kB/s) [1s:2s]


Downloading CuteCatsVideos/0.ts ...  40% |████████████████                        | (313 kB/s) [1s:2s]


Downloading CuteCatsVideos/0.ts ...  41% |████████████████                        | (313 kB/s) [1s:2s]


Downloading CuteCatsVideos/0.ts ...  43% |█████████████████                       | (313 kB/s) [1s:2s]


Downloading CuteCatsVideos/0.ts ...  44% |█████████████████                       | (313 kB/s) [1s:2s]


Downloading CuteCatsVideos/0.ts ...  45% |██████████████████                      | (313 kB/s) [1s:2s]


Downloading CuteCatsVideos/0.ts ...  46% |██████████████████                      | (313 kB/s) [1s:2s]


Downloading CuteCatsVideos/0.ts ...  47% |███████████████████                     | (313 kB/s) [1s:2s]


Downloading CuteCatsVideos/0.ts ...  49% |███████████████████                     | (313 kB/s) [1s:2s]


Downloading CuteCatsVideos/0.ts ...  50% |████████████████████                    | (313 kB/s) [1s:2s]


Downloading CuteCatsVideos/0.ts ...  51% |████████████████████                    | (313 kB/s) [1s:2s]


Downloading CuteCatsVideos/0.ts ...  52% |█████████████████████                   | (313 kB/s) [1s:2s]


Downloading CuteCatsVideos/0.ts ...  53% |█████████████████████                   | (313 kB/s) [1s:2s]


Downloading CuteCatsVideos/0.ts ...  55% |██████████████████████                  | (313 kB/s) [1s:1s]


Downloading CuteCatsVideos/0.ts ...  56% |██████████████████████                  | (313 kB/s) [1s:1s]


Downloading CuteCatsVideos/0.ts ...  57% |███████████████████████                 | (313 kB/s) [1s:1s]


Downloading CuteCatsVideos/0.ts ...  58% |███████████████████████                 | (313 kB/s) [1s:1s]


Downloading CuteCatsVideos/0.ts ...  59% |███████████████████████                 | (313 kB/s) [1s:1s]


Downloading CuteCatsVideos/0.ts ...  61% |████████████████████████                | (534 kB/s) [1s:0s]


Downloading CuteCatsVideos/0.ts ...  62% |████████████████████████                | (534 kB/s) [1s:0s]


Downloading CuteCatsVideos/0.ts ...  82% |█████████████████████████████████       | (534 kB/s) [1s:0s]


Downloading CuteCatsVideos/0.ts ...  83% |█████████████████████████████████       | (534 kB/s) [1s:0s]


Downloading CuteCatsVideos/0.ts ...  85% |██████████████████████████████████      | (534 kB/s) [1s:0s]


Downloading CuteCatsVideos/0.ts ...  86% |██████████████████████████████████      | (534 kB/s) [2s:0s]


Downloading CuteCatsVideos/0.ts ...  87% |███████████████████████████████████     | (534 kB/s) [2s:0s]


Downloading CuteCatsVideos/0.ts ...  88% |███████████████████████████████████     | (534 kB/s) [2s:0s]


Downloading CuteCatsVideos/0.ts ...  89% |███████████████████████████████████     | (534 kB/s) [2s:0s]


Downloading CuteCatsVideos/0.ts ...  91% |████████████████████████████████████    | (534 kB/s) [2s:0s]


Downloading CuteCatsVideos/0.ts ...  92% |████████████████████████████████████    | (607 kB/s) [2s:0s]


Downloading CuteCatsVideos/0.ts ...  93% |█████████████████████████████████████   | (607 kB/s) [2s:0s]


Downloading CuteCatsVideos/0.ts ...  94% |█████████████████████████████████████   | (607 kB/s) [2s:0s]


Downloading CuteCatsVideos/0.ts ...  95% |██████████████████████████████████████  | (607 kB/s) [2s:0s]


Downloading CuteCatsVideos/0.ts ...  97% |██████████████████████████████████████  | (607 kB/s) [2s:0s]


Downloading CuteCatsVideos/0.ts ...  98% |███████████████████████████████████████ | (607 kB/s) [2s:0s]


Downloading CuteCatsVideos/0.ts ...  99% |███████████████████████████████████████ | (607 kB/s) [2s:0s]


Downloading CuteCatsVideos/0.ts ... 100% |████████████████████████████████████████| (576 kB/s)
Downloading CuteCatsVideos/1.ts ...   0% |                                        |  [0s:0s]


Downloading CuteCatsVideos/1.ts ...   3% |█                                       | (1.9 MB/s) [0s:0s]


Downloading CuteCatsVideos/1.ts ...   6% |██                                      | (1.9 MB/s) [0s:0s]


Downloading CuteCatsVideos/1.ts ...   9% |███                                     | (614 kB/s) [0s:0s]


Downloading CuteCatsVideos/1.ts ...  12% |████                                    | (756 kB/s) [0s:0s]


Downloading CuteCatsVideos/1.ts ...  15% |██████                                  | (857 kB/s) [0s:0s]


Downloading CuteCatsVideos/1.ts ...  18% |███████                                 | (669 kB/s) [0s:0s]


Downloading CuteCatsVideos/1.ts ...  21% |████████                                | (746 kB/s) [0s:0s]


Downloading CuteCatsVideos/1.ts ...  24% |█████████                               | (809 kB/s) [0s:0s]


Downloading CuteCatsVideos/1.ts ...  27% |███████████                             | (870 kB/s) [0s:0s]


Downloading CuteCatsVideos/1.ts ...  30% |████████████                            | (930 kB/s) [0s:0s]


Downloading CuteCatsVideos/1.ts ...  33% |█████████████                           | (986 kB/s) [0s:0s]


Downloading CuteCatsVideos/1.ts ...  36% |██████████████                          | (1.0 MB/s) [0s:0s]


Downloading CuteCatsVideos/1.ts ...  39% |███████████████                         | (1.1 MB/s) [0s:0s]


Downloading CuteCatsVideos/1.ts ...  42% |█████████████████                       | (1.1 MB/s) [0s:0s]


Downloading CuteCatsVideos/1.ts ...  45% |██████████████████                      | (1.2 MB/s) [0s:0s]


Downloading CuteCatsVideos/1.ts ...  49% |███████████████████                     | (1.2 MB/s) [0s:0s]


Downloading CuteCatsVideos/1.ts ...  52% |████████████████████                    | (1.2 MB/s) [0s:0s]


Downloading CuteCatsVideos/1.ts ...  55% |██████████████████████                  | (1.3 MB/s) [0s:0s]


Downloading CuteCatsVideos/1.ts ...  58% |███████████████████████                 | (1.3 MB/s) [0s:0s]


Downloading CuteCatsVideos/1.ts ...  61% |████████████████████████                | (1.3 MB/s) [0s:0s]


Downloading CuteCatsVideos/1.ts ...  64% |█████████████████████████               | (1.4 MB/s) [0s:0s]


Downloading CuteCatsVideos/1.ts ...  67% |██████████████████████████              | (1.4 MB/s) [0s:0s]


Downloading CuteCatsVideos/1.ts ...  70% |████████████████████████████            | (1.4 MB/s) [0s:0s]


Downloading CuteCatsVideos/1.ts ...  88% |███████████████████████████████████     | (1.6 MB/s) [0s:0s]


Downloading CuteCatsVideos/1.ts ...  91% |████████████████████████████████████    | (1.6 MB/s) [0s:0s]


Downloading CuteCatsVideos/1.ts ...  95% |██████████████████████████████████████  | (1.6 MB/s) [0s:0s]


Merging into CuteCatsVideos Final.mp4 ...  92% |█████████████████████████████████████   | (94 it/s) [5s:0s]


Merging into CuteCatsVideos Final.mp4 ...  92% |█████████████████████████████████████   | (94 it/s) [5s:0s]


Merging into CuteCatsVideos Final.mp4 ...  93% |█████████████████████████████████████   | (94 it/s) [5s:0s]


Merging into CuteCatsVideos Final.mp4 ...  93% |█████████████████████████████████████   | (94 it/s) [5s:0s]


Merging into CuteCatsVideos Final.mp4 ...  93% |█████████████████████████████████████   | (94 it/s) [5s:0s]


Merging into CuteCatsVideos Final.mp4 ...  93% |█████████████████████████████████████   | (94 it/s) [5s:0s]


Merging into CuteCatsVideos Final.mp4 ...  93% |█████████████████████████████████████   | (94 it/s) [5s:0s]


Merging into CuteCatsVideos Final.mp4 ...  93% |█████████████████████████████████████   | (94 it/s) [5s:0s]


Merging into CuteCatsVideos Final.mp4 ...  94% |█████████████████████████████████████   | (94 it/s) [5s:0s]


Merging into CuteCatsVideos Final.mp4 ...  94% |█████████████████████████████████████   | (94 it/s) [5s:0s]


Merging into CuteCatsVideos Final.mp4 ...  94% |█████████████████████████████████████   | (94 it/s) [5s:0s]


Merging into CuteCatsVideos Final.mp4 ...  94% |█████████████████████████████████████   | (94 it/s) [5s:0s]


Merging into CuteCatsVideos Final.mp4 ...  94% |█████████████████████████████████████   | (94 it/s) [5s:0s]


Merging into CuteCatsVideos Final.mp4 ...  95% |██████████████████████████████████████  | (94 it/s) [5s:0s]


Merging into CuteCatsVideos Final.mp4 ...  95% |██████████████████████████████████████  | (94 it/s) [5s:0s]


Merging into CuteCatsVideos Final.mp4 ...  95% |██████████████████████████████████████  | (94 it/s) [5s:0s]


Merging into CuteCatsVideos Final.mp4 ...  95% |██████████████████████████████████████  | (94 it/s) [5s:0s]


Merging into CuteCatsVideos Final.mp4 ...  95% |██████████████████████████████████████  | (94 it/s) [5s:0s]


Merging into CuteCatsVideos Final.mp4 ...  95% |██████████████████████████████████████  | (94 it/s) [5s:0s]


Merging into CuteCatsVideos Final.mp4 ...  96% |██████████████████████████████████████  | (94 it/s) [5s:0s]


Merging into CuteCatsVideos Final.mp4 ...  96% |██████████████████████████████████████  | (94 it/s) [5s:0s]


Merging into CuteCatsVideos Final.mp4 ...  96% |██████████████████████████████████████  | (94 it/s) [5s:0s]


Merging into CuteCatsVideos Final.mp4 ...  96% |██████████████████████████████████████  | (94 it/s) [5s:0s]


Merging into CuteCatsVideos Final.mp4 ...  96% |██████████████████████████████████████  | (94 it/s) [5s:0s]


Merging into CuteCatsVideos Final.mp4 ...  96% |██████████████████████████████████████  | (94 it/s) [5s:0s]


Merging into CuteCatsVideos Final.mp4 ...  97% |██████████████████████████████████████  | (94 it/s) [5s:0s]


Merging into CuteCatsVideos Final.mp4 ...  97% |██████████████████████████████████████  | (94 it/s) [5s:0s]


Merging into CuteCatsVideos Final.mp4 ...  97% |███████████████████████████████████████ | (94 it/s) [5s:0s]


Merging into CuteCatsVideos Final.mp4 ...  97% |███████████████████████████████████████ | (94 it/s) [5s:0s]


Merging into CuteCatsVideos Final.mp4 ...  97% |███████████████████████████████████████ | (94 it/s) [5s:0s]


Merging into CuteCatsVideos Final.mp4 ...  98% |███████████████████████████████████████ | (94 it/s) [5s:0s]


Merging into CuteCatsVideos Final.mp4 ...  98% |███████████████████████████████████████ | (94 it/s) [5s:0s]


Merging into CuteCatsVideos Final.mp4 ...  98% |███████████████████████████████████████ | (94 it/s) [5s:0s]


Merging into CuteCatsVideos Final.mp4 ...  98% |███████████████████████████████████████ | (94 it/s) [5s:0s]


Merging into CuteCatsVideos Final.mp4 ...  98% |███████████████████████████████████████ | (94 it/s) [5s:0s]


Merging into CuteCatsVideos Final.mp4 ...  98% |███████████████████████████████████████ | (94 it/s) [5s:0s]


Merging into CuteCatsVideos Final.mp4 ...  99% |███████████████████████████████████████ | (94 it/s) [5s:0s]


Merging into CuteCatsVideos Final.mp4 ...  99% |███████████████████████████████████████ | (94 it/s) [5s:0s]


Merging into CuteCatsVideos Final.mp4 ...  99% |███████████████████████████████████████ | (94 it/s) [5s:0s]


Merging into CuteCatsVideos Final.mp4 ...  99% |███████████████████████████████████████ | (94 it/s) [5s:0s]


Merging into CuteCatsVideos Final.mp4 ...  99% |███████████████████████████████████████ | (94 it/s) [5s:0s])R" ;
}

getsauce::~getsauce()
{
}

getsauce::getsauce( const engines& engines,const engines::engine& engine,QJsonObject& ) :
	engines::engine::baseEngine( engines.Settings(),engine,engines.processEnvironment() ),
	m_engine( engine ),
	m_downloadFolder( engines.Settings().downloadFolder() + "/" )
{
}

engines::engine::baseEngine::optionsEnvironment getsauce::setProxySetting( QStringList&,const QString& e )
{
	return { "HTTPS_PROXY",e } ;
}

QString getsauce::updateTextOnCompleteDownlod( const QString& uiText,
					       const QString& bkText,
					       const QString& dopts,
					       const QString& tabName,
					       const finishedState& f )
{
	using functions = engines::engine::baseEngine ;

	if( f.cancelled() ){

		return functions::updateTextOnCompleteDownlod( bkText,dopts,tabName,f ) ;

	}else if( f.success() ){

		return engines::engine::baseEngine::updateTextOnCompleteDownlod( uiText,dopts,tabName,f ) ;

	}else if( uiText.contains( "unsupported protocol scheme" ) || uiText.contains( "URL parse failed" ) ){

		return functions::errorString( f,functions::errors::notSupportedUrl,bkText ) ;

	}else if( uiText.contains( "connection refused" ) || uiText.contains( "TLS handshake timeout" ) ){

		return functions::errorString( f,functions::errors::noNetwork,bkText ) ;

	}else if( uiText.contains( "runtime error" ) ){

		return functions::errorString( f,functions::errors::runtimeError,bkText ) ;
	}else{
		auto m = engines::engine::baseEngine::updateTextOnCompleteDownlod( uiText,dopts,tabName,f ) ;

		return m + "\n" + bkText ;
	}
}

using Output = engines::engine::baseEngine::filterOutPut ;

class getsauceFilter : public engines::engine::baseEngine::filterOutPut
{
public:
	getsauceFilter( const engines::engine& engine,int id ) :
		m_engine( engine ),
		m_id( id ),
		m_callables( getsauceFilter::meetLocalCondition,getsauceFilter::skipCondition )
	{
	}
	Output::result formatOutput( const Output::args& args ) const override
	{
		auto data = args.data.toLine( m_id ) + args.outPut ;

		auto m = data.indexOf( this->marker() ) ;

		if( m != -1 ){

			return this->formatOutput( args,data,m ) ;
		}else{
			return { args.outPut,m_engine,m_callables } ;
		}
	}
	Output::result formatOutput( const Output::args& args,const QByteArray& allData,int ) const
	{
		auto mm = allData.lastIndexOf( "Merging into " ) ;

		if( mm != -1 ){

			return this->parse( true,allData.mid( mm + 13 ),allData,args ) ;
		}else{
			mm = allData.lastIndexOf( "Downloading " ) ;

			if( mm == -1 ){

				return { args.outPut,m_engine,m_callables } ;
			}else{
				return this->parse( false,allData.mid( mm + 12 ),allData,args ) ;
			}
		}
	}
	bool meetCondition( const engines::engine::baseEngine::filterOutPut::args& args ) const override
	{
		const auto& e = args.outPut ;

		if( e.contains( "Merging into " ) && e.contains( " ..." ) ){

			return true ;
		}else{
			return e.contains( "Downloading " ) && e.contains( " ..." ) ;
		}
	}
	const engines::engine& engine() const override
	{
		return m_engine ;
	}
	static bool skipCondition( const engines::engine&,const QByteArray& e )
	{
		return e.trimmed().isEmpty() ;
	}
private:
	static bool meetLocalCondition( const engines::engine&,const QByteArray& e )
	{
		if( e.contains( "Downloading:" ) || e.contains( "Merging:" ) ){

			return true ;
		}else{
			return false ;
		}
	}
	class urlInfo
	{
	public:
		urlInfo( const QByteArray& allData ) :
			m_size( this->getEntry( "Size:",allData ) ),
			m_title( this->getEntry( "Title:",allData ) ),
			m_fileName( this->getFileName( allData ) )
		{
		}
		const QByteArray& size() const
		{
			return m_size ;
		}
		const QByteArray& title() const
		{
			return m_title ;
		}
		const QByteArray& fileName() const
		{
			return m_fileName ;
		}
	private:
		QByteArray getFileName( const QByteArray& start,const QByteArray& allData ) const
		{
			auto mm = allData.lastIndexOf( start ) ;

			if( mm != -1 ){

				const auto data = allData.mid( mm + start.size() ) ;

				mm = data.indexOf( " ..." ) ;

				if( mm != -1 ){

					return data.mid( 0,mm ) ;
				}
			}

			return {} ;
		}
		QByteArray getFileName( const QByteArray& allData ) const
		{
			auto m = this->getFileName( "Merging into ",allData ) ;

			if( m.isEmpty() ){

				m = this->getFileName( "Downloading ",allData ) ;
			}

			return m ;
		}
		QByteArray getEntry( const QByteArray& start,const QByteArray& s ) const
		{
			std::array< const char *,5 > markers{ "Type:","Quality:","Parts:","Size:","#" } ;

			auto m = s.indexOf( start ) ;

			if( m != -1 ){

				auto e = s.mid( m + start.size() ) ;

				for( const auto it : markers ){

					m = e.indexOf( it ) ;

					if( m != -1 ){

						return e.mid( 0,m ).trimmed() ;
					}
				}
			}

			return {} ;
		}
		QByteArray m_size ;
		QByteArray m_title ;
		QByteArray m_fileName ;
	} ;
	Output::result parse( bool merging,
			      const QByteArray& data,
			      const urlInfo& info,
			      const Output::args& args ) const
	{
		auto mm = data.indexOf( "..." ) ;

		if( mm == -1 ){

			return { args.outPut,m_engine,m_callables } ;
		}

		auto percentage = data.mid( mm + 3 ) ;

		mm = percentage.indexOf( "%" ) ;

		if( mm != -1 ){

			percentage = percentage.mid( 0,mm + 1 ).trimmed() ;
		}else{
			percentage = "0%" ;
		}

		QByteArray speedAndETA ;

		mm = data.indexOf( "(" ) ;

		if( mm != -1 ){

			auto a = data.mid( mm ) ;

			mm = a.indexOf( "]" ) ;

			if( mm != -1 ){

				speedAndETA = a.mid( 0,mm + 1 ) ;
			}else{
				speedAndETA = "(0 kB/s) [00:00]" ;
			}
		}else{
			speedAndETA = "(0 kB/s) [00:00]" ;
		}

		QByteArray name ;

		if( info.fileName().isEmpty() ){

			if( info.title().isEmpty() ){

				name = "unknown" ;
			}else{
				name = info.title() ;
			}
		}else{
			if( merging ){

				name = "Merging: " + info.fileName() ;
			}else{
				name = "Downloading: " + info.fileName() ;
			}
		}

		if( info.size().isEmpty() ){

			m_tmp = name + "\n" + percentage + "   " + speedAndETA ;
		}else{
			m_tmp = name + "\n" + info.size() + "   " + percentage + "   " + speedAndETA ;
		}

		return { m_tmp,m_engine,m_callables } ;
	}
	QByteArray marker() const
	{
		return "download with: " ;
	}
	const engines::engine& m_engine ;
	mutable QByteArray m_tmp ;
	int m_id ;
	engines::engine::baseEngine::filterOutPut::result::callables m_callables ;
} ;

bool getsauce::skipCondition( const QByteArray& e )
{
	const auto& engine = engines::engine::baseEngine::engine() ;
	return getsauceFilter::skipCondition( engine,e ) ;
}

engines::engine::baseEngine::DataFilter getsauce::Filter( int id )
{
	auto m = util::types::type_identity< getsauce::getsauce_dlFilter >() ;

	return { m,m_engine,id,m_downloadFolder.toUtf8() } ;
}

engines::engine::baseEngine::FilterOutPut getsauce::filterOutput( int id )
{
	const auto& engine = engines::engine::baseEngine::engine() ;

	return { util::types::type_identity< getsauceFilter >(),engine,id } ;
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

		if( utility::stringConstants::downloadFailed( e.lastText() ) ){

			const auto m = e.toStringList() ;

			for( auto it = m.rbegin() ; it != m.rend() ; it++ ){

				const QByteArray& w = *it ;

				if( w.contains( "unsupported protocol scheme" ) ){

					m_tmp = "unsupported protocol scheme" ;

					return m_tmp ;

				}else if( w.contains( "connection refused" ) || w.contains( "read: connection reset by peer" ) ){

					m_tmp = "connection refused" ;

					return m_tmp ;

				}else if( w.contains( "panic" ) ){

					m_tmp = "runtime error" ;

					return m_tmp ;

				}else if( w.contains( "TLS handshake timeout" ) ){

					m_tmp = "TLS handshake timeout" ;

					return m_tmp ;

				}else if( w.contains( "URL parse failed" ) ){

					m_tmp = "URL parse failed" ;

					return m_tmp ;
				}
			}
		}

		const auto& m = e.fileNames() ;

		if( m.size() ){

			m_tmp = m.back() ;

			return m_tmp ;
		}else{
			m_tmp = e.lastText() ;

			m_tmp.replace( "[media-downloader]","" ) ;
			m_tmp.replace( "Merging: ","" ) ;
			m_tmp.replace( "Downloading: ","" ) ;

			return m_tmp ;
		}

	}else if( this->progressLine( e ) ){

		auto m = e.lastText().indexOf( "\n" ) ;

		if( m != -1 ){

			auto s = e.lastText().mid( 0,m ) ;

			s.replace( "Downloading: ","" ) ;
			s.replace( "Merging: ","" ) ;

			e.addFileName( s ) ;
		}

		const auto& mm = e.fileNames() ;

		if( mm.size() > 1 ){

			auto a = QObject::tr( "%1 Already Downloaded" ) ;
			auto b = QString::number( mm.size() - 1 ) ;

			m_tmp = a.arg( b ).toUtf8() + "\n" + e.lastText() ;

			return m_tmp ;
		}else{
			const auto& m = e.lastText() ;

			if( m.startsWith( "Elapsed Time: " ) ){

				m_tmp = m_banner + "\n" + m ;

				return m_tmp ;
			}else{
				return m ;
			}
		}
	}else{
		return m_progress.text() ;
	}
}

getsauce::getsauce_dlFilter::~getsauce_dlFilter()
{
}

bool getsauce::getsauce_dlFilter::progressLine( const Logger::Data& e )
{
	if( e.lastLineIsProgressLine() ){

		return true ;
	}else{
		return false ;
	}
}

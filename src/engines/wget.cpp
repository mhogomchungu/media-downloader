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

#include "wget.h"
#include "../utility.h"

const char * wget::testData()
{
	return R"R(--2022-10-05 11:56:40--  https://github.com/mhogomchungu/sirikali/releases/download/1.5.0/SiriKali-1.5.0.setup.exe
Resolving github.com (github.com)... 140.82.121.3
Connecting to github.com (github.com)|140.82.121.3|:443... connected.
HTTP request sent, awaiting response... 302 Found
Location: https://objects.githubusercontent.com/github-production-release-asset-2e65be/65425538/6cb063b1-2305-4976-8e9c-5c80d3bcf3d6?X-Amz-Algorithm=AWS4-HMAC-SHA256&X-Amz-Credential=AKIAIWNJYAX4CSVEH53A%2F20221005%2Fus-east-1%2Fs3%2Faws4_request&X-Amz-Date=20221005T085725Z&X-Amz-Expires=300&X-Amz-Signature=864c663c14e313927c42dae49a60622869b1b3067f41d93f6773aedf79f18fed&X-Amz-SignedHeaders=host&actor_id=0&key_id=0&repo_id=65425538&response-content-disposition=attachment%3B%20filename%3DSiriKali-1.5.0.setup.exe&response-content-type=application%2Foctet-stream [following]
--2022-10-05 11:56:45--  https://objects.githubusercontent.com/github-production-release-asset-2e65be/65425538/6cb063b1-2305-4976-8e9c-5c80d3bcf3d6?X-Amz-Algorithm=AWS4-HMAC-SHA256&X-Amz-Credential=AKIAIWNJYAX4CSVEH53A%2F20221005%2Fus-east-1%2Fs3%2Faws4_request&X-Amz-Date=20221005T085725Z&X-Amz-Expires=300&X-Amz-Signature=864c663c14e313927c42dae49a60622869b1b3067f41d93f6773aedf79f18fed&X-Amz-SignedHeaders=host&actor_id=0&key_id=0&repo_id=65425538&response-content-disposition=attachment%3B%20filename%3DSiriKali-1.5.0.setup.exe&response-content-type=application%2Foctet-stream
Resolving objects.githubusercontent.com (objects.githubusercontent.com)... 185.199.111.133, 185.199.108.133, 185.199.109.133, ...
Connecting to objects.githubusercontent.com (objects.githubusercontent.com)|185.199.111.133|:443... connected.
HTTP request sent, awaiting response... 200 OK
Length: 12486371 (12M) [application/octet-stream]
Saving to: ‘SiriKali-1.5.0.setup.exe’


SiriKali-1.5.0.setu   0%[                    ]       0  --.-KB/s
SiriKali-1.5.0.setu   0%[                    ]  71.39K   316KB/s
SiriKali-1.5.0.setu   1%[                    ] 231.39K   529KB/s
SiriKali-1.5.0.setu   3%[                    ] 407.39K   630KB/s
SiriKali-1.5.0.setu   4%[                    ] 583.39K   688KB/s
SiriKali-1.5.0.setu   5%[>                   ] 705.20K   629KB/s
SiriKali-1.5.0.setu   7%[>                   ] 945.20K   710KB/s
SiriKali-1.5.0.setu   9%[>                   ]   1.09M   730KB/s
SiriKali-1.5.0.setu  10%[=>                  ]   1.28M   756KB/s
SiriKali-1.5.0.setu  12%[=>                  ]   1.47M   778KB/s
SiriKali-1.5.0.setu  14%[=>                  ]   1.69M   807KB/s
SiriKali-1.5.0.setu  15%[==>                 ]   1.80M   722KB/s
SiriKali-1.5.0.setu  17%[==>                 ]   2.12M   789KB/s
SiriKali-1.5.0.setu  19%[==>                 ]   2.34M   808KB/s
SiriKali-1.5.0.setu  20%[===>                ]   2.49M   805KB/s    eta 12s
SiriKali-1.5.0.setu  23%[===>                ]   2.75M   833KB/s    eta 12s
SiriKali-1.5.0.setu  24%[===>                ]   2.92M   848KB/s    eta 12s
SiriKali-1.5.0.setu  25%[====>               ]   3.08M   857KB/s    eta 12s
SiriKali-1.5.0.setu  27%[====>               ]   3.29M   866KB/s    eta 12s
SiriKali-1.5.0.setu  29%[====>               ]   3.53M   889KB/s    eta 10s
SiriKali-1.5.0.setu  31%[=====>              ]   3.78M   940KB/s    eta 10s
SiriKali-1.5.0.setu  33%[=====>              ]   3.96M   928KB/s    eta 10s
SiriKali-1.5.0.setu  34%[=====>              ]   4.06M   896KB/s    eta 10s
SiriKali-1.5.0.setu  34%[=====>              ]   4.10M   841KB/s    eta 10s
SiriKali-1.5.0.setu  34%[=====>              ]   4.11M   709KB/s    eta 11s
SiriKali-1.5.0.setu  35%[======>             ]   4.25M   690KB/s    eta 11s
SiriKali-1.5.0.setu  36%[======>             ]   4.30M   548KB/s    eta 12s
SiriKali-1.5.0.setu  36%[======>             ]   4.33M   555KB/s    eta 12s
SiriKali-1.5.0.setu  36%[======>             ]   4.39M   472KB/s    eta 12s
SiriKali-1.5.0.setu  38%[======>             ]   4.56M   468KB/s    eta 12s
SiriKali-1.5.0.setu  38%[======>             ]   4.61M   441KB/s    eta 13s
SiriKali-1.5.0.setu  39%[======>             ]   4.67M   405KB/s    eta 13s
SiriKali-1.5.0.setu  39%[======>             ]   4.73M   339KB/s    eta 14s
SiriKali-1.5.0.setu  40%[=======>            ]   4.86M   335KB/s    eta 14s
SiriKali-1.5.0.setu  42%[=======>            ]   5.07M   340KB/s    eta 14s
SiriKali-1.5.0.setu  43%[=======>            ]   5.19M   326KB/s    eta 14s
SiriKali-1.5.0.setu  44%[=======>            ]   5.31M   318KB/s    eta 14s
SiriKali-1.5.0.setu  45%[========>           ]   5.45M   285KB/s    eta 12s
SiriKali-1.5.0.setu  47%[========>           ]   5.65M   299KB/s    eta 12s
SiriKali-1.5.0.setu  49%[========>           ]   5.86M   328KB/s    eta 12s
SiriKali-1.5.0.setu  51%[=========>          ]   6.08M   401KB/s    eta 12s
SiriKali-1.5.0.setu  51%[=========>          ]   6.19M   395KB/s    eta 12s
SiriKali-1.5.0.setu  52%[=========>          ]   6.28M   499KB/s    eta 10s
SiriKali-1.5.0.setu  53%[=========>          ]   6.39M   511KB/s    eta 10s
SiriKali-1.5.0.setu  54%[=========>          ]   6.51M   531KB/s    eta 10s
SiriKali-1.5.0.setu  55%[==========>         ]   6.62M   529KB/s    eta 10s
SiriKali-1.5.0.setu  56%[==========>         ]   6.73M   546KB/s    eta 10s
SiriKali-1.5.0.setu  57%[==========>         ]   6.84M   551KB/s    eta 9s
SiriKali-1.5.0.setu  58%[==========>         ]   7.01M   678KB/s    eta 9s
SiriKali-1.5.0.setu  60%[===========>        ]   7.17M   669KB/s    eta 9s
SiriKali-1.5.0.setu  61%[===========>        ]   7.36M   692KB/s    eta 9s
SiriKali-1.5.0.setu  63%[===========>        ]   7.51M   674KB/s    eta 9s
SiriKali-1.5.0.setu  65%[============>       ]   7.85M   743KB/s    eta 7s
SiriKali-1.5.0.setu  67%[============>       ]   8.07M   748KB/s    eta 7s
SiriKali-1.5.0.setu  68%[============>       ]   8.21M   709KB/s    eta 7s
SiriKali-1.5.0.setu  70%[=============>      ]   8.41M   713KB/s    eta 7s
SiriKali-1.5.0.setu  72%[=============>      ]   8.65M   746KB/s    eta 7s
SiriKali-1.5.0.setu  74%[=============>      ]   8.81M   781KB/s    eta 5s
SiriKali-1.5.0.setu  75%[==============>     ]   8.94M   784KB/s    eta 5s
SiriKali-1.5.0.setu  76%[==============>     ]   9.06M   774KB/s    eta 5s
SiriKali-1.5.0.setu  76%[==============>     ]   9.15M   778KB/s    eta 5s
SiriKali-1.5.0.setu  78%[==============>     ]   9.29M   784KB/s    eta 5s
SiriKali-1.5.0.setu  78%[==============>     ]   9.40M   784KB/s    eta 4s
SiriKali-1.5.0.setu  80%[===============>    ]   9.56M   782KB/s    eta 4s
SiriKali-1.5.0.setu  81%[===============>    ]   9.76M   801KB/s    eta 4s
SiriKali-1.5.0.setu  84%[===============>    ]  10.07M   832KB/s    eta 4s
SiriKali-1.5.0.setu  85%[================>   ]  10.23M   827KB/s    eta 4s
SiriKali-1.5.0.setu  88%[================>   ]  10.49M   836KB/s    eta 2s
SiriKali-1.5.0.setu  89%[================>   ]  10.67M   834KB/s    eta 2s
SiriKali-1.5.0.setu  91%[=================>  ]  10.90M   848KB/s    eta 2s
SiriKali-1.5.0.setu  92%[=================>  ]  11.03M   840KB/s    eta 2s
SiriKali-1.5.0.setu  94%[=================>  ]  11.21M   837KB/s    eta 2s
SiriKali-1.5.0.setu  94%[=================>  ]  11.28M   784KB/s    eta 1s
SiriKali-1.5.0.setu  96%[==================> ]  11.45M   792KB/s    eta 1s
SiriKali-1.5.0.setu  97%[==================> ]  11.60M   803KB/s    eta 1s
SiriKali-1.5.0.setu  98%[==================> ]  11.74M   809KB/s    eta 1s
SiriKali-1.5.0.setu  99%[==================> ]  11.87M   818KB/s    eta 1s
SiriKali-1.5.0.setu 100%[===================>]  11.91M   826KB/s    in 19s

2022-10-05 11:57:06 (654 KB/s) - ‘SiriKali-1.5.0.setup.exe’ saved [12486371/12486371])R" ;
}

void wget::init( const QString& name,
		 const QString& configFileName,
		 Logger& logger,
		 const engines::enginePaths& enginePath )
{
	auto m = enginePath.enginePath( configFileName ) ;

	if( !QFile::exists( m ) ){

		QJsonObject mainObj ;

		mainObj.insert( "ShowListTableBoundary",QJsonObject() ) ;

		mainObj.insert( "DefaultListCmdOptions",QJsonArray() ) ;

		mainObj.insert( "DownloadUrl","" ) ;

		mainObj.insert( "Name",name ) ;

		mainObj.insert( "CookieArgument","" ) ;

		mainObj.insert( "DefaultDownLoadCmdOptions",[](){

			QJsonArray arr ;
			arr.append( "--progress=bar:force" ) ;
			return arr ;
		}() ) ;

		mainObj.insert( "SkipLineWithText",QJsonArray() ) ;

		utility::addJsonCmd json( mainObj ) ;

		json.add( { { "Generic" },{ { "x86",name,{ name } },
					    { "amd64",name,{ name } } } } ) ;

		auto exe = name + ".exe" ;

		json.add( { { "Windows" },{ { "x86",exe,{ exe } },
					    { "amd64",exe,{ exe } } } } ) ;

		json.done() ;

		mainObj.insert( "RemoveText",QJsonArray() ) ;

		mainObj.insert( "SplitLinesBy",QJsonArray() ) ;

		mainObj.insert( "PlayListIdArguments",QJsonArray() ) ;

		mainObj.insert( "RequiredMinimumVersionOfMediaDownloader","2.2.0" ) ;

		mainObj.insert( "PlaylistItemsArgument","" ) ;

		mainObj.insert( "ControlJsonStructure",[](){

			QJsonObject obj ;

			obj.insert( "lhs",[](){

				QJsonObject obj ;

				obj.insert( "containsAny",[](){

					QJsonArray arr ;

					arr.append( "%[" ) ;
					arr.append( "% [" ) ;
					arr.append( "<=>" ) ;

					return arr ;
				}() ) ;

				return obj ;
			}() ) ;

			return obj ;
		}() ) ;

		mainObj.insert( "VersionArgument","--version" ) ;

		mainObj.insert( "OptionsArgument","" ) ;

		mainObj.insert( "BackendPath",utility::stringConstants::defaultPath() ) ;

		mainObj.insert( "VersionStringLine",0 ) ;

		mainObj.insert( "VersionStringPosition",2 ) ;

		mainObj.insert( "BatchFileArgument","-i" ) ;

		mainObj.insert( "CanDownloadPlaylist",false ) ;

		mainObj.insert( "LikeYoutubeDl",false ) ;

		mainObj.insert( "ReplaceOutputWithProgressReport",false ) ;

		engines::file( m,logger ).write( mainObj ) ;
	}
}

wget::wget( const engines& e,const engines::engine& s,QJsonObject& ) :
	engines::engine::baseEngine( e.Settings(),s,e.processEnvironment() )
{
}

void wget::updateDownLoadCmdOptions( const engines::engine::baseEngine::updateOpts& s,
				     bool e,
				     const QStringList& m )
{
	if( !s.ourOptions.contains( "--progress=bar:force" ) ){

		s.ourOptions.append( "--progress=bar:force" ) ;
	}

	engines::engine::baseEngine::updateDownLoadCmdOptions( s,e,m ) ;
}

engines::engine::baseEngine::DataFilter wget::Filter( int id )
{
	const auto& engine = engines::engine::baseEngine::engine() ;

	return { util::types::type_identity< wget::wgetFilter >(),engine,id } ;
}

engines::engine::baseEngine::optionsEnvironment wget::setProxySetting( QStringList& e,const QString& s )
{
	e.append( "-e" ) ;
	e.append( "use_proxy=yes" ) ;	

	if( s.contains( "@" ) ){

		auto m = engines::proxySettings( s ).networkProxy() ;

		e.append( "-e" ) ;
		e.append( "http_proxy=" + m.hostName() + ":" + QString::number( m.port() ) ) ;

		e.append( "-e" ) ;
		e.append( "https_proxy=" + m.hostName() + ":" + QString::number( m.port() ) ) ;

		e.append( "--proxy-user=" + m.user() ) ;
		e.append( "--proxy-password=" + m.password() ) ;
	}else{
		e.append( "-e" ) ;
		e.append( "http_proxy=" + s ) ;
		e.append( "-e" ) ;
		e.append( "https_proxy=" + s ) ;
	}

	return {} ;
}

QString wget::updateTextOnCompleteDownlod( const QString& uiText,
					   const QString& bkText,
					   const QString& dopts,
					   const QString& tabName,
					   const finishedState& f )
{
	if( f.cancelled() ){

		return engines::engine::baseEngine::updateTextOnCompleteDownlod( bkText,dopts,tabName,f ) ;

	}else if( f.success() ){

		const auto& m = [ & ](){

			if( uiText.isEmpty() ){

				return bkText ;
			}else{
				return uiText ;
			}
		}() ;

		return engines::engine::baseEngine::updateTextOnCompleteDownlod( m,dopts,tabName,f ) ;
	}else{
		using functions = engines::engine::baseEngine ;

		if( uiText.contains( "failed: Temporary failure in name resolution" ) ){

			return functions::errorString( f,functions::errors::noNetwork,bkText ) ;

		}else if( uiText.contains( "failed: Name or service not known" ) ){

			return functions::errorString( f,functions::errors::unknownUrl,bkText ) ;
		}else{
			auto m = engines::engine::baseEngine::processCompleteStateText( f ) ;
			return m + "\n" + bkText ;
		}
	}
}

wget::~wget()
{
}

wget::wgetFilter::wgetFilter( const engines::engine& engine,int id ) :
	engines::engine::baseEngine::filter( engine,id )
{
}

static QByteArray _uiText( const QByteArray& e,const QByteArray& p,const QByteArray& length )
{
	QString result = "\n" ;

	auto m = util::split( e,' ',true ) ;

	auto size = m.size() ;

	if( size > 1 ){

		result += QObject::tr( "Speed:" ) + " " + m[ 1 ] ;
	}

	if( size > 3 ){

		auto w = m[ 3 ] ;

		auto t = QObject::tr( "Time Left" ) ;

		if( result.endsWith( ", " ) ){

			result += t + ": " + w ;
		}else{
			result += ", " + t + ": " + w ;
		}

		if( size > 4 ){

			result += " " + m[ 4 ] ;
		}
	}

	if( size ){

		auto w = m[ 0 ] ;

		auto t = QObject::tr( "Downloaded" ) ;

		if( result.endsWith( ", " ) ){

			result += t + ": " + w ;
		}else{
			result += ", " + t + ": " + w ;
		}

		if( !length.isEmpty() && length != "N/A" ){

			result += " / " + length ;
		}

		if( !p.isEmpty() ){

			result += " (" + p + ")" ;
		}
	}

	return result.toUtf8() ;
}

const QByteArray& wget::wgetFilter::operator()( Logger::Data& e )
{
	if( e.doneDownloading() ){

		auto m = e.toLine() ;

		if( m.contains( "failed: Temporary failure in name resolution" ) ){

			m_tmp = "failed: Temporary failure in name resolution" ;

			return m_tmp ;

		}else if( m.contains( "failed: Name or service not known" ) ){

			m_tmp = "failed: Name or service not known" ;

			return m_tmp ;
		}

		e.addFileName( m_title ) ;

		return m_title ;
	}

	auto line = e.toLines() ;

	if( !line.contains( "Saving to: " ) ){

		return m_preProcessing.text() ;
	}

	if( m_title.isEmpty() || m_length.isEmpty() ){

		const auto lines = util::split( line,'\n' ) ;

		if( m_title.isEmpty() ){

			for( const auto& it : lines ){

				if( it.startsWith( "Saving to: " ) ){

					m_title = it.mid( 11 ) ;
					m_title.replace( "‘","" ) ;
					m_title.replace( "’","" ) ;
					m_title.replace( "'","" ) ;
					m_title.replace( "'","" ) ;

					break ;
				}
			}
		}

		if( m_length.isEmpty() ){

			for( const auto& it : lines ){

				if( it.startsWith( "Length: " ) ){

					auto m = util::split( it,' ',true ) ;

					if( m.size() > 2 ){

						if( m[ 1 ] == "unspecified" ){

							m_length = "N/A" ;
						}else{
							m_length = m[ 2 ].toUtf8() ;

							m_length.replace( "(","" ) ;
							m_length.replace( ")","" ) ;
							m_length.replace( ",","" ) ;
						}
					}

					break ;
				}
			}
		}
	}

	if( e.lastLineIsProgressLine() ){

		if( m_title.isEmpty() ){

			return e.lastText() ;
		}else{
			const auto m = e.lastText() ;

			auto s = m.indexOf( "%[" ) ;

			if( s != -1 ){

				auto l = m.mid( s ) ;

				for( int i = s - 1 ; i >= 0 ; i-- ){

					if( m[ i ] != ' ' ){

						l.prepend( m[ i ] ) ;
					}else{
						break ;
					}
				}

				auto a = l.indexOf( '[' ) ;

				auto b = l.indexOf( ']' ) ;

				if( a != -1 && b != -1 ){

					auto aa = l.mid( 0,a ) ;

					auto bb = l.mid( b + 1 ) ;

					m_tmp = m_title + _uiText( bb,aa,m_length ) ;
				}else{
					m_tmp = m_title + "\n" + m_preProcessing.text() ;
				}
			}else{
				auto b = m.indexOf( ']' ) ;

				if( b != -1 ){

					m_tmp = m_title + _uiText( m.mid( b + 1 ),"",m_length ) ;
				}else{
					return m_preProcessing.text() ;
				}
			}

			return m_tmp ;
		}
	}else{
		if( m_title.isEmpty() ){

			return m_preProcessing.text() ;
		}else{
			m_tmp = m_title + "\n" + m_preProcessing.text() ;
			return m_tmp ;
		}
	}
}

wget::wgetFilter::~wgetFilter()
{
}

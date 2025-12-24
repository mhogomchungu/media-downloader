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

#include "quickjs.h"
#include "../utility.h"

QJsonObject quickjs::init( const QString& name,
			   const QString& configFileName,
			   Logger& logger,
			   const engines::enginePaths& enginePath )
{
	auto m = enginePath.enginePath( configFileName ) ;

	if( QFile::exists( m ) ){

		return QJsonObject() ;
	}

	QJsonObject mainObj ;

	utility::addJsonCmd json( mainObj ) ;

	json.add( { { "Generic" },{ { "x86","qjs",{ "qjs" } },
				    { "amd64","qjs",{ "qjs" } } } } ) ;

	json.add( { { "Windows" },{ { "win7x86","qjs.exe",{ "qjs.exe" } },
				    { "win7amd64","qjs.exe",{ "qjs.exe" } },
				    { "x86","qjs.exe",{ "qjs.exe" } },
				    { "amd64","qjs.exe",{ "qjs.exe" } } } } ) ;

	json.done() ;

	mainObj.insert( "Version","1" ) ;

	mainObj.insert( "DownloadUrl","https://bellard.org/quickjs/binary_releases/LATEST.json" ) ;

	mainObj.insert( "DownloadUrlWin7","" ) ;

	mainObj.insert( "AutoUpdate",true ) ;

	mainObj.insert( "Name",name ) ;

	mainObj.insert( "VersionArgument","--version" ) ;

	mainObj.insert( "BackendPath",utility::stringConstants::defaultPath() ) ;

	mainObj.insert( "VersionStringLine",1 ) ;

	mainObj.insert( "VersionStringPosition",2 ) ;

	mainObj.insert( "LikeYoutubeDl",false ) ;

	engines::file( m,logger ).write( mainObj ) ;

	return mainObj ;
}

quickjs::~quickjs()
{
}

engines::metadata quickjs::parseJsonDataFromGitHub( const QJsonDocument& e )
{
	auto version = e.object().value( "version" ).toString() ;

	if( version.isEmpty() ){

		return {} ;
	}else{
		QString fileName ;
		QString url ;

		if( utility::platformIsWindows() ){

			if( utility::CPU().x86_32() ){

				fileName = "quickjs-win-i686-%1.zip" ;

				url = "https://bellard.org/quickjs/binary_releases/" ;
			}else{
				fileName = "quickjs-win-x86_64-%1.zip" ;

				url = "https://bellard.org/quickjs/binary_releases/" ;
			}
		}else{
			if( utility::CPU().x86_32() ){

				fileName = "quickjs-linux-x86_64-%1.zip" ;

				url = "https://bellard.org/quickjs/binary_releases/" ;
			}else{
				fileName = "quickjs-linux-x86_64-%1.zip" ;

				url = "https://bellard.org/quickjs/binary_releases/" ;
			}
		}

		QJsonObject obj ;

		fileName = fileName.arg( version ) ;
		obj.insert( "browser_download_url",url + fileName ) ;
		obj.insert( "name",fileName ) ;
		obj.insert( "digest","" ) ;
		obj.insert( "size",0 ) ;

		return obj ;
	}
}

engines::engine::baseEngine::removeFilesStatus quickjs::removeFiles( const QStringList& e,const QString& a )
{
	auto m = e ;

	if( utility::platformIsLinux() ){

		m.append( a + "/run-test262" ) ;

		return engines::engine::baseEngine::removeFiles( m,a ) ;
	}else{
		m.append( a + "/libwinpthread-1.dll" ) ;

		return engines::engine::baseEngine::removeFiles( m,a ) ;
	}
}

bool quickjs::foundNetworkUrl( const QString& s )
{
	utility::CPU cpu ;

	if( utility::platformIsWindows() ){

		if( cpu.x86_64() ){

			return s.startsWith( "quickjs-win-i686" ) && s.endsWith( ".zip" ) ;

		}else if( cpu.x86_32() ){

			return s.startsWith( "quickjs-win-x86_64" ) && s.endsWith( ".zip" ) ;
		}

	}else if( utility::platformIsLinux() ){

		if( cpu.x86_64() ){

			return s.startsWith( "quickjs-linux-x86_64" ) && s.endsWith( ".zip" ) ;

		}else if( cpu.x86_32() ){

			return s.startsWith( "quickjs-linux-i686" ) && s.endsWith( ".zip" ) ;
		}
	}

	return false ;
}

QString quickjs::parseVersionInfo( const utils::qprocess::outPut& r )
{
	auto s = util::split( r.stdOut,'\n' ) ;

	if( s.size() ){

		auto e = s[ 0 ] + "\n" + s[ 0 ] ;

		e.replace( "-","." ) ;

		return e ;
	}else{
		return {} ;
	}
}

quickjs::quickjs( const engines& e,const engines::engine& s,QJsonObject& ) :
	engines::engine::baseEngine( e.Settings(),s,e.processEnvironment() )
{
	if( utility::platformisFlatPak() ){

		auto path = e.Settings().flatpakIntance().appDataLocation() + "/bin/qjs";

		if( QFile::exists( path ) ){

			QFile::remove( path ) ;
		}
	}
}

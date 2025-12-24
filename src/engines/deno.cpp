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

#include "deno.h"
#include "../utility.h"

QJsonObject deno::init( const QString& name,
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

	json.add( { { "Generic" },{ { "x86","deno",{ "deno" } },
				    { "amd64","deno",{ "deno" } },
				    { "aarch64","deno",{ "deno" } } } } ) ;

	json.add( { { "Windows" },{ { "win7x86","deno.exe",{ "deno.exe" } },
				    { "win7amd64","deno.exe",{ "deno.exe" } },
				    { "x86","deno.exe",{ "deno.exe" } },
				    { "amd64","deno.exe",{ "deno.exe" } } } } ) ;

	json.add( { { "MacOS" },{ { "amd64","deno",{ "deno" } },
				  { "aarch64","deno",{ "deno" } } } } ) ;

	json.done() ;

	mainObj.insert( "Version","1" ) ;

	mainObj.insert( "DownloadUrl","https://api.github.com/repos/denoland/deno/releases/latest" ) ;

	mainObj.insert( "DownloadUrlWin7","" ) ;

	mainObj.insert( "AutoUpdate",true ) ;

	mainObj.insert( "Name",name ) ;

	mainObj.insert( "VersionArgument","-version" ) ;

	mainObj.insert( "BackendPath",utility::stringConstants::defaultPath() ) ;

	mainObj.insert( "VersionStringLine",0 ) ;

	mainObj.insert( "VersionStringPosition",1 ) ;

	mainObj.insert( "LikeYoutubeDl",false ) ;

	engines::file( m,logger ).write( mainObj ) ;

	return mainObj ;
}

deno::~deno()
{
}

bool deno::foundNetworkUrl( const QString& s )
{
	utility::CPU cpu ;

	if( utility::platformIsWindows() ){

		if( cpu.x86_64() ){

			return s.contains( "deno-x86_64-pc-windows-msvc.zip" ) ;
		}

	}else if( utility::platformisFlatPak() ){

		return false ;

	}else if( utility::platformIsLinux() ){

		if( cpu.x86_64() ){

			return s.contains( "deno-x86_64-unknown-linux-gnu.zip" ) ;

		}else if( cpu.aarch64() ){

			return s.contains( "deno-aarch64-unknown-linux-gnu.zip" ) ;
		}

	}else if( utility::platformIsOSX() ){

		if( cpu.x86_64() ){

			return s.contains( "deno-x86_64-apple-darwin.zip" ) ;

		}else if( cpu.aarch64() ){

			return s.contains( "deno-aarch64-apple-darwin.zip" ) ;
		}
	}

	return false ;
}

deno::deno( const engines& e,const engines::engine& s,QJsonObject& ) :
	engines::engine::baseEngine( e.Settings(),s,e.processEnvironment() )
{
}

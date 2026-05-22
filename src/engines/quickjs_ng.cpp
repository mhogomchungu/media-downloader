/*
 *
 *  Copyright (c) 2026
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

#include "quickjs_ng.h"
#include "../utility.h"

void quickjs_ng::init( Logger& logger,const engines::enginePaths& enginePath )
{
	auto m = enginePath.enginePath( "quickjs_ng.json" ) ;

	if( QFile::exists( m ) ){

		return ;
	}

	QJsonObject mainObj ;

	utility::addJsonCmd json( mainObj ) ;

	json.add( { { "Generic" },{ { "x86","qjs-linux-x86",{ "qjs-linux-x86" } },
				    { "aarch64","qjs-linux-aarch64",{ "qjs-linux-aarch64" } },
				    { "amd64","qjs-linux-x86_64",{ "qjs-linux-x86_64" } } } } ) ;


	json.add( { { "Windows" },{ { "win7x86","qjs-windows-x86.exe",{ "qjs-windows-x86.exe" } },
				    { "win7amd64","qjs-windows-x86_64.exe",{ "qjs-windows-x86_64.exe" } },
				    { "x86","qjs-windows-x86.exe",{ "qjs-windows-x86.exe" } },
				    { "amd64","qjs-windows-x86_64.exe",{ "qjs-windows-x86_64.exe" } } } } ) ;

	json.add( { { "MacOS" },{ { "x86","qjs-darwin",{ "qjs-darwin" } },
				  { "aarch64","qjs-darwin",{ "qjs-darwin" } },
				  { "amd64","qjs-darwin",{ "qjs-darwin" } } } } ) ;

	json.done() ;

	mainObj.insert( "Version","1" ) ;

	mainObj.insert( "DownloadUrl","https://api.github.com/repos/quickjs-ng/quickjs/releases/latest" ) ;

	mainObj.insert( "DownloadUrlWin7","https://api.github.com/repos/quickjs-ng/quickjs/releases/latest" ) ;

	mainObj.insert( "AutoUpdate",true ) ;

	mainObj.insert( "Name","quickjs-ng" ) ;

	mainObj.insert( "VersionArgument","--version" ) ;

	mainObj.insert( "BackendPath",utility::stringConstants::defaultPath() ) ;

	mainObj.insert( "VersionStringLine",0 ) ;

	mainObj.insert( "VersionStringPosition",0 ) ;

	mainObj.insert( "LikeYoutubeDl",false ) ;

	engines::file( m,logger ).write( mainObj ) ;
}

void quickjs_ng::remove( Logger&,const engines::enginePaths& enginePath )
{
	auto m = enginePath.enginePath( "quickjs_ng.json" ) ;

	if( QFile::exists( m ) ){

		QFile::remove( m ) ;
	}

	m = enginePath.binPath( quickjs_ng::getNameAndExe().exe ) ;

	if( QFile::exists( m ) ){

		QFile::remove( m ) ;
	}
}

QString quickjs_ng::parseVersionInfo( const utils::qprocess::outPut& e )
{
	if( !e.stdOut.isEmpty() ){

		const auto s = util::split( e.stdOut,'\n' ) ;

		if( s.size() == 1 ){

			return s[ 0 ].trimmed() ;
		}else{
			for( const auto& it : s ){

				if( it.startsWith( "QuickJS-ng" ) ){

					auto m = util::split( it,' ' ) ;

					return m[ m.size() - 1 ].trimmed() ;
				}
			}
		}
	}

	return {} ;
}

quickjs_ng::nameAndExe quickjs_ng::getNameAndExe()
{
	utility::CPU cpu ;

	quickjs_ng::nameAndExe e ;

	if( utility::platformIsWindows() ){

		if( cpu.x86_64() ){

			e.exe = "qjs-windows-x86_64.exe" ;
		}else{
			e.exe = "qjs-windows-x86.exe" ;
		}
	}else{
		if( cpu.x86_64() ){

			e.exe = "qjs-linux-x86_64" ;
		}else{
			e.exe = "qjs-linux-x86" ;
		}
	}

	return e ;
}

quickjs_ng::~quickjs_ng()
{
}

quickjs_ng::quickjs_ng( const engines& e,const engines::engine& s,QJsonObject& ) :
	engines::engine::baseEngine( e.Settings(),s,e.processEnvironment() )
{
}

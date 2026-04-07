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

#include "bun.h"
#include "../utility.h"

void bun::init( settings&,Logger& logger,const engines::enginePaths& enginePath )
{
	auto m = enginePath.enginePath( "bun.json" ) ;

	QJsonObject mainObj ;

	utility::addJsonCmd json( mainObj ) ;

	json.add( { { "Generic" },{ { "x86","bun",{ "bun" } },
				{ "amd64","bun",{ "bun" } },
				{ "aarch64","bun",{ "bun" } } } } ) ;

	json.add( { { "Windows" },{ { "win7x86","bun.exe",{ "bun.exe" } },
				{ "win7amd64","bun.exe",{ "bun.exe" } },
				{ "x86","bun.exe",{ "bun.exe" } },
				{ "amd64","bun.exe",{ "bun.exe" } } } } ) ;

	json.add( { { "MacOS" },{ { "amd64","bun",{ "bun" } },
				{ "aarch64","bun",{ "bun" } } } } ) ;

	json.done() ;

	mainObj.insert( "Version","1" ) ;

	mainObj.insert( "DownloadUrl","" ) ;

	mainObj.insert( "DownloadUrlWin7","" ) ;

	mainObj.insert( "AutoUpdate",true ) ;

	mainObj.insert( "Name","bun" ) ;

	mainObj.insert( "VersionArgument","-version" ) ;

	mainObj.insert( "BackendPath",utility::stringConstants::defaultPath() ) ;

	mainObj.insert( "VersionStringLine",0 ) ;

	mainObj.insert( "VersionStringPosition",0 ) ;

	mainObj.insert( "LikeYoutubeDl",false ) ;

	engines::file( m,logger ).write( mainObj ) ;
}

void bun::remove( Logger&,const engines::enginePaths& enginePath )
{
	auto m = enginePath.enginePath( "bun.json" ) ;

	if( QFile::exists( m ) ){

		QFile::remove( m ) ;
	}

	m = enginePath.binPath( "bun" ) ;

	if( QFile::exists( m ) ){

		QFile::remove( m ) ;
	}
}

bun::bun( const engines& e,const engines::engine& s,QJsonObject& ) :
	engines::engine::baseEngine( e.Settings(),s,e.processEnvironment() )
{
}

bun::~bun()
{
}

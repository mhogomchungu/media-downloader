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
#include <QProcess>

util::version deno::version( const QString& m )
{
	QProcess cmd ;

	cmd.start( m,{ "-version" } ) ;

	cmd.waitForFinished() ;

	if( cmd.exitCode() == 0 && cmd.exitStatus() == QProcess::ExitStatus::NormalExit ){

		return cmd.readAllStandardOutput().replace( "deno","" ).trimmed() ;
	}else{
		return {} ;
	}
}

utility::addJsonCmd::entry::args deno::entryCmd( const QString& e )
{
	utility::addJsonCmd::entry::args data ;

	if( e == "Windows" ){

		data.emplace_back( "x86","deno.exe" ) ;
		data.emplace_back( "amd64","deno.exe" ) ;
	}else{
		data.emplace_back( "x86","deno" ) ;
		data.emplace_back( "aarch64","deno" ) ;
		data.emplace_back( "amd64","deno" ) ;
	}

	return data ;
}

void deno::init( QJsonObject& mainObj,settings& s,const engines::enginePaths& enginePath )
{
	auto m = enginePath.enginePath( "deno.json" ) ;

	if( QFile::exists( m ) ){

		QFile::remove( m ) ;
	}

	utility::addJsonCmd json( mainObj ) ;

	json.add( "Generic",deno::entryCmd ) ;

	json.add( "Windows",deno::entryCmd ) ;

	json.add( "MacOS",deno::entryCmd ) ;

	json.done() ;

	mainObj.insert( "Version","3" ) ;

	mainObj.insert( "DownloadUrl","https://api.github.com/repos/denoland/deno/releases/latest" ) ;

	mainObj.insert( "DownloadUrlWin7","" ) ;

	mainObj.insert( "UpdatableSupportingEngine",true ) ;

	mainObj.insert( "AutoUpdate",s.denoEnableAutoDownload() ) ;

	mainObj.insert( "Name","deno" ) ;

	mainObj.insert( "VersionArgument","--version" ) ;

	mainObj.insert( "BackendPath",utility::stringConstants::defaultPath() ) ;

	mainObj.insert( "VersionStringLine",0 ) ;

	mainObj.insert( "VersionStringPosition",1 ) ;

	mainObj.insert( "LikeYoutubeDl",false ) ;
}

void deno::remove( Logger&,const engines::enginePaths& )
{
}

deno::~deno()
{
}

bool deno::foundNetworkUrl( const QString& s )
{
	auto e = this->urlFileName( {} ) ;

	if( e.isEmpty() ){

		return false ;
	}else{
		return s.contains( e ) ;
	}
}

void deno::updateVersionString( QString& m )
{
	if( m == "2.7.0+fb4db33" ){

		//Deno 2.7.0 has a bad version string

		m = "2.7.0" ;

	}
}

QString deno::urlFileName( const QString& )
{
	utility::CPU cpu ;

	if( utility::platformIsWindows() ){

		if( cpu.x86_64() ){

			return "deno-x86_64-pc-windows-msvc.zip" ;
		}

	}else if( utility::platformIsLinux() || utility::platformisFlatPak() ){

		if( cpu.x86_64() ){

			return "deno-x86_64-unknown-linux-gnu.zip" ;

		}else if( cpu.aarch64() ){

			return "deno-aarch64-unknown-linux-gnu.zip" ;
		}

	}else if( utility::platformIsOSX() ){

		if( cpu.x86_64() ){

			return "deno-x86_64-apple-darwin.zip" ;

		}else if( cpu.aarch64() ){

			return "deno-aarch64-apple-darwin.zip" ;
		}
	}

	return {} ;
}

bool deno::autoUpdate( const engines::engine::baseEngine::onlineVersion&,const util::version& s )
{
	return s < "2.6.7" ;
}

deno::deno( const engines& e,const engines::engine& s,QJsonObject& ) :
	engines::engine::baseEngine( e.Settings(),s,e.processEnvironment() )
{
}

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

#include "python.h"

void python::init( QJsonObject& obj,const engines::enginePaths& enginePath )
{
	obj.insert( "VersionArgument","--version" ) ;
	obj.insert( "VersionStringLine",0 ) ;
	obj.insert( "VersionStringPosition",1 ) ;

	if( utility::platformIsLikeWindows() ){

		if( QFile::exists( enginePath.binPath() + "/you-get" ) ){

			auto path = enginePath.binPath() + "/" + python::folderName() ;

			QFileInfo m( path ) ;

			if( m.exists() ){

				if( m.isFile() ){

					QFile::remove( path ) ;

					python::setUrl( obj ) ;
				}
			}else{
				python::setUrl( obj ) ;
			}
		}
	}
}

void python::remove( Logger&,const engines::enginePaths& )
{
}

python::~python()
{
}

QString python::updateCmdPath( const QString& e )
{
	return e + "/python-3.8.10-win32/python.exe" ;
}

bool python::foundNetworkUrl( const QString& e )
{
	return e == this->urlFileName( {} ) ;
}

QString python::urlFileName( const QString& )
{
	return python::fileName() ;
}

engines::metadata python::parseJsonDataFromGitHub( const QJsonDocument& doc )
{
	return engines::engine::baseEngine::parseJsonDataFromGitHub( doc ) ;
}

engines::engine::baseEngine::onlineVersion python::versionInfoFromGithub( const QByteArray& )
{
	auto m = "3.8.10" ;

	return { m,m } ;
}

engines::engine::baseEngine::renameArchiveFolderStatus
python::renameArchiveFolder( const QString& archivePath,const QString& binPath )
{
	auto m = python::archiveExtension() ;

	auto oldPath = binPath + "/" + QFileInfo( archivePath ).fileName().replace( m,"" ) ;
	auto newPath = binPath + "/" + python::folderName() ;

	if( oldPath == newPath ){

		return {} ;
	}else{
		auto s = utility::rename( oldPath,newPath ) ;

		if( s.isEmpty() ){

			return {} ;
		}else{
			return { oldPath,newPath,s } ;
		}
	}
}

python::python( const engines& e,const engines::engine& s, QJsonObject& ) :
	engines::engine::baseEngine( e.Settings(),s,e.processEnvironment() )
{
}

QString python::archiveExtension()
{
	return ".zip" ;
}

QString python::folderName()
{
	return "python-3.8.10-win32" ;
}

QString python::fileName()
{
	return "python-3.8.10-win32.zip" ;
}

void python::setUrl( QJsonObject& obj )
{
	obj.insert( "ArchiveContainsFolder",true ) ;

	obj.insert( "DownloadUrl","https://api.github.com/repos/mhogomchungu/packages/releases/latest" ) ;

	obj.insert( "AutoUpdate",true ) ;
}

utility::addJsonCmd::entry::args python::entryCmd( const QString& e )
{
	utility::addJsonCmd::entry::args data ;

	if( e == "Windows" ){

		data.emplace_back( "amd64","python.exe" ) ;
	}

	return data ;
}

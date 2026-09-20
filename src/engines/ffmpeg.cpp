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

#include "ffmpeg.h"

utility::addJsonCmd::entry::args ffmpeg::entryCmd( const QString& e )
{
	utility::addJsonCmd::entry::args data ;

	if( e == "Windows" ){

		data.emplace_back( "amd64","ffmpeg.exe" ) ;
	}

	return data ;
}

void ffmpeg::init( settings&,Logger& logger,const engines::enginePaths& enginePath )
{
	auto m = enginePath.enginePath( "ffmpeg.json" ) ;

	if( QFile::exists( m ) ){

		return ;
	}

	QJsonObject mainObj ;

	utility::addJsonCmd json( mainObj ) ;

	json.add( "Windows",ffmpeg::entryCmd ) ;

	json.done() ;

	mainObj.insert( "Version","1" ) ;

	mainObj.insert( "DownloadUrl","https://api.github.com/repos/BtbN/FFmpeg-Builds/releases" ) ;

	mainObj.insert( "AutoUpdate",true ) ;

	mainObj.insert( "Name","ffmpeg" ) ;

	mainObj.insert( "VersionArgument","-version" ) ;

	mainObj.insert( "BackendPath",utility::stringConstants::defaultPath() ) ;

	mainObj.insert( "VersionStringLine",0 ) ;

	mainObj.insert( "VersionStringPosition",2 ) ;

	mainObj.insert( "LikeYoutubeDl",false ) ;

	engines::file( m,logger ).write( mainObj ) ;
}

void ffmpeg::remove( Logger&,const engines::enginePaths& enginePath )
{
	auto m = enginePath.enginePath( "ffmpeg.json" ) ;

	if( QFile::exists( m ) ){

		QFile::remove( m ) ;
	}

	m = enginePath.binPath( "ffmpeg" ) ;

	if( QFile::exists( m ) ){

		QFile::remove( m ) ;
	}
}

ffmpeg::~ffmpeg()
{
}

bool ffmpeg::foundNetworkUrl( const QString& e )
{
	return e.startsWith( "ffmpeg-" ) && e.endsWith( "-latest-win64-gpl-shared-9.0.zip" ) ;
}

engines::engine::baseEngine::renameArchiveFolderStatus
ffmpeg::renameArchiveFolder( const QString& archivePath,const QString& binPath )
{
	auto m = this->archiveExtension() ;

	const auto& name = engines::engine::baseEngine::engine().name() ;

	auto oldPath = binPath + "/" + QFileInfo( archivePath ).fileName().replace( m,"" ) ;
	auto newPath = binPath + "/" + name ;

	auto s = utility::rename( oldPath,newPath ) ;

	if( s.isEmpty() ){

		return {} ;
	}else{
		return { oldPath,newPath,s } ;
	}
}

ffmpeg::ffmpeg( const engines& e,const engines::engine& s,QJsonObject& ) :
    engines::engine::baseEngine( e.Settings(),s,e.processEnvironment() )
{
}

QString ffmpeg::archiveExtension()
{
	return ".zip" ;
}

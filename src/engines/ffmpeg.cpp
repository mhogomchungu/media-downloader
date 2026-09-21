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

#include <QDir>

QString ffmpeg::fileName()
{
	return "ffmpeg-n9.0-latest-win64-gpl-shared-9.0.zip" ;
}

QString ffmpeg::longVersionString()
{
	return "n9.0.2-3-ga5923073bf-20260920" ;
}

QString ffmpeg::shortVersionString()
{
	auto m = util::split( ffmpeg::longVersionString(),"-" ) ;

	return m[ 0 ].replace( "n","" ) ;
}

QString ffmpeg::folderName()
{
	auto m = ffmpeg::archiveExtension() ;

	return ffmpeg::fileName().replace( m,"" ) ;
}

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

	ffmpeg::checkUpdatedVersion( enginePath ) ;

	if( QFile::exists( m ) ){

		return ;
	}

	QJsonObject mainObj ;

	utility::addJsonCmd json( mainObj ) ;

	json.add( "Windows",ffmpeg::entryCmd ) ;

	json.done() ;

	mainObj.insert( "ArchiveContainsFolder",true ) ;

	mainObj.insert( "Version","1" ) ;

	mainObj.insert( "DownloadUrl","https://api.github.com/repos/BtbN/FFmpeg-Builds/releases/latest" ) ;

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

}

void ffmpeg::replaceVersionString( QString& m )
{
	if( utility::platformIsModernWindows() && m == ffmpeg::longVersionString() ){

		m = ffmpeg::shortVersionString() ;
	}
}

ffmpeg::~ffmpeg()
{
}

QString ffmpeg::updateCmdPath( const QString& e )
{
	return e + "/" + ffmpeg::folderName() + "/bin/ffmpeg.exe" ;
}

bool ffmpeg::foundNetworkUrl( const QString& e )
{
	return e == this->urlFileName( {} ) ;
}

QString ffmpeg::urlFileName( const QString& )
{
	return ffmpeg::fileName() ;
}

engines::metadata ffmpeg::parseJsonDataFromGitHub( const QJsonDocument& doc )
{
	return engines::engine::baseEngine::parseJsonDataFromGitHub( doc ) ;
}

engines::engine::baseEngine::onlineVersion ffmpeg::versionInfoFromGithub( const QByteArray& )
{
	auto a = ffmpeg::shortVersionString() ;
	return { a,a } ;
}

engines::engine::baseEngine::renameArchiveFolderStatus
ffmpeg::renameArchiveFolder( const QString& archivePath,const QString& binPath )
{
	auto m = ffmpeg::archiveExtension() ;

	auto oldPath = binPath + "/" + QFileInfo( archivePath ).fileName().replace( m,"" ) ;
	auto newPath = binPath + "/" + this->folderName() ;

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

ffmpeg::ffmpeg( const engines& e,const engines::engine& s,QJsonObject& ) :
    engines::engine::baseEngine( e.Settings(),s,e.processEnvironment() )
{
}

void ffmpeg::checkUpdatedVersion( const engines::enginePaths& enginePath )
{
	const auto e = utility::dirEntries( enginePath.binPath() ) ;

	for( const auto& it : e ){

		if( it.contains( "ffmpeg" ) && it != ffmpeg::folderName() ){

			class meaw
			{
			public:
				meaw( const QString& p ) : m_path( p )
				{
				}
				void fg()
				{

				}
				void bg()
				{
					ffmpeg::deleteFolder( m_path ) ;
				}
			private:
				QString m_path ;
			} ;

			utils::qthread::run( meaw( enginePath.binPath() + "/" + it ) ) ;

		}
	}
}

void ffmpeg::deleteFolder( const QString& path )
{
	auto m = path + "-" + QString::number( utility::simpleRandomNumber() ) ;

	QDir dir ;

	for( int i = 0 ; i < 5 ; i++ ){

		if( dir.rename( path,m ) ){

			dir.setPath( m ) ;

			dir.removeRecursively() ;

			break ;
		}else{
			QThread::currentThread()->sleep( 1 ) ;
		}
	}

}

QString ffmpeg::archiveExtension()
{
	return ".zip" ;
}

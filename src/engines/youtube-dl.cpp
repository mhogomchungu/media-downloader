/*
 *
 *  Copyright (c) 2021
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

#include "youtube-dl.h"

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

#include "../networkAccess.h"
#include "../utility.h"

static QJsonObject _defaultControlStructure()
{
	QJsonObject obj ;

	obj.insert( "Connector","&&" ) ;

	obj.insert( "lhs",[](){

		QJsonObject obj ;

		obj.insert( "startsWith","[download]" ) ;

		return obj ;
	}() ) ;

	obj.insert( "rhs",[](){

		QJsonObject obj ;

		obj.insert( "contains","ETA" ) ;

		return obj ;
	}() ) ;

	return obj ;
}

void youtube_dl::init( Logger& logger,const engines::enginePaths& enginePath )
{
	auto m = enginePath.configPath( "youtube-dl.json" ) ;

	if( !QFile::exists( m ) ){

		QJsonObject mainObj ;

		mainObj.insert( "UsePrivateExecutable",[](){

			if( networkAccess::hasNetworkSupport() ){

				return true ;
			}else{
				if( utility::platformIsWindows() ){

					return true ;
				}else{
					return false ;
				}
			}
		}() ) ;

		mainObj.insert( "CommandName","youtube-dl" ) ;

		mainObj.insert( "CommandNameWindows","youtube-dl.exe" ) ;

		mainObj.insert( "Name","youtube-dl" ) ;

		mainObj.insert( "DefaultDownLoadCmdOptions",[](){

			QJsonArray arr ;

			arr.append( "--newline" ) ;
			arr.append( "--ignore-config" ) ;
			arr.append( "--no-playlist" ) ;

			return arr ;
		}() ) ;

		mainObj.insert( "DefaultListCmdOptions",[](){

			QJsonArray arr ;
			arr.append( "-F" ) ;
			return arr ;
		}() ) ;

		mainObj.insert( "SkipLineWithText",[](){

			QJsonArray arr ;
			arr.append( "(pass -k to keep)" ) ;
			return arr ;
		}() ) ;

		mainObj.insert( "RemoveText",[](){

			QJsonArray arr ;

			return arr ;
		}() ) ;

		mainObj.insert( "SplitLinesBy",[](){

			QJsonArray arr ;

			arr.append( "\n" ) ;

			return arr ;
		}() ) ;

		mainObj.insert( "ControlJsonStructure",_defaultControlStructure() ) ;

		mainObj.insert( "DownloadUrl","https://api.github.com/repos/ytdl-org/youtube-dl/releases/latest" ) ;

		mainObj.insert( "VersionArgument","--version" ) ;

		mainObj.insert( "OptionsArgument","-f" ) ;

		mainObj.insert( "BackendPath","${default}" ) ;

		mainObj.insert( "VersionStringLine",0 ) ;

		mainObj.insert( "VersionStringPosition",0 ) ;

		mainObj.insert( "BatchFileArgument","-a" ) ;

		mainObj.insert( "CanDownloadPlaylist",true ) ;

		mainObj.insert( "LikeYoutubeDl",true ) ;

		engines::file( m,logger ).write( mainObj ) ;
	}
}

youtube_dl::youtube_dl()
{
}

youtube_dl::~youtube_dl()
{
}

void youtube_dl::updateOptions( QJsonObject& object )
{
	if( !object.contains( "SkipLineWithText" ) ){

		object.insert( "SkipLineWithText",[](){

			QJsonArray arr ;

			arr.append( "(pass -k to keep)" ) ;

			return arr ;
		}() ) ;
	}

	if( !object.contains( "ControlJsonStructure" ) ){

		object.insert( "ControlJsonStructure",_defaultControlStructure() ) ;
	}
}

std::unique_ptr< engines::engine::functions::filter > youtube_dl::Filter()
{
	return std::make_unique< youtube_dl::youtube_dlFilter >() ;
}

void youtube_dl::updateDownLoadCmdOptions( const engines::engine& engine,
					   const QString& quality,
					   const QStringList& userOptions,
					   QStringList& urls,
					   QStringList& ourOptions )
{
	Q_UNUSED( urls )

	if( userOptions.contains( "--yes-playlist" ) ){

		ourOptions.removeAll( "--no-playlist" ) ;
	}

	ourOptions.append( engine.optionsArgument() ) ;

	if( quality.isEmpty() ){

		ourOptions.append( "best" ) ;
	}else{
		ourOptions.append( quality ) ;
	}
}

youtube_dl::youtube_dlFilter::youtube_dlFilter() :
	m_processing( QObject::tr( "Processing ..." ) )
{
}

const QString& youtube_dl::youtube_dlFilter::operator()( const QString& e )
{
	if( e.startsWith( "[download]  " ) && e.contains( " ETA " ) ){

		m_tmp = e ;
		m_tmp.replace( "[download]  ","" ) ;
		return m_tmp ;

	}else if( e.startsWith( "[download] 100% of " ) ){

		m_final = e ;
		m_final.replace( "[download] ","" ) ;
		return m_final ;

	}else if( e.startsWith( "[ffmpeg] Merging formats into" ) ){

		return m_final ;
	}else{
		if( e.startsWith( "ERROR: " ) ){

			return e ;
		}else{
			if( m_final.isEmpty() ){

				return m_processing ;
			}else{
				return m_final ;
			}
		}
	}
}

youtube_dl::youtube_dlFilter::~youtube_dlFilter()
{
}

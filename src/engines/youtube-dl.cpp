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

#include "network_support.h"
#include "../utility.h"

QByteArray youtube_dl::config( const QString& enginePath ) const
{
	auto m = enginePath + "/youtube-dl.json" ;

	if( !QFile::exists( m ) ){

		QJsonObject mainObj ;

		mainObj.insert( "UsePrivateExecutable",[]()->QJsonValue{
			#if MD_NETWORK_SUPPORT
				return true ;
			#else
				#ifdef Q_OS_LINUX
					return false ;
				#else
					return true ;
				#endif
			#endif
		}() ) ;

		mainObj.insert( "CommandName",[]()->QJsonValue{

			if( utility::platformIsWindows() ){

				return "youtube-dl.exe" ;
			}else{
				return "youtube-dl" ;
			}
		}() ) ;

		mainObj.insert( "DefaultDownLoadCmdOptions",[]()->QJsonValue{

			QJsonArray arr ;
			arr.append( "--newline" ) ;
			arr.append( "--ignore-config" ) ;
			arr.append( "--no-playlist" ) ;
			arr.append( "--newline" ) ;

			return arr ;
		}() ) ;

		mainObj.insert( "DefaultListCmdOptions",[]()->QJsonValue{

			QJsonArray arr ;
			arr.append( "-F" ) ;
			return arr ;
		}() ) ;

		mainObj.insert( "DownloadUrl",[]()->QJsonValue{

			return "https://api.github.com/repos/ytdl-org/youtube-dl/releases/latest" ;
		}() ) ;

		mainObj.insert( "VersionArgument",[]()->QJsonValue{

			return "--version" ;
		}() ) ;

		mainObj.insert( "OptionsArgument",[]()->QJsonValue{

			return "-f" ;
		}() ) ;

		mainObj.insert( "BackendPath",[ & ]()->QJsonValue{

			return enginePath + "/bin" ;
		}() ) ;

		mainObj.insert( "VersionStringLine",[]()->QJsonValue{

			return 0 ;
		}() ) ;

		mainObj.insert( "VersionStringPosition",[]()->QJsonValue{

			return 0 ;
		}() ) ;

		QJsonDocument doc( mainObj ) ;

		QFile file( m ) ;
		file.open( QIODevice::WriteOnly ) ;
		file.write( doc.toJson( QJsonDocument::Indented ) ) ;
	}

	QFile file( m ) ;
	file.open( QIODevice::ReadOnly ) ;
	return file.readAll() ;
}

engines::engine::functions youtube_dl::functions() const
{
	engines::engine::functions functions ;

	functions.updateDownLoadCmdOptions = []( const engines::engine& engine,
						 const QString& quality,
						 const QStringList& userOptions,
						 QStringList& ourOptions ){

		if( userOptions.contains( "--yes-playlist" ) ){

			ourOptions.removeAll( "--no-playlist" ) ;
		}		

		ourOptions.append( engine.optionsArgument() ) ;
		ourOptions.append( quality ) ;
	} ;

	functions.hasSupport = []( const QString& url ){

		Q_UNUSED( url )

		return true ;
	} ;

	functions.processData = []( QStringList& outPut,const QByteArray& data ){

		for( const auto& m : utility::split( data ) ){

			if( m.isEmpty() ){

				continue ;

			}else if( m.startsWith( "[download]" ) && m.contains( "ETA" ) ){

				auto& s = outPut.last() ;

				if( s.startsWith( "[download]" ) && s.contains( "ETA" ) ){

					s = m ;
				}else{
					outPut.append( m ) ;
				}
			}else{
				outPut.append( m ) ;
			}
		}
	} ;

	return functions ;
}

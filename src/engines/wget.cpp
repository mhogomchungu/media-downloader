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

#include "wget.h"

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QFile>

#include "../utility.h"

wget::wget()
{
}
#include <QDebug>

QByteArray wget::config( const QString& enginePath ) const
{
	auto m = enginePath + "/engines/wget.json" ;

	if( !QFile::exists( m ) ){

		QJsonObject mainObj ;

		mainObj.insert( "UsePrivateExecutable",[]()->QJsonValue{

			return false ;
		}() ) ;

		mainObj.insert( "CommandName",[]()->QJsonValue{

			if( utility::platformIsWindows() ){

				return "wget.exe" ;
			}else{
				return "wget" ;
			}
		}() ) ;

		mainObj.insert( "Name",[]()->QJsonValue{

			return "wget" ;
		}() ) ;

		mainObj.insert( "DefaultDownLoadCmdOptions",[]()->QJsonValue{

			QJsonArray arr ;

			return arr ;
		}() ) ;

		mainObj.insert( "DefaultListCmdOptions",[]()->QJsonValue{

			QJsonArray arr ;

			return arr ;
		}() ) ;

		mainObj.insert( "DownloadUrl",[]()->QJsonValue{

			return "" ;
		}() ) ;

		mainObj.insert( "VersionArgument",[]()->QJsonValue{

			return "--version" ;
		}() ) ;

		mainObj.insert( "OptionsArgument",[]()->QJsonValue{

			return "" ;
		}() ) ;

		mainObj.insert( "BackendPath",[]()->QJsonValue{

			return "" ;
		}() ) ;

		mainObj.insert( "VersionStringLine",[]()->QJsonValue{

			return 0 ;
		}() ) ;

		mainObj.insert( "VersionStringPosition",[]()->QJsonValue{

			return 2 ;
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

engines::engine::functions wget::functions() const
{
	engines::engine::functions functions ;

	functions.updateDownLoadCmdOptions = []( const engines::engine& engine,
						 const QString& quality,
						 const QStringList& userOptions,
						 QStringList& ourOptions ){
		Q_UNUSED( userOptions )

		ourOptions.append( engine.optionsArgument() ) ;
		ourOptions.append( quality ) ;

		ourOptions.append( "--progress=bar:force" ) ;
	} ;

	functions.processData = []( QStringList& outPut,const QByteArray& data ){

		for( const auto& m : utility::split( data,'\r' ) ){

			if( m.isEmpty() ){

				continue ;

			}else if( m.contains( "%[" ) || m.contains( "<=>" ) ){

				auto& s = outPut.last() ;

				if( s.contains( "%[" ) || s.contains( "<=>" ) ){

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

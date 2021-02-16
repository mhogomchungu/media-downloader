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

QByteArray wget::config( engines::log& log,const engines::enginePaths& enginePath ) const
{
	auto m = enginePath.configPath() + "/wget.json" ;

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

		mainObj.insert( "BatchFileArgument",[]()->QJsonValue{

			return "-i" ;
		}() ) ;

		mainObj.insert( "CanDownloadPlaylist",[]()->QJsonValue{

			return false ;
		}() ) ;

		engines::file file( m,log ) ;

		file.write( mainObj ) ;
	}

	return engines::file( m,log ).readAll() ;
}

std::unique_ptr< engines::engine::functions > wget::Functions() const
{
	return std::make_unique< wget::functions >() ;
}

wget::functions::~functions()
{
}

void wget::functions::processData( QStringList& outPut,const QByteArray& data )
{
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
}

void wget::functions::updateDownLoadCmdOptions( const engines::engine& engine,
						      const QString& quality,
						      const QStringList& userOptions,
						      QStringList& ourOptions )
{
	Q_UNUSED( userOptions )

	if( !engine.optionsArgument().isEmpty() ){

		ourOptions.append( engine.optionsArgument() ) ;
	}

	if( !quality.isEmpty() ){

		ourOptions.append( quality ) ;
	}

	ourOptions.append( "--progress=bar:force" ) ;
}

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

static engines::Json _to_json( const QByteArray& data )
{
	engines::Json json( data ) ;

	if( json ){

		auto object = json.doc().object() ;

		/*
		 * We are overriding the option set in the config file to
		 * make sure it has expected value
		 */
		auto iter = object.find( "UsePrivateExecutable" ) ;

		if( iter != object.end() ){

			iter.value() = false ;
		}else{
			object.insert( "UsePrivateExecutable",false ) ;
		}

		return object ;
	}else{
		return json ;
	}
}

wget::wget()
{
}

engines::Json wget::config( Logger& logger,const engines::enginePaths& enginePath ) const
{
	auto m = enginePath.configPath() + "/wget.json" ;

	if( !QFile::exists( m ) ){

		QJsonObject mainObj ;

		mainObj.insert( "UsePrivateExecutable",false ) ;

		mainObj.insert( "CommandName","wget" ) ;

		mainObj.insert( "CommandNameWindows","wget.exe" ) ;

		mainObj.insert( "Name","wget" ) ;

		mainObj.insert( "DefaultDownLoadCmdOptions",QJsonArray() ) ;

		mainObj.insert( "DefaultListCmdOptions",QJsonArray() ) ;

		mainObj.insert( "DownloadUrl","" ) ;

		mainObj.insert( "VersionArgument","--version" ) ;

		mainObj.insert( "OptionsArgument","" ) ;

		mainObj.insert( "BackendPath","" ) ;

		mainObj.insert( "VersionStringLine",0 ) ;

		mainObj.insert( "VersionStringPosition",2 ) ;

		mainObj.insert( "BatchFileArgument","-i" ) ;

		mainObj.insert( "CanDownloadPlaylist",false ) ;

		engines::file( m,logger ).write( mainObj ) ;
	}

	return _to_json( engines::file( m,logger ).readAll() ) ;
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
						QStringList& urls,
						QStringList& ourOptions )
{
	Q_UNUSED( userOptions )
	Q_UNUSED( urls )

	if( !engine.optionsArgument().isEmpty() ){

		ourOptions.append( engine.optionsArgument() ) ;
	}

	if( !quality.isEmpty() ){

		ourOptions.append( quality ) ;
	}

	ourOptions.append( "--progress=bar:force" ) ;
}

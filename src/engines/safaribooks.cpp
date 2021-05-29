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

#include "safaribooks.h"

#include "../utility.h"

#include <QJsonObject>
#include <QJsonArray>

safaribooks::safaribooks( settings& s ) :
	engines::engine::functions( s )
{
}

safaribooks::~safaribooks()
{
}

void safaribooks::updateOptions( QJsonObject& object,settings& )
{
	if( !object.contains( "ControlJsonStructure" ) ){

		QJsonObject obj ;

		obj.insert( "Connector","&&" ) ;

		obj.insert( "lhs",[](){

			QJsonObject obj ;

			obj.insert( "containsAll",[](){

				QJsonArray arr ;

				arr.append( "[" ) ;
				arr.append( "]" ) ;

				return arr ;
			}() ) ;

			return obj ;
		}() ) ;

		obj.insert( "rhs",[](){

			QJsonObject obj ;

			obj.insert( "endsWith","%" ) ;

			return obj ;
		}() ) ;

		object.insert( "ControlJsonStructure",obj ) ;
	}

	if( !object.contains( "RemoveText" ) ){

		object.insert( "RemoveText",[](){

			QJsonArray arr ;

			arr.append( "\033[0m" ) ;
			arr.append( "\033[33m" ) ;
			arr.append( "\033[41m" ) ;
			arr.append( "\033[43m" ) ;

			return arr ;
		}() ) ;
	}

	if( !object.contains( "SplitLinesBy" ) ){

		object.insert( "SplitLinesBy",[](){

			QJsonArray arr ;

			arr.append( "\r" ) ;
			arr.append( "\n" ) ;

			return arr ;
		}() ) ;
	}
}

QString safaribooks::commandString( const engines::engine::exeArgs::cmd& cmd )
{
	auto m = "\"" + cmd.exe() + "\"" ;

	const auto& args = cmd.args() ;

	for( int i = 0 ; i < args.size() ; i++ ){

		if( i > 0 && args[ i - 1 ] == "--cred" ){

			m += " \"" + QObject::tr( "<REDACTED>" ) + "\"" ;
		}else{
			m += " \"" + args[ i ] + "\"" ;
		}
	}

	return m ;
}

void safaribooks::sendCredentials( const engines::engine& engine,
				   const QString& credentials,
				   QProcess& exe )
{
	if( utility::platformIsNOTWindows() ){

		if( credentials.isEmpty() ){

			exe.write( engine.userName().toUtf8() + "\n" ) ;
			exe.write( engine.password().toUtf8() + "\n" ) ;
		}else{
			auto m = utility::split( credentials,':',true ) ;

			if( m.size() > 1 ){

				exe.write( m.at( 0 ).toUtf8() + "\n" ) ;
				exe.write( m.at( 1 ).toUtf8() + "\n" ) ;
			}
		}

		exe.closeWriteChannel() ;
	}
}

void safaribooks::updateDownLoadCmdOptions( const engines::engine& engine,
					    const QString& quality,
					    const QStringList& userOptions,
					    QStringList& urls,
					    QStringList& ourOptions )
{
	Q_UNUSED( userOptions )

	if( urls.size() > 0 ){

		urls[ 0 ] = utility::split( urls[ 0 ],'/',true ).last() ;
	}

	ourOptions.append( "--destination" ) ;
	ourOptions.append( engines::engine::functions::Settings().downloadFolder() ) ;

	if( utility::platformIsWindows() || !quality.isEmpty() ){

		ourOptions.append( "--cred" ) ;

		if( quality.isEmpty() ){

			ourOptions.append( engine.userName() + ":" + engine.password() ) ;
		}else{
			ourOptions.append( quality ) ;
		}
	}else{
		ourOptions.append( "--login" ) ;
	}
}

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

#include "aria2c.h"
#include "../utility.h"
#include "../settings.h"

void aria2c::init( const QString& name,
		   const QString& configFileName,
		   Logger& logger,
		   const engines::enginePaths& enginePath )
{
	auto m = enginePath.enginePath( configFileName ) ;

	if( !QFile::exists( m ) ){

		QJsonObject mainObj ;

		mainObj.insert( "ShowListTableBoundary",QJsonObject() ) ;

		mainObj.insert( "DefaultListCmdOptions",QJsonArray() ) ;

		mainObj.insert( "DownloadUrl","" ) ;

		mainObj.insert( "Name",name ) ;

		mainObj.insert( "CookieArgument","" ) ;

		mainObj.insert( "DefaultDownLoadCmdOptions",QJsonArray() ) ;

		mainObj.insert( "SkipLineWithText",[](){

			QJsonArray arr ;
			arr.append( "                                                                               " ) ;
			return arr ;
		}() ) ;

		utility::addJsonCmd json( mainObj ) ;

		json.add( { { "Generic" },{ { "x86",name,{ "stdbuf","-o","L",name } },
					    { "amd64",name,{ "stdbuf","-o","L",name } } } } ) ;

		auto exe = name + ".exe" ;

		json.add( { { "Windows" },{ { "x86",exe,{ exe } },
					    { "amd64",exe,{ exe } } } } ) ;

		json.done() ;

		mainObj.insert( "RemoveText",QJsonArray() ) ;

		mainObj.insert( "SplitLinesBy",[](){

			QJsonArray arr ;

			arr.append( "\n" ) ;
			arr.append( "\r" ) ;

			return arr ;
		}() ) ;

		mainObj.insert( "PlayListIdArguments",QJsonArray() ) ;

		mainObj.insert( "RequiredMinimumVersionOfMediaDownloader","2.2.0" ) ;

		mainObj.insert( "PlaylistItemsArgument","" ) ;

		mainObj.insert( "ControlJsonStructure",[](){

			QJsonObject obj ;

			obj.insert( "Connector","&&" ) ;

			obj.insert( "lhs",[](){

				QJsonObject obj ;

				obj.insert( "startsWith","[#" ) ;

				return obj ;
			}() ) ;

			obj.insert( "rhs",[](){

				QJsonObject obj ;

				obj.insert( "contains","CN:" ) ;

				return obj ;
			}() ) ;

			return obj ;
		}() ) ;

		mainObj.insert( "VersionArgument","--version" ) ;

		mainObj.insert( "OptionsArgument","" ) ;

		mainObj.insert( "BackendPath",utility::stringConstants::defaultPath() ) ;

		mainObj.insert( "VersionStringLine",0 ) ;

		mainObj.insert( "VersionStringPosition",2 ) ;

		mainObj.insert( "BatchFileArgument","" ) ;

		mainObj.insert( "CanDownloadPlaylist",false ) ;

		mainObj.insert( "LikeYoutubeDl",false ) ;

		mainObj.insert( "ReplaceOutputWithProgressReport",false ) ;

		engines::file( m,logger ).write( mainObj ) ;
	}
}

void aria2c::trimProgressLine( QByteArray& e )
{
	e.truncate( e.size() - 1 ) ;
}

aria2c::~aria2c()
{
}

aria2c::aria2c( const engines& engines,const engines::engine& engine,QJsonObject& ) :
	engines::engine::functions( engines.Settings(),engine ),
	m_engines( engines )
{
}

engines::engine::functions::DataFilter aria2c::Filter( int id,const QString& e )
{
	auto& s = engines::engine::functions::Settings() ;
	const auto& engine = engines::engine::functions::engine() ;

	return { util::types::type_identity< aria2c::aria2c_dlFilter >(),e,s,engine,id } ;
}

QString aria2c::updateTextOnCompleteDownlod( const QString& uiText,
					     const QString& bkText,
					     const QString& dopts,
					     const finishedState& f )
{
	if( f.success() ){

		auto m = engines::engine::functions::processCompleteStateText( f ) ;
		auto e = engines::engine::functions::timer::stringElapsedTime( f.duration() ) ;

		return m + ", " + e + "\n" + uiText ;
	}else{
		return engines::engine::functions::updateTextOnCompleteDownlod( uiText,bkText,dopts,f ) ;
	}
}

void aria2c::updateDownLoadCmdOptions( const updateOpts& e )
{
	engines::engine::functions::updateDownLoadCmdOptions( e ) ;

	if( !e.ourOptions.contains( "-d" ) ){

		e.ourOptions.append( "-d" ) ;
		e.ourOptions.append( m_engines.Settings().downloadFolder() ) ;
	}

	if( !e.quality.isEmpty() ){

		e.ourOptions.append( e.quality ) ;
	}

	e.ourOptions.removeAll( "Default" ) ;
	e.ourOptions.removeAll( "default" ) ;
}

aria2c::aria2c_dlFilter::aria2c_dlFilter( const QString& e,settings&,const engines::engine& engine,int id ) :
	engines::engine::functions::filter( e,engine,id ),
	m_processId( id )
{
	if( m_processId ){}
}

const QByteArray& aria2c::aria2c_dlFilter::operator()( const Logger::Data& s )
{
	const auto data = s.toStringList() ;

	for( const auto& e : data ){

		if( e.contains( " Download complete: " ) ){

			m_fileName = e.mid( e.indexOf( " Download complete: " ) + 20 ) ;
			break ;
		}
	}

	if( s.lastLineIsProgressLine() ){

		const auto& mm = s.lastText() ;

		auto w = mm.indexOf( ' ' ) ;

		if( w != -1 ){

			for( ; w < mm.size() ; w++ ){

				if( mm[ w ] != ' ' ){

					break ;
				}
			}
		}else{
			w = 0 ;
		}

		m_tmp = mm.mid( w ) ;

		aria2c::trimProgressLine( m_tmp ) ;

		return m_tmp ;
	}

	if( m_fileName.isEmpty() ){

		return m_preProcessing.text() ;
	}else{
		return m_fileName ;
	}
}

aria2c::aria2c_dlFilter::~aria2c_dlFilter()
{
}

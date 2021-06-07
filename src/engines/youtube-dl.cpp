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

#include "../downloadmanager.h"

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

		mainObj.insert( "CommandName","youtube-dl" ) ;

		mainObj.insert( "CommandNameWindows","youtube-dl.exe" ) ;

		mainObj.insert( "Name","youtube-dl" ) ;

		mainObj.insert( "CookieArgument","--cookies" ) ;

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

		mainObj.insert( "ShowListTableBoundary",[](){

			QJsonObject obj ;

			obj.insert( "ColumnNumber","0" ) ;
			obj.insert( "Comparator","equals" ) ;
			obj.insert( "String","format" ) ;

			return obj ;
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

		mainObj.insert( "PlayListIdArguments",[](){

			QJsonArray arr ;

			arr.append( "--get-id" ) ;
			arr.append( "--get-title" ) ;

			return arr ;
		}() ) ;

		mainObj.insert( "RequiredMinimumVersionOfMediaDownloader",QString() ) ;

		mainObj.insert( "PlayListUrlPrefix","https://youtube.com/watch?v=" ) ;

		mainObj.insert( "PlaylistItemsArgument","--playlist-items" ) ;

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

		mainObj.insert( "ReplaceOutputWithProgressReport",false ) ;

		engines::file( m,logger ).write( mainObj ) ;
	}
}

youtube_dl::youtube_dl( settings& s ) :
	engines::engine::functions( s )
{
}

youtube_dl::~youtube_dl()
{
}

void youtube_dl::updateOptions( QJsonObject& object,settings& settings )
{
	if( !object.contains( "CookieArgument" ) ){

		object.insert( "CookieArgument","--cookies" ) ;
	}

	if( !object.contains( "SkipLineWithText" ) ){

		object.insert( "SkipLineWithText",[](){

			QJsonArray arr ;

			arr.append( "(pass -k to keep)" ) ;

			return arr ;
		}() ) ;
	}

	if( !object.contains( "PlayListIdArguments" ) ){

		object.insert( "PlayListIdArguments",[](){

			QJsonArray arr ;

			arr.append( "--get-id" ) ;
			arr.append( "--get-title" ) ;

			return arr ;
		}() ) ;
	}

	if( !object.contains( "ControlJsonStructure" ) ){

		object.insert( "ControlJsonStructure",_defaultControlStructure() ) ;
	}

	if( !object.contains( "PlayListIdArgument" ) ){

		object.insert( "PlayListIdArgument","--get-id" ) ;
	}

	if( !object.contains( "PlaylistItemsArgument" ) ){

		object.insert( "PlaylistItemsArgument","--playlist-items" ) ;
	}

	if( !object.contains( "PlayListUrlPrefix" ) ){

		object.insert( "PlayListUrlPrefix","https://youtube.com/watch?v=" ) ;
	}

	object.insert( "UsePrivateExecutable",!settings.useSystemProvidedVersionIfAvailable() ) ;
}

std::unique_ptr< engines::engine::functions::filter > youtube_dl::Filter( const QString& e )
{
	return std::make_unique< youtube_dl::youtube_dlFilter >( e ) ;
}

void youtube_dl::runCommandOnDownloadedFile( const QString& e,const QString& )
{
	auto& settings = engines::engine::functions::Settings() ;
	auto a = settings.commandOnSuccessfulDownload() ;

	if( !a.isEmpty() && !e.isEmpty() ){

		auto args = utility::split( a,' ',true ) ;

		auto b = settings.downloadFolder() + "/" + utility::split( e,'\n',true ).at( 0 ) ;

		if( QFile::exists( b ) ){

			args.append( b ) ;

			auto exe = args.takeAt( 0 ) ;

			QProcess::startDetached( exe,args ) ;
		}
	}
}

QString youtube_dl::updateTextOnCompleteDownlod( const engines::engine&,
						 const QString& uiText,
						 const QString& bkText,
						 const engines::engine::functions::finishedState& f )
{
	auto m = engines::engine::functions::processCompleteStateText( f ) ;
	auto e = engines::engine::functions::timer::stringElapsedTime( f.duration() ) ;

	if( f.cancelled() ){

		return engines::engine::functions::updateTextOnCompleteDownlod( bkText,f ) ;

	}else if( f.success() ){

		QStringList a ;

		for( const auto& it : utility::split( uiText,'\n',true ) ){

			if( !it.contains( engines::engine::functions::postProcessing::processingText() ) ){

				a.append( it ) ;
			}
		}

		return engines::engine::functions::updateTextOnCompleteDownlod( a.join( "\n" ),f ) ;
	}else{
		return engines::engine::functions::updateTextOnCompleteDownlod( uiText,f ) ;
	}
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

youtube_dl::youtube_dlFilter::youtube_dlFilter( const QString& e ) :
	engines::engine::functions::filter( e )
{
}

const QString& youtube_dl::youtube_dlFilter::operator()( const engines::engine&,
							 const Logger::Data& s )
{
	int downloadCounter = 0 ;

	const auto data = s.toStringList() ;

	for( const auto& e : data ){

		if( e.startsWith( "ERROR: " ) ){

			m_tmp = e ;
			return m_tmp ;
		}
		if( e.startsWith( "[download] " ) && e.contains( " has already been downloaded" ) ){

			m_fileName = e.mid( e.indexOf( " " ) + 1 ) ;
			m_fileName.truncate( m_fileName.indexOf( " has already been downloaded" ) ) ;
			return m_fileName ;
		}
		if( e.contains( "] Destination: " ) ){

			m_fileName = e.mid( e.indexOf( "] Destination: " ) + 15 ) ;
		}
		if( e.contains( " Merging formats into \"" ) ){

			m_fileName = e.mid( e.indexOf( "\"" ) + 1 ) ;
			m_fileName.truncate( m_fileName.size() - 1 ) ;
		}
		if( e.startsWith( "[download] 100% of " ) ){

			downloadCounter++ ;
		}
	}

	const auto& l = data.last() ;

	if( l.startsWith( "[download]  " ) && l.contains( " ETA " ) ){

		m_tmp = l ;
		m_tmp.replace( "[download]  ","" ) ;
		m_tmp = m_fileName + "\n" + m_tmp ;

		return m_tmp ;
	}

	if( downloadCounter == 0 ){

		return m_preProcessing.text() ;
	}else{
		return m_postProcessing.text( m_fileName ) ;
	}
}

youtube_dl::youtube_dlFilter::~youtube_dlFilter()
{
}

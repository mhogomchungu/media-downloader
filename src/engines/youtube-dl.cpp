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

#include "aria2c.h"

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

void youtube_dl::init( const QString& name,
		       const QString& configFileName,
		       Logger& logger,
		       const engines::enginePaths& enginePath )
{
	auto m = enginePath.enginePath( configFileName ) ;

	if( !QFile::exists( m ) ){

		QJsonObject mainObj ;

		if( name == "youtube-dl" ){

			mainObj.insert( "ShowListTableBoundary",[](){

				QJsonObject obj ;

				obj.insert( "ColumnNumber","0" ) ;
				obj.insert( "Comparator","equals" ) ;
				obj.insert( "String","format" ) ;

				return obj ;
			}() ) ;

			mainObj.insert( "ShowListTableExtraBoundaries",QJsonArray() ) ;

			mainObj.insert( "CommandName","youtube-dl" ) ;

			mainObj.insert( "CommandNameWindows","youtube-dl.exe" ) ;

			mainObj.insert( "DownloadUrl","https://api.github.com/repos/ytdl-org/youtube-dl/releases/latest" ) ;
		}else{
			mainObj.insert( "ShowListTableBoundary",[](){

				QJsonObject obj ;

				obj.insert( "ColumnNumber","0" ) ;
				obj.insert( "Comparator","contains" ) ;
				obj.insert( "String","--" ) ;

				return obj ;
			}() ) ;

			mainObj.insert( "ShowListTableExtraBoundaries",[](){

				QJsonArray arr ;

				QJsonObject obj ;

				obj.insert( "ColumnNumber","0" ) ;
				obj.insert( "Comparator","equals" ) ;
				obj.insert( "String","format" ) ;

				arr.append( obj ) ;

				return arr ;
			}() ) ;

			mainObj.insert( "CommandName","yt-dlp" ) ;

			mainObj.insert( "CommandNameWindows","yt-dlp.exe" ) ;

			mainObj.insert( "CommandName32BitWindows","yt-dlp_x86.exe" ) ;

			mainObj.insert( "DownloadUrl","https://api.github.com/repos/yt-dlp/yt-dlp/releases/latest" ) ;
		}

		mainObj.insert( "DefaultListCmdOptions",[](){

			QJsonArray arr ;
			arr.append( "-F" ) ;

			return arr ;
		}() ) ;

		mainObj.insert( "Name",name ) ;

		mainObj.insert( "CookieArgument","--cookies" ) ;

		mainObj.insert( "DefaultDownLoadCmdOptions",[](){

			QJsonArray arr ;

			arr.append( "--newline" ) ;
			arr.append( "--ignore-config" ) ;
			arr.append( "--no-playlist" ) ;
			arr.append( "-o" ) ;
			arr.append( "%(title)s-%(id)s.%(ext)s" ) ;

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

		mainObj.insert( "PlayListIdArguments",[](){

			QJsonArray arr ;

			arr.append( "--get-id" ) ;
			arr.append( "--get-title" ) ;
			arr.append( "--get-thumbnail" ) ;

			return arr ;
		}() ) ;

		mainObj.insert( "RequiredMinimumVersionOfMediaDownloader",QString() ) ;

		mainObj.insert( "PlaylistItemsArgument","--playlist-items" ) ;

		mainObj.insert( "ControlJsonStructure",_defaultControlStructure() ) ;

		mainObj.insert( "VersionArgument","--version" ) ;

		mainObj.insert( "OptionsArgument","-f" ) ;

		mainObj.insert( "BackendPath",utility::stringConstants::defaultPath() ) ;

		mainObj.insert( "VersionStringLine",0 ) ;

		mainObj.insert( "VersionStringPosition",0 ) ;

		mainObj.insert( "BatchFileArgument","-a" ) ;

		mainObj.insert( "CanDownloadPlaylist",true ) ;

		mainObj.insert( "LikeYoutubeDl",true ) ;

		mainObj.insert( "ReplaceOutputWithProgressReport",false ) ;

		engines::file( m,logger ).write( mainObj ) ;
	}
}

youtube_dl::youtube_dl( const engines& engines,const engines::engine& engine,QJsonObject& object ) :
	engines::engine::functions( engines.Settings(),engine ),
	m_engines( engines ),m_engine( engine )
{
	if( !object.contains( "ShowListTableBoundary" ) ){

		QJsonObject obj ;

		if( m_engine.name() == "youtube-dl" ){

			obj.insert( "ColumnNumber","0" ) ;
			obj.insert( "Comparator","equals" ) ;
			obj.insert( "String","format" ) ;
		}else{
			obj.insert( "ColumnNumber","0" ) ;
			obj.insert( "Comparator","contains" ) ;
			obj.insert( "String","--" ) ;
		}

		object.insert( "ShowListTableBoundary",obj ) ;
	}

	if( !object.contains( "ShowListTableExtraBoundaries" ) ){

		if( m_engine.name() == "youtube-dl" ){

			object.insert( "ShowListTableExtraBoundaries",QJsonArray() ) ;
		}else{
			object.insert( "ShowListTableExtraBoundaries",[](){

				QJsonArray arr ;

				QJsonObject obj ;

				obj.insert( "ColumnNumber","0" ) ;
				obj.insert( "Comparator","equals" ) ;
				obj.insert( "String","format" ) ;

				arr.append( obj ) ;

				return arr ;
			}() ) ;
		}
	}

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
			arr.append( "--get-thumbnail" ) ;

			return arr ;
		}() ) ;
	}

	if( !object.contains( "LikeYoutubeDl" ) ){

		object.insert( "LikeYoutubeDl",true ) ;
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

	object.insert( "UsePrivateExecutable",!engines.Settings().useSystemProvidedVersionIfAvailable() ) ;

	m_objs = object.value( "ShowListTableExtraBoundaries" ).toArray() ;
	m_objs.insert( 0,object.value( "ShowListTableBoundary" ).toObject() ) ;
}

youtube_dl::~youtube_dl()
{
}

bool youtube_dl::breakShowListIfContains( const QStringList& e )
{
	auto _match_found = []( const QJsonObject& obj,const QStringList& e ){

		auto a    = obj.value( "ColumnNumber" ).toString() ;
		auto cmp  = obj.value( "Comparator" ).toString() ;
		auto text = obj.value( "String" ).toString() ;

		if( !a.isEmpty() && !cmp.isEmpty() && !text.isEmpty() ){

			bool valid ;
			auto number = a.toInt( &valid ) ;

			if( valid && number < e.size() ){

				if( cmp == "equals" ){

					return text == e[ number ] ;

				}else if( cmp == "contains" ){

					return e[ number ].contains( text ) ;
				}
			}
		}

		return false ;
	} ;

	if( m_objs.size() == 0 ){

		if( e.size() > 1 ){

			return e.at( 0 ) == "format" || e.at( 2 ).contains( "-" ) ;
		}else{
			return false ;
		}
	}else{
		for( const auto& it : util::asConst( m_objs ) ){

			if( it.isObject() && _match_found( it.toObject(),e ) ){

				return true ;
			}
		}

		return false ;
	}
}

engines::engine::functions::DataFilter youtube_dl::Filter( const QString& e )
{
	return { util::types::type_identity< youtube_dl::youtube_dlFilter >(),e,m_engine } ;
}

void youtube_dl::runCommandOnDownloadedFile( const QString& e,const QString& )
{
	auto& settings = engines::engine::functions::Settings() ;
	auto a = settings.commandOnSuccessfulDownload() ;

	if( !a.isEmpty() && !e.isEmpty() ){

		auto args = util::split( a,' ',true ) ;

		auto b = settings.downloadFolder() + "/" + util::split( e,'\n',true ).at( 0 ) ;

		if( QFile::exists( b ) ){

			args.append( b ) ;

			auto exe = args.takeAt( 0 ) ;

			QProcess::startDetached( exe,args ) ;
		}
	}
}

QString youtube_dl::updateTextOnCompleteDownlod( const QString& uiText,
						 const QString& bkText,
						 const engines::engine::functions::finishedState& f )
{
	auto m = engines::engine::functions::processCompleteStateText( f ) ;
	auto e = engines::engine::functions::timer::stringElapsedTime( f.duration() ) ;

	if( f.cancelled() ){

		return engines::engine::functions::updateTextOnCompleteDownlod( bkText,f ) ;

	}else if( f.success() ){

		QStringList a ;

		for( const auto& it : util::split( uiText,'\n',true ) ){

			if( !it.contains( engines::engine::functions::postProcessing::processingText() ) ){

				a.append( it ) ;
			}
		}

		return engines::engine::functions::updateTextOnCompleteDownlod( a.join( "\n" ),f ) ;
	}else{
		return engines::engine::functions::updateTextOnCompleteDownlod( uiText,f ) ;
	}
}

void youtube_dl::updateDownLoadCmdOptions( const engines::engine::functions::updateOpts& s )
{
	if( s.userOptions.contains( "--yes-playlist" ) ){

		s.ourOptions.removeAll( "--no-playlist" ) ;
	}

	if( !s.ourOptions.contains( "--newline" ) ){

		s.ourOptions.append( "--newline" ) ;
	}

	s.ourOptions.append( m_engine.optionsArgument() ) ;

	if( s.quality.isEmpty() ){

		s.ourOptions.append( "bestvideo+bestaudio/best" ) ;
	}else{
		s.ourOptions.append( s.quality ) ;
	}

	if( !s.indexAsString.isEmpty() ){

		for( int m = 0 ; m < s.ourOptions.size() ; m++ ){

			if( s.ourOptions[ m ] == "-o" ){

				if( m + 1 < s.ourOptions.size() ){

					auto& e = s.ourOptions[ m + 1 ] ;

					e.replace( "%(autonumber)s",s.indexAsString ) ;
					e.replace( "%(playlist_index)s",s.indexAsString ) ;

					break ;
				}
			}
		}
	}
}

youtube_dl::youtube_dlFilter::youtube_dlFilter( const QString& e,const engines::engine& engine ) :
	engines::engine::functions::filter( e,engine )
{
}

const QString& youtube_dl::youtube_dlFilter::operator()( const Logger::Data& s )
{
	int downloadCounter = 0 ;

	const auto data = s.toStringList() ;

	for( const auto& e : data ){

		if( e.startsWith( "ERROR: " ) || e.startsWith( "yt-dlp: error:" ) ){

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
		if( e.contains( "has already been recorded in archive" ) ){

			m_tmp = engines::engine::mediaAlreadInArchiveText() ;

			return m_tmp ;
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

		m_tmp = m_fileName + "\n" + mm.mid( w ) ;

		const auto& engine = engines::engine::functions::filter::engine() ;

		if( engine.name() == "yt-dlp-aria2c" ){

			aria2c::trimProgressLine( m_tmp ) ;
		}

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

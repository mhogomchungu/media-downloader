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

QJsonObject youtube_dl::init( const QString& name,
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

			mainObj.insert( "CommandName","youtube-dl" ) ;

			mainObj.insert( "CommandNameWindows","youtube-dl.exe" ) ;

			mainObj.insert( "DownloadUrl","https://api.github.com/repos/ytdl-org/youtube-dl/releases/latest" ) ;

			mainObj.insert( "DefaultListCmdOptions",[](){

				QJsonArray arr ;
				arr.append( "-F" ) ;

				return arr ;
			}() ) ;
		}else{
			mainObj.insert( "DefaultListCmdOptions",[](){

				QJsonArray arr ;

				arr.append( "--print" ) ;
				arr.append( "%(formats)j" ) ;

				return arr ;
			}() ) ;

			mainObj.insert( "CommandName","yt-dlp" ) ;

			mainObj.insert( "CommandNameWindows","yt-dlp.exe" ) ;

			mainObj.insert( "CommandName32BitWindows","yt-dlp_x86.exe" ) ;

			mainObj.insert( "DownloadUrl","https://api.github.com/repos/yt-dlp/yt-dlp/releases/latest" ) ;
		}

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

		mainObj.insert( "RequiredMinimumVersionOfMediaDownloader","2.1.1" ) ;

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

		return mainObj ;
	}else{
		return QJsonObject() ;
	}
}

youtube_dl::youtube_dl( const engines& engines,
			const engines::engine& engine,
			QJsonObject& obj,
			Logger& logger,
			const engines::enginePaths& enginePath ) :
	engines::engine::functions( engines.Settings(),engine ),
	m_engine( engine )
{
	auto name = obj.value( "Name" ).toString() ;

	if( name == "youtube-dl" || name == "yt-dlp" ){

		auto version = obj.value( "RequiredMinimumVersionOfMediaDownloader" ).toString() ;

		if( version.isEmpty() || util::version( version ) < "2.1.1" ){

			auto configFileName = name + ".json" ;

			auto m = enginePath.enginePath( configFileName ) ;

			QFile::remove( m ) ;

			while( QFile::exists( m ) ){}

			obj = youtube_dl::init( name,configFileName,logger,enginePath ) ;
		}
	}
}

youtube_dl::~youtube_dl()
{
}

std::vector< QStringList > youtube_dl::mediaProperties( const QByteArray& e )
{
	if( !m_engine.name().contains( "yt-dlp" ) ){

		return engines::engine::functions::mediaProperties( e ) ;
	}

	QJsonParseError err ;

	auto json = QJsonDocument::fromJson( e,&err ) ;

	if( err.error == QJsonParseError::NoError ){

		std::vector< QStringList > m ;
		std::vector< QStringList > mm ;

		const auto array = json.array() ;

		utility::locale s ;

		auto _append = [ & ]( QString& s,const char * str,const QString& sstr ){

			if( sstr == "none" || sstr == "0" ){

				return ;
			}

			s += str + sstr + ", " ;
		} ;

		for( const auto& it : array ){

			auto obj       = it.toObject() ;

			auto id        = obj.value( "format_id" ).toString() ;
			auto ext       = obj.value( "ext" ).toString() ;
			auto rsn       = obj.value( "resolution" ).toString() ;

			auto fileSize  = s.formattedDataSize( obj.value( "filesize" ).toInt() ) ;
			auto tbr       = QString::number( obj.value( "tbr" ).toDouble() ) ;
			auto vbr       = QString::number( obj.value( "vbr" ).toDouble() ) ;

			auto container = obj.value( "container" ).toString() ;
			auto proto     = obj.value( "protocol" ).toString() ;
			auto vcodec    = obj.value( "vcodec" ).toString() ;
			auto video_ext = obj.value( "video_ext" ).toString() ;
			auto acodec    = obj.value( "acodec" ).toString() ;
			auto audio_ext = obj.value( "audio_ext" ).toString() ;

			if( acodec == "none" && !rsn.isEmpty() && rsn != "audio only" ){

				rsn += "\nvideo only" ;
			}

			QString s ;

			if( container.isEmpty() ){

				s = QString( "Proto: %1, File Size: %2\n" ).arg( proto,fileSize ) ;
			}else{
				s = QString( "Proto: %1, File Size: %2\ncontainer: %3\n" ).arg( proto,fileSize,container ) ;
			}

			_append( s,"acodec: ",acodec ) ;
			_append( s,"vcodec: ",vcodec ) ;
			_append( s,"tbr: ",tbr ) ;
			_append( s,"vbr: ",vbr ) ;

			if( s.endsWith( ", " ) ){

				s.truncate( s.size() - 2 ) ;
			}

			if( rsn != "audio only" && !rsn.contains( "video only" ) ){

				mm.emplace_back( QStringList{ id,ext,rsn,s } ) ;
			}else{
				m.emplace_back( QStringList{ id,ext,rsn,s } ) ;
			}
		}

		for( auto& it : mm ){

			m.emplace_back( std::move( it ) ) ;
		}

		return m ;
	}else{
		return {} ;
	}
}

QStringList youtube_dl::dumpJsonArguments()
{
	if( m_engine.name() == "youtube-dl" ){

		return engines::engine::functions::dumpJsonArguments() ;
	}else{
		//auto a = R"R({"url":%(url)j,"id":%(id)j,"thumbnail":%(thumbnail)j,"duration":%(duration)j,"title":%(title)j,"upload_date":%(upload_date)j,"webpage_url":%(webpage_url)j})R" ;
		auto a = R"R({"id":%(id)j,"thumbnail":%(thumbnail)j,"duration":%(duration)j,"title":%(title)j,"upload_date":%(upload_date)j,"webpage_url":%(webpage_url)j})R" ;

		return { "--newline","--print",a } ;
	}
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

		auto b = settings.downloadFolder() + "/" + util::split( e,'\n',true ).at( 0 ) ;

		if( QFile::exists( b ) ){

			auto args = util::split(a, ' ', true);

			args.append( b ) ;

			auto exe = args.takeAt( 0 ) ;

			QProcess::startDetached( exe,args ) ;
		}
	}
}

QString youtube_dl::updateTextOnCompleteDownlod( const QString& uiText,
						 const QString& bkText,
						 const QString& dopts,
						 const engines::engine::functions::finishedState& f )
{
	if( f.cancelled() ){

		return engines::engine::functions::updateTextOnCompleteDownlod( bkText,dopts,f ) ;

	}else if( f.success() ){

		QStringList a ;

		for( const auto& it : util::split( uiText,'\n',true ) ){

			auto x = engines::engine::functions::postProcessing::processingText() ;
			auto y = engines::engine::functions::preProcessing::processingText() ;

			if( !it.contains( x ) && !it.contains( y ) ){

				a.append( it ) ;
			}
		}

		return engines::engine::functions::updateTextOnCompleteDownlod( a.join( "\n" ),dopts,f ) ;
	}else{
		return engines::engine::functions::updateTextOnCompleteDownlod( uiText,dopts,f ) ;
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

	if( !s.quality.isEmpty() && s.quality.compare( "Default",Qt::CaseInsensitive ) ){

		s.ourOptions.append( m_engine.optionsArgument() ) ;

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

	if( m_engine.name().contains( "yt-dlp" ) ){

		while( s.ourOptions.contains( "--progress-template" ) ){

			utility::arguments( s.ourOptions ).removeOptionWithArgument( "--progress-template" ) ;
		}

		s.ourOptions.append( "--progress-template" ) ;
		s.ourOptions.append( "download:[download] %(progress._percent_str)s of %(progress._total_bytes_str)s at %(progress._speed_str)s ETA %(progress._eta_str)s" ) ;
		s.ourOptions.append( "--progress-template" ) ;
		s.ourOptions.append( "postprocess:postprocessing" ) ;
	}
}

youtube_dl::youtube_dlFilter::youtube_dlFilter( const QString& e,const engines::engine& engine ) :
	engines::engine::functions::filter( e,engine ),
	m_likeYtdlp( engine.name().contains( "yt-dlp" ) )
{
}

const QByteArray& youtube_dl::youtube_dlFilter::operator()( const Logger::Data& s )
{
	if( m_likeYtdlp ){

		return this->ytdlpOutput( s ) ;
	}else{
		return this->youtubedlOutput( s ) ;
	}
}

youtube_dl::youtube_dlFilter::~youtube_dlFilter()
{
}

const QByteArray& youtube_dl::youtube_dlFilter::youtubedlOutput( const Logger::Data& s )
{
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
		if( e.contains( "has already been recorded in archive" ) ){

			m_tmp = engines::engine::mediaAlreadInArchiveText().toUtf8() ;

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

		return m_tmp ;
	}

	if( m_fileName.isEmpty() ){

		return m_preProcessing.text() ;
	}else{
		return m_preProcessing.text( m_fileName ) ;
	}
}

const QByteArray& youtube_dl::youtube_dlFilter::ytdlpOutput( const Logger::Data& s )
{
	bool downloadingCompleted = false ;

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
		if( e.contains( "has already been recorded in archive" ) ){

			m_tmp = engines::engine::mediaAlreadInArchiveText().toUtf8() ;

			return m_tmp ;
		}
		if( e == "postprocessing" ){

			downloadingCompleted = true ;
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

	if( downloadingCompleted ){

		return m_postProcessing.text( m_fileName ) ;
	}else{
		return m_preProcessing.text() ;
	}
}

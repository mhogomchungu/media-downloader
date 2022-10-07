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

#include "yt-dlp.h"

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

#include "../networkAccess.h"
#include "../utility.h"

#include "../downloadmanager.h"

#include "aria2c.h"

const char * yt_dlp::testData()
{
	return R"R([youtube] Gkz9F4GGxOs: Downloading webpage
[youtube] Gkz9F4GGxOs: Downloading android player API JSON
[info] Gkz9F4GGxOs: Downloading 1 format(s): 242+250
[download] Destination: Так мы ещё не тонули) хорошее начало гонки) [Gkz9F4GGxOs].f242.webm
[download]   0.0% of 42.90MiB at   22.04KiB/s ETA 33:16
[download]   0.0% of 42.90MiB at   63.79KiB/s ETA 11:28
[download]   0.0% of 42.90MiB at  146.31KiB/s ETA 05:00
[download]   0.0% of 42.90MiB at  309.76KiB/s ETA 02:21
[download]   0.1% of 42.90MiB at  350.52KiB/s ETA 02:05
[download]   0.1% of 42.90MiB at  461.81KiB/s ETA 01:35
[download]   0.3% of 42.90MiB at  410.63KiB/s ETA 01:46
[download]   0.6% of 42.90MiB at  241.69KiB/s ETA 03:00
[download]   1.0% of 42.90MiB at  164.48KiB/s ETA 04:24
[download]   1.2% of 42.90MiB at  166.03KiB/s ETA 04:21
[download]   1.6% of 42.90MiB at  171.56KiB/s ETA 04:11
[download]   2.1% of 42.90MiB at  176.84KiB/s ETA 04:03
[download]   2.5% of 42.90MiB at  178.79KiB/s ETA 03:59
[download]   2.9% of 42.90MiB at  186.78KiB/s ETA 03:48
[download]  69.1% of 42.90MiB at  152.17KiB/s ETA 01:29
[download]  99.5% of 42.90MiB at  135.25KiB/s ETA 00:01
[download]  99.7% of 42.90MiB at  134.26KiB/s ETA 00:01
[download]  99.9% of 42.90MiB at  134.83KiB/s ETA 00:00
[download] 100.0% of 42.90MiB at  134.53KiB/s ETA 00:00
[download] 100% of 42.90MiB in 04:55
[download] Destination: Так мы ещё не тонули) хорошее начало гонки) [Gkz9F4GGxOs].f250.webm
[download]   0.0% of 11.47MiB at    7.37KiB/s ETA 26:34
[download]   0.0% of 11.47MiB at   21.91KiB/s ETA 08:56
[download]   0.1% of 11.47MiB at   50.74KiB/s ETA 03:51
[download]   0.1% of 11.47MiB at  107.71KiB/s ETA 01:48
[download]   0.3% of 11.47MiB at  130.00KiB/s ETA 01:30
[download]  95.4% of 11.47MiB at  137.81KiB/s ETA 00:03
[download]  96.6% of 11.47MiB at  138.09KiB/s ETA 00:02
[download]  97.8% of 11.47MiB at  138.81KiB/s ETA 00:01
[download]  99.0% of 11.47MiB at  136.19KiB/s ETA 00:00
[download] 100.0% of 11.47MiB at  136.42KiB/s ETA 00:00
[download] 100% of 11.47MiB in 01:08
[Merger] Merging formats into "Так мы ещё не тонули) хорошее начало гонки) [Gkz9F4GGxOs].webm"
Deleting original file Так мы ещё не тонули) хорошее начало гонки) [Gkz9F4GGxOs].f250.webm (pass -k to keep)
Deleting original file Так мы ещё не тонули) хорошее начало гонки) [Gkz9F4GGxOs].f242.webm (pass -k to keep))R" ;
}

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

QJsonObject yt_dlp::init( const QString& name,
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

			utility::addJsonCmd json( mainObj ) ;

			json.add( { { "Generic" },{ { "x86","youtube-dl",{ "youtube-dl" } },
						    { "amd64","youtube-dl",{ "youtube-dl" } } } } ) ;

			json.add( { { "Windows" },{ { "x86","youtube-dl.exe",{ "youtube-dl.exe" } },
						    { "amd64","youtube-dl.exe",{ "youtube-dl.exe" } } } } ) ;

			json.done() ;

			mainObj.insert( "DownloadUrl","https://api.github.com/repos/ytdl-org/youtube-dl/releases/latest" ) ;

			mainObj.insert( "DefaultListCmdOptions",[](){

				QJsonArray arr ;
				arr.append( "-F" ) ;

				return arr ;
			}() ) ;
		}else{
			utility::addJsonCmd json( mainObj ) ;

			json.add( { { "Generic" },{ { "x86","yt-dlp",{ "yt-dlp" } },
						    { "amd64","yt-dlp",{ "yt-dlp" } } } } ) ;

			json.add( { { "Windows" },{ { "x86","yt-dlp_x86.exe",{ "yt-dlp_x86.exe" } },
						    { "amd64","yt-dlp.exe",{ "yt-dlp.exe" } } } } ) ;

			json.done() ;

			mainObj.insert( "DefaultListCmdOptions",[](){

				QJsonArray arr ;

				arr.append( "--print" ) ;
				arr.append( "%(formats)j" ) ;

				return arr ;
			}() ) ;

			mainObj.insert( "DefaultCommentsCmdOptions",[](){

				QJsonArray arr ;

				arr.append( "--get-comments" ) ;
				arr.append( "--no-download" ) ;
				arr.append( "--print" ) ;
				arr.append( "{\"title\":%(title)j,\"comments\":%(comments)j}" ) ;

				return arr ;
			}() ) ;

			mainObj.insert( "DefaultSubstitlesCmdOptions",[](){

				QJsonArray arr ;

				arr.append( "--no-download" ) ;
				arr.append( "--print" ) ;
				arr.append( "{\"title\":%(title)j,\"automatic_captions\":%(automatic_captions)j,\"subtitles\":%(subtitles)j}" ) ;

				return arr ;
			}() ) ;

			mainObj.insert( "DefaultSubtitleDownloadOptions",[](){

				QJsonArray arr ;

				arr.append( "--embed-subs" ) ;

				return arr ;
			}() ) ;

			mainObj.insert( "DownloadUrl","https://api.github.com/repos/yt-dlp/yt-dlp/releases/latest" ) ;
		}

		mainObj.insert( "RequiredMinimumVersionOfMediaDownloader","2.2.0" ) ;

		mainObj.insert( "Name",name ) ;

		mainObj.insert( "CookieArgument","--cookies" ) ;

		mainObj.insert( "DefaultDownLoadCmdOptions",[](){

			QJsonArray arr ;

			arr.append( "--newline" ) ;
			arr.append( "--ignore-config" ) ;
			arr.append( "--no-playlist" ) ;
			arr.append( "-o" ) ;
			arr.append( "%(title).200s-%(id)s.%(ext)s" ) ;

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

yt_dlp::yt_dlp( const engines& engines,
		const engines::engine& engine,
		QJsonObject& obj,
		Logger& logger,
		const engines::enginePaths& enginePath,
		const util::version& version ) :
	engines::engine::functions( engines.Settings(),engine,engines.processEnvironment() ),
	m_engine( engine ),
	m_version( version ),
	m_supportsLazyPlaylist( "2022.06.22" )
{
	auto name = obj.value( "Name" ).toString() ;

	if( name == "youtube-dl" || name == "yt-dlp" ){

		if( obj.value( "Cmd" ).isUndefined() ){

			auto configFileName = name + ".json" ;

			auto m = enginePath.enginePath( configFileName ) ;

			QFile::remove( m ) ;

			obj = yt_dlp::init( name,configFileName,logger,enginePath ) ;
		}
	}

	if( name.contains( "yt-dlp" ) ){

		obj.insert( "DefaultListCmdOptions",[](){

			QJsonArray arr ;

			arr.append( "--print" ) ;
			arr.append( "%(formats)j" ) ;

			return arr ;
		}() ) ;

		if( !obj.contains( "DefaultCommentsCmdOptions" ) ){

			obj.insert( "DefaultCommentsCmdOptions",[](){

				QJsonArray arr ;

				arr.append( "--get-comments" ) ;
				arr.append( "--no-download" ) ;
				arr.append( "--print" ) ;
				arr.append( "{\"title\":%(title)j,\"comments\":%(comments)j}" ) ;

				return arr ;
			}() ) ;
		}

		if( !obj.contains( "DefaultSubstitlesCmdOptions" ) ){

			obj.insert( "DefaultSubstitlesCmdOptions",[](){

				QJsonArray arr ;

				arr.append( "--no-download" ) ;
				arr.append( "--print" ) ;
				arr.append( "{\"title\":%(title)j,\"automatic_captions\":%(automatic_captions)j,\"subtitles\":%(subtitles)j}" ) ;

				return arr ;
			}() ) ;
		}

		if( !obj.contains( "DefaultSubtitleDownloadOptions" ) ){

			obj.insert( "DefaultSubtitleDownloadOptions",[](){

				QJsonArray arr ;

				arr.append( "--embed-subs" ) ;

				return arr ;
			}() ) ;
		}
	}else{
		if( !obj.contains( "DefaultListCmdOptions" ) ){

			obj.insert( "DefaultListCmdOptions",[](){

				QJsonArray arr ;
				arr.append( "-F" ) ;

				return arr ;
			}() ) ;
		}
	}
}

yt_dlp::~yt_dlp()
{
}

std::vector< engines::engine::functions::mediaInfo > yt_dlp::mediaProperties( const QByteArray& e )
{
	if( m_engine.name() == "youtube-dl" ){

		return engines::engine::functions::mediaProperties( e ) ;

	}else if( m_engine.name().contains( "yt-dlp" ) ){

		QJsonParseError err ;

		auto json = QJsonDocument::fromJson( e,&err ) ;

		if( err.error == QJsonParseError::NoError ){

			return this->mediaProperties( json.array() ) ;
		}else{
			return {} ;
		}
	}else{
		return {} ;
	}
}

std::vector< engines::engine::functions::mediaInfo > yt_dlp::mediaProperties( const QJsonArray& array )
{
	if( array.isEmpty() ){

		return {} ;
	}

	if( m_engine.name() == "youtube-dl" ){

		return engines::engine::functions::mediaProperties( array ) ;
	}

	std::vector< engines::engine::functions::mediaInfo > firstToShow ;
	std::vector< engines::engine::functions::mediaInfo > secondToShow ;
	std::vector< engines::engine::functions::mediaInfo > thirdtToShow ;

	utility::locale s ;

	enum class mediaType{ audioOnly,videoOnly,audioVideo,unknown } ;

	auto _append = [ & ]( QString& s,const char * str,const QString& sstr,bool formatBitrate ){

		if( sstr == "none" || sstr.isEmpty() ){

			return ;
		}

		if( formatBitrate ){

			auto m = sstr.indexOf( '.' ) ;

			if( m == -1 ){

				s += str + sstr + "k, " ;
			}else{
				s += str + sstr.mid( 0,m ) + "k, " ;
			}
		}else{
			s += str + sstr + ", " ;
		}
	} ;

	for( const auto& it : array ){

		auto obj       = it.toObject() ;

		auto url       = obj.value( "url" ).toString() ;
		auto id        = obj.value( "format_id" ).toString() ;
		auto ext       = obj.value( "ext" ).toString() ;
		auto rsn       = obj.value( "resolution" ).toString() ;

		auto fileSize  = s.formattedDataSize( obj.value( "filesize" ).toInt() ) ;
		auto tbr       = QString::number( obj.value( "tbr" ).toDouble() ) ;
		auto vbr       = QString::number( obj.value( "vbr" ).toDouble() ) ;
		auto abr       = QString::number( obj.value( "abr" ).toDouble() ) ;
		auto asr       = QString::number( obj.value( "asr" ).toInt() ) ;

		auto container = obj.value( "container" ).toString() ;
		auto proto     = obj.value( "protocol" ).toString() ;
		auto vcodec    = obj.value( "vcodec" ).toString() ;
		//auto video_ext = obj.value( "video_ext" ).toString() ;
		auto acodec    = obj.value( "acodec" ).toString() ;
		//auto audio_ext = obj.value( "audio_ext" ).toString() ;
		auto fmtNotes  = obj.value( "format_note" ).toString() ;

		mediaType mt = mediaType::unknown ;

		if( rsn.isEmpty() ){

			rsn = fmtNotes ;
		}else{
			if( rsn == "audio only" ){

				mt = mediaType::audioOnly ;
			}else{
				bool hasVideo = vcodec != "none" ;
				bool hasAudio = acodec != "none" ;

				if( hasVideo && hasAudio ){

					rsn += "\naudio video" ;

					mt = mediaType::audioVideo ;

				}else if( hasVideo && !hasAudio ){

					rsn += "\nvideo only" ;

					mt = mediaType::videoOnly ;

				}else if( !hasVideo && hasAudio ){

					rsn += "\naudio only" ;

					mt = mediaType::audioOnly ;
				}
			}

			rsn += "\n" + fmtNotes ;
		}

		QString s ;

		if( container.isEmpty() ){

			s = QString( "Proto: %1, File Size: %2\n" ).arg( proto,fileSize ) ;
		}else{
			s = QString( "Proto: %1, File Size: %2\ncontainer: %3\n" ).arg( proto,fileSize,container ) ;
		}

		_append( s,"acodec: ",acodec,false ) ;
		_append( s,"vcodec: ",vcodec,false ) ;

		if( tbr != "0" ){

			_append( s,"tbr: ",tbr,true ) ;
		}

		if( asr != "0" ){

			_append( s,"asr: ",asr + "Hz",false ) ;
		}

		if( mt == mediaType::audioVideo ){

			_append( s,"vbr: ",vbr,true ) ;
			_append( s,"abr: ",abr,true ) ;

		}else if( mt == mediaType::audioOnly ){

			_append( s,"abr: ",abr,true ) ;

		}else if( mt == mediaType::videoOnly ){

			_append( s,"vbr: ",vbr,true ) ;
		}

		if( s.endsWith( ", " ) ){

			s.truncate( s.size() - 2 ) ;
		}

		QStringList arr{ url } ;

		if( ext == "mhtml" ){

			firstToShow.emplace_back( arr,id,ext,rsn,s ) ;

		}else if( rsn != "audio only" && !rsn.contains( "video only" ) ){

			thirdtToShow.emplace_back( arr,id,ext,rsn,s ) ;
		}else{
			secondToShow.emplace_back( arr,id,ext,rsn,s ) ;
		}
	}

	for( auto& it : secondToShow ){

		firstToShow.emplace_back( std::move( it ) ) ;
	}

	for( auto& it : thirdtToShow ){

		firstToShow.emplace_back( std::move( it ) ) ;
	}

	return firstToShow ;
}

QStringList yt_dlp::dumpJsonArguments()
{
	if( m_engine.name() == "youtube-dl" ){

		return engines::engine::functions::dumpJsonArguments() ;
	}else{
		auto a = R"R({"uploader":%(uploader)j,"id":%(id)j,"thumbnail":%(thumbnail)j,"duration":%(duration)j,"title":%(title)j,"upload_date":%(upload_date)j,"webpage_url":%(webpage_url)j,"formats":%(formats)j,"n_entries":%(n_entries)j,"playlist_id":%(playlist_id)j,"playlist_title":%(playlist_title)j,"playlist":%(playlist)j,"playlist_count":%(playlist_count)j,"playlist_uploader":%(playlist_uploader)j,"playlist_uploader_id":%(playlist_uploader_id)j})R" ;

		return { "--newline","--print",a } ;
	}
}

bool yt_dlp::breakShowListIfContains( const QStringList& e )
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

bool yt_dlp::supportsShowingComments()
{
	return m_engine.name().contains( "yt-dlp" ) ;
}

bool yt_dlp::updateVersionInfo()
{
	return m_engine.name().contains( "yt-dlp" ) ;
}

engines::engine::functions::DataFilter yt_dlp::Filter( int id,const QString& e )
{
	return { util::types::type_identity< yt_dlp::youtube_dlFilter >(),id,e,m_engine } ;
}

void yt_dlp::runCommandOnDownloadedFile( const QString& e,const QString& )
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

QString yt_dlp::updateTextOnCompleteDownlod( const QString& uiText,
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
		auto m = engines::engine::functions::processCompleteStateText( f ) ;
		return m + "\n" + bkText ;
	}
}

void yt_dlp::updateDownLoadCmdOptions( const engines::engine::functions::updateOpts& s )
{
	if( s.userOptions.contains( "--yes-playlist" ) ){

		s.ourOptions.removeAll( "--no-playlist" ) ;
	}

	if( !s.ourOptions.contains( "--newline" ) ){

		s.ourOptions.append( "--newline" ) ;
	}

	engines::engine::functions::updateDownLoadCmdOptions( s ) ;

	for( int m = 0 ; m < s.ourOptions.size() ; m++ ){

		if( s.ourOptions[ m ] == "-o" ){

			if( m + 1 < s.ourOptions.size() ){

				auto& e = s.ourOptions[ m + 1 ] ;

				e.replace( "%(autonumber)s",s.uiIndex.toString( s.ourOptions ) ) ;
				e.replace( "%(playlist_index)s",s.uiIndex.toString() ) ;
				e.replace( "%(playlist_autonumber)s",s.uiIndex.toString( s.ourOptions ) ) ;
				e.replace( "%(playlist_id)s",s.playlist_id ) ;
				e.replace( "%(playlist_title)s",s.playlist_title ) ;
				e.replace( "%(playlist)s",s.playlist ) ;
				e.replace( "%(playlist_count)s",s.playlist_count ) ;
				e.replace( "%(playlist_uploader)s",s.playlist_uploader ) ;
				e.replace( "%(playlist_uploader_id)s",s.playlist_uploader_id ) ;
				e.replace( "%(n_entries)s",s.n_entries ) ;

				m++ ;
			}
		}
	}

	if( m_engine.name().contains( "yt-dlp" ) ){

		while( s.ourOptions.contains( "--progress-template" ) ){

			utility::arguments( s.ourOptions ).removeOptionWithArgument( "--progress-template" ) ;
		}

		s.ourOptions.append( "--progress-template" ) ;
		s.ourOptions.append( "download:[download] %(progress._percent_str)s of %(progress._total_bytes_str)s at %(progress._speed_str)s ETA %(progress._eta_str)s" ) ;
	}
}

void yt_dlp::updateGetPlaylistCmdOptions( QStringList& e )
{
	if( m_version.valid() ){

		if( m_version >= m_supportsLazyPlaylist ){

			e.append( "--lazy-playlist" ) ;
		}
	}
}

yt_dlp::youtube_dlFilter::youtube_dlFilter( int processId,const QString& e,const engines::engine& engine ) :
	engines::engine::functions::filter( e,engine,processId ),
	m_likeYtdlp( engine.name().contains( "yt-dlp" ) ),
	m_processId( processId )
{
	Q_UNUSED( m_processId )
}

const QByteArray& yt_dlp::youtube_dlFilter::operator()( const Logger::Data& s )
{
	if( s.doneDownloading() ){

		if( !m_fileName.isEmpty() ){

			return m_postProcessing.text( m_fileName ) ;
		}
	}

	if( m_likeYtdlp ){

		return this->ytdlpOutput( s ) ;
	}else{
		return this->youtubedlOutput( s ) ;
	}
}

yt_dlp::youtube_dlFilter::~youtube_dlFilter()
{
}

const QByteArray& yt_dlp::youtube_dlFilter::youtubedlOutput( const Logger::Data& s )
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
		}
		if( e.contains( "] Destination: " ) ){

			m_fileName = e.mid( e.indexOf( "] Destination: " ) + 15 ) ;
		}
		if( e.contains( " Merging formats into \"" ) ){

			auto m = e.mid( e.indexOf( '"' ) + 1 ) ;
			auto s = m.lastIndexOf( '"' ) ;

			if( s != -1 ){

				m.truncate( s ) ;
			}

			m_fileName = m ;
		}
		if( e.contains( "has already been recorded in archive" ) ){

			m_tmp = engines::engine::mediaAlreadInArchiveText().toUtf8() ;

			return m_tmp ;
		}
	}

	if( s.lastLineIsProgressLine() ){

		const auto& mm = s.lastText() ;

		if( mm.startsWith( "frame=" ) || mm.startsWith( "size=" ) ){

			/*
			* ffmpeg output
			*/

			m_tmp = m_fileName + "\n" + mm ;
			return m_tmp ;
		}

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

	return m_preProcessing.text() ;
}

const QByteArray& yt_dlp::youtube_dlFilter::ytdlpOutput( const Logger::Data& s )
{
	const auto data = s.toStringList() ;

	for( const auto& e : data ){

		if( e.startsWith( "ERROR: " ) || e.startsWith( "yt-dlp: error:" ) ){

			m_tmp = e ;
			return m_tmp ;
		}
		if( e.startsWith( "[download] " ) && e.contains( " has already been downloaded" ) ){

			m_fileName = e.mid( e.indexOf( " " ) + 1 ) ;
			m_fileName.truncate( m_fileName.indexOf( " has already been downloaded" ) ) ;
		}
		if( e.contains( "] Destination: " ) ){

			m_fileName = e.mid( e.indexOf( "] Destination: " ) + 15 ) ;
		}
		if( e.contains( " Merging formats into \"" ) ){

			auto m = e.mid( e.indexOf( '"' ) + 1 ) ;
			auto s = m.lastIndexOf( '"' ) ;

			if( s != -1 ){

				m.truncate( s ) ;
			}

			m_fileName = m ;
		}
		if( e.contains( "has already been recorded" ) ){

			m_tmp = engines::engine::mediaAlreadInArchiveText().toUtf8() ;

			return m_tmp ;
		}
	}

	if( s.lastLineIsProgressLine() ){

		const auto& mm = s.lastText() ;

		if( mm.startsWith( "[DL:" ) ){

			/*
			 * aria2c when doing concurrent downloads
			 */

			m_tmp = m_fileName + "\n" + mm ;
			return m_tmp ;

		}else if( mm.startsWith( "frame=" ) || mm.startsWith( "size=" ) ){

			/*
			 * ffmpeg output
			 */

			m_tmp = m_fileName + "\n" + mm ;
			return m_tmp ;
		}else{
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
	}

	return m_preProcessing.text() ;
}

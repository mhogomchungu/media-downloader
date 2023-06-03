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

#include "aria2c.h"

const char * yt_dlp::testData()
{
	return R"R([youtube] Extracting URL: https://www.youtube.com/watch?v=tn2USd5KeVM
[youtube] Extracting URL: https://www.youtube.com/watch?v=tn2USd5KeVM
[youtube] tn2USd5KeVM: Downloading webpage
[youtube] tn2USd5KeVM: Downloading webpage
[youtube] tn2USd5KeVM: Downloading android player API JSON
[youtube] tn2USd5KeVM: Downloading android player API JSON
[info] tn2USd5KeVM: Downloading 1 format(s): 242+250
[info] tn2USd5KeVM: Downloading 1 format(s): 242+250
[dashsegments] Total fragments: 1
[download] Destination: For You, I Will-tn2USd5KeVM.f242.webm
[dashsegments] Total fragments: 1
[download] Destination: For You, I Will-tn2USd5KeVM.f242.webm
[download] {"filename":"For You, I Will-tn2USd5KeVM.f242.webm","downloaded_bytes":"1024","ETA":"801","total_bytes_estimate":"3006209.0","total_bytes":"NA","speed":"3748.8989290898444","fragment_index":"0","fragment_count":"1"}
[download] {"filename":"For You, I Will-tn2USd5KeVM.f242.webm","downloaded_bytes":"1024","ETA":"801","total_bytes_estimate":"3006209.0","total_bytes":"NA","speed":"3748.8989290898444","fragment_index":"0","fragment_count":"1"}
[download] {"filename":"For You, I Will-tn2USd5KeVM.f242.webm","downloaded_bytes":"3072","ETA":"270","total_bytes_estimate":"3006209.0","total_bytes":"NA","speed":"11100.353549598756","fragment_index":"0","fragment_count":"1"}
[download] {"filename":"For You, I Will-tn2USd5KeVM.f242.webm","downloaded_bytes":"3072","ETA":"270","total_bytes_estimate":"3006209.0","total_bytes":"NA","speed":"11100.353549598756","fragment_index":"0","fragment_count":"1"}
[download] {"filename":"For You, I Will-tn2USd5KeVM.f242.webm","downloaded_bytes":"7168","ETA":"117","total_bytes_estimate":"3006209.0","total_bytes":"NA","speed":"25553.916984198353","fragment_index":"0","fragment_count":"1"}
[download] {"filename":"For You, I Will-tn2USd5KeVM.f242.webm","downloaded_bytes":"7168","ETA":"117","total_bytes_estimate":"3006209.0","total_bytes":"NA","speed":"25553.916984198353","fragment_index":"0","fragment_count":"1"}
[download] {"filename":"For You, I Will-tn2USd5KeVM.f242.webm","downloaded_bytes":"15360","ETA":"55","total_bytes_estimate":"3006209.0","total_bytes":"NA","speed":"54191.706305212036","fragment_index":"0","fragment_count":"1"}
[download] {"filename":"For You, I Will-tn2USd5KeVM.f242.webm","downloaded_bytes":"15360","ETA":"55","total_bytes_estimate":"3006209.0","total_bytes":"NA","speed":"54191.706305212036","fragment_index":"0","fragment_count":"1"}
[download] {"filename":"For You, I Will-tn2USd5KeVM.f242.webm","downloaded_bytes":"31744","ETA":"28","total_bytes_estimate":"3006209.0","total_bytes":"NA","speed":"105082.03807274862","fragment_index":"0","fragment_count":"1"}
[download] {"filename":"For You, I Will-tn2USd5KeVM.f242.webm","downloaded_bytes":"31744","ETA":"28","total_bytes_estimate":"3006209.0","total_bytes":"NA","speed":"105082.03807274862","fragment_index":"0","fragment_count":"1"}
[download] {"filename":"For You, I Will-tn2USd5KeVM.f242.webm","downloaded_bytes":"64512","ETA":"15","total_bytes_estimate":"3006209.0","total_bytes":"NA","speed":"186530.45246344642","fragment_index":"0","fragment_count":"1"}
[download] {"filename":"For You, I Will-tn2USd5KeVM.f242.webm","downloaded_bytes":"64512","ETA":"15","total_bytes_estimate":"3006209.0","total_bytes":"NA","speed":"186530.45246344642","fragment_index":"0","fragment_count":"1"}
[download] {"filename":"For You, I Will-tn2USd5KeVM.f242.webm","downloaded_bytes":"130048","ETA":"9","total_bytes_estimate":"3006209.0","total_bytes":"NA","speed":"302596.7195118163","fragment_index":"0","fragment_count":"1"}
[download] {"filename":"For You, I Will-tn2USd5KeVM.f242.webm","downloaded_bytes":"130048","ETA":"9","total_bytes_estimate":"3006209.0","total_bytes":"NA","speed":"302596.7195118163","fragment_index":"0","fragment_count":"1"}
[download] {"filename":"For You, I Will-tn2USd5KeVM.f242.webm","downloaded_bytes":"261120","ETA":"5","total_bytes_estimate":"3006209.0","total_bytes":"NA","speed":"513690.6226527927","fragment_index":"0","fragment_count":"1"}
[download] {"filename":"For You, I Will-tn2USd5KeVM.f242.webm","downloaded_bytes":"261120","ETA":"5","total_bytes_estimate":"3006209.0","total_bytes":"NA","speed":"513690.6226527927","fragment_index":"0","fragment_count":"1"}
[download] {"filename":"For You, I Will-tn2USd5KeVM.f242.webm","downloaded_bytes":"523264","ETA":"3","total_bytes_estimate":"3006209.0","total_bytes":"NA","speed":"759468.0671639751","fragment_index":"0","fragment_count":"1"}
[download] {"filename":"For You, I Will-tn2USd5KeVM.f242.webm","downloaded_bytes":"523264","ETA":"3","total_bytes_estimate":"3006209.0","total_bytes":"NA","speed":"759468.0671639751","fragment_index":"0","fragment_count":"1"}
[download] {"filename":"For You, I Will-tn2USd5KeVM.f242.webm","downloaded_bytes":"1047552","ETA":"2","total_bytes_estimate":"3006209.0","total_bytes":"NA","speed":"944726.817633977","fragment_index":"0","fragment_count":"1"}
[download] {"filename":"For You, I Will-tn2USd5KeVM.f242.webm","downloaded_bytes":"1047552","ETA":"2","total_bytes_estimate":"3006209.0","total_bytes":"NA","speed":"944726.817633977","fragment_index":"0","fragment_count":"1"}
[download] {"filename":"For You, I Will-tn2USd5KeVM.f242.webm","downloaded_bytes":"2096128","ETA":"0","total_bytes_estimate":"3006209.0","total_bytes":"NA","speed":"1130690.3381772884","fragment_index":"0","fragment_count":"1"}
[download] {"filename":"For You, I Will-tn2USd5KeVM.f242.webm","downloaded_bytes":"2096128","ETA":"0","total_bytes_estimate":"3006209.0","total_bytes":"NA","speed":"1130690.3381772884","fragment_index":"0","fragment_count":"1"}
[download] {"filename":"For You, I Will-tn2USd5KeVM.f242.webm","downloaded_bytes":"3006209","ETA":"0","total_bytes_estimate":"3006209.0","total_bytes":"NA","speed":"1050349.8707767602","fragment_index":"0","fragment_count":"1"}
[download] {"filename":"For You, I Will-tn2USd5KeVM.f242.webm","downloaded_bytes":"3006209","ETA":"0","total_bytes_estimate":"3006209.0","total_bytes":"NA","speed":"1050349.8707767602","fragment_index":"0","fragment_count":"1"}
[download] {"filename":"For You, I Will-tn2USd5KeVM.f242.webm","downloaded_bytes":"3006209","ETA":"0","total_bytes_estimate":"3006209.0","total_bytes":"NA","speed":"1048710.8298590486","fragment_index":"1","fragment_count":"1"}
[download] {"filename":"For You, I Will-tn2USd5KeVM.f242.webm","downloaded_bytes":"3006209","ETA":"0","total_bytes_estimate":"3006209.0","total_bytes":"NA","speed":"1048710.8298590486","fragment_index":"1","fragment_count":"1"}
[download] {"filename":"For You, I Will-tn2USd5KeVM.f242.webm","downloaded_bytes":"3006209","ETA":"NA","total_bytes_estimate":"NA","total_bytes":"3006209","speed":"1045432.6172785787","fragment_index":"NA","fragment_count":"NA"}
[download] {"filename":"For You, I Will-tn2USd5KeVM.f242.webm","downloaded_bytes":"3006209","ETA":"NA","total_bytes_estimate":"NA","total_bytes":"3006209","speed":"1045432.6172785787","fragment_index":"NA","fragment_count":"NA"}
[dashsegments] Total fragments: 1
[download] Destination: For You, I Will-tn2USd5KeVM.f250.webm
[dashsegments] Total fragments: 1
[download] Destination: For You, I Will-tn2USd5KeVM.f250.webm
[download] {"filename":"For You, I Will-tn2USd5KeVM.f250.webm","downloaded_bytes":"1024","ETA":"198","total_bytes_estimate":"801810.0","total_bytes":"NA","speed":"4036.8055100230085","fragment_index":"0","fragment_count":"1"}
[download] {"filename":"For You, I Will-tn2USd5KeVM.f250.webm","downloaded_bytes":"1024","ETA":"198","total_bytes_estimate":"801810.0","total_bytes":"NA","speed":"4036.8055100230085","fragment_index":"0","fragment_count":"1"}
[download] {"filename":"For You, I Will-tn2USd5KeVM.f250.webm","downloaded_bytes":"3072","ETA":"66","total_bytes_estimate":"801810.0","total_bytes":"NA","speed":"11936.322350783255","fragment_index":"0","fragment_count":"1"}
[download] {"filename":"For You, I Will-tn2USd5KeVM.f250.webm","downloaded_bytes":"3072","ETA":"66","total_bytes_estimate":"801810.0","total_bytes":"NA","speed":"11936.322350783255","fragment_index":"0","fragment_count":"1"}
[download] {"filename":"For You, I Will-tn2USd5KeVM.f250.webm","downloaded_bytes":"7168","ETA":"28","total_bytes_estimate":"801810.0","total_bytes":"NA","speed":"27459.17039337285","fragment_index":"0","fragment_count":"1"}
[download] {"filename":"For You, I Will-tn2USd5KeVM.f250.webm","downloaded_bytes":"7168","ETA":"28","total_bytes_estimate":"801810.0","total_bytes":"NA","speed":"27459.17039337285","fragment_index":"0","fragment_count":"1"}
[download] {"filename":"For You, I Will-tn2USd5KeVM.f250.webm","downloaded_bytes":"15360","ETA":"13","total_bytes_estimate":"801810.0","total_bytes":"NA","speed":"57882.20012434548","fragment_index":"0","fragment_count":"1"}
[download] {"filename":"For You, I Will-tn2USd5KeVM.f250.webm","downloaded_bytes":"15360","ETA":"13","total_bytes_estimate":"801810.0","total_bytes":"NA","speed":"57882.20012434548","fragment_index":"0","fragment_count":"1"}
[download] {"filename":"For You, I Will-tn2USd5KeVM.f250.webm","downloaded_bytes":"31744","ETA":"6","total_bytes_estimate":"801810.0","total_bytes":"NA","speed":"112194.0950090543","fragment_index":"0","fragment_count":"1"}
[download] {"filename":"For You, I Will-tn2USd5KeVM.f250.webm","downloaded_bytes":"31744","ETA":"6","total_bytes_estimate":"801810.0","total_bytes":"NA","speed":"112194.0950090543","fragment_index":"0","fragment_count":"1"}
[download] {"filename":"For You, I Will-tn2USd5KeVM.f250.webm","downloaded_bytes":"64512","ETA":"3","total_bytes_estimate":"801810.0","total_bytes":"NA","speed":"193614.70508961147","fragment_index":"0","fragment_count":"1"}
[download] {"filename":"For You, I Will-tn2USd5KeVM.f250.webm","downloaded_bytes":"64512","ETA":"3","total_bytes_estimate":"801810.0","total_bytes":"NA","speed":"193614.70508961147","fragment_index":"0","fragment_count":"1"}
[download] {"filename":"For You, I Will-tn2USd5KeVM.f250.webm","downloaded_bytes":"130048","ETA":"2","total_bytes_estimate":"801810.0","total_bytes":"NA","speed":"324906.7034416374","fragment_index":"0","fragment_count":"1"}
[download] {"filename":"For You, I Will-tn2USd5KeVM.f250.webm","downloaded_bytes":"130048","ETA":"2","total_bytes_estimate":"801810.0","total_bytes":"NA","speed":"324906.7034416374","fragment_index":"0","fragment_count":"1"}
[download] {"filename":"For You, I Will-tn2USd5KeVM.f250.webm","downloaded_bytes":"261120","ETA":"1","total_bytes_estimate":"801810.0","total_bytes":"NA","speed":"469521.50453802437","fragment_index":"0","fragment_count":"1"}
[download] {"filename":"For You, I Will-tn2USd5KeVM.f250.webm","downloaded_bytes":"261120","ETA":"1","total_bytes_estimate":"801810.0","total_bytes":"NA","speed":"469521.50453802437","fragment_index":"0","fragment_count":"1"}
[download] {"filename":"For You, I Will-tn2USd5KeVM.f250.webm","downloaded_bytes":"523264","ETA":"0","total_bytes_estimate":"801810.0","total_bytes":"NA","speed":"679335.6383840272","fragment_index":"0","fragment_count":"1"}
[download] {"filename":"For You, I Will-tn2USd5KeVM.f250.webm","downloaded_bytes":"523264","ETA":"0","total_bytes_estimate":"801810.0","total_bytes":"NA","speed":"679335.6383840272","fragment_index":"0","fragment_count":"1"}
[download] {"filename":"For You, I Will-tn2USd5KeVM.f250.webm","downloaded_bytes":"801810","ETA":"0","total_bytes_estimate":"801810.0","total_bytes":"NA","speed":"798716.483346946","fragment_index":"0","fragment_count":"1"}
[download] {"filename":"For You, I Will-tn2USd5KeVM.f250.webm","downloaded_bytes":"801810","ETA":"0","total_bytes_estimate":"801810.0","total_bytes":"NA","speed":"798716.483346946","fragment_index":"0","fragment_count":"1"}
[download] {"filename":"For You, I Will-tn2USd5KeVM.f250.webm","downloaded_bytes":"801810","ETA":"0","total_bytes_estimate":"801810.0","total_bytes":"NA","speed":"795310.8810879494","fragment_index":"1","fragment_count":"1"}
[download] {"filename":"For You, I Will-tn2USd5KeVM.f250.webm","downloaded_bytes":"801810","ETA":"0","total_bytes_estimate":"801810.0","total_bytes":"NA","speed":"795310.8810879494","fragment_index":"1","fragment_count":"1"}
[download] {"filename":"For You, I Will-tn2USd5KeVM.f250.webm","downloaded_bytes":"801810","ETA":"NA","total_bytes_estimate":"NA","total_bytes":"801810","speed":"791098.3168199448","fragment_index":"NA","fragment_count":"NA"}
[download] {"filename":"For You, I Will-tn2USd5KeVM.f250.webm","downloaded_bytes":"801810","ETA":"NA","total_bytes_estimate":"NA","total_bytes":"801810","speed":"791098.3168199448","fragment_index":"NA","fragment_count":"NA"}
[postprocess] {"ETA":"","filename":"For You, I Will-tn2USd5KeVM.webm"}
[postprocess] {"ETA":"","filename":"For You, I Will-tn2USd5KeVM.webm"}
[Merger] Merging formats into "For You, I Will-tn2USd5KeVM.webm"
[Merger] Merging formats into "For You, I Will-tn2USd5KeVM.webm"
[postprocess] {"ETA":"","filename":"For You, I Will-tn2USd5KeVM.webm"}
[postprocess] {"ETA":"","filename":"For You, I Will-tn2USd5KeVM.webm"}
Deleting original file For You, I Will-tn2USd5KeVM.f242.webm (pass -k to keep)
Deleting original file For You, I Will-tn2USd5KeVM.f250.webm (pass -k to keep)
[postprocess] {"ETA":"","filename":"For You, I Will-tn2USd5KeVM.webm"}
[postprocess] {"ETA":"","filename":"For You, I Will-tn2USd5KeVM.webm"}
Deleting original file For You, I Will-tn2USd5KeVM.f242.webm (pass -k to keep)
Deleting original file For You, I Will-tn2USd5KeVM.f250.webm (pass -k to keep)
[postprocess] {"ETA":"","filename":"For You, I Will-tn2USd5KeVM.webm"}
[postprocess] {"ETA":"","filename":"For You, I Will-tn2USd5KeVM.webm"})R" ;
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

			mainObj.insert( "DumptJsonArguments",[](){

				QJsonArray arr ;

				arr.append( "--dump-json" ) ;

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

			mainObj.insert( "DumptJsonArguments",[](){

				QJsonArray arr ;

				auto a = R"R({"uploader":%(uploader)j,"id":%(id)j,"thumbnail":%(thumbnail)j,"duration":%(duration)j,"title":%(title)j,"upload_date":%(upload_date)j,"webpage_url":%(webpage_url)j,"formats":%(formats)j,"n_entries":%(n_entries)j,"playlist_id":%(playlist_id)j,"playlist_title":%(playlist_title)j,"playlist":%(playlist)j,"playlist_count":%(playlist_count)j,"playlist_uploader":%(playlist_uploader)j,"playlist_uploader_id":%(playlist_uploader_id)j})R" ;

				arr.append( "--newline" ) ;
				arr.append( "--print" ) ;

				arr.append( a ) ;

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

		mainObj.insert( "EncodingArgument","--encoding" ) ;

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
	m_likeYtdlp( m_engine.name() != "youtube-dl" )
{
	Q_UNUSED( m_version )

	auto name = obj.value( "Name" ).toString() ;

	if( name == "youtube-dl" || name == "yt-dlp" ){

		if( obj.value( "Cmd" ).isUndefined() ){

			auto configFileName = name + ".json" ;

			auto m = enginePath.enginePath( configFileName ) ;

			QFile::remove( m ) ;

			obj = yt_dlp::init( name,configFileName,logger,enginePath ) ;
		}
	}

	if( !obj.contains( "EncodingArgument" ) ){

		obj.insert( "EncodingArgument","--encoding" ) ;
	}

	if( name.contains( "yt-dlp" ) || name == "ytdl-patched" ){

		if( !obj.contains( "DumptJsonArguments" ) ){

			obj.insert( "DumptJsonArguments",[](){

				QJsonArray arr ;

				auto a = R"R({"uploader":%(uploader)j,"id":%(id)j,"thumbnail":%(thumbnail)j,"duration":%(duration)j,"title":%(title)j,"upload_date":%(upload_date)j,"webpage_url":%(webpage_url)j,"formats":%(formats)j,"n_entries":%(n_entries)j,"playlist_id":%(playlist_id)j,"playlist_title":%(playlist_title)j,"playlist":%(playlist)j,"playlist_count":%(playlist_count)j,"playlist_uploader":%(playlist_uploader)j,"playlist_uploader_id":%(playlist_uploader_id)j})R" ;

				arr.append( "--newline" ) ;
				arr.append( "--print" ) ;

				arr.append( a ) ;

				return arr ;
			}() ) ;
		}

		if( !obj.contains( "DumptJsonArguments" ) ){

			obj.insert( "DefaultListCmdOptions",[](){

				QJsonArray arr ;

				arr.append( "--print" ) ;
				arr.append( "%(formats)j" ) ;

				return arr ;
			}() ) ;
		}

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
		obj.insert( "CanDownloadPlaylist",false ) ;

		if( !obj.contains( "DumptJsonArguments" ) ){

			obj.insert( "DumptJsonArguments",[](){

				QJsonArray arr ;

				arr.append( "--dump-json" ) ;

				return arr ;
			}() ) ;
		}

		if( !obj.contains( "DefaultListCmdOptions" ) ){

			obj.insert( "DefaultListCmdOptions",[](){

				QJsonArray arr ;
				arr.append( "-F" ) ;

				return arr ;
			}() ) ;
		}
	}
}

#define COMPACTYEAR "2022"

void yt_dlp::appendCompatOption( QStringList& e )
{
	e.append( "--compat-options" ) ;
	e.append( COMPACTYEAR ) ;
}

const char * yt_dlp::youtube_dlFilter::compatYear()
{
	return "yt-dlp: error: wrong OPTS for --compat-options: " COMPACTYEAR ;
}

yt_dlp::~yt_dlp()
{
}

bool yt_dlp::meetCondition( const QByteArray& e )
{
	return utils::misc::startsWithAny( e,"[download]","[postprocess]" ) && e.contains( "ETA" ) ;
}

yt_dlp::result yt_dlp::formatYdDlpOutput( const Logger::locale& locale,const QByteArray& e )
{
	if( e.startsWith( "[postprocess]" ) ){

		auto obj = QJsonDocument::fromJson( e.mid( 14 ) ).object() ;

		return { obj.value( "filename" ).toString(),"" } ;
	}

	auto obj = QJsonDocument::fromJson( e.mid( 11 ) ).object() ;

	yt_dlp::result result ;

	result.filename = obj.value( "filename" ).toString() ;

	auto downloaded_str = obj.value( "downloaded_bytes" ).toString() ;
	auto totalbytesEstimate = obj.value( "total_bytes_estimate" ).toString() ;
	auto eta = obj.value( "ETA" ).toString() ;
	auto speed = obj.value( "speed" ).toString() ;
	auto totalBytes = obj.value( "total_bytes" ).toString() ;

	result.progress = e.mid( 0,11 ) ;

	if( downloaded_str == "NA" ){

		result.progress += "NA / " ;
	}else{
		result.progress += locale.formattedDataSize( qint64( downloaded_str.toDouble() ) )  + " / " ;
	}

	double percentage = 0 ;

	if( totalBytes != "NA" ){

		auto mm = totalBytes.toDouble() ;

		if( mm != 0 ){

			percentage = downloaded_str.toDouble() * 100 / mm ;
		}

		result.progress += locale.formattedDataSize( qint64( mm ) ) ;

	}else if( totalbytesEstimate != "NA" ){

		auto mm = totalbytesEstimate.toDouble() ;

		if( mm != 0 ){

			percentage = downloaded_str.toDouble() * 100 / mm ;
		}

		result.progress += "~" + locale.formattedDataSize( qint64( mm ) ) ;
	}else{
		result.progress += "NA" ;
	}

	if( percentage < 100 ){

		result.progress += " (" + QString::number( percentage,'f',2 ) + "%)" ;
	}else{
		result.progress += " (100%)" ;
	}

	if( speed != "NA" ){

		auto mm = speed.toDouble() ;

		result.progress += " at " + locale.formattedDataSize( qint64( mm ) ) + "/s" ;
	}

	if( eta == "NA" ){

		result.progress += ", ETA NA" ;
	}else{
		result.progress += ", ETA " + locale.secondsToString( eta.toInt() ) ;
	}

	return result ;
}

std::vector< engines::engine::functions::mediaInfo > yt_dlp::mediaProperties( const QByteArray& e )
{
	const auto& name = m_engine.name() ;

	if( name == "youtube-dl" ){

		return engines::engine::functions::mediaProperties( e ) ;

	}else if( m_likeYtdlp ){

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

	Logger::locale s ;

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
	return this->likeYtdlp() ;
}

bool yt_dlp::updateVersionInfo()
{
	return false ;
}

bool yt_dlp::likeYtdlp()
{
	return m_likeYtdlp ;
}

void yt_dlp::updateLocalOptions( QStringList& opts )
{
	if( this->likeYtdlp() ){

		opts.prepend( "--break-on-reject" ) ;
		opts.prepend( "!playlist" ) ;
		opts.prepend( "--match-filter" ) ;
	}
}

void yt_dlp::setTextEncondig( const QString& args,QStringList& opts )
{
	const auto& e = engines::engine::functions::Settings().textEncoding() ;

	if( !e.isEmpty() && !args.isEmpty() ){

		opts.append( args ) ;
		opts.append( e ) ;
	}
}

engines::engine::functions::DataFilter yt_dlp::Filter( int id,const QString& e )
{
	return { util::types::type_identity< yt_dlp::youtube_dlFilter >(),id,e,m_engine,m_likeYtdlp } ;
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
	using functions = engines::engine::functions ;

	if( f.cancelled() ){

		return functions::updateTextOnCompleteDownlod( bkText,dopts,f ) ;

	}else if( f.success() ){

		auto e = engines::engine::mediaAlreadInArchiveText() ;

		if( uiText.contains( e ) ){

			auto m = engines::engine::functions::updateTextOnCompleteDownlod( bkText,dopts,f ) ;

			return m + "\n" + e  ;
		}

		QStringList a ;

		for( const auto& it : util::split( uiText,'\n',true ) ){

			auto x = engines::engine::functions::postProcessing::processingText() ;
			auto y = engines::engine::functions::preProcessing::processingText() ;

			if( !it.contains( x ) && !it.contains( y ) ){

				a.append( it ) ;
			}
		}

		return engines::engine::functions::updateTextOnCompleteDownlod( a.join( "\n" ),dopts,f ) ;

	}else if( uiText == "EngineNeedUpdating" ){

		const auto& name = this->engine().name() ;
		auto version = "2023.01.06" ;

		return QObject::tr( "Please Update \"%1\" To Atleast Version \"%2\"" ).arg( name,version ) ;

	}else if( uiText.contains( "Requested format is not available" ) ){

		return functions::errorString( f,functions::errors::unknownFormat,bkText ) ;

	}else if( uiText.contains( "Temporary failure in name resolution" ) ){

		return functions::errorString( f,functions::errors::noNetwork,bkText ) ;

	}else if( uiText.contains( " is not a valid URL" ) ){

		return functions::errorString( f,functions::errors::unknownUrl,bkText ) ;

	}else if( uiText.contains( "ERROR: Unsupported URL:" ) ){

		return functions::errorString( f,functions::errors::notSupportedUrl,bkText ) ;
	}else{
		auto m = engines::engine::functions::updateTextOnCompleteDownlod( uiText,dopts,f ) ;
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

	auto _replace = [ this ]( QStringList& s,QString& txt,const QString& original,const QString& New ){

		if( m_likeYtdlp ){

			if( txt.contains( original ) ){

				s.append( "--parse-metadata" ) ;
				s.append( New + ":" + original ) ;
			}
		}else{
			txt.replace( original,New ) ;
		}
	} ;

	for( int m = 0 ; m < s.ourOptions.size() ; m++ ){

		if( s.ourOptions[ m ] == "-o" ){

			if( m + 1 < s.ourOptions.size() ){

				auto& e = s.ourOptions[ m + 1 ] ;

				auto w = s.uiIndex.toString( true,s.ourOptions ) ;
				auto ww = s.uiIndex.toString( false,s.ourOptions ) ;

				_replace( s.ourOptions,e,"%(autonumber)s",ww ) ;
				_replace( s.ourOptions,e,"%(playlist_index)s",w ) ;
				_replace( s.ourOptions,e,"%(playlist_autonumber)s",w ) ;
				_replace( s.ourOptions,e,"%(playlist_id)s",s.playlist_id ) ;
				_replace( s.ourOptions,e,"%(playlist_title)s",s.playlist_title ) ;
				_replace( s.ourOptions,e,"%(playlist)s",s.playlist ) ;
				_replace( s.ourOptions,e,"%(playlist_count)s",s.playlist_count ) ;
				_replace( s.ourOptions,e,"%(playlist_uploader)s",s.playlist_uploader ) ;
				_replace( s.ourOptions,e,"%(playlist_uploader_id)s",s.playlist_uploader_id ) ;
				_replace( s.ourOptions,e,"%(n_entries)s",s.n_entries ) ;

				m++ ;
			}
		}
	}

	if( m_likeYtdlp ){

		this->appendCompatOption( s.ourOptions ) ;

		if( m_engine.name() == "yt-dlp" || m_engine.name() == "ytdl-patched" ){

			while( s.ourOptions.contains( "--progress-template" ) ){

				utility::arguments( s.ourOptions ).removeOptionWithArgument( "--progress-template" ) ;
			}

			s.ourOptions.append( "--progress-template" ) ;
			s.ourOptions.append( R"R(download:[download] {"filename":"%(progress.filename)s","downloaded_bytes":"%(progress.downloaded_bytes)s","ETA":"%(progress.eta)s","total_bytes_estimate":"%(progress.total_bytes_estimate)s","total_bytes":"%(progress.total_bytes)s","speed":"%(progress.speed)s","fragment_index":"%(progress.fragment_index)s","fragment_count":"%(progress.fragment_count)s"})R" ) ;

			//s.ourOptions.append( "--progress-template" ) ;
			//s.ourOptions.append( R"R(postprocess:[postprocess] {"ETA":"","filename":"%(info.filepath)s"})R" ) ;
		}
	}
}

void yt_dlp::updateGetPlaylistCmdOptions( QStringList& e )
{
	if( this->likeYtdlp() ){

		e.append( "--lazy-playlist" ) ;

		this->appendCompatOption( e ) ;
	}
}

void yt_dlp::updateCmdOptions( QStringList& e )
{
	if( this->likeYtdlp() ){

		this->appendCompatOption( e ) ;
	}
}

yt_dlp::youtube_dlFilter::youtube_dlFilter( int processId,
					    const QString& e,
					    const engines::engine& engine,
					    bool likeYtdlp ) :
	engines::engine::functions::filter( e,engine,processId )
{
	Q_UNUSED( likeYtdlp )
}

const QByteArray& yt_dlp::youtube_dlFilter::operator()( const Logger::Data& s )
{
	if( s.lastLineIsProgressLine() ){

		auto obj = QJsonDocument::fromJson( s.lastText() ).object() ;

		auto fileName = obj.value( "filename" ).toString().toUtf8() ;
		auto progress = obj.value( "progress" ).toString().toUtf8() ;

		if( !fileName.isEmpty() ){

			this->setFileName( fileName ) ;
		}

		m_tmp = m_fileName + "\n" + progress ;

		return m_tmp ;
	}

	const auto m = s.toStringList() ;

	if( s.doneDownloading() ){

		if( utility::stringConstants::downloadFailed( s.lastText() ) ){

			for( auto it = m.rbegin() ; it != m.rend() ; it++ ){

				const QByteArray& e = *it ;

				if( e.startsWith( "ERROR: " ) ){

					m_tmp = e ;

					return m_tmp ;
				}
			}
		}

		if( m_fileName.isEmpty() ){

			/*
			 * Failed to find file name, try again.
			 *
			 */

			this->parseOutput( m ) ;

			if( m_fileName.isEmpty() ){

				//????
				return m_tmp ;
			}else{
				return m_fileName ;
			}
		}else{
			return m_fileName ;
		}
	}

	return this->parseOutput( m ) ;
}

yt_dlp::youtube_dlFilter::~youtube_dlFilter()
{
}

const QByteArray& yt_dlp::youtube_dlFilter::parseOutput( const Logger::Data::QByteArrayList& data )
{
	for( const auto& m : data ){

		const QByteArray& e = m ;

		if( e.contains( this->compatYear() ) ){

			m_tmp = "EngineNeedUpdating" ;

			return m_tmp ;
		}
		if( e.startsWith( "ERROR: " ) || e.startsWith( "yt-dlp: error:" ) ){

			m_tmp = e ;

			return m_tmp ;
		}
		if( e.startsWith( "[download] " ) && e.contains( " has already been downloaded" ) ){

			auto m = e.mid( e.indexOf( " " ) + 1 ) ;
			m.truncate( m.indexOf( " has already been downloaded" ) ) ;

			this->setFileName( m ) ;
		}
		if( e.contains( "] Destination: " ) ){

			this->setFileName( e.mid( e.indexOf( "] Destination: " ) + 15 ) ) ;
		}
		if( e.contains( " Merging formats into \"" ) ){

			auto m = e.mid( e.indexOf( '"' ) + 1 ) ;
			auto s = m.lastIndexOf( '"' ) ;

			if( s != -1 ){

				m.truncate( s ) ;
			}

			this->setFileName( m ) ;
		}
		if( e.contains( "has already been recorded" ) ){

			m_tmp = engines::engine::mediaAlreadInArchiveText().toUtf8() ;

			return m_tmp ;
		}
	}

	return m_preProcessing.text() ;
}

void yt_dlp::youtube_dlFilter::setFileName( const QByteArray& fileName )
{
	if( m_fileName != fileName ){

		m_fileName = fileName ;
	}
}

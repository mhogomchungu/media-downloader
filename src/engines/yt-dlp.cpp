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
	return R"R([youtube] Extracting URL: https://www.youtube.com/watch?v=DqM8lN5oYIQ
[youtube] DqM8lN5oYIQ: Downloading webpage
[youtube] DqM8lN5oYIQ: Downloading android player API JSON
[info] DqM8lN5oYIQ: Downloading 1 format(s): 242+250
[dashsegments] Total fragments: 2
[download] Destination: [FULL] Ja Morant speaks with Jalen Rose about suspension and time away from Grizzlies ｜ NBA on ESPN-DqM8lN5oYIQ.f242.webm
[download] {"downloaded_bytes":"1024","ETA":"9085","total_bytes_estimate":"20971520.0","total_bytes":"NA","speed":"2308.166670786825"}
[download] {"downloaded_bytes":"3072","ETA":"3041","total_bytes_estimate":"20971520.0","total_bytes":"NA","speed":"6894.097028533271"}
[download] {"downloaded_bytes":"7168","ETA":"1309","total_bytes_estimate":"20971520.0","total_bytes":"NA","speed":"16007.600578439704"}
[download] {"downloaded_bytes":"15360","ETA":"618","total_bytes_estimate":"20971520.0","total_bytes":"NA","speed":"33894.52471806959"}
[download] {"downloaded_bytes":"31744","ETA":"338","total_bytes_estimate":"20971520.0","total_bytes":"NA","speed":"61904.458799013206"}
[download] {"downloaded_bytes":"64512","ETA":"188","total_bytes_estimate":"20971520.0","total_bytes":"NA","speed":"110804.18758456211"}
[download] {"downloaded_bytes":"130048","ETA":"104","total_bytes_estimate":"20971520.0","total_bytes":"NA","speed":"199614.373190396"}
[download] {"downloaded_bytes":"261120","ETA":"66","total_bytes_estimate":"20971520.0","total_bytes":"NA","speed":"311350.1081777341"}
[download] {"downloaded_bytes":"523264","ETA":"46","total_bytes_estimate":"20971520.0","total_bytes":"NA","speed":"438358.8704671926"}
[download] {"downloaded_bytes":"1047552","ETA":"38","total_bytes_estimate":"20971520.0","total_bytes":"NA","speed":"520347.03257018956"}
[download] {"downloaded_bytes":"1687150","ETA":"36","total_bytes_estimate":"20971520.0","total_bytes":"NA","speed":"533387.5427934055"}
[download] {"downloaded_bytes":"2243237","ETA":"33","total_bytes_estimate":"20971520.0","total_bytes":"NA","speed":"561741.2993071564"}
[download] {"downloaded_bytes":"2912850","ETA":"33","total_bytes_estimate":"20971520.0","total_bytes":"NA","speed":"545522.0357999972"}
[download] {"downloaded_bytes":"3410516","ETA":"33","total_bytes_estimate":"20971520.0","total_bytes":"NA","speed":"529182.3904762757"}
[download] Got error: The read operation timed out. Retrying (1/10)...
[download] {"downloaded_bytes":"3411540","ETA":"184","total_bytes_estimate":"20971520.0","total_bytes":"NA","speed":"94931.37059723618"}
[download] {"downloaded_bytes":"3413588","ETA":"184","total_bytes_estimate":"20971520.0","total_bytes":"NA","speed":"94982.60609781978"}
[download] {"downloaded_bytes":"3417684","ETA":"184","total_bytes_estimate":"20971520.0","total_bytes":"NA","speed":"95091.72055404988"}
[download] {"downloaded_bytes":"3425876","ETA":"184","total_bytes_estimate":"20971520.0","total_bytes":"NA","speed":"95313.96093333402"}
[download] {"downloaded_bytes":"3442260","ETA":"183","total_bytes_estimate":"20971520.0","total_bytes":"NA","speed":"95550.50791965693"}
[download] {"downloaded_bytes":"3475028","ETA":"181","total_bytes_estimate":"20971520.0","total_bytes":"NA","speed":"96186.96763395301"}
[download] {"downloaded_bytes":"3540564","ETA":"178","total_bytes_estimate":"20971520.0","total_bytes":"NA","speed":"97453.39141565803"}
[download] {"downloaded_bytes":"3671636","ETA":"172","total_bytes_estimate":"20971520.0","total_bytes":"NA","speed":"100158.71076850696"}
[download] {"downloaded_bytes":"3933780","ETA":"161","total_bytes_estimate":"20971520.0","total_bytes":"NA","speed":"105351.72898265478"}
[download] {"downloaded_bytes":"4318974","ETA":"147","total_bytes_estimate":"20971520.0","total_bytes":"NA","speed":"112818.31764849504"}
[download] {"downloaded_bytes":"4727343","ETA":"134","total_bytes_estimate":"20971520.0","total_bytes":"NA","speed":"120423.48982678542"}
[download] {"downloaded_bytes":"5146840","ETA":"125","total_bytes_estimate":"20971520.0","total_bytes":"NA","speed":"125909.89467468052"}
[download] {"downloaded_bytes":"5405616","ETA":"119","total_bytes_estimate":"20971520.0","total_bytes":"NA","speed":"130124.82017291748"}
[download] {"downloaded_bytes":"5794890","ETA":"112","total_bytes_estimate":"20971520.0","total_bytes":"NA","speed":"134943.9961686846"}
[download] {"downloaded_bytes":"6072675","ETA":"106","total_bytes_estimate":"20971520.0","total_bytes":"NA","speed":"139242.74476722535"}
[download] {"downloaded_bytes":"6487383","ETA":"99","total_bytes_estimate":"20971520.0","total_bytes":"NA","speed":"144886.2947201563"}
[download] {"downloaded_bytes":"6843957","ETA":"93","total_bytes_estimate":"20971520.0","total_bytes":"NA","speed":"150461.2678086423"}
[download] {"downloaded_bytes":"7345365","ETA":"86","total_bytes_estimate":"20971520.0","total_bytes":"NA","speed":"156771.06208633882"}
[download] {"downloaded_bytes":"7712113","ETA":"82","total_bytes_estimate":"20971520.0","total_bytes":"NA","speed":"160949.32426648482"}
[download] {"downloaded_bytes":"8057594","ETA":"78","total_bytes_estimate":"20971520.0","total_bytes":"NA","speed":"164634.86796998224"}
[download] {"downloaded_bytes":"8394255","ETA":"76","total_bytes_estimate":"20971520.0","total_bytes":"NA","speed":"165329.03273587"}
[download] {"downloaded_bytes":"8578144","ETA":"74","total_bytes_estimate":"20971520.0","total_bytes":"NA","speed":"166086.57107819003"}
[download] {"downloaded_bytes":"8788056","ETA":"73","total_bytes_estimate":"20971520.0","total_bytes":"NA","speed":"166826.85308883293"}
[download] {"downloaded_bytes":"8991928","ETA":"71","total_bytes_estimate":"20971520.0","total_bytes":"NA","speed":"167932.39852304332"}
[download] {"downloaded_bytes":"9227104","ETA":"70","total_bytes_estimate":"20971520.0","total_bytes":"NA","speed":"167768.37084486702"}
[download] {"downloaded_bytes":"9388828","ETA":"69","total_bytes_estimate":"20971520.0","total_bytes":"NA","speed":"167245.30252918118"}
[download] {"downloaded_bytes":"9530921","ETA":"67","total_bytes_estimate":"20971520.0","total_bytes":"NA","speed":"168560.3697487629"}
[download] {"downloaded_bytes":"9815107","ETA":"65","total_bytes_estimate":"20971520.0","total_bytes":"NA","speed":"169731.25337726605"}
[download] {"downloaded_bytes":"10036241","ETA":"63","total_bytes_estimate":"20971520.0","total_bytes":"NA","speed":"171032.2210184886"}
[download] {"downloaded_bytes":"10295512","ETA":"61","total_bytes_estimate":"20971520.0","total_bytes":"NA","speed":"172446.69390799373"}
[download] {"downloaded_bytes":"10485760","ETA":"60","total_bytes_estimate":"20971520.0","total_bytes":"NA","speed":"173029.9962740324"}
[download] {"downloaded_bytes":"10485760","ETA":"60","total_bytes_estimate":"20971520.0","total_bytes":"NA","speed":"173022.33349307108"}
[download] {"downloaded_bytes":"10486784","ETA":"7","total_bytes_estimate":"11796647.0","total_bytes":"NA","speed":"1750.3708801274133"}
[download] {"downloaded_bytes":"10488832","ETA":"7","total_bytes_estimate":"11796647.0","total_bytes":"NA","speed":"5233.999068962576"}
[download] {"downloaded_bytes":"10492928","ETA":"7","total_bytes_estimate":"11796647.0","total_bytes":"NA","speed":"12167.922688077086"}
[download] {"downloaded_bytes":"10501120","ETA":"7","total_bytes_estimate":"11796647.0","total_bytes":"NA","speed":"25889.86023588585"}
[download] {"downloaded_bytes":"10517504","ETA":"7","total_bytes_estimate":"11796647.0","total_bytes":"NA","speed":"48335.52949889239"}
[download] {"downloaded_bytes":"10550272","ETA":"7","total_bytes_estimate":"11796647.0","total_bytes":"NA","speed":"74798.11794521647"}
[download] {"downloaded_bytes":"10615808","ETA":"6","total_bytes_estimate":"11796647.0","total_bytes":"NA","speed":"113537.05415908484"}
[download] {"downloaded_bytes":"10746880","ETA":"6","total_bytes_estimate":"11796647.0","total_bytes":"NA","speed":"158368.77158775582"}
[download] {"downloaded_bytes":"11007211","ETA":"4","total_bytes_estimate":"11796647.0","total_bytes":"NA","speed":"165801.77290220343"}
[download] {"downloaded_bytes":"11181183","ETA":"3","total_bytes_estimate":"11796647.0","total_bytes":"NA","speed":"182626.39573505012"}
[download] {"downloaded_bytes":"11443612","ETA":"2","total_bytes_estimate":"11796647.0","total_bytes":"NA","speed":"199632.3245340071"}
[download] {"downloaded_bytes":"11708788","ETA":"0","total_bytes_estimate":"11796647.0","total_bytes":"NA","speed":"209707.98127237352"}
[download] {"downloaded_bytes":"11796647","ETA":"0","total_bytes_estimate":"11796647.0","total_bytes":"NA","speed":"56075.20221969847"}
[download] {"downloaded_bytes":"11796647","ETA":"0","total_bytes_estimate":"11796647.0","total_bytes":"NA","speed":"56068.910933410865"}
[download] {"downloaded_bytes":"11796647","ETA":"NA","total_bytes_estimate":"NA","total_bytes":"11796647","speed":"140451.7907306989"}
[dashsegments] Total fragments: 1
[download] Destination: [FULL] Ja Morant speaks with Jalen Rose about suspension and time away from Grizzlies ｜ NBA on ESPN-DqM8lN5oYIQ.f250.webm
[download] {"downloaded_bytes":"1024","ETA":"4166","total_bytes_estimate":"7821248.0","total_bytes":"NA","speed":"1877.0881600069229"}
[download] {"downloaded_bytes":"3072","ETA":"1393","total_bytes_estimate":"7821248.0","total_bytes":"NA","speed":"5609.286754494647"}
[download] {"downloaded_bytes":"7168","ETA":"599","total_bytes_estimate":"7821248.0","total_bytes":"NA","speed":"13044.76737572774"}
[download] {"downloaded_bytes":"15360","ETA":"281","total_bytes_estimate":"7821248.0","total_bytes":"NA","speed":"27738.50536001991"}
[download] {"downloaded_bytes":"31744","ETA":"147","total_bytes_estimate":"7821248.0","total_bytes":"NA","speed":"52633.986992512706"}
[download] {"downloaded_bytes":"64512","ETA":"82","total_bytes_estimate":"7821248.0","total_bytes":"NA","speed":"94500.08352154797"}
[download] {"downloaded_bytes":"130048","ETA":"47","total_bytes_estimate":"7821248.0","total_bytes":"NA","speed":"162792.25518585017"}
[download] {"downloaded_bytes":"261120","ETA":"25","total_bytes_estimate":"7821248.0","total_bytes":"NA","speed":"292855.12300958077"}
[download] {"downloaded_bytes":"523264","ETA":"17","total_bytes_estimate":"7821248.0","total_bytes":"NA","speed":"419365.08258043684"}
[download] {"downloaded_bytes":"1047552","ETA":"13","total_bytes_estimate":"7821248.0","total_bytes":"NA","speed":"513673.9872437038"}
[download] {"downloaded_bytes":"1709763","ETA":"10","total_bytes_estimate":"7821248.0","total_bytes":"NA","speed":"580649.2188449841"}
[download] {"downloaded_bytes":"2441153","ETA":"8","total_bytes_estimate":"7821248.0","total_bytes":"NA","speed":"637930.5499757356"}
[download] {"downloaded_bytes":"3270233","ETA":"7","total_bytes_estimate":"7821248.0","total_bytes":"NA","speed":"648863.1869848456"}
[download] {"downloaded_bytes":"3953688","ETA":"6","total_bytes_estimate":"7821248.0","total_bytes":"NA","speed":"631228.8194858104"}
[download] {"downloaded_bytes":"4512248","ETA":"5","total_bytes_estimate":"7821248.0","total_bytes":"NA","speed":"605954.9042261117"}
[download] {"downloaded_bytes":"4984605","ETA":"4","total_bytes_estimate":"7821248.0","total_bytes":"NA","speed":"580024.1245883891"}
[download] {"downloaded_bytes":"5396310","ETA":"4","total_bytes_estimate":"7821248.0","total_bytes":"NA","speed":"564560.0096001641"}
[download] {"downloaded_bytes":"5822757","ETA":"3","total_bytes_estimate":"7821248.0","total_bytes":"NA","speed":"539568.81081942"}
[download] {"downloaded_bytes":"6168686","ETA":"3","total_bytes_estimate":"7821248.0","total_bytes":"NA","speed":"534567.255447496"}
[download] {"downloaded_bytes":"6631381","ETA":"2","total_bytes_estimate":"7821248.0","total_bytes":"NA","speed":"516307.89647017943"}
[download] {"downloaded_bytes":"6986221","ETA":"1","total_bytes_estimate":"7821248.0","total_bytes":"NA","speed":"514246.3167705335"}
[download] {"downloaded_bytes":"7464042","ETA":"0","total_bytes_estimate":"7821248.0","total_bytes":"NA","speed":"515202.6942332197"}
[download] {"downloaded_bytes":"7821248","ETA":"0","total_bytes_estimate":"7821248.0","total_bytes":"NA","speed":"517071.85018308816"}
[download] {"downloaded_bytes":"7821248","ETA":"0","total_bytes_estimate":"7821248.0","total_bytes":"NA","speed":"516993.44943826686"}
[download] {"downloaded_bytes":"7821248","ETA":"NA","total_bytes_estimate":"NA","total_bytes":"7821248","speed":"516245.4840510907"}
[Merger] Merging formats into "[FULL] Ja Morant speaks with Jalen Rose about suspension and time away from Grizzlies ｜ NBA on ESPN-DqM8lN5oYIQ.webm"
Deleting original file [FULL] Ja Morant speaks with Jalen Rose about suspension and time away from Grizzlies ｜ NBA on ESPN-DqM8lN5oYIQ.f242.webm (pass -k to keep)
Deleting original file [FULL] Ja Morant speaks with Jalen Rose about suspension and time away from Grizzlies ｜ NBA on ESPN-DqM8lN5oYIQ.f250.webm (pass -k to keep)
)R" ;
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

QByteArray yt_dlp::formatYdDlpOutput( const QByteArray& e )
{
	static utility::locale locale ;

	auto obj = QJsonDocument::fromJson( e.mid( 11 ) ).object() ;

	auto downloaded_str = obj.value( "downloaded_bytes" ).toString() ;
	auto totalbytesEstimate = obj.value( "total_bytes_estimate" ).toString() ;
	auto eta = obj.value( "ETA" ).toString() ;
	auto speed = obj.value( "speed" ).toString() ;
	auto totalBytes = obj.value( "total_bytes" ).toString() ;

	QString ss = e.mid( 0,11 ) ;

	if( downloaded_str == "NA" ){

		ss += "NA" ;
	}else{
		ss += locale.formattedDataSize( qint64( downloaded_str.toDouble() ) )  + " / " ;
	}

	double percentage = 0 ;

	if( totalBytes != "NA" ){

		auto mm = totalBytes.toDouble() ;

		if( mm != 0 ){

			percentage = downloaded_str.toDouble() / mm ;
		}

		ss += locale.formattedDataSize( qint64( mm ) ) ;

	}else if( totalbytesEstimate != "NA" ){

		auto mm = totalbytesEstimate.toDouble() ;

		if( mm != 0 ){

			percentage = downloaded_str.toDouble() / mm ;
		}

		ss += locale.formattedDataSize( qint64( mm ) ) ;
	}else{
		ss += "NA" ;
	}

	percentage *= 100 ;

	if( percentage < 100 ){

		ss += " (" + QString::number( percentage,'f',2 ) + "%) at " ;
	}else{
		ss += " (100%) at " ;
	}

	if( speed == "NA" ){

		ss += "NA ETA " ;
	}else{
		auto mm = speed.toDouble() ;

		ss += locale.formattedDataSize( qint64( mm ) ) + "/s eta " ;
	}

	if( eta == "NA" ){

		ss += "NA" ;
	}else{
		auto mm = eta.toInt() ;

		if( mm < 3600 ){

			ss += QTime( 0,0,0,0 ).addSecs( mm ).toString( "mm:ss" ) ;
		}else{
			ss += QTime( 0,0,0,0 ).addSecs( mm ).toString( "hh:mm:ss" ) ;
		}
	}

	return ss.toUtf8() ;
}

std::vector< engines::engine::functions::mediaInfo > yt_dlp::mediaProperties( const QByteArray& e )
{
	const auto& name = m_engine.name() ;

	if( name == "youtube-dl" ){

		return engines::engine::functions::mediaProperties( e ) ;

	}else if( name.contains( "yt-dlp" ) || name == "ytdl-patched" ){

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
	//return this->likeYtdlp() ;
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
	}else {
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

		while( s.ourOptions.contains( "--progress-template" ) ){

			utility::arguments( s.ourOptions ).removeOptionWithArgument( "--progress-template" ) ;
		}

		s.ourOptions.append( "--progress-template" ) ;
		s.ourOptions.append( R"R(download:[download] {"downloaded_bytes":"%(progress.downloaded_bytes)s","ETA":"%(progress.eta)s","total_bytes_estimate":"%(progress.total_bytes_estimate)s","total_bytes":"%(progress.total_bytes)s","speed":"%(progress.speed)s"})R" ) ;
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
					    bool likeYtDlp ) :
	engines::engine::functions::filter( e,engine,processId ),
	m_likeYtdlp( likeYtDlp )
{
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

		if( e.contains( this->compatYear() ) ){

			m_tmp = "EngineNeedUpdating" ;

			return m_tmp ;
		}
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
		}else{ //
			m_tmp = m_fileName + "\n" + mm.mid( 11 ) ;

			const auto& engine = engines::engine::functions::filter::engine() ;

			if( engine.name() == "yt-dlp-aria2c" ){

				aria2c::trimProgressLine( m_tmp ) ;
			}

			return m_tmp ;
		}
	}

	return m_preProcessing.text() ;
}

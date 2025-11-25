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

#include "engines.h"

#include "engines/yt-dlp.h"
#include "engines/generic.h"
#include "engines/safaribooks.h"
#include "engines/gallery-dl.h"
#include "engines/aria2c.h"
#include "engines/lux.h"
#include "engines/wget.h"
#include "engines/svtplay-dl.h"
#include "engines/you-get.h"
#include "engines/deno.h"
#include "engines/quickjs.h"
#include "engines/getsauce.h"

#include "reportFinished.h"
#include "utility.h"
#include "version.h"
#include "tableWidget.h"
#include "context.hpp"
#include "networkAccess.h"

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QDesktopServices>
#include <QNetworkProxyFactory>
#include <QDir>

QStringList engines::dirEntries( const QString& e ) const
{
	auto filters = QDir::Filter::Dirs | QDir::Filter::NoDotAndDotDot ;

	return QDir( e ).entryList( filters ) ;
}

QProcessEnvironment engines::getEnvPaths() const
{
	auto env = QProcessEnvironment::systemEnvironment() ;

	const auto& basePath = m_enginePaths.binPath() ;

	auto separator = [ & ](){

		if( utility::platformIsLikeWindows() ){

			return ";" ;
		}else{
			return ":" ;
		}
	}() ;

	QString s ;

	const auto l = this->dirEntries( basePath ) ;

	if( utility::platformIsWindows() ){

		const auto& mm = m_settings.windowsOnly3rdPartyBinPath() ;

		s = mm ;

		auto m = this->dirEntries( mm ) ;

		m.removeOne( "aria2-1.36.0-win-32bit-build1" ) ;
		m.removeOne( "aria2-1.37.0-win-32bit-build1" ) ;
		m.removeOne( "ffmpeg-n5.0-latest-win32-gpl-shared-5.0" ) ;
		m.removeOne( "ffmpeg-n6.0-latest-win32-gpl-shared-6.0" ) ;
		m.removeOne( "python-3.8.10-embed-win32" ) ;
		m.removeOne( "wget-1.21.3-win32" ) ;

		for( const auto& it : util::asConst( m ) ){

			s += separator + mm + "/" + it ;
			s += separator + mm + "/" + it + "/bin" ;
		}

	}else if( utility::platformIsOSX() ){

		s = utility::OSXApplicationDirPath() ;

		s += separator + utility::OSX3rdPartyDirPath() ;
	}

	auto it  = l.begin() ;
	auto end = l.end() ;

	if( it != end ){

		const auto& m = *it ;

		if( s.isEmpty() ){

			s = basePath + "/" + m ;
		}else{
			s += separator + basePath + "/" + m ;
		}

		s += separator + basePath + "/" + m + "/bin" ;

		it++ ;
	}

	for( ; it != end ; it++ ){

		const auto& m = *it ;

		s += separator + basePath + "/" + m ;
		s += separator + basePath + "/" + m + "/bin" ;
	}

	if( s.isEmpty() ){

		s = basePath ;
	}else{
		s += separator + basePath ;
	}

	env.insert( "PATH",s + separator + env.value( "PATH" ) ) ;

	env.insert( "LANG","C" ) ;

	return env ;
}

engines::engines( Logger& l,const engines::enginePaths& paths,settings& s,int id ) :
	m_logger( l ),
	m_settings( s ),
	m_enginePaths( paths ),
	m_processEnvironment( this->getEnvPaths() ),
	m_defaultEngine( l,m_enginePaths )
{
	this->updateEngines( true,id ) ;

	this->showBanner() ;
}

void engines::showBanner()
{
	m_bannerId = utility::sequentialID() ;

	const auto& id = m_bannerId ;

	auto aa = m_settings.showLocalAndLatestVersionInformation() ;
	auto bb = m_settings.showVersionInfoAndAutoDownloadUpdates() ;
	auto cc = m_settings.showLocalVersionInformationOnly() ;

	if( aa || bb || cc ){

		m_logger.add( utility::barLine(),id ) ;

		m_logger.add( QObject::tr( "To Disable These Checks, Do The Following:-" ),id ) ;
		m_logger.add( QObject::tr( "1. Go To \"Configure\" Tab." ),id ) ;
		m_logger.add( QObject::tr( "2. Go To \"General Options\" Sub Tab." ),id ) ;
		m_logger.add( QObject::tr( "3. Click \"Actions At StartUp\" Menu." ),id ) ;
		m_logger.add( QObject::tr( "4. Select \"Do Nothing\"." ),id ) ;

		m_logger.add( utility::barLine(),id ) ;
	}

	if( utility::platformIsWindows() ){

		if( m_settings.portableVersion() ){

			m_logger.add( QObject::tr( "Running In Portable Mode" ),id ) ;
		}else{
			m_logger.add( QObject::tr( "Running In Installation Mode" ),id ) ;
		}
	}

	m_logger.add( QObject::tr( "Download Path: %1" ).arg( m_settings.downloadFolder( m_logger ) ),id ) ;
	m_logger.add( QObject::tr( "App Data Path: %1" ).arg( m_enginePaths.basePath() ),id ) ;

	if( m_settings.printMediaPlayers() ){

		m_logger.add( utility::barLine(),id ) ;

		for( const auto& it : utility::getMediaPlayers() ){

			m_logger.add( it.name + ": " + it.exePath,id ) ;
		}
	}
}

void engines::setNetworkProxy( engines::proxySettings e,bool firstTime,networkAccess& n )
{
	if( e.isSet() ){

		if( m_networkProxy != e ){

			const auto& s = e.networkProxyString() ;

			if( !firstTime ){

				m_logger.add( utility::barLine(),m_bannerId ) ;
			}

			auto mm = QObject::tr( "Setting Proxy Server Address Of %1" ).arg( s ) ;

			m_logger.add( mm,m_bannerId ) ;

			m_logger.add( utility::barLine(),m_bannerId ) ;
		}
	}else{
		if( firstTime ){

			m_logger.add( utility::barLine(),m_bannerId ) ;

		}else if( m_networkProxy != e ){

			m_logger.add( utility::barLine(),m_bannerId ) ;

			m_logger.add( QObject::tr( "Unsetting Proxy Server Address" ),m_bannerId ) ;

			m_logger.add( utility::barLine(),m_bannerId ) ;
		}
	}

	m_networkProxy = e.move() ;

	n.setProxySettings( m_networkProxy.networkProxy() ) ;
}

void engines::openUrls( tableWidget& table,int row,const engines::engine& engine ) const
{
	if( reportFinished::finishedStatus::finishedWithSuccess( table,row ) ){

		const auto& ee = table.uiText( row ) ;
		const auto& ss = table.entryAt( row ).fileNames ;

		engine.openLocalFile( { ee,m_settings.downloadFolder(),ss } ) ;
	}
}

const QString& engines::defaultEngineName() const
{
	return m_defaultEngine.name() ;
}

util::result< engines::engine > engines::getEngineByPath( const QString& e ) const
{
	auto path = m_enginePaths.enginePath( e ) ;

	util::Json json( engines::file( path,m_logger ).readAll() ) ;

	if( json ){

		auto object = json.doc().object() ;

		auto minVersion = object.value( "RequiredMinimumVersionOfMediaDownloader" ).toString() ;

		if( !minVersion.isEmpty() ){

			util::version min = minVersion ;
			util::version cur = utility::compileTimeVersion() ;

			if( min > cur ){

				auto name = object.value( "Name" ).toString() ;

				auto m = QObject::tr( "Engine \"%1\" requires atleast version \"%2\" of Media Downloader" ) ;

				m_logger.add( m.arg( name,minVersion ),utility::sequentialID() ) ;

				return {} ;
			}
		}

		return { m_logger,m_enginePaths,object,*this,utility::sequentialID() } ;
	}else{
		return {} ;
	}
}

QStringList engines::engine::dumpJsonArguments( engines::engine::tab tab ) const
{
	if( this->name() == "gallery-dl" ){

		if( tab == engine::tab::playlist ){

			auto m = m_dumpJsonArguments ;

			m.append( "-o" ) ;
			m.append( "metadata.mode=jsonl" ) ;

			return m ;
		}else{
			auto m = m_dumpJsonArguments ;

			m.append( "--range" ) ;
			m.append( "1" ) ;

			return m ;
		}

	}else if( tab == engine::tab::playlist ){

		if( this->name() == "yt-dlp-test" ){

			return { "--media-downloader-test-engine-yt-dlp-playlist" } ;
		}else{
			return yt_dlp::jsonNoFormatsArgumentList() ;
		}
	}else{
		if( this->name() == "yt-dlp-test" ){

			return { "--media-downloader-test-engine-yt-dlp-metadata" } ;
		}else{
			return m_dumpJsonArguments ;
		}
	}
}

void engines::setDefaultEngine( const QString& name )
{
	m_settings.setDefaultEngine( name,settings::tabName::basic ) ;
	m_settings.setDefaultEngine( name,settings::tabName::batch ) ;

	const auto& e = this->getEngineByName( name ) ;

	if( e.has_value() && e.value().canDownloadPlaylist() ){

		m_settings.setDefaultEngine( name,settings::tabName::playlist ) ;
	}
}

void engines::engineAdd( const QString& jsonFile,util::result< engines::engine > m,int id )
{
	if( m ){

		if( m->exePath().isEmpty() ){

			auto s = QObject::tr( "Error, executable to backend \"%1\" could not be found" ) ;

			m_logger.add( s.arg( m->name() ),id ) ;
		}else{
			m_backends.emplace_back( std::move( m.value() ) ) ;
		}
	}else{
		m_logger.add( QObject::tr( "Error, failed to parse config file \"%1\"" ).arg( jsonFile ),id ) ;
	}
}

void engines::updateEngines( bool addAll,int id )
{
	m_backends.clear() ;

	this->engineAdd( "",this->getEngineByPath( m_defaultEngine.configFileName() ),id ) ;

	const auto mm = this->enginesList() ;

	for( const auto& it : mm ){

		this->engineAdd( it,this->getEngineByPath( it ),id ) ;
	}

	if( addAll ){

		if( utility::platformIsWindows() ){

			this->engineAdd( "",{ *this,m_logger,"bsdtar","--version",0,1,id },id ) ;
		}else{
			auto m = utility::platformIsLinux() ? 3 : 1 ;

			this->engineAdd( "",{ *this,m_logger,"tar","--version",0,m,id },id ) ;
		}

		this->engineAdd( "",{ *this,m_logger,"ffmpeg","-version",0,2,id },id ) ;

		for( const auto& it : this->getEngines() ){

			const auto& e = it.exePath().exe() ;

			if( e.size() > 0 && e.at( 0 ).contains( "python" ) ){

				if( utility::platformIsWindows() ){

					this->engineAdd( it.name(),{ *this,m_logger,"python","--version",0,1,id },id ) ;
				}else{
					this->engineAdd( it.name(),{ *this,m_logger,"python3","--version",0,1,id },id ) ;
				}
				break ;
			}
		}
	}

	const auto& engines = *this ;

	for( auto& it : m_backends ){

		const auto& name = it.name() ;

		if( it.likeYtDlp() ){

			it.setBackend< yt_dlp >( engines ) ;

		}else if( name.contains( "safaribooks" ) ){

			it.setBackend< safaribooks >( engines ) ;

		}else if( name.contains( "gallery-dl" ) ){

			it.setBackend< gallery_dl >( engines ) ;

		}else if( name == "aria2c" ){

			it.setBackend< aria2c >( engines ) ;

		}else if( name.contains( "lux" ) ){

			it.setBackend< lux >( engines ) ;

		}else if( name.contains( "you-get" ) ){

			it.setBackend< you_get >( engines ) ;

		}else if( name.contains( "svtplay-dl" ) ){

			it.setBackend< svtplay_dl >( engines ) ;

		}else if( name.contains( "wget" ) ){

			it.setBackend< wget >( engines ) ;

		}else if( name.contains( "deno" ) ){

			it.setBackend< deno >( engines ) ;

		}else if( name.contains( "quickjs" ) ){

			it.setBackend< quickjs >( engines ) ;

		}else if( name.contains( "getsauce" ) ){

			it.setBackend< getsauce >( engines ) ;
		}else{
			it.setBackend< generic >( engines ) ;
		}
	}
}

const std::vector< engines::engine >& engines::getEngines() const
{
	return m_backends ;
}

engines::Iterator engines::getEnginesIterator() const
{
	return { m_backends,utility::sequentialID() } ;
}

const engines::engine& engines::defaultEngine( const QString& name,int id ) const
{
	auto m = this->getEngineByName( name ) ;

	if( m ){

		return m.value() ;
	}else{
		m_logger.add( "Error: engines::defaultEngine: Unknown Engine: " + name,id ) ;

		if( m_backends.size() > 0 ){

			return m_backends[ 0 ] ;
		}else{
			static engines::engine engine ;

			return engine ;
		}
	}
}

util::result_ref< const engines::engine& > engines::getEngineByName( const QString& name ) const
{
	for( const auto& it : m_backends ){

		if( it.name() == name ){

			return it ;
		}
	}

	return {} ;
}

util::result_ref< const engines::engine& > engines::getCompleteEngineByPath( const QString& e ) const
{
	auto m = this->getEngineByPath( e ) ;

	if( m && m->valid() ){

		return this->getEngineByName( m->name() ) ;
	}else{
		return {} ;
	}
}

const engines::enginePaths& engines::engineDirPaths() const
{
	return m_enginePaths ;
}

settings& engines::Settings() const
{
	return m_settings ;
}

bool engines::filePathIsValid( const QFileInfo& info )
{
	return info.exists() && info.isFile() ;
}

QString engines::findExecutable( const QString& exeName,const QStringList& paths,QFileInfo& info ) const
{
	for( const auto& it : paths ){

		auto m = it + "/" + exeName ;

		info.setFile( m ) ;

		if( engines::filePathIsValid( info ) ){

			return m ;
		}
	}

	return {} ;
}

QString engines::findExecutable( const QString& exeName ) const
{
	if( utility::platformIsWindows() && exeName == "media-downloader.exe" ){

		return utility::windowsApplicationDirPath() + "/media-downloader.exe" ;
	}

	QFileInfo info( exeName ) ;

	if( info.isAbsolute() ){

		return exeName ;
	}

	if( utility::platformIsLikeWindows() ){

		auto paths = this->processEnvironment().value( "PATH" ).split( ';' ) ;

		auto m = this->findExecutable( exeName,paths,info ) ;

		if( m.isEmpty() && !exeName.endsWith( ".exe" ) ){

			m = this->findExecutable( exeName + ".exe",paths,info ) ;
		}

		return m ;
	}else{
		auto paths = this->processEnvironment().value( "PATH" ).split( ':' ) ;

		return this->findExecutable( exeName,paths,info ) ;
	}
}

const QProcessEnvironment& engines::processEnvironment() const
{
	return m_processEnvironment ;
}

QString engines::addEngine( const QByteArray& data,const QString& path,int id )
{
	util::Json json( data ) ;

	if( json ){

		auto object = json.doc().object() ;

		auto name = object.value( "Name" ).toString() ;

		if( !name.isEmpty() ){

			auto e = m_enginePaths.enginePath( path ) ;

			QFile f( e ) ;

			if( f.open( QIODevice::WriteOnly | QIODevice::Truncate ) ){

				f.write( data ) ;

				f.flush() ;

				f.close() ;

				for( int i = 0 ; i < 5 ; i++ ){

					if( QFile::exists( e ) ){

						break ;
					}else{
						utility::waitForOneSecond() ;
					}
				}

				this->updateEngines( false,id ) ;

				return name ;
			}
		}
	}

	m_logger.add( QObject::tr( "Failed To Load A Plugin" ) + ": " + json.errorString(),id ) ;

	return {} ;
}

void engines::removeEngine( const QString& e,int id )
{
	const auto& engine = this->getCompleteEngineByPath( e ) ;

	if( engine ){

		utility::removeFile( m_enginePaths.enginePath( e ) ) ;

		if( engine->archiveContainsFolder() ){

			QFileInfo m( m_enginePaths.binPath( engine->name() ) ) ;

			if( m.exists() && m.isDir() ){

				utility::removeFolder( m.filePath() ) ;
			}
		}else{
			auto exe = QDir::fromNativeSeparators( engine->exePath().realExe() ) ;
			auto binPath = QDir::fromNativeSeparators( m_enginePaths.binPath() ) ;

			if( exe.startsWith( binPath ) && QFile::exists( exe ) ){

				engine->removeFiles( { exe },binPath ) ;
			}
		}

		if( m_backends.size() > 0 ){

			const auto& name = engine->name() ;

			auto _reset_default = [ & ]( const QString& name,settings::tabName n ){

				if( name == m_settings.defaultEngine( n,this->defaultEngineName() ) ){

					m_settings.setDefaultEngine( m_backends[ 0 ].name(),n ) ;
				}
			} ;

			_reset_default( name,settings::tabName::basic ) ;
			_reset_default( name,settings::tabName::batch ) ;
			_reset_default( name,settings::tabName::playlist ) ;
		}

		this->updateEngines( false,id ) ;
	}
}

QStringList engines::enginesList() const
{
	auto m = QDir( m_enginePaths.enginePath() ).entryList( QDir::Filter::Files ) ;

	m.removeAll( m_defaultEngine.configFileName() ) ;

	m.removeOne( "youtube-dl.json" ) ;

	return m ;
}

engines::engine::engine( const engines& engines,
			 Logger& logger,
			 const QString& name,
			 const QString& versionArgument,
			 int line,
			 int position,
			 int id ) :
	m_line( line ),
	m_position( position ),
	m_valid( true ),
	m_likeYtDlp( false ),
	m_supportingEngine( true ),
	m_versionArgument( versionArgument ),
	m_name( name ),
	m_commandName( utility::platformIsLikeWindows() ? name + ".exe" : name )
{
	auto m = engines.findExecutable( m_commandName ) ;

	if( m.isEmpty() ){

		m_valid = false ;
		logger.add( QObject::tr( "Failed to find executable \"%1\"" ).arg( m_commandName ),id ) ;
	}else{
		m_exePath = m ;
	}
}

QStringList engines::engine::toStringList( const QJsonValue& value,bool protectSpace ) const
{
	QStringList m ;

	const auto array = value.toArray() ;

	for( const auto& it : array ){

		auto s = it.toString() ;

		if( s.contains( ' ' ) && protectSpace ){

			m.append( "\"" + s + "\"" ) ;
		}else{
			m.append( s ) ;
		}
	}

	return m ;
}

void engines::engine::updateOptions()
{
	if( utility::platformIsWindows7() ){

		m_extraArguments = this->toStringList( m_jsonObject.value( "ExtraArgumentsWin7" ) ) ;

	}else if( utility::platformisFlatPak() ){

		m_extraArguments = this->toStringList( m_jsonObject.value( "ExtraArgumentsFlatpak" ) ) ;

		if( m_extraArguments.isEmpty() && this->likeYtDlp() ){

			m_extraArguments.append( "--no-js-runtimes" ) ;
			m_extraArguments.append( "--js-runtimes" ) ;
			m_extraArguments.append( "quickjs" ) ;
		}
	}else{
		m_extraArguments = this->toStringList( m_jsonObject.value( "ExtraArguments" ) ) ;
	}

	m_controlStructure                = m_jsonObject.value( "ControlJsonStructure" ).toObject() ;
	m_canDownloadPlaylist             = m_jsonObject.value( "CanDownloadPlaylist" ).toBool() ;
	m_replaceOutputWithProgressReport = m_jsonObject.value( "ReplaceOutputWithProgressReport" ).toBool( false ) ;
	m_userName                        = m_jsonObject.value( "UserName" ).toString() ;
	m_password                        = m_jsonObject.value( "Password" ).toString() ;
	m_optionsArgument                 = m_jsonObject.value( "OptionsArgument" ).toString() ;
	m_playlistItemsArgument           = m_jsonObject.value( "PlaylistItemsArgument" ).toString() ;
	m_batchFileArgument               = m_jsonObject.value( "BatchFileArgument" ).toString() ;
	m_cookieArgument                  = m_jsonObject.value( "CookieArgument" ).toString() ;
	m_cookieTextFileArgument          = m_jsonObject.value( "CookieArgumentTextFile" ).toString() ;
	m_encodingArgument                = m_jsonObject.value( "EncodingArgument" ).toString() ;
	m_dumpJsonArguments               = this->toStringList( m_jsonObject.value( "DumptJsonArguments" ) ) ;
	m_splitLinesBy                    = this->toStringList( m_jsonObject.value( "SplitLinesBy" ) ) ;
	m_removeText                      = this->toStringList( m_jsonObject.value( "RemoveText" ) ) ;
	m_skiptLineWithText               = this->toStringList( m_jsonObject.value( "SkipLineWithText" ) ) ;
	m_defaultDownLoadCmdOptions       = this->toStringList( m_jsonObject.value( "DefaultDownLoadCmdOptions" ),true ) ;
	m_defaultListCmdOptions           = this->toStringList( m_jsonObject.value( "DefaultListCmdOptions" ) ) ;
	m_defaultCommentsCmdOptions       = this->toStringList( m_jsonObject.value( "DefaultCommentsCmdOptions" ) ) ;
	m_defaultSubstitlesCmdOptions     = this->toStringList( m_jsonObject.value( "DefaultSubstitlesCmdOptions" ) ) ;
	m_defaultSubtitleDownloadOptions  = this->toStringList( m_jsonObject.value( "DefaultSubtitleDownloadOptions" ) ) ;
}

QJsonObject engines::engine::getCmd( const QJsonObject& cmd )
{
	if( utility::platformIsWindows() ){

		return cmd.value( "Windows" ).toObject() ;
	}else{
		if( utility::platformIsOSX() ){

			auto m = cmd.value( "MacOS" ).toObject() ;

			if( !m.isEmpty() ){

				return m ;
			}
		}

		return cmd.value( "Generic" ).toObject() ;
	}
}

engines::engine::cmd engines::engine::getCommands( const QJsonObject& cmd )
{
	auto obj = [ & ](){

		utility::CPU cpu ;

		if( cpu.x86_32() ){

			return this->getCmd( cmd ).value( "x86" ).toObject() ;

		}else if( cpu.x86_64() ){

			return this->getCmd( cmd ).value( "amd64" ).toObject() ;

		}else if( cpu.aarch64() ){

			auto m = this->getCmd( cmd ).value( "aarch64" ).toObject() ;

			if( !m.isEmpty() ){

				return m ;
			}
		}

		return this->getCmd( cmd ).value( "amd64" ).toObject() ;
	}() ;

	auto m = obj.value( "Name" ).toString() ;

	auto s = this->toStringList( obj.value( "Args" ).toArray() ) ;

	return { m,s,s.size() == 1 } ;
}

engines::engine::engine( Logger& logger,
			 const enginePaths& ePaths,
			 const util::Json& json,
			 const engines& engines,
			 int id ) :
	m_jsonObject( json.doc().object() ),
	m_line( m_jsonObject.value( "VersionStringLine" ).toInt() ),
	m_position( m_jsonObject.value( "VersionStringPosition" ).toInt() ),
	m_valid( true ),
	m_likeYtDlp( m_jsonObject.value( "LikeYoutubeDl" ).toBool() ),
	m_autoUpdate( m_jsonObject.value( "AutoUpdate" ).toBool( true ) ),
	m_archiveContainsFolder( m_jsonObject.value( "ArchiveContainsFolder" ).toBool() ),
	m_versionArgument( m_jsonObject.value( "VersionArgument" ).toString() ),
	m_name( m_jsonObject.value( "Name" ).toString() ),
	m_configVersion( m_jsonObject.value( "Version" ).toString() ),
	m_exeFolderPath( m_jsonObject.value( "BackendPath" ).toString() ),
	m_downloadUrl( m_jsonObject.value( "DownloadUrl" ).toString() )
{
	if( utility::platformIsOSX() ){

		auto m = m_jsonObject.value( "DownloadUrlMAC" ).toString() ;

		if( !m.isEmpty() ){

			m_downloadUrl = m ;
		}
	}

	if( m_name == "deno" || m_name == "quickjs" ){

		m_supportingEngine = true ;
	}else{
		m_supportingEngine = false ;
	}

	if( m_name == "svtplay-dl" ){

		m_archiveContainsFolder = utility::platformIsWindows() ;

		m_downloadUrl = svtplay_dl::downloadUrl() ;
	}

	auto defaultPath = utility::stringConstants::defaultPath() ;
	auto backendPath = utility::stringConstants::backendPath() ;

	if( m_exeFolderPath == defaultPath || m_exeFolderPath == backendPath ){

		m_exeFolderPath = ePaths.binPath() ;
	}

	auto cmd = m_jsonObject.value( "Cmd" ) ;

	auto m = this->getCommands( cmd.toObject() ) ;

	if( utility::platformIsWindows7() && this->likeYtDlp() ){

		if( cmd.isUndefined() ){

			yt_dlp::setNicolaasjanYtdlpOptions( m_commandName,m_downloadUrl ) ;
		}else{
			QJsonValue value ;

			if( utility::CPU().x86_32() ){

				value = cmd.toObject().value( "Windows" ).toObject().value( "win7x86" ) ;
			}else{
				value = cmd.toObject().value( "Windows" ).toObject().value( "win7amd64" ) ;
			}

			if( value.isUndefined() ){

				yt_dlp::setNicolaasjanYtdlpOptions( m_commandName,m_downloadUrl ) ;
			}else{
				auto obj = value.toObject() ;

				m_commandName = obj.value( "Name" ).toString() ;

				m_downloadUrl = m_jsonObject.value( "DownloadUrlWin7" ).toString() ;
			}
		}
	}else{
		m_commandName = m.name ;
	}

	if( m.noCheckArgs ){

		this->parseMultipleCmdArgs( logger,engines,ePaths,id ) ;
	}else{
		this->parseMultipleCmdArgs( m.args,backendPath,logger,ePaths,engines,id ) ;
	}
}

QString engines::engine::updateCmdPath( Logger& logger,const QString& e ) const
{
	auto exe = m_engine->updateCmdPath( e ) ;

	if( exe.isEmpty() ){

		auto m = "Trouble Ahead, Engine's Exe Not Found: " + m_engine->engine().name() ;

		logger.add( m,utility::sequentialID() ) ;
	}

	m_exePath.updateRealExe( exe ) ;

	return exe ;
}

void engines::engine::parseMultipleCmdArgs( Logger& logger,
					    const engines& engines,
					    const engines::enginePaths&,
					    int id )
{
	auto m = engines.findExecutable( m_commandName ) ;

	if( m.isEmpty() ){

		if( this->validDownloadUrl() && !m_exeFolderPath.isEmpty() ){

			m_exePath = m_exeFolderPath + "/" + m_commandName ;
		}else{
			m_valid = false ;
			logger.add( utility::failedToFindExecutableString( m_commandName ),id ) ;
		}
	}else{
		auto a = this->validDownloadUrl() ;
		auto b = !m_commandName.startsWith( "media-downloader" ) ;
		auto c = !m_exeFolderPath.isEmpty() ;

		if( a && b && c ){

			/*
			 * backends that are internally managed
			 */

			if( m.startsWith( m_exeFolderPath ) ){
				/*
				 * backend found in internal bin folder
				 */
				m_exePath = m ;
			}else{
				/*
				 * backend not found in internal bin folder, lets assume its path
				 * and hope for the best
				 */
				m_exePath = m_exeFolderPath + "/" + m_commandName ;
			}
		}else{
			/*
			 * backends that are managed outside like wget or aria2c
			 */
			m_exePath = m ;
		}
	}
}

void engines::engine::parseMultipleCmdArgs( QStringList& cmdNames,
					    const QString& backendPath,
					    Logger& logger,
					    const enginePaths& ePaths,
					    const engines& engines,
					    int id )
{
	if( cmdNames.isEmpty() ){

		m_valid = false ;
		return ;
	}

	this->parseMultipleCmdArgs( logger,engines,ePaths,id ) ;

	auto cmd = cmdNames.takeAt( 0 ) ;

	for( auto& it : cmdNames ){

		it.replace( backendPath,ePaths.binPath() ) ;
		it.replace( utility::stringConstants::commandName(),m_commandName ) ;
	}

	QString subCmd ;

	for( auto& it : cmdNames ){

		if( it.endsWith( m_commandName ) ){

			if( it == m_commandName ){

				auto m = engines.findExecutable( m_commandName ) ;

				if( m.isEmpty() ){

					if( m_archiveContainsFolder ){

						subCmd = m_exeFolderPath + "/" + m_name + "/" + it ;
					}else{
						subCmd = m_exeFolderPath + "/" + it ;
					}

					it = subCmd ;
				}else{
					it = m ;
					subCmd = m ;
				}
			}else{
				subCmd = it ;
			}
		}
	}

	if( cmd == "python3" ){

		auto m = engines.findExecutable( "python3" ) ;

		if( m.isEmpty() ){

			m_valid = false ;
			logger.add( QObject::tr( "Failed to find python3 executable for backend \"%1\"" ).arg( m_name ),id ) ;
		}else{
			if( utility::platformIsWindows() ){

				m_exePath = { m,subCmd,cmdNames } ;
			}else{
				m_exePath = { m,subCmd,cmdNames } ;
			}
		}
	}else{
		auto m = engines.findExecutable( cmd ) ;

		if( m.isEmpty() ){

			m_valid = false ;
			logger.add( QObject::tr( "Failed to find executable \"%1\"" ).arg( cmd ),id ) ;
		}else{
			m_exePath = { m,subCmd,cmdNames } ;
		}
	}
}

const QString& engines::engine::commandName() const
{
	return m_commandName ;
}

bool engines::engine::breakShowListIfContains( const QStringList& e ) const
{
	return m_engine->breakShowListIfContains( e ) ;
}

QString engines::engine::setVersionString( const QString& data ) const
{
	auto m = this->versionString( data ) ;

	if( !m.isEmpty() ){

		m_version = m ;
	}

	return m ;
}

QString engines::engine::versionString( const QString& data ) const
{
	auto a = util::split( data,'\n',true ) ;

	if( m_line < a.size() ){

		auto b = a[ m_line ] ;
		auto c = util::split( b,' ',true ) ;

		if( m_position < c.size() ){

			auto m = c[ m_position ] ;

			m.replace( ",","" ).replace( "v","" ) ;

			if( m == "N-121066-g189d0b83b2-20250915" ){

				m = "n8.1-dev-121066-g189d0b83b2-20250915" ;
			}

			return m ;
		}
	}

	return {} ;
}

bool engines::engine::validDownloadUrl() const
{
	auto a = "https://api.github.com" ;
	auto b = "https://bellard.org/quickjs/binary_releases" ;

	return m_downloadUrl.startsWith( a ) || m_downloadUrl.startsWith( b ) ;
}

void engines::engine::setPermissions( const QString& e ) const
{
	utility::setPermissions( e ) ;
}

engines::enginePaths::enginePaths( settings& s )
{
	m_basePath = s.configPaths() ;

	while( m_basePath.endsWith( '/' ) ){

		m_basePath.truncate( m_basePath.size() - 1 ) ;
	}

	m_binPath       = m_basePath + "/bin" ;
	m_enginePath    = m_basePath + "/engines.v1" ;
	m_dataPath      = m_basePath + "/data" ;
	m_updatePath    = m_basePath + "/update" ;
	m_updateNewPath = m_basePath + "/update_new" ;
	m_tmp           = m_basePath + "/tmp" ;

	QDir dir ;

	dir.mkpath( m_basePath ) ;
	dir.mkpath( m_binPath ) ;
	dir.mkpath( m_enginePath ) ;
	dir.mkpath( m_dataPath ) ;
	dir.mkpath( m_tmp ) ;

	yt_dlp::checkIfBinaryExist( m_binPath,s.windowsOnly3rdPartyBinPath() ) ;
}

QString engines::enginePaths::socketPath()
{
	if( utility::platformIsWindows() ){

		return "\\\\.\\pipe\\MediaDownloaderIPC" ;
	}else{
		auto m = m_basePath + "/tmp" ;
		QDir().mkpath( m ) ;
		return m  + "/ipc" ;
	}
}

void engines::enginePaths::confirmPaths( Logger& logger ) const
{
	QFileInfo fileInfo ;

	std::vector< QString > warning ;

	auto _check_exists = [ & ]( const QString& m,bool checkIfExecutable ){

		fileInfo.setFile( m ) ;

		if( fileInfo.exists() ){

			if( !fileInfo.isWritable() ){

				warning.emplace_back( "Trouble Ahead, Folder Not Writable: " + m ) ;
			}
			if( !fileInfo.isReadable() ){

				warning.emplace_back( "Trouble Ahead, Folder Not Readable: " + m ) ;
			}
			if( checkIfExecutable && !fileInfo.isExecutable() ){

				warning.emplace_back( "Trouble Ahead, Folder Not Executable: " + m ) ;
			}
		}else{
			warning.emplace_back( "Trouble Ahead, Folder Does Not Exist: " + m ) ;
		}
	} ;

	utility::checkPermissions perms ;

	perms.enable() ;

	_check_exists( m_basePath,false ) ;
	_check_exists( m_binPath,true ) ;
	_check_exists( m_enginePath,false ) ;
	_check_exists( m_dataPath,false ) ;
	_check_exists( m_tmp,false ) ;

	perms.disable() ;

	if( !warning.empty() ){

		auto id = utility::sequentialID() ;

		const auto& m = utility::barLine() ;

		logger.add( m,id ) ;

		for( const auto& it : warning ){

			logger.add( it,id ) ;
		}

		logger.add( m,id ) ;
	}
}

QString engines::engine::baseEngine::errorString( const engine::engine::baseEngine::finishedState& f,
						 engines::engine::baseEngine::errors err,
						 const QString& bkText )
{
	if( err == engines::engine::baseEngine::errors::unknownFormat ){

		auto m = engines::engine::baseEngine::processCompleteStateText( f ) ;
		return m + "\n" + QObject::tr( "Requested Format Is Not Available" ) + "\n" + bkText ;

	}else if( err == engines::engine::baseEngine::errors::noNetwork ){

		auto m = engines::engine::baseEngine::processCompleteStateText( f ) ;
		return m + "\n" + QObject::tr( "Network Error Occured" ) + "\n" + bkText ;

	}else if( err == engines::engine::baseEngine::errors::unknownUrl ){

		auto m = engines::engine::baseEngine::processCompleteStateText( f ) ;
		return m + "\n" + QObject::tr( "Invalid Url Entered" ) + "\n" + bkText ;

	}else if( err == engines::engine::baseEngine::errors::notSupportedUrl ){

		auto m = engines::engine::baseEngine::processCompleteStateText( f ) ;
		return m + "\n" + QObject::tr( "Url Is Not Supported" ) + "\n" + bkText ;

	}else if( err == engines::engine::baseEngine::errors::logInRequired ){

		auto m = engines::engine::baseEngine::processCompleteStateText( f ) ;
		return m + "\n" + QObject::tr( "Sign In To Confirm You Are Not A Bot" ) + "\n" + bkText ;

	}else if( err == engines::engine::baseEngine::errors::runtimeError ){

		return QObject::tr( "Download Failed, Engine crashed" ) + "\n" + bkText ;
	}else{
		return {} ;
	}
}

QString engines::engine::baseEngine::processCompleteStateText( const engine::engine::baseEngine::finishedState& f )
{
	if( f.cancelled() ){

		return QObject::tr( "Download cancelled" ) ;

	}else if( f.success() ){

		return QObject::tr( "Download completed" ) ;
	}else{
		using m = engines::ProcessExitState::ExitStatus ;

		auto s = f.exitStatus() ;

		if( s == m::NormalExit ){

			auto m = QString::number( f.errorCode() ) ;

			auto a = QObject::tr( "Download Failed" ) ;
			auto b = "(" + QObject::tr( "ErrorCode" ) + "=" + m + ")" ;

			return a + b ;

		}else if( s == m::FailedToStart ){

			return QObject::tr( "Download Failed, Engine failed to start" ) ;
		}else{
			return QObject::tr( "Download Failed, Engine crashed" ) ;
		}
	}
}

bool engines::engine::baseEngine::meetExtraCondition( const QByteArray& l,const QJsonObject& obj )
{
	const QString line = l ;

	if( obj.contains( "startsWith" ) ){

		return line.startsWith( obj.value( "startsWith" ).toString() ) ;
	}

	if( obj.contains( "endsWith" ) ){

		return line.endsWith( obj.value( "endsWith" ).toString() ) ;
	}

	if( obj.contains( "contains" ) ){

		return line.contains( obj.value( "contains" ).toString() ) ;
	}

	if( obj.contains( "containsAny" ) ){

		const auto arr = obj.value( "containsAny" ).toArray() ;

		for( const auto& it : arr ){

			if( line.contains( it.toString() ) ) {

				return true ;
			}
		}

		return false ;
	}

	if( obj.contains( "containsAll" ) ){

		const auto arr = obj.value( "containsAll" ).toArray() ;

		for( const auto& it : arr ){

			if( !line.contains( it.toString() ) ) {

				return false ;
			}
		}

		return true ;
	}

	return false ;
}

bool engines::engine::baseEngine::meetCondition( const engines::engine& engine,const QByteArray& line )
{
	const auto& obj = engine.controlStructure() ;

	auto connector = obj.value( "Connector" ).toString() ;

	if( connector.isEmpty() ){

		auto m = obj.value( "lhs" ) ;

		if( m.isObject() ){

			return engines::engine::baseEngine::meetExtraCondition( line,m.toObject() ) ;
		}else{
			return false ;
		}
	}else{
		auto obj1 = obj.value( "lhs" ) ;
		auto obj2 = obj.value( "rhs" ) ;

		if( obj1.isObject() && obj2.isObject() ){

			auto a = engines::engine::baseEngine::meetExtraCondition( line,obj1.toObject() ) ;
			auto b = engines::engine::baseEngine::meetExtraCondition( line,obj2.toObject() ) ;

			if( connector == "&&" ){

				return a && b ;

			}else if( connector == "||" ){

				return a || b ;
			}else{
				return false ;
			}
		}else{
			return false ;
		}
	}
}

class defaultFilter : public engines::engine::baseEngine::filterOutPut
{
public:
	defaultFilter( const engines::engine& engine ) : m_engine( engine )
	{
	}
	engines::engine::baseEngine::filterOutPut::result
	formatOutput( const filterOutPut::args& args ) const override
	{
		return { args.outPut,m_engine,engines::engine::baseEngine::meetCondition } ;
	}
	bool meetCondition( const filterOutPut::args& args ) const override
	{
		return engines::engine::baseEngine::meetCondition( m_engine,args.outPut ) ;
	}
	const engines::engine& engine() const override
	{
		return m_engine ;
	}
private:
	const engines::engine& m_engine ;
} ;

engines::engine::baseEngine::FilterOutPut engines::engine::baseEngine::filterOutput()
{
	return { util::types::type_identity< defaultFilter >(),m_engine } ;
}

engines::engine::baseEngine::~baseEngine()
{
}

const QProcessEnvironment& engines::engine::baseEngine::processEnvironment() const
{
	return m_processEnvironment ;
}

engines::metadata engines::engine::baseEngine::parseJsonDataFromGitHub( const QJsonDocument& doc )
{
	class meaw
	{
	public:
		meaw( engines::engine::baseEngine& parent ) : m_parent( parent )
		{
		}
		bool operator()( const QJsonObject& obj )
		{
			return m_parent.foundNetworkUrl( obj.value( "name" ).toString() ) ;
		}
	private:
		engines::engine::baseEngine& m_parent ;
	} ;

	return utility::parseJsonDataFromGitHub( doc,meaw( *this ) ) ;
}

std::vector< engines::engine::baseEngine::mediaInfo > engines::engine::baseEngine::mediaProperties( Logger&,const QByteArray& e )
{
	auto args = util::split( e,'\n' ) ;

	QStringList m ;

	utility::reverse( args ).forEach( [ & ]( const QByteArray& s ){

		auto a = util::split( s,' ',true ) ;

		if( a.size() > 1 ){

			if( m_engine.breakShowListIfContains( a ) ){

				return true ;
			}else{
				m.insert( 0,s ) ;
			}
		}

		return false ;
	} ) ;

	std::vector< engines::engine::baseEngine::mediaInfo > s ;

	for( const auto& it : m ){

		auto a = util::split( it,' ',true ) ;

		if( a.size() > 3 ){

			auto format     = a.takeAt( 0 ) ;
			auto extension  = a.takeAt( 0 ) ;
			auto resolution = a.takeAt( 0 ) ;
			auto notes      = a.join( " " ) ;

			s.emplace_back( format,extension,resolution,"NA","0",notes,"","" ) ;
		}
	}

	return s ;
}

std::vector< engines::engine::baseEngine::mediaInfo > engines::engine::baseEngine::mediaProperties( Logger&,const QJsonArray& )
{
	return {} ;
}

void engines::engine::baseEngine::updateOutPutChannel( QProcess::ProcessChannel& ) const
{
}

bool engines::engine::baseEngine::breakShowListIfContains( const QStringList& )
{
	return false ;
}

bool engines::engine::baseEngine::supportsShowingComments()
{
	return false ;
}

bool engines::engine::baseEngine::updateVersionInfo()
{
	return false ;
}

QByteArray engines::engine::baseEngine::parseError( const QByteArray& )
{
	return {} ;
}

void engines::engine::baseEngine::setTextEncondig( const QString&,QStringList& )
{
}

QJsonObject engines::engine::baseEngine::parseJson( const QString&,const QByteArray& )
{
	return {} ;
}

std::vector< QByteArray > engines::engine::baseEngine::parseJsonData( QByteArray& )
{
	return {} ;
}

QString engines::engine::baseEngine::updateCmdPath( const QString& e )
{
	return e ;
}

engines::engine::baseEngine::DataFilter engines::engine::baseEngine::Filter( int id )
{
	return { util::types::type_identity< engines::engine::baseEngine::filter >(),m_engine,id } ;
}

QString engines::engine::baseEngine::deleteEngineBinFolder( const QString& e )
{
	auto m = e + "/" + m_engine.name() ;

	QDir dir( m ) ;

	if( dir.exists() && !dir.removeRecursively() ){

		return m ;
	}else{
		return {} ;
	}
}

void engines::engine::baseEngine::runCommandOnDownloadedFile( const std::vector< QByteArray >& fileNames )
{
	auto df = m_settings.downloadFolder() + "/" ;

	m_settings.runCommandOnSuccessfulDownload( this->engine().name(),df,fileNames ) ;
}

QString engines::engine::baseEngine::commandString( const engines::engine::exeArgs::cmd& cmd )
{
	auto m = "\"" + cmd.exe() + "\"" ;

	for( const auto& it : cmd.args() ){

		m += " \"" + it + "\"" ;
	}

	return m ;
}

QStringList engines::engine::baseEngine::horizontalHeaderLabels() const
{
	QStringList s ;

	s.append( QObject::tr( "Format Code" ) ) ;
	s.append( QObject::tr( "Extension" ) ) ;
	s.append( QObject::tr( "Resolution" ) ) ;
	s.append( QObject::tr( "Size" ) ) ;
	s.append( QObject::tr( "Note" ) ) ;

	return s ;
}

void engines::engine::baseEngine::updateEnginePaths( const Context&,QString&,QString&,QString& )
{
}

QString engines::engine::baseEngine::parseVersionInfo( const utils::qprocess::outPut& r )
{
	return utility::parseVersionInfo( r ) ;
}

void engines::engine::baseEngine::updateLocalOptions( QStringList& )
{
}

engines::engine::baseEngine::removeFilesStatus engines::engine::baseEngine::removeFiles( const QStringList& e,const QString& )
{
	engines::engine::baseEngine::removeFilesStatus s ;

	for( const auto& it : e ){

		auto m = utility::removeFile( it ) ;

		if( !m.isEmpty() ){

			s.add( it,m ) ;
		}
	}

	return s ;
}

engines::engine::baseEngine::optionsEnvironment engines::engine::baseEngine::setProxySetting( QStringList&,const QString& )
{
	return {} ;
}

QString engines::engine::baseEngine::setCredentials( QStringList&,QStringList& )
{
	return {} ;
}

QString engines::engine::baseEngine::downloadFolder( const QString& e )
{
	return e ;
}

util::Json engines::engine::baseEngine::parsePlayListData( const QString&,const QByteArray& e )
{
	return e ;
}

void engines::openUrls( const QString& s ) const
{
	m_settings.openUrl( s ) ;
}

void engines::engine::baseEngine::openLocalFile( const engines::engine::baseEngine::localFile& l )
{
	auto e = [ & ](){

		if( l.fileNames.size() == 0 ){

			auto m = util::split( l.uiText,'\n',true ) ;

			if( m.size() > 1 ){

				return m[ 1 ] ;
			}else{
				return QString() ;
			}
		}else{
			return QString( l.fileNames.back() ) ;
		}
	}() ;

	auto s = QDir::fromNativeSeparators( e ) ;
	auto ss = QDir::fromNativeSeparators( l.downloadFolder ) ;

	if( s.startsWith( ss ) ){

		m_settings.openUrl( s ) ;
	}else{
		m_settings.openUrl( l.downloadFolder + "/" + e ) ;
	}
}

engines::engine::baseEngine::onlineVersion engines::engine::baseEngine::versionInfoFromGithub( const QByteArray& e )
{
	QJsonParseError err ;
	auto doc = QJsonDocument::fromJson( e,&err ) ;

	if( err.error == QJsonParseError::NoError ){

		auto version = doc.object().value( "tag_name" ).toString() ;

		if( version.contains( "v" ) || version.contains( "," ) ){

			auto m = version ;

			m.replace( ",","" ).replace( "v","" ) ;

			return { m,m } ;
		}else{
			return { version,version } ;
		}
	}else{
		return { {},{} } ;
	}
}

bool engines::engine::baseEngine::foundNetworkUrl( const QString& s )
{
	return s == m_engine.commandName() ;
}

engines::engine::baseEngine::renameArchiveFolderStatus
engines::engine::baseEngine::renameArchiveFolder( const QString&,const QString& )
{
	return {} ;
}

QString engines::engine::baseEngine::updateTextOnCompleteDownlod( const QString& uiText,
								  const QString& dopts,
								  const QString& tabName,
								  const engines::engine::baseEngine::finishedState& f )
{
	Q_UNUSED( tabName )

	auto m = engines::engine::baseEngine::processCompleteStateText( f ) ;
	auto e = engines::engine::baseEngine::timer::stringElapsedTime( f.duration() ) ;

	if( dopts.isEmpty() || f.success() ){

		if( uiText.isEmpty() ){

			return m + ", " + e ;
		}else{
			return m + ", " + e + "\n" + uiText ;
		}
	}else{
		if( uiText.isEmpty() ){

			return dopts + "\n" + m + ", " + e ;
		}else{
			return dopts + "\n" + m + ", " + e + "\n" + uiText ;
		}
	}
}

QString engines::engine::baseEngine::updateTextOnCompleteDownlod( const QString& uiText,
								  const QString& bkText,
								  const QString& dopts,
								  const QString& tabName,
								  const engine::engine::baseEngine::finishedState& f )
{
	Q_UNUSED( uiText )

	if( f.success() ){

		return engines::engine::baseEngine::updateTextOnCompleteDownlod( bkText,dopts,tabName,f ) ;
	}else{
		return engines::engine::baseEngine::updateTextOnCompleteDownlod( bkText,dopts,tabName,f ) ;
	}
}

void engines::engine::baseEngine::sendCredentials( const QString&,QProcess& )
{
}

class updateLogger
{
public:
	updateLogger( const QByteArray& data,
		      const engines::engine& engine,
		      Logger::Data& outPut,
		      int id,
		      bool humanReadableJson ) :
		m_outPut( outPut ),
		m_filterOutPut( engine.filterOutput() ),
		m_id( id ),
		m_engine( engine )
	{
		if( data.isEmpty() ){

			return ;
		}

		if( this->validJson( humanReadableJson,data ) ){

			return ;
		}

		const auto& sp = m_engine.splitLinesBy() ;

		if( sp.size() == 1 && sp[ 0 ].size() > 0 ){

			this->add( data,sp[ 0 ][ 0 ] ) ;

		}else if( sp.size() == 2 && sp[ 0 ].size() > 0 && sp[ 1 ].size() > 0 ){

			const auto mm = util::split( data,sp[ 0 ][ 0 ] ) ;

			for( const auto& m : mm ){

				this->add( m,sp[ 1 ][ 0 ] ) ;
			}
		}else{
			const auto mm = util::split( data,'\r' ) ;

			for( const auto& m : mm ){

				this->add( m,'\n' ) ;
			}
		}
	}
private:
	QJsonDocument json( const QByteArray& data,QJsonParseError * err )
	{
		auto a = "0xdeadbeef>>MediaDownloaderEndMarker<<0xdeadbeef\n" ;

		if( data.endsWith( a ) ){

			auto m = QByteArray( data ).replace( a,"" ) ;

			return QJsonDocument::fromJson( m,err ) ;
		}else{
			return QJsonDocument::fromJson( data,err ) ;
		}
	}
	QByteArray updateFormats( const QJsonArray& oldFormats,QJsonObject& oldObject )
	{
		QJsonObject newObject ;

		QJsonArray newFormats ;

		for( const auto& it : oldFormats ){

			auto obj = it.toObject() ;

			obj.remove( "url" ) ;

			newFormats.append( obj ) ;
		}

		if( !newFormats.isEmpty() ){

			newObject.insert( "formats",newFormats ) ;
		}

		oldObject.remove( "formats" ) ;

		for( auto it = oldObject.begin() ; it != oldObject.end() ; it++ ){

			const auto& s = it.value() ;

			if( s.isString() ){

				auto ss = s.toString() ;

				if( ss != "NA" && ss != "\"NA\"" ){

					newObject.insert( it.key(),ss ) ;
				}
			}else{
				newObject.insert( it.key(),it.value() ) ;
			}
		}

		auto m = QJsonDocument::JsonFormat::Indented ;

		return QJsonDocument( newObject ).toJson( m ) ;
	}
	bool validJson( const QByteArray& data )
	{
		QJsonParseError err ;

		auto json = this->json( data,&err ) ;

		if( err.error == QJsonParseError::NoError ){

			auto oldObject = json.object() ;

			const auto oldFormats = oldObject.value( "formats" ).toArray() ;

			if( oldFormats.size() ){

				auto m = this->updateFormats( oldFormats,oldObject ) ;

				m_outPut.add( m,m_id ) ;
			}else{
				auto m = QJsonDocument::JsonFormat::Indented ;

				auto s = QJsonDocument( oldObject ).toJson( m ) ;

				m_outPut.add( s,m_id ) ;
			}

			return true ;
		}else{
			return false ;
		}
	}
	bool validJson( bool humanReadable,const QByteArray& data )
	{
		if( m_engine.likeYtDlp() && humanReadable ){

			if( data.startsWith( '[' ) || data.startsWith( '{' ) ){

				return this->validJson( data ) ;
			}
		}

		return false ;
	}
	bool skipLine( const QByteArray& line ) const
	{
		if( line.isEmpty() ){

			return true ;
		}else{
			for( const auto& it : m_engine.skiptLineWithText() ){

				if( line.contains( it.toUtf8() ) ){

					return true ;
				}
			}

			return false ;
		}
	}
	void add( const QByteArray& data,QChar token )
	{
		const auto mm = util::split( data,token ) ;

		for( const auto& e : mm ){

			if( !this->skipLine( e ) ){

				if( m_filterOutPut.meetCondition( m_locale,m_outPut,e ) ){

					this->logProgress( e ) ;
				}else{
					m_outPut.add( e,m_id ) ;
				}
			}
		}
	}
	void logProgress( const QByteArray& e )
	{
		auto result = m_filterOutPut.formatOutput( m_locale,m_outPut,e ) ;

		const auto& m = result.progress() ;

		if( m_outPut.mainLogger() ){

			if( !m.isEmpty() ){

				m_outPut.replaceOrAdd( m,m_id,result.meetCondition() ) ;
			}
		}else{
			m_outPut.replaceOrAdd( m,m_id,result.meetCondition() ) ;
		}
	}
	Logger::Data& m_outPut ;
	Logger::locale m_locale ;
	engines::engine::baseEngine::FilterOutPut m_filterOutPut ;
	int m_id ;
	const engines::engine& m_engine ;
} ;

void engines::engine::baseEngine::processData( Logger::Data& outPut,
					      const QByteArray& data,
					      int id,
					      bool readableJson )
{
	const auto& txt = m_engine.removeText() ;

	if( txt.isEmpty() ){

		updateLogger( data,m_engine,outPut,id,readableJson ) ;
	}else{
		auto dd = data ;

		for( const auto& it : txt ){

			dd.replace( it.toUtf8(),"" ) ;
		}

		updateLogger( dd,m_engine,outPut,id,readableJson ) ;
	}
}

void engines::engine::baseEngine::processData( Logger::Data& outPut,
					       const QString& e,
					       int id,
					       bool readableJson )
{
	Q_UNUSED( readableJson )

	outPut.replaceOrAdd( e.toUtf8(),id,[]( const QString& line ){

		auto a = line.startsWith( engines::engine::baseEngine::preProcessing::processingText() ) ;
		auto b = engines::engine::baseEngine::timer::timerText( line ) ;

		return a || b ;
	} ) ;
}

void engines::engine::baseEngine::updateDownLoadCmdOptions( const engines::engine::baseEngine::updateOpts& s,
							    bool downloadOptionsAsLast,
							    const QStringList& extraOpts )
{
	if( downloadOptionsAsLast ){

		if( !s.uiOptions.isEmpty() ){

			s.ourOptions.append( s.uiOptions ) ;
		}else{
			s.ourOptions.append( s.userOptions ) ;
		}
	}else{
		if( !s.userOptions.isEmpty() ){

			s.ourOptions.append( s.userOptions ) ;
		}else{
			s.ourOptions.append( s.uiOptions ) ;
		}
	}

	s.ourOptions.append( extraOpts ) ;
}

void engines::engine::baseEngine::updateGetPlaylistCmdOptions( QStringList& )
{
}

void engines::engine::baseEngine::updateCmdOptions( QStringList& )
{
}

engines::engine::baseEngine::baseEngine( settings& s,
					 const engines::engine& engine,
					 const QProcessEnvironment& env ) :
	m_settings( s ),m_engine( engine ),m_processEnvironment( env )
{
}

settings& engines::engine::baseEngine::Settings() const
{
	return m_settings ;
}

const engines::engine& engines::engine::baseEngine::engine() const
{
	return m_engine ;
}

void engines::file::write( const QString& e )
{
	if( m_file.open( QIODevice::WriteOnly ) ){

		m_file.write( e.toUtf8() ) ;
	}else{
		this->failToOpenForWriting() ;
	}
}

void engines::file::write( const QJsonDocument& doc,QJsonDocument::JsonFormat format )
{
	if( m_file.open( QIODevice::WriteOnly ) ){

		m_file.write( doc.toJson( format ) ) ;
	}else{
		this->failToOpenForWriting() ;
	}
}

void engines::file::write( const QJsonObject& obj,QJsonDocument::JsonFormat format )
{
	this->write( QJsonDocument( obj ),format ) ;
}

QByteArray engines::file::readAll()
{
	if( m_file.open( QIODevice::ReadOnly ) ){

		return m_file.readAll() ;
	}else{
		this->failToOpenForReading() ;
		return QByteArray() ;
	}
}

QStringList engines::file::readAllAsLines()
{
	QStringList m ;

	if( m_file.open( QIODevice::ReadOnly ) ){

		while( !m_file.atEnd() ){

			auto s = m_file.readLine().trimmed() ;

			if( !s.isEmpty() ){

				m.append( s ) ;
			}
		}
	}else{
		this->failToOpenForReading() ;
	}

	return m ;
}

void engines::file::failToOpenForWriting()
{
	auto id = utility::sequentialID() ;
	m_logger.add( QObject::tr( "Failed to open file for writing" ) + ": " + m_filePath,id ) ;
}

void engines::file::failToOpenForReading()
{
	auto id = utility::sequentialID() ;
	m_logger.add( QObject::tr( "Failed to open file for reading" ) + ": " + m_filePath,id ) ;
}

engines::engine::baseEngine::filter::filter( const engines::engine& engine,int id ) :
	m_engine( engine ),m_processId( id )
{
	if( m_processId ){}
}

const QByteArray& engines::engine::baseEngine::filter::operator()( Logger::Data& s )
{
	if( m_engine.replaceOutputWithProgressReport() ){

		return m_processing.text() ;

	}else if( s.isEmpty() ){

		static QByteArray e ;
		return e ;
	}else{
		if( utility::stringConstants::doneDownloadingText( s.lastText() ) ){

			return m_tmp ;
		}else{
			m_tmp = s.lastText() ;

			if( m_tmp.startsWith( "[media-downloader] cmd:" ) ){

				return m_processing.text() ;
			}else{
				return m_tmp ;
			}
		}
	}
}

engines::engine::baseEngine::filter::~filter()
{
}

const engines::engine& engines::engine::baseEngine::filter::engine() const
{
	return m_engine ;
}

engines::engine::baseEngine::preProcessing::preProcessing() :
	m_processingDefaultText( engines::engine::baseEngine::preProcessing::processingText() )
{
}

engines::engine::baseEngine::preProcessing::preProcessing( const QByteArray& e,int s ) :
	m_maxCounter( s ),
	m_processingDefaultText( e )
{
}

QByteArray engines::engine::baseEngine::preProcessing::processingText()
{
	return QObject::tr( "Processing" ).toUtf8() ;
}

void engines::engine::baseEngine::preProcessing::reset()
{
	m_counter = 0 ;
	m_counterDots = " ..." ;
}

const QByteArray& engines::engine::baseEngine::preProcessing::text()
{
	if( m_counter < m_maxCounter ){

		m_counterDots += " ..." ;
	}else{
		m_counterDots = " ..." ;
		m_counter = 0 ;
	}

	m_txt = m_processingDefaultText + m_counterDots ;

	m_counter++ ;

	return m_txt ;
}

const QByteArray& engines::engine::baseEngine::preProcessing::text( const QByteArray& e )
{
	if( m_counter < 16 ){

		m_counterDots += " ..." ;
	}else{
		m_counterDots = " ..." ;
		m_counter = 0 ;
	}

	m_counter++ ;

	m_txt = e + "\n" + m_processingDefaultText + m_counterDots ;

	return m_txt ;
}

QByteArray engines::engine::baseEngine::postProcessing::processingText()
{
	return QObject::tr( "Post Processing" ).toUtf8() ;
}

engines::engine::baseEngine::postProcessing::postProcessing() :
	m_processingDefaultText( engines::engine::baseEngine::postProcessing::processingText() )
{
}

engines::engine::baseEngine::postProcessing::postProcessing( const QByteArray& e ) :
	m_processingDefaultText( e )
{
}

const QByteArray& engines::engine::baseEngine::postProcessing::text( const QByteArray& e )
{
	if( m_counter < 16 ){

		m_counterDots += " ..." ;
	}else{
		m_counterDots = " ..." ;
		m_counter = 0 ;
	}

	m_counter++ ;

	m_txt = e + "\n" + m_processingDefaultText + m_counterDots ;

	return m_txt ;
}

bool engines::engine::baseEngine::timer::timerText( const QString& e )
{
	return e.startsWith( engines::engine::baseEngine::timer::timerText() ) ;
}

QString engines::engine::baseEngine::timer::timerText()
{
	return QObject::tr( "Elapsed Time:" ) + " " ;
}

QString engines::engine::baseEngine::timer::startTimerText()
{
	return engines::engine::baseEngine::timer::timerText() + "00:00:00" ;
}

QString engines::engine::baseEngine::timer::stringElapsedTime( qint64 milliseconds )
{
	if( milliseconds <= 0 ){

		return engines::engine::baseEngine::timer::startTimerText() ;
	}

	auto m = engines::engine::baseEngine::timer::duration( milliseconds ) ;

	return engines::engine::baseEngine::timer::timerText() + m ;
}

QString engines::engine::baseEngine::timer::duration( qint64 milliseconds )
{
	auto seconds = milliseconds / 1000;
	milliseconds = milliseconds % 1000;
	auto minutes = seconds / 60 ;
	seconds      = seconds % 60 ;
	auto hours   = minutes / 60 ;
	minutes      = minutes % 60 ;

	QTime time ;
	time.setHMS( int( hours ),int( minutes ),int( seconds ),int( milliseconds ) ) ;

	return time.toString( "hh:mm:ss" ) ;
}

int engines::engine::baseEngine::timer::toSeconds( const QString& e )
{
	auto _toNumber = []( const QString& e ){

		return e.toInt() ;
	} ;

	if( e.endsWith( "m" ) ){

		auto s = e ;
		s.replace( "m","" ) ;

		return 60 * _toNumber( s ) ;
	}

	auto m = util::split( e,':',true ) ;

	if( m.size() == 3 ){

		return 3600 * _toNumber( m[ 0 ] ) + 60 * _toNumber( m[ 1 ] ) + _toNumber( m[ 2 ] ) ;

	}else if( m.size() == 2 ){

		return 3600 * _toNumber( m[ 0 ] ) + 360 * _toNumber( m[ 1 ] ) ;

	}else if( m.size() == 1 ){

		return 3600 * _toNumber( m[ 0 ] ) ;
	}else{
		return 0 ;
	}
}

qint64 engines::engine::baseEngine::timer::elapsedTime()
{
	return engines::engine::baseEngine::timer::currentTime() - m_startTime ;
}

qint64 engines::engine::baseEngine::timer::currentTime()
{
	return QDateTime().currentMSecsSinceEpoch() ;
}

QString engines::engine::baseEngine::timer::stringElapsedTime()
{
	return engines::engine::baseEngine::timer::stringElapsedTime( this->elapsedTime() ) ;
}

void engines::engine::baseEngine::timer::reset()
{
	m_startTime = engines::engine::baseEngine::timer::currentTime() ;
}

engines::configDefaultEngine::configDefaultEngine( Logger&logger,const enginePaths& enginePath ) :
	m_name( "yt-dlp" ),
	m_configFileName( m_name + ".json" )
{
	yt_dlp::init( this->name(),this->configFileName(),logger,enginePath ) ;

	if( utility::platformIsWindows() ){

		aria2c::init( "aria2c","aria2c.json",logger,enginePath ) ;
		wget::init( "wget","wget.json",logger,enginePath ) ;

		if( utility::platformisLegacyWindows() ){

			quickjs::init( "quickjs","quickjs.json",logger,enginePath ) ;
		}else{
			deno::init( "deno","deno.json",logger,enginePath ) ;
		}

	}else if( utility::platformisFlatPak() ){

		quickjs::init( "quickjs","quickjs.json",logger,enginePath ) ;
	}else{
		deno::init( "deno","deno.json",logger,enginePath ) ;
	}
}

engines::engine::baseEngine::filterOutPut::~filterOutPut()
{
}

bool engines::proxySettings::operator!=( const engines::proxySettings& other ) const
{
	return this->networkProxyString() != other.networkProxyString() ;
}

QNetworkProxy engines::proxySettings::toQNetworkProxy( const QString& u ) const
{
	QNetworkProxy proxy ;

	if( u.isEmpty() ){

		proxy.setType( QNetworkProxy::NoProxy ) ;

		return proxy ;
	}else{
		auto url = u ;

		if( url.startsWith( "socks5" ) ){

			proxy.setType( QNetworkProxy::Socks5Proxy ) ;
		}else{
			proxy.setType( QNetworkProxy::HttpProxy ) ;
		}

		auto e = url.indexOf( "://" ) ;

		if( e != -1 ){

			url = url.mid( e + 3 ) ;
		}

		e = url.indexOf( '@' ) ;

		if( e != -1 ){

			auto credentials = url.mid( 0,e ) ;

			auto ee = credentials.indexOf( ':' ) ;

			if( ee != -1 ){

				proxy.setUser( credentials.mid( 0,ee ) ) ;
				proxy.setPassword( credentials.mid( ee + 1 ) ) ;
			}

			url = url.mid( e + 1 ) ;
		}

		e = url.indexOf( ':' ) ;

		if( e != -1 ){

			proxy.setPort( url.mid( e + 1 ).replace( "/","" ).toInt() ) ;

			url = url.mid( 0,e ) ;
		}

		proxy.setHostName( url ) ;

		if( proxy.hostName().isEmpty() ){

			proxy.setType( QNetworkProxy::NoProxy ) ;
		}

		return proxy ;
	}
}

void engines::proxySettings::setApplicationProxy( const QString& e ) const
{
	if( m_currentProxyString != e ){

		m_currentProxyString = e ;

		QNetworkProxy::setApplicationProxy( this->toQNetworkProxy( e ) ) ;
	}
}

void engines::proxySettings::setDefaultProxy() const
{
	QNetworkProxy::setApplicationProxy( m_networkProxy ) ;
}

QString engines::proxySettings::toString( const QNetworkProxy& e ) const
{
	if( e.type() == QNetworkProxy::NoProxy ){

		return {} ;
	}else{
		QString type ;
		QString credentials ;
		QString host ;

		if( e.type() == QNetworkProxy::Socks5Proxy ){

			type = "socks5://" ;
		}

		if( !e.user().isEmpty() && !e.password().isEmpty() ){

			credentials = e.user() + ":" + e.password() + "@" ;
		}

		if( !e.hostName().isEmpty() ){

			host = e.hostName() + ":" + QString::number( e.port() ) ;
		}

		return type + credentials + host ;
	}
}

QProcessEnvironment engines::engine::baseEngine::optionsEnvironment::update( const QProcessEnvironment& e ) const
{
	auto m = e ;

	for( const auto& it : m_pairs ){

		m.insert( it.key,it.value ) ;
	}

	return m ;
}

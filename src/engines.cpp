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

#include "downloadmanager.hpp"
#include "utility.h"
#include "version.h"
#include "tableWidget.h"
#include "context.hpp"

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QDesktopServices>

#include <QDir>

static QProcessEnvironment _getEnvPaths( const engines::enginePaths& paths,settings& settings )
{
	auto env = QProcessEnvironment::systemEnvironment() ;

	const auto& basePath = paths.binPath() ;

	auto m = QDir( basePath ).entryList( QDir::Filter::Dirs | QDir::Filter::NoDotAndDotDot ) ;

	auto separator = [ & ](){

		if( utility::platformIsLikeWindows() ){

			return ";" ;
		}else{
			return ":" ;
		}
	}() ;

	auto s = basePath ;

	if( utility::platformIsWindows() ){

		auto mm = settings.exeOriginalPath() ;

		s += separator + mm ;

		auto m = QDir( mm + "/3rdParty" ).entryList( QDir::Filter::Dirs | QDir::Filter::NoDotAndDotDot ) ;

		for( const auto& it : m ){

			s += separator + mm + "/3rdParty/" + it ;
			s += separator + mm + "/3rdParty/" + it + "/bin" ;
		}
	}

	for( const auto& it : m ){

		s += separator + basePath + "/" + it ;
		s += separator + basePath + "/" + it + "/bin" ;
	}

	auto p = env.value( "PATH" ) ;

	if( s.endsWith( separator ) ){

		env.insert( "PATH",s + p ) ;
	}else{
		env.insert( "PATH",s + separator + p ) ;
	}

	env.insert( "LANG","C" ) ;

	return env ;
}

engines::engines( Logger& l,settings& s,int id ) :
	m_logger( l ),
	m_settings( s ),
	m_enginePaths( m_settings ),
	m_processEnvironment( _getEnvPaths( m_enginePaths,m_settings ) ),
	m_defaultEngine( l,s )
{
	if( s.showVersionInfoWhenStarting() ){

		m_logger.add( QByteArray( "*****************************************************" ),id ) ;

		m_logger.add( QObject::tr( "To Disable These Checks, Do The Following:-" ),id ) ;
		m_logger.add( QObject::tr( "1. Go To \"Configure\" Tab." ),id ) ;
		m_logger.add( QObject::tr( "2. Go To \"General Options\" Sub Tab." ),id ) ;
		m_logger.add( QObject::tr( "3. Uncheck \"Show Version Info When Starting\"." ),id ) ;

		m_logger.add( QByteArray( "*****************************************************" ),id ) ;
	}

	const auto& utxt = m_settings.runningUpdatedText() ;

	if( !utxt.isEmpty() ){

		m_logger.add( utxt,id ) ;
	}

	if( m_settings.portableVersion() ){

		m_logger.add( QObject::tr( "Running in portable mode" ),id ) ;
		m_logger.add( QObject::tr( "Download path: " ) + m_settings.downloadFolder( m_logger ),id ) ;
	}
	this->updateEngines( true,id ) ;
}

static void _openUrls( tableWidget& table,int row,settings& settings,bool galleryDl )
{
	if( downloadManager::finishedStatus::finishedWithSuccess( table,row ) ){

		auto m = util::split( table.uiText( row ),'\n',true ) ;

		m.removeFirst() ;

		for( const auto& it : m ){

			if( galleryDl ){

				auto e = settings.downloadFolder() ;
				auto m = QUrl::fromLocalFile( e + "/gallery-dl/" + it ) ;

				QDesktopServices::openUrl( m ) ;
			}else{
				auto s = QDir::fromNativeSeparators( it ) ;
				auto ss = QDir::fromNativeSeparators( settings.downloadFolder() ) ;

				if( s.startsWith( ss ) ){

					auto m = QUrl::fromLocalFile( s ) ;

					QDesktopServices::openUrl( m ) ;
				}else{
					auto m = QUrl::fromLocalFile( settings.downloadFolder() + "/" + it ) ;

					QDesktopServices::openUrl( m ) ;
				}
			}
		}
	}
}

void engines::openUrls( tableWidget& table,int row ) const
{
	_openUrls( table,row,m_settings,false ) ;
}

void engines::openUrls( tableWidget& table,int row,const engines::engine& engine ) const
{
	_openUrls( table,row,m_settings,engine.name() == "gallery-dl" ) ;
}

void engines::openUrls( const QString& path ) const
{
	auto m = QUrl::fromLocalFile( path ) ;

	QDesktopServices::openUrl( m ) ;
}

const QString& engines::defaultEngineName() const
{
	return m_defaultEngine.name() ;
}

static util::result< engines::engine > _get_engine_by_path( const QString& e,
							    const engines& engines,
							    Logger& logger,
							    const engines::enginePaths& enginePaths )
{
	auto path = enginePaths.enginePath( e ) ;

	util::Json json( engines::file( path,logger ).readAll() ) ;

	if( json ){

		auto object = json.doc().object() ;

		auto minVersion = object.value( "RequiredMinimumVersionOfMediaDownloader" ).toString() ;

		if( !minVersion.isEmpty() ){

			if( util::version( minVersion ) > VERSION ){

				auto name = object.value( "Name" ).toString() ;

				auto m = QObject::tr( "Engine \"%1\" requires atleast version \"%2\" of Media Downloader" ) ;

				logger.add( m.arg( name,minVersion ),utility::sequentialID() ) ;

				return {} ;
			}
		}

		return { logger,enginePaths,object,engines,utility::sequentialID() } ;
	}else{
		return {} ;
	}
}

QStringList engines::engine::dumpJsonArguments( engines::engine::tab tab ) const
{
	if( tab == engine::tab::playlist ){

		return yt_dlp::jsonNoFormatsArgumentList() ;
	}else{
		return m_dumpJsonArguments ;
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

void engines::updateEngines( bool addAll,int id )
{
	m_backends.clear() ;

	auto _engine_add = [ & ]( const QString& jsonFile,util::result< engines::engine > m ){

		if( m ){

			if( m->exePath().isEmpty() ){

				m_logger.add( QObject::tr( "Error, executable to backend \"%1\" could not be found" ).arg( m->name() ),id ) ;
			}else{
				m_backends.emplace_back( std::move( m.value() ) ) ;
			}
		}else{
			m_logger.add( QObject::tr( "Error, failed to parse config file \"%1\"" ).arg( jsonFile ),id ) ;
		}
	} ;

	_engine_add( "",_get_engine_by_path( m_defaultEngine.configFileName(),*this,m_logger,m_enginePaths ) ) ;

	for( const auto& it : this->enginesList() ){

		_engine_add( it,_get_engine_by_path( it,*this,m_logger,m_enginePaths ) ) ;
	}

	if( addAll ){

		_engine_add( "",{ *this,m_logger,"ffmpeg","-version",0,2,id } ) ;

		_engine_add( "",{ *this,m_logger,"aria2c","--version",0,2,id } ) ;

		for( const auto& it : this->getEngines() ){

			const auto& e = it.exePath().exe() ;

			if( e.size() > 0 && e.at( 0 ).contains( "python" ) ){

				if( utility::platformIsWindows() ){

					_engine_add( it.name(),{ *this,m_logger,"python","--version",0,1,id } ) ;
				}else{
					_engine_add( it.name(),{ *this,m_logger,"python3","--version",0,1,id } ) ;
				}
				break ;
			}
		}
	}

	const auto& engines = *this ;

	for( auto& it : m_backends ){

		const auto& name = it.name() ;

		if( it.likeYoutubeDl() ){

			it.setBackend< yt_dlp >( engines,m_logger,m_enginePaths,it.versionInfo() ) ;

		}else if( name.contains( "safaribooks" ) ){

			it.setBackend< safaribooks >( engines ) ;

		}else if( name.contains( "gallery-dl" ) ){

			it.setBackend< gallery_dl >( engines ) ;

		}else if( name == "aria2c" ){

			it.setBackend< aria2c >( engines ) ;

		}else if( name.contains( "lux" ) ){

			it.setBackend< lux >( engines,m_settings.downloadFolder() ) ;

		}else if( name.contains( "you-get" ) ){

			it.setBackend< you_get >( engines ) ;

		}else if( name.contains( "svtplay-dl" ) ){

			it.setBackend< svtplay_dl >( engines ) ;

		}else if( name.contains( "wget" ) ){

			it.setBackend< wget >( engines ) ;

		}else if( it.mainEngine() ){

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
	auto m =  this->getEngineByName( name ) ;

	if( m ){

		return m.value() ;
	}else{
		m_logger.add( "Error: engines::defaultEngine: Unknown Engine: " + name,id ) ;

		if( m_backends.size() > 0 ){

			return m_backends[ 0 ] ;
		}else{
			static engines::engine engine( m_logger ) ;

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

const engines::enginePaths& engines::engineDirPaths() const
{
	return m_enginePaths ;
}

static QStringList _toStringList( const QJsonValue& value,bool protectSpace = false ){

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

settings& engines::Settings() const
{
	return m_settings ;
}

bool engines::filePathIsValid( const QFileInfo& info )
{
	if( utility::platformIsLikeWindows() ){

		return info.exists() && info.isFile() ;
	}else{
		return info.exists() && info.isFile() && info.isExecutable() ;
	}
}

static QString _findExecutable( const QString& exeName,const QStringList& paths,QFileInfo& info )
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
	QFileInfo info( exeName ) ;

	if( info.isAbsolute() ){

		return exeName ;
	}

	auto paths = [ this ](){

		if( utility::platformIsWindows() || utility::platformisOS2() ){

			return this->processEnvironment().value( "PATH" ).split( ';' ) ;
		}else{
			return this->processEnvironment().value( "PATH" ).split( ':' ) ;
		}
	}() ;

	if( utility::platformIsWindows() ){

		auto m = _findExecutable( exeName,paths,info ) ;

		if( m.isEmpty() && !exeName.endsWith( ".exe" ) ){

			m = _findExecutable( exeName + ".exe",paths,info ) ;
		}

		return m ;
	}else{
		return _findExecutable( exeName,paths,info ) ;
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

			if( f.open( QIODevice::WriteOnly ) ){

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
	const auto& engine = _get_engine_by_path( e,*this,m_logger,m_enginePaths ) ;

	if( engine && engine->valid() ){

		QFile::remove( m_enginePaths.enginePath( e ) ) ;

		if( engine->archiveContainsFolder() ){

			QFileInfo m( m_enginePaths.binPath( engine->name() ) ) ;

			if( m.exists() && m.isDir() ){

				QDir( m.filePath() ).removeRecursively() ;
			}
		}else{
			auto exe = QDir::fromNativeSeparators( engine->exePath().realExe() ) ;
			auto binPath = QDir::fromNativeSeparators( m_enginePaths.binPath() ) ;

			if( exe.startsWith( binPath ) && QFile::exists( exe ) ){

				QFile::remove( exe ) ;
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

	return m ;
}

engines::engine::engine( Logger& )
{
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
	m_mainEngine( false ),
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

void engines::engine::updateOptions()
{
	m_controlStructure                = m_jsonObject.value( "ControlJsonStructure" ).toObject() ;
	m_canDownloadPlaylist             = m_jsonObject.value( "CanDownloadPlaylist" ).toBool() ;
	m_replaceOutputWithProgressReport = m_jsonObject.value( "ReplaceOutputWithProgressReport" ).toBool( false ) ;
	m_userName                        = m_jsonObject.value( "UserName" ).toString() ;
	m_password                        = m_jsonObject.value( "Password" ).toString() ;
	m_optionsArgument                 = m_jsonObject.value( "OptionsArgument" ).toString() ;
	m_playlistItemsArgument           = m_jsonObject.value( "PlaylistItemsArgument" ).toString() ;
	m_batchFileArgument               = m_jsonObject.value( "BatchFileArgument" ).toString() ;
	m_cookieArgument                  = m_jsonObject.value( "CookieArgument" ).toString() ;
	m_encodingArgument                = m_jsonObject.value( "EncodingArgument" ).toString() ;
	m_dumpJsonArguments               = _toStringList( m_jsonObject.value( "DumptJsonArguments" ) ) ;
	m_splitLinesBy                    = _toStringList( m_jsonObject.value( "SplitLinesBy" ) ) ;
	m_removeText                      = _toStringList( m_jsonObject.value( "RemoveText" ) ) ;
	m_skiptLineWithText               = _toStringList( m_jsonObject.value( "SkipLineWithText" ) ) ;
	m_defaultDownLoadCmdOptions       = _toStringList( m_jsonObject.value( "DefaultDownLoadCmdOptions" ),true ) ;
	m_defaultListCmdOptions           = _toStringList( m_jsonObject.value( "DefaultListCmdOptions" ) ) ;
	m_defaultCommentsCmdOptions       = _toStringList( m_jsonObject.value( "DefaultCommentsCmdOptions" ) ) ;
	m_defaultSubstitlesCmdOptions     = _toStringList( m_jsonObject.value( "DefaultSubstitlesCmdOptions" ) ) ;
	m_defaultSubtitleDownloadOptions  = _toStringList( m_jsonObject.value( "DefaultSubtitleDownloadOptions" ) ) ;
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
	m_likeYoutubeDl( m_jsonObject.value( "LikeYoutubeDl" ).toBool( false ) ),
	m_mainEngine( true ),
	m_archiveContainsFolder( m_jsonObject.value( "ArchiveContainsFolder" ).toBool() ),
	m_versionArgument( m_jsonObject.value( "VersionArgument" ).toString() ),
	m_name( m_jsonObject.value( "Name" ).toString() ),
	m_exeFolderPath( m_jsonObject.value( "BackendPath" ).toString() ),
	m_downloadUrl( m_jsonObject.value( "DownloadUrl" ).toString() )
{
	auto defaultPath = utility::stringConstants::defaultPath() ;
	auto backendPath = utility::stringConstants::backendPath() ;

	if( m_exeFolderPath == defaultPath || m_exeFolderPath == backendPath ){

		m_exeFolderPath = ePaths.binPath() ;
	}

	auto cmd = m_jsonObject.value( "Cmd" ) ;

	if( cmd.isUndefined() ){

		if( utility::platformIsWindows() ){

			m_commandName = m_jsonObject.value( "CommandNameWindows" ).toString() ;

			if( utility::platformIs32Bit() ){

				auto m = m_jsonObject.value( "CommandName32BitWindows" ).toString() ;

				if( !m.isEmpty() ){

					m_commandName = m ;
				}
			}

			if( !m_commandName.endsWith( ".exe" ) ){

				m_commandName += ".exe" ;
			}
		}else{
			m_commandName = m_jsonObject.value( "CommandName" ).toString() ;
		}

		auto cmdNames = [ & ](){

			if( utility::platformIsWindows() ){

				if( utility::platformIs32Bit() ){

					auto m = _toStringList( m_jsonObject.value( "CommandNames32BitWindows" ) ) ;

					if( !m.isEmpty() ){

						return m ;
					}else{
						return _toStringList( m_jsonObject.value( "CommandNamesWindows" ) ) ;
					}
				}else{
					return _toStringList( m_jsonObject.value( "CommandNamesWindows" ) ) ;
				}
			}else{
				return _toStringList( m_jsonObject.value( "CommandNames" ) ) ;
			}
		}() ;

		if( cmdNames.isEmpty() ){

			this->parseMultipleCmdArgs( logger,engines,ePaths,id ) ;
		}else{
			this->parseMultipleCmdArgs( cmdNames,backendPath,logger,ePaths,engines,id ) ;
		}
	}else{
		auto cmdNames = [ & ](){

			auto obj = [ & ](){

				if( utility::platformIsWindows() ){

					return cmd.toObject().value( "Windows" ).toObject() ;
				}else{
					if( utility::platformIsOSX() ){

						auto m = cmd.toObject().value( "MacOS" ) ;

						if( !m.isUndefined() ){

							return m.toObject() ;
						}
					}

					return cmd.toObject().value( "Generic" ).toObject() ;
				}
			}() ;

			if( utility::platformIs32Bit() ){

				auto o = obj.value( "x86" ).toObject() ;

				m_commandName = o.value( "Name" ).toString() ;
				return _toStringList( o.value( "Args" ).toArray() ) ;
			}else{
				auto o = obj.value( "amd64" ).toObject() ;

				m_commandName = o.value( "Name" ).toString() ;
				return _toStringList( o.value( "Args" ).toArray() ) ;
			}
		}() ;

		if( cmdNames.size() == 1 ){

			this->parseMultipleCmdArgs( logger,engines,ePaths,id ) ;
		}else{
			this->parseMultipleCmdArgs( cmdNames,backendPath,logger,ePaths,engines,id ) ;
		}
	}
}

QString engines::engine::updateCmdPath( const QString& e ) const
{
	auto exe = m_functions->updateCmdPath( e ) ;

	if( exe.isEmpty() ){

		//problems ahead, exe should never be empty
	}

	const_cast< engines::engine::exeArgs * >( &m_exePath )->updateRealExe( exe ) ;

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
		if( this->validDownloadUrl() &&
		    !m_commandName.startsWith( "media-downloader" ) &&
		    !m_exeFolderPath.isEmpty() ){

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
	return m_functions->breakShowListIfContains( e ) ;
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

			return m ;
		}
	}

	return {} ;
}

void engines::engine::setPermissions( const QString& e ) const
{
	utility::setPermissions( e ) ;
}

engines::enginePaths::enginePaths( settings& s )
{
	m_basePath      = s.configPaths() ;

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

QString engines::engine::functions::errorString( const engine::engine::functions::finishedState& f,
						 engines::engine::functions::errors err,
						 const QString& bkText )
{
	if( err == engines::engine::functions::errors::unknownFormat ){

		auto m = engines::engine::functions::processCompleteStateText( f ) ;
		return m + "\n" + QObject::tr( "Requested Format Is Not Available" ) + "\n" + bkText ;

	}else if( err == engines::engine::functions::errors::noNetwork ){

		auto m = engines::engine::functions::processCompleteStateText( f ) ;
		return m + "\n" + QObject::tr( "Network Error Occured" ) + "\n" + bkText ;

	}else if( err == engines::engine::functions::errors::unknownUrl ){

		auto m = engines::engine::functions::processCompleteStateText( f ) ;
		return m + "\n" + QObject::tr( "Invalid Url Entered" ) + "\n" + bkText ;

	}else if( err == engines::engine::functions::errors::notSupportedUrl ){

		auto m = engines::engine::functions::processCompleteStateText( f ) ;
		return m + "\n" + QObject::tr( "Url Is Not Supported" ) + "\n" + bkText ;
	}else{
		return {} ;
	}
}

QString engines::engine::functions::processCompleteStateText( const engine::engine::functions::finishedState& f )
{
	if( f.cancelled() ){

		return QObject::tr( "Download cancelled" ) ;

	}else if( f.success() ){

		return QObject::tr( "Download completed" ) ;
	}else{
		if( f.exitStatus() == QProcess::NormalExit ){

			auto m = QString::number( f.errorCode() ) ;

			auto a = QObject::tr( "Download Failed" ) ;
			auto b = "(" + QObject::tr( "ErrorCode" ) + "=" + m + ")" ;

			return a + b ;
		}else{
			return QObject::tr( "Download Failed, Engine crashed" ) ;
		}
	}
}

static bool _meetExtraCondition( const QByteArray& l,const QJsonObject& obj )
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

bool engines::engine::functions::meetCondition( const engines::engine& engine,const QByteArray& line )
{
	const auto& obj = engine.controlStructure() ;

	auto connector = obj.value( "Connector" ).toString() ;

	if( connector.isEmpty() ){

		auto m = obj.value( "lhs" ) ;

		if( m.isObject() ){

			return _meetExtraCondition( line,m.toObject() ) ;
		}else{
			return false ;
		}
	}else{
		auto obj1 = obj.value( "lhs" ) ;
		auto obj2 = obj.value( "rhs" ) ;

		if( obj1.isObject() && obj2.isObject() ){

			auto a = _meetExtraCondition( line,obj1.toObject() ) ;
			auto b = _meetExtraCondition( line,obj2.toObject() ) ;

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

class defaultFilter : public engines::engine::functions::filterOutPut
{
public:
	defaultFilter( const engines::engine& engine ) : m_engine( engine )
	{
	}
	engines::engine::functions::filterOutPut::result
	formatOutput( const Logger::locale&,Logger::Data&,const QByteArray& e ) const override
	{
		return { e,m_engine,engines::engine::functions::meetCondition } ;
	}
	bool meetCondition( const Logger::locale&,Logger::Data&,const QByteArray& e ) const override
	{
		return engines::engine::functions::meetCondition( m_engine,e ) ;
	}
	const engines::engine& engine() const override
	{
		return m_engine ;
	}
private:
	const engines::engine& m_engine ;
} ;

engines::engine::functions::FilterOutPut engines::engine::functions::filterOutput()
{
	return { util::types::type_identity< defaultFilter >(),m_engine } ;
}

engines::engine::functions::~functions()
{
}

const QProcessEnvironment& engines::engine::functions::processEnvironment() const
{
	return m_processEnvironment ;
}

std::vector< engines::engine::functions::mediaInfo > engines::engine::functions::mediaProperties( const QByteArray& e )
{
	auto args = util::split( e,'\n' ) ;

	QStringList m ;

	utility::make_reverseIterator( args ).forEach( [ & ]( const QByteArray& s ){

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

	std::vector< engines::engine::functions::mediaInfo > s ;

	for( const auto& it : m ){

		auto a = util::split( it,' ',true ) ;

		if( a.size() > 3 ){

			auto format     = a.takeAt( 0 ) ;
			auto extension  = a.takeAt( 0 ) ;
			auto resolution = a.takeAt( 0 ) ;
			auto notes      = a.join( " " ) ;

			s.emplace_back( format,extension,resolution,notes ) ;
		}
	}

	return s ;
}

std::vector< engines::engine::functions::mediaInfo > engines::engine::functions::mediaProperties( const QJsonArray& )
{
	return {} ;
}

void engines::engine::functions::updateOutPutChannel( QProcess::ProcessChannel& ) const
{
}

bool engines::engine::functions::breakShowListIfContains( const QStringList& )
{
	return false ;
}

bool engines::engine::functions::supportsShowingComments()
{
	return false ;
}

bool engines::engine::functions::updateVersionInfo()
{
	return false ;
}

void engines::engine::functions::setTextEncondig( const QString&,QStringList& )
{
}

QString engines::engine::functions::updateCmdPath( const QString& e )
{
	return e ;
}

engines::engine::functions::DataFilter engines::engine::functions::Filter( int id )
{
	return { util::types::type_identity< engines::engine::functions::filter >(),m_engine,id } ;
}

void engines::engine::functions::runCommandOnDownloadedFile( const QString& e,const QString& s )
{
	auto a = m_settings.commandOnSuccessfulDownload() ;

	if( !a.isEmpty() && !e.isEmpty() ){

		auto args = util::split( a,' ',true ) ;
		auto exe = args.takeAt( 0 ) ;
		args.append( "bla bla bla" ) ;

		bool success = false ;

		for( const auto& it : util::split( e,'\n',true ) ){

			auto b = m_settings.downloadFolder() + it ;

			if( QFile::exists( b ) ){

				success = true ;
				args.replace( args.size() - 1,b ) ;

				QProcess::startDetached( exe,args ) ;
			}
		}

		if( !success && !s.isEmpty() ){

			auto b = m_settings.downloadFolder() + "/" + util::split( s,'/',true ).last() ;

			if( QFile::exists( b ) ){

				args.replace( args.size() - 1,b ) ;

				QProcess::startDetached( exe,args ) ;
			}
		}
	}
}

QString engines::engine::functions::commandString( const engines::engine::exeArgs::cmd& cmd )
{
	auto m = "\"" + cmd.exe() + "\"" ;

	for( const auto& it : cmd.args() ){

		m += " \"" + it + "\"" ;
	}

	return m ;
}

QStringList engines::engine::functions::horizontalHeaderLabels() const
{
	QStringList s ;

	s.append( QObject::tr( "Format Code" ) ) ;
	s.append( QObject::tr( "Extension" ) ) ;
	s.append( QObject::tr( "Resolution" ) ) ;
	s.append( QObject::tr( "Note" ) ) ;

	return s ;
}

void engines::engine::functions::updateEnginePaths( const Context&,QString&,QString&,QString& )
{
}

bool engines::engine::functions::likeYtdlp()
{
	return false ;
}

void engines::engine::functions::updateLocalOptions( QStringList& )
{
}

QString engines::engine::functions::setCredentials( QStringList&,QStringList& )
{
	return {} ;
}

engines::engine::functions::onlineVersion engines::engine::functions::versionInfoFromGithub( const QByteArray& e )
{
	QJsonParseError err ;
	auto doc = QJsonDocument::fromJson( e,&err ) ;

	if( err.error == QJsonParseError::NoError ){

		auto version = doc.object().value( "tag_name" ).toString() ;

		if( version.contains( "v" ) || version.contains( "," ) ){

			auto m = version ;

			m.replace( ",","" ).replace( "v","" ) ;

			return { version,m } ;
		}else{
			return { version,version } ;
		}
	}else{
		return { {},{} } ;
	}
}

bool engines::engine::functions::foundNetworkUrl( const QString& s )
{
	return s == m_engine.commandName() ;
}

void engines::engine::functions::renameArchiveFolder( const QString& )
{
}

QString engines::engine::functions::updateTextOnCompleteDownlod( const QString& uiText,
								 const QString& dopts,
								 const engines::engine::functions::finishedState& f )
{
	auto m = engines::engine::functions::processCompleteStateText( f ) ;
	auto e = engines::engine::functions::timer::stringElapsedTime( f.duration() ) ;

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

QString engines::engine::functions::updateTextOnCompleteDownlod( const QString& uiText,
								 const QString& bkText,
								 const QString& dopts,
								 const engine::engine::functions::finishedState& f )
{
	Q_UNUSED( uiText )

	if( f.success() ){

		return engines::engine::functions::updateTextOnCompleteDownlod( bkText,dopts,f ) ;
	}else{
		return engines::engine::functions::updateTextOnCompleteDownlod( bkText,dopts,f ) ;
	}
}

void engines::engine::functions::sendCredentials( const QString&,QProcess& )
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

			for( const auto& m : util::split( data,sp[ 0 ][ 0 ] ) ){

				this->add( m,sp[ 1 ][ 0 ] ) ;
			}
		}else{
			for( const auto& m : util::split( data,'\r' ) ){

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
	bool validJson( const QByteArray& data )
	{
		QJsonParseError err ;

		auto json = this->json( data,&err ) ;

		if( err.error == QJsonParseError::NoError ){

			auto obj = json.object() ;

			auto oldFormats = obj.value( "formats" ).toArray() ;

			QJsonArray newFormats ;

			for( const auto& it : oldFormats ){

				auto obj = it.toObject() ;

				obj.remove( "url" ) ;

				newFormats.append( obj ) ;
			}

			if( newFormats.isEmpty() ){

				obj.remove( "formats" ) ;
			}else{
				obj.insert( "formats",newFormats ) ;
			}

			auto m = QJsonDocument::JsonFormat::Indented ;

			auto s = QJsonDocument( obj ).toJson( m ) ;

			m_outPut.add( s,m_id ) ;

			return true ;
		}else{
			return false ;
		}
	}
	bool validJson( bool humanReadable,const QByteArray& data )
	{
		if( m_engine.likeYoutubeDl() && humanReadable ){

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
		for( const auto& e : util::split( data,token ) ){

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

		if( m_outPut.mainLogger() ){

			if( !result.progress().isEmpty() ){

				const auto& m = result.progress() ;

				m_outPut.replaceOrAdd( m,m_id,result.meetCondition() ) ;
			}
		}else{
			m_outPut.setFilePath( result.fileName() ) ;

			const auto& m = result.progress() ;

			m_outPut.replaceOrAdd( m,m_id,result.meetCondition() ) ;
		}
	}
	Logger::Data& m_outPut ;
	Logger::locale m_locale ;
	engines::engine::functions::FilterOutPut m_filterOutPut ;
	int m_id ;
	const engines::engine& m_engine ;
} ;

void engines::engine::functions::processData( Logger::Data& outPut,
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

void engines::engine::functions::processData( Logger::Data& outPut,
					      const QString& e,
					      int id,
					      bool readableJson )
{
	Q_UNUSED( readableJson )

	outPut.replaceOrAdd( e.toUtf8(),id,[]( const QString& line ){

		auto a = line.startsWith( engines::engine::functions::preProcessing::processingText() ) ;
		auto b = engines::engine::functions::timer::timerText( line ) ;

		return a || b ;

	} ) ;
}

void engines::engine::functions::updateDownLoadCmdOptions( const engines::engine::functions::updateOpts& s )
{
	if( !s.uiOptions.isEmpty() ){

		s.ourOptions.append( s.uiOptions ) ;
	}
}

void engines::engine::functions::updateGetPlaylistCmdOptions( QStringList& )
{
}

void engines::engine::functions::updateCmdOptions( QStringList& )
{
}

engines::engine::functions::functions( settings& s,const engines::engine& engine,const QProcessEnvironment& env ) :
	m_settings( s ),m_engine( engine ),m_processEnvironment( env )
{
}

settings& engines::engine::functions::Settings() const
{
	return m_settings ;
}

const engines::engine& engines::engine::functions::engine() const
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

engines::engine::functions::filter::filter( const engines::engine& engine,int id ) :
	m_engine( engine ),m_processId( id )
{
	if( m_processId ){}
}

const QByteArray& engines::engine::functions::filter::operator()( const Logger::Data& s )
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

engines::engine::functions::filter::~filter()
{
}

const engines::engine& engines::engine::functions::filter::engine() const
{
	return m_engine ;
}

engines::engine::functions::preProcessing::preProcessing() :
	m_processingDefaultText( engines::engine::functions::preProcessing::processingText() )
{
}

engines::engine::functions::preProcessing::preProcessing( const QByteArray& e,int s ) :
	m_maxCounter( s ),
	m_processingDefaultText( e )
{
}

QByteArray engines::engine::functions::preProcessing::processingText()
{
	return QObject::tr( "Processing" ).toUtf8() ;
}

void engines::engine::functions::preProcessing::reset()
{
	m_counter = 0 ;
	m_counterDots = " ..." ;
}

const QByteArray& engines::engine::functions::preProcessing::text()
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

const QByteArray& engines::engine::functions::preProcessing::text( const QByteArray& e )
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

QByteArray engines::engine::functions::postProcessing::processingText()
{
	return QObject::tr( "Post Processing" ).toUtf8() ;
}

engines::engine::functions::postProcessing::postProcessing() :
	m_processingDefaultText( engines::engine::functions::postProcessing::processingText() )
{
}

engines::engine::functions::postProcessing::postProcessing( const QByteArray& e ) :
	m_processingDefaultText( e )
{
}

const QByteArray& engines::engine::functions::postProcessing::text( const QByteArray& e )
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

bool engines::engine::functions::timer::timerText( const QString& e )
{
	return e.startsWith( engines::engine::functions::timer::timerText() ) ;
}

QString engines::engine::functions::timer::timerText()
{
	return QObject::tr( "Elapsed Time:" ) + " " ;
}

QString engines::engine::functions::timer::startTimerText()
{
	return engines::engine::functions::timer::timerText() + "00:00:00" ;
}

QString engines::engine::functions::timer::stringElapsedTime( qint64 milliseconds )
{
	if( milliseconds <= 0 ){

		return engines::engine::functions::timer::startTimerText() ;
	}

	auto m = engines::engine::functions::timer::duration( milliseconds ) ;

	return engines::engine::functions::timer::timerText() + m ;
}

QString engines::engine::functions::timer::duration( qint64 milliseconds )
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

int engines::engine::functions::timer::toSeconds( const QString& e )
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

qint64 engines::engine::functions::timer::elapsedTime()
{
	return engines::engine::functions::timer::currentTime() - m_startTime ;
}

qint64 engines::engine::functions::timer::currentTime()
{
	return QDateTime().currentMSecsSinceEpoch() ;
}

QString engines::engine::functions::timer::stringElapsedTime()
{
	return engines::engine::functions::timer::stringElapsedTime( this->elapsedTime() ) ;
}

void engines::engine::functions::timer::reset()
{
	m_startTime = engines::engine::functions::timer::currentTime() ;
}

engines::configDefaultEngine::configDefaultEngine( Logger&logger,const enginePaths& enginePath ) :
	m_name( "yt-dlp" ),
	m_configFileName( m_name + ".json" )
{
	yt_dlp::init( this->name(),this->configFileName(),logger,enginePath ) ;

	if( utility::platformIsWindows() ){

		aria2c::init( "aria2c","aria2c.json",logger,enginePath ) ;
		wget::init( "wget","wget.json",logger,enginePath ) ;
	}
}

engines::engine::functions::filterOutPut::~filterOutPut()
{
}

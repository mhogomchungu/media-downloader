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

#include "engines/youtube-dl.h"
#include "engines/generic.h"
#include "engines/safaribooks.h"
#include "engines/gallery-dl.h"

#include "downloadmanager.h"
#include "utility.h"
#include "version.h"
#include "tableWidget.h"

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

		if( utility::platformIsWindows() ){

			return ";" ;
		}else{
			return ":" ;
		}
	}() ;

	auto s = basePath ;

	if( utility::platformIsWindows() ){

		auto mm = QDir::currentPath() ;

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

	if( settings.useSystemProvidedVersionIfAvailable() ){

		if( s.endsWith( separator ) ){

			env.insert( "PATH",p + s ) ;
		}else{
			env.insert( "PATH",p + separator + s ) ;
		}
	}else{
		if( s.endsWith( separator ) ){

			env.insert( "PATH",s + p ) ;
		}else{
			env.insert( "PATH",s + separator + p ) ;
		}
	}

	env.insert( "LANG","C" ) ;

	return env ;
}

engines::engines( Logger& l,settings& s ) :
	m_logger( l ),
	m_settings( s ),
	m_enginePaths( m_settings ),
	m_processEnvironment( _getEnvPaths( m_enginePaths,m_settings ) )
{
	if( settings::portableVersion() ){

		m_logger.add( QObject::tr( "Running in portable mode" ) ) ;
		m_logger.add( QObject::tr( "Download path: " ) + m_settings.downloadFolder( m_logger ) ) ;
	}

	youtube_dl::init( m_logger,m_enginePaths ) ;

	this->updateEngines( true ) ;
}

static void _openUrls( tableWidget& table,int row,settings& settings,bool galleryDl )
{
	if( downloadManager::finishedStatus::finishedWithSuccess( table,row ) ){

		auto m = utility::split( table.uiText( row ),'\n',true ) ;

		m.removeFirst() ;

		for( const auto& it : m ){

			if( galleryDl ){

				auto e = settings.downloadFolder() ;
				auto m = QUrl::fromLocalFile( e + "/gallery-dl/" + it ) ;

				QDesktopServices::openUrl( m ) ;
			}else{
				auto m = QUrl::fromLocalFile( settings.downloadFolder() + "/" + it ) ;

				QDesktopServices::openUrl( m ) ;
			}
		}
	}
}

void engines::openUrls( tableWidget& table,int row,const QString& engineName ) const
{
	if( engineName.isEmpty() ){

		_openUrls( table,row,m_settings,false ) ;
	}else{
		const auto& engine = this->getEngineByName( engineName ) ;

		if( engine && ( engine->likeYoutubeDl() || engine->name() == "gallery-dl" ) ) {

			_openUrls( table,row,m_settings,engine->name() == "gallery-dl" ) ;
		}
	}
}

void engines::openUrls( const QString& path ) const
{
	auto m = QUrl::fromLocalFile( path ) ;

	QDesktopServices::openUrl( m ) ;
}

void engines::updateEngines( bool addAll )
{
	auto _engine_add = [ & ]( engines::engine m ){

		if( m.valid() ){

			if( m.exePath().isEmpty() && !m.usingPrivateBackend() ){

				m_logger.add( QObject::tr( "Error, executable to backend \"%1\" could not be found" ).arg( m.name() ) ) ;
			}else{
				m_backends.emplace_back( std::move( m ) ) ;
			}
		}
	} ;

	_engine_add( this->getEngineByPath( "youtube-dl.json" ) ) ;

	for( const auto& it : this->enginesList() ){

		_engine_add( this->getEngineByPath( it ) ) ;
	}

	if( addAll ){

		_engine_add( { *this,m_logger,"ffmpeg","-version",0,2 } ) ;

		if( utility::platformIsWindows() ){

			//_engine_add( { *this,m_logger,"AtomicParsley","-v",0,2 } ) ;
		}

		for( const auto& it : this->getEngines() ){

			const auto& e = it.exePath().exe() ;

			if( e.size() > 0 && e.at( 0 ).contains( "python" ) ){

				_engine_add( { *this,m_logger,"python3","--version",0,1 } ) ;
				break ;
			}
		}
	}
}

const std::vector< engines::engine >& engines::getEngines() const
{
	return m_backends ;
}

const engines::engine& engines::defaultEngine( const QString& name ) const
{
	auto m =  this->getEngineByName( name ) ;

	if( m ){

		return m.value() ;
	}else{
		m_logger.add( "Error: engines::defaultEngine: Unknown Engine: " + name ) ;

		if( m_backends.size() > 0 ){

			return m_backends[ 0 ] ;
		}else{
			static engines::engine engine( m_logger ) ;

			return engine ;
		}
	}
}

engines::result_ref< const engines::engine& > engines::getEngineByName( const QString& name ) const
{
	for( const auto& it : m_backends ){

		if( it.name() == name ){

			return it ;
		}
	}

	return {} ;
}

class version{
public:
	version( int major,int minor,int patch ) :
		m_valid( true ),m_major( major ),m_minor( minor ),m_patch( patch )
	{
	}
	template< typename T >
	version( const T& e )
	{
		auto s = utility::split( e,'.',true ) ;

		int m = s.size() ;

		if( m == 1 ){

			m_major = s.at( 0 ).toInt( &m_valid ) ;

		}else if( m == 2 ){

			m_major = s.at( 0 ).toInt( &m_valid ) ;

			if( m_valid ){

				m_minor = s.at( 1 ).toInt( &m_valid ) ;
			}

		}else if( m >= 3 ) {

			m_major = s.at( 0 ).toInt( &m_valid ) ;

			if( m_valid ){

				m_minor = s.at( 1 ).toInt( &m_valid ) ;

				if( m_valid ){

					m_patch = s.at( 2 ).toInt( &m_valid ) ;
				}
			}
		}
	}
	bool valid() const
	{
		return m_valid ;
	}
	bool operator==( const version& other ) const
	{
		return m_major == other.m_major && m_minor == other.m_minor && m_patch == other.m_patch ;
	}
	bool operator<( const version& other ) const
	{
		if( m_major < other.m_major ){

			return true ;

		}else if( m_major == other.m_major ){

			if( m_minor < other.m_minor ){

				return true ;

			}else if( m_minor == other.m_minor ){

				return m_patch < other.m_patch ;
			}
		}

		return false ;
	}
	/*
	 * a != b equal to !(a == b)
	 * a <= b equal to (a < b) || (a == b)
	 * a >= b equal to !(a < b)
	 * a > b  equal to !(a <= b)
	 */
	bool operator>=( const version& other ) const
	{
		return !( *this < other ) ;
	}
	bool operator<=( const version& other ) const
	{
		return ( *this < other ) || ( *this == other ) ;
	}
	bool operator!=( const version& other ) const
	{
		return !( *this == other ) ;
	}
	bool operator>( const version& other ) const
	{
		return !( *this <= other ) ;
	}
private:
	bool m_valid = false ;
	int m_major = 0 ;
	int m_minor = 0 ;
	int m_patch = 0 ;
};

engines::engine engines::getEngineByPath( const QString& e ) const
{
	auto path = m_enginePaths.configPath( e ) ;

	engines::Json json( engines::file( path,m_logger ).readAll() ) ;

	if( json ){

		auto object = json.doc().object() ;

		auto minVersion = object.value( "RequiredMinimumVersionOfMediaDownloader" ).toString() ;

		if( !minVersion.isEmpty() ){

			if( version( minVersion ) > VERSION ){

				auto name = object.value( "Name" ).toString() ;

				auto m = QObject::tr( "Engine \"%1\" requires atleast version \"%2\" of Media Downloader" ) ;

				m_logger.add( m.arg( name,minVersion ) ) ;

				return { m_logger } ;
			}
		}

		auto name = object.value( "Name" ).toString() ;

		if( object.value( "LikeYoutubeDl" ).toBool( false ) || name == "youtube-dl" ){

			auto functions = std::make_unique< youtube_dl >( m_settings ) ;

			functions->updateOptions( object,m_settings ) ;

			return { m_logger,m_enginePaths,object,*this,std::move( functions ) } ;

		}else if( name == "safaribooks" ){

			auto functions = std::make_unique< safaribooks >( m_settings ) ;

			functions->updateOptions( object,m_settings ) ;

			return { m_logger,m_enginePaths,object,*this,std::move( functions ) } ;

		}else if( name == "gallery-dl" ){

			auto functions = std::make_unique< gallery_dl >( m_settings ) ;

			functions->updateOptions( object,m_settings ) ;

			return { m_logger,m_enginePaths,object,*this,std::move( functions ) } ;
		}else{
			auto functions = std::make_unique< generic >( m_settings ) ;

			functions->updateOptions( object,m_settings ) ;

			return { m_logger,m_enginePaths,object,*this,std::move( functions ) } ;
		}
	}else{
		return { m_logger } ;
	}
}

const engines::enginePaths& engines::engineDirPaths() const
{
	return m_enginePaths ;
}

static QStringList _toStringList( const QJsonValue& value ){

	QStringList m ;

	const auto array = value.toArray() ;

	for( const auto& it : array ){

		m.append( it.toString() ) ;
	}

	return m ;
}

QString engines::findExecutable( const QString& exeName ) const
{
	if( exeName == "python3" ){

		return utility::python3Path() ;
	}else{
		auto path = this->processEnvironment().value( "PATH" ) ;

		if( utility::platformIsWindows() ){

			return QStandardPaths::findExecutable( exeName,path.split( ";" ) ) ;
		}else{
			return QStandardPaths::findExecutable( exeName,path.split( ":" ) ) ;
		}
	}
}

const QProcessEnvironment& engines::processEnvironment() const
{
	return m_processEnvironment ;
}

void engines::addEngine( const QByteArray& data,const QString& path )
{
	engines::Json json( data ) ;

	if( json ){

		auto object = json.doc().object() ;

		auto name = object.value( "Name" ).toString() ;

		if( !name.isEmpty() ){

			auto e = m_enginePaths.configPath( path ) ;

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

				m_backends.clear() ;

				m_settings.setDefaultEngine( name,settings::tabName::basic ) ;
				m_settings.setDefaultEngine( name,settings::tabName::batch ) ;

				if( object.value( "CanDownloadPlaylist" ).toBool( false ) ){

					m_settings.setDefaultEngine( name,settings::tabName::playlist ) ;
				}

				this->updateEngines( false ) ;
			}
		}
	}
}

void engines::removeEngine( const QString& e )
{
	const auto engine = this->getEngineByPath( e ) ;

	if( engine.valid() ){

		QFile::remove( m_enginePaths.configPath( e ) ) ;

		const auto& exe = engine.exePath().realExe() ;

		if( engine.usingPrivateBackend() && QFile::exists( exe ) ){

			QFile::remove( exe ) ;
		}

		if( m_backends.size() > 0 ){

			const auto& name = engine.name() ;

			auto _reset_default = [ & ]( const QString& name,settings::tabName n ){

				if( name == m_settings.defaultEngine( n ) ){

					m_settings.setDefaultEngine( m_backends[ 0 ].name(),n ) ;
				}
			} ;

			_reset_default( name,settings::tabName::basic ) ;
			_reset_default( name,settings::tabName::batch ) ;
			_reset_default( name,settings::tabName::playlist ) ;
		}

		m_backends.clear() ;

		this->updateEngines( false ) ;
	}
}

QStringList engines::enginesList() const
{
	auto m = QDir( m_enginePaths.configPath() ).entryList( QDir::Filter::Files ) ;

	m.removeAll( "youtube-dl.json" ) ;

	return m ;
}

engines::engine::engine( const engines& engines,
			 Logger& logger,
			 const QString& name,
			 const QString& versionArgument,
			 int line,
			 int position ) :
	m_line( line ),
	m_position( position ),
	m_valid( true ),
	m_mainEngine( false ),
	m_name( name ),
	m_commandName( name ),
	m_commandNameWindows( m_commandName + ".exe" ),
	m_versionArgument( versionArgument ),
	m_showListBreaker( logger )
{
	auto m = engines.findExecutable( m_commandName ) ;

	if( m.isEmpty() ){

		m_valid = false ;
		logger.add( QObject::tr( "Failed to find executable \"%1\"" ).arg( m_commandName ) ) ;
	}else{
		m_exePath = m ;
	}
}

engines::engine::engine( Logger& logger,
			 const enginePaths& ePaths,
			 const engines::Json& json,
			 const engines& engines,
			 std::unique_ptr< engine::functions > functions ) :
	m_jsonObject( json.doc().object() ),
	m_functions( std::move( functions ) ),
	m_line( m_jsonObject.value( "VersionStringLine" ).toInt() ),
	m_position( m_jsonObject.value( "VersionStringPosition" ).toInt() ),
	m_valid( true ),
	m_usingPrivateBackend( m_jsonObject.value( "UsePrivateExecutable" ).toBool() ),
	m_canDownloadPlaylist( m_jsonObject.value( "CanDownloadPlaylist" ).toBool() ),
	m_likeYoutubeDl( m_jsonObject.value( "LikeYoutubeDl" ).toBool( false ) ),
	m_mainEngine( true ),
	m_replaceOutputWithProgressReport( m_jsonObject.value( "ReplaceOutputWithProgressReport" ).toBool( false ) ),
	m_name( m_jsonObject.value( "Name" ).toString() ),
	m_commandName( m_jsonObject.value( "CommandName" ).toString() ),
	m_commandNameWindows( m_jsonObject.value( "CommandNameWindows" ).toString() ),
	m_userName( m_jsonObject.value( "UserName" ).toString() ),
	m_password( m_jsonObject.value( "Password" ).toString() ),
	m_exeFolderPath( m_jsonObject.value( "BackendPath" ).toString() ),
	m_versionArgument( m_jsonObject.value( "VersionArgument" ).toString() ),
	m_optionsArgument( m_jsonObject.value( "OptionsArgument" ).toString() ),
	m_downloadUrl( m_jsonObject.value( "DownloadUrl" ).toString() ),
	m_playListUrlPrefix( m_jsonObject.value( "PlayListUrlPrefix" ).toString() ),
	m_playlistItemsArgument( m_jsonObject.value( "PlaylistItemsArgument" ).toString() ),
	m_batchFileArgument( m_jsonObject.value( "BatchFileArgument" ).toString() ),
	m_cookieArgument( m_jsonObject.value( "CookieArgument" ).toString() ),
	m_playListIdArguments( _toStringList( m_jsonObject.value( "PlayListIdArguments" ) ) ),
	m_splitLinesBy( _toStringList( m_jsonObject.value( "SplitLinesBy" ) ) ),
	m_removeText( _toStringList( m_jsonObject.value( "RemoveText" ) ) ),
	m_skiptLineWithText( _toStringList( m_jsonObject.value( "SkipLineWithText" ) ) ),
	m_defaultDownLoadCmdOptions( _toStringList( m_jsonObject.value( "DefaultDownLoadCmdOptions" ) ) ),
	m_defaultListCmdOptions( _toStringList( m_jsonObject.value( "DefaultListCmdOptions" ) ) ),
	m_controlStructure( m_jsonObject.value( "ControlJsonStructure" ).toObject() ),
	m_showListBreaker( logger,m_jsonObject.value( "ShowListTableBoundary" ).toObject() )
{
	if( utility::platformIs32BitWindows() ){

		auto m = m_jsonObject.value( "CommandName32BitWindows" ).toString() ;

		if( !m.isEmpty() ){

			m_commandNameWindows = m ;
		}
	}

	auto cmdNames = [ & ](){

		if( utility::platformIsWindows() ){

			if( utility::platformIs32BitWindows() ){

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

	if( utility::platformIsWindows() && m_commandNameWindows.isEmpty() ){

		if( m_commandName.endsWith( ".exe" ) ){

			m_commandNameWindows = m_commandName ;
		}else{
			m_commandNameWindows = m_commandName + ".exe" ;
		}
	}

	if( m_exeFolderPath == "${default}" || m_exeFolderPath == "${BackendPath}" ){

		m_exeFolderPath = ePaths.binPath() ;
	}

	const auto& commandName = this->commandName() ;

	if( cmdNames.isEmpty() ){

		if( this->usingPrivateBackend() && !m_exeFolderPath.isEmpty() ){

			m_exePath = m_exeFolderPath + "/" + commandName ;
		}else{
			auto m = engines.findExecutable( commandName ) ;

			if( m.isEmpty() ){

				if( !this->downloadUrl().isEmpty() && !m_exeFolderPath.isEmpty() ){

					m_usingPrivateBackend = true ;
					m_exePath = m_exeFolderPath + "/" + commandName ;
				}else{
					m_valid = false ;
					logger.add( utility::failedToFindExecutableString( commandName ) ) ;
				}
			}else{
				m_exePath = m ;
			}
		}
	}else{
		auto cmd = cmdNames.takeAt( 0 ) ;

		for( auto& it : cmdNames ){

			it.replace( "${BackendPath}",ePaths.binPath() ) ;
			it.replace( "${CommandName}",commandName ) ;
		}

		auto subCmd = [ & ]()->QString{

			for( const auto& it : cmdNames ){

				if( it.endsWith( commandName ) ){

					return it ;
				}
			}

			return cmd ;
		}() ;

		if( cmd == "python3" ){

			auto m = engines.findExecutable( "python3" ) ;

			if( m.isEmpty() ){

				m_valid = false ;
				logger.add( QObject::tr( "Failed to find python3 executable for backend \"%1\"" ).arg( m_name ) ) ;
			}else{
				if( utility::platformIsWindows() ){
					/*
					 * 1. Python's getpass() cant seem to read data from QProcess on Windows.
					 *
					 * 2. Investigate using winpty as a work around.
					 *
					 * 3. In the mean time, credentials on windows are sent as CLI arguments.
					 */

					//auto cwd = QDir().currentPath() ;
					//auto w = cwd + "\\winpty-0.4.3-cygwin-2.8.0-x64\\bin\\winpty.exe" ;
					//m_exePath = { { w,"-Xallow-non-tty","-Xplain",m },ee,cmdNames } ;

					m_exePath = { m,subCmd,cmdNames } ;
				}else{
					m_exePath = { m,subCmd,cmdNames } ;
				}
			}
		}else{
			auto m = engines.findExecutable( cmd ) ;

			if( m.isEmpty() ){

				m_valid = false ;
				logger.add( QObject::tr( "Failed to find executable \"%1\"" ).arg( cmd ) ) ;
			}else{
				m_exePath = { m,subCmd,cmdNames } ;
			}
		}
	}
}

const QString& engines::engine::commandName() const
{
	if( utility::platformIsWindows() ){

		return m_commandNameWindows ;
	}else{
		return m_commandName ;
	}
}

bool engines::engine::breakShowListIfContains( const QStringList& e ) const
{
	if( m_showListBreaker ){

		return m_showListBreaker.breakerFound( e ) ;
	}else{
		return e.at( 0 ) == "format" || e.at( 2 ).contains( "-" ) ;
	}
}

QString engines::engine::versionString( const QString& data ) const
{
	auto a = utility::split( data,'\n',true ) ;

	if( m_line < a.size() ){

		auto b = a[ m_line ] ;
		auto c = utility::split( b,' ',true ) ;

		if( m_position < c.size() ){

			return c[ m_position ] ;
		}
	}

	return {} ;
}

engines::enginePaths::enginePaths( settings& s )
{
	auto m = s.configPaths() ;

	if( !m.isEmpty() ){

		m_basePath = m.first() ;
	}else{
		//?????
		m_basePath = QDir::homePath() + "/.config/media-downloader/" ;
	}

	m_binPath = m_basePath + "/bin" ;
	m_configPath = m_basePath + "/engines.v1" ;

	QDir().mkpath( m_basePath ) ;
	QDir().mkpath( m_binPath ) ;
	QDir().mkpath( m_configPath ) ;
}

QString engines::engine::functions::processCompleteStateText( const engine::engine::functions::finishedState& f )
{
	if( f.cancelled() ){

		return QObject::tr( "Download cancelled" ) ;

	}else if( f.success() ){

		return QObject::tr( "Download completed" ) ;
	}else{
		return QObject::tr( "Download Failed" ) ;
	}
}

engines::engine::functions::~functions()
{
}

std::unique_ptr< engines::engine::functions::filter > engines::engine::functions::Filter( const QString& e )
{
	return std::make_unique< engines::engine::functions::filter >( e ) ;
}

void engines::engine::functions::runCommandOnDownloadedFile( const QString& e,const QString& s )
{
	auto a = m_settings.commandOnSuccessfulDownload() ;

	if( !a.isEmpty() && !e.isEmpty() ){

		auto args = utility::split( a,' ',true ) ;
		auto exe = args.takeAt( 0 ) ;
		args.append( "bla bla bla" ) ;

		bool success = false ;

		for( const auto& it : utility::split( e,'\n',true ) ){

			auto b = m_settings.downloadFolder() + it ;

			if( QFile::exists( b ) ){

				success = true ;
				args.replace( args.size() - 1,b ) ;

				QProcess::startDetached( exe,args ) ;
			}
		}

		if( !success && !s.isEmpty() ){

			auto b = m_settings.downloadFolder() + "/" + utility::split( s,'/',true ).last() ;

			if( QFile::exists( b ) ){

				args.replace( args.size() - 1,b ) ;

				QProcess::startDetached( exe,args ) ;
			}
		}
	}
}

void engines::engine::functions::updateOptions( QJsonObject&,settings& )
{
}

QString engines::engine::functions::commandString( const engines::engine::exeArgs::cmd& cmd )
{
	auto m = "\"" + cmd.exe() + "\"" ;

	for( const auto& it : cmd.args() ){

		m += " \"" + it + "\"" ;
	}

	return m ;
}

QString engines::engine::functions::updateTextOnCompleteDownlod( const QString& uiText,
								 const engines::engine::functions::finishedState& f )
{
	auto m = engines::engine::functions::processCompleteStateText( f ) ;
	auto e = engines::engine::functions::timer::stringElapsedTime( f.duration() ) ;

	return m + ", " + e + "\n" + uiText ;
}

QString engines::engine::functions::updateTextOnCompleteDownlod( const engines::engine&,
								 const QString& uiText,
								 const QString& bkText,
								 const engine::engine::functions::finishedState& f )
{
	Q_UNUSED( uiText )

	if( f.success() ){

		return engines::engine::functions::updateTextOnCompleteDownlod( bkText,f ) ;
	}else{
		return engines::engine::functions::updateTextOnCompleteDownlod( bkText,f ) ;
	}
}

void engines::engine::functions::sendCredentials( const engines::engine&,
						  const QString&,
						  QProcess& )
{
}

static bool _meet_condition( const QString& line,const QJsonObject& obj )
{
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

static bool _meet_condition( const engines::engine& engine,const QString& line )
{
	const auto& obj = engine.controlStructure() ;

	auto connector = obj.value( "Connector" ).toString() ;

	if( connector.isEmpty() ){

		auto oo = obj.value( "lhs" ) ;

		if( oo.isObject() ){

			return _meet_condition( line,oo.toObject() ) ;
		}else{
			return false ;
		}
	}else{
		auto obj1 = obj.value( "lhs" ) ;
		auto obj2 = obj.value( "rhs" ) ;

		if( obj1.isObject() && obj2.isObject() ){

			auto a = _meet_condition( line,obj1.toObject() ) ;
			auto b = _meet_condition( line,obj2.toObject() ) ;

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

static bool _skip_line( const QByteArray& line,const engines::engine& engine )
{
	if( line.isEmpty() ){

		return true ;
	}else{
		for( const auto& it : engine.skiptLineWithText() ){

			if( line.contains( it.toUtf8() ) ){

				return true ;
			}
		}

		return false ;
	}
}

static void _add( const QByteArray& data,
		  QChar token,
		  const engines::engine& engine,
		  Logger::Data& outPut,
		  int id )
{
	for( const auto& e : utility::split( data,token ) ){

		if( _skip_line( e,engine ) ){

			continue ;

		}else if( _meet_condition( engine,e ) ){

			if( id == -1 ){

				if( outPut.isEmpty() ){

					outPut.add( e ) ;
				}else{
					auto& s = outPut.lastText() ;

					if( _meet_condition( engine,s ) ){

						outPut.replaceLast( e ) ;
					}else{
						outPut.add( e ) ;
					}
				}
			}else{
				auto s = static_cast< bool( * )( const engines::engine&,const QString& ) >( _meet_condition ) ;

				outPut.replaceOrAdd( engine,e,id,s,[ &engine ]( const QString& e ){

					if( engine.likeYoutubeDl() ){

						return e.startsWith( "[download] 100.0%" ) ;
					}else{
						return false ;
					}
				} ) ;
			}
		}else{
			outPut.add( e,id ) ;
		}
	}
}

void engines::engine::functions::processData( const engines::engine& engine,
					      Logger::Data& outPut,
					      const QString& e,
					      int id )
{
	outPut.replaceOrAdd( engine,e,id,[]( const engines::engine&,const QString& line ){

		auto a = line.startsWith( engines::engine::functions::preProcessing::processingText() ) ;
		auto b = engines::engine::functions::timer::timerText( line ) ;

		return a || b ;

	},[]( const QString& ){

		return false ;
	} ) ;
}

void engines::engine::functions::processData( const engines::engine& engine,
					      Logger::Data& outPut,
					      QByteArray data,
					      int id )
{
	for( const auto& it : engine.removeText() ){

		data.replace( it.toUtf8(),"" ) ;
	}

	const auto& sp = engine.splitLinesBy() ;

	if( sp.size() == 1 && sp[ 0 ].size() > 0 ){

		_add( data,sp[ 0 ][ 0 ],engine,outPut,id ) ;

	}else if( sp.size() == 2 && sp[ 0 ].size() > 0 && sp[ 1 ].size() > 0 ){

		for( const auto& m : utility::split( data,sp[ 0 ][ 0 ] ) ){

			_add( m,sp[ 1 ][ 0 ],engine,outPut,id ) ;
		}
	}else{
		for( const auto& m : utility::split( data,'\r' ) ){

			_add( m,'\n',engine,outPut,id ) ;
		}
	}
}

void engines::engine::functions::updateDownLoadCmdOptions( const engines::engine& engine,
							   const QString & quality,
							   const QStringList& userOptions,
							   QStringList& urls,
							   QStringList& ourOptions )
{
	Q_UNUSED( userOptions )
	Q_UNUSED( urls )

	if( !engine.optionsArgument().isEmpty() ){

		ourOptions.append( engine.optionsArgument() ) ;
	}

	if( !quality.isEmpty() ){

		ourOptions.append( quality ) ;
	}
}

engines::engine::functions::functions( settings& s ) :
	m_settings( s )
{
}

settings& engines::engine::functions::Settings()
{
	return m_settings ;
}

void engines::file::write( const QString& e )
{
	if( m_file.open( QIODevice::WriteOnly ) ){

		m_file.write( e.toUtf8() ) ;
	}else{
		m_logger.add( QObject::tr( "Failed to open file for writing" ) + ": " + m_filePath ) ;
	}
}

void engines::file::write( const QJsonDocument& doc,QJsonDocument::JsonFormat format )
{
	if( m_file.open( QIODevice::WriteOnly ) ){

		m_file.write( doc.toJson( format ) ) ;
	}else{
		m_logger.add( QObject::tr( "Failed to open file for writing" ) + ": " + m_filePath ) ;
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
		m_logger.add( QObject::tr( "Failed to open file for reading" ) + ": " + m_filePath ) ;

		return QByteArray() ;
	}
}

engines::engine::functions::filter::filter( const QString& e ) :
	m_quality( e )
{
}

const QString& engines::engine::functions::filter::operator()( const engines::engine& engine,
							       const Logger::Data& s )
{
	if( engine.replaceOutputWithProgressReport() ){

		return m_processing.text() ;

	}else if( s.isEmpty() ){

		static QString e ;
		return e ;
	}else{
		const auto& m = s.lastText() ;

		if( m.startsWith( "[media-downloader] cmd:" ) ){

			return m_processing.text() ;
		}else{
			return m ;
		}
	}
}

engines::engine::functions::filter::~filter()
{
}

const QString& engines::engine::functions::filter::quality()
{
	return m_quality ;
}

engines::engine::functions::preProcessing::preProcessing() :
	m_processingDefaultText( engines::engine::functions::preProcessing::processingText() )
{
}

engines::engine::functions::preProcessing::preProcessing( const QString& e ) :
	m_processingDefaultText( e )
{
}

QString engines::engine::functions::preProcessing::processingText()
{
	return QObject::tr( "Processing" ) ;
}

const QString& engines::engine::functions::preProcessing::text()
{
	if( m_counter < 16 ){

		m_counterDots += " ..." ;
	}else{
		m_counterDots = " ..." ;
		m_counter = 0 ;
	}

	m_txt = m_processingDefaultText + m_counterDots ;

	m_counter++ ;

	return m_txt ;
}

QString engines::engine::functions::postProcessing::processingText()
{
	return QObject::tr( "Post Processing" ) ;
}

engines::engine::functions::postProcessing::postProcessing() :
	m_processingDefaultText( engines::engine::functions::postProcessing::processingText() )
{
}

engines::engine::functions::postProcessing::postProcessing( const QString& e ) :
	m_processingDefaultText( e )
{

}

const QString& engines::engine::functions::postProcessing::text( const QString& e )
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

engines::engine::showListBreaker::showListBreaker( Logger& )
{
}

engines::engine::showListBreaker::showListBreaker( Logger&,QJsonObject o ) :
	m_obj( std::move( o ) )
{
	if( !m_obj.isEmpty() ){

		auto a = m_obj.value( "ColumnNumber" ).toString() ;
		m_cmp  = m_obj.value( "Comparator" ).toString() ;
		m_text = m_obj.value( "String" ).toString() ;

		if( !a.isEmpty() && !m_cmp.isEmpty() && !m_text.isEmpty() ){

			m_number = a.toInt( &m_valid ) ;
		}
	}
}

bool engines::engine::showListBreaker::breakerFound( const QStringList& e ) const
{
	if( m_number < e.size() ){

		if( m_cmp == "equals" ){

			return m_text == e[ m_number ] ;

		}else if( m_cmp == "contains" ){

			return e[ m_number ].contains( m_text ) ;
		}
	}

	return false ;
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
	return QObject::tr( "Elapsed Time:" ) + " 00:00:00" ;
}

QString engines::engine::functions::timer::stringElapsedTime( int milliseconds )
{
	if( milliseconds <= 0 ){

		return QObject::tr( "Elapsed Time:" ) + " " + QString( "00:00:00" ) ;
	}

	auto m = engines::engine::functions::timer::duration( milliseconds ) ;

	return QObject::tr( "Elapsed Time:" ) + " " + m ;
}

QString engines::engine::functions::timer::duration( int milliseconds )
{
	int seconds      = milliseconds / 1000;
	milliseconds     = milliseconds % 1000;
	int minutes      = seconds / 60 ;
	seconds          = seconds % 60 ;
	int hours        = minutes / 60 ;
	minutes          = minutes % 60 ;

	QTime time ;
	time.setHMS( hours,minutes,seconds,milliseconds ) ;

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

	auto m = utility::split( e,':',true ) ;

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

int engines::engine::functions::timer::elapsedTime()
{
	return static_cast< int >( QDateTime().currentMSecsSinceEpoch() - m_startTime ) ;
}

QString engines::engine::functions::timer::stringElapsedTime()
{
	return engines::engine::functions::timer::stringElapsedTime( this->elapsedTime() ) ;
}

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

#include "utility.h"

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

#include <QDir>

static QProcessEnvironment _getEnvPaths( const engines::enginePaths& paths )
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

	return env ;
}

engines::engines( Logger& l,settings& s ) :
	m_logger( l ),
	m_settings( s ),
	m_enginePaths( m_settings ),
	m_processEnvironment( _getEnvPaths( m_enginePaths ) )
{
	youtube_dl::init( m_logger,m_enginePaths ) ;

	this->updateEngines( true ) ;
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

const engines::engine& engines::defaultEngine() const
{
	auto m =  this->getEngineByName( m_settings.defaultEngine() ) ;

	if( m ){

		return m.value() ;
	}else{
		m_logger.add( "Error: engines::defaultEngine: Unknown Engine: " + m_settings.defaultEngine() ) ;

		if( m_backends.size() > 0 ){

			return m_backends[ 0 ] ;
		}else{
			static engines::engine engine ;

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

engines::engine engines::getEngineByPath( const QString& e ) const
{
	auto path = m_enginePaths.configPath( e ) ;

	engines::Json json( engines::file( path,m_logger ).readAll() ) ;

	if( json ){

		auto object = json.doc().object() ;

		if( object.value( "LikeYoutubeDl" ).toBool( false ) ||
				object.value( "Name" ).toString() == "youtube-dl" ){

			auto functions = std::make_unique< youtube_dl >() ;

			functions->updateOptions( object ) ;

			return { m_logger,m_enginePaths,object,*this,std::move( functions ) } ;

		}else if( object.value( "Name" ).toString() == "safaribooks" ){

			auto functions = std::make_unique< safaribooks >( m_settings ) ;

			functions->updateOptions( object ) ;

			return { m_logger,m_enginePaths,object,*this,std::move( functions ) } ;
		}else{
			auto functions = std::make_unique< generic >() ;

			functions->updateOptions( object ) ;

			return { m_logger,m_enginePaths,object,*this,std::move( functions ) } ;
		}
	}else{
		return {} ;
	}
}

void engines::setDefaultEngine( const QString& name )
{
	m_settings.setDefaultEngine( name ) ;
}

void engines::setDefaultEngine( const engines::engine& engine )
{
	this->setDefaultEngine( engine.name() ) ;
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

		auto name = json.doc().object().value( "Name" ).toString() ;

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

				this->setDefaultEngine( name ) ;

				this->updateEngines( true ) ;
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

			this->setDefaultEngine( m_backends[ 0 ] ) ;
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
	m_versionArgument( versionArgument )
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
	m_name( m_jsonObject.value( "Name" ).toString() ),
	m_commandName( m_jsonObject.value( "CommandName" ).toString() ),
	m_commandNameWindows( m_jsonObject.value( "CommandNameWindows" ).toString() ),
	m_userName( m_jsonObject.value( "UserName" ).toString() ),
	m_password( m_jsonObject.value( "Password" ).toString() ),
	m_exeFolderPath( m_jsonObject.value( "BackendPath" ).toString() ),
	m_versionArgument( m_jsonObject.value( "VersionArgument" ).toString() ),
	m_optionsArgument( m_jsonObject.value( "OptionsArgument" ).toString() ),
	m_downloadUrl( m_jsonObject.value( "DownloadUrl" ).toString() ),
	m_batchFileArgument( m_jsonObject.value( "BatchFileArgument" ).toString() ),
	m_splitLinesBy( _toStringList( m_jsonObject.value( "SplitLinesBy" ) ) ),
	m_removeText( _toStringList( m_jsonObject.value( "RemoveText" ) ) ),
	m_skiptLineWithText( _toStringList( m_jsonObject.value( "SkipLineWithText" ) ) ),
	m_defaultDownLoadCmdOptions( _toStringList( m_jsonObject.value( "DefaultDownLoadCmdOptions" ) ) ),
	m_defaultListCmdOptions( _toStringList( m_jsonObject.value( "DefaultListCmdOptions" ) ) ),
	m_controlStructure( m_jsonObject.value( "ControlJsonStructure" ).toObject() )
{
	auto cmdNames = [ & ](){

		if( utility::platformIsWindows() ){

			return _toStringList( m_jsonObject.value( "CommandNamesWindows" ) ) ;
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

				m_valid = false ;
				logger.add( QObject::tr( "Failed to find executable \"%1\"" ).arg( commandName ) ) ;
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

engines::engine::functions::~functions()
{
}

std::unique_ptr< engines::engine::functions::filter > engines::engine::functions::Filter()
{
	return std::make_unique< engines::engine::functions::filter >() ;
}

void engines::engine::functions::updateOptions( QJsonObject& )
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
		  QStringList& outPut )
{
	for( const auto& e : utility::split( data,token ) ){

		if( _skip_line( e,engine ) ){

			continue ;

		}else if( _meet_condition( engine,e ) ){

			auto& s = outPut.last() ;

			if( _meet_condition( engine,s ) ){

				s = e ;
			}else{
				outPut.append( e ) ;
			}
		}else{
			outPut.append( e ) ;
		}
	}
}

void engines::engine::functions::processData( const engines::engine& engine,
					      QStringList& outPut,
					      QByteArray data )
{
	for( const auto& it : engine.removeText() ){

		data.replace( it.toUtf8(),"" ) ;
	}

	const auto& sp = engine.splitLinesBy() ;

	if( sp.size() == 1 && sp[ 0 ].size() > 0 ){

		_add( data,sp[ 0 ][ 0 ],engine,outPut ) ;

	}else if( sp.size() == 2 && sp[ 0 ].size() > 0 && sp[ 1 ].size() > 0 ){

		for( const auto& m : utility::split( data,sp[ 0 ][ 0 ] ) ){

			_add( m,sp[ 1 ][ 0 ],engine,outPut ) ;
		}
	}else{
		for( const auto& m : utility::split( data,'\r' ) ){

			_add( m,'\n',engine,outPut ) ;
		}
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

const QString& engines::engine::functions::filter::operator()( const QString& e )
{
	return e ;
}

engines::engine::functions::filter::~filter()
{
}

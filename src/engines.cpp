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
#include "engines/wget.h"
#include "engines/safaribooks.h"

#include "utility.h"

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

#include <QDir>

template< typename Engine >
static engines::engine _add_engine( Logger& logger,
				    const engines::enginePaths& enginePath,
				    const Engine& engine )
{
	auto json = engine.config( logger,enginePath ) ;

	if( json ){

		return engines::engine( logger,enginePath,json.doc(),engine.Functions() ) ;
	}else{
		logger.add( QObject::tr( "Failed to parse json file" ) + " :" + json.errorString() ) ;

		return {} ;
	}
}

engines::engines( Logger& l,settings& s ) :
	m_logger( l ),
	m_settings( s ),
	m_enginePaths( m_settings )
{
	this->updateEngines() ;
}

void engines::updateEngines()
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

	_engine_add( _add_engine( m_logger,m_enginePaths,youtube_dl() ) ) ;

	if( utility::platformIsLinux() ){

		_engine_add( _add_engine( m_logger,m_enginePaths,wget() ) ) ;
	}

	for( const auto& it : this->enginesList() ){

		auto path = m_enginePaths.configPath() + "/" + it ;

		engines::Json json( engines::file( path,m_logger ).readAll() ) ;

		if( json ){

			auto object = json.doc().object() ;

			if( object.value( "LikeYoutubeDl" ).toBool( false ) ){

				_engine_add( engines::engine( m_logger,m_enginePaths,json,youtube_dl().Functions() ) ) ;

			}else if( object.value( "Name" ).toString() == "safaribooks" ){

				_engine_add( engines::engine( m_logger,m_enginePaths,json,safaribooks( m_settings ).Functions() ) ) ;
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

utility::result_ref< const engines::engine& > engines::getEngineByName( const QString& name ) const
{
	for( const auto& it : m_backends ){

		if( it.name() == name ){

			return it ;
		}
	}

	return {} ;
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

void engines::addEngine( const QByteArray& data,const QString& path )
{
	engines::Json json( data ) ;

	if( json ){

		auto name = json.doc().object().value( "Name" ).toString() ;

		if( !name.isEmpty() ){

			auto e = m_enginePaths.configPath() + "/" + path ;

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

				this->updateEngines() ;
			}
		}
	}
}

void engines::removeEngine( const QString& e )
{
	if( QFile::remove( m_enginePaths.configPath() + "/" + e ) ){

		m_backends.clear() ;

		this->setDefaultEngine( m_backends[ 0 ] ) ;

		this->updateEngines() ;
	}
}

QStringList engines::enginesList() const
{
	auto m = QDir( m_enginePaths.configPath() ).entryList( QDir::Filter::Files ) ;

	m.removeAll( "youtube-dl.json" ) ;
	m.removeAll( "wget.json" ) ;

	return m ;
}

engines::engine::engine( Logger& logger,
			 const enginePaths& ePaths,
			 const engines::Json& json,
			 std::unique_ptr< engine::functions > functions ) :
	m_jsonObject( json.doc().object() ),
	m_functions( std::move( functions ) ),
	m_line( m_jsonObject.value( "VersionStringLine" ).toInt() ),
	m_position( m_jsonObject.value( "VersionStringPosition" ).toInt() ),
	m_valid( true ),
	m_usingPrivateBackend( m_jsonObject.value( "UsePrivateExecutable" ).toBool() ),
	m_canDownloadPlaylist( m_jsonObject.value( "CanDownloadPlaylist" ).toBool() ),
	m_likeYoutubeDl( m_jsonObject.value( "LikeYoutubeDl" ).toBool( false ) ),
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
	m_defaultDownLoadCmdOptions( _toStringList( m_jsonObject.value( "DefaultDownLoadCmdOptions" ) ) ),
	m_defaultListCmdOptions( _toStringList( m_jsonObject.value( "DefaultListCmdOptions" ) ) )
{
	auto cmdNames = _toStringList( m_jsonObject.value( "CommandNames" ) ) ;

	if( utility::platformIsWindows() && m_commandNameWindows.isEmpty() ){

		if( m_commandName.endsWith( ".exe" ) ){

			m_commandNameWindows = m_commandName ;
		}else{
			m_commandNameWindows = m_commandName + ".exe" ;
		}
	}

	const auto& commandName = this->commandName() ;

	if( cmdNames.isEmpty() ){

		if( this->usingPrivateBackend() && !m_exeFolderPath.isEmpty() ){

			if( m_exeFolderPath == "${default}" || m_exeFolderPath == "${BackendPath}" ){

				m_exeFolderPath = ePaths.binPath() ;
			}

			m_exePath = m_exeFolderPath + "/" + commandName ;
		}else{
			auto m = QStandardPaths::findExecutable( commandName ) ;

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

			auto m = utility::python3Path() ;

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
			auto m = QStandardPaths::findExecutable( cmd ) ;

			if( m.isEmpty() ){

				m_valid = false ;
				logger.add( QObject::tr( "Failed to find executable \"%1\"" ).arg( cmd ) ) ;
			}else{
				m_exePath = { m,subCmd,cmdNames } ;
			}
		}
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

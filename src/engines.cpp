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

		return engines::engine( json.doc(),engine.Functions() ) ;
	}else{
		logger.add( QObject::tr( "Failed to parse json file" ) + " :" + json.errorString() ) ;

		return {} ;
	}
}

engines::engines( Logger& l ) : m_logger( l )
{
	enginePaths enginePaths ;

	auto _engine_add = [ & ]( engines::engine m ){

		if( m.valid() ){

			if( m.exePath().isEmpty() && !m.usingPrivateBackend() ){

				m_logger.add( QObject::tr( "Error, executable to backend \"%1\" could not be found" ).arg( m.name() ) ) ;
			}else{
				m_backends.emplace_back( std::move( m ) ) ;
			}
		}
	} ;

	_engine_add( _add_engine( m_logger,enginePaths,youtube_dl() ) ) ;

	if( utility::platformIsLinux() ){

		_engine_add( _add_engine( m_logger,enginePaths,wget() ) ) ;
	}

	if( m_backends.size() > 0 ){

		this->setDefaultEngine( m_backends[ 0 ].name() ) ;
	}
}

const std::vector< engines::engine >& engines::getEngines() const
{
	return m_backends ;
}

const engines::engine& engines::defaultEngine() const
{
	auto m =  this->getEngineByName( m_defaultEngine ) ;

	if( m ){

		return m.value() ;
	}else{
		m_logger.add( "Error: engines::defaultEngine: Unknown Engine:" + m_defaultEngine ) ;

		return m_backends[ 0 ] ;
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
	m_defaultEngine = name ;
}

void engines::setDefaultEngine( const engines::engine& engine )
{
	m_defaultEngine = engine.name() ;
}

static QStringList _toStringList( const QJsonValue& value ){

	QStringList m ;

	const auto array = value.toArray() ;

	for( const auto& it : array ){

		m.append( it.toString() ) ;
	}

	return m ;
}

engines::engine::engine( const engines::Json& json,std::unique_ptr< engine::functions > functions ) :
	m_jsonObject( json.doc().object() ),
	m_functions( std::move( functions ) ),
	m_line( m_jsonObject.value( "VersionStringLine" ).toInt() ),
	m_position( m_jsonObject.value( "VersionStringPosition" ).toInt() ),
	m_valid( true ),
	m_usingPrivateBackend( m_jsonObject.value( "UsePrivateExecutable" ).toBool() ),
	m_canDownloadPlaylist( m_jsonObject.value( "CanDownloadPlaylist" ).toBool() ),
	m_name( m_jsonObject.value( "Name" ).toString() ),
	m_commandName( m_jsonObject.value( "CommandName" ).toString() ),
	m_exeFolderPath( m_jsonObject.value( "BackendPath" ).toString() ),
	m_versionArgument( m_jsonObject.value( "VersionArgument" ).toString() ),
	m_optionsArgument( m_jsonObject.value( "OptionsArgument" ).toString() ),
	m_downloadUrl( m_jsonObject.value( "DownloadUrl" ).toString() ),
	m_batchFileArgument( m_jsonObject.value( "BatchFileArgument" ).toString() ),
	m_defaultDownLoadCmdOptions( _toStringList( m_jsonObject.value( "DefaultDownLoadCmdOptions" ) ) ),
	m_defaultListCmdOptions( _toStringList( m_jsonObject.value( "DefaultListCmdOptions" ) ) )
{
	if( this->usingPrivateBackend() ){

		m_exePath = m_exeFolderPath + "/" + m_commandName ;
	}else{
		m_exePath = QStandardPaths::findExecutable( m_commandName ) ;
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

engines::enginePaths::enginePaths()
{
	auto m = QStandardPaths::standardLocations( QStandardPaths::AppDataLocation ) ;

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

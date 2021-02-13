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

#include "utility.h"

#include "engines/youtube-dl.h"
#include "engines/wget.h"

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

#include <QDir>

#include <QDebug>

template< typename Engine >
static engines::engine _add_engine( engines::log& log,
				    const engines::enginePaths& enginePath,
				    const Engine& engine )
{
	QJsonParseError error ;

	auto json = QJsonDocument::fromJson( engine.config( log,enginePath ),&error ) ;

	if( error.error != QJsonParseError::NoError ){

		log.add( "Failed to parse json file at: " + error.errorString() ) ;

		return {} ;
	}else{
		return engines::engine( json,engine.functions() ) ;
	}
}

engines::engines( QPlainTextEdit& textEdit ) : m_log( textEdit )
{
	enginePaths enginePaths ;

	auto m = _add_engine( m_log,enginePaths,youtube_dl() ) ;

	if( m.valid() ){

		m_backends.emplace_back( std::move( m ) ) ;
	}

	m = _add_engine( m_log,enginePaths,wget() ) ;

	if( m.valid() && !m.exePath().isEmpty() ){

		m_backends.emplace_back( std::move( m ) ) ;
	}

	if( m_backends.size() == 0 ){

		m_log.add( "Warning: Using internal config options for youtube-dl backend" ) ;

		QByteArray m ;

		if( utility::platformIsLinux() ){

			m = "{\"BackendPath\":\"/home/ink/.local/share/media-downloader/bin\",\"CommandName\":\"youtube-dl\",\"DefaultDownLoadCmdOptions\":[\"--newline\",\"--ignore-config\",\"--no-playlist\",\"--newline\"],\"DefaultListCmdOptions\":[\"-F\"],\"DownloadUrl\":\"https://api.github.com/repos/ytdl-org/youtube-dl/releases/latest\",\"Name\":\"youtube-dl\",\"OptionsArgument\":\"-f\",\"UsePrivateExecutable\":false,\"VersionArgument\":\"--version\",\"VersionStringLine\":0,\"VersionStringPosition\":0}" ;
		}else{
			m = "{\"BackendPath\":\"/home/ink/.local/share/media-downloader/bin\",\"CommandName\":\"youtube-dl.exe\",\"DefaultDownLoadCmdOptions\":[\"--newline\",\"--ignore-config\",\"--no-playlist\",\"--newline\"],\"DefaultListCmdOptions\":[\"-F\"],\"DownloadUrl\":\"https://api.github.com/repos/ytdl-org/youtube-dl/releases/latest\",\"Name\":\"youtube-dl\",\"OptionsArgument\":\"-f\",\"UsePrivateExecutable\":true,\"VersionArgument\":\"--version\",\"VersionStringLine\":0,\"VersionStringPosition\":0}" ;
		}

		QJsonParseError error ;

		auto json = QJsonDocument::fromJson( m,&error ) ;

		m_backends.emplace_back( json,youtube_dl().functions() ) ;
	}

	this->setDefaultEngine( m_backends[ 0 ].name() ) ;
}

const std::vector< engines::engine >& engines::getEngines()
{
	return m_backends ;
}

const engines::engine& engines::defaultEngine()
{
	return this->getEngineByName( m_defaultEngine ) ;
}

const engines::engine & engines::getEngineByName( const QString& name )
{
	for( size_t i = 1 ; i < m_backends.size() ; i++ ){

		const auto& m = m_backends[ i ] ;

		if( m.name() == name ){

			return m ;
		}
	}

	return m_backends[ 0 ] ;
}

void engines::setDefaultEngine( const QString& name )
{
	m_defaultEngine = name ;
}

static QStringList _toStringList( const QJsonValue& value ){

	QStringList m ;

	auto array = value.toArray() ;

	for( int i = 0 ; i < array.size() ;i ++ ){

		m.append( array.at( i ).toString() ) ;
	}

	return m ;
}

engines::engine::engine( const QJsonDocument& json,engines::engine::functions functions ) :
	m_jsonObject( json.object() ),
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
	m_downloadPath( m_jsonObject.value( "DownloadUrl" ).toString() ),
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

void engines::log::add( const QString& s )
{
	auto a = m_textEdit.toPlainText() ;

	if( a.isEmpty() ){

		m_textEdit.setPlainText( "[media-downloader] " + s ) ;
	}else{
		m_textEdit.setPlainText( a + "\n[media-downloader] " + s ) ;
	}

	m_textEdit.moveCursor( QTextCursor::End ) ;
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

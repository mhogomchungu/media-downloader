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

#include "settings.h"
#include "utility.h"
#include "locale_path.h"
#include "translator.h"

#include<QDir>

settings::settings() : m_settings( "media-downloader","media-downloader" )
{
#if QT_VERSION >= QT_VERSION_CHECK( 5,6,0 )

	auto m = this->highDpiScalingFactor() ;

	if( m != "1.0" ){

		QApplication::setAttribute( Qt::AA_EnableHighDpiScaling ) ;

		qputenv( "QT_SCALE_FACTOR",m ) ;
	}
#endif
}

QString settings::downloadFolder()
{
	if( !m_settings.contains( "DownloadFolder" ) ){
#ifdef Q_OS_LINUX
		m_settings.setValue( "DownloadFolder",QDir::homePath() ) ;
	#else
		m_settings.setValue( "DownloadFolder",QDir::homePath() + "/Desktop" ) ;
#endif
	}

	return m_settings.value( "DownloadFolder" ).toString() ;
}

void settings::setPresetToDefaults()
{
	QStringList s{ "144p(bestvideo[height=144][ext=mp4]+bestaudio[ext=m4a]/bestvideo[height=144]+bestaudio)",
		       "240p(bestvideo[height=240][ext=mp4]+bestaudio[ext=m4a]/bestvideo[height=240]+bestaudio)",
		       "360p(bestvideo[height=360][ext=mp4]+bestaudio[ext=m4a]/bestvideo[height=360]+bestaudio)",
		       "480p(bestvideo[height=480][ext=mp4]+bestaudio[ext=m4a]/bestvideo[height=480]+bestaudio)",
		       "720p(bestvideo[height=720][ext=mp4]+bestaudio[ext=m4a]/bestvideo[height=720]+bestaudio)",
		       "1080p(bestvideo[height=1080][ext=mp4]+bestaudio[ext=m4a]/bestvideo[height=1080]+bestaudio)",
		       "1440p(bestvideo[height=1440][ext=mp4]+bestaudio[ext=m4a]/bestvideo[height=1440]+bestaudio)",
		       "2160p(bestvideo[height=2160][ext=mp4]+bestaudio[ext=m4a]/bestvideo[height=2160]+bestaudio)",
		       "Best-audiovideo(best)","Best-audio(bestaudio)" } ;

	m_settings.setValue( "PresetOptionsDefaults",s ) ;

	m_settings.setValue( "PresetOptions",m_settings.value( "PresetOptionsDefaults" ).toStringList() ) ;
}

void settings::setPresetOptions( const QString& e )
{
	auto m = e ;
	m.replace( "\n","" ) ;
	this->setPresetOptions( utility::split( m,',',true ) ) ;
}

void settings::setPresetOptions( const QStringList& m )
{
	m_settings.setValue( "PresetOptions",m ) ;
}

QString settings::presetOptions()
{
	if( !m_settings.contains( "PresetOptions" ) ){

		this->setPresetToDefaults() ;
	}

	return m_settings.value( "PresetOptions" ).toStringList().join( ',' ) ;
}

QString settings::cmdName()
{
	if( !m_settings.contains( "CommandName" ) ){

		if( utility::platformIsWindows() ){

			m_settings.setValue( "CommandName","youtube-dl.exe" ) ;
		}else{
			m_settings.setValue( "CommandName","youtube-dl" ) ;
		}
	}

	return m_settings.value( "CommandName" ).toString() ;
}

QStringList settings::presetOptionsList()
{
	return utility::split( this->presetOptions(),',',true ) ;
}

QStringList settings::defaultDownLoadCmdOptions()
{
	if( !m_settings.contains( "DefaultDownLoadCmdOptions" ) ){

		QStringList m{ "--newline","--ignore-config","--no-playlist" } ;

		m_settings.setValue( "DefaultDownLoadCmdOptions",m ) ;
	}

	return m_settings.value( "DefaultDownLoadCmdOptions" ).toStringList() ;
}

QStringList settings::defaultListCmdOptions()
{
	if( !m_settings.contains( "DefaultListCmdOptions" ) ){

		m_settings.setValue( "DefaultListCmdOptions",QStringList{ "-F" } ) ;
	}

	return m_settings.value( "DefaultListCmdOptions" ).toStringList() ;
}

bool settings::showTrayIcon()
{
	if( !m_settings.contains( "ShowTrayIcon" ) ){

		m_settings.setValue( "ShowTrayIcon",false ) ;
	}

	return m_settings.value( "ShowTrayIcon" ).toBool() ;
}

bool settings::autoDownload()
{
	if( !m_settings.contains( "AutoDownload" ) ){

		m_settings.setValue( "AutoDownload",false ) ;
	}

	return m_settings.value( "AutoDownload" ).toBool() ;
}

void settings::setHighDpiScalingFactor( const QString& m )
{
	m_settings.setValue( "EnabledHighDpiScalingFactor",m.toUtf8() ) ;
}

QByteArray settings::highDpiScalingFactor()
{
	if( !m_settings.contains( "EnabledHighDpiScalingFactor" ) ){

		m_settings.setValue( "EnabledHighDpiScalingFactor",QByteArray( "1.0" ) ) ;
	}

	return m_settings.value( "EnabledHighDpiScalingFactor" ).toByteArray() ;
}

void settings::setDownloadFolder( const QString& m )
{
	m_settings.setValue( "DownloadFolder",m ) ;
}

static QStringList _directoryList( const QString& e )
{
	QDir d( e ) ;

	auto s = d.entryList() ;

	s.removeOne( "." ) ;
	s.removeOne( ".." ) ;

	return s ;
}

QStringList settings::localizationLanguages()
{
	QStringList m ;

	const auto e = _directoryList( this->localizationLanguagePath() ) ;

	for( const auto& it : e ){

		if( !it.startsWith( "qt_" ) && it.endsWith( ".qm" ) ){

			auto name = it ;
			name.remove( ".qm" ) ;

			m.append( name ) ;
		}
	}

	return m ;
}

QString settings::localizationLanguagePath()
{
	if( !m_settings.contains( "TranslationsPath" ) ){

		if( utility::platformIsWindows() ){

			m_settings.setValue( "TranslationsPath",QDir().currentPath() + "/translations" ) ;

		}else if( utility::platformIsOSX() ){

			m_settings.setValue( "TranslationsPath",TRANSLATION_PATH ) ;
		}else{
			m_settings.setValue( "TranslationsPath",TRANSLATION_PATH ) ;
		}
	}

	return m_settings.value( "TranslationsPath" ).toString() ;
}

void settings::setLocalizationLanguage( const QString& language )
{
	m_settings.setValue( "Language",language ) ;
}

QString settings::localizationLanguage()
{
	if( !m_settings.contains( "Language" ) ){

		m_settings.setValue( "Language","en_US" ) ;
	}

	return m_settings.value( "Language" ).toString() ;
}

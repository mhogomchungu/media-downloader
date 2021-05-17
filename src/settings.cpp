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
#include "logger.h"

#include <QDir>
#include <QApplication>
#include <QFile>
#include <QStyleFactory>

bool settings::portableVersion()
{
	if( utility::platformIsWindows() ){

		auto a = QFile::exists( settings::portableVersionConfigPath() ) ;
		auto b = QFile::exists( QDir::currentPath() + "/media-downloader.exe" ) ;

		return a && b ;
	}else{
		return false ;
	}
}

QString settings::portableVersionConfigPath()
{
	return QDir::currentPath() + "/local" ;
}

QString settings::darkMode()
{
	if( !m_settings.contains( "DarkModeName" ) ){

		m_settings.setValue( "DarkModeName",settings::darkModes().unTranslatedAt( 0 ) ) ;
	}

	return m_settings.value( "DarkModeName" ).toString() ;
}

void settings::setDarkMode( const QString& e )
{
	m_settings.setValue( "DarkModeName",e ) ;
}

static std::unique_ptr< QSettings > _init()
{
	if( settings::portableVersion() ){

		QCoreApplication::setOrganizationName( "media-downloader" ) ;
		QCoreApplication::setApplicationName( "media-downloader" ) ;

		QDir().mkpath( settings::portableVersionConfigPath() + "/settings/" ) ;

		auto m = settings::portableVersionConfigPath() + "/settings/settings.ini" ;

		return std::make_unique< QSettings >( m,QSettings::IniFormat ) ;
	}else{
		return std::make_unique< QSettings >( "media-downloader","media-downloader" ) ;
	}
}

settings::settings() :
	m_settingsP( _init() ),
	m_settings( *m_settingsP ),
	m_portableVersion( settings::portableVersion() )
{
#if QT_VERSION >= QT_VERSION_CHECK( 5,6,0 )

	m_EnableHighDpiScaling = true ;
	QApplication::setAttribute( Qt::AA_EnableHighDpiScaling ) ;
#else
	m_EnableHighDpiScaling = false ;
#endif	
	auto m = this->highDpiScalingFactor() ;

	if( m != "1.0" ){

		qputenv( "QT_SCALE_FACTOR",m ) ;
	}
}

void settings::setTabNumber( int s )
{
	m_settings.setValue( "TabNumber",s ) ;
}

int settings::tabNumber()
{
	if( !m_settings.contains( "TabNumber" ) ){

		m_settings.setValue( "TabNumber",0 ) ;
	}

	return m_settings.value( "TabNumber" ).toInt() ;
}

int settings::maxConcurrentDownloads()
{
	if( !m_settings.contains( "MaxConcurrentDownloads" ) ){

		m_settings.setValue( "MaxConcurrentDownloads",4 ) ;
	}

	return m_settings.value( "MaxConcurrentDownloads" ).toInt() ;
}

void settings::setMaxConcurrentDownloads( int s )
{
	m_settings.setValue( "MaxConcurrentDownloads",s ) ;
}

void settings::setDownloadFolder( const QString& m )
{
	m_settings.setValue( "DownloadFolder",m ) ;
}

template< typename Function >
static QString _downloadFolder( QSettings& settings,bool portableVersion,Function function )
{
	if( portableVersion ){

		if( settings.contains( "DownloadFolder" ) ){

			auto m = settings.value( "DownloadFolder" ).toString() ;

			if( QFile::exists( m ) ){

				return m ;
			}else{
				function( QObject::tr( "Resetting download folder to default" ) ) ;
				settings.remove( "DownloadFolder" ) ;
				return QDir::currentPath() + "/Downloads" ;
			}
		}else{
			return QDir::currentPath() + "/Downloads" ;
		}
	}

	if( !settings.contains( "DownloadFolder" ) ){

		settings.setValue( "DownloadFolder",utility::homePath() ) ;
	}

	return settings.value( "DownloadFolder" ).toString() ;
}

QString settings::downloadFolder()
{
	return _downloadFolder( m_settings,m_portableVersion,[]( const QString& ){} ) ;
}

QString settings::downloadFolder( Logger& logger )
{
	return _downloadFolder( m_settings,m_portableVersion,[ &logger ]( const QString& e ){

		logger.add( e ) ;
	} ) ;
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

void settings::setDefaultEngine( const QString& e )
{
	m_settings.setValue( "DefaultEngine",e ) ;
}

QString settings::defaultEngine()
{
	if( !m_settings.contains( "DefaultEngine" ) ){

		m_settings.setValue( "DefaultEngine","youtube-dl" ) ;
	}

	return m_settings.value( "DefaultEngine" ).toString() ;
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

QStringList settings::presetOptionsList()
{
	return utility::split( this->presetOptions(),',',true ) ;
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

		m_settings.setValue( "AutoDownload",true ) ;
	}

	return m_settings.value( "AutoDownload" ).toBool() ;
}

bool settings::showVersionInfoWhenStarting()
{
	if( !m_settings.contains( "ShowVersionInfoWhenStarting" ) ){

		m_settings.setValue( "ShowVersionInfoWhenStarting",true ) ;
	}

	return m_settings.value( "ShowVersionInfoWhenStarting" ).toBool() ;
}

bool settings::concurrentDownloading()
{
	if( !m_settings.contains( "ConcurrentDownloading" ) ){

		m_settings.setValue( "ConcurrentDownloading",true ) ;
	}

	return m_settings.value( "ConcurrentDownloading" ).toBool() ;
}

bool settings::useSystemProvidedVersionIfAvailable()
{
	if( !m_settings.contains( "UseSystemProvidedVersionIfAvailable" ) ){

		if( utility::platformIsWindows() ){

			m_settings.setValue( "UseSystemProvidedVersionIfAvailable",false ) ;
		}else{
			m_settings.setValue( "UseSystemProvidedVersionIfAvailable",true ) ;
		}
	}

	return m_settings.value( "UseSystemProvidedVersionIfAvailable" ).toBool() ;
}

bool settings::doNotGetUrlTitle()
{
	if( !m_settings.contains( "DoNotGetURLTitle" ) ){

		m_settings.setValue( "DoNotGetURLTitle",true ) ;
	}

	return m_settings.value( "DoNotGetURLTitle" ).toBool() ;
}

void settings::setTheme( QApplication& app )
{
	settings::darkModes darkModes( this->darkMode() ) ;

	if( darkModes.darkModeIsSet() ){

		if( darkModes.fusionTheme() ){

			app.setStyle( QStyleFactory::create( "Fusion" ) ) ;
			QPalette darkPalette ;
			QColor darkColor = QColor( 45,45,45 ) ;
			QColor disabledColor = QColor( 127,127,127 ) ;
			darkPalette.setColor( QPalette::Window,darkColor ) ;
			darkPalette.setColor( QPalette::WindowText,Qt::white ) ;
			darkPalette.setColor( QPalette::Base,QColor( 18,18,18 ) ) ;
			darkPalette.setColor( QPalette::AlternateBase,darkColor ) ;
			darkPalette.setColor( QPalette::ToolTipBase,Qt::white ) ;
			darkPalette.setColor( QPalette::ToolTipText,Qt::white ) ;
			darkPalette.setColor( QPalette::Text,Qt::white ) ;
			darkPalette.setColor( QPalette::Disabled,QPalette::Text,disabledColor ) ;
			darkPalette.setColor( QPalette::Button,darkColor ) ;
			darkPalette.setColor( QPalette::ButtonText,Qt::white ) ;
			darkPalette.setColor( QPalette::Disabled,QPalette::ButtonText,disabledColor ) ;
			darkPalette.setColor( QPalette::BrightText,Qt::red) ;
			darkPalette.setColor( QPalette::Link,QColor( 42,130,218 ) ) ;
			darkPalette.setColor( QPalette::Highlight,QColor( 42,130,218 ) ) ;
			darkPalette.setColor( QPalette::HighlightedText,Qt::black ) ;
			darkPalette.setColor( QPalette::Disabled,QPalette::HighlightedText,disabledColor ) ;

			app.setPalette( darkPalette ) ;

			app.setStyleSheet("QToolTip { color: #ffffff; background-color: #2a82da; border: 1px solid white; }");
		}else{
			QFile file( darkModes.themeFileName() ) ;
			file.open( QFile::ReadOnly ) ;
			app.setStyleSheet( file.readAll() ) ;
		}
	}
}

void settings::setUseSystemProvidedVersionIfAvailable( bool e )
{
	m_settings.setValue( "UseSystemProvidedVersionIfAvailable",e ) ;
}

void settings::setConcurrentDownloading( bool e )
{
	m_settings.setValue( "ConcurrentDownloading",e ) ;
}

void settings::setShowVersionInfoWhenStarting( bool e )
{
	m_settings.setValue( "ShowVersionInfoWhenStarting",e ) ;
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

bool settings::enabledHighDpiScaling()
{
	return m_EnableHighDpiScaling ;
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

QStringList settings::configPaths()
{
	if( settings::portableVersion() ){

		return { settings::portableVersionConfigPath() } ;
	}
#if QT_VERSION >= QT_VERSION_CHECK( 5,6,0 )
	return QStandardPaths::standardLocations( QStandardPaths::AppDataLocation ) ;
#else
	return QStringList{ QDir::homePath() + "/.config/media-downloader/" } ;
#endif
}

QString settings::commandOnSuccessfulDownload()
{
	if( !m_settings.contains( "CommandOnSuccessfulDownload" ) ){

		m_settings.setValue( "CommandOnSuccessfulDownload",QString() ) ;
	}

	return m_settings.value( "CommandOnSuccessfulDownload" ).toString() ;
}

QString settings::commandWhenAllFinished()
{
	if( !m_settings.contains( "CommandWhenAllFinished" ) ){

		m_settings.setValue( "CommandWhenAllFinished",QString() ) ;
	}

	return m_settings.value( "CommandWhenAllFinished" ).toString() ;
}

static QString _getTabOption( settings::tabName e )
{
	if( e == settings::tabName::basic ){

		return "LastUsedOptionBasicTab" ;

	}else if( e == settings::tabName::batch ){

		return "LastUsedOptionBatchTab" ;

	}else if( e == settings::tabName::playlist ){

		return "LastUsedOptionPlayListTab" ;
	}else{
		return "" ;
	}
}

QString settings::lastUsedOption( settings::tabName e )
{
	auto s = _getTabOption( e ) ;

	if( !m_settings.contains( s ) ){

		m_settings.setValue( s,QString() ) ;
	}

	return m_settings.value( s ).toString() ;
}

void settings::setLastUsedOption( const QString& e,settings::tabName s )
{
	m_settings.setValue( _getTabOption( s ),e ) ;
}

QString settings::localizationLanguagePath()
{
	if( m_portableVersion ){

		return QDir().currentPath() + "/translations" ;
	}

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

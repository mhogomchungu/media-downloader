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
#include "themes.h"

#include <QDir>
#include <QFile>

#include <cstring>

static QString _configPath()
{
#if QT_VERSION >= QT_VERSION_CHECK( 5,6,0 )
	auto s = QStandardPaths::standardLocations( QStandardPaths::AppDataLocation ) ;

	if( s.isEmpty() ){

		return QDir::homePath() + "/.config/media-downloader/" ;
	}else{
		return s.first() + "/media-downloader/" ;
	}
#else
	return QDir::homePath() + "/.config/media-downloader/" ;
#endif
}

bool settings::portableVersion()
{
	return m_portableVersion ;
}

static QString _monitorClipboadUrl( settings::tabName e )
{
	if( e == settings::tabName::basic ){

		return "BasicDownloaderMonitorClipboadUrl" ;

	}else if( e == settings::tabName::batch ){

		return "BatchDownloaderMonitorClipboadUrl" ;

	}else if( e == settings::tabName::playlist ){

		return "PlaylistDownloaderMonitorClipboadUrl" ;
	}else{
		return "" ;
	}
}

void settings::setMonitorClipboardUrl( bool e,settings::tabName t )
{
	m_settings.setValue( _monitorClipboadUrl( t ),e ) ;
}

bool settings::monitorClipboardUrl( settings::tabName tabName )
{
	auto m = _monitorClipboadUrl( tabName ) ;

	if( !m_settings.contains( m ) ){

		m_settings.setValue( m,false ) ;
	}

	return m_settings.value( m ).toBool() ;
}

QString settings::themeName()
{
	if( !m_settings.contains( "ThemeName" ) ){

		m_settings.setValue( "ThemeName",themes().unTranslatedAt( 0 ) ) ;
	}

	return m_settings.value( "ThemeName" ).toString() ;
}

void settings::setThemeName( const QString& e )
{
	m_settings.setValue( "ThemeName",e ) ;
}

static QString _getOptionsHistoryTabName( settings::tabName e )
{
	if( e == settings::tabName::basic ){

		return "BasicDownloaderOptionsHistory" ;

	}else if( e == settings::tabName::batch ){

		return "BatchDownloaderOptionsHistory" ;

	}else if( e == settings::tabName::playlist ){

		return "PlaylistDownloaderOptionsHistory" ;
	}else{
		return "" ;
	}
}

QStringList settings::getOptionsHistory( settings::tabName e )
{
	auto m = _getOptionsHistoryTabName( e ) ;

	if( !m_settings.contains( m ) ){

		m_settings.setValue( m,QStringList() ) ;
	}

	return m_settings.value( m ).toStringList() ;
}

void settings::clearOptionsHistory( settings::tabName e )
{
	m_settings.setValue( _getOptionsHistoryTabName( e ),QStringList() ) ;
}

static void _addToHistory( QSettings& settings,
			   QStringList&& history,
			   const QString& key,
			   const QString& input,
			   int max )
{
	if( !input.isEmpty() && !history.contains( input ) ){

		if( history.size() == max ){

			history.removeLast() ;
		}

		history.insert( 0,input ) ;

		settings.setValue( key,history ) ;
	}
}

void settings::addToplaylistRangeHistory( const QString& e )
{
	if( this->saveHistory() ){

		_addToHistory( m_settings,
			       this->playlistRangeHistory(),
			       "PlaylistRangeHistory",
			       e,
			       this->historySize() ) ;
	}
}

void settings::addOptionsHistory( const QString& e,settings::tabName s )
{
	if( this->saveHistory() ){

		_addToHistory( m_settings,
			       this->getOptionsHistory( s ),
			       _getOptionsHistoryTabName( s ),
			       e,
			       this->historySize() ) ;
	}
}

void settings::clearPlaylistRangeHistory()
{
	m_settings.setValue( "PlaylistRangeHistory",QStringList{ "--break-on-existing" } ) ;
}

void settings::clearPlaylistUrlHistory()
{
	m_settings.setValue( "PlaylistUrlHistory",QStringList() ) ;
}

void settings::setAutoSavePlaylistOnExit( bool e )
{
	m_settings.setValue( "AutoSavePlaylistOnExit",e ) ;
}

bool settings::autoSavePlaylistOnExit()
{
	if( !m_settings.contains( "AutoSavePlaylistOnExit" ) ){

		m_settings.setValue( "AutoSavePlaylistOnExit",true ) ;
	}

	return m_settings.value( "AutoSavePlaylistOnExit" ).toBool() ;
}

bool settings::useInternalArchiveFile()
{
	if( !m_settings.contains( "UseInternalArchiveFile" ) ){

		m_settings.setValue( "UseInternalArchiveFile",true ) ;
	}

	return m_settings.value( "UseInternalArchiveFile" ).toBool() ;
}

bool settings::checkForUpdates()
{
	if( !m_settings.contains( "CheckForUpdates" ) ){

		m_settings.setValue( "CheckForUpdates",utility::platformIsWindows() ) ;
	}

	return m_settings.value( "CheckForUpdates" ).toBool() ;
}

bool settings::enableLibraryTab()
{
	if( !m_settings.contains( "EnableLibraryTab" ) ){

		m_settings.setValue( "EnableLibraryTab",false ) ;
	}

	return m_settings.value( "EnableLibraryTab" ).toBool() ;
}

bool settings::checkForEnginesUpdates()
{
	if( !m_settings.contains( "CheckForEnginesUpdates" ) ){

		m_settings.setValue( "CheckForEnginesUpdates",true ) ;
	}

	return m_settings.value( "CheckForEnginesUpdates" ).toBool() ;
}

bool settings::notRunningUpdatedVersion()
{
	return m_exeOrgPath.isEmpty() ;
}

void settings::setEnableLibraryTab( bool e )
{
	m_settings.setValue( "EnableLibraryTab",e ) ;
}

void settings::setCheckForUpdates( bool e )
{
	m_settings.setValue( "CheckForUpdates",e ) ;
}

void settings::setUseInternalArchiveFile( bool e )
{
	m_settings.setValue( "UseInternalArchiveFile",e ) ;
}

int settings::networkTimeOut()
{
	if( !m_settings.contains( "NetworkTimeOutInSeconds" ) ){

		m_settings.setValue( "NetworkTimeOutInSeconds",30 ) ;
	}

	return m_settings.value( "NetworkTimeOutInSeconds" ).toInt() * 1000 ;
}

QStringList settings::playlistRangeHistory()
{
	if( !m_settings.contains( "PlaylistRangeHistory" ) ){

		m_settings.setValue( "PlaylistRangeHistory",QStringList{ "--break-on-existing" } ) ;
	}

	return m_settings.value( "PlaylistRangeHistory" ).toStringList() ;
}

QStringList settings::playlistUrlHistory()
{
	if( !m_settings.contains( "PlaylistUrlHistory" ) ){

		m_settings.setValue( "PlaylistUrlHistory",QStringList() ) ;
	}

	return m_settings.value( "PlaylistUrlHistory" ).toStringList() ;
}

void settings::setPlaylistRangeHistoryLastUsed( const QString& e )
{
	m_settings.setValue( "playlistRangeHistoryLastUsed",e ) ;
}

QString settings::playlistRangeHistoryLastUsed()
{
	if( !m_settings.contains( "playlistRangeHistoryLastUsed" ) ){

		m_settings.setValue( "playlistRangeHistoryLastUsed",QString( "--break-on-existing" ) ) ;
	}

	return m_settings.value( "playlistRangeHistoryLastUsed" ).toString() ;
}

static std::unique_ptr< QSettings > _set_config( const QString& path )
{
	QDir().mkpath( path + "/settings" ) ;

	auto m = path + "/settings/settings.ini" ;

	return std::make_unique< QSettings >( m,QSettings::IniFormat ) ;
}

static QString _portable_updated_data_path( int argc,char ** argv )
{
	for( int i = 0 ; i < argc ; i++ ){

		if( std::strcmp( argv[ i ],"--dataPath" ) == 0 ){

			if( i + 1 < argc ){

				return argv[ i + 1 ] ;
			}
		}
	}

	return {} ;
}

static bool _portableVersionInit( int argc,
				  char ** argv,
				  QString& dataPath,
				  QString& exePath,
				  QString& exeOrgPath )
{
	if( utility::platformIsWindows() ){

		exePath = utility::windowsApplicationDirPath() ;

		dataPath = _portable_updated_data_path( argc,argv ) ;

		for( int i = 0 ; i < argc ; i++ ){

			if( std::strcmp( argv[ i ],"--exe-org-path" ) == 0 ){

				if( i + 1 < argc ){

					exeOrgPath = argv[ i + 1 ] ;
				}
			}
		}

		for( int i = 0 ; i < argc ; i++ ){

			if( std::strcmp( argv[ i ],"--portable" ) == 0 ){

				return true ;
			}
		}

		auto a = exePath  + "/local" ;

		if( QFile::exists( a ) ){

			dataPath = a ;

			return true ;
		}else{
			dataPath = _configPath() ;

			return false ;
		}
	}else{
		dataPath = _configPath() ;

		return false ;
	}
}

static std::unique_ptr< QSettings > _init( const QString& dataPath,bool portableVersion )
{	
	if( utility::platformIsWindows() ){

		if( portableVersion ){

			return _set_config( dataPath ) ;
		}

		auto appPath      = _configPath() ;
		auto settingsPath = appPath + "/settings" ;

		if( QFile::exists( settingsPath ) ){

			return _set_config( appPath ) ;
		}else{
			/*
			 * Migrating from registry based config to text file config.
			 */
			QSettings oldSettings( "media-downloader","media-downloader" ) ;

			auto newSettings = _set_config( appPath ) ;

			const auto keys = oldSettings.allKeys() ;

			for( const auto& it : keys ){

				newSettings->setValue( it,oldSettings.value( it ) ) ;
			}

			oldSettings.clear() ;

			return newSettings ;
		}
	}else{
		return std::make_unique< QSettings >( "media-downloader","media-downloader" ) ;
	}
}

settings::settings( int argc,char ** argv ) :
	m_portableVersion( _portableVersionInit( argc,argv,m_dataPath,m_exePath,m_exeOrgPath ) ),
	m_settingsP( _init( m_dataPath,portableVersion() ) ),
	m_settings( *m_settingsP )
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

QSettings& settings::bk()
{
	return m_settings ;
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

int settings::maxLoggerProcesses()
{
	if( !m_settings.contains( "MaxLoggerProcesses" ) ){

		m_settings.setValue( "MaxLoggerProcesses",0 ) ;
	}

	return m_settings.value( "MaxLoggerProcesses" ).toInt() ;
}

size_t settings::maxConcurrentDownloads()
{
	if( !m_settings.contains( "MaxConcurrentDownloads" ) ){

		m_settings.setValue( "MaxConcurrentDownloads",4 ) ;
	}

	return static_cast< size_t >( m_settings.value( "MaxConcurrentDownloads" ).toInt() ) ;
}

void settings::setMaxConcurrentDownloads( int s )
{
	m_settings.setValue( "MaxConcurrentDownloads",s ) ;
}

void settings::setDownloadFolder( const QString& m )
{
	if( m.isEmpty() ){

		auto s = utility::stringConstants::mediaDownloaderDefaultDownloadPath() ;

		m_settings.setValue( "DownloadFolder",s ) ;
	}else{
		m_settings.setValue( "DownloadFolder",m ) ;
	}
}

template< typename Function >
static QString _downloadFolder( QSettings& settings,bool portableVersion,Function function )
{
	auto mediaDownloaderCWD = utility::stringConstants::mediaDownloaderCWD() ;

	auto mediaDownloaderDefaultDownloadPath = utility::stringConstants::mediaDownloaderDefaultDownloadPath() ;

	if( portableVersion ){

		if( settings.contains( "DownloadFolder" ) ){

			auto m = settings.value( "DownloadFolder" ).toString() ;

			if( m.startsWith( mediaDownloaderCWD ) ){

				m.replace( mediaDownloaderCWD,QDir::currentPath() ) ;

				return m ;
			}

			if( m.startsWith( mediaDownloaderDefaultDownloadPath ) ){

				m.replace( mediaDownloaderDefaultDownloadPath,QDir::currentPath() + "/Downloads" ) ;

				return m ;
			}

			if( QFile::exists( m ) ){

				return m ;
			}else{
				function( QObject::tr( "Resetting download folder to default" ) ) ;
				settings.setValue( "DownloadFolder",mediaDownloaderDefaultDownloadPath ) ;
				return QDir::currentPath() + "/Downloads" ;
			}
		}else{
			settings.setValue( "DownloadFolder",mediaDownloaderDefaultDownloadPath ) ;
			return QDir::currentPath() + "/Downloads" ;
		}
	}else{
		if( !settings.contains( "DownloadFolder" ) ){

			settings.setValue( "DownloadFolder",mediaDownloaderDefaultDownloadPath ) ;
		}

		auto m = settings.value( "DownloadFolder" ).toString() ;

		if( m.startsWith( mediaDownloaderCWD ) ){

			m.replace( mediaDownloaderCWD,QDir::currentPath() ) ;

			return m ;
		}

		if( m.startsWith( mediaDownloaderDefaultDownloadPath ) ){

			m.replace( mediaDownloaderDefaultDownloadPath,utility::homePath() ) ;
		}

		return m ;
	}
}

QString settings::downloadFolder()
{
	return _downloadFolder( m_settings,this->portableVersion(),[]( const QString& ){} ) ;
}

QString settings::downloadFolder( Logger& logger )
{
	return _downloadFolder( m_settings,this->portableVersion(),[ &logger ]( const QString& e ){

		logger.add( e,utility::sequentialID() ) ;
	} ) ;
}

bool settings::showTrayIcon()
{
	if( !m_settings.contains( "ShowTrayIcon" ) ){

		m_settings.setValue( "ShowTrayIcon",false ) ;
	}

	return m_settings.value( "ShowTrayIcon" ).toBool() ;
}

void settings::setshowTrayIcon( bool e )
{
	m_settings.setValue( "ShowTrayIcon",e ) ;
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

QString settings::cookieFilePath( const QString& engineName )
{
	auto m = "CookieFilePath_" + engineName ;

	if( !m_settings.contains( m ) ){

		m_settings.setValue( m,QString() ) ;
	}

	return m_settings.value( m ).toString() ;
}

void settings::setCookieFilePath( const QString& engineName,const QString& cookieFilePath )
{
	m_settings.setValue( "CookieFilePath_" + engineName,cookieFilePath ) ;
}

void settings::setTheme( QApplication& app,const QString& themeBasePath )
{
	themes ths( this->themeName(),themeBasePath ) ;

	if( !QFile::exists( themeBasePath ) ){

		QDir().mkpath( themeBasePath ) ;

		auto defaultThemePath = ths.defaultthemeFullPath() ;

		if( !QFile::exists( defaultThemePath ) ){

			QFile f( defaultThemePath ) ;

			if( f.open( QIODevice::WriteOnly ) ){

				f.write( QJsonDocument( ths.defaultTheme() ).toJson( QJsonDocument::Indented ) ) ;
			}
		}
	}

	if( ths.usingThemes() ){

		QFile f( ths.themeFullPath() ) ;

		if( !f.open( QIODevice::ReadOnly ) ){

			ths.setDefaultTheme( app ) ;
		}else{
			auto obj = QJsonDocument::fromJson( f.readAll() ).object() ;

			if( obj.isEmpty() ){

				return ths.setDefaultTheme( app ) ;
			}

			ths.setTheme( app,obj ) ;
		}
	}
}

void settings::setUseSystemProvidedVersionIfAvailable( bool e )
{
	m_settings.setValue( "UseSystemProvidedVersionIfAvailable",e ) ;
}

void settings::setShowThumbnails( bool e )
{
	m_settings.setValue( "ShowThumbnails",e ) ;
}

bool settings::showThumbnails()
{
	if( !m_settings.contains( "ShowThumbnails" ) ){

		m_settings.setValue( "ShowThumbnails",false ) ;
	}

	return m_settings.value( "ShowThumbnails" ).toBool() ;
}

bool settings::saveHistory()
{
	if( !m_settings.contains( "SaveHistory" ) ){

		m_settings.setValue( "SaveHistory",true ) ;
	}

	return m_settings.value( "SaveHistory" ).toBool() ;
}

bool settings::playlistDownloaderSaveHistory()
{
	if( !m_settings.contains( "PlaylistDownloaderSaveHistory" ) ){

		m_settings.setValue( "PlaylistDownloaderSaveHistory",true ) ;
	}

	return m_settings.value( "PlaylistDownloaderSaveHistory" ).toBool() ;
}

bool settings::singleInstance()
{
	if( !m_settings.contains( "SingleInstance" ) ){

		m_settings.setValue( "SingleInstance",true ) ;
	}

	return m_settings.value( "SingleInstance" ).toBool() ;
}

void settings::setPlaylistDownloaderSaveHistory( bool e )
{
	m_settings.setValue( "PlaylistDownloaderSaveHistory",e ) ;
}

int settings::stringTruncationSize()
{
	if( !m_settings.contains( "StringTruncationSize" ) ){

		m_settings.setValue( "StringTruncationSize",100 ) ;
	}

	return m_settings.value( "StringTruncationSize" ).toInt() ;
}

int settings::historySize()
{
	if( !m_settings.contains( "HistorySize" ) ){

		m_settings.setValue( "HistorySize",10 ) ;
	}

	return m_settings.value( "HistorySize" ).toInt() ;
}

static QString _thumbnailTabName( const QString& s, settings::tabName e )
{
	if( e == settings::tabName::batch ){

		return s + "_batch" ;

	}else if( e == settings::tabName::playlist ){

		return s + "_playlist" ;
	}else{
		return "" ;
	}
}

int settings::thumbnailWidth( settings::tabName s )
{
	auto m = _thumbnailTabName( "ThumbnailWidth",s ) ;

	if( !m_settings.contains( m ) ){

		m_settings.setValue( m,128 ) ;
	}

	return m_settings.value( m ).toInt() ;
}

int settings::thumbnailHeight( settings::tabName s )
{
	auto m = _thumbnailTabName( "ThumbnailHeight",s ) ;

	if( !m_settings.contains( m ) ){

		m_settings.setValue( m,72 ) ;
	}

	return m_settings.value( m ).toInt() ;
}

void settings::setShowVersionInfoWhenStarting( bool e )
{
	m_settings.setValue( "ShowVersionInfoWhenStarting",e ) ;
}

void settings::setHighDpiScalingFactor( const QString& m )
{
	m_settings.setValue( "EnabledHighDpiScalingFactor",m.toUtf8() ) ;
}

QString settings::textEncoding()
{
	if( !m_settings.contains( "YtDlpTextEncoding" ) ){

		m_settings.setValue( "YtDlpTextEncoding",QString() ) ;
	}

	return m_settings.value( "YtDlpTextEncoding" ).toString() ;
}

void settings::setTextEncoding( const QString& e )
{
	m_settings.setValue( "YtDlpTextEncoding",e ) ;
}

void settings::setlibraryDownloadFolder( const QString& e )
{
	m_settings.setValue( "LibraryDownloadFolder",QDir::fromNativeSeparators( e ) ) ;
}

QString settings::libraryDownloadFolder()
{
	if( !m_settings.contains( "LibraryDownloadFolder" ) ){

		auto m = QDir::fromNativeSeparators( this->downloadFolder() ) ;

		m_settings.setValue( "LibraryDownloadFolder",m ) ;
	}

	return m_settings.value( "LibraryDownloadFolder" ).toString() ;
}

static QString _getDefaultEngineName( settings::tabName e )
{
	if( e == settings::tabName::basic ){

		return "BasicDownloaderDefaultEngine" ;

	}else if( e == settings::tabName::batch ){

		return "BatchDownloaderDefaultEngine" ;

	}else if( e == settings::tabName::playlist ){

		return "PlaylistDownloaderDefaultEngine" ;
	}else{
		return "" ;
	}
}

void settings::setDefaultEngine( const QString& e,settings::tabName n )
{
	m_settings.setValue( _getDefaultEngineName( n ),e ) ;
}

QString settings::defaultEngine( settings::tabName n,const QString& engineName )
{
	auto m = _getDefaultEngineName( n ) ;

	if( !m_settings.contains( m ) ){

		m_settings.setValue( m,engineName ) ;
	}

	return m_settings.value( m ).toString() ;
}

QByteArray settings::highDpiScalingFactor()
{
	if( !m_settings.contains( "EnabledHighDpiScalingFactor" ) ){

		m_settings.setValue( "EnabledHighDpiScalingFactor",QByteArray( "1.0" ) ) ;
	}

	return m_settings.value( "EnabledHighDpiScalingFactor" ).toByteArray() ;
}

QPixmap settings::defaultVideoThumbnailIcon( settings::tabName m )
{
	auto width = this->thumbnailWidth( m ) ;
	auto height = this->thumbnailHeight( m ) ;

	return QIcon( ":/video" ).pixmap( width,height ) ;
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

const QString& settings::configPaths()
{
	return m_dataPath ;
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

static QString _getTabOption( const QString& s,settings::tabName e )
{
	if( e == settings::tabName::basic ){

		return "LastUsedOptionBasicTab_" + s ;

	}else if( e == settings::tabName::batch ){

		return "LastUsedOptionBatchTab_" + s ;

	}else if( e == settings::tabName::playlist ){

		return "LastUsedOptionPlayListTab_" + s ;
	}else{
		return "" ;
	}
}

QString settings::lastUsedOption( const QString& m,settings::tabName e )
{
	auto s = _getTabOption( m,e ) ;

	if( !m_settings.contains( s ) ){

		m_settings.setValue( s,QString() ) ;
	}

	return m_settings.value( s ).toString() ;
}

void settings::setLastUsedOption( const QString& m,const QString& e,settings::tabName s )
{
	m_settings.setValue( _getTabOption( m,s ),e ) ;
}

QString settings::localizationLanguagePath()
{
	if( utility::platformIsWindows() ){

		return m_exePath + "/translations" ;
	}

	if( !m_settings.contains( "TranslationsPath" ) ){

		if( utility::platformIsOSX() ){

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

void settings::setWindowDimensions( const QString& window,const QString& dimenstion )
{
	m_settings.setValue( "WindowDimensions_" + window,dimenstion ) ;
}

QString settings::windowsDimensions( const QString& window )
{
	auto m = "WindowDimensions_" + window ;

	if( !m_settings.contains( m ) ){

		m_settings.setValue( m,QString() ) ;
	}

	return m_settings.value( m ).toString() ;
}

QString settings::localizationLanguage()
{
	if( !m_settings.contains( "Language" ) ){

		m_settings.setValue( "Language","en_US" ) ;
	}

	return m_settings.value( "Language" ).toString() ;
}

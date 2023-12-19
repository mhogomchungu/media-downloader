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

static QString _downloadLocation()
{
#if QT_VERSION >= QT_VERSION_CHECK( 5,6,0 )
	auto s = QStandardPaths::standardLocations( QStandardPaths::DownloadLocation ) ;

	if( s.isEmpty() ){

		return QDir::homePath() + "/Downloads" ;
	}else{
		return s.first() ;
	}
#else
	return QDir::homePath() + "/Downloads" ;
#endif
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

template< typename Type >
QVariant _get_value( QSettings& s,const QString& opt,const Type& e )
{
	if( !s.contains( opt ) ){

		s.setValue( opt,e ) ;
	}

	return s.value( opt ) ;
}

QByteArray _getOption( QSettings& s,const QString& opt,const QByteArray& e )
{
	return _get_value( s,opt,e ).toByteArray() ;
}

QString _getOption( QSettings& s,const QString& opt,const QString& e )
{
	return _get_value( s,opt,e ).toString() ;
}

bool _getOption( QSettings& s,const QString& opt,bool e )
{
	return _get_value( s,opt,e ).toBool() ;
}

int _getOption( QSettings& s,const QString& opt,int e )
{
	return _get_value( s,opt,e ).toInt() ;
}

QStringList _getOption( QSettings& s,const QString& opt,const QStringList& e )
{
	return _get_value( s,opt,e ).toStringList() ;
}

bool settings::monitorClipboardUrl( settings::tabName tabName )
{
	auto m = _monitorClipboadUrl( tabName ) ;

	return _getOption( m_settings,m,false ) ;
}

QString settings::themeName()
{
	return _getOption( m_settings,"ThemeName",themes().unTranslatedAt( 0 ) ) ;
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

	return _getOption( m_settings,m,QStringList() ) ;
}

void settings::clearOptionsHistory( settings::tabName e )
{
	m_settings.setValue( _getOptionsHistoryTabName( e ),QStringList() ) ;
}

static void _addToHistory( QSettings& settings,
			   QStringList& history,
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

		auto a = this->playlistRangeHistory() ;
		auto b = "PlaylistRangeHistory" ;

		_addToHistory( m_settings,a,b,e,this->historySize() ) ;
	}
}

void settings::addOptionsHistory( const QString& e,settings::tabName s )
{
	if( this->saveHistory() ){

		auto a = this->getOptionsHistory( s ) ;
		auto b = _getOptionsHistoryTabName( s ) ;

		_addToHistory( m_settings,a,b,e,this->historySize() ) ;
	}
}

void settings::clearPlaylistRangeHistory()
{
	QStringList s{ "--break-on-existing" } ;

	m_settings.setValue( "PlaylistRangeHistory",s ) ;
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
	return _getOption( m_settings,"AutoSavePlaylistOnExit",true ) ;
}

bool settings::useInternalArchiveFile()
{
	return _getOption( m_settings,"UseInternalArchiveFile",true ) ;
}

bool settings::checkForUpdates()
{
	auto m = utility::platformIsWindows() ;

	return _getOption( m_settings,"CheckForUpdates",m ) ;
}

bool settings::enableLibraryTab()
{
	return _getOption( m_settings,"EnableLibraryTab",false ) ;
}

bool settings::checkForEnginesUpdates()
{
	return _getOption( m_settings,"CheckForEnginesUpdates",true ) ;
}

bool settings::autoHideDownloadWhenCompleted()
{
	return _getOption( m_settings,"AutoHideDownloadWhenCompleted",false ) ;
}

qint64 settings::timeOutWaitingForClipboardData()
{
	return _getOption( m_settings,"TimeOutWaitingForClipboardData",30000 ) ;
}

void settings::setAutoHideDownloadWhenCompleted( bool e )
{
	m_settings.setValue( "AutoHideDownloadWhenCompleted",e ) ;
}

int settings::textAlignment()
{
	auto m = _getOption( m_settings,"MainTableTextAlignment",QString( "center" ) ) ;

	if( m == "center" ){

		return Qt::AlignCenter ;

	}else if( m == "left" ){

		return Qt::AlignLeft | Qt::AlignVCenter ;

	}else if( m == "right" ){

		return Qt::AlignRight | Qt::AlignVCenter;
	}else{
		m_settings.setValue( "MainTableTextAlignment","center" ) ;
		return Qt::AlignCenter ;
	}
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
	return _getOption( m_settings,"NetworkTimeOutInSeconds",30 )  * 1000 ;
}

QStringList settings::playlistRangeHistory()
{
	QStringList s{ "--break-on-existing" } ;

	return _getOption( m_settings,"PlaylistRangeHistory",s ) ;
}

QStringList settings::playlistUrlHistory()
{
	return _getOption( m_settings,"PlaylistUrlHistory",QStringList() ) ;
}

void settings::setPlaylistRangeHistoryLastUsed( const QString& e )
{
	m_settings.setValue( "playlistRangeHistoryLastUsed",e ) ;
}

QString settings::playlistRangeHistoryLastUsed()
{
	QString s( "--break-on-existing" ) ;

	return _getOption( m_settings,"playlistRangeHistoryLastUsed",s ) ;
}

static std::unique_ptr< QSettings > _set_config( const QString& path )
{
	QDir().mkpath( path + "/settings" ) ;

	auto m = path + "/settings/settings.ini" ;

	return std::make_unique< QSettings >( m,QSettings::IniFormat ) ;
}

static std::unique_ptr< QSettings > _init( const QString& dataPath,bool portableVersion )
{	
	if( utility::platformIsWindows() ){

		if( portableVersion ){

			return _set_config( dataPath ) ;
		}else{
			return _set_config( _configPath() ) ;
		}
	}else{
		auto path = _configPath() ;

		if( QFile::exists( path + "/settings/settings.ini" ) ){

			return _set_config( path ) ;
		}else{
			/*
			 * Migrating to .ini config file
			 */
			QSettings oldSettings( "media-downloader","media-downloader" ) ;

			auto newSettings = _set_config( path ) ;

			const auto keys = oldSettings.allKeys() ;

			for( const auto& it : keys ){

				newSettings->setValue( it,oldSettings.value( it ) ) ;
			}

			oldSettings.clear() ;

			return newSettings ;
		}
	}
}

settings::settings( const utility::cliArguments& args ) :
	m_options( args ),
	m_settingsP( _init( m_options.dataPath(),m_options.portableVersion() ) ),
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
	return _getOption( m_settings,"TabNumber",0 ) ;
}

int settings::maxLoggerProcesses()
{
	return _getOption( m_settings,"MaxLoggerProcesses",0 ) ;
}

size_t settings::maxConcurrentDownloads()
{
	auto m = _getOption( m_settings,"MaxConcurrentDownloads",4 ) ;

	return static_cast< size_t >( m ) ;
}

const QString& settings::windowsOnly3rdPartyBinPath()
{
	return m_options.windowsOnly3rdPartyBinPath() ;
}

const QString& settings::windowsOnlyExeBinPath()
{
	return m_options.m_exePath ;
}

const QString& settings::windowsOnlyDefaultPortableVersionDownloadFolder()
{
	return m_options.windowsOnlyDefaultPortableVersionDownloadFolder() ;
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

static QString _downloadFolder( QSettings& settings,const QString& defaultPath,Logger * logger )
{
	auto mediaDownloaderCWD = utility::stringConstants::mediaDownloaderCWD() ;

	auto mm = utility::stringConstants::mediaDownloaderDefaultDownloadPath() ;

	if( !settings.contains( "DownloadFolder" ) ){

		settings.setValue( "DownloadFolder",mm ) ;
	}

	auto m = settings.value( "DownloadFolder" ).toString() ;

	if( m.startsWith( mediaDownloaderCWD ) ){

		m.replace( mediaDownloaderCWD,QDir::currentPath() ) ;

	}else if( m.startsWith( mm ) ){

		m.replace( mm,defaultPath ) ;
	}

	if( QFile::exists( m ) ){

		return m ;
	}else{
		if( logger ){

			auto id = utility::sequentialID() ;

			auto s = utility::barLine() ;

			logger->add( s,id ) ;

			logger->add( QObject::tr( "Resetting download folder to default" ),id ) ;

			logger->add( s,id ) ;
		}

		settings.setValue( "DownloadFolder",mm ) ;

		QDir().mkpath( defaultPath ) ;

		return defaultPath ;
	}
}

QString settings::downloadFolder( Logger * logger )
{
	if( utility::platformIsWindows() ){

		if( this->portableVersion() ){

			const auto& dPath = this->windowsOnlyDefaultPortableVersionDownloadFolder() ;

			return _downloadFolder( m_settings,dPath,logger ) ;
		}else{
			return _downloadFolder( m_settings,_downloadLocation(),logger ) ;
		}
	}else{
		return _downloadFolder( m_settings,_downloadLocation(),logger ) ;
	}
}

QString settings::downloadFolder()
{
	return this->downloadFolder( nullptr ) ;
}

QString settings::downloadFolder( Logger& logger )
{
	return this->downloadFolder( &logger ) ;
}

bool settings::showTrayIcon()
{
	return _getOption( m_settings,"ShowTrayIcon",false ) ;
}

void settings::setshowTrayIcon( bool e )
{
	m_settings.setValue( "ShowTrayIcon",e ) ;
}

bool settings::autoDownload()
{
	return _getOption( m_settings,"AutoDownload",false ) ;
}

bool settings::downloadOptionsAsLast()
{
	return _getOption( m_settings,"DownloadOptionsAsLast",false ) ;
}

bool settings::autoDownloadWhenAddedInBatchDownloader()
{
	return _getOption( m_settings,"AutoDownloadWhenAddedInBatchDownloader",false ) ;
}

bool settings::showVersionInfoWhenStarting()
{
	return _getOption( m_settings,"ShowVersionInfoWhenStarting",true ) ;
}

bool settings::concurrentDownloading()
{
	return _getOption( m_settings,"ConcurrentDownloading",true ) ;
}

QString settings::cookieFilePath( const QString& engineName )
{
	auto m = "CookieFilePath_" + engineName ;

	return _getOption( m_settings,m,QString() ) ;
}

void settings::setCookieFilePath( const QString& engineName,const QString& cookieFilePath )
{
	m_settings.setValue( "CookieFilePath_" + engineName,cookieFilePath ) ;
}

void settings::setTheme( QApplication& app,const QString& themeBasePath )
{
	themes( this->themeName(),themeBasePath ).set( app ) ;
}

void settings::setUseSystemProvidedVersionIfAvailable( bool e )
{
	m_settings.setValue( "UseSystemProvidedVersionIfAvailable",e ) ;
}

void settings::setShowMetaDataInBatchDownloader( bool e )
{
	m_settings.setValue( "ShowMetaDataInBatchDownloader",e ) ;
}

bool settings::showMetaDataInBatchDownloader()
{
	return _getOption( m_settings,"ShowMetaDataInBatchDownloader",true ) ;
}

bool settings::saveHistory()
{
	return _getOption( m_settings,"SaveHistory",true ) ;
}

bool settings::playlistDownloaderSaveHistory()
{
	return _getOption( m_settings,"PlaylistDownloaderSaveHistory",true ) ;
}

bool settings::singleInstance()
{
	return _getOption( m_settings,"SingleInstance",true ) ;
}

void settings::setPlaylistDownloaderSaveHistory( bool e )
{
	m_settings.setValue( "PlaylistDownloaderSaveHistory",e ) ;
}

int settings::stringTruncationSize()
{
	return _getOption( m_settings,"StringTruncationSize",100 ) ;
}

int settings::historySize()
{
	return _getOption( m_settings,"HistorySize",10 ) ;
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

	return _getOption( m_settings,m,128 ) ;
}

int settings::thumbnailHeight( settings::tabName s )
{
	auto m = _thumbnailTabName( "ThumbnailHeight",s ) ;

	return _getOption( m_settings,m,72 ) ;
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
	return _getOption( m_settings,"YtDlpTextEncoding",QString() ) ;
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
	auto m = QDir::fromNativeSeparators( this->downloadFolder() ) ;

	return _getOption( m_settings,"LibraryDownloadFolder",m ) ;
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

	return _getOption( m_settings,m,engineName ) ;
}

QByteArray settings::highDpiScalingFactor()
{
	return _getOption( m_settings,"EnabledHighDpiScalingFactor",QByteArray( "1.0" ) ) ;
}

QPixmap settings::defaultVideoThumbnailIcon( settings::tabName m )
{
	auto width = this->thumbnailWidth( m ) ;
	auto height = this->thumbnailHeight( m ) ;

	return QIcon( ":/video" ).pixmap( width,height ) ;
}

void settings::setLibraryShowFolderFirst( bool e )
{
	m_settings.setValue( "LibraryShowFolderFirst",e ) ;
}

void settings::setLibraryArrangeAscending( bool e )
{
	m_settings.setValue( "LibraryArrangeAscending",e ) ;
}

void settings::setLibraryArrangeByDate( bool e )
{
	m_settings.setValue( "LibraryArrangeByDate",e ) ;
}

bool settings::libraryShowFolderFirst()
{
	return _getOption( m_settings,"LibraryShowFolderFirst",true ) ;
}

bool settings::libraryArrangeAscending()
{
	return _getOption( m_settings,"LibraryArrangeAscending",true ) ;
}

bool settings::libraryArrangeByDate()
{
	return _getOption( m_settings,"LibraryArrangeByDate",true ) ;
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
	return m_options.dataPath() ;
}

QString settings::commandOnSuccessfulDownload()
{
	return _getOption( m_settings,"CommandOnSuccessfulDownload",QString() ) ;
}

QString settings::commandWhenAllFinished()
{
	return _getOption( m_settings,"CommandWhenAllFinished",QString() ) ;
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

	return _getOption( m_settings,s,QString() ) ;
}

void settings::setLastUsedOption( const QString& m,const QString& e,settings::tabName s )
{
	m_settings.setValue( _getTabOption( m,s ),e ) ;
}

QString settings::localizationLanguagePath()
{
	if( utility::platformIsWindows() ){

		return m_options.windowsOnlyExePath() + "/translations" ;
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

	return _getOption( m_settings,m,QString() ) ;
}

QString settings::localizationLanguage()
{
	return _getOption( m_settings,"Language",QString( "en_US" ) ) ;
}

bool settings::portableVersion()
{
	return m_options.portableVersion() ;
}

settings::options::options( const utility::cliArguments& args )
{
	if( utility::platformIsWindows() ){

		m_exePath = utility::windowsApplicationDirPath() ;

		if( args.runningUpdated() ){

			m_dataPath = args.dataPath() ;

			m_defaultPortableVersionDownloadFolder = args.originalPath() + "/Downloads" ;

			m_portableVersion = args.portable() ;

			m_exe3PartyBinPath = m_exePath + "/3rdParty" ;

			if( !QFile::exists( m_exe3PartyBinPath ) ){

				m_exe3PartyBinPath = args.originalPath() + "/3rdParty" ;
			}

			const auto& m = args.originalVersion() ;
			const auto& mm = utility::runningVersionOfMediaDownloader() ;

			utility::setHelpVersionOfMediaDownloader( m + "/" + mm ) ;
		}else{
			auto a = m_exePath  + "/local" ;

			m_exe3PartyBinPath = m_exePath + "/3rdParty" ;

			m_defaultPortableVersionDownloadFolder = m_exePath + "/Downloads" ;

			if( utility::pathIsFolderAndExists( a ) ){

				m_dataPath = a ;

				m_portableVersion = true ;
			}else{
				m_dataPath = _configPath() ;

				m_portableVersion = false ;
			}
		}
	}else{
		m_dataPath = _configPath() ;

		m_portableVersion = false ;
	}
}

settings::proxySettings settings::getProxySettings()
{
	return { m_settings } ;
}

settings::proxySettings::proxySettings( QSettings& s ) : m_settings( s )
{
	if( !m_settings.contains( "ProxySettingsType" ) ){

		m_settings.setValue( "ProxySettingsType","None" ) ;
	}
}

settings::proxySettings& settings::proxySettings::setProxySettings( settings::proxySettings::Type s,const QString& e )
{
	if( s == settings::proxySettings::Type::none ){

		m_settings.setValue( "ProxySettingsType","None" ) ;

	}else if( s == settings::proxySettings::Type::system ){

		m_settings.setValue( "ProxySettingsType","System" ) ;

	}else if( s == settings::proxySettings::Type::env ){

		m_settings.setValue( "ProxySettingsType","Env" ) ;

	}else if( s == settings::proxySettings::Type::manual ){

		m_settings.setValue( "ProxySettingsType","Manual" ) ;

		m_settings.setValue( "ProxySettingsCustomSource",e.toUtf8() ) ;
	}

	return *this ;
}

settings::proxySettings::type settings::proxySettings::types() const
{
	auto m = m_settings.value( "ProxySettingsType" ).toString() ;

	if( m == "None" ){

		return settings::proxySettings::Type::none ;

	}else if( m == "System" ){

		return settings::proxySettings::Type::system ;

	}else if( m == "Env" ){

		return settings::proxySettings::Type::env ;

	}else if( m == "Manual" ){

		return settings::proxySettings::Type::manual ;
	}else{
		m_settings.setValue( "ProxySettingsType","None" ) ;

		return settings::proxySettings::Type::none ;
	}
}

QByteArray settings::proxySettings::proxyAddress() const
{
	return m_settings.value( "ProxySettingsCustomSource" ).toByteArray() ;
}

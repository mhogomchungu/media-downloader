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
#include "directoryEntries.h"

#include <QDir>
#include <QFile>

#include <cstring>
#include <algorithm>

#include <QDesktopServices>

#include <QPalette>
#include <QStyleHints>

QString settings::monitorClipboadUrl( settings::tabName e )
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
	m_settings.setValue( this->monitorClipboadUrl( t ),e ) ;
}

bool settings::monitorClipboardUrl( settings::tabName tabName )
{
	auto m = this->monitorClipboadUrl( tabName ) ;

	return this->getOption( m,false ) ;
}

QString settings::themeName()
{
	return this->getOption( "ThemeName",themes().unTranslatedAt( 0 ) ) ;
}

void settings::setThemeName( const QString& e )
{
	m_settings.setValue( "ThemeName",e ) ;
}

QString settings::setOptionWithEngineName( const QString& opts,const QString& engineName )
{
	return opts + "-" + engineName ;
}

QString settings::getOptionsHistoryTabName( settings::tabName e,const QString& engineName )
{
	if( e == settings::tabName::basic ){

		return this->setOptionWithEngineName( "BasicDownloaderOptionsHistory",engineName ) ;

	}else if( e == settings::tabName::batch ){

		return this->setOptionWithEngineName( "BatchDownloaderOptionsHistory",engineName ) ;

	}else if( e == settings::tabName::playlist ){

		return this->setOptionWithEngineName( "PlaylistDownloaderOptionsHistory",engineName ) ;
	}else{
		return "" ;
	}
}

QStringList settings::getOptionsHistory( settings::tabName e,const QString& engineName )
{
	auto m = this->getOptionsHistoryTabName( e,engineName ) ;

	return this->getOption( m,QStringList() ) ;
}

void settings::clearOptionsHistory( settings::tabName e,const QString& engineName )
{
	m_settings.setValue( this->getOptionsHistoryTabName( e,engineName ),QStringList() ) ;
}

void settings::addToHistory( QSettings& settings,
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

void settings::addToplaylistRangeHistory( const QString& engineName,const QString& e )
{
	if( this->saveHistory() ){

		auto a = this->playlistRangeHistory( engineName ) ;
		auto b = this->setOptionWithEngineName( "PlaylistRangeHistory",engineName ) ;

		this->addToHistory( m_settings,a,b,e,this->historySize() ) ;
	}
}

void settings::addOptionsHistory( const QString& engineName,const QString& e,settings::tabName s )
{
	if( this->saveHistory() ){

		auto a = this->getOptionsHistory( s,engineName ) ;
		auto b = this->getOptionsHistoryTabName( s,engineName ) ;

		this->addToHistory( m_settings,a,b,e,this->historySize() ) ;
	}
}

void settings::clearPlaylistRangeHistory( const QString& engineName )
{
	QStringList s ;

	if( engineName.startsWith( "gallery-dl" ) ){

		m_settings.setValue( this->setOptionWithEngineName( "PlaylistRangeHistory",engineName ),s ) ;
	}else{
		s.append( "--break-on-existing" ) ;

		m_settings.setValue( this->setOptionWithEngineName( "PlaylistRangeHistory",engineName ),s ) ;
	}
}

void settings::clearPlaylistUrlHistory( const QString& engineName )
{
	m_settings.setValue( this->setOptionWithEngineName( "PlaylistUrlHistory",engineName ),QStringList() ) ;
}

void settings::setAutoSavePlaylistOnExit( bool e )
{
	m_settings.setValue( "AutoSavePlaylistOnExit",e ) ;
}

bool settings::autoSavePlaylistOnExit()
{
	return this->getOption( "AutoSavePlaylistOnExit",true ) ;
}

bool settings::useInternalArchiveFile()
{
	return this->getOption( "UseInternalArchiveFile",true ) ;
}

bool settings::enableLibraryTab()
{
	return this->getOption( "EnableLibraryTab",true ) ;
}

bool settings::printMediaPlayers()
{
	return m_printMediaPlayers ;
}

bool settings::checkForEnginesUpdates()
{
	return this->getOption( "CheckForEnginesUpdates",true ) ;
}

bool settings::autoHideDownloadWhenCompleted()
{
	return this->getOption( "AutoHideDownloadWhenCompleted",false ) ;
}

bool settings::deleteFilesOnCanceledDownload()
{
	return this->getOption( "DeleteFilesOnCanceledDownload",false ) ;
}

bool settings::autoSetDefaultEngineAndOptions()
{
	return this->getOption( "AutoSetDefaultEngineAndOptions",false ) ;
}

bool settings::cookieSourceSetToBrowerName()
{
	return this->getOption( "CookieSourceSetToBrowerName",true ) ;
}

void settings::setCookieSourceSetToBrowerName( bool e )
{
	m_settings.setValue( "CookieSourceSetToBrowerName",e ) ;
}

qint64 settings::timeOutWaitingForClipboardData()
{
	return this->getOption( "TimeOutWaitingForClipboardData",5000 ) ;
}

void settings::setAutoHideDownloadWhenCompleted( bool e )
{
	m_settings.setValue( "AutoHideDownloadWhenCompleted",e ) ;
}

Qt::Alignment settings::textAlignment()
{
	auto m = this->getOption( "MainTableTextAlignment",QString( "center" ) ) ;

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
	return this->getOption( "NetworkTimeOutInSeconds",30 )  * 1000 ;
}

QStringList settings::playlistRangeHistory( const QString& engineName )
{
	QStringList s ;

	if( !engineName.startsWith( "gallery-dl" ) ){

		s.append( "--break-on-existing" ) ;
	}

	return this->getOption( this->setOptionWithEngineName( "PlaylistRangeHistory",engineName ),s ) ;
}

QStringList settings::playlistUrlHistory( const QString& engineName )
{
	return this->getOption( this->setOptionWithEngineName( "PlaylistUrlHistory",engineName ),QStringList() ) ;
}

void settings::setPlaylistRangeHistoryLastUsed( const QString& engineName,const QString& e )
{
	m_settings.setValue( this->setOptionWithEngineName( "playlistRangeHistoryLastUsed",engineName ),e ) ;
}

QString settings::playlistRangeHistoryLastUsed( const QString& engineName )
{
	QString s ;

	if( !engineName.startsWith( "gallery-dl" ) ){

		s = "--break-on-existing" ;
	}

	return this->getOption( this->setOptionWithEngineName( "playlistRangeHistoryLastUsed",engineName ),s ) ;
}

QString settings::gitHubDownloadUrl()
{
	QString channel = utility::runningGitVersion() ? "git" : "release" ;

	auto m = this->getOption( "WindowsUpdateChannel",channel ) ;

	const auto& e = utility::fakeRunningVersionOfMediaDownloader() ;

	if( !e.isEmpty() ){

		m = utility::runningGitVersion( e ) ? "git" : "release" ;
	}

	if( m.compare( "release",Qt::CaseInsensitive ) == 0 ){

		return "https://api.github.com/repos/mhogomchungu/media-downloader/releases/latest" ;
	}else{
		return "https://api.github.com/repos/mhogomchungu/media-downloader-git/releases/latest" ;
	}
}

std::unique_ptr< QSettings > settings::setConfig( const QString& path )
{
	QDir().mkpath( path + "/settings" ) ;

	auto m = path + "/settings/settings.ini" ;

	return std::make_unique< QSettings >( m,QSettings::IniFormat ) ;
}

std::unique_ptr< QSettings > settings::init()
{	
	if( utility::platformIsWindows() ){

		if( m_options.portableVersion() ){

			return this->setConfig( m_options.dataPath() ) ;
		}else{
			return this->setConfig( m_appDataPath ) ;
		}
	}else{
		if( QFile::exists( m_appDataPath + "/settings/settings.ini" ) ){

			return this->setConfig( m_appDataPath ) ;
		}else{
			/*
			 * Migrating to .ini config file
			 */
			QSettings oldSettings( "media-downloader","media-downloader" ) ;

			auto newSettings = this->setConfig( m_appDataPath ) ;

			const auto keys = oldSettings.allKeys() ;

			for( const auto& it : keys ){

				newSettings->setValue( it,oldSettings.value( it ) ) ;
			}

			oldSettings.clear() ;

			return newSettings ;
		}
	}
}

#if QT_VERSION >= QT_VERSION_CHECK( 5,6,0 )

QString settings::appDataLocation()
{
	auto s = QStandardPaths::standardLocations( QStandardPaths::AppDataLocation ) ;

	if( s.isEmpty() ){

		return QDir::homePath() + "/.local/share/media-downloader/" ;
	}else{
		return s.first() + "/media-downloader/" ;
	}
}

QString settings::downloadLocation()
{
	auto s = QStandardPaths::standardLocations( QStandardPaths::DownloadLocation ) ;

	if( s.isEmpty() ){

		return QDir::homePath() + "/Downloads" ;
	}else{
		return s.first() ;
	}
}

#else

QString settings::appDataLocation()
{
	return QDir::homePath() + "/.local/share/media-downloader/" ;
}

QString settings::downloadLocation()
{
	return QDir::homePath() + "/Downloads" ;
}

#endif

settings::settings( const utility::cliArguments& args ) :
	m_printMediaPlayers( args.printMediaPlayers() ),
	m_appDataPath( this->appDataLocation() ),
	m_options( args,m_appDataPath ),
	m_settingsP( this->init() ),
	m_settings( *m_settingsP ),
	m_flatpakRuntimeOptions( *this )
{
#if QT_VERSION >= QT_VERSION_CHECK( 5,6,0 )

	m_EnableHighDpiScaling = true ;
	#if QT_VERSION < QT_VERSION_CHECK( 6,0,0 )
		QApplication::setAttribute( Qt::AA_EnableHighDpiScaling ) ;
	#endif
#else
	m_EnableHighDpiScaling = false ;
#endif
	m_MdScaleFactor = this->highDpiScalingFactor() ;

	m_defaultScaleFactor = qgetenv( "QT_SCALE_FACTOR" ) ;

	if( !m_MdScaleFactor.isEmpty() ){

		qputenv( "QT_SCALE_FACTOR",m_MdScaleFactor ) ;
	}
}

void settings::openUrl( const QString& e )
{
	auto m = QUrl::fromLocalFile( e ) ;

	if( m_MdScaleFactor.isEmpty() ){

		QDesktopServices::openUrl( m ) ;
	}else{
		if( m_defaultScaleFactor.isEmpty() ){

			qunsetenv( "QT_SCALE_FACTOR" ) ;
		}else{
			qputenv( "QT_SCALE_FACTOR",m_defaultScaleFactor ) ;
		}

		QDesktopServices::openUrl( m ) ;

		qputenv( "QT_SCALE_FACTOR",m_MdScaleFactor ) ;
	}
}

settings::~settings()
{
	this->clearFlatPakTemps() ;
}

QSettings& settings::bk()
{
	return m_settings ;
}

void settings::init_done()
{
	class meaw
	{
	public:
		meaw( settings& s ) : m_parent( s )
		{
		}
		void bg()
		{
			if( utility::platformIsWindows() ){

				const auto& m = m_parent.m_options.pathToOldUpdatedVersion() ;

				if( !m.isEmpty() ){

					QDir( m ).removeRecursively() ;
				}

			}else if( utility::platformisFlatPak() ){

				m_parent.clearFlatPakTemps() ;

				m_parent.flatpakIntance().getVLC().checkAvailability() ;
			}
		}
		void fg()
		{
		}
	private:
		settings& m_parent ;
	} ;

	utils::qthread::run( meaw( *this ) ) ;
}

void settings::setTabNumber( int s )
{
	m_settings.setValue( "TabNumber",s ) ;
}

void settings::saveMainWindowDimensions( const QRect& s )
{
	auto x = QString::number( s.x() ) ;
	auto y = QString::number( s.y() ) ;
	auto w = QString::number( s.width() ) ;
	auto h = QString::number( s.height() ) ;

	auto m = QString( "%1 %2 %3 %4" ).arg( x,y,w,h ) ;

	m_settings.setValue( "MainWindowDimensions",m ) ;
}

void settings::setMainWindowDimensions( QWidget * s )
{
	if( m_settings.contains( "MainWindowDimensions" ) ){

		auto e = m_settings.value( "MainWindowDimensions" ).toString() ;

		auto m = util::split( e," " ) ;

		if( m.size() == 4 ){

			auto x = m[ 0 ].toInt() ;
			auto y = m[ 1 ].toInt() ;
			auto w = m[ 2 ].toInt() ;
			auto h = m[ 3 ].toInt() ;

			s->setGeometry( { x,y,w,h } ) ;

			s->setFixedSize( s->size() ) ;
		}
	}
}

int settings::tabNumber()
{
	return this->getOption( "TabNumber",0 ) ;
}

int settings::maxLoggerProcesses()
{
	return this->getOption( "MaxLoggerProcesses",0 ) ;
}

size_t settings::maxConcurrentDownloads()
{
	auto m = this->getOption( "MaxConcurrentDownloads",4 ) ;

	return static_cast< size_t >( m ) ;
}

bool settings::darkTheme()
{
	const QPalette defaultPalette ;
	const auto text = defaultPalette.color( QPalette::WindowText ) ;
	const auto window = defaultPalette.color( QPalette::Window ) ;
	return text.lightness() > window.lightness() ;
}

QIcon settings::getIcon( const QString& e )
{
	if( e == "media-downloader" ){

		auto m = "io.github.mhogomchungu.media-downloader" ;

		return QIcon::fromTheme( m,QIcon( ":/media-downloader" ) ) ;

	}else if( this->darkTheme() ){

		auto m = "io.github.mhogomchungu.media-downloader_white_" + e ;

		return QIcon::fromTheme( m,QIcon( ":/icons/white/" + e ) ) ;
	}else{
		auto m = "io.github.mhogomchungu.media-downloader_black_" + e ;

		return QIcon::fromTheme( m,QIcon( ":/icons/black/" + e ) ) ;
	}
}

const QString& settings::windowsOnly3rdPartyBinPath()
{
	return m_options.windowsOnly3rdPartyBinPath() ;
}

const QString& settings::windowsOnlyExeBinPath()
{
	return m_options.windowsOnlyExePath() ;
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

QString settings::downloadFolder( const QString& defaultPath,settings::sLogger& logger )
{
	auto mediaDownloaderCWD = utility::stringConstants::mediaDownloaderCWD() ;

	auto mm = utility::stringConstants::mediaDownloaderDefaultDownloadPath() ;

	if( !m_settings.contains( "DownloadFolder" ) ){

		m_settings.setValue( "DownloadFolder",mm ) ;
	}

	auto m = m_settings.value( "DownloadFolder" ).toString() ;

	if( m.startsWith( mediaDownloaderCWD ) ){

		m.replace( mediaDownloaderCWD,QDir::currentPath() ) ;

	}else if( m.startsWith( mm ) ){

		m.replace( mm,defaultPath ) ;
	}

	if( QFile::exists( m ) ){

		return m ;
	}else{
		auto id = utility::sequentialID() ;

		auto s = utility::barLine() ;

		logger.add( s,id ) ;

		logger.add( QObject::tr( "Resetting download folder to default" ).toUtf8(),id ) ;

		logger.add( s,id ) ;

		m_settings.setValue( "DownloadFolder",mm ) ;

		QDir().mkpath( defaultPath ) ;

		return defaultPath ;
	}
}

QString settings::downloadFolderImp( settings::sLogger logger )
{
	if( utility::platformIsWindows() ){

		if( this->portableVersion() ){

			const auto& dPath = this->windowsOnlyDefaultPortableVersionDownloadFolder() ;

			return this->downloadFolder( dPath,logger ) ;
		}else{
			return this->downloadFolder( this->downloadLocation(),logger ) ;
		}
	}else{
		return this->downloadFolder( this->downloadLocation(),logger ) ;
	}
}

QString settings::downloadFolder()
{
	return this->downloadFolderImp( {} ) ;
}

QString settings::downloadFolder( Logger& logger )
{
	return this->downloadFolderImp( logger ) ;
}

bool settings::showTrayIcon()
{
	return this->getOption( "ShowTrayIcon",false ) ;
}

void settings::setshowTrayIcon( bool e )
{
	m_settings.setValue( "ShowTrayIcon",e ) ;
}

bool settings::autoDownload()
{
	return this->getOption( "AutoDownload",false ) ;
}

bool settings::downloadOptionsAsLast()
{
	return this->getOption( "DownloadOptionsAsLast",false ) ;
}

bool settings::autoDownloadWhenAddedInBatchDownloader()
{
	return this->getOption( "AutoDownloadWhenAddedInBatchDownloader",false ) ;
}

void settings::setAutoDownloadWhenAddedInBatchDownloader( bool e )
{
	m_settings.setValue( "AutoDownloadWhenAddedInBatchDownloader",e ) ;
}

bool settings::showVersionInfoAndAutoDownloadUpdates()
{
	return this->getOption( "ShowVersionInfoAndAutoDownloadUpdates",true ) ;
}

bool settings::showLocalAndLatestVersionInformation()
{
	return this->getOption( "ShowLocalAndLatestVersionInformation",false ) ;
}

bool settings::showLocalVersionInformationOnly()
{
	return this->getOption( "ShowLocalVersionInformationOnly",false ) ;
}

bool settings::concurrentDownloading()
{
	return this->getOption( "ConcurrentDownloading",true ) ;
}

QString settings::cookieBrowserName( const QString& engineName )
{
	auto m = "CookieBrowser_" + engineName ;

	return this->getOption( m,QString() ) ;
}

QString settings::cookieBrowserTextFilePath( const QString& engineName )
{
	auto m = "CookieTextFilePath_" + engineName ;

	return this->getOption( m,QString() ) ;
}

void settings::setCookieBrowserName( const QString& engineName,const QString& browserName )
{
	m_settings.setValue( "CookieBrowser_" + engineName,browserName ) ;
}

void settings::setCookieTextFilePath(const QString & engineName,const QString& cookiePath )
{
	m_settings.setValue( "CookieTextFilePath_" + engineName,cookiePath ) ;
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
	return this->getOption( "ShowMetaDataInBatchDownloader",true ) ;
}

bool settings::saveHistory()
{
	return this->getOption( "SaveHistory",true ) ;
}

bool settings::playlistDownloaderSaveHistory()
{
	return this->getOption( "PlaylistDownloaderSaveHistory",true ) ;
}

bool settings::singleInstance()
{
	return this->getOption( "SingleInstance",true ) ;
}

void settings::setPlaylistDownloaderSaveHistory( bool e )
{
	m_settings.setValue( "PlaylistDownloaderSaveHistory",e ) ;
}

int settings::stringTruncationSize()
{
	return this->getOption( "StringTruncationSize",100 ) ;
}

int settings::historySize()
{
	return this->getOption( "HistorySize",10 ) ;
}

QString settings::thumbnailTabName( const QString& s, settings::tabName e )
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
	auto m = this->thumbnailTabName( "ThumbnailWidth",s ) ;

	return this->getOption( m,128 ) ;
}

int settings::thumbnailHeight( settings::tabName s )
{
	auto m = this->thumbnailTabName( "ThumbnailHeight",s ) ;

	return this->getOption( m,72 ) ;
}

int settings::desktopNotificationTimeOut()
{
	return this->getOption( "DesktopNotificationTimeOut",10000 ) ;
}

void settings::setOpenWith( const QString& e )
{
	m_settings.setValue( "OpenWith",e ) ;
}

settings::mediaPlayer settings::openWith( Logger& logger )
{	
	static auto s = this->openWith() ;

	return { *this,s,logger } ;
}

std::vector< settings::mediaPlayer::PlayerOpts > settings::openWith()
{
	std::vector< settings::mediaPlayer::PlayerOpts > ss ;

	for( auto& it : utility::getMediaPlayers() ){

		ss.emplace_back( std::move( it.exePath ),std::move( it.name ) ) ;
	}

	auto mm = this->getOption( "OpenWith",QString() ) ;

	if( mm.isEmpty() ){

		return ss ;
	}

	auto m = util::split( mm,":" ) ;

	if( m.size() < 2 ){

		return ss ;
	}

	auto name = m[ 0 ] ;

	using pl = settings::mediaPlayer::PlayerOpts ;

	auto it = std::find_if( ss.begin(),ss.end(),[ & ]( const pl& p ){

		return QString::compare( p.name,name,Qt::CaseInsensitive ) == 0 ;
	} ) ;

	if( it == ss.end() ){

		if( utility::platformIsWindows() ){

			auto mm = util::join( m,1,":" ) ;

			if( QFile::exists( mm ) ){

				ss.insert( ss.begin(),{ mm,name } ) ;
			}
		}else{
			mm = QStandardPaths::findExecutable( util::join( m,1,":" ) ) ;

			if( !mm.isEmpty() ){

				ss.insert( ss.begin(),{ mm,name } ) ;
			}
		}
	}

	return ss ;
}

void settings::setShowLocalVersionInformationOnly( bool e )
{
	m_settings.setValue( "ShowLocalVersionInformationOnly",e ) ;
}

void settings::setShowLocalAndLatestVersionInformation( bool e )
{
	m_settings.setValue( "ShowLocalAndLatestVersionInformation",e ) ;
}

void settings::setShowVersionInfoAndAutoDownloadUpdates( bool e )
{
	m_settings.setValue( "ShowVersionInfoAndAutoDownloadUpdates",e ) ;
}

void settings::setHighDpiScalingFactor( const QString& m )
{
	m_settings.setValue( "EnabledHighDpiScalingFactor",m ) ;
}

QString settings::textEncoding( const QString& engineName )
{
	auto m = m_settings.value( "YtDlpTextEncoding" ).toString() ;

	return this->getOption( "TextEncoding_" + engineName,m ) ;
}

void settings::setTextEncoding( const QString& e,const QString& engineName )
{
	m_settings.setValue( "TextEncoding_" + engineName,e ) ;
}

void settings::setlibraryDownloadFolder( const QString& e )
{
	m_settings.setValue( "LibraryDownloadFolder",QDir::fromNativeSeparators( e ) ) ;
}

QString settings::libraryDownloadFolder()
{
	auto m = QDir::fromNativeSeparators( this->downloadFolder() ) ;

	return this->getOption( "LibraryDownloadFolder",m ) ;
}

QString settings::getDefaultEngineName( settings::tabName e )
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
	m_settings.setValue( this->getDefaultEngineName( n ),e ) ;
}

QString settings::defaultEngine( settings::tabName n,const QString& engineName )
{
	auto m = this->getDefaultEngineName( n ) ;

	return this->getOption( m,engineName ) ;
}

QByteArray settings::highDpiScalingFactor()
{
	auto m = this->getOption( "EnabledHighDpiScalingFactor",QString( "1.0" ) ) ;

	if( m == "1.0" ){

		return {} ;
	}else{
		return m.toUtf8() ;
	}
}

QPixmap settings::defaultVideoThumbnailIcon( settings::tabName m )
{
	auto width = this->thumbnailWidth( m ) ;
	auto height = this->thumbnailHeight( m ) ;

	return this->getIcon( "video" ).pixmap( width,height ) ;
}

settings::LogsLimits settings::getLogsLimits()
{
	return { 4096,2048 } ;
}

void settings::setDesktopNotifyOnDownloadComplete( bool e )
{
	m_settings.setValue( "DesktopNotifyOnDownloadComplete",e ) ;
}

void settings::setDesktopNotifyOnAllDownloadComplete( bool e )
{
	m_settings.setValue( "DesktopNotifyOnAllDownloadComplete",e ) ;
}

bool settings::desktopNotifyOnDownloadComplete()
{
	return this->getOption( "DesktopNotifyOnDownloadComplete",false ) ;
}

bool settings::desktopNotifyOnAllDownloadComplete()
{
	return this->getOption( "DesktopNotifyOnAllDownloadComplete",false ) ;
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
	return this->getOption( "LibraryShowFolderFirst",true ) ;
}

bool settings::libraryArrangeAscending()
{
	return this->getOption( "LibraryArrangeAscending",true ) ;
}

bool settings::libraryArrangeByDate()
{
	return this->getOption( "LibraryArrangeByDate",true ) ;
}

bool settings::enabledHighDpiScaling()
{
	return m_EnableHighDpiScaling ;
}

QStringList settings::directoryList( const QString& e )
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

	const auto e = this->directoryList( this->localizationLanguagePath() ) ;

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

const QString& settings::appDataPath()
{
	return m_appDataPath ;
}

void settings::runCommandOnSuccessfulDownload( const QString& s,
					       const QString& df,
					       const std::vector< QByteArray >& e )
{
	auto m = this->getOption( "CommandOnSuccessfulDownload",QString() ) ;

	if( !m.isEmpty() && e.size() ){

		auto args = util::splitPreserveQuotes( m ) ;

		auto exe = args.at( 0 ) ;

		args.replace( 0,s ) ;

		for( const auto& it : e ){

			auto m = df + it ;

			if( QFile::exists( m ) ){

				args.append( m ) ;
			}
		}

		QProcess::startDetached( exe,args ) ;
	}
}

QString settings::commandWhenAllFinished()
{
	return this->getOption( "CommandWhenAllFinished",QString() ) ;
}

QString settings::getTabOption( const QString& s,settings::tabName e )
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
	auto s = this->getTabOption( m,e ) ;

	return this->getOption( s,QString() ) ;
}

void settings::setLastUsedOption( const QString& m,const QString& e,settings::tabName s )
{
	m_settings.setValue( this->getTabOption( m,s ),e ) ;
}

QString settings::localizationLanguagePath()
{
	if( utility::platformIsWindows() ){

		return m_options.windowsOnlyExePath() + "/translations" ;

	}else if( utility::platformIsOSX() ){

		return utility::OSXtranslationFilesPath() ;
	}else{
		auto m = QCoreApplication::applicationDirPath() ;

		return m + "/../share/media-downloader/translations/" ;
	}
}

void settings::setLocalizationLanguage( const QString& language )
{
	m_settings.setValue( "Language",language ) ;
}

void settings::setWindowDimensions( const QString& window,const QString& dimenstion )
{
	m_settings.setValue( "WindowDimensions_" + window,dimenstion ) ;
}

void settings::clearFlatPakTemps()
{
	if( utility::platformisFlatPak() ){

		auto ee = m_appDataPath + "tmp" ;

		directoryManager::readAll( ee ).forEachFile( [ & ]( const QString& e ){

			if( e.endsWith( ".m3u8" ) ){

				QFile::remove( ee + "/" + e ) ;
			}
		} ) ;
	}
}

QString settings::windowsDimensions( const QString& window )
{
	auto m = "WindowDimensions_" + window ;

	return this->getOption( m,QString() ) ;
}

QString settings::localizationLanguage()
{
	return this->getOption( "Language",QString( "en_US" ) ) ;
}

bool settings::portableVersion()
{
	return m_options.portableVersion() ;
}

settings::options::options( const utility::cliArguments& args,const QString& appPath )
{
	if( utility::platformIsWindows() ){

		m_exePath = utility::windowsApplicationDirPath() ;

		if( args.runningUpdated() ){

			m_pathToOldUpdatedVersion = args.pathToOldUpdatedVersion() ;

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
				m_dataPath = appPath ;

				m_portableVersion = false ;
			}
		}
	}else{
		m_dataPath = appPath ;

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

settings::mediaPlayer::mediaPlayer( settings& e,
				   const std::vector< settings::mediaPlayer::PlayerOpts >& s,
				   Logger& logger ) :
	m_playerOpts( s ),
	m_logger( logger ),
	m_settings( e )
{
}

void settings::mediaPlayer::action::logError() const
{
	auto id = utility::sequentialID() ;

	auto bar = utility::barLine() ;

	auto m = QObject::tr( "Failed To Start Executable %1" ) ;

	m_logger.add( bar,id ) ;

	m_logger.add( m.arg( m_playerOpts.name ),id ) ;

	m_logger.add( bar,id ) ;
}

void settings::mediaPlayer::action::run( const QString& exe,const QStringList& args ) const
{
	if( !QProcess::startDetached( exe,args ) ){

		this->logError() ;
	}
}

QByteArray settings::hash( quint64 i,const QString& s )
{
	auto m = utility::simpleRandomNumber() ;

	auto e = QString::number( m + i ) + s ;

	QCryptographicHash hash( QCryptographicHash::Sha256 ) ;

	hash.addData( e.toUtf8() ) ;

	return hash.result().toHex().mid( 0,8 ) ;
}

QString settings::tmpFile( const QString& e,const QString& s )
{
	QString m ;

	for( quint64 i = 0 ; i < 100 ; i++ ){

		m = "tmp/" + this->hash( i,s ) + ".m3u8" ;

		if( e.endsWith( "/" ) ){

			m = e + m ;
		}else{
			m = e + "/" + m ;
		}

		if( !QFile::exists( m ) ){

			break ;
		}
	}

	return m ;
}

QStringList settings::mediaPlayer::action::setVLCoptions( const QStringList& m ) const
{
	QStringList urls ;

	urls.append( m[ 0 ] ) ;

	urls.append( "--input-slave" ) ;

	QString u = m[ 1 ] ;

	for( int s = 2 ; s < m.size() ; s++ ){

		u += "#" + m[ s ] ;
	}

	urls.append( u ) ;

	return urls ;
}

void settings::mediaPlayer::action::operator()() const
{
	if( utility::platformisFlatPak() ){

		if( m_playerOpts.exePath == "vlc" ){

			const auto& f = m_settings.flatpakIntance().getVLC() ;

			if( f.valid() ){

				auto m = f.args() ;

				if( m_urls.size() > 1 ){

					m.append( this->setVLCoptions( m_urls ) ) ;
				}else{
					m.append( m_urls ) ;
				}

				this->run( f.exe(),m ) ;
			}else{
				this->logError() ;
			}
		}else{
			auto urls = m_urls.join( "\n" ) ;

			auto m = m_settings.tmpFile( m_appDataPath,urls ) ;

			QFile ff( m ) ;

			if( ff.open( QIODevice::WriteOnly ) ){

				auto duration = m_obj.value( "duration" ).toString().toUtf8() ;
				auto title    = m_obj.value( "title" ).toString().toUtf8() ;

				QByteArray aa = "#EXTM3U\n\n" ;

				if( duration != "0" && !title.contains( "NA" ) ){

					aa += "#EXTINF:" + duration + ", " + title + "\n" ;
				}

				ff.write( aa + urls.toUtf8() + "\n" ) ;

				ff.close() ;

				QDesktopServices::openUrl( QUrl::fromLocalFile( m ) ) ;
			}
		}

	}else if( m_playerOpts.exePath.contains( "vlc",Qt::CaseInsensitive ) ){

		if( m_urls.size() > 1 ){

			this->run( m_playerOpts.exePath,this->setVLCoptions( m_urls ) ) ;
		}else{
			this->run( m_playerOpts.exePath,m_urls ) ;
		}
	}else{
		this->run( m_playerOpts.exePath,m_urls ) ;
	}
}

void settings::sLogger::add( const QByteArray& data,int id )
{
	if( m_logger ){

		m_logger->add( data,id ) ;
	}
}

settings::flatpakRuntimeOptions::flatpakRuntimeOptions( settings& s ) :
	m_settings( this->flatpkakInfoFile(),QSettings::IniFormat ),
	m_globalBinPath( m_settings.value( "Instance/app-path" ).toString() ),
	m_architecture( m_settings.value( "Instance/arch" ).toString() ),
	m_commitId( m_settings.value( "Instance/app-commit" ).toString() ),
	m_runtimePath( m_settings.value( "Instance/runtime-path" ).toString() ),
	m_localBinPath( "/app/bin" ),
	m_appDataLocation( s.appDataLocation() )
{
	if( !m_globalBinPath.isEmpty() ){

		m_globalBinPath += "/bin" ;
	}
}

QString settings::flatpakRuntimeOptions::flatpkakInfoFile()
{
	if( QFile::exists( "/.flatpak-info" ) ){

		return "/.flatpak-info" ;
	}else{
		auto m = qgetenv( "XDG_RUNTIME_DIR" ) ;

		if( m.isEmpty() ){

			//???
			return {} ;
		}else{
			return m + "/flatpak-info" ;
		}
	}
}

const settings::flatpakRuntimeOptions& settings::flatpakIntance()
{
	return m_flatpakRuntimeOptions ;
}

const settings::flatpakRuntimeOptions::VLC& settings::flatpakRuntimeOptions::getVLC() const
{
	return m_vlc ;
}

void settings::flatpakRuntimeOptions::VLC::checkAvailability() const
{
	if( this->checkAvailability( { "--host","vlc" } ) ){

		this->checkAvailability( { "--host","flatpak","run","org.videolan.VLC" } ) ;
	}
}

bool settings::flatpakRuntimeOptions::VLC::checkAvailability( const QStringList& e ) const
{
	QProcess exe ;

	exe.start( "flatpak-spawn",e + QStringList{ "--version" } ) ;

	exe.waitForFinished() ;

	if( exe.exitCode() == 0 && exe.exitStatus() == QProcess::ExitStatus::NormalExit ){

		m_args = e ;

		return false ;
	}else{
		return true ;
	}
}

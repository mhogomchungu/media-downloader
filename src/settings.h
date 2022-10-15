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

#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>
#include <QString>
#include <QStringList>
#include <QByteArray>
#include <QComboBox>
#include <QPixmap>
#include <QDir>
#include <QApplication>

#include <vector>
#include <memory>

#include <QStandardPaths>

class Logger ;
class QApplication ;

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

class settings
{
public:	
	enum class tabName{ basic,batch,playlist } ;

	settings( int,char ** ) ;

	QSettings& bk() ;

	size_t maxConcurrentDownloads() ;

	QString downloadFolder() ;
	QString libraryDownloadFolder() ;
	QString downloadFolder( Logger& ) ;
	QString localizationLanguagePath() ;
	QString localizationLanguage() ;
	QString commandOnSuccessfulDownload() ;
	QString commandWhenAllFinished() ;
	QString themeName() ;
	QString defaultEngine( settings::tabName,const QString& ) ;
	QString cookieFilePath( const QString& engineName ) ;
	QString windowsDimensions( const QString& windowName ) ;
	QString playlistRangeHistoryLastUsed() ;
	QString configPaths() ;
	QString updatedVersionPath() ;
	QStringList getOptionsHistory( settings::tabName ) ;
	QStringList playlistRangeHistory() ;
	QStringList playlistUrlHistory() ;

	QString lastUsedOption( const QString&,settings::tabName ) ;

	QStringList localizationLanguages() ;

	QByteArray highDpiScalingFactor() ;

	QPixmap defaultVideoThumbnailIcon( settings::tabName ) ;

	static bool portableVersion() ;
	static QString portableVersionConfigPath() ;

	bool monitorClipboardUrl( settings::tabName ) ;
	bool enabledHighDpiScaling() ;
	bool showTrayIcon() ;
	bool autoDownload() ;
	bool showVersionInfoWhenStarting() ;
	bool concurrentDownloading() ;
	bool useSystemProvidedVersionIfAvailable() ;
	bool showThumbnails() ;
	bool saveHistory() ;
	bool playlistDownloaderSaveHistory() ;
	bool singleInstance() ;
	bool autoSavePlaylistOnExit() ;
	bool useInternalArchiveFile() ;
	bool checkForUpdates() ;
	bool enableLibraryTab() ;

	int networkTimeOut() ;
	int stringTruncationSize() ;
	int historySize() ;
	int tabNumber() ;
	int maxLoggerProcesses() ;
	int thumbnailWidth( settings::tabName ) ;
	int thumbnailHeight( settings::tabName ) ;

	void setCheckForUpdates( bool ) ;
	void setUseInternalArchiveFile( bool ) ;
	void clearOptionsHistory( settings::tabName ) ;
	void addToplaylistRangeHistory( const QString& ) ;
	void clearPlaylistRangeHistory() ;
	void clearPlaylistUrlHistory() ;
	void setAutoSavePlaylistOnExit( bool ) ;
	void addOptionsHistory( const QString&,settings::tabName ) ;
	void setshowTrayIcon( bool ) ;
	void setTheme( QApplication&,const QString& ) ;
	void setUseSystemProvidedVersionIfAvailable( bool ) ;
	void setMaxConcurrentDownloads( int ) ;
	void setTabNumber( int ) ;
	void setEnableLibraryTab( bool ) ;
	void setMonitorClipboardUrl( bool,settings::tabName ) ;
	void setShowThumbnails( bool ) ;
	void setPlaylistDownloaderSaveHistory( bool ) ;
	void setShowVersionInfoWhenStarting( bool ) ;
	void setThemeName( const QString& ) ;
	void setPlaylistRangeHistoryLastUsed( const QString& ) ;
	void setHighDpiScalingFactor( const QString& ) ;
	void setlibraryDownloadFolder( const QString& ) ;
	void setCookieFilePath( const QString& engineName,const QString& cookieFilePath ) ;
	void setDefaultEngine( const QString&,settings::tabName ) ;
	void setLastUsedOption( const QString& engineName,const QString& options,settings::tabName ) ;
	void setDownloadFolder( const QString& ) ;
	void setLocalizationLanguage( const QString& language ) ;
	void setWindowDimensions( const QString& window,const QString& dimenstion ) ;
private:
	bool m_EnableHighDpiScaling ;
	std::unique_ptr< QSettings > m_settingsP ;
	QSettings& m_settings ;
	bool m_portableVersion ;
};

#endif

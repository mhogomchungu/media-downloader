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

namespace utility
{
	class cliArguments ;
}
class settings
{
public:	
	class proxySettings
	{
	public:
		enum class Type{ system,env,manual,none } ;

		class type
		{
		public:
			type( settings::proxySettings::Type s ) : m_type( s )
			{
			}
			bool none() const
			{
				return m_type == settings::proxySettings::Type::none ;
			}
			bool system() const
			{
				return m_type == settings::proxySettings::Type::system ;
			}
			bool env() const
			{
				return m_type == settings::proxySettings::Type::env ;
			}
			bool manual() const
			{
				return m_type == settings::proxySettings::Type::manual ;
			}
		private:
			settings::proxySettings::Type m_type ;
		} ;
		proxySettings( QSettings& ) ;
		proxySettings& setProxySettings( settings::proxySettings::Type,const QString& = {} ) ;
		settings::proxySettings::type types() const ;
		QByteArray proxyAddress() const ;
	private:
		QSettings& m_settings ;
	} ;

	settings::proxySettings getProxySettings() ;

	enum class tabName{ basic,batch,playlist } ;

	settings( const utility::cliArguments& ) ;
	~settings() ;
	QSettings& bk() ;
	void init_done() ;

	size_t maxConcurrentDownloads() ;

	QIcon getIcon( const QString& ) ;

	const QString& windowsOnly3rdPartyBinPath() ;
	const QString& windowsOnlyExeBinPath() ;
	const QString& windowsOnlyDefaultPortableVersionDownloadFolder() ;

	class mediaPlayer
	{
	public:
		struct PlayerOpts
		{
			PlayerOpts( QString e,QString n ) :
				exePath( std::move( e ) ),name( std::move( n ) )
			{
			}
			QString exePath ;
			QString name ;
		} ;
		class action
		{
		public:
			action( const QString& url,
				Logger& logger,
				const settings::mediaPlayer::PlayerOpts& opts,
				const QString& app,
				const QJsonObject& obj ) :
				m_url( url ),
				m_playerOpts( opts ),
				m_logger( logger ),
				m_appDataPath( app ),
				m_obj( obj )
			{
			}
			action move()
			{
				return std::move( *this ) ;
			}
			void operator()() const ;
			void logError() const ;
		private:
			QString m_url ;
			const settings::mediaPlayer::PlayerOpts& m_playerOpts ;
			Logger& m_logger ;
			const QString& m_appDataPath ;
			const QJsonObject& m_obj ;
		} ;

		mediaPlayer( const std::vector< settings::mediaPlayer::PlayerOpts >&,Logger& ) ;
		const std::vector< settings::mediaPlayer::PlayerOpts >& opts() const
		{
			return m_playerOpts ;
		}
		bool valid() const
		{
			return !m_playerOpts.empty() ;
		}
		settings::mediaPlayer::action ac( const QString& url,
						  const settings::mediaPlayer::PlayerOpts& opts,
						  const QString& appDataPath,
						  const QJsonObject& obj ) const
		{
			return { url,m_logger,opts,appDataPath,obj } ;
		}
	private:
		const std::vector< settings::mediaPlayer::PlayerOpts >& m_playerOpts ;
		Logger& m_logger ;
	} ;

	settings::mediaPlayer openWith( Logger& ) ;

	void runCommandOnSuccessfulDownload( const QString&,
					     const QString&,
					     const std::vector< QByteArray >& ) ;

	QString downloadFolder() ;
	QString libraryDownloadFolder() ;
	QString downloadFolder( Logger& ) ;
	QString localizationLanguagePath() ;
	QString localizationLanguage() ;
	QString commandWhenAllFinished() ;
	QString themeName() ;
	QString defaultEngine( settings::tabName,const QString& ) ;
	QString cookieBrowserName( const QString& engineName ) ;
	QString cookieBrowserTextFilePath( const QString& engineName ) ;
	QString windowsDimensions( const QString& windowName ) ;
	QString playlistRangeHistoryLastUsed() ;
	QString gitHubDownloadUrl() ;
	const QString& configPaths() ;
	const QString& appDataPath() ;
	QString textEncoding( const QString& ) ;
	QStringList getOptionsHistory( settings::tabName ) ;
	QStringList playlistRangeHistory() ;
	QStringList playlistUrlHistory() ;

	QString lastUsedOption( const QString&,settings::tabName ) ;

	QStringList localizationLanguages() ;

	QByteArray highDpiScalingFactor() ;

	QPixmap defaultVideoThumbnailIcon( settings::tabName ) ;

	class LogsLimits
	{
	public:
		LogsLimits( size_t m,size_t l ) :
			m_maxLogEntries( m ),m_logEntriesToRemove( l )
		{
		}
		size_t maxLogEntries() const
		{
			return m_maxLogEntries ;
		}
		size_t removeEntriesCount() const
		{
			return m_logEntriesToRemove ;
		}
	private:
		size_t m_maxLogEntries ;
		size_t m_logEntriesToRemove ;
	} ;

	LogsLimits getLogsLimits() ;

	bool desktopNotifyOnDownloadComplete() ;
	bool desktopNotifyOnAllDownloadComplete() ;
	bool libraryShowFolderFirst() ;
	bool libraryArrangeAscending() ;
	bool libraryArrangeByDate() ;
	bool portableVersion() ;
	bool monitorClipboardUrl( settings::tabName ) ;
	bool enabledHighDpiScaling() ;
	bool showTrayIcon() ;
	bool autoDownload() ;
	bool downloadOptionsAsLast() ;
	bool autoDownloadWhenAddedInBatchDownloader() ;
	bool showVersionInfoAndAutoDownloadUpdates() ;
	bool showLocalAndLatestVersionInformation() ;
	bool showLocalVersionInformationOnly() ;
	bool concurrentDownloading() ;
	bool showMetaDataInBatchDownloader() ;
	bool saveHistory() ;
	bool playlistDownloaderSaveHistory() ;
	bool singleInstance() ;
	bool autoSavePlaylistOnExit() ;
	bool useInternalArchiveFile() ;
	bool enableLibraryTab() ;
	bool printMediaPlayers() ;
	bool checkForEnginesUpdates() ;
	bool autoHideDownloadWhenCompleted() ;
	bool deleteFilesOnCanceledDownload() ;
	bool autoSetDefaultEngineAndOptions() ;
	bool cookieSourceSetToBrowerName() ;

	void setMainWindowDimensions( QWidget * ) ;

	qint64 timeOutWaitingForClipboardData() ;

	Qt::Alignment textAlignment() ;
	int networkTimeOut() ;
	int stringTruncationSize() ;
	int historySize() ;
	int tabNumber() ;
	int maxLoggerProcesses() ;
	int thumbnailWidth( settings::tabName ) ;
	int thumbnailHeight( settings::tabName ) ;
	int desktopNotificationTimeOut() ;

	void setOpenWith( const QString& ) ;
	void setCookieSourceSetToBrowerName( bool ) ;
	void setShowLocalVersionInformationOnly( bool ) ;
	void setShowLocalAndLatestVersionInformation( bool ) ;
	void setLibraryShowFolderFirst( bool ) ;
	void setLibraryArrangeAscending( bool ) ;
	void setLibraryArrangeByDate( bool ) ;
	void setAutoHideDownloadWhenCompleted( bool ) ;
	void setCheckForUpdates( bool ) ;
	void setDesktopNotifyOnDownloadComplete( bool ) ;
	void setDesktopNotifyOnAllDownloadComplete( bool ) ;
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
	void saveMainWindowDimensions( const QRect& ) ;
	void openUrl( const QString& ) ;
	void setEnableLibraryTab( bool ) ;
	void setMonitorClipboardUrl( bool,settings::tabName ) ;
	void setShowMetaDataInBatchDownloader( bool ) ;
	void setPlaylistDownloaderSaveHistory( bool ) ;
	void setShowVersionInfoAndAutoDownloadUpdates( bool ) ;
	void setThemeName( const QString& ) ;
	void setPlaylistRangeHistoryLastUsed( const QString& ) ;
	void setHighDpiScalingFactor( const QString& ) ;
	void setTextEncoding( const QString&,const QString& ) ;
	void setlibraryDownloadFolder( const QString& ) ;
	void setCookieBrowserName( const QString& engineName,const QString& browserName ) ;
	void setCookieTextFilePath( const QString& engineName,const QString& browserName ) ;
	void setDefaultEngine( const QString&,settings::tabName ) ;
	void setLastUsedOption( const QString& engineName,const QString& options,settings::tabName ) ;
	void setDownloadFolder( const QString& ) ;
	void setLocalizationLanguage( const QString& language ) ;
	void setWindowDimensions( const QString& window,const QString& dimenstion ) ;
private:	
	QString appDataLocation() ;
	void clearFlatPakTemps() ;
	void init_done_imp() ;

	std::vector< settings::mediaPlayer::PlayerOpts > openWith() ;

	QVariant getValue( const QString& opt,const QVariant& e )
	{
		if( !m_settings.contains( opt ) ){

			m_settings.setValue( opt,e ) ;
		}

		return m_settings.value( opt ) ;
	}

	QByteArray getOption( const QString& opt,const QByteArray& e )
	{
		return this->getValue( opt,e ).toByteArray() ;
	}

	QString getOption( const QString& opt,const QString& e )
	{
		return this->getValue( opt,e ).toString() ;
	}

	bool getOption( const QString& opt,bool e )
	{
		return this->getValue( opt,e ).toBool() ;
	}

	int getOption( const QString& opt,int e )
	{
		return this->getValue( opt,e ).toInt() ;
	}

	QStringList getOption( const QString& opt,const QStringList& e )
	{
		return this->getValue( opt,e ).toStringList() ;
	}

	class sLogger
	{
	public:
		sLogger() : m_logger( nullptr )
		{
		}
		sLogger( Logger& logger ) : m_logger( &logger )
		{
		}
		void add( const QByteArray&,int ) ;
	private:
		Logger * m_logger ;
	} ;

	QString downloadFolderImp( settings::sLogger ) ;
	QString downloadFolder( const QString& defaultPath,settings::sLogger& ) ;

	std::unique_ptr< QSettings > init() ;

	QString downloadLocation() ;

	class options
	{
	public:
		options( const utility::cliArguments&,const QString& ) ;

		const QString& dataPath() const
		{
			return m_dataPath ;
		}
		const QString& windowsOnly3rdPartyBinPath() const
		{
			return m_exe3PartyBinPath ;
		}
		const QString& windowsOnlyExePath() const
		{
			return m_exePath ;
		}
		const QString& windowsOnlyDefaultPortableVersionDownloadFolder() const
		{
			return m_defaultPortableVersionDownloadFolder ;
		}
		bool portableVersion() const
		{
			return m_portableVersion ;
		}
		const QString& pathToOldUpdatedVersion() const
		{
			return m_pathToOldUpdatedVersion ;
		}
	private:
		QString m_dataPath ;
		QString m_exePath ;
		QString m_exe3PartyBinPath ;
		QString m_defaultPortableVersionDownloadFolder ;
		QString m_pathToOldUpdatedVersion ;
		bool m_portableVersion ;
	} ;

	bool m_EnableHighDpiScaling ;
	bool m_printMediaPlayers ;
	QString m_appDataPath ;
	QByteArray m_defaultScaleFactor ;
	QByteArray m_MdScaleFactor ;
	options m_options ;
	std::unique_ptr< QSettings > m_settingsP ;
	QSettings& m_settings ;
};

#endif

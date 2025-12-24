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
			action( const QStringList& urls,
				Logger& logger,
				const settings::mediaPlayer::PlayerOpts& opts,
				const QString& app,
				const QJsonObject& obj,
				settings& ss ) :
				m_urls( urls ),
				m_playerOpts( opts ),
				m_logger( logger ),
				m_appDataPath( app ),
				m_obj( obj ),
				m_settings( ss )
			{
			}
			action move()
			{
				return std::move( *this ) ;
			}
			void operator()() const ;
			void logError() const ;
		private:
			void run( const QString& exe,const QStringList& args ) const ;
			QStringList setVLCoptions( const QStringList& m ) const ;
			QStringList m_urls ;
			const settings::mediaPlayer::PlayerOpts& m_playerOpts ;
			Logger& m_logger ;
			const QString& m_appDataPath ;
			const QJsonObject& m_obj ;
			settings& m_settings ;
		} ;

		mediaPlayer( settings&,const std::vector< settings::mediaPlayer::PlayerOpts >&,Logger& ) ;
		const std::vector< settings::mediaPlayer::PlayerOpts >& opts() const
		{
			return m_playerOpts ;
		}
		bool valid() const
		{
			return !m_playerOpts.empty() ;
		}
		settings::mediaPlayer::action ac(  const QStringList& urls,
						  const settings::mediaPlayer::PlayerOpts& opts,
						  const QString& appDataPath,
						  const QJsonObject& obj ) const
		{
			return { urls,m_logger,opts,appDataPath,obj,m_settings } ;
		}
	private:
		const std::vector< settings::mediaPlayer::PlayerOpts >& m_playerOpts ;
		Logger& m_logger ;
		settings& m_settings ;
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
	QString playlistRangeHistoryLastUsed( const QString& engineName ) ;
	QString gitHubDownloadUrl() ;

	const QString& configPaths() ;
	const QString& appDataPath() ;

	QString textEncoding( const QString& ) ;
	QStringList getOptionsHistory( settings::tabName,const QString& ) ;
	QStringList playlistRangeHistory( const QString& ) ;
	QStringList playlistUrlHistory( const QString& ) ;

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

	void setAutoDownloadWhenAddedInBatchDownloader( bool ) ;
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
	void clearOptionsHistory( settings::tabName,const QString& ) ;
	void addToplaylistRangeHistory( const QString&,const QString& ) ;
	void clearPlaylistRangeHistory( const QString& ) ;
	void clearPlaylistUrlHistory( const QString& ) ;
	void setAutoSavePlaylistOnExit( bool ) ;
	void addOptionsHistory( const QString& engineName,const QString&,settings::tabName ) ;
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
	void setPlaylistRangeHistoryLastUsed( const QString& engineName,const QString& ) ;
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
	QString tmpFile( const QString&,const QString& ) ;

	class flatpakRuntimeOptions
	{
	public:
		class VLC
		{
		public:
			QString exe() const
			{
				return "flatpak-spawn" ;
			}
			const QStringList& args() const
			{
				return m_args ;
			}
			bool valid() const
			{
				return !m_args.isEmpty() ;
			}
			void checkAvailability() const ;
		private:
			bool checkAvailability( const QStringList& ) const ;
			mutable QStringList m_args ;
		} ;

		const VLC& getVLC() const ;

		flatpakRuntimeOptions( settings& ) ;

		const QString& globalBinPath() const
		{
			return m_globalBinPath ;
		}
		const QString& architecture() const
		{
			return m_architecture ;
		}
		const QString& commitId() const
		{
			return m_commitId ;
		}
		const QString& localBinPath() const
		{
			return m_localBinPath ;
		}
		const QString& runtimePath() const
		{
			return m_runtimePath ;
		}
		const QString& appDataLocation() const
		{
			return m_appDataLocation ;
		}
	private:
		QString flatpkakInfoFile() ;
		QSettings m_settings ;
		QString m_globalBinPath ;
		QString m_architecture ;
		QString m_commitId ;
		QString m_runtimePath ;
		QString m_localBinPath ;
		QString m_appDataLocation ;
		VLC m_vlc ;
	} ;

	const flatpakRuntimeOptions& flatpakIntance() ;
private:
	void addToHistory( QSettings& settings,
			   QStringList& history,
			   const QString& key,
			   const QString& input,
			   int max ) ;
	bool darkTheme() ;
	QString monitorClipboadUrl( settings::tabName ) ;
	QByteArray hash( quint64,const QString& ) ;
	QString getTabOption( const QString&,settings::tabName ) ;
	QStringList directoryList( const QString& ) ;
	QString getDefaultEngineName( settings::tabName ) ;
	QString thumbnailTabName( const QString&, settings::tabName ) ;
	QString setOptionWithEngineName( const QString& opts,const QString& engineName ) ;
	QString getOptionsHistoryTabName( settings::tabName,const QString& engineName ) ;
	QString appDataLocation() ;
	void clearFlatPakTemps() ;

	std::unique_ptr< QSettings > setConfig( const QString& path ) ;
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
	flatpakRuntimeOptions m_flatpakRuntimeOptions ;
};

#endif

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

#include <vector>
#include <memory>

#include <QStandardPaths>

class Logger ;

class settings
{
public:
	class darkModes
	{
	public:
		const QStringList& typesUntranslated() const
		{
			return m_untranslated ;
		}
		const QStringList& typesTranslated() const
		{
			return m_translated ;
		}
		const QString& translatedAt( int s ) const
		{
			return m_translated[ s ] ;
		}
		const QString& unTranslatedAt( int s ) const
		{
			return m_untranslated[ s ] ;
		}
		int translatedIndexAt( const QString& e ) const
		{
			return this->indexAt( e,m_translated ) ;
		}
		int unTranslatedIndexAt( const QString& e ) const
		{
			return this->indexAt( e,m_untranslated ) ;
		}
		static QString themeFileName( const QString& e )
		{
			if( e == "Dark Theme 1" ){

				return ":dark.qss" ;

			}else if( e == "Dark Theme 2" ){

				return ":qdarkstyle/dark/style.qss" ;
			}else{
				return QString() ;
			}
		}
	private:
		int indexAt( const QString& e,const QStringList& s ) const
		{
			for( int i = 0 ; i < s.size() ; i++ ){

				if( s[ i ] == e ){

					return i ;
				}
			}

			return 0 ;
		}
		QStringList m_untranslated{ "Normal","Dark Theme 1","Dark Theme 2" } ;
		QStringList m_translated{ QObject::tr( "Normal" ),QObject::tr( "Dark Theme 1" ),QObject::tr( "Dark Theme 2" ) } ;
	} ;

	settings() ;

	int tabNumber() ;
	int maxConcurrentDownloads() ;

	QString downloadFolder() ;
	QString downloadFolder( Logger& ) ;
	QString presetOptions() ;
	QString defaultEngine() ;
	QString localizationLanguagePath() ;
	QString localizationLanguage() ;
	QString commandOnSuccessfulDownload() ;
	QString commandWhenAllFinished() ;
	QString darkMode() ;

	enum class tabName{ basic,batch,playlist } ;

	QString lastUsedOption( settings::tabName ) ;

	QStringList presetOptionsList() ;
	QStringList localizationLanguages() ;

	QStringList configPaths() ;

	QByteArray highDpiScalingFactor() ;

	static bool portableVersion() ;
	static QString portableVersionConfigPath() ;

	bool enabledHighDpiScaling() ;
	bool showTrayIcon() ;
	bool autoDownload() ;
	bool showVersionInfoWhenStarting() ;
	bool concurrentDownloading() ;
	bool useSystemProvidedVersionIfAvailable() ;
	bool doNotGetUrlTitle() ;

	void setUseSystemProvidedVersionIfAvailable( bool ) ;
	void setMaxConcurrentDownloads( int ) ;
	void setTabNumber( int ) ;
	void setConcurrentDownloading( bool ) ;
	void setShowVersionInfoWhenStarting( bool ) ;
	void setDarkMode( const QString& ) ;
	void setHighDpiScalingFactor( const QString& ) ;
	void setPresetOptions( const QString& ) ;
	void setDefaultEngine( const QString& ) ;
	void setLastUsedOption( const QString&,settings::tabName ) ;
	void setPresetOptions( const QStringList& ) ;
	void setPresetToDefaults() ;
	void setDownloadFolder( const QString& ) ;
	void setLocalizationLanguage( const QString& language ) ;
private:
	bool m_EnableHighDpiScaling ;
	std::unique_ptr< QSettings > m_settingsP ;
	QSettings& m_settings ;
	bool m_portableVersion ;
};

#endif

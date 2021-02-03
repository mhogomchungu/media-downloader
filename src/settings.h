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
#include <QMenu>

class translator ;

class settings
{
public:
	settings() ;
	QByteArray highDpiScalingFactor() ;
	QString downloadFolder() ;
	QString presetOptions() ;
	QString cmdName() ;
	QStringList presetOptionsList() ;
	QStringList defaultDownLoadCmdOptions() ;
	QStringList defaultListCmdOptions() ;
	bool showTrayIcon() ;
	bool autoDownload() ;
	void setHighDpiScalingFactor( const QString& ) ;
	void setPresetOptions( const QString& ) ;
	void setPresetOptions( const QStringList& ) ;
	void setPresetToDefaults() ;
	void setDownloadFolder( const QString& ) ;
	void setLocalizationLanguage( const QString& language ) ;
	QString localizationLanguagePath() ;
	void languageMenu( QMenu * m,QAction * ac,translator& s ) ;
	const QString& localizationLanguage() ;
	QString localizationLanguageConfig() ;
	void setLocalizationLanguage( bool translate,QMenu * m,translator& translator ) ;
	QStringList localizationLanguages() ;
private:
	class woof
	{
	public:
		woof()
		{
			m_pair.emplace_back( QObject::tr( "English (US)" ),"en_US" ) ;
			m_pair.emplace_back( QObject::tr( "Polish (Poland)" ),"pl_PL" ) ;
		}
		const QString& toUiName( const QString& configName )
		{
			for( const auto& it : m_pair ){

				if( it.second == configName ){

					return it.first ;
				}
			}

			return ShouldNotGetHere ;
		}
		const QString& toConfigName( const QString& uiName )
		{
			for( const auto& it : m_pair ){

				if( it.first == uiName ){

					return it.second ;
				}
			}

			return ShouldNotGetHere ;
		}
	private:
		QString ShouldNotGetHere ;
		std::vector< std::pair< QString,QString > > m_pair ;
	}m_knownTranslations ;

	QSettings m_settings ;
};

#endif

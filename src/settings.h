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

#include <QStandardPaths>

class settings
{
public:
	settings() ;

	int tabNumber() ;

	QString downloadFolder() ;
	QString presetOptions() ;
	QString defaultEngine() ;

	QString localizationLanguagePath() ;
	QString localizationLanguage() ;

	QStringList presetOptionsList() ;
	QStringList localizationLanguages() ;

	QStringList configPaths() ;

	QByteArray highDpiScalingFactor() ;

	bool enabledHighDpiScaling() ;
	bool showTrayIcon() ;
	bool autoDownload() ;
	bool showVersionInfoWhenStarting() ;
	bool sequentialDownloading() ;

	void setTabNumber( int ) ;
	void setSequentialDownloading( bool ) ;
	void setShowVersionInfoWhenStarting( bool ) ;
	void setHighDpiScalingFactor( const QString& ) ;
	void setPresetOptions( const QString& ) ;
	void setDefaultEngine( const QString& ) ;
	void setPresetOptions( const QStringList& ) ;
	void setPresetToDefaults() ;
	void setDownloadFolder( const QString& ) ;
	void setLocalizationLanguage( const QString& language ) ;
private:
	bool m_EnableHighDpiScaling ;
	QSettings m_settings ;
};

#endif

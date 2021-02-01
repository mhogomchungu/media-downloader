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

#include<QSettings>
#include<QString>
#include<QStringList>
#include<QByteArray>

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
private:
	QSettings m_settings ;
};

#endif

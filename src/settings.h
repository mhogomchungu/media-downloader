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

class settings
{
public:
	class backend
	{
	public:
		backend( const QString& name,const QString& va,const QString& oa,const QString& dp ) :
			m_name( name ),m_va( va ),m_oa( oa ),m_downloadPath( dp )
		{
		}
		const QString& name()
		{
			return m_name ;
		}
		const QString& versionArgument()
		{
			return m_va ;
		}
		const QString& optionsArgument()
		{
			return m_oa ;
		}
		const QString& downloadPath()
		{
			return m_downloadPath ;
		}
	private:
		QString m_name ;
		QString m_va ;
		QString m_oa ;
		QString m_downloadPath ;
	};

	settings() ;

	QString downloadFolder() ;
	QString presetOptions() ;
	backend& backEnd() ;
	QString localizationLanguagePath() ;
	QString localizationLanguage() ;
	QString backendPath() ;
	QStringList presetOptionsList() ;
	QStringList defaultDownLoadCmdOptions() ;
	QStringList defaultListCmdOptions() ;
	QStringList localizationLanguages() ;

	QByteArray highDpiScalingFactor() ;

	bool showTrayIcon() ;
	bool autoDownload() ;
	bool usePrivateBackEnd() ;
	void setHighDpiScalingFactor( const QString& ) ;
	void setPresetOptions( const QString& ) ;
	void setPresetOptions( const QStringList& ) ;
	void setPresetToDefaults() ;
	void setDownloadFolder( const QString& ) ;
	void setLocalizationLanguage( const QString& language ) ;
private:
	QSettings m_settings ;
	settings::backend m_backend ;
};

#endif

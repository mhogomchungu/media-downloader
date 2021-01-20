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

#include<QDir>

settings::settings() : m_settings( "media-downloader","media-downloader" )
{
}

QByteArray settings::highDpiScalingFactor()
{
	if( !m_settings.contains( "EnabledHighDpiScalingFactor" ) ){

		m_settings.setValue( "EnabledHighDpiScalingFactor","1.0" ) ;
	}

	return m_settings.value( "EnabledHighDpiScalingFactor" ).toByteArray() ;
}

QString settings::downloadFolder()
{
	if( !m_settings.contains( "DownloadFolder" ) ){
#ifdef Q_OS_LINUX
		m_settings.setValue( "DownloadFolder",QDir::homePath() ) ;
	#else
		m_settings.setValue( "DownloadFolder",QDir::homePath() + "/Desktop" ) ;
#endif
	}

	return m_settings.value( "DownloadFolder" ).toString() ;
}

void settings::setPresetToDefaults()
{
	if( !m_settings.contains( "PresetOptionsDefaults" ) ){

		QStringList s{ "Best",
			       "Low(240p)(133+140)",
			       "Medium(360p)(134+140)",
			       "High(720p)(136+140)",
			       "Very High(1080p)(137+140)",
			       "Super High(2160p60)(138+140)" } ;

		m_settings.setValue( "PresetOptionsDefaults",s ) ;
	}

	m_settings.setValue( "PresetOptions",m_settings.value( "PresetOptionsDefaults" ).toStringList() ) ;
}

void settings::setPresetOptions( const QString& e )
{
	this->setPresetOptions( utility::split( e,',' ) ) ;
}

void settings::setPresetOptions( const QStringList& m )
{
	m_settings.setValue( "PresetOptions",m ) ;
}

QString settings::presetOptions()
{
	if( !m_settings.contains( "PresetOptions" ) ){

		this->setPresetToDefaults() ;
	}

	return m_settings.value( "PresetOptions" ).toStringList().join( ',' ) ;
}

QString settings::cmdName()
{
	if( !m_settings.contains( "CommandName" ) ){

		m_settings.setValue( "CommandName","youtube-dl" ) ;
	}

	return m_settings.value( "CommandName" ).toString() ;
}

QStringList settings::presetOptionsList()
{
	return utility::split( this->presetOptions(),',' ) ;
}

QStringList settings::defaultDownLoadCmdOptions()
{
	if( !m_settings.contains( "DefaultDownLoadCmdOptions" ) ){

		QStringList m{ "--newline","--ignore-config","--no-playlist" } ;

		m_settings.setValue( "DefaultDownLoadCmdOptions",m ) ;
	}

	return m_settings.value( "DefaultDownLoadCmdOptions" ).toStringList() ;
}

QStringList settings::defaultListCmdOptions()
{
	if( !m_settings.contains( "DefaultListCmdOptions" ) ){

		m_settings.setValue( "DefaultListCmdOptions",QStringList{ "-F" } ) ;
	}

	return m_settings.value( "DefaultListCmdOptions" ).toStringList() ;
}

void settings::setHighDpiScalingFactor( const QString& m )
{
	m_settings.setValue( "EnabledHighDpiScalingFactor",m ) ;
}

void settings::setDownloadFolder( const QString& m )
{
	m_settings.setValue( "DownloadFolder",m ) ;
}

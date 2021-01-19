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
	if( !m_settings.contains( "DefaultPresetOptions" ) ){

		m_settings.setValue( "DefaultPresetOptions",QStringList{ "18","22" } ) ;
	}

	this->setPresetOptions( m_settings.value( "DefaultPresetOptions" ).toStringList() ) ;
}

void settings::setPresetOptions( const QStringList& m )
{
	m_settings.setValue( "PresetOptions",m ) ;
}

QStringList settings::presetOptions()
{
	if( !m_settings.contains( "PresetOptions" ) ){

		this->setPresetToDefaults() ;
	}

	return m_settings.value( "PresetOptions" ).toStringList() ;
}

void settings::setHighDpiScalingFactor( const QString& m )
{
	m_settings.setValue( "EnabledHighDpiScalingFactor",m ) ;
}

void settings::setDownloadFolder( const QString& m )
{
	m_settings.setValue( "DownloadFolder",m ) ;
}

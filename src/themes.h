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

#ifndef THEMES_H
#define THEMES_H

#include <QString>
#include <QStringList>
#include <QComboBox>
#include <QJsonObject>
#include <QJsonArray>
#include <QDir>
#include <QApplication>
#include <QStyleFactory>

#include <vector>

class themes
{
public:
	themes() = default ;
	themes( const QString& themeName,const QString& themePath ) ;
	themes( const QString& themePath ) ;
	QStringList typesUntranslated() const ;
	QStringList typesTranslated() const ;
	const QString& translatedAt( int s ) const ;
	const QString& unTranslatedAt( int s ) const ;
	int translatedIndexAt( const QString& e ) const ;
	int unTranslatedIndexAt( const QString& e ) const ;
	bool usingThemes() const ;
	void setComboBox( QComboBox& cb,const QString& dm ) const ;
	QString defaultthemeFullPath() ;
	QString themeFullPath() ;
	void setDefaultTheme( QApplication& app ) ;
	void setTheme( QApplication& app,const QJsonObject& obj ) ;
	QJsonObject defaultTheme() ;
private:
	QColor getColor( const QString& e,const QJsonObject& obj ) ;
	void updateThemes() ;
	int indexAt( const QString& e,const QStringList& s ) const ;

	QString m_theme ;
	QString m_themePath ;
	QString m_defaultDarkTheme = "Dark" ;

	struct Pair{
		Pair( const QString& u,const QString& t ) :
			untranslated( u ),translated( t )
		{
		}
		QString untranslated ;
		QString translated ;
	};

	std::vector< Pair > m_strings{ { "Normal",QObject::tr( "Normal" ) },
				       { "Dark",QObject::tr( "Dark" ) } } ;
} ;

#endif

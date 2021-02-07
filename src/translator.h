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

#ifndef TRANSLATOR_H
#define TRANSLATOR_H

#include <QWidget>
#include <QTranslator>

#include "settings.h"

class translator
{
public:
	static const char * CLEARSCREEN ;
	static const char * CLEAROPTIONS ;

        translator( settings& ) ;
	void setLanguage( const QByteArray& e ) ;
	void setDefaultLanguage() ;
	~translator() ;
	const QString& UIName( const QString& name ) ;
	const QString& name( const QString& UIName ) ;
	const QString& untranslated( const QString& string ) ;
	QString translate( const QString& internalName ) ;
	const char * UINameUnTranslated( const QString& name ) ;
	void addString( const QString& translatedString,
			const char * untranslatedString,
			const QString& internalName ) ;
	struct entry{
		entry( const QString&,const char *,const QString& ) ;
		QString UINameTranslated ;
		const char * UINameUnTranslated ;
		QString internalName ;
	} ;
	QAction * addAction( QMenu * m,translator::entry ) ;
	QMenu * addMenu( QMenu * m,translator::entry ) ;
private:
	void clear( void ) ;
	QTranslator * m_translator = nullptr ;
	std::vector< entry > m_languages ;
	std::vector< std::pair< QAction *,entry > > m_actions ;
	std::vector< std::pair< QMenu *,entry > > m_menus ;

	settings& m_settings ;
private:
};

#endif


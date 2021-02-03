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
#include "utility.h"

class translator
{
public:
        translator( settings& ) ;
	void setLanguage( const QByteArray& e ) ;
	void setDefaultLanguage() ;
	~translator() ;
	const QString& UIName( const QString& name ) ;
	const QString& name( const QString& UIName ) ;
	QString translate( const QString& internalName ) ;
	const char * UINameUnTranslated( const QString& name ) ;
private:
	struct entry{
	        entry( const QString&,const char *,const QString& ) ;
		QString UINameTranslated ;
		const char * UINameUnTranslated ;
		QString internalName ;
	} ;
	void clear( void ) ;
	QTranslator * m_translator = nullptr ;
	std::vector< entry > m_languages ;
	settings& m_settings ;
private:
};

#endif


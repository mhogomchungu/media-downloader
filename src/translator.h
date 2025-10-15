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
#include <QMenu>
#include <QAction>
#include <QApplication>

#include <vector>

#include "settings.h"

class Context ;

class translator
{
public:
	translator( settings&,QApplication& ) ;
	void setContext( Context& ) ;
	void setLanguage( const QString& e ) ;
	void setDefaultLanguage() ;
	~translator() ;
	const QString& UIName( const QString& name ) ;
	const QString& name( const QString& UIName ) ;
	const QString& untranslated( const QString& string ) ;
	QString translate( const QString& internalName ) ;
	const char * UINameUnTranslated( const QString& name ) ;
	struct entry{
		entry( const QString&,const char *,const QString& ) ;
		QString UINameTranslated ;
		const char * UINameUnTranslated ;
		QString internalName ;
		entry move()
		{
			return std::move( *this ) ;
		}
	} ;
	QAction * addAction( QMenu * m,translator::entry,bool permanentEntry = false ) ;
	QMenu * addMenu( QMenu * m,translator::entry,bool permanentEntry = false ) ;
private:
	void addString( const QString& translatedString,
			const char * untranslatedString,
			const QString& internalName ) ;
	void clear() ;
	QApplication& m_qapp ;
	QTranslator * m_translator = nullptr ;
	Context * m_ctx = nullptr ;
	std::vector< entry > m_languages ;
	std::vector< std::pair< QAction *,entry > > m_actions ;
	std::vector< std::pair< QMenu *,entry > > m_menus ;

	settings& m_settings ;
	QString m_pathLanguageFiles ;
};

#endif


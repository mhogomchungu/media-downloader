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

#include "translator.h"
#include "settings.h"
#include "locale_path.h"

#include <QCoreApplication>

const char * translator::CLEAROPTIONS = "Clear Options" ;
const char * translator::CLEARSCREEN = "Clear Screen" ;

translator::translator( settings& s ) : m_settings( s )
{
	this->setDefaultLanguage() ;
}

void translator::setLanguage( const QString& e )
{
	QCoreApplication::installTranslator( [ & ](){

		this->clear() ;

		m_translator = new QTranslator() ;

		m_translator->load( e,m_settings.localizationLanguagePath() ) ;

		return m_translator ;
	}() ) ;

	m_languages.clear() ;
	//m_menus.clear() ;
	//m_actions.clear() ;

	this->addString( QObject::tr( "Polish (Poland)" ),"Polish (Poland)","pl_PL" ) ;
	this->addString( QObject::tr( "English (US)" ),"English (US)","en_US" ) ;
}

void translator::setDefaultLanguage()
{
	this->setLanguage( m_settings.localizationLanguage() ) ;
}

translator::~translator()
{
}

const QString& translator::UIName( const QString& internalName )
{
	for( const auto& it : m_languages ){

		if( it.internalName == internalName ){

			return it.UINameTranslated ;
		}
	}

	static QString s ;
	return s ;
}

const QString& translator::name( const QString& UIName )
{
	for( const auto& it : m_languages ){

		if( it.UINameTranslated == UIName ){

			return it.internalName ;
		}
	}

	static QString s ;
	return s ;
}

QString translator::translate( const QString& internalName )
{
	return QObject::tr( this->UINameUnTranslated( internalName ) ) ;
}

const char * translator::UINameUnTranslated( const QString& internalName )
{
	for( const auto& it : m_languages ){

		if( it.internalName == internalName ){

			return it.UINameUnTranslated ;
		}
	}

	return "" ;
}

void translator::addString( const QString& translatedString,
			    const char * untranslatedString,
			    const QString & internalName)
{
	m_languages.emplace_back( entry( translatedString,untranslatedString,internalName ) ) ;
}

QAction * translator::addAction( QMenu * m,translator::entry e )
{
	auto ac = m->addAction( e.UINameTranslated ) ;

	ac->setObjectName( e.UINameUnTranslated ) ;

	//m_actions.emplace_back( ac,std::move( e ) ) ;

	return ac ;
}

QMenu * translator::addMenu( QMenu * m,translator::entry e )
{
	auto menu = m->addMenu( e.UINameTranslated ) ;

	//m_menus.emplace_back( m,std::move( e ) ) ;

	return menu ;
}

void translator::clear()
{
	if( m_translator ){

		QCoreApplication::removeTranslator( m_translator ) ;
		delete m_translator ;
		m_translator = nullptr ;
	}
}

translator::entry::entry( const QString& a,const char * b,const QString& c ) :
	UINameTranslated( a ),UINameUnTranslated( b ),internalName( c )
{
}

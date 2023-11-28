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

#include "themes.h"

#include <QJsonDocument>

themes::themes( const QString& themeName,const QString& themePath )  :
	m_theme( themeName ),
	m_themePath( themePath )
{
	this->updateThemes() ;

	if( !this->typesUntranslated().contains( m_theme ) ){

		m_theme = this->unTranslatedAt( 0 ) ;
	}
}

themes::themes( const QString& themePath ) : m_themePath( themePath )
{
	this->updateThemes() ;
}

QStringList themes::typesUntranslated() const
{
	QStringList m ;

	for( const auto& it : m_strings ){

		m.append( it.untranslated ) ;
	}

	return m ;
}

QStringList themes::typesTranslated() const
{
	QStringList m ;

	for( const auto& it : m_strings ){

		m.append( it.translated ) ;
	}

	return m ;
}

const QString& themes::translatedAt( int s ) const
{
	return m_strings[ static_cast< size_t >( s ) ].translated ;
}

const QString& themes::unTranslatedAt( int s ) const
{
	return m_strings[ static_cast< size_t >( s ) ].untranslated ;
}

int themes::translatedIndexAt( const QString& e ) const
{
	for( size_t i = 0 ; i < m_strings.size() ; i++ ){

		if( m_strings[ i ].translated == e ){

			return static_cast< int >( i ) ;
		}
	}

	return 0 ;
}

int themes::unTranslatedIndexAt( const QString& e ) const
{
	for( size_t i = 0 ; i < m_strings.size() ; i++ ){

		if( m_strings[ i ].untranslated == e ){

			return static_cast< int >( i ) ;
		}
	}

	return 0 ;
}

bool themes::usingThemes() const
{
	return this->unTranslatedAt( 0 ) != m_theme ;
}

void themes::setComboBox( QComboBox& cb,const QString& dm ) const
{
	cb.clear() ;

	cb.addItems( this->typesTranslated() ) ;

	cb.setCurrentIndex( this->unTranslatedIndexAt( dm ) ) ;
}

QString themes::defaultthemeFullPath() const
{
	return m_themePath + "/" + m_defaultDarkTheme + ".json" ;
}

QString themes::defaultPureDarkthemeFullPath() const
{
	return m_themePath + "/Pure Dark.json" ;
}

QString themes::themeFullPath() const
{
	if( m_theme == m_defaultDarkTheme ){

		return this->defaultthemeFullPath() ;
	}else{
		return m_themePath + "/" + m_theme + ".json" ;
	}
}

void themes::setDefaultTheme( QApplication& app ) const
{
	this->setTheme( app,this->defaultTheme() ) ;
}

void themes::setTheme( QApplication& app,const QJsonObject& obj ) const
{
	app.setStyle( QStyleFactory::create( "Fusion" ) ) ;

	QPalette m ;

	m.setColor( QPalette::Window,this->getColor( "QPalette::Window",obj ) ) ;
	m.setColor( QPalette::WindowText,this->getColor( "QPalette::WindowText",obj ) ) ;
	m.setColor( QPalette::Base,this->getColor( "QPalette::Base",obj ) ) ;
	m.setColor( QPalette::AlternateBase,this->getColor( "QPalette::AlternateBase",obj ) ) ;
	m.setColor( QPalette::ToolTipBase,this->getColor( "QPalette::ToolTipBase",obj ) ) ;
	m.setColor( QPalette::ToolTipText,this->getColor( "QPalette::ToolTipText",obj ) ) ;
	m.setColor( QPalette::Text,this->getColor( "QPalette::Text",obj ) ) ;
	m.setColor( QPalette::Disabled,QPalette::Text,this->getColor( "QPalette::Disabled,QPalette::Text",obj ) ) ;
	m.setColor( QPalette::Button,this->getColor( "QPalette::Button",obj ) ) ;
	m.setColor( QPalette::ButtonText,this->getColor( "QPalette::ButtonText",obj ) ) ;
	m.setColor( QPalette::Disabled,QPalette::ButtonText,this->getColor( "QPalette::Disabled,QPalette::ButtonText",obj ) ) ;
	m.setColor( QPalette::BrightText,this->getColor( "QPalette::BrightText",obj ) ) ;
	m.setColor( QPalette::Link,this->getColor( "QPalette::Link",obj ) ) ;
	m.setColor( QPalette::Highlight,this->getColor( "QPalette::Highlight",obj ) ) ;
	m.setColor( QPalette::HighlightedText,this->getColor( "QPalette::HighlightedText",obj ) ) ;
	m.setColor( QPalette::Disabled,QPalette::HighlightedText,this->getColor( "QPalette::Disabled,QPalette::HighlightedText",obj ) ) ;

	app.setPalette( m ) ;

	auto s = obj.value( "QToolTipStyleSheet" ).toString() ;

	if( !s.isEmpty() ){

		app.setStyleSheet( s ) ;
	}
}

static QJsonObject _baseTheme()
{
	QJsonObject obj ;

	obj.insert( "disabledColor",[](){

		QJsonObject obj ;

		obj.insert( "rgba",[](){

			QJsonArray arr ;

			arr.append( 127 ) ;
			arr.append( 127 ) ;
			arr.append( 127 ) ;
			arr.append( 255 ) ;

			return arr ;
		}() ) ;

		return obj ;
	}() ) ;

	obj.insert( "QPalette::WindowText",[](){

		QJsonObject obj ;

		obj.insert( "Qt::GlobalColor","Qt::lightGray" ) ;

		return obj ;
	}() ) ;

	obj.insert( "QPalette::Window",[](){

		QJsonObject obj ;

		obj.insert( "GlobalColor","darkColor" ) ;

		return obj ;
	}() ) ;

	obj.insert( "QPalette::Base",[](){

		QJsonObject obj ;

		obj.insert( "rgba",[](){

			QJsonArray arr ;

			arr.append( 41 ) ;
			arr.append( 42 ) ;
			arr.append( 43 ) ;
			arr.append( 255 ) ;

			return arr ;
		}() ) ;

		return obj ;
	}() ) ;

	obj.insert( "QPalette::AlternateBase",[](){

		QJsonObject obj ;

		obj.insert( "GlobalColor","darkColor" ) ;

		return obj ;
	}() ) ;

	obj.insert( "QPalette::ToolTipBase",[](){

		QJsonObject obj ;

		obj.insert( "GlobalColor","darkColor" ) ;

		return obj ;
	}() ) ;

	obj.insert( "QPalette::ToolTipText",[](){

		QJsonObject obj ;

		obj.insert( "Qt::GlobalColor","Qt::lightGray" ) ;

		return obj ;
	}() ) ;

	obj.insert( "QPalette::Text",[](){

		QJsonObject obj ;

		obj.insert( "Qt::GlobalColor","Qt::lightGray" ) ;

		return obj ;
	}() ) ;

	obj.insert( "QPalette::Disabled,QPalette::Text",[](){

		QJsonObject obj ;

		obj.insert( "GlobalColor","disabledColor" ) ;

		return obj ;
	}() ) ;

	obj.insert( "QPalette::Button",[](){

		QJsonObject obj ;

		obj.insert( "GlobalColor","darkColor" ) ;

		return obj ;
	}() ) ;

	obj.insert( "QPalette::ButtonText",[](){

		QJsonObject obj ;

		obj.insert( "Qt::GlobalColor","Qt::lightGray" ) ;

		return obj ;
	}() ) ;

	obj.insert( "QPalette::Disabled,QPalette::ButtonText",[](){

		QJsonObject obj ;

		obj.insert( "GlobalColor","disabledColor" ) ;

		return obj ;
	}() ) ;

	obj.insert( "QPalette::BrightText",[](){

		QJsonObject obj ;

		obj.insert( "Qt::GlobalColor","Qt::yellow" ) ;

		return obj ;
	}() ) ;

	obj.insert( "QPalette::Link",[](){

		QJsonObject obj ;

		obj.insert( "rgba",[](){

			QJsonArray arr ;

			arr.append( 77 ) ;
			arr.append( 148 ) ;
			arr.append( 209 ) ;
			arr.append( 255 ) ;

			return arr ;
		}() ) ;

		return obj ;
	}() ) ;

	obj.insert( "QPalette::Highlight",[](){

		QJsonObject obj ;

		obj.insert( "rgba",[](){

			QJsonArray arr ;

			arr.append( 27 ) ;
			arr.append( 129 ) ;
			arr.append( 231 ) ;
			arr.append( 255 ) ;

			return arr ;
		}() ) ;

		return obj ;
	}() ) ;

	obj.insert( "QPalette::HighlightedText",[](){

		QJsonObject obj ;

		obj.insert( "Qt::GlobalColor","Qt::black" ) ;

		return obj ;
	}() ) ;

	obj.insert( "QPalette::Disabled,QPalette::HighlightedText",[](){

		QJsonObject obj ;

		obj.insert( "GlobalColor","disabledColor" ) ;

		return obj ;
	}() ) ;

	obj.insert( "QToolTipStyleSheet","QToolTip { color: #ffffff; background-color: #2a82da; border: 1px solid white; }" ) ;

	return obj ;
}

QJsonObject themes::defaultTheme() const
{
	auto obj = _baseTheme() ;

	obj.insert( "darkColor",[](){

		QJsonObject obj ;

		obj.insert( "rgba",[](){

			QJsonArray arr ;

			arr.append( 45 ) ;
			arr.append( 45 ) ;
			arr.append( 45 ) ;
			arr.append( 255 ) ;

			return arr ;
		}() ) ;

		return obj ;
	}() ) ;

	return obj ;
}

QJsonObject themes::defaultPureDarkTheme() const
{
	auto obj = _baseTheme() ;

	obj.insert( "darkColor",[](){

		QJsonObject obj ;

		obj.insert( "rgba",[](){

			QJsonArray arr ;

			arr.append( 0 ) ;
			arr.append( 0 ) ;
			arr.append( 0 ) ;
			arr.append( 0 ) ;

			return arr ;
		}() ) ;

		return obj ;
	}() ) ;

	return obj ;
}

void themes::set( QApplication& app ) const
{
	if( !QFile::exists( m_themePath ) ){

		QDir().mkpath( m_themePath ) ;
	}

	auto defaultThemePath = this->defaultthemeFullPath() ;

	if( !QFile::exists( defaultThemePath ) ){

		QFile f( defaultThemePath ) ;

		if( f.open( QIODevice::WriteOnly ) ){

			QJsonDocument doc( this->defaultTheme() ) ;

			f.write( doc.toJson( QJsonDocument::Indented ) ) ;
		}
	}

	auto defaultPureDarkThemePath = this->defaultPureDarkthemeFullPath() ;

	if( !QFile::exists( defaultPureDarkThemePath ) ){

		QFile f( defaultPureDarkThemePath ) ;

		if( f.open( QIODevice::WriteOnly ) ){

			QJsonDocument doc( this->defaultPureDarkTheme() ) ;

			f.write( doc.toJson( QJsonDocument::Indented ) ) ;
		}
	}

	if( this->usingThemes() ){

		QFile f( this->themeFullPath() ) ;

		if( !f.open( QIODevice::ReadOnly ) ){

			this->setDefaultTheme( app ) ;
		}else{
			auto obj = QJsonDocument::fromJson( f.readAll() ).object() ;

			if( obj.isEmpty() ){

				this->setDefaultTheme( app ) ;
			}else{
				this->setTheme( app,obj ) ;
			}
		}
	}
}

static QColor _qtColor( const QString& aa )
{
	if( aa == "Qt::color0" ){

		return Qt::color0 ;

	}else if( aa == "Qt::color1" ){

		return Qt::color1 ;

	}else if( aa == "Qt::black" ){

		return Qt::black ;

	}else if( aa == "Qt::white" ){

		return Qt::white ;

	}else if( aa == "Qt::darkGray" ){

		return Qt::darkGray ;

	}else if( aa == "Qt::gray" ){

		return Qt::gray ;

	}else if( aa == "Qt::lightGray" ){

		return Qt::lightGray ;

	}else if( aa == "Qt::red" ){

		return Qt::red ;

	}else if( aa == "Qt::green" ){

		return Qt::green ;

	}else if( aa == "Qt::blue" ){

		return Qt::blue ;

	}else if( aa == "Qt::cyan" ){

		return Qt::cyan ;

	}else if( aa == "Qt::magenta" ){

		return Qt::magenta ;

	}else if( aa == "Qt::yellow" ){

		return Qt::yellow ;

	}else if( aa == "Qt::darkRed" ){

		return Qt::darkRed ;

	}else if( aa == "Qt::darkGreen" ){

		return Qt::darkGreen ;

	}else if( aa == "Qt::darkBlue" ){

		return Qt::darkBlue ;

	}else if( aa == "Qt::darkCyan" ){

		return Qt::darkCyan ;

	}else if( aa == "Qt::darkMagenta" ){

		return Qt::darkMagenta ;

	}else if( aa == "Qt::darkYellow" ){

		return Qt::darkYellow ;

	}else if( aa == "Qt::transparent" ){

		return Qt::transparent ;
	}else{
		return {} ;
	}
}

static QColor _getRGBA( const QJsonArray& aa )
{
	if( aa.size() == 3 ){

		auto a = aa[ 0 ].toInt() ;
		auto b = aa[ 1 ].toInt() ;
		auto c = aa[ 2 ].toInt() ;

		return QColor( a,b,c ) ;

	}else if( aa.size() == 4 ){

		auto a = aa[ 0 ].toInt() ;
		auto b = aa[ 1 ].toInt() ;
		auto c = aa[ 2 ].toInt() ;
		auto d = aa[ 3 ].toInt() ;

		return QColor( a,b,c,d ) ;
	}else{
		return {} ;
	}
}

QColor themes::getColor( const QString& e,const QJsonObject& obj ) const
{
	auto oo = obj.value( e ).toObject() ;

	auto a = oo.value( "rgba" ) ;

	if( !a.isUndefined() ){

		return _getRGBA( a.toArray() ) ;
	}

	a = oo.value( "GlobalColor" ) ;

	if( !a.isUndefined() ){

		auto aa = a.toString() ;

		if( aa == "darkColor" || aa == "disabledColor" ){

			auto m = obj.value( aa ).toObject() ;

			auto mm = m.value( "rgba" ) ;

			if( mm.isUndefined() ){

				auto mmm = m.value( "Qt::GlobalColor" ) ;

				if( mmm.isUndefined() ){

					return {} ;
				}else{
					return _qtColor( mmm.toString() ) ;
				}
			}else{
				return _getRGBA( mm.toArray() ) ;
			}
		}else{
			return {} ;
		}
	}

	a = oo.value( "Qt::GlobalColor" ) ;

	if( a.isUndefined() ){

		return {} ;
	}else{
		return _qtColor( a.toString() ) ;
	}
}

void themes::updateThemes()
{
	auto s = QDir( m_themePath ).entryList( QDir::Filter::Files ) ;

	s.removeOne( m_defaultDarkTheme + ".json" ) ;

	for( auto& it : s ){

		it.replace( ".json","" ) ;

		m_strings.emplace_back( it,it ) ;
	}
}

int themes::indexAt( const QString& e,const QStringList& s ) const
{
	for( int i = 0 ; i < s.size() ; i++ ){

		if( s[ i ] == e ){

			return i ;
		}
	}

	return 0 ;
}

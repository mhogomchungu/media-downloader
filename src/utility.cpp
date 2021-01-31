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

#include "utility.h"

QStringList utility::split( const QString& e,char token,bool skipEmptyParts )
{
	if( skipEmptyParts ){
		#if QT_VERSION < QT_VERSION_CHECK( 5,15,0 )
			return e.split( token,QString::SkipEmptyParts ) ;
		#else
			return e.split( token,Qt::SkipEmptyParts ) ;
		#endif
	}else{
		return e.split( token ) ;
	}
}

QList< QByteArray > utility::split( const QByteArray& e,char token )
{
	return e.split( token ) ;
}

#ifdef Q_OS_LINUX

bool utility::platformIsLinux()
{
	return true ;
}

bool utility::platformIsOSX()
{
	return false ;
}

bool utility::platformIsWindows()
{
	return false ;
}

#endif

#ifdef Q_OS_MACOS

bool utility::platformIsOSX()
{
	return true ;
}

bool utility::platformIsLinux()
{
	return false ;
}

bool utility::platformIsWindows()
{
    return false ;
}

#endif

#ifdef Q_OS_WIN

bool utility::platformIsWindows()
{
	return true ;
}

bool utility::platformIsLinux()
{
	return false ;
}

bool utility::platformIsOSX()
{
	return false ;
}

#endif

bool utility::platformIsNOTWindows()
{
	return !utility::platformIsWindows() ;
}

static void _add( QMenu * menu,const QStringList& args )
{
	for( const auto& it : args ){

		auto a = it ;

		auto b = a.lastIndexOf( '(' ) ;

		if( b != -1 ){

			auto m = a.mid( 0,b ) ;
			auto mm = a.mid( b + 1 ) ;
			mm.truncate( mm.size() - 1 ) ;
			menu->addAction( m )->setObjectName( mm ) ;
		}else{
			menu->addAction( it )->setObjectName( it ) ;
		}
	}
}

QMenu * utility::details::sMo( settings * settings,const QStringList& opts,bool addClear,QPushButton * w )
{
	auto m = w->menu() ;

	if( m ){

		m->deleteLater() ;
	}

	auto menu = new QMenu( w ) ;

	menu->addAction( QObject::tr( "Preset Options" ) )->setEnabled( false ) ;

	menu->addSeparator() ;

	_add( menu,settings->presetOptionsList() ) ;

	if( !opts.empty() ){

		menu->addSeparator() ;

		menu->addAction( QObject::tr( "Found Options" ) )->setEnabled( false ) ;

		menu->addSeparator() ;

		_add( menu,opts ) ;
	}

	if( addClear ){

		menu->addSeparator() ;

		const auto& cotr = selectedAction::clearOptionTextTr() ;
		const auto& co = selectedAction::clearOptionTextTr() ;

		const auto& cstr = selectedAction::clearScreenTextTr() ;
		const auto& cs = selectedAction::clearScreenText() ;

		menu->addAction( cotr )->setObjectName( co ) ;
		menu->addAction( cstr )->setObjectName( cs ) ;
	}

	w->setMenu( menu ) ;

	return menu ;
}

bool utility::youtubePath( const QString& e)
{
	return e.contains( "youtube.com" ) ;
}

bool utility::youtubePaths( const QStringList& e )
{
	return e.contains( "youtube.com" ) ;
}

bool utility::hasDigitsOnly( const QString& e )
{
	for( const auto& it : e ){

		if( !( it >= '0' && it <= '9'  ) ){

			return false ;
		}
	}

	return true ;
}

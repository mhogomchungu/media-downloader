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

#include "settings.h"
#include "context.hpp"

#include <QEventLoop>

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

int utility::terminateProcess( unsigned long )
{
	return 0 ;
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

int utility::terminateProcess( unsigned long )
{
	return 0 ;
}
#endif

#ifdef Q_OS_WIN

#include <windows.h>

int utility::terminateProcess( unsigned long pid )
{
	FreeConsole() ;

	if( AttachConsole( pid ) == TRUE ) {

		/*
		 * Add a fake Ctrl-C handler for avoid instant kill in this console
		 * WARNING: do not revert it or current program will also killed
		 */

		SetConsoleCtrlHandler( nullptr,true ) ;

		if( GenerateConsoleCtrlEvent( CTRL_C_EVENT,0 ) == TRUE ){

			return 0 ;
		}
	}

	return 1 ;
}

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

		a.replace( "Best-audiovideo(",QObject::tr( "Best-audiovideo" ) + "(" ) ;
		a.replace( "Best-audio(",QObject::tr( "Best-audio" ) + "(" ) ;

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

QMenu * utility::details::sMo( const Context& ctx,
			       const QStringList& opts,
			       bool addClear,
			       QPushButton * w )
{
	auto m = w->menu() ;

	if( m ){

		m->deleteLater() ;
	}

	auto menu = new QMenu( w ) ;

	auto& translator = ctx.Translator() ;
	auto& settings = ctx.Settings() ;

	translator::entry ss( QObject::tr( "Preset Options" ),"Preset Options","Preset Options" ) ;
	auto ac = translator.addAction( menu,std::move( ss ) ) ;

	ac->setEnabled( false ) ;

	menu->addSeparator() ;

	_add( menu,settings.presetOptionsList() ) ;

	if( !opts.empty() ){

		menu->addSeparator() ;

		translator::entry ss( QObject::tr( "Found Options" ),"Found Options","Found Options" ) ;

		_add( translator.addMenu( menu,std::move( ss ) ),opts ) ;
	}

	if( addClear ){

		menu->addSeparator() ;

		translator::entry ss( QObject::tr( "Clear Options" ),
						   translator::CLEAROPTIONS,
						   translator::CLEAROPTIONS ) ;

		translator.addAction( menu,std::move( ss ) ) ;

		translator::entry sx( QObject::tr( "Clear Screen" ),
						   translator::CLEARSCREEN,
						   translator::CLEARSCREEN ) ;

		translator.addAction( menu,std::move( sx ) ) ;
	}

	w->setMenu( menu ) ;

	return menu ;
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

QString utility::homePath()
{
	if( utility::platformIsWindows() ){

		return QDir::homePath() + "/Desktop" ;
	}else{
		return QDir::homePath() ;
	}
}

void utility::waitForOneSecond()
{
	utility::wait( 1 ) ;
}

void utility::wait( int time )
{
	QEventLoop e ;

	utility::Timer( 1,[ & ]( int counter ){

		if( counter == time ){

			e.exit() ;
			return true ;
		}else{
			return false ;
		}
	} ) ;

	e.exec() ;
}

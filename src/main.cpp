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

#include "mainwindow.h"
#include "settings.h"
#include "translator.h"
#include "utility"

#include <cstring>
#include <cstdlib>

#include <QFile>
#include <QTextStream>

int main( int argc,char * argv[] )
{
	if( utility::platformIsWindows() ){

		if( argc > 2 && std::strcmp( argv[ 1 ],"-T" ) == 0 ){

			return utility::terminateProcess( std::strtoul( argv[ 2 ],nullptr,10 ) ) ;
		}
	}

	settings settings ;

	QApplication app( argc,argv ) ;

	app.setApplicationName( "media-downloader" ) ;

	auto m = settings.darkMode() ;

	if( !m.isEmpty() ){

		QFile file( settings::darkModes::themeFileName( m ) ) ;
		file.open( QFile::ReadOnly ) ;
		app.setStyleSheet( QString( file.readAll() ) ) ;
	}

	translator translator( settings,app ) ;

	return MainWindow( app,settings,translator ).exec() ;
}

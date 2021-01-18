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
#include <QApplication>
#include <QSettings>

int main( int argc,char * argv[] )
{
	QSettings settings( "media-downloader","media-downloader" ) ;

	if( !settings.contains( settings::EnabledHighDpiScalingFactor ) ){

		settings.setValue( settings::EnabledHighDpiScalingFactor,"1.0" ) ;
	}

	auto m = settings.value( settings::EnabledHighDpiScalingFactor ).toByteArray() ;

	if( m != "1.0" ){

		QApplication::setAttribute( Qt::AA_EnableHighDpiScaling ) ;

		qputenv( "QT_SCALE_FACTOR",m ) ;
	}

	QApplication a( argc,argv ) ;

	MainWindow w( settings ) ;
	w.show() ;

	return a.exec() ;
}

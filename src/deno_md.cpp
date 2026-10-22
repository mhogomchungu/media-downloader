/*
 *
 *  Copyright (c) 2026
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

/*
 * Deno does not work as expected in flatpak because the application starts with cwd not set.
 * We set it to a path that is always available before calling deno.
 */

#include <iostream>
#include <QFile>
#include <QCoreApplication>
#include <QStringList>
#include <QString>
#include <QProcess>
#include <QStandardPaths>
#include <QDir>

static QString appPath()
{
	auto s = QStandardPaths::standardLocations( QStandardPaths::AppDataLocation ) ;

	if( s.isEmpty() ){

		//?????
		return "" ;
	}else{
		return s.first() + "/media-downloader/" ;
	}
}

static QString denoPath( const QString& basePath )
{
	return basePath + "/bin/deno" ;
}

static QString tmpPath( const QString& basePath )
{
	return basePath + "/tmp" ;
}

static QByteArray stdinData()
{
	QFile file ;

	if( file.open( stdin,QIODevice::ReadOnly ) ){

		return file.readAll() ;
	}else{
		return {} ;
	}
}

int main( int argc,char * argv[] )
{
	auto basePath = appPath() ;

	QCoreApplication app( argc,argv ) ;

	Q_UNUSED( app )

	QStringList args ;

	for( int i = 1 ; i < argc ; i++ ){

		args.append( argv[ i ] ) ;
	}

	QDir::setCurrent( tmpPath( basePath ) ) ;

	QProcess deno ;

	deno.start( denoPath( basePath ),args ) ;

	deno.waitForStarted() ;

	deno.write( stdinData() ) ;

	deno.closeWriteChannel() ;

	deno.waitForFinished() ;

	auto m = deno.exitCode() ;

	if( m == 0 ){

		std::cout << deno.readAllStandardOutput().data() ;
	}else{
		std::cerr << deno.readAllStandardError().data() ;
	}

	return m ;
}

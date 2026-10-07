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
 * In flatpak, deno sends "Failed getting cwd: No such file or directory (os error 2)"
 * warnind to std error before printing version info to std out.
 *
 * yt-dlp, combines both outputs and then gets confused by the above warning and rejects deno
 * when it calls deno with "--version" argument to get its version.
 *
 * The purpose of this binary is to supress deno warning.
 */

#include <iostream>
#include <QFile>
#include <QCoreApplication>
#include <QStringList>
#include <QString>
#include <QProcess>
#include <QStandardPaths>

static QString denoPath()
{
	auto s = QStandardPaths::standardLocations( QStandardPaths::AppDataLocation ) ;

	if( s.isEmpty() ){

		//?????
		return "" ;
	}else{
		return s.first() + "/media-downloader/bin/deno" ;
	}
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
	auto exe = denoPath() ;

	QCoreApplication app( argc,argv ) ;

	Q_UNUSED( app )

	QStringList args ;

	for( int i = 1 ; i < argc ; i++ ){

		args.append( argv[ i ] ) ;
	}

	QProcess deno ;

	deno.start( exe,args ) ;

	deno.waitForStarted() ;

	deno.write( stdinData() ) ;

	deno.closeWriteChannel() ;

	deno.waitForFinished() ;

	auto m = deno.exitCode() ;

	if( m == 0 ){

		std::cout << deno.readAllStandardOutput().data() ;
	}else{
		auto m = deno.readAllStandardError() ;

		auto s = "Failed getting cwd: No such file or directory (os error 2)" ;

		std::cerr << m.replace( s,"" ).trimmed().data() ;
	}

	return m ;
}

/*
 *
 *  Copyright (c) 2022
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
#include "tests.h"

static const char * safaribooks = R"R($ python3 safaribooks.py --cred "my_email@gmail.com:MyPassword1!" 9781491958698

       ____     ___         _
      / __/__ _/ _/__ _____(_)
     _\ \/ _ `/ _/ _ `/ __/ /
    /___/\_,_/_/ \_,_/_/ /_/
      / _ )___  ___  / /__ ___
     / _  / _ \/ _ \/  '_/(_-<
    /____/\___/\___/_/\_\/___/

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
[-] Logging into Safari Books Online...
[*] Retrieving book info...
[-] Title: Test-Driven Development with Python, 2nd Edition
[-] Authors: Harry J.W. Percival
[-] Identifier: 9781491958698
[-] ISBN: 9781491958704
[-] Publishers: O'Reilly Media, Inc.
[-] Rights: Copyright © O'Reilly Media, Inc.
[-] Description: By taking you through the development of a real web application
from beginning to end, the second edition of this hands-on guide demonstrates the
practical advantages of test-driven development (TDD) with Python. You’ll learn
how to write and run tests before building each part of your app, and then develop
the minimum amount of code required to pass those tests. The result? Clean code
that works.In the process, you’ll learn the basics of Django, Selenium, Git,
jQuery, and Mock, along with curre...
[-] Release Date: 2017-08-18
[-] URL: https://learning.oreilly.com/library/view/test-driven-development-with/9781491958698/
[*] Retrieving book chapters...
[*] Output directory:
    /XXXX/safaribooks/Books/Test-Driven Development with Python 2nd Edition (9781491958698)
[-] Downloading book contents... (53 chapters)
    [----------------------------------------------------------------------------------------------------------------------------------------------------------------] 0%
    [###############################----------------------------------------------------------------------------------------] 50%
    [#####################################################################] 100%
[-] Downloading book CSSs... (2 files)
    [----------------------------------------------------------------------------------------------------------------------------------------------------------------] 0%
    [###############################----------------------------------------------------------------------------------------] 50%
    [#####################################################################] 100%
    [-] Downloading book images... (142 files)
    [----------------------------------------------------------------------------------------------------------------------------------------------------------------] 0%
    [###############################----------------------------------------------------------------------------------------] 50%
    [#####################################################################] 100%
[-] Creating EPUB file...
[*] Done: /XXXX/safaribooks/Books/Test-Driven Development with Python 2nd Edition
(9781491958698)/9781491958698.epub

    If you like it, please * this project on GitHub to make it known:
	https://github.com/lorenzodifuccia/safaribooks
    e don't forget to renew your Safari Books Online subscription:
	https://learning.oreilly.com

[!] Bye!!)R" ;

static const char * yt_dlp = R"R([youtube] Gkz9F4GGxOs: Downloading webpage
[youtube] Gkz9F4GGxOs: Downloading android player API JSON
[info] Gkz9F4GGxOs: Downloading 1 format(s): 242+250
[download] Destination: Так мы ещё не тонули) хорошее начало гонки) [Gkz9F4GGxOs].f242.webm
[download]   0.0% of 42.90MiB at   22.04KiB/s ETA 33:16
[download]   0.0% of 42.90MiB at   63.79KiB/s ETA 11:28
[download]   0.0% of 42.90MiB at  146.31KiB/s ETA 05:00
[download]   0.0% of 42.90MiB at  309.76KiB/s ETA 02:21
[download]   0.1% of 42.90MiB at  350.52KiB/s ETA 02:05
[download]   0.1% of 42.90MiB at  461.81KiB/s ETA 01:35
[download]   0.3% of 42.90MiB at  410.63KiB/s ETA 01:46
[download]   0.6% of 42.90MiB at  241.69KiB/s ETA 03:00
[download]   1.0% of 42.90MiB at  164.48KiB/s ETA 04:24
[download]   1.2% of 42.90MiB at  166.03KiB/s ETA 04:21
[download]   1.6% of 42.90MiB at  171.56KiB/s ETA 04:11
[download]   2.1% of 42.90MiB at  176.84KiB/s ETA 04:03
[download]   2.5% of 42.90MiB at  178.79KiB/s ETA 03:59
[download]   2.9% of 42.90MiB at  186.78KiB/s ETA 03:48
[download]  69.1% of 42.90MiB at  152.17KiB/s ETA 01:29
[download]  99.5% of 42.90MiB at  135.25KiB/s ETA 00:01
[download]  99.7% of 42.90MiB at  134.26KiB/s ETA 00:01
[download]  99.9% of 42.90MiB at  134.83KiB/s ETA 00:00
[download] 100.0% of 42.90MiB at  134.53KiB/s ETA 00:00
[download] 100% of 42.90MiB in 04:55
[download] Destination: Так мы ещё не тонули) хорошее начало гонки) [Gkz9F4GGxOs].f250.webm
[download]   0.0% of 11.47MiB at    7.37KiB/s ETA 26:34
[download]   0.0% of 11.47MiB at   21.91KiB/s ETA 08:56
[download]   0.1% of 11.47MiB at   50.74KiB/s ETA 03:51
[download]   0.1% of 11.47MiB at  107.71KiB/s ETA 01:48
[download]   0.3% of 11.47MiB at  130.00KiB/s ETA 01:30
[download]  95.4% of 11.47MiB at  137.81KiB/s ETA 00:03
[download]  96.6% of 11.47MiB at  138.09KiB/s ETA 00:02
[download]  97.8% of 11.47MiB at  138.81KiB/s ETA 00:01
[download]  99.0% of 11.47MiB at  136.19KiB/s ETA 00:00
[download] 100.0% of 11.47MiB at  136.42KiB/s ETA 00:00
[download] 100% of 11.47MiB in 01:08
[Merger] Merging formats into "Так мы ещё не тонули) хорошее начало гонки) [Gkz9F4GGxOs].webm"
Deleting original file Так мы ещё не тонули) хорошее начало гонки) [Gkz9F4GGxOs].f250.webm (pass -k to keep)
Deleting original file Так мы ещё не тонули) хорошее начало гонки) [Gkz9F4GGxOs].f242.webm (pass -k to keep))R" ;

#include "../util.hpp"
#include <iostream>

#include <QString>
#include <QEventLoop>

static bool _run_test( const QStringList& args )
{
	for( const auto& it : args ){

		if( it.startsWith( "--media-downloader-test-engine" ) ){

			return true ;
		}
	}

	return false ;
}

bool tests::test_engine( const QStringList& args,QApplication& app )
{
	if( _run_test( args ) ){

		class myApp
		{
		public:
			struct args
			{
				const QStringList& args ;
				QApplication& app ;
			} ;
			myApp( const myApp::args& args ) : m_args( args )
			{
			}
			void start( const QByteArray& )
			{
				for( const auto& it : m_args.args ){

					if( it.startsWith( "--media-downloader-test-engine-safaribooks" ) ){

						return this->testEngine( safaribooks ) ;

					}else if( it.startsWith( "--media-downloader-test-engine-yt-dlp" ) ){

						return this->testEngine( yt_dlp ) ;
					}
				}

				this->done() ;
			}
			void done()
			{
				m_args.app.quit() ;
			}
			void testEngine( const char * output )
			{
				m_list = util::split( output,'\n' ) ;

				util::Timer( 500,[ this ]( int ){

					if( m_counter < m_list.size() ){

						std::cout << m_list.at( m_counter ).constData() << std::endl ;

						m_counter++ ;

						return false ;
					}else{
						this->done() ;

						return true ;
					}
				} ) ;
			}
		private:
			QList< QByteArray > m_list ;
			myApp::args m_args ;
			int m_counter = 0 ;
		};

		myApp::args mapp{ args,app } ;
		util::multipleInstance< myApp,myApp::args >( app,std::move( mapp ),QByteArray() ).exec() ;

		return true ;
	}else{
		return false ;
	}
}

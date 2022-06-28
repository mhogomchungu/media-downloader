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

static const char * safaribooks = R"R(
$ python3 safaribooks.py --cred "my_email@gmail.com:MyPassword1!" 9781491958698

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

[!] Bye!!
)R" ;

#include "../util.hpp"
#include <iostream>

#include <QString>
#include <QEventLoop>

template< typename testClass >
static void test_safaribooks( testClass& tc )
{
	util::Timer( 500,[ list = util::split( safaribooks,'\n' ),&tc,counter = 0 ]( int )mutable{

		if( counter < list.size() ){

			std::cout << list.at( counter ).toStdString() << std::endl ;

			counter++ ;

			return false ;
		}else{
			tc.done() ;
			return true ;
		}
	} ) ;
}

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

						return test_safaribooks( *this ) ;
					}
				}

				this->done() ;
			}
			void done()
			{
				m_args.app.quit() ;
			}
		private:
			myApp::args m_args ;
		};

		util::multipleInstance< myApp,myApp::args >( app,{ myApp::args{ args,app } },{} ).exec() ;

		return true ;
	}else{
		return false ;
	}
}

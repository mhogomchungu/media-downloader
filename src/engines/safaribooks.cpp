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

#include "safaribooks.h"

#include "../utility.h"

#include <QJsonObject>
#include <QJsonArray>

const char * safaribooks::testData()
{
	return R"R($ python3 safaribooks.py --cred "my_email@gmail.com:MyPassword1!" 9781491958698

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
}

safaribooks::safaribooks( const engines& engines,const engines::engine& engine,QJsonObject& object ) :
	engines::engine::baseEngine( engines.Settings(),engine,engines.processEnvironment() ),
	m_engine( engine )
{
	if( !object.contains( "ControlJsonStructure" ) ){

		QJsonObject obj ;

		obj.insert( "Connector","&&" ) ;

		obj.insert( "lhs",[](){

			QJsonObject obj ;

			obj.insert( "containsAll",[](){

				QJsonArray arr ;

				arr.append( "[" ) ;
				arr.append( "]" ) ;

				return arr ;
			}() ) ;

			return obj ;
		}() ) ;

		obj.insert( "rhs",[](){

			QJsonObject obj ;

			obj.insert( "endsWith","%" ) ;

			return obj ;
		}() ) ;

		object.insert( "ControlJsonStructure",obj ) ;
	}

	if( !object.contains( "RemoveText" ) ){

		object.insert( "RemoveText",[](){

			QJsonArray arr ;

			arr.append( "\033[0m" ) ;
			arr.append( "\033[33m" ) ;
			arr.append( "\033[41m" ) ;
			arr.append( "\033[43m" ) ;

			return arr ;
		}() ) ;
	}

	if( !object.contains( "SplitLinesBy" ) ){

		object.insert( "SplitLinesBy",[](){

			QJsonArray arr ;

			arr.append( "\r" ) ;
			arr.append( "\n" ) ;

			return arr ;
		}() ) ;
	}
}

safaribooks::~safaribooks()
{
}

QString safaribooks::commandString( const engines::engine::exeArgs::cmd& cmd )
{
	auto m = "\"" + cmd.exe() + "\"" ;

	const auto& args = cmd.args() ;

	for( int i = 0 ; i < args.size() ; i++ ){

		if( i > 0 && args[ i - 1 ] == "--cred" ){
			m += " \"" + args[ i ] + "\"" ;

			//m += " \"" + QObject::tr( "<REDACTED>" ) + "\"" ;
		}else{
			m += " \"" + args[ i ] + "\"" ;
		}
	}

	return m ;
}

void safaribooks::sendCredentials( const QString& credentials,QProcess& exe )
{
	if( utility::platformIsNOTWindows() ){

		if( credentials.isEmpty() ){

			exe.write( m_engine.userName().toUtf8() + "\n" ) ;
			exe.write( m_engine.password().toUtf8() + "\n" ) ;
		}else{
			auto m = util::split( credentials,':',true ) ;

			if( m.size() > 1 ){

				exe.write( m.at( 0 ).toUtf8() + "\n" ) ;
				exe.write( m.at( 1 ).toUtf8() + "\n" ) ;
			}
		}

		exe.closeWriteChannel() ;
	}
}

static bool _credential_line( const QString& m )
{
	return m.contains( "@" ) && m.contains( ":" ) ;
}

static QString _find_credentials( const QStringList& cred )
{
	for( int i = 0 ; i < cred.size() ; i++ ){

		const auto& m = cred[ i ] ;

		if( _credential_line( m ) ){

			return m ;
		}
	}

	return QString() ;
}

static void _clear( QStringList& m )
{
	for( int i = 0 ; i < m.size() ; ){

		const auto& e = m[ i ] ;

		if( _credential_line( e ) || e == "--cred" ){

			m.removeAt( i ) ;
		}else{
			i++ ;
		}
	}
}

QString safaribooks::setCredentials( QStringList& uiOptions,QStringList& otherOptions )
{
	auto m = _find_credentials( uiOptions ) ;

	if( m.isEmpty() ){

		m = _find_credentials( otherOptions ) ;
	}

	_clear( uiOptions ) ;
	_clear( otherOptions ) ;

	return m ;
}

void safaribooks::updateDownLoadCmdOptions( const engines::engine::baseEngine::updateOpts& s,
					    bool e,
					    const QStringList& extraOpts )
{
	if( s.urls.size() > 0 ){

		const auto m = util::split( s.urls[ 0 ],'/',true ) ;

		s.urls[ 0 ] = m.last() ;
	}

	s.ourOptions.append( "--destination" ) ;
	s.ourOptions.append( engines::engine::baseEngine::Settings().downloadFolder() ) ;

	if( utility::platformIsWindows() || !s.credentials.isEmpty() ){

		s.ourOptions.append( "--cred" ) ;

		if( s.credentials.isEmpty() ){

			s.ourOptions.append( m_engine.userName() + ":" + m_engine.password() ) ;
		}else{
			s.ourOptions.append( s.credentials ) ;
		}
	}else{
		s.ourOptions.append( "--login" ) ;
	}

	engines::engine::baseEngine::updateDownLoadCmdOptions( s,e,extraOpts ) ;
}

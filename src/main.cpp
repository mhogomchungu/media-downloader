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
#include "util.hpp"

class myApp
{
public:
	struct args
	{
		QApplication& app ;
		settings& s ;
		const QStringList& args ;
	};
	myApp( const myApp::args& a ) :
		m_traslator( a.s,a.app ),
		m_app( a.app,a.s,m_traslator,a.args )
	{
	}
	void start( const QByteArray& e )
	{
		m_app.Show() ;
		m_app.processEvent( e ) ;
	}
	void exit()
	{
		m_app.quitApp() ;
	}
	void event( const QByteArray& e )
	{
		m_app.processEvent( e ) ;
	}
	void anotherInstanceRunning()
	{
		//m_app.log( QObject::tr( "There seem to be another instance running,exiting this one" ) ) ;
	}
	void previousVersionCrashed()
	{
		//m_app.log( QObject::tr( "Previous instance seem to have crashed,trying to clean up before starting" ) ) ;
	}
private:
	translator m_traslator ;
	MainWindow m_app ;
};

int main( int argc,char * argv[] )
{
	const auto m = utility::Terminator::terminate( argc,argv ) ;

	if( m ){

		return m.value() ;
	}else{
		settings settings ;

		QApplication mqApp( argc,argv ) ;

		settings.setTheme( mqApp ) ;

		mqApp.setApplicationName( "media-downloader" ) ;

		auto args = mqApp.arguments() ;

		auto spath = engines::enginePaths( settings ).socketPath() ;

		utility::arguments opts( args ) ;

		QJsonObject jsonArgs ;

		jsonArgs.insert( "-u",opts.hasValue( "-u" ) ) ;
		jsonArgs.insert( "-a",opts.hasOption( "-a" ) ) ;
		jsonArgs.insert( "-s",opts.hasOption( "-s" ) ) ;

		auto json = QJsonDocument( jsonArgs ).toJson( QJsonDocument::Indented ) ;

		if( opts.hasOption( "-s" ) || !settings.singleInstance() ){

			myApp::args mOpts{ mqApp,settings,args } ;

			myApp mApp( mOpts ) ;

			mApp.start( json ) ;

			return mqApp.exec() ;
		}else{
			util::oneinstance< myApp,myApp::args > instance( spath,json,{ mqApp,settings,args } ) ;

			return mqApp.exec() ;
		}
	}
}

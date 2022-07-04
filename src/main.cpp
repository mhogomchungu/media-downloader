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
#include "engines/tests.h"

class myApp
{
public:
	struct args
	{
		QApplication& app ;
		settings& Settings ;
		const QStringList& args ;
	};
	myApp( const myApp::args& args ) :
		m_traslator( args.Settings,args.app ),
		m_app( args.app,args.Settings,m_traslator,args.args )
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
private:
	translator m_traslator ;
	MainWindow m_app ;
};

int main( int argc,char * argv[] )
{
	settings settings ;

	QApplication mqApp( argc,argv ) ;

	engines::enginePaths paths( settings ) ;

	settings.setTheme( mqApp,paths.themePath() ) ;

	mqApp.setApplicationName( "media-downloader" ) ;

	auto args = mqApp.arguments() ;

	//if( tests::test_engine( args,mqApp ) ){

	//	return 0 ;
	//}

	auto spath = paths.socketPath() ;

	utility::arguments opts( args ) ;

	QJsonObject jsonArgs ;

	jsonArgs.insert( "-u",opts.hasValue( "-u" ) ) ;
	jsonArgs.insert( "-a",opts.hasOption( "-a" ) ) ;
	jsonArgs.insert( "-s",opts.hasOption( "-s" ) ) ;

	auto json = QJsonDocument( jsonArgs ).toJson( QJsonDocument::Indented ) ;

	myApp::args mArgs{ mqApp,settings,args } ;

	if( opts.hasOption( "-s" ) || !settings.singleInstance() ){

		return util::multipleInstance< myApp,myApp::args >( mqApp,std::move( mArgs ),json ).exec() ;
	}else{
		auto instanceArgs = util::make_oneinstance_args( [ & ](){

			std::cout << "There seem to be another instance running,exiting this one" << std::endl ;
			mqApp.exit() ;
		},[](){
			std::cout << "Previous instance seem to have crashed,trying to clean up before starting" << std::endl ;
		} ) ;

		using type = decltype( instanceArgs ) ;

		util::oneinstance< myApp,myApp::args,type > instance( spath,
								      json,
								      std::move( mArgs ),
								      std::move( instanceArgs ) ) ;

		return mqApp.exec() ;
	}
}

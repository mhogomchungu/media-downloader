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
#include "utils/single_instance.hpp"
#include "engines/tests.h"

class myApp
{
public:
	struct args
	{
		QApplication& app ;
		settings& Settings ;
		const engines::enginePaths& ePaths ;
		const utility::cliArguments& cargs ;
	};
	myApp( const myApp::args& args ) :
		m_traslator( args.Settings,args.app ),
		m_app( args.app,args.Settings,m_traslator,args.ePaths,args.cargs )
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
	void hasEvent( const QByteArray& e )
	{
		m_app.processEvent( e ) ;
	}
private:
	translator m_traslator ;
	MainWindow m_app ;
};

int main( int argc,char * argv[] )
{
	utility::cliArguments cargs( argc,argv ) ;

	if( utility::onlyWantedVersionInfo( cargs ) ){

		return 0 ;
	}

	settings settings( cargs ) ;

	if( utility::startedUpdatedVersion( settings,cargs ) ){

		return 0 ;
	}

	QApplication mqApp( argc,argv ) ;

	engines::enginePaths paths( settings ) ;

	settings.setTheme( mqApp,paths.themePath() ) ;

	const auto& args = cargs.arguments() ;

	if( tests::test_engine( args,mqApp ) ){

		return 0 ;
	}

	auto spath = paths.socketPath() ;

	QJsonObject jsonArgs ;

	jsonArgs.insert( "-u",cargs.value( "-u" ) ) ;
	jsonArgs.insert( "-a",cargs.value( "-a" ) ) ;
	jsonArgs.insert( "-s",cargs.value( "-s" ) ) ;
	jsonArgs.insert( "--proxy",cargs.value( "--proxy" ) ) ;

	auto json = QJsonDocument( jsonArgs ).toJson( QJsonDocument::Indented ) ;

	utils::app::appInfo< myApp,myApp::args > m( { mqApp,settings,paths,cargs },spath,mqApp,json ) ;

	if( cargs.contains( "-s" ) || !settings.singleInstance() ){

		return utils::app::runMultiInstances( std::move( m ) ) ;
	}else{
		return utils::app::runOneInstance( std::move( m ) ) ;
	}
}

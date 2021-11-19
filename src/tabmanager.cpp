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

#include "tabmanager.h"

tabManager::tabManager( settings& s,
			translator& t,
			engines& e,
			Logger& l,
			Ui::MainWindow& ui,
			QWidget& w,
			MainWindow& mw,
			utility::versionInfo& u,
			QString debug ) :
	m_currentTab( s.tabNumber() ),
	m_ctx( s,t,ui,w,mw,l,e,u,*this,debug ),
	m_about( m_ctx ),
	m_configure( m_ctx ),
	m_basicdownloader( m_ctx ),
	m_batchdownloader( m_ctx ),
	m_playlistdownloader( m_ctx ),
	m_library( m_ctx )
{
	u.setContext( m_ctx ) ;

	const auto& engines = m_ctx.Engines().getEngines() ;

	if( engines.size() > 0 ){

		if( s.showVersionInfoWhenStarting() ){

			s.setTabNumber( 0 ) ;

			ui.tabWidget->setCurrentIndex( 0 ) ;

			m_ctx.logger().updateView( true ) ;

			auto& vinfo = m_ctx.versionInfo() ;

			m_initConnection = QObject::connect( &vinfo,&utility::versionInfo::vinfoDone,[ this,&ui,&s ](){

				QObject::disconnect( m_initConnection ) ;

				this->init_done( ui,s ) ;
			} ) ;

			vinfo.check( engines ) ;
		}else{
			this->init_done( ui,s ) ;
		}
	}else{
		this->disableAll() ;

		ui.pbQuit->setEnabled( true ) ;
	}
}

void tabManager::init_done(Ui::MainWindow & ui, settings & settings)
{
	this->setDefaultEngines() ;

	m_about.init_done() ;
	m_configure.init_done() ;
	m_basicdownloader.init_done() ;
	m_batchdownloader.init_done() ;
	m_playlistdownloader.init_done() ;
	m_library.init_done() ;

	auto& m = ui ;
	auto& s = settings ;

	m.tabWidget->setCurrentIndex( s.tabNumber() ) ;

	switch( s.tabNumber() ) {
		case 0 : m_basicdownloader.tabEntered() ; break ;
		case 1 : m_batchdownloader.tabEntered() ; break ;
		case 2 : m_playlistdownloader.tabEntered() ; break ;
		case 3 : m_library.tabEntered() ; break ;
		case 4 : m_configure.tabEntered() ; break ;
		case 5 : m_about.tabEntered() ; break ;
	}

	QObject::connect( m.tabWidget,&QTabWidget::currentChanged,[ this ]( int index ){

		switch( index ) {
			case 0 : m_basicdownloader.tabEntered() ; break ;
			case 1 : m_batchdownloader.tabEntered() ; break ;
			case 2 : m_playlistdownloader.tabEntered() ; break ;
			case 3 : m_library.tabEntered() ; break ;
			case 4 : m_configure.tabEntered() ; break ;
			case 5 : m_about.tabEntered() ; break ;
		}

		if( m_currentTab != index ){

			switch( m_currentTab ) {
				case 0 : m_basicdownloader.tabExited() ; break ;
				case 1 : m_batchdownloader.tabExited() ; break ;
				case 2 : m_playlistdownloader.tabExited() ; break ;
				case 3 : m_library.tabExited() ; break ;
				case 4 : m_configure.tabExited() ; break ;
				case 5 : m_about.tabExited() ; break ;
			}

			m_currentTab = index ;
		}
	} ) ;
}

void tabManager::setDefaultEngines()
{
	QStringList s ;

	for( const auto& engine : m_ctx.Engines().getEngines() ){

		if( engine.mainEngine() && engine.backendExists() && !engine.broken() ){

			s.append( engine.name() ) ;
		}
	}

	m_basicdownloader.updateEnginesList( s ) ;
	m_batchdownloader.updateEnginesList( s ) ;
	m_playlistdownloader.updateEnginesList( s ) ;
	m_configure.updateEnginesList( s ) ;
}

tabManager& tabManager::gotEvent( const QByteArray& e )
{
	m_basicdownloader.gotEvent( e ) ;
	m_batchdownloader.gotEvent( e ) ;
	m_playlistdownloader.gotEvent( e ) ;

	return *this ;
}

tabManager& tabManager::enableAll()
{
	m_about.enableAll() ;
	m_configure.enableAll() ;
	m_basicdownloader.enableAll() ;
	m_batchdownloader.enableAll() ;
	m_playlistdownloader.enableAll() ;
	m_library.enableAll() ;

	m_uiEnabled = true ;

	return *this ;
}

tabManager& tabManager::disableAll()
{
	m_about.disableAll() ;
	m_configure.disableAll() ;
	m_basicdownloader.disableAll() ;
	m_batchdownloader.disableAll() ;
	m_playlistdownloader.disableAll() ;
	m_library.disableAll() ;

	m_uiEnabled = false ;

	return *this ;
}

tabManager& tabManager::resetMenu()
{
	m_about.resetMenu() ;
	m_configure.resetMenu() ;
	m_basicdownloader.resetMenu() ;
	m_batchdownloader.resetMenu() ;
	m_playlistdownloader.resetMenu() ;
	m_library.resetMenu() ;

	return *this ;
}

tabManager& tabManager::reTranslateUi()
{
	m_about.retranslateUi() ;
	m_configure.retranslateUi() ;
	m_basicdownloader.retranslateUi() ;
	m_batchdownloader.retranslateUi() ;
	m_playlistdownloader.retranslateUi() ;
	m_library.retranslateUi() ;

	return *this ;
}

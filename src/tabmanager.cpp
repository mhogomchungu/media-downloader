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
#include "proxy.h"

#include <QMimeData>
#include <QClipboard>
#include <QDateTime>

tabManager::tabManager( settings& s,
			translator& t,
			engines& e,
			Logger& l,
			Ui::MainWindow& ui,
			QWidget& w,
			MainWindow& mw,
			const QString& appName,
			utility::printOutPut& op ) :
	m_currentTab( s.tabNumber() ),
	m_ctx( s,t,ui,w,mw,l,e,*this,appName,op ),
	m_about( m_ctx ),
	m_configure( m_ctx ),
	m_basicdownloader( m_ctx ),
	m_batchdownloader( m_ctx ),
	m_playlistdownloader( m_ctx ),
	m_library( m_ctx )
{
	t.setContext( m_ctx ) ;

	qRegisterMetaType< QClipboard::Mode >() ;

	m_clipboard = QApplication::clipboard() ;

	if( m_clipboard ){

		auto m = Qt::QueuedConnection ;
		QObject::connect( m_clipboard,&QClipboard::changed,this,&tabManager::clipboardEvent,m ) ;
	}

	const auto& engines = m_ctx.Engines().getEngines() ;

	if( engines.size() > 0 ){

		ui.tabWidget->setCurrentIndex( 0 ) ;

		m_ctx.logger().updateView( true ) ;

		m_ctx.getVersionInfo().checkMediaDownloaderUpdate( engines ) ;
	}else{
		this->disableAll() ;

		ui.pbQuit->setEnabled( true ) ;
	}
}

void tabManager::init_done()
{
	auto& m = m_ctx.Ui() ;
	auto& s = m_ctx.Settings() ;

	this->setDefaultEngines() ;

	m_about.init_done() ;
	m_configure.init_done() ;
	m_basicdownloader.init_done() ;
	m_batchdownloader.init_done() ;
	m_playlistdownloader.init_done() ;
	m_library.init_done() ;

	utility::initDone() ;

	if( s.tabNumber() == 3 ){

		//We do not want to start with a library tab because it may hang the UI
		//before it is completely visible

		s.setTabNumber( 0 ) ;
	}

	m.tabWidget->setCurrentIndex( s.tabNumber() ) ;

	switch( s.tabNumber() )
	{
		case 0 : m_basicdownloader.tabEntered() ; break ;
		case 1 : m_batchdownloader.tabEntered() ; break ;
		case 2 : m_playlistdownloader.tabEntered() ; break ;
		case 3 : m_library.tabEntered() ; break ;
		case 4 : m_configure.tabEntered() ; break ;
		case 5 : m_about.tabEntered() ; break ;
	}

	QObject::connect( m.tabWidget,&QTabWidget::currentChanged,[ this ]( int index ){

		switch( index )
		{
			case 0 : m_basicdownloader.tabEntered() ; break ;
			case 1 : m_batchdownloader.tabEntered() ; break ;
			case 2 : m_playlistdownloader.tabEntered() ; break ;
			case 3 : m_library.tabEntered() ; break ;
			case 4 : m_configure.tabEntered() ; break ;
			case 5 : m_about.tabEntered() ; break ;
		}

		if( m_currentTab != index ){

			switch( m_currentTab )
			{
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

	s.init_done() ;
}

void tabManager::setDefaultEngines()
{
	QStringList s ;

	for( const auto& engine : m_ctx.Engines().getEngines() ){

		if( !engine.supportingEngine() && engine.backendExists() && !engine.broken() ){

			s.append( engine.name() ) ;
		}
	}

	m_basicdownloader.updateEnginesList( s ) ;
	m_batchdownloader.updateEnginesList( s ) ;
	m_playlistdownloader.updateEnginesList( s ) ;
	m_configure.updateEnginesList( s ) ;
}

void tabManager::setProxy( const settings::proxySettings& proxy,const settings::proxySettings::type& m )
{
	proxy::set( m_ctx,m_firstTimeSettingProxy,proxy.proxyAddress(),m ) ;
}

void tabManager::clipboardEvent( QClipboard::Mode mode )
{
	if( mode == QClipboard::Mode::Clipboard ){

		if( utility::platformIsWindows() ){

			//this->bgThreadClipboardHandler() ;
			this->mainThreadClipboardHandler() ;
		}else{
			this->mainThreadClipboardHandler() ;
		}
	}
}

void tabManager::mainThreadClipboardHandler()
{
	auto e = m_clipboard->mimeData() ;

	if( e && e->hasText() ){

		auto m = e->text() ;

		if( m.startsWith( "http" ) || m.startsWith( "yt-dlp " ) ){

			m_batchdownloader.clipboardData( m,true ) ;
		}
	}
}

void tabManager::bgThreadClipboardHandler()
{
	class timeOutMonitor
	{
	public:
		timeOutMonitor( tabManager& parent ) :
			m_timeOut( parent.m_ctx.Settings().timeOutWaitingForClipboardData() ),
			m_then( m_timeOut > 0 ? QDateTime::currentMSecsSinceEpoch() : 0 )
		{
		}
		bool notTimedOut() const
		{
			if( m_timeOut > 0 ){

				auto now = QDateTime::currentMSecsSinceEpoch() ;

				return ( now - m_then ) <= m_timeOut ;
			}else{
				return true ;
			}
		}
	private:
		qint64 m_timeOut ;
		qint64 m_then ;
	} ;

	class meaw
	{
	public:
		meaw( tabManager& parent ) :
			m_parent( parent ),
			m_timer( m_parent ),
			m_id( m_parent.m_ctx.nativeHandleToMainWindow() )
		{
		}
		QString bg()
		{
			return utility::windowsGetClipBoardText( m_id ) ;
		}
		void fg( const QString& e )
		{
			if( m_timer.notTimedOut() ){

				if( e.startsWith( "http" ) || e.startsWith( "yt-dlp " ) ){

					m_parent.m_batchdownloader.clipboardData( e,true ) ;
				}
			}else{
				auto a = QObject::tr( "Warning: Skipping Clipboard Content" ) ;
				m_parent.m_batchdownloader.clipboardData( a,false ) ;
			}
		}
	private:
		tabManager& m_parent ;
		timeOutMonitor m_timer ;
		ContextWinId m_id ;
	} ;

	utils::qthread::run( meaw( *this ) ) ;
}

tabManager& tabManager::gotEvent( const QByteArray& s )
{
	QJsonParseError err ;
	auto jsonDoc = QJsonDocument::fromJson( s,&err ) ;

	if( err.error == QJsonParseError::NoError ){

		auto e = jsonDoc.object() ;

		if( m_firstTimeSettingProxy ){

			auto m = e.value( "--proxy" ).toString() ;

			if( m.isEmpty() ){

				auto s = m_ctx.Settings().getProxySettings() ;
				auto t = s.types() ;

				if( !t.none() ){

					this->setProxy( s,t ) ;
				}else{
					m_ctx.setNetworkProxy( m,m_firstTimeSettingProxy ) ;
				}
			}else{
				m_ctx.setNetworkProxy( m,m_firstTimeSettingProxy ) ;
			}

			m_firstTimeSettingProxy = false ;
		}

		m_basicdownloader.gotEvent( e ) ;
		m_batchdownloader.gotEvent( e ) ;
		m_playlistdownloader.gotEvent( e ) ;
	}

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

	if( m_firstTime ){

		this->setDefaultEngines() ;

		m_firstTime =  false ;
	}

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
	m_playlistdownloader.retranslateUi() ;
	m_library.retranslateUi() ;
	m_batchdownloader.retranslateUi() ;

	return *this ;
}

tabManager& tabManager::exiting()
{
	m_about.exiting() ;
	m_configure.exiting() ;
	m_basicdownloader.exiting() ;
	m_batchdownloader.exiting() ;
	m_playlistdownloader.exiting() ;
	m_library.exiting() ;

	return *this ;
}

void tabManager::keyPressed( utility::mainWindowKeyCombo m )
{
	switch( m_ctx.Ui().tabWidget->currentIndex() )
	{
		case 0 : m_basicdownloader.keyPressed( m ) ; break ;
		case 1 : m_batchdownloader.keyPressed( m ) ; break ;
		case 2 : m_playlistdownloader.keyPressed( m ) ; break ;
		case 3 : m_library.keyPressed( m )         ; break ;
		case 4 : m_configure.keyPressed( m )       ; break ;
		case 5 : m_about.keyPressed( m )           ; break ;
	}
}

void tabManager::textAlignmentChanged( Qt::LayoutDirection m )
{
	m_about.textAlignmentChanged( m ) ;
	m_configure.textAlignmentChanged( m ) ;
	m_basicdownloader.textAlignmentChanged( m ) ;
	m_batchdownloader.textAlignmentChanged( m ) ;
	m_playlistdownloader.textAlignmentChanged( m ) ;
	m_library.textAlignmentChanged( m ) ;
}

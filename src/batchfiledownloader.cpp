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

#include "batchfiledownloader.h"
#include "tabmanager.h"

#include <QFileDialog>

batchfiledownloader::batchfiledownloader( const Context& ctx ) :
	m_ctx( ctx ),
	m_settings( m_ctx.Settings() ),
	m_ui( m_ctx.Ui() ),
	m_mainWindow( m_ctx.mainWidget() ),
	m_tabManager( m_ctx.TabManager() )
{
	connect( m_ui.pbFileDownloaderQuit,&QPushButton::clicked,[ this ](){

		m_tabManager.basicDownloader().appQuit() ;
	} ) ;

	connect( m_ui.pbFileDownloaderDownload,&QPushButton::clicked,[ this ](){

		this->download() ;
	} ) ;

	m_ui.pbFileDownloaderFilePath->setIcon( [](){

		return QIcon( ":file" ) ;
	}() ) ;

	connect( m_ui.pbFileDownloaderFilePath,&QPushButton::clicked,[ this ](){

		auto e = QFileDialog::getOpenFileName( &m_mainWindow,tr( "Set Batch File" ),QDir::homePath() ) ;

		if( !e.isEmpty() ){

			m_ui.lineEditFileDownloader->setText( e ) ;
		}
	} ) ;

	connect( m_ui.pbFileDownloaderOptionsHistory,&QPushButton::clicked,[ this ](){

		auto s = utility::showHistory( *m_ui.lineEditFileOptions,
					       m_settings.getOptionsHistory( settings::tabName::batch ),
					       m_settings,settings::tabName::batch ) ;

		if( s ){

			this->download() ;
		}
	} ) ;

	this->resetMenu() ;
}

void batchfiledownloader::init_done()
{
}

void batchfiledownloader::enableAll()
{
	m_ui.lineEditFileDownloader->setEnabled( true ) ;
	m_ui.lineEditFileOptions->setEnabled( true ) ;
	m_ui.pbFileDownloaderDownload->setEnabled( true ) ;
	m_ui.pbFileDownloaderFilePath->setEnabled( true ) ;
	m_ui.pbFileDownloaderOptions->setEnabled( true ) ;
	m_ui.pbFileDownloaderQuit->setEnabled( true ) ;
	m_ui.labelFileDownloader->setEnabled( true ) ;
	m_ui.labelFileDownloederPath->setEnabled( true ) ;
	m_ui.labelFileDownloederPathOptions->setEnabled( true ) ;
	m_ui.pbFileDownloaderOptionsHistory->setEnabled( true ) ;
}

void batchfiledownloader::resetMenu()
{
	utility::setMenuOptions( m_ctx,{},false,true,m_ui.pbFileDownloaderOptions,[ this ]( QAction * ac ){

		m_ui.lineEditFileOptions->setText( ac->objectName() ) ;

		this->download() ;
	} ) ;
}

void batchfiledownloader::retranslateUi()
{
	this->resetMenu() ;
}

void batchfiledownloader::tabEntered()
{
}

void batchfiledownloader::tabExited()
{
}

void batchfiledownloader::gotEvent( const QString& )
{
}

void batchfiledownloader::download()
{
	auto url = m_ui.lineEditFileDownloader->text() ;

	if( url.isEmpty() ){

		return ;
	}

	QString list = engines::file( url,m_ctx.logger() ).readAll() ;

	if( !list.isEmpty() ){

		auto l = util::split( list,'\n',true ) ;

		auto options = m_ui.lineEditFileOptions->text();

		auto mm = m_ctx.Engines().defaultEngineName() ;

		auto m = m_settings.defaultEngine( settings::tabName::batch,mm ) ;

		const auto& engine = m_ctx.Engines().defaultEngine( m ) ;

		m_tabManager.batchDownloader().download( engine,options,l ) ;
	}
}

void batchfiledownloader::disableAll()
{
	m_ui.lineEditFileDownloader->setEnabled( false ) ;
	m_ui.lineEditFileOptions->setEnabled( false ) ;
	m_ui.pbFileDownloaderDownload->setEnabled( false ) ;
	m_ui.pbFileDownloaderFilePath->setEnabled( false ) ;
	m_ui.pbFileDownloaderOptions->setEnabled( false ) ;
	m_ui.pbFileDownloaderQuit->setEnabled( false ) ;
	m_ui.labelFileDownloader->setEnabled( false ) ;
	m_ui.labelFileDownloederPath->setEnabled( false ) ;
	m_ui.labelFileDownloederPathOptions->setEnabled( false ) ;
	m_ui.pbFileDownloaderOptionsHistory->setEnabled( false ) ;
}

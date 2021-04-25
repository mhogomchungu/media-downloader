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

#include "configure.h"
#include "tabmanager.h"
#include "mainwindow.h"
#include "networkAccess.h"
#include "utility.h"

#include <QFileDialog>
#include <QFile>

configure::configure( const Context& ctx ) :
	m_ctx( ctx ),
	m_settings( m_ctx.Settings() ),
	m_ui( m_ctx.Ui() ),
	m_mainWindow( m_ctx.mainWidget() ),
	m_tabManager( m_ctx.TabManager() ),
	m_networkAccess( m_ctx )
{
	m_ui.lineEditConfigureScaleFactor->setEnabled( m_settings.enabledHighDpiScaling() ) ;

	m_setEnabled = true ;

	connect( m_ui.pbConfigureQuit,&QPushButton::clicked,[ this ](){

		this->saveOptions() ;
		m_tabManager.basicDownloader().appQuit() ;
	} ) ;

	connect( m_ui.cbConfigureBatchDownloadConcurrently,&QCheckBox::stateChanged,[ this ]( int ){

		this->enableConcurrentTextField() ;
	} ) ;

	connect( m_ui.cbUseSystemVersionIfAvailable,&QCheckBox::stateChanged,[ this ]( int ){

		this->manageDownloadButton() ;
	} ) ;

	connect( m_ui.pbConfigureAddAPlugin,&QPushButton::clicked,[ this ](){

		auto m = QFileDialog::getOpenFileName( &m_ctx.mainWidget(),tr( "Select An Engine File" ),utility::homePath() ) ;

		if( !m.isEmpty() ){

			auto d = engines::file( m,m_ctx.logger() ).readAll() ;

			if( !d.isEmpty() ){

				m_ctx.Engines().addEngine( d,utility::split( m,'/',true ).last() ) ;

				m_ctx.TabManager().basicDownloader().setAsActive().downloadDefaultEngine() ;
			}
		}
	} ) ;

	connect( m_ui.pbConfigureRemoveAPlugin,&QPushButton::clicked,[ this ](){

		QMenu m ;

		for( const auto& it : m_ctx.Engines().enginesList() ){

			auto e = it ;
			m.addAction( e.replace( ".json","" ) )->setObjectName( it ) ;
		}

		m.addSeparator() ;

		m.addAction( tr( "Cancel" ) )->setEnabled( false ) ;

		connect( &m,&QMenu::triggered,[ & ]( QAction * ac ){

			m_ctx.Engines().removeEngine( ac->objectName() ) ;

			m_ctx.TabManager().basicDownloader().setAsActive().updateEngines() ;
		} ) ;

		m.exec( QCursor::pos() ) ;
	} ) ;	

	connect( m_ui.pbConfigureDownload,&QPushButton::clicked,[ this ](){

		this->downloadFromGitHub( m_ctx.Engines().defaultEngine() ) ;
	} ) ;

	this->resetMenu() ;

	auto s = static_cast< void( QComboBox::* )( int ) >( &QComboBox::activated ) ;

	connect( m_ui.cbConfigureLanguage,s,[ & ]( int s ){

		const auto& languages = m_settings.localizationLanguages() ;

		if( s != -1 && s < languages.size() ){

			const auto& m = languages.at( s ) ;

			m_settings.setLocalizationLanguage( m ) ;

			m_ctx.Translator().setLanguage( m ) ;

			m_ctx.mainWindow().retranslateUi() ;

			m_tabManager.reTranslateUi() ;
		}		
	} ) ;

	connect( m_ui.pbConfigureSetPresetDefaults,&QPushButton::clicked,[ & ](){

		m_settings.setPresetToDefaults() ;

		m_ui.textEditConfigurePresetOptions->setPlainText( m_settings.presetOptions() ) ;
	} ) ;

	m_ui.pbConfigureDownloadPath->setIcon( [](){

		return QIcon( ":folder" ) ;
	}() ) ;

	connect( m_ui.pbConfigureDownloadPath,&QPushButton::clicked,[ this ](){

		auto e = QFileDialog::getExistingDirectory( &m_mainWindow,
							    tr( "Set Download Folder" ),
							    QDir::homePath(),
							    QFileDialog::ShowDirsOnly ) ;

		if( !e.isEmpty() ){

			m_ui.lineEditConfigureDownloadPath->setText( e ) ;
		}
	} ) ;

	m_ui.lineEditConfigureScaleFactor->setText( m_settings.highDpiScalingFactor() ) ;

	m_ui.lineEditConfigureDownloadPath->setText( m_settings.downloadFolder() ) ;

	m_ui.textEditConfigurePresetOptions->setText( m_settings.presetOptions() ) ;

	m_ui.cbConfigureShowVersionInfo->setChecked( m_settings.showVersionInfoWhenStarting() ) ;

	m_ui.cbConfigureBatchDownloadConcurrently->setChecked( m_settings.concurrentDownloading() ) ;

	m_ui.cbUseSystemVersionIfAvailable->setChecked( m_settings.useSystemProvidedVersionIfAvailable() ) ;

	m_ui.cbUseSystemVersionIfAvailable->setEnabled( utility::platformIsLinux() ) ;

	m_ui.lineEditConfigureMaximuConcurrentDownloads->setText( QString::number( m_settings.maxConcurrentDownloads() ) ) ;
}

void configure::init_done()
{
	this->enableConcurrentTextField() ;
}

void configure::retranslateUi()
{
	this->resetMenu() ;

	this->manageDownloadButton() ;
}

void configure::downloadFromGitHub( const engines::engine& engine )
{
	m_networkAccess.download( engine ) ;
}

void configure::tabEntered()
{
	this->manageDownloadButton() ;
}

void configure::tabExited()
{
	this->saveOptions() ;
}

void configure::enableConcurrentTextField()
{
	auto s = m_ui.cbConfigureBatchDownloadConcurrently->isChecked() ;

	m_ui.lineEditConfigureMaximuConcurrentDownloads->setEnabled( s ) ;
	m_ui.labelMaximumConcurrentDownloads->setEnabled( s ) ;
}

void configure::saveOptions()
{
	m_settings.setHighDpiScalingFactor( m_ui.lineEditConfigureScaleFactor->text() ) ;
	m_settings.setPresetOptions( m_ui.textEditConfigurePresetOptions->toPlainText() ) ;
	m_settings.setDownloadFolder( m_ui.lineEditConfigureDownloadPath->text() ) ;
	m_settings.setShowVersionInfoWhenStarting( m_ui.cbConfigureShowVersionInfo->isChecked() ) ;
	m_settings.setConcurrentDownloading( m_ui.cbConfigureBatchDownloadConcurrently->isChecked() ) ;
	m_settings.setUseSystemProvidedVersionIfAvailable( m_ui.cbUseSystemVersionIfAvailable->isChecked() ) ;

	auto s = m_ui.lineEditConfigureMaximuConcurrentDownloads->text() ;

	if( s.isEmpty() ){

		m_settings.setMaxConcurrentDownloads( 4 ) ;
	}else{
		bool ok ;

		auto m = s.toInt( &ok ) ;

		if( ok ){

			if( m == 0 ){

				m_settings.setMaxConcurrentDownloads( 1 ) ;
			}else{
				m_settings.setMaxConcurrentDownloads( m ) ;
			}
		}
	}
}

void configure::manageDownloadButton()
{
	const auto& engine = m_ctx.Engines().defaultEngine() ;

	if( networkAccess::hasNetworkSupport() && m_setEnabled ){

		m_ui.pbConfigureDownload->setEnabled( !engine.downloadUrl().isEmpty() ) ;
	}else{
		m_ui.pbConfigureDownload->setEnabled( false ) ;
	}

	m_ui.pbConfigureDownload->setText( tr( "Update" ) + " " + engine.name() ) ;
}

void configure::resetMenu()
{
	const auto& languages = m_settings.localizationLanguages() ;

	const auto& language  = m_settings.localizationLanguage() ;

	auto& translator = m_ctx.Translator() ;

	m_ui.cbConfigureLanguage->clear() ;

	int index = 0 ;

	for( int i = 0 ; i < languages.size() ; i++ ){

		auto& e = languages[ i ] ;

		if( e == language ){

			index = i ;
		}

		m_ui.cbConfigureLanguage->addItem( translator.UIName( e ) ) ;
	}

	m_ui.cbConfigureLanguage->setCurrentIndex( index ) ;	
}

void configure::enableAll()
{
	m_setEnabled = true ;

	this->manageDownloadButton() ;
	m_ui.cbConfigureShowVersionInfo->setEnabled( true ) ;
	m_ui.cbConfigureLanguage->setEnabled( true ) ;
	m_ui.labelConfigureLanguage->setEnabled( true ) ;
	m_ui.lineEditConfigureDownloadPath->setEnabled( true ) ;
	m_ui.textEditConfigurePresetOptions->setEnabled( true ) ;
	m_ui.lineEditConfigureDownloadPath->setEnabled( true ) ;
	m_ui.pbConfigureDownloadPath->setEnabled( true ) ;
	m_ui.pbConfigureSetPresetDefaults->setEnabled( true ) ;
	m_ui.labelConfigureScaleFactor->setEnabled( true ) ;
	m_ui.labelConfigureDownloadPath->setEnabled( true ) ;
	m_ui.pbConfigureQuit->setEnabled( true ) ;
	m_ui.pbConfigureAddAPlugin->setEnabled( true ) ;
	m_ui.pbConfigureRemoveAPlugin->setEnabled( true ) ;
	m_ui.cbConfigureBatchDownloadConcurrently->setEnabled( true ) ;
	m_ui.labelMaximumConcurrentDownloads->setEnabled( true ) ;
	m_ui.cbUseSystemVersionIfAvailable->setEnabled( true ) ;

	this->enableConcurrentTextField() ;

	if( m_settings.enabledHighDpiScaling() ){

		m_ui.lineEditConfigureScaleFactor->setEnabled( true ) ;
	}
}

void configure::disableAll()
{
	m_setEnabled = false ;
	m_ui.cbUseSystemVersionIfAvailable->setEnabled( false ) ;
	m_ui.lineEditConfigureMaximuConcurrentDownloads->setEnabled( false ) ;
	m_ui.labelMaximumConcurrentDownloads->setEnabled( false ) ;
	m_ui.pbConfigureAddAPlugin->setEnabled( false ) ;
	m_ui.pbConfigureRemoveAPlugin->setEnabled( false ) ;
	m_ui.cbConfigureShowVersionInfo->setEnabled( false ) ;
	m_ui.pbConfigureDownload->setEnabled( false ) ;
	m_ui.cbConfigureLanguage->setEnabled( false ) ;
	m_ui.labelConfigureLanguage->setEnabled( false ) ;
	m_ui.pbConfigureQuit->setEnabled( false ) ;
	m_ui.lineEditConfigureScaleFactor->setEnabled( false ) ;
	m_ui.lineEditConfigureDownloadPath->setEnabled( false ) ;
	m_ui.textEditConfigurePresetOptions->setEnabled( false ) ;
	m_ui.lineEditConfigureDownloadPath->setEnabled( false ) ;
	m_ui.pbConfigureDownloadPath->setEnabled( false ) ;
	m_ui.pbConfigureSetPresetDefaults->setEnabled( false ) ;
	m_ui.labelConfigureScaleFactor->setEnabled( false ) ;
	m_ui.labelConfigureDownloadPath->setEnabled( false ) ;
	m_ui.cbConfigureBatchDownloadConcurrently->setEnabled( false ) ;
}

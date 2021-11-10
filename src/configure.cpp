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
	m_engines( m_ctx.Engines() ),
	m_networkAccess( m_ctx ),
	m_tablePresetOptions( *m_ui.tableWidgetConfigurePresetOptions,m_ctx.mainWidget().font() )
{
	m_ui.lineEditConfigureScaleFactor->setEnabled( m_settings.enabledHighDpiScaling() ) ;

	m_ui.tabWidgetConfigure->setCurrentIndex( 0 ) ;

	settings::darkModes modes ;

	modes.setComboBox( *m_ui.comboBoxConfigureDarkTheme,m_settings.darkMode() ) ;

	auto cc = static_cast< void ( QComboBox::* )( int ) >( &QComboBox::currentIndexChanged ) ;

	m_tablePresetOptions.connect( &QTableWidget::currentItemChanged,[ this ]( QTableWidgetItem * c,QTableWidgetItem * p ){

		m_tablePresetOptions.selectRow( c,p,0 ) ;
	} ) ;

	connect( m_ui.comboBoxConfigureDarkTheme,cc,[ this,modes = std::move( modes ) ]( int index ){

		if( index != -1 ){

			m_settings.setDarkMode( modes.unTranslatedAt( index ) ) ;
		}
	} ) ;

	connect( m_ui.pbConfigureAddToPresetList,&QPushButton::clicked,[ this ](){

		auto a = m_ui.lineEditConfigureUiName->text() ;
		auto b = m_ui.lineEditConfigurePresetOptions->text() ;

		if( !a.isEmpty() && !b.isEmpty() ){

			m_tablePresetOptions.add( { a,b } ) ;

			m_tablePresetOptions.selectLast() ;

			m_ui.lineEditConfigureUiName->clear() ;
			m_ui.lineEditConfigurePresetOptions->clear() ;
		}
	} ) ;

	m_tablePresetOptions.connect( &QTableWidget::customContextMenuRequested,[ this ]( QPoint ){

		QMenu m ;

		connect( m.addAction( tr( "Remove" ) ),&QAction::triggered,[ this ](){

			auto row = m_tablePresetOptions.currentRow() ;

			if( row != -1 ){

				m_tablePresetOptions.removeRow( row ) ;
			}
		} ) ;

		m.exec( QCursor::pos() ) ;
	} ) ;

	connect( m_ui.cbConfigureEngines,cc,[ this,modes = std::move( modes ) ]( int index ){

		if( index != -1 ){

			auto m = m_ui.cbConfigureEngines->itemText( index ) ;

			this->setEngineOptions( m ) ;
		}
	} ) ;

	connect( m_ui.pbConfigureCookiePath,&QPushButton::clicked,[ this ](){

		auto m = QFileDialog::getOpenFileName( &m_ctx.mainWidget(),tr( "Select A Cookie File" ),utility::homePath() ) ;

		if( !m.isEmpty() ){

			m_ui.lineEditConfigureCookiePath->setText( m ) ;
		}
	} ) ;

	connect( m_ui.pbConfigureQuit,&QPushButton::clicked,[ this ](){

		this->saveOptions() ;
		m_tabManager.basicDownloader().appQuit() ;
	} ) ;

	connect( m_ui.pbConfigureSave,&QPushButton::clicked,[ this ](){

		this->saveOptions() ;
	} ) ;

	connect( m_ui.cbConfigureShowThumbnails,&QCheckBox::stateChanged,[]( int ){

	} ) ;

	connect( m_ui.cbUseSystemVersionIfAvailable,&QCheckBox::stateChanged,[]( int ){

	} ) ;

	connect( m_ui.pbConfigureAddAPlugin,&QPushButton::clicked,[ this ](){

		auto m = QFileDialog::getOpenFileName( &m_ctx.mainWidget(),tr( "Select An Engine File" ),utility::homePath() ) ;

		if( !m.isEmpty() ){

			auto d = engines::file( m,m_ctx.logger() ).readAll() ;

			if( !d.isEmpty() ){

				if( m_ctx.Engines().addEngine( d,util::split( m,'/',true ).last() ) ){

					m_ctx.TabManager().basicDownloader().setAsActive().downloadDefaultEngine() ;
				}
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

	connect( m_ui.pbConfigureDownload,&QPushButton::clicked,[](){

	} ) ;

	connect( &m_menu,&QMenu::triggered,[ & ]( QAction * ac ){

		auto m = ac->objectName() ;

		if( !m.isEmpty() ){

			this->downloadFromGitHub( m_ctx.Engines().defaultEngine( m ) ) ;
		}
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

	connect( m_ui.pbConfigureSetPresetDefaults,&QPushButton::clicked,[ this ](){

		m_settings.setPresetJsonDefaultOptions() ;

		this->showOptions() ;
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

	connect( m_ui.pbConfigureEngineDefaultOptions,&QPushButton::clicked,[ & ](){

		auto mm = m_ui.cbConfigureEngines->currentText() ;

		const auto& s = m_engines.getEngineByName( mm ) ;

		if( s ){

			auto m = s->defaultDownLoadCmdOptions().join( " " ) ;

			m_ui.lineEditConfigureDownloadOptions->setText( m ) ;

			m_settings.setEngineDefaultDownloadOptions( s->name(),m ) ;
		}
	} ) ;

	m_ui.lineEditConfigureScaleFactor->setText( m_settings.highDpiScalingFactor() ) ;

	m_ui.lineEditConfigureDownloadPath->setText( m_settings.downloadFolder() ) ;

	m_ui.cbConfigureShowVersionInfo->setChecked( m_settings.showVersionInfoWhenStarting() ) ;

	m_ui.cbConfigureShowThumbnails->setChecked( m_settings.showThumbnails() ) ;

	m_ui.cbUseSystemVersionIfAvailable->setChecked( m_settings.useSystemProvidedVersionIfAvailable() ) ;

	m_ui.cbUseSystemVersionIfAvailable->setEnabled( utility::platformIsLinux() ) ;

	m_ui.lineEditConfigureMaximuConcurrentDownloads->setText( QString::number( m_settings.maxConcurrentDownloads() ) ) ;

	this->showOptions() ;
}

void configure::init_done()
{
	m_tablePresetOptions.selectLast() ;
}

void configure::retranslateUi()
{
	this->resetMenu() ;

	settings::darkModes().setComboBox( *m_ui.comboBoxConfigureDarkTheme,m_settings.darkMode() ) ;
}

void configure::downloadFromGitHub( const engines::Iterator& iter )
{
	m_networkAccess.download( iter ) ;
}

void configure::tabEntered()
{
	m_menu.clear() ;

	for( const auto& it : m_ctx.Engines().getEngines() ){

		if( !it.downloadUrl().isEmpty() ){

			auto ac = m_menu.addAction( it.name() ) ;

			ac->setObjectName( it.name() ) ;

			ac->setEnabled( networkAccess::hasNetworkSupport() ) ;
		}
	}

	m_menu.addSeparator() ;

	m_menu.addAction( tr( "Cancel" ) ) ;

	m_ui.pbConfigureDownload->setMenu( &m_menu ) ;
}

void configure::tabExited()
{
	this->saveOptions() ;
}

void configure::updateEnginesList( const QStringList& e )
{
	auto& cb = *m_ui.cbConfigureEngines ;

	cb.clear() ;

	for( const auto& it : e ){

		cb.addItem( it ) ;

		this->setEngineOptions( it ) ;
	}

	cb.setCurrentIndex( 0 ) ;
	this->setEngineOptions( cb.currentText() ) ;
}

networkAccess& configure::network()
{
	return m_networkAccess ;
}

void configure::saveOptions()
{
	auto m = m_ui.cbConfigureShowThumbnails->isChecked() ;

	m_ctx.TabManager().batchDownloader().setThumbnailColumnSize( m ) ;

	m_settings.setShowThumbnails( m ) ;
	m_settings.setHighDpiScalingFactor( m_ui.lineEditConfigureScaleFactor->text() ) ;
	m_settings.setDownloadFolder( m_ui.lineEditConfigureDownloadPath->text() ) ;
	m_settings.setShowVersionInfoWhenStarting( m_ui.cbConfigureShowVersionInfo->isChecked() ) ;
	m_settings.setUseSystemProvidedVersionIfAvailable( m_ui.cbUseSystemVersionIfAvailable->isChecked() ) ;

	auto s = m_ui.lineEditConfigureMaximuConcurrentDownloads->text() ;

	if( s.isEmpty() ){

		m_settings.setMaxConcurrentDownloads( 4 ) ;
	}else{
		bool ok ;

		auto m = s.toInt( &ok ) ;

		if( ok ){

			if( m == 0 ){

				m_settings.setMaxConcurrentDownloads( 4 ) ;
			}else{
				m_settings.setMaxConcurrentDownloads( m ) ;
			}
		}
	}

	auto mm = m_ui.cbConfigureEngines->currentText() ;

	const auto& ss = m_engines.getEngineByName( mm ) ;

	if( ss ){

		auto m = m_ui.lineEditConfigureDownloadOptions->text() ;

		m_settings.setEngineDefaultDownloadOptions( ss->name(),m ) ;

		if( !ss->cookieArgument().isEmpty() ){

			m_settings.setCookieFilePath( ss->name(),m_ui.lineEditConfigureCookiePath->text() ) ;
		}
	}

	this->savePresetOptions() ;
	m_ctx.TabManager().resetMenu() ;
}

void configure::setEngineOptions( const QString& e )
{
	const auto& s = m_engines.getEngineByName( e ) ;

	if( s ){

		auto m = m_settings.engineDefaultDownloadOptions( s->name() ) ;

		if( m.isEmpty() ){

			const auto& e = s->defaultDownLoadCmdOptions() ;
			m_settings.setEngineDefaultDownloadOptions( s->name(),e.join( " " ) ) ;
			m_ui.lineEditConfigureDownloadOptions->setText( e.join( " " ) ) ;
		}else{
			m_ui.lineEditConfigureDownloadOptions->setText( m ) ;
		}

		auto enable = !s->cookieArgument().isEmpty() ;

		m_ui.lineEditConfigureCookiePath->setText( m_settings.cookieFilePath( s->name() ) ) ;
		m_ui.lineEditConfigureCookiePath->setEnabled( enable ) ;
		m_ui.pbConfigureCookiePath->setEnabled( enable ) ;
		m_ui.labelPathToCookieFile->setEnabled( enable ) ;
	}
}

void configure::savePresetOptions()
{
	auto presetOptions = m_settings.setpresetOptions() ;

	auto& table = m_tablePresetOptions.get() ;

	auto rowCount = table.rowCount() ;

	for( int i = 0 ; i < rowCount ; i++ ){

		auto uiName = table.item( i,0 )->text() ;
		auto options = table.item( i,1 )->text() ;

		presetOptions.add( uiName,options ) ;
	}
}

void configure::showOptions()
{
	auto mm = m_settings.presetOptions() ;

	m_tablePresetOptions.clear() ;

	auto _addItem = [ this ]( const QString& uiName,const QString& options ){

		m_tablePresetOptions.add( { uiName,options } ) ;
	} ;

	if( mm.isEmpty() ){

		m_settings.presetOptions( _addItem ) ;
	}else{
		for( const auto& it : util::split( mm,',',true ) ){

			auto b = it.indexOf( '(' ) ;

			if( b != -1 ){

				auto a = it ;
				auto c = a.mid( 0,b ) ;
				auto cc = a.mid( b + 1 ) ;
				cc.truncate( cc.size() - 1 ) ;

				_addItem( c,cc ) ;
			}
		}

		this->savePresetOptions() ;

		m_settings.setPresetOptions( QStringList() ) ;
	}
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
	const auto& s = m_engines.getEngineByName( m_ui.cbConfigureEngines->currentText() ) ;

	if( s ){

		auto enable = !s->cookieArgument().isEmpty() ;

		m_ui.lineEditConfigureCookiePath->setEnabled( enable ) ;
		m_ui.pbConfigureCookiePath->setEnabled( enable ) ;
		m_ui.labelPathToCookieFile->setEnabled( enable ) ;
	}

	m_ui.tableWidgetConfigurePresetOptions->setEnabled( true ) ;
	m_ui.lineEditConfigurePresetOptions->setEnabled( true ) ;
	m_ui.lineEditConfigureUiName->setEnabled( true ) ;
	m_ui.labelConfugureUiName->setEnabled( true ) ;
	m_ui.labelConfigureOptionsPresetOptiions->setEnabled( true ) ;
	m_ui.pbConfigureAddToPresetList->setEnabled( true ) ;
	m_ui.pbConfigureEngineDefaultOptions->setEnabled( true ) ;
	m_ui.lineEditConfigureDownloadOptions->setEnabled( true ) ;
	m_ui.labelConfigureOptions->setEnabled( true ) ;
	m_ui.cbConfigureEngines->setEnabled( true ) ;
	m_ui.labelConfigureEngines->setEnabled( true ) ;
	m_ui.pbConfigureSave->setEnabled( true ) ;
	m_ui.lineEditConfigureMaximuConcurrentDownloads->setEnabled( true ) ;
	m_ui.comboBoxConfigureDarkTheme->setEnabled( true ) ;
	m_ui.pbConfigureDownload->setEnabled( true ) ;
	m_ui.labelConfigureTheme->setEnabled( true ) ;
	m_ui.cbConfigureShowVersionInfo->setEnabled( true ) ;
	m_ui.cbConfigureLanguage->setEnabled( true ) ;
	m_ui.labelConfigureLanguage->setEnabled( true ) ;
	m_ui.lineEditConfigureDownloadPath->setEnabled( true ) ;
	m_ui.lineEditConfigureDownloadPath->setEnabled( true ) ;
	m_ui.pbConfigureDownloadPath->setEnabled( true ) ;
	m_ui.pbConfigureSetPresetDefaults->setEnabled( true ) ;
	m_ui.labelConfigureScaleFactor->setEnabled( true ) ;
	m_ui.labelConfigureDownloadPath->setEnabled( true ) ;
	m_ui.pbConfigureQuit->setEnabled( true ) ;
	m_ui.pbConfigureAddAPlugin->setEnabled( true ) ;
	m_ui.pbConfigureRemoveAPlugin->setEnabled( true ) ;
	m_ui.cbConfigureShowThumbnails->setEnabled( true ) ;
	m_ui.labelMaximumConcurrentDownloads->setEnabled( true ) ;

	if( m_settings.enabledHighDpiScaling() ){

		m_ui.lineEditConfigureScaleFactor->setEnabled( true ) ;
	}

	m_ui.cbUseSystemVersionIfAvailable->setEnabled( utility::platformIsLinux() ) ;
}

void configure::disableAll()
{
	m_ui.tableWidgetConfigurePresetOptions->setEnabled( false ) ;
	m_ui.lineEditConfigurePresetOptions->setEnabled( false ) ;
	m_ui.lineEditConfigureUiName->setEnabled( false ) ;
	m_ui.labelConfugureUiName->setEnabled( false ) ;
	m_ui.labelConfigureOptionsPresetOptiions->setEnabled( false ) ;
	m_ui.pbConfigureAddToPresetList->setEnabled( false ) ;
	m_ui.tableWidgetConfigurePresetOptions->setEnabled( false ) ;
	m_ui.labelPathToCookieFile->setEnabled( false ) ;
	m_ui.lineEditConfigureCookiePath->setEnabled( false ) ;
	m_ui.pbConfigureCookiePath->setEnabled( false ) ;
	m_ui.pbConfigureEngineDefaultOptions->setEnabled( false ) ;
	m_ui.lineEditConfigureDownloadOptions->setEnabled( false ) ;
	m_ui.labelConfigureOptions->setEnabled( false ) ;
	m_ui.cbConfigureEngines->setEnabled( false ) ;
	m_ui.labelConfigureEngines->setEnabled( false ) ;
	m_ui.pbConfigureSave->setEnabled( false ) ;
	m_ui.comboBoxConfigureDarkTheme->setEnabled( false ) ;
	m_ui.pbConfigureDownload->setEnabled( false ) ;
	m_ui.labelConfigureTheme->setEnabled( false ) ;
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
	m_ui.lineEditConfigureDownloadPath->setEnabled( false ) ;
	m_ui.pbConfigureDownloadPath->setEnabled( false ) ;
	m_ui.pbConfigureSetPresetDefaults->setEnabled( false ) ;
	m_ui.labelConfigureScaleFactor->setEnabled( false ) ;
	m_ui.labelConfigureDownloadPath->setEnabled( false ) ;
	m_ui.cbConfigureShowThumbnails->setEnabled( false ) ;
}

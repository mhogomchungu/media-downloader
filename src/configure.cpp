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
#include "themes.h"
#include "mainwindow.h"
#include "versionInfo.h"

#include <QFileDialog>
#include <QFile>
#include <QDesktopServices>
#include <QClipboard>

configure::configure( const Context& ctx ) :
	m_ctx( ctx ),
	m_settings( m_ctx.Settings() ),
	m_ui( m_ctx.Ui() ),
	m_mainWindow( m_ctx.mainWidget() ),
	m_tabManager( m_ctx.TabManager() ),
	m_engines( m_ctx.Engines() ),
	m_tablePresetOptions( *m_ui.tableWidgetConfigurePresetOptions,m_ctx.mainWidget().font() ),
	m_tableUrlToDefaultEngine( *m_ui.tableWidgetConfigureUrl,m_ctx.mainWidget().font() ),
	m_tableDefaultDownloadOptions( *m_ui.tableWidgetEnginesDefaultOptions,m_ctx.mainWidget().font() ),
	m_presetOptions( m_ctx,m_settings ),
	m_downloadDefaultOptions( m_ctx,"downloadDefaultOptions.json" ),
	m_downloadEngineDefaultOptions( m_ctx,"downloadEngineDefaultOptions.json" )
{
	m_ui.tableWidgetConfigureUrl->setColumnWidth( 0,180 ) ;

	m_ui.lineEditConfigureScaleFactor->setEnabled( m_settings.enabledHighDpiScaling() ) ;

	m_ui.tabWidgetConfigure->setCurrentIndex( 0 ) ;

	auto themesFolderPath = m_engines.engineDirPaths().themePath() ;

	themes ths( themesFolderPath ) ;

	ths.setComboBox( *m_ui.comboBoxConfigureDarkTheme,m_settings.themeName() ) ;

	auto cc = static_cast< void ( QComboBox::* )( int ) >( &QComboBox::currentIndexChanged ) ;

	m_tablePresetOptions.connect( &QTableWidget::currentItemChanged,[ this ]( QTableWidgetItem * c,QTableWidgetItem * p ){

		m_tablePresetOptions.selectRow( c,p,0 ) ;
	} ) ;

	m_tableUrlToDefaultEngine.connect( &QTableWidget::currentItemChanged,[ this ]( QTableWidgetItem * c,QTableWidgetItem * p ){

		m_tablePresetOptions.selectRow( c,p,0 ) ;
	} ) ;

	m_tableDefaultDownloadOptions.connect( &QTableWidget::currentItemChanged,[ this ]( QTableWidgetItem * c,QTableWidgetItem * p ){

		m_tablePresetOptions.selectRow( c,p,0 ) ;
	} ) ;

	m_ui.pbOpenThemeFolder->setIcon( QIcon( ":/json" ) ) ;

	connect( m_ui.pbOpenThemeFolder,&QPushButton::clicked,[ themesFolderPath ](){

		QDesktopServices::openUrl( QUrl( "file:///" + themesFolderPath,QUrl::TolerantMode ) ) ;
	} ) ;

	m_ui.pbOpenBinFolder->setIcon( QIcon( ":/executable" ) ) ;

	connect( m_ui.pbOpenBinFolder,&QPushButton::clicked,[ this,themesFolderPath ](){

		const auto& m = m_engines.engineDirPaths().binPath() ;

		QDesktopServices::openUrl( QUrl( "file:///" + m,QUrl::TolerantMode ) ) ;
	} ) ;

	connect( m_ui.comboBoxConfigureDarkTheme,cc,[ this,ths = std::move( ths ) ]( int index ){

		if( index != -1 ){

			m_settings.setThemeName( ths.unTranslatedAt( index ) ) ;
		}
	} ) ;

	connect( m_ui.pbConfigureAddToPresetList,&QPushButton::clicked,[ this ](){

		auto a = m_ui.lineEditConfigureWebsite->text() ;
		auto b = m_ui.lineEditConfigureUiName->text() ;
		auto c = m_ui.lineEditConfigurePresetOptions->text() ;

		if( !a.isEmpty() && !b.isEmpty() ){

			m_tablePresetOptions.add( { a,b,c } ) ;

			m_tablePresetOptions.selectLast() ;

			m_ui.lineEditConfigureUiName->clear() ;
			m_ui.lineEditConfigurePresetOptions->clear() ;
			m_ui.lineEditConfigureWebsite->clear() ;
		}
	} ) ;

	m_tableUrlToDefaultEngine.connect( &QTableWidget::customContextMenuRequested,[ this ]( QPoint ){

		QMenu m ;

		connect( m.addAction( tr( "Remove" ) ),&QAction::triggered,[ this ](){

			auto m = m_tableUrlToDefaultEngine.currentRow() ;

			if( m != -1 ){

				m_downloadDefaultOptions.remove( m_tableUrlToDefaultEngine.stuffAt( m ) ) ;
				m_tableUrlToDefaultEngine.removeRow( m ) ;
				m_tableUrlToDefaultEngine.selectLast() ;
			}
		} ) ;

		m.exec( QCursor::pos() ) ;
	} ) ;

	m_tableDefaultDownloadOptions.connect( &QTableWidget::customContextMenuRequested,[ this ]( QPoint ){

		QMenu m ;

		connect( m.addAction( tr( "Set As Default" ) ),&QAction::triggered,[ this ](){

			auto m = m_tableDefaultDownloadOptions.currentRow() ;

			if( m != -1 ){

				auto mm = m_ui.cbConfigureEngines->currentText() ;

				const auto& s = m_ctx.Engines().getEngineByName( mm ) ;

				if( s ){
					auto obj = m_tableDefaultDownloadOptions.stuffAt( m ) ;

					m_downloadEngineDefaultOptions.setAsDefault( obj ) ;

					this->populateOptionsTable( s ) ;
				}
			}
		} ) ;

		connect( m.addAction( tr( "Remove" ) ),&QAction::triggered,[ this ](){

			auto m = m_tableDefaultDownloadOptions.currentRow() ;

			if( m != -1 ){

				auto obj = m_tableDefaultDownloadOptions.stuffAt( m ) ;
				m_downloadEngineDefaultOptions.remove( obj ) ;

				auto mm = m_ui.cbConfigureEngines->currentText() ;

				const auto& s = m_ctx.Engines().getEngineByName( mm ) ;

				this->populateOptionsTable( s ) ;
			}
		} ) ;

		connect( m.addAction( tr( "Copy" ) ),&QAction::triggered,[ this ](){

			auto s = m_tableDefaultDownloadOptions.currentRow() ;

			auto m = QApplication::clipboard() ;

			if( s != -1 && m ){

				auto obj = m_tableDefaultDownloadOptions.stuffAt( s ) ;

				auto str = obj.value( "options" ).toString() ;

				if( !str.isEmpty() ){

					m->setText( str ) ;
				}
			}
		} ) ;

		m.exec( QCursor::pos() ) ;
	} ) ;

	connect( m_ui.pbAddDefaultDownloadOption,&QPushButton::clicked,[ this ](){

		auto m = m_ui.lineEditAddDefaultDownloadOption->text() ;

		if( !m.isEmpty() ){

			auto mm = m_ui.cbConfigureEngines->currentText() ;

			const auto& s = m_ctx.Engines().getEngineByName( mm ) ;

			if( s ){

				auto obj = m_downloadEngineDefaultOptions.addOpt( "no",s->name(),m ) ;

				m_tableDefaultDownloadOptions.add( { "no",m },std::move( obj ) ) ;

				m_tableDefaultDownloadOptions.selectLast() ;

				m_ui.lineEditAddDefaultDownloadOption->clear() ;
			}
		}
	} ) ;

	connect( m_ui.pbConfigureManageUrl,&QPushButton::clicked,[ this ](){

		auto a = m_ui.lineEditConfigureManageUrl->text() ;
		auto b = m_ui.lineEditConfigureManageOptions->text() ;
		auto c = m_ui.cbConfigureEnginesUrlManager->currentText() ;

		if( !a.isEmpty() ){

			if( b.isEmpty() ){

				b = "Default" ;
			}

			m_ui.lineEditConfigureManageUrl->clear() ;
			m_ui.lineEditConfigureManageOptions->clear() ;

			auto obj = m_downloadDefaultOptions.add( a,b,c ) ;

			m_tableUrlToDefaultEngine.add( { a,b },std::move( obj ) ) ;

			m_tableUrlToDefaultEngine.selectLast() ;
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

		connect( m.addAction( tr( "Copy" ) ),&QAction::triggered,[ this ](){

			auto row = m_tablePresetOptions.currentRow() ;

			if( row != -1 ){

				auto mm = QApplication::clipboard() ;

				if( mm ){

					mm->setText( m_tablePresetOptions.item( row,1 ).text() ) ;
				}
			}
		} ) ;

		m.exec( QCursor::pos() ) ;
	} ) ;

	connect( m_ui.cbConfigureEngines,cc,[ this ]( int index ){

		if( index != -1 ){

			auto m = m_ui.cbConfigureEngines->itemText( index ) ;

			this->populateOptionsTable( m_engines.getEngineByName( m ) ) ;

			this->setEngineOptions( m,engineOptions::options ) ;
		}
	} ) ;

	connect( m_ui.cbConfigureEnginesUrlManager,cc,[ this ]( int index ){

		if( index != -1 ){

			auto m = m_ui.cbConfigureEngines->itemText( index ) ;

			this->setEngineOptions( m,engineOptions::url ) ;
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
		m_ctx.mainWindow().quitApp() ;
	} ) ;

	connect( m_ui.pbConfigureSave,&QPushButton::clicked,[ this ](){

		this->saveOptions() ;
	} ) ;

	connect( m_ui.cbShowTrayIcon,&QCheckBox::stateChanged,[ this ]( int s ){

		auto checked = s == Qt::CheckState::Checked ;

		m_settings.setshowTrayIcon( checked ) ;

		m_ctx.mainWindow().showTrayIcon( checked ) ;
	} ) ;

	connect( m_ui.pbConfigureAddAPlugin,&QPushButton::clicked,[ this ](){

		auto m = QFileDialog::getOpenFileName( &m_ctx.mainWidget(),tr( "Select An Engine File" ),utility::homePath() ) ;

		if( m.isEmpty() ){

			return ;
		}

		auto id = utility::sequentialID() ;

		auto d = engines::file( m,m_ctx.logger() ).readAll() ;

		if( d.isEmpty() ){

			return ;
		}

		auto name = m_ctx.Engines().addEngine( d,util::split( m,'/',true ).last(),id ) ;

		if( name.isEmpty() ){

			return ;
		}

		auto& t = m_ctx.TabManager() ;

		t.basicDownloader().setAsActive() ;

		const auto& engine = m_ctx.Engines().getEngineByName( name ) ;

		if( engine ){

			class woof : public versionInfo::idone
			{
			public:
				woof( const Context& ctx,QString de ) :
					m_ctx( ctx ),m_defaultEngine( std::move( de ) )
				{
				}
				void operator()() override
				{
					if( m_success ){

						utility::setDefaultEngine( m_ctx,m_defaultEngine ) ;
					}
				}
				void failed() override
				{
					m_success = false ;
				}
			private:
				const Context& m_ctx ;
				QString m_defaultEngine ;
				bool m_success = true ;
			} ;

			auto& m = m_ctx.getVersionInfo() ;

			m.check( { engine.value(),id },
				 { util::types::type_identity< woof >(),m_ctx,std::move( name ) } ) ;
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

			auto id = utility::sequentialID() ;

			m_ctx.Engines().removeEngine( ac->objectName(),id ) ;

			auto& t = m_ctx.TabManager() ;

			t.basicDownloader().setAsActive() ;

			t.setDefaultEngines() ;
		} ) ;

		m.exec( QCursor::pos() ) ;
	} ) ;

	connect( m_ui.pbConfigureDownload,&QPushButton::clicked,[](){

	} ) ;

	connect( &m_menu,&QMenu::triggered,[ & ]( QAction * ac ){

		auto m = ac->objectName() ;

		if( !m.isEmpty() ){

			auto id = utility::sequentialID() ;

			if( m == m_ctx.appName() ){

				class meaw : public networkAccess::status
				{
				public:
					meaw( const Context& ctx ) : m_ctx( ctx )
					{
					}
					void done()
					{
						m_ctx.TabManager().enableAll() ;
					}
				private:
					const Context& m_ctx ;
				} ;

				networkAccess::Status s{ util::types::type_identity< meaw >(),m_ctx } ;

				m_ctx.network().updateMediaDownloader( id,std::move( s ) ) ;
			}else{
				this->downloadFromGitHub( { m_ctx.Engines().defaultEngine( m,id ),id } ) ;
			}
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

			m_ctx.logger().reTranslateLogWindow() ;
		}
	} ) ;

	connect( m_ui.pbConfigureSetPresetDefaults,&QPushButton::clicked,[ this ](){

		m_presetOptions.setDefaults() ;
		m_tablePresetOptions.clear() ;

		this->showOptions() ;
	} ) ;

	m_ui.pbConfigureDownloadPath->setIcon( QIcon( ":/folder" ) ) ;

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

			m_downloadEngineDefaultOptions.removeAll( s->name() ) ;

			this->populateOptionsTable( s ) ;
		}
	} ) ;

	m_ui.lineEditConfigureScaleFactor->setText( m_settings.highDpiScalingFactor() ) ;

	m_ui.lineEditConfigureDownloadPath->setText( m_settings.downloadFolder() ) ;

	m_ui.cbConfigureShowVersionInfo->setChecked( m_settings.showVersionInfoWhenStarting() ) ;

	m_ui.cbConfigureShowThumbnails->setChecked( m_settings.showThumbnails() ) ;

	m_ui.cbUseSystemVersionIfAvailable->setChecked( m_settings.useSystemProvidedVersionIfAvailable() ) ;

	m_ui.cbAutoSaveNotDownloadedMedia->setChecked( m_settings.autoSavePlaylistOnExit() ) ;

	m_ui.lineEditConfigureTextEncoding->setText( m_settings.textEncoding() ) ;

	m_ui.cbCheckForUpdates->setChecked( m_settings.checkForUpdates() ) ;

	m_ui.cbShowTrayIcon->setChecked( m_settings.showTrayIcon() ) ;

	connect( m_ui.cbCheckForUpdates,&QCheckBox::stateChanged,[ this ]( int e ){

		auto s = static_cast< Qt::CheckState >( e ) ;

		if( s == Qt::Checked ){

			m_ctx.getVersionInfo().checkForUpdates() ;
		}
	} ) ;

	if( utility::platformIsLikeWindows() ){

		m_ui.cbUseSystemVersionIfAvailable->setEnabled( !m_settings.portableVersion() ) ;
	}else{
		m_ui.cbUseSystemVersionIfAvailable->setEnabled( utility::platformIsLinux() ) ;
	}

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

	themes().setComboBox( *m_ui.comboBoxConfigureDarkTheme,m_settings.themeName() ) ;
}

void configure::downloadFromGitHub( const engines::Iterator& iter )
{
	m_ctx.network().download( iter,{ true,false } ) ;
}

void configure::tabEntered()
{
	m_menu.clear() ;

	auto ac = m_menu.addAction( m_ctx.appName() ) ;

	ac->setObjectName( m_ctx.appName() ) ;

	ac->setEnabled( utility::platformIsWindows() ) ;

	m_menu.addSeparator() ;

	for( const auto& it : m_ctx.Engines().getEngines() ){

		if( it.validDownloadUrl() ){

			auto ac = m_menu.addAction( it.name() ) ;

			ac->setObjectName( it.name() ) ;

			ac->setEnabled( networkAccess::hasNetworkSupport() ) ;
		}
	}

	m_menu.addSeparator() ;

	m_menu.addAction( tr( "Cancel" ) ) ;

	m_ui.pbConfigureDownload->setMenu( &m_menu ) ;

	auto mm = m_ui.cbConfigureEngines->currentText() ;

	this->populateOptionsTable( m_engines.getEngineByName( mm ) ) ;
}

void configure::populateOptionsTable( const util::result_ref< const engines::engine& >& s )
{
	m_tableDefaultDownloadOptions.clear() ;

	if( s ){

		m_downloadEngineDefaultOptions.forEach( [ s,this ]( const configure::downloadDefaultOptions::optsEngines& opts,QJsonObject obj ){

			if( s->name() == opts.engine ){

				m_tableDefaultDownloadOptions.add( { opts.inuse,opts.options },std::move( obj ) ) ;
			}

			return false ;
		} ) ;

		if( m_tableDefaultDownloadOptions.rowCount() == 0 ){

			const auto& e = s->defaultDownLoadCmdOptions() ;

			if( !e.isEmpty() ){

				auto b = e.join( " " ) ;

				auto obj = m_downloadEngineDefaultOptions.addOpt( "yes",s->name(),b ) ;

				m_tableDefaultDownloadOptions.add( { "yes",b },std::move( obj ) ) ;
			}
		}

		m_tableDefaultDownloadOptions.selectLast() ;
	}
}

void configure::tabExited()
{
	this->saveOptions() ;
}

void configure::updateEnginesList( const QStringList& e )
{
	auto& cb = *m_ui.cbConfigureEngines ;
	auto& xb = *m_ui.cbConfigureEnginesUrlManager ;

	cb.clear() ;
	xb.clear() ;

	for( const auto& it : e ){

		cb.addItem( it ) ;
		xb.addItem( it ) ;

		this->setEngineOptions( it,engineOptions::both ) ;
	}

	cb.setCurrentIndex( 0 ) ;
	xb.setCurrentIndex( 0 ) ;

	this->setEngineOptions( cb.currentText(),engineOptions::both ) ;
}

QString configure::engineDefaultDownloadOptions( const QString& engineName )
{
	QString options ;

	using mm = configure::downloadDefaultOptions::optsEngines ;

	m_downloadEngineDefaultOptions.forEach( [ & ]( const mm& opts,const QJsonObject& ){

		if( opts.engine == engineName && opts.inuse == "yes" ){

			options = opts.options ;

			return true ;
		}else{
			return false ;
		}
	} ) ;

	return options ;
}

void configure::setDownloadOptions( int row,tableWidget& table )
{
	m_downloadDefaultOptions.setDownloadOptions( row,table ) ;
}

void configure::saveOptions()
{
	m_downloadDefaultOptions.save() ;
	m_downloadEngineDefaultOptions.save() ;

	auto m = m_ui.cbConfigureShowThumbnails->isChecked() ;

	m_ctx.TabManager().batchDownloader().setThumbnailColumnSize( m ) ;

	m_settings.setShowThumbnails( m ) ;
	m_settings.setHighDpiScalingFactor( m_ui.lineEditConfigureScaleFactor->text() ) ;
	m_settings.setDownloadFolder( m_ui.lineEditConfigureDownloadPath->text() ) ;
	m_settings.setShowVersionInfoWhenStarting( m_ui.cbConfigureShowVersionInfo->isChecked() ) ;
	m_settings.setUseSystemProvidedVersionIfAvailable( m_ui.cbUseSystemVersionIfAvailable->isChecked() ) ;
	m_settings.setAutoSavePlaylistOnExit( m_ui.cbAutoSaveNotDownloadedMedia->isChecked() ) ;
	m_settings.setCheckForUpdates( m_ui.cbCheckForUpdates->isChecked() ) ;
	m_settings.setTextEncoding( m_ui.lineEditConfigureTextEncoding->text() ) ;

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

		if( !ss->cookieArgument().isEmpty() ){

			m_settings.setCookieFilePath( ss->name(),m_ui.lineEditConfigureCookiePath->text() ) ;
		}
	}

	this->savePresetOptions() ;
	m_ctx.TabManager().resetMenu() ;
}

void configure::setEngineOptions( const QString& e,engineOptions tab )
{
	const auto& s = m_engines.getEngineByName( e ) ;

	if( s ){

		auto _setUpUrl = [ & ](){

			const auto& engineName = s->name() ;

			m_tableUrlToDefaultEngine.clear() ;

			m_downloadDefaultOptions.forEach( [ this,&engineName ]( const downloadDefaultOptions::opts& e,QJsonObject obj ){

				if( engineName.isEmpty() || engineName == e.engine ){

					m_tableUrlToDefaultEngine.add( { e.url,e.downloadOptions },std::move( obj ) ) ;
				}

				return false ;
			} ) ;

			m_tableUrlToDefaultEngine.selectLast() ;
		} ;

		auto _setUpDownloadOptions = [ & ](){

			auto enable = !s->cookieArgument().isEmpty() ;

			m_ui.lineEditConfigureCookiePath->setText( m_settings.cookieFilePath( s->name() ) ) ;
			m_ui.lineEditConfigureCookiePath->setEnabled( enable ) ;
			m_ui.pbConfigureCookiePath->setEnabled( enable ) ;
			m_ui.labelPathToCookieFile->setEnabled( enable ) ;
		} ;

		if( tab == engineOptions::url ){

			_setUpUrl() ;

		}else if( tab == engineOptions::options ){

			_setUpDownloadOptions() ;
		}else{
			_setUpUrl() ;
			_setUpDownloadOptions() ;
		}
	}
}

void configure::savePresetOptions()
{
	auto& table = m_tablePresetOptions.get() ;

	auto rowCount = table.rowCount() ;

	m_presetOptions.clear() ;

	for( int i = 0 ; i < rowCount ; i++ ){

		auto website = table.item( i,0 )->text() ;
		auto uiName  = table.item( i,1 )->text() ;
		auto options = table.item( i,2 )->text() ;

		const auto& e = m_tablePresetOptions.stuffAt( i ) ;

		if( e.isEmpty() ){

			m_presetOptions.add( uiName,options,website ) ;
		}else{
			m_presetOptions.add( e,options,website ) ;
		}
	}
}

void configure::showOptions()
{
	m_presetOptions.forEach( [ this ]( const QString& uiName,const QString& options,const QString website ){

		if( uiName == "Default" ){

			m_tablePresetOptions.add( { website,tr( "Default" ),options },"Default" ) ;

		}else if( uiName == "Best-audio MP3" ){

			m_tablePresetOptions.add( { website,tr( "Best-audio MP3" ),options },"Best-audio MP3" ) ;

		}else if( uiName == "Best-audio Default" ){

			m_tablePresetOptions.add( { website,tr( "Best-audio Default" ),options },"Best-audio Default" ) ;

		}else if( uiName == "Best-audiovideo" ){

			m_tablePresetOptions.add( { website,tr( "Best-audiovideo" ),options },"Best-audiovideo" ) ;
		}else{
			m_tablePresetOptions.add( { website,uiName,options },{} ) ;
		}
	} )  ;
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

void configure::exiting()
{
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

	m_ui.label_6->setEnabled( true ) ;
	m_ui.label_3->setEnabled( true ) ;
	m_ui.label_4->setEnabled( true ) ;
	m_ui.label_5->setEnabled( true ) ;
	m_ui.lineEditConfigureTextEncoding->setEnabled( true ) ;
	m_ui.labelConfigureTextEncoding->setEnabled( true ) ;
	m_ui.cbCheckForUpdates->setEnabled( true ) ;
	m_ui.pbOpenThemeFolder->setEnabled( true ) ;
	m_ui.pbOpenBinFolder->setEnabled( true ) ;
	m_ui.cbConfigureEnginesUrlManager->setEnabled( true ) ;
	m_ui.tableWidgetConfigureUrl->setEnabled( true ) ;
	m_ui.labelConfigureEngines_2->setEnabled( true ) ;
	m_ui.pbConfigureManageUrl->setEnabled( true ) ;
	m_ui.lineEditConfigureManageUrl->setEnabled( true ) ;
	m_ui.lineEditConfigureManageOptions->setEnabled( true ) ;
	m_ui.lineEditAddDefaultDownloadOption->setEnabled( true ) ;
	m_ui.tableWidgetEnginesDefaultOptions->setEnabled( true ) ;
	m_ui.pbAddDefaultDownloadOption->setEnabled( true ) ;
	m_ui.tableWidgetConfigurePresetOptions->setEnabled( true ) ;
	m_ui.lineEditConfigurePresetOptions->setEnabled( true ) ;
	m_ui.lineEditConfigureUiName->setEnabled( true ) ;
	m_ui.labelConfugureUiName->setEnabled( true ) ;
	m_ui.labelConfigureOptionsPresetOptiions->setEnabled( true ) ;
	m_ui.pbConfigureAddToPresetList->setEnabled( true ) ;
	m_ui.pbConfigureEngineDefaultOptions->setEnabled( true ) ;
	m_ui.cbConfigureEngines->setEnabled( true ) ;
	m_ui.labelConfigureEngines->setEnabled( true ) ;
	m_ui.pbConfigureSave->setEnabled( true ) ;
	m_ui.lineEditConfigureMaximuConcurrentDownloads->setEnabled( true ) ;
	m_ui.comboBoxConfigureDarkTheme->setEnabled( true ) ;
	m_ui.pbConfigureDownload->setEnabled( true ) ;
	m_ui.labelConfigureTheme->setEnabled( true ) ;
	m_ui.cbConfigureShowVersionInfo->setEnabled( true ) ;
	m_ui.cbAutoSaveNotDownloadedMedia->setEnabled( true ) ;
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
	m_ui.cbShowTrayIcon->setEnabled( true ) ;
	m_ui.lineEditConfigureWebsite->setEnabled( true ) ;
	m_ui.labelConfugureWebSite->setEnabled( true ) ;

	if( m_settings.enabledHighDpiScaling() ){

		m_ui.lineEditConfigureScaleFactor->setEnabled( true ) ;
	}

	if( utility::platformIsLikeWindows() ){

		m_ui.cbUseSystemVersionIfAvailable->setEnabled( !m_settings.portableVersion() ) ;
	}else{
		m_ui.cbUseSystemVersionIfAvailable->setEnabled( utility::platformIsLinux() ) ;
	}
}

void configure::disableAll()
{
	m_ui.label_3->setEnabled( false ) ;
	m_ui.label_4->setEnabled( false ) ;
	m_ui.label_5->setEnabled( false ) ;
	m_ui.label_6->setEnabled( false ) ;
	m_ui.lineEditConfigureTextEncoding->setEnabled( false ) ;
	m_ui.labelConfigureTextEncoding->setEnabled( false ) ;
	m_ui.cbCheckForUpdates->setEnabled( false ) ;
	m_ui.pbOpenThemeFolder->setEnabled( false ) ;
	m_ui.labelConfigureEngines_2->setEnabled( false ) ;
	m_ui.cbConfigureEnginesUrlManager->setEnabled( false ) ;
	m_ui.lineEditConfigureManageUrl->setEnabled( false ) ;
	m_ui.tableWidgetConfigureUrl->setEnabled( false ) ;
	m_ui.pbConfigureManageUrl->setEnabled( false ) ;
	m_ui.pbOpenBinFolder->setEnabled( false ) ;
	m_ui.lineEditAddDefaultDownloadOption->setEnabled( false ) ;
	m_ui.tableWidgetEnginesDefaultOptions->setEnabled( false ) ;
	m_ui.cbShowTrayIcon->setEnabled( false ) ;
	m_ui.pbAddDefaultDownloadOption->setEnabled( false ) ;
	m_ui.lineEditConfigureManageOptions->setEnabled( false ) ;
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
	m_ui.cbAutoSaveNotDownloadedMedia->setEnabled( false ) ;
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
	m_ui.lineEditConfigureWebsite->setEnabled( false ) ;
	m_ui.labelConfugureWebSite->setEnabled( false ) ;
}

configure::presetOptions::presetOptions( const Context& ctx,settings& s ) :
	m_path( ctx.Engines().engineDirPaths().dataPath( "presetOptions.json" ) )
{
	QJsonParseError err ;

	QSettings& m = s.bk() ;

	QByteArray data ;

	if( QFile::exists( m_path ) ){

		QFile f( m_path ) ;

		f.open( QIODevice::ReadOnly ) ;

		data = f.readAll() ;

	}else if( m.contains( "PresetJsonOptions" ) ){

		auto a = m.value( "PresetJsonOptions" ).toByteArray() ;

		m.remove( "PresetJsonOptions" ) ;

		data = QByteArray::fromHex( a ) ;
	}else{
		data = this->defaultData() ;
	}

	auto json = QJsonDocument::fromJson( data,&err ) ;

	if( err.error == QJsonParseError::NoError ){

		m_array = json.array() ;
	}
}

configure::presetOptions::~presetOptions()
{
	QFile f( m_path ) ;
	f.open( QIODevice::WriteOnly | QIODevice::Truncate ) ;
	f.write( QJsonDocument( m_array ).toJson( QJsonDocument::Indented ) ) ;
}

void configure::presetOptions::clear()
{
	auto s = m_array.size() ;

	for( int i = 0 ; i < s ; i++ ){

		m_array.removeAt( 0 ) ;
	}
}

void configure::presetOptions::setDefaults()
{
	this->clear() ;

	QJsonParseError err ;

	auto json = QJsonDocument::fromJson( this->defaultData(),&err ) ;

	if( err.error == QJsonParseError::NoError ){

		m_array = json.array() ;
	}
}

void configure::presetOptions::add( const QString& uiName,const QString& options,const QString& website )
{
	QJsonObject o ;

	o.insert( "uiName",uiName ) ;
	o.insert( "options",options ) ;
	o.insert( "website",website ) ;

	m_array.append( o ) ;
}

QByteArray configure::presetOptions::defaultData()
{
	return R"R([
    {
	"options": "bestvideo[height=144][ext=mp4]+bestaudio[ext=m4a]/bestvideo[height=144]+bestaudio",
	"uiName": "144p",
	"website": "Youtube"
    },
    {
	"options": "bestvideo[height=240][ext=mp4]+bestaudio[ext=m4a]/bestvideo[height=240]+bestaudio",
	"uiName": "240p",
	"website": "Youtube"
    },
    {
	"options": "bestvideo[height=360][ext=mp4]+bestaudio[ext=m4a]/bestvideo[height=360]+bestaudio",
	"uiName": "360p",
	"website": "Youtube"
    },
    {
	"options": "bestvideo[height=480][ext=mp4]+bestaudio[ext=m4a]/bestvideo[height=480]+bestaudio",
	"uiName": "480p",
	"website": "Youtube"
    },
    {
	"options": "bestvideo[height=720][ext=mp4]+bestaudio[ext=m4a]/bestvideo[height=720]+bestaudio",
	"uiName": "720p",
	"website": "Youtube"
    },
    {
	"options": "bestvideo[height=1080][ext=mp4]+bestaudio[ext=m4a]/bestvideo[height=1080]+bestaudio",
	"uiName": "1080p",
	"website": "Youtube"
    },
    {
	"options": "bestvideo[height=1440][ext=mp4]+bestaudio[ext=m4a]/bestvideo[height=1440]+bestaudio",
	"uiName": "1440p",
	"website": "Youtube"
    },
    {
	"options": "bestvideo[height=2160][ext=mp4]+bestaudio[ext=m4a]/bestvideo[height=2160]+bestaudio",
	"uiName": "2160p",
	"website": "Youtube"
    },
    {
	"options": "Default",
	"uiName": "Default",
	"website": "Youtube"
    },
    {
	"options": "bestvideo+bestaudio",
	"uiName": "Best-audiovideo",
	"website": "Youtube"
    },
    {
	"options": "bestaudio -x --embed-thumbnail --audio-format mp3",
	"uiName": "Best-audio MP3",
	"website": "Youtube"
    },
    {
	"options": "bestaudio -x --embed-thumbnail",
	"uiName": "Best-audio Default",
	"website": "Youtube"
    }
])R";
}

configure::downloadDefaultOptions::downloadDefaultOptions( const Context& ctx,const QString& name ) :
	m_path( ctx.Engines().engineDirPaths().dataPath( name ) )
{
	QJsonParseError err ;

	if( QFile::exists( m_path ) ){

		QFile f( m_path ) ;

		f.open( QIODevice::ReadOnly ) ;

		auto json = QJsonDocument::fromJson( f.readAll(),&err ) ;

		if( err.error == QJsonParseError::NoError ){

			m_array = json.array() ;
		}
	}
}

void configure::downloadDefaultOptions::save()
{
	QFile f( m_path ) ;
	f.open( QIODevice::WriteOnly | QIODevice::Truncate ) ;
	f.write( QJsonDocument( m_array ).toJson( QJsonDocument::Indented ) ) ;
}

bool configure::downloadDefaultOptions::isEmpty( const QString& m )
{
	for( const auto& it : util::asConst( m_array ) ){

		auto s = it.toObject().value( "engineName" ).toString() ;

		if( s == m ){

			return false ;
		}
	}

	return true ;
}

QJsonObject configure::downloadDefaultOptions::addOpt( const QString& inUse,
						       const QString& engineName,
						       const QString& options )
{
	for( const auto& it : util::asConst( m_array ) ){

		auto obj = it.toObject() ;

		auto a = obj.value( "options" ).toString() ;
		auto b = obj.value( "engineName" ).toString() ;

		if( a == options && b == engineName ){

			return obj ;
		}
	}

	QJsonObject obj ;

	obj.insert( "default",inUse ) ;
	obj.insert( "options",options ) ;
	obj.insert( "engineName",engineName ) ;

	m_array.append( obj ) ;

	return obj ;
}

QJsonObject configure::downloadDefaultOptions::add( const QString& url,
						    const QString& opts,
						    const QString& engineName )
{
	return this->add( { "contains",opts,engineName,url } ) ;
}

QJsonObject configure::downloadDefaultOptions::add( const configure::downloadDefaultOptions::opts& e )
{
	QJsonObject obj ;

	obj.insert( "comparator",e.comparator ) ;
	obj.insert( "downloadOption",e.downloadOptions ) ;
	obj.insert( "engine",e.engine ) ;
	obj.insert( "url",e.url ) ;

	m_array.append( obj ) ;

	return obj ;
}

void configure::downloadDefaultOptions::remove( const QJsonObject& e )
{
	for( int i = 0 ; i < m_array.size() ; i++ ){

		if( m_array[ i ].toObject() == e ){

			m_array.removeAt( i ) ;
			break ;
		}
	}

	auto engineName = e.value( "engineName" ) ;

	for( int i = 0 ; i < m_array.size() ; i++ ){

		auto obj = m_array[ i ].toObject() ;

		auto name = obj.value( "engineName" ) ;

		if( name == engineName ){

			auto s = obj.value( "default" ) ;

			if( s == "yes" ){

				return ;
			}
		}
	}

	for( int i = 0 ; i < m_array.size() ; i++ ){

		auto obj = m_array[ i ].toObject() ;

		auto name = obj.value( "engineName" ) ;

		if( name == engineName ){

			obj.insert( "default","yes" ) ;

			m_array[ i ] = obj ;

			break ;
		}
	}
}

void configure::downloadDefaultOptions::removeAll( const QString& e )
{
	while( [ this,&e ](){

		for( int i = 0 ; i < m_array.size() ; i++ ){

			auto s = m_array[ i ].toObject().value( "engineName" ) ;

			if( s == e ){

				m_array.removeAt( i ) ;
				return true ;
			}
		}

		return false ;
	}() ){} ;
}

QJsonObject configure::downloadDefaultOptions::setAsDefault( const QJsonObject& e )
{
	auto engineName = e.value( "engineName" ).toString() ;
	auto options = e.value( "options" ).toString() ;

	QJsonObject xbj ;

	for( int i = 0 ; i < m_array.size() ; i++ ){

		auto obj = m_array[ i ].toObject() ;
		auto engine = obj.value( "engineName" ).toString() ;
		auto opts = obj.value( "options" ).toString() ;

		if( engine == engineName ){

			if( options == opts ){

				xbj = obj ;

				obj.insert( "default","yes" ) ;
			}else{
				obj.insert( "default","no" ) ;
			}

			m_array[ i ] = obj ;
		}
	}

	return xbj ;
}

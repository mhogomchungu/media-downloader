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
	m_tablePresetOptions( *m_ui.tableWidgetConfigurePresetOptions,m_ctx.mainWidget().font() ),
	m_tableUrlToDefaultEngine( *m_ui.tableWidgetConfigureUrl,m_ctx.mainWidget().font() ),
	m_presetOptions( m_ctx,m_settings ),
	m_downloadDefaultOptions( m_ctx )
{
	m_ui.tableWidgetConfigureUrl->setColumnWidth( 0,180 ) ;

	m_ui.widgetConfigureManageUrl->setVisible( false ) ;

	m_ui.lineEditConfigureScaleFactor->setEnabled( m_settings.enabledHighDpiScaling() ) ;

	m_ui.tabWidgetConfigure->setCurrentIndex( 0 ) ;

	settings::darkModes modes ;

	modes.setComboBox( *m_ui.comboBoxConfigureDarkTheme,m_settings.darkMode() ) ;

	auto cc = static_cast< void ( QComboBox::* )( int ) >( &QComboBox::currentIndexChanged ) ;

	m_tablePresetOptions.connect( &QTableWidget::currentItemChanged,[ this ]( QTableWidgetItem * c,QTableWidgetItem * p ){

		m_tablePresetOptions.selectRow( c,p,0 ) ;
	} ) ;

	m_tableUrlToDefaultEngine.connect( &QTableWidget::currentItemChanged,[ this ]( QTableWidgetItem * c,QTableWidgetItem * p ){

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

	m_tableUrlToDefaultEngine.connect( &QTableWidget::customContextMenuRequested,[ this ]( QPoint ){

		QMenu m ;

		connect( m.addAction( tr( "Add" ) ),&QAction::triggered,[ this ](){

			m_ui.widgetConfigureManageUrl->setVisible( true ) ;
		} ) ;

		connect( m.addAction( tr( "Delete" ) ),&QAction::triggered,[ this ](){

			auto m = m_tableUrlToDefaultEngine.currentRow() ;

			if( m != -1 ){

				m_downloadDefaultOptions.remove( m_tableUrlToDefaultEngine.stuffAt( m ) ) ;
				m_tableUrlToDefaultEngine.removeRow( m ) ;
				m_tableUrlToDefaultEngine.selectLast() ;
			}
		} ) ;

		m.exec( QCursor::pos() ) ;
	} ) ;

	connect( m_ui.pbConfigureManageUrlCancel,&QPushButton::clicked,[ this ](){

		m_ui.widgetConfigureManageUrl->setVisible( false ) ;
	} ) ;

	connect( m_ui.pbConfigureManageUrl,&QPushButton::clicked,[ this ](){

		auto a = m_ui.lineEditConfigureManageUrl->text() ;
		auto b = m_ui.lineEditConfigureManageOptions->text() ;
		auto c = m_ui.cbConfigureEngines->currentText() ;

		if( !a.isEmpty() ){

			if( b.isEmpty() ){

				b = "Default" ;
			}

			m_ui.lineEditConfigureManageUrl->clear() ;
			m_ui.lineEditConfigureManageOptions->clear() ;

			auto obj = m_downloadDefaultOptions.add( a,b,c ) ;

			m_tableUrlToDefaultEngine.add( { a,b },std::move( obj ) ) ;

			m_tableUrlToDefaultEngine.selectLast() ;

			m_ui.widgetConfigureManageUrl->setVisible( false ) ;
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

				auto name = m_ctx.Engines().addEngine( d,util::split( m,'/',true ).last() ) ;

				if( !name.isEmpty() ){

					auto& t = m_ctx.TabManager() ;

					t.basicDownloader().setAsActive() ;
					t.setDefaultEngines() ;
					m_ctx.versionInfo().check( m_ctx.Engines().defaultEngine( name ) ) ;
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

		m_presetOptions.setDefaults() ;
		m_tablePresetOptions.clear() ;

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
	m_ctx.versionInfo().network().download( iter ) ;
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

void configure::setDownloadOptions( int row,tableWidget& table )
{
	m_downloadDefaultOptions.setDownloadOptions( row,table ) ;
}

void configure::saveOptions()
{
	m_downloadDefaultOptions.save() ;

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

		const auto& engineName = s->name() ;

		m_tableUrlToDefaultEngine.clear() ;

		m_downloadDefaultOptions.forEach( [ this,&engineName ]( const downloadDefaultOptions::opts& e,QJsonObject obj ){

			if( engineName.isEmpty() || engineName == e.engine ){

				m_tableUrlToDefaultEngine.add( { e.url,e.downloadOptions },std::move( obj ) ) ;
			}

			return false ;
		} ) ;

		m_tableUrlToDefaultEngine.selectLast() ;

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
	auto& table = m_tablePresetOptions.get() ;

	auto rowCount = table.rowCount() ;

	m_presetOptions.clear() ;

	for( int i = 0 ; i < rowCount ; i++ ){

		auto uiName = table.item( i,0 )->text() ;
		auto options = table.item( i,1 )->text() ;

		m_presetOptions.add( uiName,options ) ;
	}
}

void configure::showOptions()
{
	this->presetOptionsForEach( [ this ]( const QString& uiName,const QString& options ){

		m_tablePresetOptions.add( { uiName,options } ) ;
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

void configure::presetOptions::add( const QString& uiName,const QString& options )
{
	QJsonObject o ;

	o.insert( "uiName",uiName ) ;
	o.insert( "options",options ) ;

	m_array.append( o ) ;
}

QByteArray configure::presetOptions::defaultData()
{
	return R"R([
    {
	"options": "bestvideo[height=144][ext=mp4]+bestaudio[ext=m4a]/bestvideo[height=144]+bestaudio",
	"uiName": "144p"
    },
    {
	"options": "bestvideo[height=240][ext=mp4]+bestaudio[ext=m4a]/bestvideo[height=240]+bestaudio",
	"uiName": "240p"
    },
    {
	"options": "bestvideo[height=360][ext=mp4]+bestaudio[ext=m4a]/bestvideo[height=360]+bestaudio",
	"uiName": "360p"
    },
    {
	"options": "bestvideo[height=480][ext=mp4]+bestaudio[ext=m4a]/bestvideo[height=480]+bestaudio",
	"uiName": "480p"
    },
    {
	"options": "bestvideo[height=720][ext=mp4]+bestaudio[ext=m4a]/bestvideo[height=720]+bestaudio",
	"uiName": "720p"
    },
    {
	"options": "bestvideo[height=1080][ext=mp4]+bestaudio[ext=m4a]/bestvideo[height=1080]+bestaudio",
	"uiName": "1080p"
    },
    {
	"options": "bestvideo[height=1440][ext=mp4]+bestaudio[ext=m4a]/bestvideo[height=1440]+bestaudio",
	"uiName": "1440p"
    },
    {
	"options": "bestvideo[height=2160][ext=mp4]+bestaudio[ext=m4a]/bestvideo[height=2160]+bestaudio",
	"uiName": "2160p"
    },
    {
	"options": "Default",
	"uiName": "Default"
    },
    {
	"options": "bestvideo+bestaudio",
	"uiName": "Best-audiovideo"
    },
    {
	"options": "bestaudio -x --embed-thumbnail --audio-format mp3",
	"uiName": "Best-audio"
    }
])R";
}

configure::downloadDefaultOptions::downloadDefaultOptions( const Context& ctx ) :
	m_path( ctx.Engines().engineDirPaths().dataPath( "downloadDefaultOptions.json" ) )
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

QJsonObject configure::downloadDefaultOptions::add( const QString& url,
						    const QString& opts,
						    const QString& engineName )
{
	QJsonObject obj ;

	obj.insert( "comparator","contains" ) ;
	obj.insert( "downloadOption",opts ) ;
	obj.insert( "engine",engineName ) ;
	obj.insert( "url",url ) ;

	m_array.append( obj ) ;

	return obj ;
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
}

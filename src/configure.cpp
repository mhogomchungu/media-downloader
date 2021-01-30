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

#include <QFileDialog>

configure::configure()
{
}

void configure::init( settings * settings,Ui::MainWindow * ui,QWidget * mainWidget )
{
	m_ui = ui ;
	m_mainWindow = mainWidget ;
	m_settings = settings ;

#if QT_VERSION < QT_VERSION_CHECK( 5,6,0 )
	m_ui->lineEditConfigureScaleFactor->setEnabled( false ) ;
#endif
	connect( m_ui->pbConfigureQuit,&QPushButton::clicked,[](){

		tabManager::instance().basicDownloader().appQuit() ;
	} ) ;

	connect( m_ui->pbConfigureSet,&QPushButton::clicked,[ this ](){

		m_settings->setHighDpiScalingFactor( m_ui->lineEditConfigureScaleFactor->text() ) ;
		m_settings->setPresetOptions( m_ui->textEditConfigurePresetOptions->toPlainText() ) ;
		m_settings->setDownloadFolder( m_ui->lineEditConfigureDownloadPath->text() ) ;

		tabManager::instance().resetMenu().basicDownloader().setAsActive() ;
	} ) ;

	connect( m_ui->pbConfigureSetPresetDefaults,&QPushButton::clicked,[ this ](){

		m_settings->setPresetToDefaults() ;

		m_ui->textEditConfigurePresetOptions->setPlainText( m_settings->presetOptions() ) ;
	} ) ;

	m_ui->pbConfigureDownloadPath->setIcon( [](){

		return QIcon( ":folder" ) ;
	}() ) ;

	connect( m_ui->pbConfigureDownloadPath,&QPushButton::clicked,[ this ](){

		auto e = QFileDialog::getExistingDirectory( m_mainWindow,
							    tr( "Set Download Folder" ),
							    QDir::homePath(),
							    QFileDialog::ShowDirsOnly ) ;

		if( !e.isEmpty() ){

			m_ui->lineEditConfigureDownloadPath->setText( e ) ;
		}
	} ) ;

	m_ui->lineEditConfigureScaleFactor->setText( m_settings->highDpiScalingFactor() ) ;

	m_ui->lineEditConfigureDownloadPath->setText( m_settings->downloadFolder() ) ;

	m_ui->textEditConfigurePresetOptions->setText( m_settings->presetOptions() ) ;
}

void configure::resetMenu()
{
}

void configure::enableAll()
{
	m_ui->lineEditConfigureDownloadPath->setEnabled( true ) ;
	m_ui->textEditConfigurePresetOptions->setEnabled( true ) ;
	m_ui->lineEditConfigureDownloadPath->setEnabled( true ) ;
	m_ui->pbConfigureDownloadPath->setEnabled( true ) ;
	m_ui->pbConfigureSet->setEnabled( true ) ;
	m_ui->pbConfigureSetPresetDefaults->setEnabled( true ) ;
	m_ui->labelConfigureScaleFactor->setEnabled( true ) ;
	m_ui->labelConfigurePresetOptions->setEnabled( true ) ;
	m_ui->labelConfigureDownloadPath->setEnabled( true ) ;
	m_ui->pbConfigureQuit->setEnabled( true ) ;
#if QT_VERSION >= QT_VERSION_CHECK( 5,6,0 )
	m_ui->lineEditConfigureScaleFactor->setEnabled( true ) ;
#endif
}

void configure::disableAll()
{
	m_ui->pbConfigureQuit->setEnabled( false ) ;
	m_ui->lineEditConfigureScaleFactor->setEnabled( false ) ;
	m_ui->lineEditConfigureDownloadPath->setEnabled( false ) ;
	m_ui->textEditConfigurePresetOptions->setEnabled( false ) ;
	m_ui->lineEditConfigureDownloadPath->setEnabled( false ) ;
	m_ui->pbConfigureDownloadPath->setEnabled( false ) ;
	m_ui->pbConfigureSet->setEnabled( false ) ;
	m_ui->pbConfigureSetPresetDefaults->setEnabled( false ) ;
	m_ui->labelConfigureScaleFactor->setEnabled( false ) ;
	m_ui->labelConfigurePresetOptions->setEnabled( false ) ;
	m_ui->labelConfigureDownloadPath->setEnabled( false ) ;
}

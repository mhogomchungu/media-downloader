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
#include "ui_mainwindow.h"

#include <QFile>
#include <QDir>
#include <QMenu>
#include <QProcess>
#include <QFileDialog>

#include <iostream>

static QStringList _split( const QString& e,char token = '\n' )
{
#if QT_VERSION < QT_VERSION_CHECK( 5,15,0 )
	return e.split( token,QString::SkipEmptyParts ) ;
#else
	return e.split( token,Qt::SkipEmptyParts ) ;
#endif
}

MainWindow::MainWindow( QSettings& settings ) :
	QMainWindow( nullptr ),
	m_settings( settings ),
	m_ui( new Ui::MainWindow )
{
	m_ui->setupUi( this ) ;

	m_ui->tabWidget->setCurrentIndex( 0 ) ;

	m_ui->pbCancel->setEnabled( false ) ;

	m_ui->plainTextEdit->setReadOnly( true ) ;

	m_ui->frame->setVisible( false ) ;

	this->window()->setFixedSize( this->window()->size() ) ;

	this->window()->setWindowIcon( [](){

		return QIcon( ":media-downloader" ) ;
	}() ) ;

	m_ui->pbDownloadPath->setIcon( [](){

		return QIcon( ":folder" ) ;
	}() ) ;

	connect( m_ui->pbList,&QPushButton::clicked,[ this ](){

		this->list() ;
	} ) ;

	if( !m_settings.contains( "PresetNumbers" ) ){

		m_settings.setValue( "PresetNumbers",QStringList( { "18","22" } ) ) ;
	}

	m_ui->pbEntries->setMenu( this->setMenu() ) ;

	connect( m_ui->pbDownload,&QPushButton::clicked,[ this ](){

		this->download() ;
	} ) ;

	connect( m_ui->pbConfigure,&QPushButton::clicked,[ this ](){

		m_ui->frame->setVisible( true ) ;

		m_ui->pbConfigure->setEnabled( false ) ;

		m_ui->pbDownloadPath->setFocus() ;

		auto m = m_settings.value( "PresetNumbers" ).toStringList().join( ',' ) ;

		m_ui->lineEditPresetNumbers->setText( m ) ;

		this->disableAll() ;
	} ) ;

	connect( m_ui->pbDownloadPath,&QPushButton::clicked,[ this ](){

		auto e = QFileDialog::getExistingDirectory( this,tr( "Set Download Folder"),QDir::homePath(),QFileDialog::ShowDirsOnly ) ;

		if( !e.isEmpty() ){

			m_settings.setValue( "DownloadFolder",e ) ;

			m_ui->lineEditDownloadPath->setText( e ) ;
		}
	} ) ;

	connect( m_ui->pbSet,&QPushButton::clicked,[ this ](){

		m_ui->frame->setVisible( false ) ;

		m_ui->pbConfigure->setEnabled( true ) ;

		m_settings.setValue( settings::EnabledHighDpiScalingFactor,m_ui->lineEditScaleFactor->text() ) ;

		m_settings.setValue( "PresetNumbers",_split( m_ui->lineEditPresetNumbers->text(),',' ) ) ;

		m_ui->pbEntries->setMenu( this->setMenu() ) ;

		this->enableAll() ;
	} ) ;

	if( !m_settings.contains( "DownloadFolder" ) ){

#ifdef Q_OS_LINUX
		m_settings.setValue( "DownloadFolder",QDir::homePath() ) ;
	#else
		m_settings.setValue( "DownloadFolder",QDir::homePath() + "/Desktop" ) ;
#endif
	}

	m_ui->lineEditScaleFactor->setText( settings.value( settings::EnabledHighDpiScalingFactor ).toByteArray() ) ;

	m_ui->lineEditDownloadPath->setText( m_settings.value( "DownloadFolder" ).toString() ) ;

	m_downloadFolder = m_settings.value( "DownloadFolder" ).toString() ;

	m_trayIcon.setIcon( [](){

		return QIcon( ":media-downloader" ) ;
	}() ) ;

	auto m = new QMenu( this ) ;

	connect( m->addAction( "Quit" ),&QAction::triggered,[](){

		QCoreApplication::quit() ;
	} ) ;

	m_trayIcon.setContextMenu( m ) ;

	m_trayIcon.show() ;
}

MainWindow::~MainWindow()
{
	delete m_ui ;
}

void MainWindow::run( const QString& cmd,const QStringList& args )
{
	this->disableAll() ;

	auto exe = new QProcess() ;

	exe->setWorkingDirectory( m_downloadFolder ) ;

	exe->setProcessChannelMode( QProcess::ProcessChannelMode::MergedChannels ) ;

	auto conn = QObject::connect( m_ui->pbCancel,&QPushButton::clicked,[ exe ](){

		exe->terminate() ;
	} ) ;

	connect( exe,&QProcess::readyReadStandardOutput,[ this,exe ](){

		const auto split = _split( exe->readAllStandardOutput() ) ;

		for( const auto& m : split ){

			if( m.startsWith( "[download] Destination" ) ){

				m_tmp.append( m ) ;

			}else if( m.startsWith( "[download]" ) && m.contains( "ETA" ) ){

				auto& s = m_tmp.last() ;

				if( s.startsWith( "[download]" ) && s.contains( "ETA" ) ){

					s = m ;
				}else{
					m_tmp.append( m ) ;
				}
			}else{
				m_tmp.append( m ) ;
			}
		}

		m_ui->plainTextEdit->setPlainText( m_tmp.join( '\n' ) ) ;

		m_ui->plainTextEdit->moveCursor( QTextCursor::End ) ;
	} ) ;

	auto s = static_cast< void( QProcess::* )( int,QProcess::ExitStatus ) >( &QProcess::finished ) ;

	connect( exe,s,[ this,exe,conn ]( int e,QProcess::ExitStatus ss ){

		Q_UNUSED( e )
		Q_UNUSED( ss )

		QObject::disconnect( conn ) ;

		exe->deleteLater() ;

		this->enableAll() ;

		m_ui->pbCancel->setEnabled( false ) ;
	} ) ;

	exe->start( cmd,args ) ;
}

void MainWindow::list()
{
	m_ui->pbCancel->setEnabled( true ) ;

	m_tmp.clear() ;

	this->run( "youtube-dl",{ "-F",m_ui->lineEditURL->text() } ) ;
}

void MainWindow::download()
{
	m_ui->pbCancel->setEnabled( true ) ;

	auto entry = m_ui->lineEditNumber->text() ;
	auto url = m_ui->lineEditURL->text() ;

	QStringList args ;

	args.append( "--newline" ) ;
	args.append( "--ignore-config" ) ;

	if( entry.isEmpty() ){

		args.append( url ) ;
	}else{
		auto m = _split( entry,' ' ) ;

		if( m.size() == 1 && !m.at( 0 ).startsWith( '-' ) ){

			args.append( "-f" ) ;
			args.append( entry ) ;
			args.append( url ) ;
		}else{
			for( const auto& it : m ){

				args.append( it ) ;
			}

			args.append( url ) ;
		}
	}

	this->run( "youtube-dl",args ) ;
}

void MainWindow::enableAll()
{
	m_ui->pbEntries->setEnabled( true ) ;
	m_ui->pbConfigure->setEnabled( true ) ;
	m_ui->label_2->setEnabled( true ) ;
	m_ui->label->setEnabled( true ) ;
	m_ui->pbList->setEnabled( true ) ;
	m_ui->pbDownload->setEnabled( true ) ;
	m_ui->lineEditURL->setEnabled( true ) ;
	m_ui->lineEditNumber->setEnabled( true ) ;
}

void MainWindow::disableAll()
{
	m_ui->pbEntries->setEnabled( false ) ;
	m_ui->pbConfigure->setEnabled( false ) ;
	m_ui->label_2->setEnabled( false ) ;
	m_ui->label->setEnabled( false ) ;
	m_ui->pbList->setEnabled( false ) ;
	m_ui->pbDownload->setEnabled( false ) ;
	m_ui->lineEditURL->setEnabled( false ) ;
	m_ui->lineEditNumber->setEnabled( false ) ;
}

void MainWindow::closeEvent( QCloseEvent * e )
{
	e->ignore() ;

	this->hide() ;

	QCoreApplication::exit() ;
}

QMenu * MainWindow::setMenu()
{
	auto m = m_ui->pbEntries->menu() ;

	if( m ){

		m->deleteLater() ;
	}

	const auto entries = m_settings.value( "PresetNumbers" ).toStringList() ;

	auto menu = new QMenu( this ) ;

	for( const auto& it : entries ){

		menu->addAction( it ) ;
	}

	connect( menu,&QMenu::triggered,[ this ]( QAction * ac ){

		m_ui->lineEditNumber->setText( ac->text() ) ;
	} ) ;

	return menu ;
}

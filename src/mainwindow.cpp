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

#include <iostream>

MainWindow::MainWindow( QWidget * parent ) :
	QMainWindow( parent ),
	m_settings( "media-downloader","media-downloader" ),
	m_ui( new Ui::MainWindow )
{
	m_ui->setupUi( this ) ;

	m_ui->tabWidget->setCurrentIndex( 0 ) ;

	m_ui->pbCancel->setEnabled( false ) ;

	m_ui->plainTextEdit->setReadOnly( true ) ;

	this->window()->setFixedSize( this->window()->size() ) ;

	QIcon icon( ":media-downloader" ) ;

	this->window()->setWindowIcon( icon ) ;

	connect( m_ui->pbList,&QPushButton::clicked,[ this ](){

		this->list() ;
	} ) ;

	connect( m_ui->pbDownload,&QPushButton::clicked,[ this ](){

		this->download() ;
	} ) ;

	if( !m_settings.contains( "DownloadFolder" ) ){

#ifdef Q_OS_LINUX
		m_settings.setValue( "DownloadFolder",QDir::homePath() ) ;
	#else
		m_settings.setValue( "DownloadFolder",QDir::homePath() + "/Desktop" ) ;
#endif
	}

	m_downloadFolder = m_settings.value( "DownloadFolder" ).toString() ;

	m_trayIcon.setIcon( icon ) ;

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

		QString a = exe->readAllStandardOutput() ;

		const auto split = a.split( '\n',Qt::SkipEmptyParts ) ;

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

	auto a = m_ui->lineEditNumber->text() ;
	auto b = m_ui->lineEditURL->text() ;

	if( a.isEmpty() ){

		this->run( "youtube-dl",{ "--newline",b } ) ;
	}else{
		this->run( "youtube-dl",{ "--newline","-f",a,b } ) ;
	}
}

void MainWindow::enableAll()
{
	m_ui->label_2->setEnabled( true ) ;
	m_ui->label->setEnabled( true ) ;
	m_ui->pbList->setEnabled( true ) ;
	m_ui->pbDownload->setEnabled( true ) ;
	m_ui->lineEditURL->setEnabled( true ) ;
	m_ui->lineEditNumber->setEnabled( true ) ;
}

void MainWindow::disableAll()
{
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

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

#include "basicdownloader.h"
#include "ui_mainwindow.h"

#include <QFile>
#include <QDir>
#include <QMenu>
#include <QProcess>
#include <QFileDialog>

basicdownloader::basicdownloader( settings& s ) : m_settings( s )
{
}

void basicdownloader::init( Ui::MainWindow * mainWidgetUI,QWidget * mainWidget )
{
	m_ui = mainWidgetUI ;
	m_mainWindow = mainWidget ;

	m_ui->tabWidget->setCurrentIndex( 0 ) ;

	m_ui->pbCancel->setEnabled( false ) ;

	m_ui->plainTextEdit->setReadOnly( true ) ;

	m_ui->frame->setVisible( false ) ;

	m_ui->pbDownloadPath->setIcon( [](){

		return QIcon( ":folder" ) ;
	}() ) ;

	connect( m_ui->pbList,&QPushButton::clicked,[ this ](){

		this->list() ;
	} ) ;

	m_ui->pbEntries->setMenu( this->setMenu() ) ;

	connect( m_ui->pbDownload,&QPushButton::clicked,[ this ](){

		this->download() ;
	} ) ;

	connect( m_ui->pbSetPresetDefaults,&QPushButton::clicked,[ this ](){

		m_settings.setPresetToDefaults() ;

		m_ui->textEditPresetOptions->setPlainText( m_settings.presetOptions() ) ;
	} ) ;

	connect( m_ui->pbQuit,&QPushButton::clicked,[](){

		QCoreApplication::quit() ;
	} ) ;

	connect( m_ui->pbConfigure,&QPushButton::clicked,[ this ](){

		m_ui->plainTextEdit->setEnabled( false ) ;

		m_ui->frame->setVisible( true ) ;

		m_ui->pbConfigure->setEnabled( false ) ;

		m_ui->pbDownloadPath->setFocus() ;

		auto m = m_settings.presetOptions() ;

		m_ui->textEditPresetOptions->setPlainText( m ) ;

		this->disableAll() ;
	} ) ;

	connect( m_ui->pbDownloadPath,&QPushButton::clicked,[ this ](){

		auto e = QFileDialog::getExistingDirectory( m_mainWindow,
							    tr( "Set Download Folder" ),
							    QDir::homePath(),
							    QFileDialog::ShowDirsOnly ) ;

		if( !e.isEmpty() ){

			m_ui->lineEditDownloadPath->setText( e ) ;
		}
	} ) ;

	connect( m_ui->pbSet,&QPushButton::clicked,[ this ](){

		m_ui->plainTextEdit->setEnabled( true ) ;

		m_ui->frame->setVisible( false ) ;

		m_ui->pbConfigure->setEnabled( true ) ;

		m_settings.setHighDpiScalingFactor( m_ui->lineEditScaleFactor->text() ) ;
		m_settings.setPresetOptions( m_ui->textEditPresetOptions->toPlainText() ) ;
		m_settings.setDownloadFolder( m_ui->lineEditDownloadPath->text() ) ;

		m_ui->pbEntries->setMenu( this->setMenu() ) ;

		this->enableAll() ;
	} ) ;

	m_ui->lineEditScaleFactor->setText( m_settings.highDpiScalingFactor() ) ;

	m_ui->lineEditDownloadPath->setText( m_settings.downloadFolder() ) ;
}

void basicdownloader::run( const QString &cmd,const QStringList& args )
{
	m_tmp.append( "[media-downloader] cmd: " + [ & ](){

		auto m = cmd ;

		for( const auto& it : args ){

			m += " \"" + it + "\"" ;
		}

		return m + "\n" ;
	}() ) ;

	this->disableAll() ;

	utility::run( cmd,args,utility::readChannel::stdOut,[ this ]( QProcess& exe ){

		exe.setWorkingDirectory( m_settings.downloadFolder() ) ;

		exe.setProcessChannelMode( QProcess::ProcessChannelMode::MergedChannels ) ;

		return QObject::connect( m_ui->pbCancel,&QPushButton::clicked,[ &exe ](){

			exe.terminate() ;
		} ) ;

	},[ this ]( int,QProcess::ExitStatus,QMetaObject::Connection& conn ){

		QObject::disconnect( conn ) ;

		this->enableAll() ;

		m_ui->pbCancel->setEnabled( false ) ;

	},[ this ]( const QByteArray& data,QMetaObject::Connection& ){

		for( const auto& m : utility::split( data ) ){

			if( m.isEmpty() ){

				continue ;

			}else if( !m_tmp.isEmpty() && m.startsWith( "[download]" ) && m.contains( "ETA" ) ){

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
}

void basicdownloader::list()
{
	m_ui->pbCancel->setEnabled( true ) ;

	m_tmp.clear() ;

	auto args = m_settings.defaultListCmdOptions() ;
	args.append( m_ui->lineEditURL->text() ) ;

	this->run( m_settings.cmdName(),args ) ;
}

void basicdownloader::download()
{
	m_ui->pbCancel->setEnabled( true ) ;

	auto entry = m_ui->lineEditNumber->text() ;
	auto url = m_ui->lineEditURL->text() ;

	auto args = m_settings.defaultDownLoadCmdOptions() ;

	if( entry.isEmpty() ){

		args.append( url ) ;
	}else{
		auto m = utility::split( entry,' ' ) ;

		if( m.size() == 1 && !m.at( 0 ).startsWith( '-' ) ){

			args.append( "-f" ) ;
			args.append( entry ) ;
			args.append( url ) ;
		}else{
			if( m.contains( "--yes-playlist" ) ){

				args.removeAll( "--no-playlist" ) ;
			}

			for( const auto& it : m ){

				args.append( it ) ;
			}

			args.append( url ) ;
		}
	}

	this->run( m_settings.cmdName(),args ) ;
}

void basicdownloader::enableAll()
{
	m_ui->pbEntries->setEnabled( true ) ;
	m_ui->pbConfigure->setEnabled( true ) ;
	m_ui->label_2->setEnabled( true ) ;
	m_ui->label->setEnabled( true ) ;
	m_ui->pbList->setEnabled( true ) ;
	m_ui->pbDownload->setEnabled( true ) ;
	m_ui->lineEditURL->setEnabled( true ) ;
	m_ui->lineEditNumber->setEnabled( true ) ;
	m_ui->pbQuit->setEnabled( true ) ;
}

void basicdownloader::disableAll()
{
	m_ui->pbQuit->setEnabled( false ) ;
	m_ui->pbEntries->setEnabled( false ) ;
	m_ui->pbConfigure->setEnabled( false ) ;
	m_ui->label_2->setEnabled( false ) ;
	m_ui->label->setEnabled( false ) ;
	m_ui->pbList->setEnabled( false ) ;
	m_ui->pbDownload->setEnabled( false ) ;
	m_ui->lineEditURL->setEnabled( false ) ;
	m_ui->lineEditNumber->setEnabled( false ) ;
}

QMenu * basicdownloader::setMenu()
{
	auto m = m_ui->pbEntries->menu() ;

	if( m ){

		m->deleteLater() ;
	}

	const auto entries = m_settings.presetOptionsList() ;

	auto menu = new QMenu( m_mainWindow ) ;

	for( const auto& it : entries ){

		auto a = it ;

		a.replace( "\n","" ) ;

		auto b = a.lastIndexOf( '(' ) ;

		if( b != -1 ){

			auto m = a.mid( 0,b ) ;
			auto mm = a.mid( b + 1 ) ;
			mm.truncate( mm.size() - 1 ) ;
			menu->addAction( m )->setObjectName( mm ) ;
		}else{
			menu->addAction( it )->setObjectName( it ) ;
		}
	}

	menu->addSeparator() ;

	menu->addAction( tr( "Clear Options" ) )->setObjectName( "Clear Options" ) ;
	menu->addAction( tr( "Clear Screen" ) )->setObjectName( "Clear Screen" ) ;

	connect( menu,&QMenu::triggered,[ this ]( QAction * ac ){

		const auto& e = ac->objectName() ;

		if( e == "Clear Options" ){

			m_ui->lineEditNumber->clear() ;

		}else if( e == "Clear Screen" ){

			m_tmp.clear() ;

			m_ui->plainTextEdit->clear() ;
		}else{
			auto m = ac->text() ;

			if( m == "Best" ){

				m_ui->lineEditNumber->setText( "best" ) ;
			}else{
				m_ui->lineEditNumber->setText( ac->objectName() ) ;
			}
		}
	} ) ;

	return menu ;
}

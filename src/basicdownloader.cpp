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

#include <QFile>
#include <QDir>
#include <QMenu>
#include <QFileDialog>

#include "tabmanager.h"

basicdownloader::basicdownloader()
{
}

void basicdownloader::init( settings * settings,Ui::MainWindow * ui,QWidget * mainWidget )
{
	m_ui = ui ;
	m_mainWindow = mainWidget ;
	m_settings = settings ;

	this->setAsActive() ;

	m_ui->pbCancel->setEnabled( false ) ;

	m_ui->plainTextEdit->setReadOnly( true ) ;

	connect( m_ui->pbList,&QPushButton::clicked,[ this ](){

		this->list() ;
	} ) ;

	this->resetMenu() ;

	connect( m_ui->pbDownload,&QPushButton::clicked,[ this ](){

		this->download() ;
	} ) ;

	connect( m_ui->pbQuit,&QPushButton::clicked,[ this ](){

		this->appQuit() ;
	} ) ;
}

void basicdownloader::resetMenu()
{
	utility::setMenuOptions( m_settings,true,m_ui->pbEntries,[ this ]( QAction * aa ){

		utility::selectedAction ac( aa ) ;

		if( ac.clearOptions() ){

			m_ui->lineEditOptions->clear() ;

		}else if( ac.clearScreen() ){

			m_tmp.clear() ;

			m_ui->plainTextEdit->clear() ;
		}else{
			if( ac.best() ){

				m_ui->lineEditOptions->setText( ac.bestText() ) ;
			}else{
				m_ui->lineEditOptions->setText( ac.objectName() ) ;
			}
		}
	} ) ;
}

void basicdownloader::setAsActive()
{
	m_ui->tabWidget->setCurrentIndex( 0 ) ;
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

	tabManager::instance().disableAll() ;

	utility::run( cmd,args,[ this ]( QProcess& exe ){

		exe.setWorkingDirectory( m_settings->downloadFolder() ) ;

		exe.setProcessChannelMode( QProcess::ProcessChannelMode::MergedChannels ) ;

		return QObject::connect( m_ui->pbCancel,&QPushButton::clicked,[ &exe ](){

			exe.terminate() ;
		} ) ;

	},[ this ]( int,QProcess::ExitStatus,QMetaObject::Connection& conn ){

		QObject::disconnect( conn ) ;

		tabManager::instance().enableAll() ;

		m_ui->pbCancel->setEnabled( false ) ;

	},[ this ]( QProcess::ProcessChannel,const QByteArray& data,QMetaObject::Connection& ){

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

	auto args = m_settings->defaultListCmdOptions() ;
	args.append( m_ui->lineEditURL->text() ) ;

	this->run( m_settings->cmdName(),args ) ;
}

void basicdownloader::download()
{
	this->download( m_ui->lineEditOptions->text(),{ m_ui->lineEditURL->text() },false ) ;
}

void basicdownloader::download( const QString& options,const QStringList& urls,bool update )
{
	if( update ){

		m_ui->lineEditOptions->setText( options ) ;

		m_ui->lineEditURL->setText( urls.join( ' ' ) ) ;
	}

	m_ui->tabWidget->setCurrentIndex( 0 ) ;

	m_ui->pbCancel->setEnabled( true ) ;

	auto args = m_settings->defaultDownLoadCmdOptions() ;

	if( options.isEmpty() ){

		args.append( urls ) ;
	}else{
		auto m = utility::split( options,' ' ) ;

		if( m.size() == 1 && !m.at( 0 ).startsWith( '-' ) ){

			args.append( "-f" ) ;
			args.append( options ) ;
			args.append( urls ) ;
		}else{
			if( m.contains( "--yes-playlist" ) ){

				args.removeAll( "--no-playlist" ) ;
			}

			for( const auto& it : m ){

				args.append( it ) ;
			}

			args.append( urls ) ;
		}
	}

	this->run( m_settings->cmdName(),args ) ;
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
	m_ui->lineEditOptions->setEnabled( true ) ;
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
	m_ui->lineEditOptions->setEnabled( false ) ;
}

void basicdownloader::appQuit()
{
	QCoreApplication::quit() ;
}

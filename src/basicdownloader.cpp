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
#include <QtDebug>
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

	m_ui->labelFailedToFixExe->setVisible( false ) ;

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

void basicdownloader::init_done()
{
	if( utility::platformIsWindows() ){

		QStringList paths{ QDir::currentPath(),QDir::homePath() + "/bin" } ;

		QString e ;

		auto m = m_settings->cmdName() ;

		for( const auto& it : paths ){

			e = it + "/" + m ;

			if( QFile::exists( e ) ){

				m_exe = e ;

				return ;
			}
		}

		this->failedToFindExe( paths ) ;
	}else{
		m_exe = m_settings->cmdName() ;
	}
}

void basicdownloader::resetMenu( const QStringList& args )
{
	utility::setMenuOptions( m_settings,args,true,m_ui->pbEntries,[ this ]( QAction * aa ){

		utility::selectedAction ac( aa ) ;

		if( ac.clearOptions() ){

			m_ui->lineEditOptions->clear() ;

		}else if( ac.clearScreen() ){

			m_ui->plainTextEdit->clear() ;
			m_ui->lineEditURL->clear() ;
			m_ui->lineEditOptions->clear() ;
		}else{
			if( ac.best() ){

				m_ui->lineEditOptions->setText( ac.bestText() ) ;
			}else{
				m_ui->lineEditOptions->setText( ac.objectName() ) ;
			}

			if( m_settings->autoDownload() ){

				if( !m_ui->lineEditURL->text().isEmpty() ){

					this->download() ;
				}
			}
		}
	} ) ;
}

void basicdownloader::setAsActive()
{
	m_ui->tabWidget->setCurrentIndex( 0 ) ;
}

void basicdownloader::run( const QString &cmd,const QStringList& args,bool list_requested )
{
	tabManager::instance().disableAll() ;

	struct context{
		bool list_requested ;
		QStringList output ;
		QMetaObject::Connection conn ;
	} ;

	utility::run( cmd,args,[ this,&list_requested,&cmd,&args ]( QProcess& exe ){

		m_ui->plainTextEdit->setPlainText( "[media-downloader] cmd: " + [ & ](){

			auto m = cmd ;

			for( const auto& it : args ){

				m += " \"" + it + "\"" ;
			}

			return m + "\n" ;
		}() ) ;

		m_ui->plainTextEdit->moveCursor( QTextCursor::End ) ;

		exe.setWorkingDirectory( m_settings->downloadFolder() ) ;

		exe.setProcessChannelMode( QProcess::ProcessChannelMode::MergedChannels ) ;

		auto m = QObject::connect( m_ui->pbCancel,&QPushButton::clicked,[ &exe ](){

			exe.terminate() ;
		} ) ;

		return context{ list_requested,{},std::move( m ) } ;

	},[ this ]( int,QProcess::ExitStatus,context& ctx ){

		QObject::disconnect( ctx.conn ) ;

		if( ctx.list_requested ){

			this->listRequested( ctx.output ) ;
		}

		tabManager::instance().enableAll() ;

		m_ui->pbCancel->setEnabled( false ) ;

	},[ this ]( QProcess::ProcessChannel,const QByteArray& data,context& ctx ){

		auto& output = ctx.output ;

		for( const auto& m : utility::split( data ) ){

			if( m.isEmpty() ){

				continue ;

			}else if( !output.isEmpty() && m.startsWith( "[download]" ) && m.contains( "ETA" ) ){

				auto& s = output.last() ;

				if( s.startsWith( "[download]" ) && s.contains( "ETA" ) ){

					s = m ;
				}else{
					output.append( m ) ;
				}
			}else{
				output.append( m ) ;
			}
		}

		m_ui->plainTextEdit->setPlainText( output.join( '\n' ) ) ;

		m_ui->plainTextEdit->moveCursor( QTextCursor::End ) ;
	} ) ;
}

void basicdownloader::listRequested( const QStringList& args )
{
	QStringList opts ;

	for( const auto& it : args ){

		auto a = utility::split( it,' ',true ) ;

		const auto& e = a.at( 0 ) ;

		if( utility::hasDigitsOnly( e ) ){

			if( utility::hasDigitsOnly( e ) ){

				opts.append( a.at( 0 ) ) ;
			}
		}
	}

	this->resetMenu( opts ) ;
}

void basicdownloader::failedToFindExe( const QStringList& e )
{
	tabManager::instance().disableAll() ;

	m_ui->pbQuit->setEnabled( true ) ;

	QString m ;

	for( const auto& it : e ){

		m += it + "\n" ;
	}

	m.truncate( m.size() - 1 ) ;

	auto a = tr( "Failed To Locate \"%1\" in Below Paths:-" ).arg( m_settings->cmdName() ) ;
	auto b = "-------------------------" ;
	auto c = tr( "Please Add It In One Of The Paths And Restart" ) ;

	m_ui->labelFailedToFixExe->setText( a + "\n\n" + b + "\n" + m + "\n" + b + "\n\n" + c ) ;

	m_ui->labelFailedToFixExe->setVisible( true ) ;
}

void basicdownloader::list()
{
	m_ui->pbCancel->setEnabled( true ) ;

	auto args = m_settings->defaultListCmdOptions() ;
	args.append( m_ui->lineEditURL->text().split( ' ' ) ) ;

	this->run( m_exe,args,utility::youtubePath( m_ui->lineEditURL->text() ) ) ;
}

void basicdownloader::download()
{
	this->download( m_ui->lineEditOptions->text(),m_ui->lineEditURL->text(),false ) ;
}

void basicdownloader::download( const utility::args& args,const QString& url,bool s )
{
	this->download( args,QStringList( url ),s ) ;
}

void basicdownloader::download( const utility::args& args,
				const QStringList& urls,
				bool update )
{
	if( update ){

		m_ui->lineEditOptions->setText( args.quality + " " + args.otherOptions.join( ' ' ) ) ;

		m_ui->lineEditURL->setText( urls.join( ' ' ) ) ;
	}

	m_ui->tabWidget->setCurrentIndex( 0 ) ;

	m_ui->pbCancel->setEnabled( true ) ;

	auto opts = m_settings->defaultDownLoadCmdOptions() ;

	if( args.otherOptions.contains( "--yes-playlist" ) ){

		opts.removeAll( "--no-playlist" ) ;
	}

	for( const auto& it : args.otherOptions ){

		opts.append( it ) ;
	}

	if( !args.quality.isEmpty() ){

		opts.append( "-f" ) ;
		opts.append( args.quality ) ;
	}

	opts.append( urls ) ;

	this->run( m_exe,opts,false ) ;
}

void basicdownloader::enableAll()
{
	m_ui->pbEntries->setEnabled( true ) ;
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

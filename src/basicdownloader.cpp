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
#include <QStandardPaths>

#include "tabmanager.h"

basicdownloader::basicdownloader( Context& ctx ) :
	m_ctx( ctx ),
	m_settings( m_ctx.Settings() ),
	m_ui( m_ctx.Ui() ),
	m_tabManager( m_ctx.TabManager() )
{
	this->setAsActive() ;

	m_ui.pbCancel->setEnabled( false ) ;

	m_ui.plainTextEdit->setReadOnly( true ) ;

	m_ui.labelFailedToFixExe->setVisible( false ) ;

	connect( m_ui.pbList,&QPushButton::clicked,[ this ](){

		this->list() ;
	} ) ;

	this->resetMenu() ;

	connect( m_ui.pbDownload,&QPushButton::clicked,[ this ](){

		this->download() ;
	} ) ;

	connect( m_ui.pbQuit,&QPushButton::clicked,[ this ](){

		this->appQuit() ;
	} ) ;
}

void basicdownloader::init_done()
{
	const auto& exe = m_settings.backEnd().name() ;

	if( m_settings.usePrivateBackEnd() ){

		m_exe = m_settings.backendPath() + "/" + exe ;

		if( QFile::exists( m_exe ) ){

			this->checkAndPrintInstalledVersion() ;
		}else{
			m_ctx.TabManager().Configure().downloadYoutubeDl() ;
		}
	}else{
		m_exe = QStandardPaths::findExecutable( exe ) ;

		if( m_exe.isEmpty() ){

			auto m = tr( "Failed to find version information, make sure \"%1\" is installed and works properly" ).arg( exe )  ;

			m = "[media-downloader] " + m ;

			m_ui.plainTextEdit->setPlainText( m ) ;

			m_tabManager.disableAll() ;

			this->enableQuit() ;
		}else{
			this->checkAndPrintInstalledVersion() ;
		}
	}
}

void basicdownloader::resetMenu( const QStringList& args )
{
	utility::setMenuOptions( m_ctx,args,true,m_ui.pbEntries,[ this ]( QAction * aa ){

		utility::selectedAction ac( aa ) ;

		if( ac.clearOptions() ){

			m_ui.lineEditOptions->clear() ;

		}else if( ac.clearScreen() ){

			m_ui.plainTextEdit->clear() ;
			m_ui.lineEditURL->clear() ;
			m_ui.lineEditOptions->clear() ;
		}else{
			if( ac.best() ){

				m_ui.lineEditOptions->setText( ac.bestText() ) ;
			}else{
				m_ui.lineEditOptions->setText( ac.objectName() ) ;
			}

			if( m_settings.autoDownload() ){

				if( !m_ui.lineEditURL->text().isEmpty() ){

					this->download() ;
				}
			}
		}
	} ) ;
}

basicdownloader& basicdownloader::setAsActive()
{
	m_ui.tabWidget->setCurrentIndex( 0 ) ;
	return *this ;
}

void basicdownloader::retranslateUi()
{
	this->resetMenu() ;
}

void basicdownloader::checkAndPrintInstalledVersion( const QStringList& list )
{
	m_tabManager.disableAll() ;

	struct ctx
	{
		ctx( QString x ) : version( std::move( x ) )
		{
		}
		QString version ;
		QByteArray data ;
	};

	auto& backEnd = m_settings.backEnd() ;

	const auto& exeName = backEnd.name() ;

	utility::run( m_exe,{ backEnd.versionArgument() },[ this,&list,&exeName ]( QProcess& exe ){

		exe.setProcessChannelMode( QProcess::ProcessChannelMode::MergedChannels ) ;

		QString e ;

		if( !list.isEmpty() ){

			e = list.join( '\n' ) + "\n" ;
		}

		auto a = e + "[media-downloader] " ;
		auto b = a + tr( "Checking installed version of" ) + " " + exeName ;

		m_ui.plainTextEdit->setPlainText( b ) ;

		return ctx( std::move( b ) ) ;

	},[ this,&exeName ]( int exitCode,QProcess::ExitStatus exitStatus,ctx& ctx ){

		if( exitStatus == QProcess::ExitStatus::CrashExit || exitCode != 0 ){

			auto m = tr( "Failed to find version information, make sure \"%1\" is installed and works properly" ).arg( exeName )  ;

			m = "\n[media-downloader] " + m ;

			m_ui.plainTextEdit->setPlainText( ctx.version + m ) ;

			m_tabManager.disableAll() ;

			this->enableQuit() ;
		}else{
			auto c = "\n[media-downloader] " + tr( "Found version" ) + ": " + ctx.data ;

			m_ui.plainTextEdit->setPlainText( ctx.version + c ) ;

			m_tabManager.enableAll() ;
		}

	},[]( QProcess::ProcessChannel,QByteArray data,ctx& ctx ){

		ctx.data += data ;
	} ) ;
}

class context
{
public:
	context( bool a,QPlainTextEdit * b,QMetaObject::Connection c,QStringList d ) :
		m_list_requested( a ),
		m_view( b ),
		m_conn( std::move( c ) ),
		m_output( std::move( d ) )
	{
		this->postData() ;
	}
	void postData()
	{
		m_view->setPlainText( m_output.join( '\n' ) ) ;
		m_view->moveCursor( QTextCursor::End ) ;
	}
	void postData( QByteArray data )
	{
		for( const auto& m : utility::split( data ) ){

			if( m.isEmpty() ){

				continue ;

			}else if( m.startsWith( "[download]" ) && m.contains( "ETA" ) ){

				auto& s = m_output.last() ;

				if( s.startsWith( "[download]" ) && s.contains( "ETA" ) ){

					s = m ;
				}else{
					m_output.append( m ) ;
				}
			}else{
				m_output.append( m ) ;
			}
		}

		this->postData() ;
	}
	template< typename Function >
	void listRequested( Function function )
	{
		if( m_list_requested ){

			function( m_output ) ;
		}
	}
	void disconnect()
	{
		QObject::disconnect( m_conn ) ;
	}
private:
	bool m_list_requested ;
	QPlainTextEdit * m_view ;
	QMetaObject::Connection m_conn ;
	QStringList m_output ;
} ;

void basicdownloader::run( const QString& cmd,const QStringList& args,bool list_requested )
{
	m_tabManager.disableAll() ;

	utility::run( cmd,args,[ this,&list_requested,&cmd,&args ]( QProcess& exe ){

		exe.setWorkingDirectory( m_settings.downloadFolder() ) ;

		exe.setProcessChannelMode( QProcess::ProcessChannelMode::MergedChannels ) ;

		auto m = QObject::connect( m_ui.pbCancel,&QPushButton::clicked,[ &exe ](){

			exe.terminate() ;
		} ) ;

		QStringList outPut( "[media-downloader] cmd: " + [ & ](){

			auto m = "\"" + cmd + "\"" ;

			for( const auto& it : args ){

				m += " \"" + it + "\"" ;
			}

			return m + "\n" ;
		}() ) ;

		return context( list_requested,m_ui.plainTextEdit,std::move( m ),std::move( outPut ) ) ;

	},[ this ]( int,QProcess::ExitStatus,context& ctx ){

		ctx.disconnect() ;

		ctx.listRequested( [ this ]( const QStringList& e ){

			this->listRequested( e ) ;
		} ) ;

		m_tabManager.enableAll() ;

		m_ui.pbCancel->setEnabled( false ) ;

	},[]( QProcess::ProcessChannel,QByteArray data,context& ctx ){

		ctx.postData( std::move( data ) ) ;
	} ) ;
}

void basicdownloader::listRequested( const QStringList& args )
{
	QStringList opts ;

	for( int i = args.size() - 1 ; i >= 0 ; i-- ){

		auto a = utility::split( args[ i ],' ',true ) ;

		if( a.size() > 1 ){

			const auto& e = a.at( 0 ) ;

			if( e == "format" && a.at( 1 ) == "code" ){

				break ;
			}else{
				opts.insert( 0,e ) ;
			}
		}
	}

	this->resetMenu( opts ) ;
}

void basicdownloader::list()
{
	m_ui.pbCancel->setEnabled( true ) ;

	auto args = m_settings.defaultListCmdOptions() ;
	args.append( m_ui.lineEditURL->text().split( ' ' ) ) ;

	this->run( m_exe,args,true ) ;
}

void basicdownloader::download()
{
	auto m = utility::split( m_ui.lineEditURL->text(),' ',true ) ;

	this->download( m_ui.lineEditOptions->text(),m,false ) ;
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

		m_ui.lineEditOptions->setText( args.quality + " " + args.otherOptions.join( ' ' ) ) ;

		m_ui.lineEditURL->setText( urls.join( ' ' ) ) ;
	}

	m_ui.tabWidget->setCurrentIndex( 0 ) ;

	m_ui.pbCancel->setEnabled( true ) ;

	auto opts = m_settings.defaultDownLoadCmdOptions() ;

	if( args.otherOptions.contains( "--yes-playlist" ) ){

		opts.removeAll( "--no-playlist" ) ;
	}

	for( const auto& it : args.otherOptions ){

		opts.append( it ) ;
	}

	if( !args.quality.isEmpty() ){

		opts.append( m_settings.backEnd().optionsArgument() ) ;
		opts.append( args.quality ) ;
	}

	opts.append( urls ) ;

	this->run( m_exe,opts,false ) ;
}

void basicdownloader::post( const QString& e )
{
	m_ui.plainTextEdit->setPlainText( e ) ;
	m_ui.plainTextEdit->moveCursor( QTextCursor::End ) ;
}

void basicdownloader::enableQuit()
{
	m_ui.pbQuit->setEnabled( true ) ;
}

void basicdownloader::enableAll()
{
	m_ui.pbEntries->setEnabled( true ) ;
	m_ui.label_2->setEnabled( true ) ;
	m_ui.label->setEnabled( true ) ;
	m_ui.pbList->setEnabled( true ) ;
	m_ui.pbDownload->setEnabled( true ) ;
	m_ui.lineEditURL->setEnabled( true ) ;
	m_ui.lineEditOptions->setEnabled( true ) ;
	m_ui.pbQuit->setEnabled( true ) ;
}

void basicdownloader::disableAll()
{
	m_ui.pbQuit->setEnabled( false ) ;
	m_ui.pbEntries->setEnabled( false ) ;
	m_ui.label_2->setEnabled( false ) ;
	m_ui.label->setEnabled( false ) ;
	m_ui.pbList->setEnabled( false ) ;
	m_ui.pbDownload->setEnabled( false ) ;
	m_ui.lineEditURL->setEnabled( false ) ;
	m_ui.lineEditOptions->setEnabled( false ) ;
}

void basicdownloader::appQuit()
{
	QCoreApplication::quit() ;
}

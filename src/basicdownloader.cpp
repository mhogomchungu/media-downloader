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

basicdownloader::basicdownloader( Context& ctx ) :
	m_ctx( ctx ),
	m_settings( m_ctx.Settings() ),
	m_ui( m_ctx.Ui() ),
	m_tabManager( m_ctx.TabManager() )
{
	this->setAsActive() ;

	m_ui.rbYoutubeDl->setChecked( true ) ;

	m_ui.pbCancel->setEnabled( false ) ;

	m_ui.plainTextEdit->setReadOnly( true ) ;

	m_ui.labelFailedToFixExe->setVisible( false ) ;

	m_ui.rbWget->setEnabled( false ) ;
	m_ui.rbWget->setObjectName( "false" ) ;

	m_ui.rbYoutubeDl->setEnabled( false ) ;
	m_ui.rbYoutubeDl->setObjectName( "false" ) ;

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

	connect( m_ui.rbYoutubeDl,&QRadioButton::toggled,[ this ]( bool e ){

		if( e ){

			m_ctx.Engines().setDefaultEngine( "youtube-dl" ) ;
		}
	} ) ;

	connect( m_ui.rbWget,&QRadioButton::toggled,[ this ]( bool e ){

		if( e ){

			m_ctx.Engines().setDefaultEngine( "wget" ) ;
		}
	} ) ;
}

void basicdownloader::init_done()
{
	auto a = m_ui.plainTextEdit->toPlainText() ;

	if( !a.isEmpty() ){

		auto b =  utility::split( a,'\n',false ) ;

		this->printDefaultBkVersionInfo( b ) ;
	}else{
		this->printDefaultBkVersionInfo() ;
	}
}

void basicdownloader::printDefaultBkVersionInfo( const QStringList& data )
{
	if( m_counter >= m_ctx.Engines().getEngines().size() ){

		m_output.clear() ;
		return ;
	}

	const auto& engine = m_ctx.Engines().getEngines()[ m_counter ] ;

	m_counter++ ;

	const auto& exe = engine.exePath() ;

	if( engine.usingPrivateBackend() ){

		if( QFile::exists( exe ) ){

			this->checkAndPrintInstalledVersion( engine,data ) ;
		}else{
			m_ctx.TabManager().Configure().downloadYoutubeDl() ;
		}
	}else{
		if( exe.isEmpty() ){

			this->disableEngine( engine ) ;

			auto m = tr( "Failed to find version information, make sure \"%1\" is installed and works properly" ).arg( engine.name() )  ;

			m = "[media-downloader] " + m ;

			m_ui.plainTextEdit->setPlainText( m ) ;

			m_tabManager.disableAll() ;

			this->enableQuit() ;
		}else{
			this->checkAndPrintInstalledVersion( engine,data ) ;
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
			m_ui.lineEditOptions->setText( ac.objectName() ) ;

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

const QStringList& basicdownloader::currentVersionData()
{
	return m_output ;
}

void basicdownloader::disableEngine( const engines::engine& engine )
{
	const auto& m = engine.name() ;

	if( m == "youtube-dl" ){

		m_ui.rbYoutubeDl->setEnabled( false ) ;
		m_ui.rbYoutubeDl->setObjectName( "false" ) ;

	}else if( m == "wget" ){

		m_ui.rbWget->setEnabled( false ) ;
		m_ui.rbWget->setObjectName( "false" ) ;
	}
}

void basicdownloader::enableEngine( const engines::engine& engine )
{
	const auto& m = engine.name() ;

	if( m == "youtube-dl" ){

		m_ui.rbYoutubeDl->setEnabled( true ) ;
		m_ui.rbYoutubeDl->setObjectName( "true" ) ;

	}else if( m == "wget" ){

		m_ui.rbWget->setEnabled( true ) ;
		m_ui.rbWget->setObjectName( "true" ) ;
	}
}

void basicdownloader::retranslateUi()
{
	this->resetMenu() ;
}

void basicdownloader::checkAndPrintInstalledVersion( const engines::engine& engine,
						     const QStringList& list )
{
	m_tabManager.disableAll() ;

	struct ctx
	{
		ctx( const engines::engine& e ) :
			engine( e )
		{
		}
		QString version ;
		QByteArray data ;
		const engines::engine& engine ;
	};

	const auto& exe = engine.exePath() ;

	utility::run( exe,{ engine.versionArgument() },[ this,&list,&engine ]( QProcess& exe ){

		exe.setProcessChannelMode( QProcess::ProcessChannelMode::MergedChannels ) ;

		QString e ;

		if( !list.isEmpty() ){

			e = list.join( '\n' ) + "\n" ;
		}

		auto a = e + "[media-downloader] " ;
		auto b = a + tr( "Checking installed version of" ) + " " + engine.name() ;

		m_output.append( b ) ;
		m_ui.plainTextEdit->setPlainText( m_output.join( '\n' ) ) ;

		return ctx( engine ) ;

	},[ this ]( int exitCode,QProcess::ExitStatus exitStatus,ctx& ctx ){

		if( exitStatus == QProcess::ExitStatus::CrashExit || exitCode != 0 ){

			auto m = tr( "Failed to find version information, make sure \"%1\" is installed and works properly" ).arg( ctx.engine.name() )  ;

			m = "\n[media-downloader] " + m ;

			m_output.append( ctx.version + m ) ;

			this->post( m_output.join( '\n' ) ) ;

			m_tabManager.disableAll() ;

			this->disableEngine( ctx.engine ) ;

			this->enableQuit() ;
		}else{
			this->enableEngine( ctx.engine ) ;

			auto c = "[media-downloader] " + tr( "Found version" ) + ": " + ctx.engine.versionString( ctx.data ) ;

			m_output.append( ctx.version + c ) ;

			this->post( m_output.join( '\n' ) ) ;

			m_tabManager.enableAll() ;
		}

		this->printDefaultBkVersionInfo() ;

	},[]( QProcess::ProcessChannel,QByteArray data,ctx& ctx ){

		ctx.data += data ;
	} ) ;
}

class context
{
public:
	context( const engines::engine& engine,
		 bool a,
		 QPlainTextEdit * b,
		 QMetaObject::Connection c,
		 QStringList d ) :
		m_engine( engine ),
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
		m_engine.processData( m_output,data ) ;
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
	const engines::engine& m_engine ;
	bool m_list_requested ;
	QPlainTextEdit * m_view ;
	QMetaObject::Connection m_conn ;
	QStringList m_output ;
} ;

void basicdownloader::run( const engines::engine& engine,
			   const QStringList& args,
			   bool list_requested )
{
	m_tabManager.disableAll() ;

	utility::run( engine.exePath(),args,[ this,&list_requested,&engine,&args ]( QProcess& exe ){

		exe.setWorkingDirectory( m_settings.downloadFolder() ) ;

		exe.setProcessChannelMode( QProcess::ProcessChannelMode::MergedChannels ) ;

		auto m = QObject::connect( m_ui.pbCancel,&QPushButton::clicked,[ &exe ](){

			exe.terminate() ;
		} ) ;

		QStringList outPut( "[media-downloader] cmd: " + [ & ](){

			auto m = "\"" + engine.exePath() + "\"" ;

			for( const auto& it : args ){

				m += " \"" + it + "\"" ;
			}

			return m + "\n" ;
		}() ) ;

		return context( engine,
				list_requested,
				m_ui.plainTextEdit,
				std::move( m ),
				std::move( outPut ) ) ;

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

	auto url = m_ui.lineEditURL->text() ;

	const auto& backend = m_ctx.Engines().defaultEngine() ;

	auto args = backend.defaultListCmdOptions() ;
	args.append( url.split( ' ' ) ) ;

	this->run( backend,args,true ) ;
}

void basicdownloader::download()
{
	QString url = m_ui.lineEditURL->text() ;

	auto m = utility::split( url,' ',true ) ;

	const auto& engine = m_ctx.Engines().defaultEngine() ;

	this->download( engine,m_ui.lineEditOptions->text(),m,false ) ;
}

void basicdownloader::download( const engines::engine& engine,
				const utility::args& args,
				const QString& url,bool s )
{
	this->download( engine,args,QStringList( url ),s ) ;
}

void basicdownloader::download( const engines::engine& engine,
				const utility::args& args,
				const QStringList& urls,
				bool update )
{
	if( update ){

		m_ui.lineEditOptions->setText( args.quality + " " + args.otherOptions.join( ' ' ) ) ;

		m_ui.lineEditURL->setText( urls.join( ' ' ) ) ;
	}

	m_ui.tabWidget->setCurrentIndex( 0 ) ;

	m_ui.pbCancel->setEnabled( true ) ;

	auto opts = engine.defaultDownLoadCmdOptions() ;

	for( const auto& it : args.otherOptions ){

		opts.append( it ) ;
	}

	engine.updateDownLoadCmdOptions( args.quality,args.otherOptions,opts ) ;

	opts.append( urls ) ;

	this->run( engine,opts,false ) ;
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
	m_ui.rbWget->setEnabled( m_ui.rbWget->objectName() == "true" ) ;
	m_ui.rbYoutubeDl->setEnabled( m_ui.rbYoutubeDl->objectName() == "true" ) ;

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
	m_ui.rbWget->setEnabled( false ) ;
	m_ui.rbYoutubeDl->setEnabled( false ) ;
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

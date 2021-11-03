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

#include "utility.h"

#include "settings.h"
#include "context.hpp"
#include "downloadmanager.h"
#include "tableWidget.h"

#include <QEventLoop>
#include <QDesktopServices>
#include <QClipboard>
#include <QMimeData>
#include <QFileDialog>

const char * utility::selectedAction::CLEAROPTIONS = "Clear Options" ;
const char * utility::selectedAction::CLEARSCREEN  = "Clear Screen" ;
const char * utility::selectedAction::OPENFOLDER   = "Open Download Folder" ;

#ifdef Q_OS_LINUX

bool utility::platformIsLinux()
{
	return true ;
}

bool utility::platformIsOSX()
{
	return false ;
}

bool utility::platformIsWindows()
{
	return false ;
}

QString utility::python3Path()
{
	return QStandardPaths::findExecutable( "python3" ) ;
}

bool utility::platformIs32BitWindows()
{
	return false ;
}

util::result< int > utility::Terminator::terminate( int,char ** )
{
	return {} ;
}

#endif

#ifdef Q_OS_MACOS

QString utility::python3Path()
{
	return QStandardPaths::findExecutable( "python3" ) ;
}

bool utility::platformIsOSX()
{
	return true ;
}

bool utility::platformIsLinux()
{
	return false ;
}

bool utility::platformIsWindows()
{
	return false ;
}

util::result< int > utility::Terminator::terminate( int,char ** )
{
	return {} ;
}

bool utility::platformIs32BitWindows()
{
	return false ;
}

#endif

#ifdef Q_OS_WIN

#include <windows.h>

#include <cstring>
#include <cstdlib>

template< typename Function,typename Deleter,typename ... Arguments >
auto unique_rsc( Function&& function,Deleter&& deleter,Arguments&& ... args )
{
	using A = std::remove_pointer_t< std::result_of_t< Function( Arguments&& ... ) > > ;
	using B = std::decay_t< Deleter > ;

	return std::unique_ptr< A,B >( function( std::forward< Arguments >( args ) ... ),
				       std::forward< Deleter >( deleter ) ) ;
}

template< typename Type,typename Deleter >
auto unique_ptr( Type type,Deleter&& deleter )
{
	return unique_rsc( []( auto arg ){ return arg ; },
			   std::forward< Deleter >( deleter ),type ) ;
}

static int _terminateWindowApp( unsigned long pid )
{
	FreeConsole() ;

	if( AttachConsole( pid ) == TRUE ) {

		SetConsoleCtrlHandler( nullptr,true ) ;

		if( GenerateConsoleCtrlEvent( CTRL_C_EVENT,0 ) == TRUE ){

			return 0 ;
		}
	}

	return 1 ;
}

util::result< int > utility::Terminator::terminate( int argc,char ** argv )
{
	if( argc > 2 && std::strcmp( argv[ 1 ],"-T" ) == 0 ){

		return _terminateWindowApp( std::strtoul( argv[ 2 ],nullptr,10 ) ) ;
	}else{
		return {} ;
	}
}

static HKEY _reg_open_key( const char * subKey,HKEY hkey )
{
	HKEY m ;
	REGSAM wow64 = KEY_QUERY_VALUE | KEY_WOW64_64KEY ;
	REGSAM wow32 = KEY_QUERY_VALUE | KEY_WOW64_32KEY ;
	unsigned long x = 0 ;

	if( RegOpenKeyExA( hkey,subKey,x,wow64,&m ) == ERROR_SUCCESS ){

		return m ;

	}else if( RegOpenKeyExA( hkey,subKey,x,wow32,&m ) == ERROR_SUCCESS ){

		return m ;
	}else{
		return nullptr ;
	}
}

static void _reg_close_key( HKEY hkey )
{
	if( hkey != nullptr ){

		RegCloseKey( hkey ) ;
	}
}

static QByteArray _reg_get_value( HKEY hkey,const char * key )
{
	if( hkey != nullptr ){

		DWORD dwType = REG_SZ ;

		std::array< char,4096 > buffer ;

		std::fill( buffer.begin(),buffer.end(),'\0' ) ;

		auto e = reinterpret_cast< BYTE * >( buffer.data() ) ;
		auto m = static_cast< DWORD >( buffer.size() ) ;

		if( RegQueryValueEx( hkey,key,nullptr,&dwType,e,&m ) == ERROR_SUCCESS ){

			return { buffer.data(),static_cast< int >( m ) } ;
		}
	}

	return {} ;
}

static QString _readRegistry( const char * subKey,const char * key,HKEY hkey )
{
	auto s = unique_rsc( _reg_open_key,_reg_close_key,subKey,hkey ) ;

	return _reg_get_value( s.get(),key ) ;
}

QString utility::python3Path()
{
	std::array< HKEY,2 > hkeys{ HKEY_CURRENT_USER,HKEY_LOCAL_MACHINE } ;

	std::string path = "Software\\Python\\PythonCore\\3.X\\InstallPath" ;

	char * str = &path[ 0 ] ;

	for( const auto& it : hkeys ){

		for( char s = '9' ; s >= '0' ; s-- ){

			str[ 29 ] = s ;

			auto c = _readRegistry( str,"ExecutablePath",it ) ;

			if( !c.isEmpty() ){

				return c ;
			}
		}
	}

	return {} ;
}

#include <QSysInfo>

bool utility::platformIs32BitWindows()
{
	return QSysInfo::currentCpuArchitecture() != "x86_64" ;
}

bool utility::platformIsWindows()
{
	return true ;
}

bool utility::platformIsLinux()
{
	return false ;
}

bool utility::platformIsOSX()
{
	return false ;
}

#endif

utility::debug& utility::debug::operator<<( const QString& e )
{
	return _print( e.toStdString().c_str() ) ;
}

utility::debug& utility::debug::operator<<( const QStringList& e )
{
	if( e.isEmpty() ){

		return _print( "()" ) ;
	}else{
		QString m = "(\"" + e.at( 0 ) + "\"" ;

		for( int s = 1 ; s < e.size() ; s++ ){

			m += ", \"" + e.at( s ) + "\"" ;
		}

		m += ")";

		return _print( m.toStdString().c_str() ) ;
	}
}

utility::debug& utility::debug::operator<<( const QByteArray& e )
{
	return _print( e.data() ) ;
}

bool utility::Terminator::terminate( QProcess& exe )
{
	if( utility::platformIsWindows() ){

		if( exe.state() == QProcess::ProcessState::Running ){

			//QStringList args{ "-T",QString::number( exe.processId() ) } ;

			//QProcess::startDetached( "media-downloader.exe",args ) ;

			QStringList args{ "/F","/T","/PID",QString::number( exe.processId() ) } ;

			QProcess::startDetached( "taskkill",args ) ;
		}
	}else{
		exe.terminate() ;
	}

	return true ;
}

bool utility::platformIsNOTWindows()
{
	return !utility::platformIsWindows() ;
}

QMenu * utility::setUpMenu( const Context& ctx,
			    const QStringList&,
			    bool addClear,
			    bool addOpenFolder,
			    bool combineText,
			    QWidget * parent )
{
	auto menu = new QMenu( parent ) ;

	auto& translator = ctx.Translator() ;
	auto& settings = ctx.Settings() ;

	translator::entry ss( QObject::tr( "Preset Options" ),"Preset Options","Preset Options" ) ;
	auto ac = translator.addAction( menu,std::move( ss ) ) ;

	ac->setEnabled( false ) ;

	menu->addSeparator() ;

	settings.presetOptions( [ & ]( const QString& uiName,const QString& options ){

		auto a = uiName ;

		a.replace( "Best-audiovideo",QObject::tr( "Best-audiovideo" ) ) ;
		a.replace( "Best-audio",QObject::tr( "Best-audio" ) ) ;

		if( combineText ){

			menu->addAction( a )->setObjectName( options + "\n" + a ) ;
		}else{
			menu->addAction( a )->setObjectName( options ) ;
		}
	} ) ;

	if( addClear ){

		menu->addSeparator() ;

		translator::entry sx( QObject::tr( "Clear" ),
						   utility::selectedAction::CLEARSCREEN,
						   utility::selectedAction::CLEARSCREEN ) ;

		translator.addAction( menu,std::move( sx ) ) ;
	}

	if( addOpenFolder ){

		menu->addSeparator() ;

		translator::entry mm( QObject::tr( "Open Download Folder" ),
						   utility::selectedAction::OPENFOLDER,
						   utility::selectedAction::OPENFOLDER ) ;

		translator.addAction( menu,std::move( mm ) ) ;
	}

	return menu ;
}

bool utility::hasDigitsOnly( const QString& e )
{
	for( const auto& it : e ){

		if( !( it >= '0' && it <= '9'  ) ){

			return false ;
		}
	}

	return true ;
}

QString utility::homePath()
{
	if( utility::platformIsWindows() ){

		return QDir::homePath() + "/Desktop" ;
	}else{
		return QDir::homePath() ;
	}
}

void utility::waitForOneSecond()
{
	utility::wait( 1000 ) ;
}

void utility::wait( int time )
{
	QEventLoop e ;

	util::Timer( time,[ & ](){ e.exit() ;} ) ;

	e.exec() ;
}

void utility::openDownloadFolderPath( const QString& url )
{
	if( utility::platformIsWindows() ){

		QProcess::startDetached( "explorer.exe",{ QDir::toNativeSeparators( url ) } ) ;
	}else{
		QDesktopServices::openUrl( url ) ;
	}
}

QStringList utility::updateOptions( const updateOptionsStruct& s )
{
	const engines::engine& engine  = s.engine ;
	const engines::enginePaths& ep = s.enginePaths ;
	settings& settings             = s.stts ;
	const utility::args& args      = s.args ;
	const QString& indexAsString   = s.indexAsString ;
	const QStringList& urls        = s.urls ;
	bool forceDownload             = s.forceDownload ;
	const QString& downloadPath    = settings.downloadFolder() ;

	auto opts = [ & ](){

		auto m = settings.engineDefaultDownloadOptions( engine.name() ) ;

		if( m.isEmpty() ){

			return engine.defaultDownLoadCmdOptions() ;
		}else{
			return util::splitPreserveQuotes( m ) ;
		}
	}() ;

	for( const auto& it : args.otherOptions() ){

		opts.append( it ) ;
	}

	auto url = urls ;

	engine.updateDownLoadCmdOptions( { args.quality(),args.otherOptions(),indexAsString,url,opts } ) ;

	opts.append( url ) ;

	const auto& ca = engine.cookieArgument() ;
	const auto& cv = settings.cookieFilePath( engine.name() ) ;

	if( !ca.isEmpty() && !cv.isEmpty() ){

		opts.append( ca ) ;
		opts.append( cv ) ;
	}

	for( auto& it : opts ){

		it.replace( utility::stringConstants::mediaDownloaderDataPath(),ep.dataPath() ) ;
		it.replace( utility::stringConstants::mediaDownloaderDefaultDownloadPath(),downloadPath ) ;
		it.replace( utility::stringConstants::mediaDownloaderCWD(),QDir::currentPath() ) ;
	}

	if( forceDownload ){

		utility::arguments( opts ).removeOptionWithArgument( "--download-archive" ) ;
	}

	return opts ;
}

int utility::concurrentID()
{
	static int id = -1 ;

	id++ ;

	return id ;
}

QString utility::failedToFindExecutableString( const QString& cmd )
{
	return QObject::tr( "Failed to find executable \"%1\"" ).arg( cmd ) ;
}

QString utility::clipboardText()
{
	auto m = QApplication::clipboard() ;
	auto e = m->mimeData() ;

	if( e->hasText() ){

		return e->text() ;
	}else{
		return {} ;
	}
}

QString utility::downloadFolder( const Context& ctx )
{
	return ctx.Settings().downloadFolder() ;
}

const QProcessEnvironment& utility::processEnvironment( const Context& ctx )
{
	return ctx.Engines().processEnvironment() ;
}

void utility::saveDownloadList( const Context& ctx,QMenu& m,tableWidget& tableWidget )
{
	QObject::connect( m.addAction( QObject::tr( "Save List To File" ) ),&QAction::triggered,[ &ctx,&tableWidget ](){

		auto e = QFileDialog::getSaveFileName( &ctx.mainWidget(),
						       QObject::tr( "Save List To File" ),
						       utility::homePath() + "/MediaDowloaderList.txt" ) ;

		if( !e.isEmpty() ){

			QJsonArray arr ;

			tableWidget.forEach( [ & ]( const tableWidget::entry& e ){

				if( !downloadManager::finishedStatus::finishedWithSuccess( e.runningState ) ){

					arr.append( [ & ](){

						QJsonObject obj ;

						obj.insert( "url",e.url ) ;
						obj.insert( "uiText",e.uiText ) ;
						//QString img = tableWidget::thumbnailData( e.thumbnail.image ) ;
						//obj.insert( "thumbnail",img ) ;

						return obj ;
					}() ) ;
				}
			} ) ;

			auto stuff = QJsonDocument( arr ).toJson( QJsonDocument::Indented ) ;

			engines::file( e,ctx.logger() ).write( stuff ) ;
		}
	} ) ;
}

bool utility::isRelativePath( const QString& e )
{
	return QDir::isRelativePath( e ) ;
}

utility::MediaEntry::MediaEntry( const QByteArray& data ) : m_json( data )
{
	if( m_json ){

		auto object = m_json.doc().object() ;

		m_title        = object.value( "title" ).toString() ;
		m_url          = object.value( "webpage_url" ).toString() ;
		m_uploadDate   = object.value( "upload_date" ).toString() ;
		m_id           = object.value( "id" ).toString() ;
		m_thumbnailUrl = object.value( "thumbnail" ).toString() ;

		if( !m_uploadDate.isEmpty() ){

			m_uploadDate = QObject::tr( "Upload Date:" ) + " " + m_uploadDate ;
		}

		m_intDuration = object.value( "duration" ).toInt() ;

		if( m_intDuration != 0 ){

			auto s = engines::engine::functions::timer::duration( m_intDuration * 1000 ) ;
			m_duration = QObject::tr( "Duration:" ) + " " + s ;
		}
	}
}

QString utility::MediaEntry::uiText() const
{
	auto title = [ & ](){

		if( m_title.isEmpty() || m_title == "\n" ){

			return m_url ;
		}else{
			return m_title ;
		}
	}() ;

	if( m_duration.isEmpty() ){

		if( m_uploadDate.isEmpty() ){

			return title ;
		}else{
			return m_uploadDate + "\n" + title ;
		}
	}else{
		if( m_uploadDate.isEmpty() ){

			return m_duration + "\n" + title ;
		}else{
			return m_duration + ", " + m_uploadDate + "\n" + title ;
		}
	}
}

const engines::engine& utility::resolveEngine( const QString& uiText,
					       const engines::engine& engine,
					       const engines& engines )
{
	auto u = tableWidget::engineName() ;

	if( uiText.contains( u ) ){

		const auto mm = util::split( uiText,'\n',true ) ;

		for( const auto& it : mm ){

			if( it.startsWith( u ) ){

				auto s = it.mid( u.size() ) ;

				const auto& ee = engines.getEngineByName( s ) ;

				if( ee.has_value() ){

					return ee.value() ;
				}
			}
		}
	}

	return engine ;
}

QString utility::locale::formattedDataSize( qint64 s ) const
{
#if QT_VERSION >= QT_VERSION_CHECK( 5,14,0 )
	return m_locale.formattedDataSize( s ) ;
#else
	std::array< const char *,7 > sizes = { "EiB", "PiB", "TiB", "GiB", "MiB", "KiB", "B" } ;

	qint64  multiplier = 1024ULL * 1024ULL * 1024ULL * 1024ULL * 1024ULL * 1024ULL ;

	QString result ;

	for( size_t i = 0 ; i < sizes.size() ; i++,multiplier /= 1024 ){

		if( s < multiplier ){

			continue ;
		}

		if( s % multiplier == 0 ){

			auto a = QString::number( s / multiplier ) ;
			auto b = sizes[ i ] ;

			result = QString( "%1 %2" ).arg( a,b ) ;
		}else{
			auto a = static_cast< double >( s ) / static_cast< double >( multiplier ) ;
			auto b = sizes[ i ] ;
			auto c = QString::number( a,'f',2 ) ;

			result = QString( "%1 %2" ).arg( c,b ) ;
		}

	    return result ;
	}

	return {} ;
#endif
}

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
#include "tabmanager.h"
#include "version.h"

#include <QEventLoop>
#include <QDesktopServices>
#include <QClipboard>
#include <QMimeData>
#include <QFileDialog>
#include <QSysInfo>

#include <ctime>
#include <cstring>

const char * utility::selectedAction::CLEAROPTIONS = "Clear Options" ;
const char * utility::selectedAction::CLEARSCREEN  = "Clear Screen" ;
const char * utility::selectedAction::OPENFOLDER   = "Open Download Folder" ;

#if defined(__OS2__) || defined(OS2) || defined(_OS2)

bool utility::platformisOS2()
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

bool utility::platformIsWindows()
{
	return false ;
}

QString utility::python3Path()
{
	return QStandardPaths::findExecutable( "python3" ) ;
}

util::result< int > utility::Terminator::terminate( int,char ** )
{
	return {} ;
}

#endif

#ifdef Q_OS_LINUX

bool utility::platformisOS2()
{
	return false ;
}

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

util::result< int > utility::Terminator::terminate( int,char ** )
{
	return {} ;
}

#endif

#ifdef Q_OS_MACOS

bool utility::platformisOS2()
{
	return false ;
}

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

#endif

#ifdef Q_OS_WIN

#include <libloaderapi.h>
#include <array>

QString utility::windowsApplicationDirPath()
{
	std::array< char,4096 > buffer ;

	GetModuleFileNameA( nullptr,buffer.data(),static_cast< DWORD >( buffer.size() ) ) ;

	auto m = QDir::fromNativeSeparators( buffer.data() ) ;
	auto s = m.lastIndexOf( '/' ) ;

	if( s != -1 ){

		m.truncate( s ) ;
	}

	return m ;
}

#else

QString utility::windowsApplicationDirPath()
{
	return {} ;
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

static HKEY _reg_open_key( const char * subKey,HKEY hkey,REGSAM sam )
{
	HKEY m ;
	REGSAM wow64 = KEY_WOW64_64KEY | sam ;
	REGSAM wow32 = KEY_WOW64_32KEY | sam ;

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

		// Docs says i should not close predefined keys
		//RegCloseKey( hkey ) ;
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

static QString _readRegistry( const char * subKey,const char * key,HKEY hkey,REGSAM sam )
{
	auto s = unique_rsc( _reg_open_key,_reg_close_key,subKey,hkey,sam ) ;

	return _reg_get_value( s.get(),key ) ;
}

static void _python3Paths( QStringList& list,HKEY hkey )
{
	auto s = unique_rsc( _reg_open_key,
			     _reg_close_key,
			     "SOFTWARE\\Python\\PythonCore",
			     hkey,
			     KEY_ENUMERATE_SUB_KEYS ) ;

	auto key = s.get() ;

	if( key != nullptr ){

		std::array< char,20000 > buffer ;
		DWORD size ;

		for( int s = 0 ; ; s++ ){

			size = static_cast< DWORD >( buffer.size() ) ;

			auto m = RegEnumKeyExA( key,s,buffer.data(),&size,nullptr,nullptr,nullptr,nullptr ) ;

			if( m == ERROR_SUCCESS ){

				auto mm =  "SOFTWARE\\Python\\PythonCore\\" + QByteArray( buffer.data() ) + "\\InstallPath" ;

				auto eee = _readRegistry( mm.data(),"ExecutablePath",hkey,KEY_QUERY_VALUE ) ;

				if( !eee.isEmpty() ){

					list.append( eee ) ;
				}
			}else{
				break ;
			}
		}
	}
}

QString utility::python3Path()
{
	std::array< HKEY,2 > hkeys{ HKEY_CURRENT_USER,HKEY_LOCAL_MACHINE } ;
	QStringList list ;

	for( const auto& it : hkeys ){

		_python3Paths( list,it ) ;
	}

	if( list.isEmpty() ){

		return {} ;
	}else{
		list.sort( Qt::CaseInsensitive ) ;

		return list.last() ;
	}
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

bool utility::platformisOS2()
{
	return false ;
}

#endif

utility::debug& utility::debug::operator<<( const QString& e )
{
	return _print( e.toStdString().c_str() ) ;
}

utility::debug& utility::debug::operator<<( const QList<QByteArray>& e )
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
	auto& configure = ctx.TabManager().Configure() ;

	translator::entry ss( QObject::tr( "Preset Options" ),"Preset Options","Preset Options" ) ;
	auto ac = translator.addAction( menu,std::move( ss ) ) ;

	ac->setEnabled( false ) ;

	menu->addSeparator() ;

	configure.presetOptionsForEach( [ & ]( const QString& uiName,const QString& options ){

		auto a = uiName ;

		a.replace( "Best-audiovideo",QObject::tr( "Best-audiovideo" ) ) ;
		a.replace( "Best-audio",QObject::tr( "Best-audio" ) ) ;
		a.replace( "Default",QObject::tr( "Default" ) ) ;

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
	const tableWidget::entry& ent  = s.tableEntry ;
	const engines::engine& engine  = s.engine ;
	const engines::enginePaths& ep = s.ctx.Engines().engineDirPaths() ;
	settings& settings             = s.stts ;
	const utility::args& args      = s.args ;
	const QString& indexAsString   = s.indexAsString ;
	const QStringList& urls        = s.urls ;
	bool forceDownload             = s.forceDownload ;
	const QString& downloadPath    = settings.downloadFolder() ;

	auto opts = [ & ](){

		auto m = s.ctx.TabManager().Configure().engineDefaultDownloadOptions( engine.name() ) ;

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

	engine.updateDownLoadCmdOptions( { args.quality(),
					   args.otherOptions(),
					   indexAsString,
					   ent.playlist,
					   ent.playlist_count,
					   ent.playlist_id,
					   ent.playlist_title,
					   ent.playlist_uploader,
					   ent.playlist_uploader_id,
					   ent.n_entries,
					   url,
					   opts } ) ;

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

void utility::initDone()
{
}

int utility::sequentialID()
{
	static int id = 0 ;

	--id ;

	return id ;
}

int utility::concurrentID()
{
	static int id = -1 ;

	++id ;

	return id ;
}

QString utility::failedToFindExecutableString( const QString& cmd )
{
	return QObject::tr( "Failed to find executable \"%1\"" ).arg( cmd ) ;
}

QString utility::clipboardText()
{
	auto m = QApplication::clipboard() ;

	if( m ){

		auto e = m->mimeData() ;

		if( e->hasText() ){

			return e->text() ;
		}
	}

	return {} ;
}

QString utility::downloadFolder( const Context& ctx )
{
	return ctx.Settings().downloadFolder() ;
}

static QJsonArray _saveDownloadList( tableWidget& tableWidget,bool noFinishedSuccess )
{
	QJsonArray arr ;

	auto downloadOpts = utility::stringConstants::downloadOptions() ;
	auto engineName = utility::stringConstants::engineName() ;

	auto _add = [ &downloadOpts,&engineName,&arr ]( const tableWidget::entry& e ){

		if( e.url.isEmpty() ){

			return ;
		}

		auto obj = e.uiJson ;

		obj.insert( "runningState",e.runningState ) ;

		const auto& m = e.uiText ;

		if( m.startsWith( downloadOpts ) || m.startsWith( engineName ) ){

			const auto m = util::split( e.uiText,'\n',true ) ;

			for( const auto& it : m ){

				if( it.startsWith( downloadOpts ) ){

					auto m = it.indexOf( ':' ) ;

					obj.insert( "downloadOptions",it.mid( m + 2 ) ) ;

				}else if( it.startsWith( engineName ) ){

					auto m = it.indexOf( ':' ) ;

					obj.insert( "engineName",it.mid( m + 2 ) ) ;
				}
			}
		}

		arr.append( obj ) ;
	} ;

	if( noFinishedSuccess ){

		tableWidget.forEach( [ & ]( const tableWidget::entry& e ){

			using gg = downloadManager::finishedStatus ;

			if( !gg::finishedWithSuccess( e.runningState ) ){

				_add( e ) ;
			}
		} ) ;
	}else{
		tableWidget.forEach( [ & ]( const tableWidget::entry& e ){

			_add( e ) ;
		} ) ;
	}

	return arr ;
}

void utility::saveDownloadList( const Context& ctx,tableWidget& tableWidget,bool pld )
{
	if( ctx.Settings().autoSavePlaylistOnExit() ){

		if( pld ){

			if( tableWidget.rowCount() == 1 ){

				return ;
			}
		}else{
			if( tableWidget.rowCount() == 0 ){

				return ;
			}
		}

		auto arr = _saveDownloadList( tableWidget,true ) ;

		auto e = ctx.Engines().engineDirPaths().dataPath( "autoSavedList.json" ) ;

		if( QFile::exists( e ) ){

			auto m = engines::file( e,ctx.logger() ).readAll() ;

			QFile::remove( e ) ;

			const auto rr = QJsonDocument::fromJson( m ).array() ;

			for( const auto& it : rr ){

				arr.append( it ) ;
			}
		}

		auto m = QJsonDocument( arr ).toJson( QJsonDocument::Indented ) ;

		engines::file( e,ctx.logger() ).write( m ) ;
	}
}

void utility::saveDownloadList( const Context& ctx,QMenu& m,tableWidget& tableWidget,bool pld )
{
	QObject::connect( m.addAction( QObject::tr( "Save List To File" ) ),&QAction::triggered,[ &ctx,&tableWidget,pld ](){

		QString filePath ;

		if( pld && tableWidget.rowCount() > 1 ){

			auto uploader = tableWidget.entryAt( 1 ).uiJson.value( "uploader" ).toString() ;

			if( uploader.isEmpty() ){

				filePath = utility::homePath() + "/MediaDowloaderList-" + uploader + ".json" ;
			}else{
				filePath = utility::homePath() + "/MediaDowloaderList.json" ;
			}
		}else{
			filePath = utility::homePath() + "/MediaDowloaderList.json" ;
		}

		auto s = QFileDialog::getSaveFileName( &ctx.mainWidget(),
						       QObject::tr( "Save List To File" ),
						       filePath ) ;
		if( !s.isEmpty() ){

			auto e = _saveDownloadList( tableWidget,false ) ;

			auto m = QJsonDocument( e ).toJson( QJsonDocument::Indented ) ;

			engines::file( s,ctx.logger() ).write( m ) ;
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
		m_formats      = object.value( "formats" ).toArray() ;
		m_uploader     = object.value( "uploader" ).toString() ;

		m_playlist             = object.value( "playlist" ).toString() ;
		m_playlist_id          = object.value( "playlist_id" ).toString() ;
		m_playlist_title       = object.value( "playlist_title" ).toString() ;
		m_playlist_uploader    = object.value( "playlist_uploader" ).toString() ;
		m_playlist_uploader_id = object.value( "playlist_uploader_id" ).toString() ;

		m_n_entries            = QString::number( object.value( "n_entries" ).toInt() ) ;
		m_playlist_count       = QString::number( object.value( "playlist_count" ).toInt() ) ;

		if( !m_uploadDate.isEmpty() ){

			m_uploadDate = utility::stringConstants::uploadDate() + " " + m_uploadDate ;
		}

		m_intDuration = object.value( "duration" ).toInt() ;

		if( m_intDuration != 0 ){

			auto s = engines::engine::functions::timer::duration( m_intDuration * 1000 ) ;
			m_duration = utility::stringConstants::duration() + " " + s ;
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

QJsonObject utility::MediaEntry::uiJson() const
{
	QJsonObject obj ;

	auto u = QString( m_uploadDate ).replace( utility::stringConstants::uploadDate() + " ","" ) ;
	auto d = QString( m_duration ).replace( utility::stringConstants::duration() + " ","" ) ;

	obj.insert( "title",m_title ) ;
	obj.insert( "url",m_url ) ;
	obj.insert( "duration",d ) ;
	obj.insert( "uploadDate",u ) ;
	obj.insert( "uploader",m_uploader ) ;

	return obj ;
}

const engines::engine& utility::resolveEngine( const tableWidget& table,
					       const engines::engine& engine,
					       const engines& engines,
					       int row )
{
	const auto& engineName = table.engineName( row ) ;

	if( engineName.isEmpty() ){

		return engine ;
	}else{
		const auto& ee = engines.getEngineByName( engineName ) ;

		if( ee.has_value() ){

			return ee.value() ;
		}else{
			return engine ;
		}
	}
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

void utility::versionInfo::check( const engines::Iterator& iter,const QString& setDefaultEngine )
{
	if( iter.engine().name() =="media-downloader" ){

		return this->printEngineVersionInfo( iter ) ;
	}

	const auto& engine = iter.engine() ;

	if( engine.usingPrivateBackend() && engine.validDownloadUrl() && networkAccess::hasNetworkSupport() ){

		if( engine.backendExists() ){

			this->printEngineVersionInfo( iter ) ;

			utility::setDefaultEngine( *m_ctx,setDefaultEngine ) ;

		}else if( !engine.exePath().realExe().isEmpty() ){

			m_networkAccess->download( iter,setDefaultEngine ) ;
		}
	}else{
		if( engine.exePath().isEmpty() ){

			m_ctx->logger().add( QObject::tr( "Failed to find version information, make sure \"%1\" is installed and works properly" ).arg( engine.name() ),iter.id() ) ;
		}else{
			this->printEngineVersionInfo( iter ) ;

			utility::setDefaultEngine( *m_ctx,setDefaultEngine ) ;
		}
	}
}

utility::versionInfo::~versionInfo()
{
}

void utility::versionInfo::updateMediaDownloader( const engines::Iterator& iter )
{
	m_networkAccess->download( iter,{} ) ;
}

void utility::versionInfo::printEngineVersionInfo( const engines::Iterator& iter )
{
	const auto& engine = iter.engine() ;

	m_ctx->TabManager().disableAll() ;

	engines::engine::exeArgs::cmd cmd( engine.exePath(),{ engine.versionArgument() } ) ;

	auto id = utility::sequentialID() ;

	m_ctx->logger().add( QObject::tr( "Checking installed version of" ) + " " + engine.name(),id ) ;

	if( !m_ctx->debug().isEmpty() ){

		auto exe = "cmd: \"" + cmd.exe() + "\"" ;

		for( const auto& it : cmd.args() ){

			exe += " \"" + it + "\"" ;
		}

		m_ctx->logger().add( exe,id ) ;
	}

	utils::qprocess::run( cmd.exe(),cmd.args(),[ iter,this,id ]( const utils::qprocess::outPut& r ){

		const auto& engine = iter.engine() ;

		if( r.success() ){

			auto& logger = m_ctx->logger() ;

			logger.add( QObject::tr( "Found version" ) + ": " + engine.setVersionString( r.stdOut ),id ) ;

			m_ctx->TabManager().enableAll() ;
		}else{
			m_ctx->logger().add( QObject::tr( "Failed to find version information, make sure \"%1\" is installed and works properly" ).arg( engine.name() ),id ) ;

			m_ctx->TabManager().enableAll() ;

			engine.setBroken() ;
		}

		if( iter.hasNext() ){

			this->check( iter.next() ) ;
		}else{
			emit vinfoDone() ;
		}

	},QProcess::ProcessChannelMode::MergedChannels ) ;
}

bool utility::platformIs32Bit()
{
#if QT_VERSION >= QT_VERSION_CHECK( 5,4,0 )
	return QSysInfo::currentCpuArchitecture() != "x86_64" ;
#else
	//?????
	return false ;
#endif
}

void utility::addJsonCmd::add( const utility::addJsonCmd::entry& e )
{
	m_obj.insert( e.platform,[ & ]{

		QJsonObject s ;

		for( const auto& it : e.platformData ){

			s.insert( it.archName,[ & ](){

				QJsonObject a ;

				a.insert( "Name",it.exeName ) ;

				a.insert( "Args",[ & ](){

					QJsonArray arr ;

					for( const auto& xt : it.exeArgs ){

						arr.append( xt ) ;
					}

					return arr ;
				}() ) ;

				return a ;
			}() ) ;
		}

		return s ;

	}() ) ;
}


QString utility::fromSecsSinceEpoch( qint64 s )
{
	std::time_t epoch = static_cast< std::time_t >( s ) ;
	return QString( std::asctime( std::gmtime( &epoch ) ) ).trimmed() ;
}

QString utility::setDownloadOptions( const engines::engine& engine,
				     tableWidget& table,
				     int row,
				     const QString& downloadOpts )
{
	auto u = table.downloadingOptions( row ) ;

	auto m = table.subTitle( row ) ;

	if( !m.isEmpty() ){

		auto s = util::split( m,' ',true ) ;

		auto e = engine.defaultSubtitleDownloadOptions().join( " " ) ;

		if( s.at( 0 ) == "ac:" ){

			m = " " + e + " --write-auto-subs --sub-langs " + s.at( 1 ) ;
		}else{
			m = " " + e + " --sub-langs " + s.at( 1 ) ;
		}
	}

	if( u.isEmpty() ){

		if( downloadOpts.isEmpty() ){

			return "Default" + m ;
		}else{
			return downloadOpts + m ;
		}
	}else{
		return u + m ;
	}
}

void utility::setDefaultEngine( const Context& ctx,const QString& name )
{
	if( !name.isEmpty() ){

		ctx.Engines().setDefaultEngine( name ) ;

		ctx.TabManager().setDefaultEngines() ;
	}
}

bool utility::onlyWantedVersionInfo( int argc,char ** argv )
{
	for( int s = 0 ; s < argc ; s++ ){

		if( std::strcmp( argv[ s ],"--version" ) == 0 ){

			std::cout << util::split( VERSION,'\n' ).at( 0 ).constData() << std::endl ;

			return true ;
		}
	}

	return false ;
}

bool utility::startedUpdatedVersion( settings& s,int argc,char ** argv )
{
	auto m = s.updatedVersionPath() ;

	auto exePath = [ & ](){

		if(utility::platformIsLikeWindows() ){

			return m + "/bin/media-downloader.exe" ;
		}else{
			return m + "/bin/media-downloader" ;
		}
	}() ;

	if( QFile::exists( exePath ) ){

		QStringList args ;

		for( int i = 1 ; i < argc ; i++ ){

			args.append( *( argv + i ) ) ;
		}

		if( utility::platformIsWindows() ){

			auto env = QProcessEnvironment::systemEnvironment() ;
			auto paths = env.value( "PATH" ) ;
			env.insert( "PATH",QDir::currentPath() + ";" + paths ) ;

			QProcess exe ;

			exe.setProgram( exePath ) ;
			exe.setArguments( args ) ;
			exe.setProcessEnvironment( env ) ;

			//exe.startDetached() ;
		}else{
			QProcess::startDetached( exePath,args ) ;
		}

		return true ;
	}else{
		return false ;
	}
}

bool utility::platformIsLikeWindows()
{
	return utility::platformIsWindows() || utility::platformisOS2() ;
}

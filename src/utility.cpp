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

#include "flatpak.h"

#include "settings.h"
#include "context.hpp"
#include "downloadmanager.hpp"
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

#endif

#ifdef Q_OS_MACOS

bool utility::platformisOS2()
{
	return false ;
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

#endif

#ifdef Q_OS_WIN

#include <windows.h>
#include <iphlpapi.h>
#include <libloaderapi.h>
#include <winuser.h>
#include <winbase.h>
#include <dwmapi.h>
#include <winreg.h>

#include <array>
#include <cstring>

#include <QOperatingSystemVersion>

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

class adaptorInfo
{
public:
	adaptorInfo()
	{
		auto m = this->requiredSize() ;

		if( m ){

			auto e = HeapAlloc( GetProcessHeap(),0,m ) ;

			auto s = static_cast< PIP_ADAPTER_INFO >( e ) ;

			if( GetAdaptersInfo( s,&m ) == NO_ERROR ){

				m_handle = s ;
			}else{
				this->free( s ) ;
			}
		}
	}
	QString address()
	{
		if( m_handle ){

			for( auto it = m_handle ; it != nullptr ; it = it->Next ){

				auto gateway = it->GatewayList.IpAddress.String ;
				auto address = it->IpAddressList.IpAddress.String ;

				if( std::strcmp( address,"0.0.0.0" ) ){

					if( std::strcmp( gateway,"0.0.0.0" ) ){

						return gateway ;
					}
				}
			}
		}

		return {} ;
	}
	~adaptorInfo()
	{
		this->free( m_handle ) ;
	}
private:
	void free( PIP_ADAPTER_INFO s )
	{
		HeapFree( GetProcessHeap(),0,s ) ;
	}
	ULONG requiredSize()
	{
		ULONG m = 0 ;

		if( GetAdaptersInfo( nullptr,&m ) == ERROR_BUFFER_OVERFLOW ){

			return m ;
		}else{
			return 0 ;
		}
	}

	PIP_ADAPTER_INFO m_handle = nullptr ;
} ;

QString utility::windowsGateWayAddress()
{
	return adaptorInfo().address() ;
}

QString utility::windowsGetClipBoardText( const ContextWinId& wId )
{
	class String
	{
	public:
		void operator=( const char * s )
		{
			m_value = s ;
		}
		void operator=( const wchar_t * s )
		{
			m_value = QString::fromWCharArray( s ) ;
		}
		operator QString()
		{
			return m_value ;
		}
	private:
		QString m_value ;
	} ;

	String s ;

	if( IsClipboardFormatAvailable( CF_TEXT ) ){

		if( OpenClipboard( wId.value() ) ){

			auto hglb = GetClipboardData( CF_TEXT ) ;

			if( hglb ){

				auto lptstr = static_cast< LPTSTR >( GlobalLock( hglb ) ) ;

				if( lptstr ){

					s = lptstr ;

					GlobalUnlock( hglb ) ;
				}
			}

			CloseClipboard() ;
		}
	}

	return s ;
}

void utility::windowsSetDarkModeTitleBar( const Context& ctx )
{
	auto os = QOperatingSystemVersion::OSType::Windows ;

	auto minVersion = QOperatingSystemVersion( os,10,0,17763 ) ;

	auto currentVersion = QOperatingSystemVersion::current() ;

	if( currentVersion >= minVersion ){

		auto m = ctx.nativeHandleToMainWindow().value() ;

		BOOL dark = 1 ;

		DWORD DWMWA_USE_IMMERSIVE_DARK_MODE = 20 ;

		if( DwmSetWindowAttribute( m,DWMWA_USE_IMMERSIVE_DARK_MODE,&dark,sizeof( BOOL ) ) ){

			DWMWA_USE_IMMERSIVE_DARK_MODE = 19 ;

			DwmSetWindowAttribute( m,DWMWA_USE_IMMERSIVE_DARK_MODE,&dark,sizeof( BOOL ) ) ;
		}
	}
}

std::vector< utility::PlayerOpts > utility::getMediaPlayers()
{
	class buffer
	{
	public:
		buffer()
		{
			m_buffer[ 0 ] = '\0' ;
		}
		DWORD * size()
		{
			return &m_size ;
		}
		bool valid()
		{
			if( m_buffer[ 0 ] == '\0' ){

				return false ;

			}else if( this->equal( ".mp4" ) || this->equal( ".MP4" ) ){

				return false ;
			}else{
				return this->endsWith( ".mp4" ) || this->endsWith( ".MP4" ) ;
			}
		}
		operator char*()
		{
			return m_buffer.data() ;
		}
		operator const char*() const
		{
			return m_buffer.data() ;
		}
		operator QString()
		{
			return m_buffer.data() ;
		}
	private:
		bool equal( const char * b ) const
		{
			return std::strcmp( m_buffer.data(),b ) == 0 ;
		}
		bool endsWith( const utility::strl& m ) const
		{
			auto b   = m.data() ;
			auto len = m.size() ;

			auto a = static_cast< int >( m_size ) ;
			auto l = static_cast< int >( len ) ;

			if( a < l ){

				return false ;
			}else{
				auto aa = static_cast< size_t >( m_size ) ;

				return std::memcmp( m_buffer.data() + aa - len,b,len ) == 0 ;
			}
		} ;
		std::array< char,4096 > m_buffer ;
		DWORD m_size = 4096 ;
	} ;

	class Hkey
	{
	public:
		Hkey( Hkey& hkey,const buffer& subKey ) :
			m_status( this->open( hkey,subKey,hkey.regSam() ) )
		{
		}
		Hkey() : m_status( this->open( HKEY_CLASSES_ROOT,nullptr ) )
		{
		}
		~Hkey()
		{
			if( m_key ){

				RegCloseKey( m_key ) ;
			}
		}
		DWORD keyCount()
		{
			auto N = nullptr ;

			DWORD keyCount = 0 ;

			auto st = RegQueryInfoKeyA( m_key,N,N,N,&keyCount,N,N,N,N,N,N,N ) ;

			if( st == ERROR_SUCCESS ){

				return keyCount ;
			}else{
				return 0 ;
			}
		}
		QString getExePath()
		{
			auto N = nullptr ;

			buffer subKey ;

			auto path = "shell\\open\\command" ;

			auto st = RegGetValueA( m_key,path,N,RRF_RT_REG_SZ,N,subKey,subKey.size() ) ;

			if( st == ERROR_SUCCESS ){

				return subKey ;
			}else{
				return {} ;
			}
		}
		buffer getSubKey( DWORD i )
		{
			auto N = nullptr ;

			buffer subKey ;

			auto st = RegEnumKeyExA( m_key,i,subKey,subKey.size(),N,N,N,N ) ;

			if( st == ERROR_SUCCESS ){

				return subKey ;
			}else{
				return {} ;
			}
		}
		operator HKEY()
		{
			return m_key ;
		}
		operator bool()
		{
			return m_status == ERROR_SUCCESS ;
		}
		REGSAM regSam()
		{
			return m_regSam ;
		}
	private:
		LSTATUS open( HKEY hkey,const char * subKey )
		{
			REGSAM wow64 = KEY_READ | KEY_WOW64_64KEY ;
			REGSAM wow32 = KEY_READ | KEY_WOW64_32KEY ;

			auto st = this->open( hkey,subKey,wow64 ) ;

			if( st == ERROR_SUCCESS ){

				m_regSam = wow64 ;
			}else{
				st = this->open( hkey,subKey,wow32 ) ;

				if( st == ERROR_SUCCESS ){

					m_regSam = wow32 ;
				}
			}

			return st ;
		}
		LSTATUS open( HKEY hkey,const char * subKey,REGSAM regSam )
		{
			DWORD x = 0 ;

			return RegOpenKeyExA( hkey,subKey,x,regSam,&m_key ) ;
		}
		HKEY m_key = nullptr ;
		LSTATUS m_status ;
		REGSAM m_regSam ;
	} ;

	Hkey rootKey ;

	if( !rootKey ){

		return {} ;
	}

	std::vector< utility::PlayerOpts > s ;

	auto keyCount = rootKey.keyCount() ;

	for( DWORD i = 0 ; i < keyCount ; i++ ){

		auto subKey = rootKey.getSubKey( i ) ;

		if( !subKey.valid() ){

			continue ;
		}

		Hkey key( rootKey,subKey ) ;

		if( !key ){

			continue ;
		}

		auto p = util::splitPreserveQuotes( key.getExePath() ) ;

		if( p.size() ){

			auto m = p.first() ;

			if( m.endsWith( "wmplayer.exe" ) ){

				s.emplace_back( m,"Windows Media Player" ) ;
			}else{
				auto na = util::split( subKey,"." ) ;

				s.emplace_back( m,na.first() ) ;
			}
		}
	}

	return s ;
}

#else

std::vector< utility::PlayerOpts > utility::getMediaPlayers()
{
	std::vector< utility::PlayerOpts > m ;

	if( utility::platformisFlatPak() ){

		m.emplace_back( "","Flatpak" ) ;
	}else{
		struct app
		{
			app( const char * u,const char * e ) : uiName( u ),exeName( e )
			{
			}
			const char * uiName ;
			const char * exeName ;
		};

		std::array< app,3 > apps = { { { "VLC","vlc" },
					       { "SMPlayer","smplayer" },
					       { "MPV","mpv" } } } ;

		for( const auto& it : apps ){

			auto s = QStandardPaths::findExecutable( it.exeName ) ;

			if( !s.isEmpty() ){

				m.emplace_back( s,it.uiName ) ;
			}
		}
	}

	return m ;
}

void utility::windowsSetDarkModeTitleBar( const Context& )
{
}

QString utility::windowsGetClipBoardText( const ContextWinId& )
{
	return {} ;
}

QString utility::windowsApplicationDirPath()
{
	return {} ;
}

QString utility::windowsGateWayAddress()
{
	return {} ;
}

#endif

#ifdef Q_OS_WIN

#if QT_VERSION >= QT_VERSION_CHECK( 6,6,0 )

void utility::checkPermissions::enable()
{
}

void utility::checkPermissions::disable()
{
}

#else

extern Q_CORE_EXPORT int qt_ntfs_permission_lookup ;

void utility::checkPermissions::enable()
{
	qt_ntfs_permission_lookup++ ;
}

void utility::checkPermissions::disable()
{
	qt_ntfs_permission_lookup-- ;
}

#endif

#else

void utility::checkPermissions::enable()
{
}

void utility::checkPermissions::disable()
{
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

static void _kill_children_recursively( const QString& id )
{
	auto path = QString( "/proc/%1/task/" ).arg( id ) ;

	auto filter = QDir::Filter::Dirs | QDir::Filter::NoDotAndDotDot ;

	const auto ff = QDir( path ).entryList( filter ) ;

	for( const auto& it : ff ){

		QFile file( path + it + "/children" ) ;

		if( file.open( QIODevice::ReadOnly ) ){

			const auto pids = util::split( file.readAll(),' ',true ) ;

			for( const auto& it : pids ){

				_kill_children_recursively( it ) ;

				QProcess exe ;
				exe.start( "kill",{ "-s","SIGTERM",it } ) ;
				exe.waitForFinished( -1 ) ;
			}
		}
	}
}

bool utility::Terminator::terminate( QProcess& exe )
{
	if( utility::platformIsWindows() ){

		if( exe.state() == QProcess::ProcessState::Running ){

			QStringList args{ "/F","/T","/PID",QString::number( exe.processId() ) } ;

			QProcess::startDetached( "taskkill",args ) ;
		}

	}else if( utility::platformIsLinux() ){

		utils::qthread::run( [ &exe ](){

			_kill_children_recursively( QString::number( exe.processId() ) ) ;

			exe.terminate() ;
		} ) ;
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
	auto& tr = ctx.Translator() ;

	auto& configure = ctx.TabManager().Configure() ;

	struct webEntries
	{
		struct entries
		{
			entries( const QString& u,const QString& b ) : uiName( u ),bkName( b )
			{
			}
			QString uiName ;
			QString bkName ;
		} ;

		webEntries( const QString& w,const QString& uiName,const QString& bkName ) : website( w )
		{
			values.emplace_back( uiName,bkName ) ;
		}

		QString website ;
		std::vector< webEntries::entries > values ;
	} ;

	class entries
	{
	public:
		void add( const QString& website,const QString& uiName,const QString& bkName )
		{
			for( auto& it : m_entries ){

				if( it.website == website ){

					it.values.emplace_back( uiName,bkName ) ;

					return ;
				}
			}

			m_entries.emplace_back( website,uiName,bkName ) ;
		}
		void sort()
		{
		}
		void add( QMenu * menu,translator& tr )
		{
			this->sort() ;

			for( const auto& it : m_entries ){

				if( it.website.isEmpty() ){

					translator::entry ss( QObject::tr( "Preset Options" ),"","" ) ;
					tr.addAction( menu,ss.move() )->setEnabled( false ) ;
				}else{
					auto m = QObject::tr( "%1 Preset Options" ).arg( it.website ) ;

					translator::entry ss( m,"","" ) ;
					tr.addAction( menu,ss.move(),false )->setEnabled( false ) ;
				}

				for( const auto& xt : it.values ){

					menu->addAction( xt.uiName )->setObjectName( xt.bkName ) ;
				}

				menu->addSeparator() ;
			}
		}
	private:
		std::vector< webEntries > m_entries ;
	} ;

	entries mm ;

	configure.presetOptionsForEach( [ & ]( const configure::presetEntry& e ){

		if( combineText ){

			auto m = e.options + "\n" + e.uiNameTranslated ;

			mm.add( e.websiteTranslated,e.uiNameTranslated,m ) ;
		}else{
			mm.add( e.websiteTranslated,e.uiNameTranslated,e.options ) ;
		}
	} ) ;

	mm.add( menu,tr ) ;

	if( addClear ){

		menu->addSeparator() ;

		auto m = utility::selectedAction::CLEARSCREEN ;
		translator::entry sx( QObject::tr( "Clear" ),m,m ) ;

		tr.addAction( menu,sx.move() ) ;
	}

	if( addOpenFolder ){

		menu->addSeparator() ;

		auto m = utility::selectedAction::OPENFOLDER ;
		translator::entry mm( QObject::tr( "Open Download Folder" ),m,m ) ;

		tr.addAction( menu,mm.move() ) ;
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

	util::Timer( time,[ & ](){ e.exit() ; } ) ;

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

QStringList utility::updateOptions( const utility::updateOptionsStruct& s )
{
	const tableWidget::entry& ent  = s.tableEntry ;
	const engines::engine& engine  = s.engine ;
	const engines::enginePaths& ep = s.ctx.Engines().engineDirPaths() ;
	settings& settings             = s.stts ;
	const utility::args& args      = s.args ;
	const QStringList& urls        = s.urls ;
	bool forceDownload             = s.forceDownload ;
	const QString& downloadPath    = settings.downloadFolder() ;

	const utility::uiIndex& uiIndex       = s.uiIndex;
	const utility::downLoadOptions& dopts = s.dopts ;

	QStringList opts ;

	const auto& p = s.ctx.Engines().networkProxy() ;

	if( p.isSet() ){

		engine.setProxySetting( opts,p.networkProxyString() ) ;
	}

	auto oopts = [ & ](){

		if( dopts.hasExtraOptions ){

			return QStringList() ;
		}else{
			auto& t = s.ctx.TabManager().Configure() ;

			auto m = t.engineDefaultDownloadOptions( engine.name() ) ;

			if( m.isEmpty() ){

				return engine.defaultDownLoadCmdOptions() ;
			}else{
				return util::splitPreserveQuotes( m ) ;
			}
		}
	}() ;

	opts = opts + oopts ;

	for( const auto& it : args.otherOptions() ){

		opts.append( it ) ;
	}

	auto url = urls ;

	engines::engine::baseEngine::updateOpts ups( args,ent,uiIndex,url,opts ) ;

	engine.updateDownLoadCmdOptions( ups,settings.downloadOptionsAsLast() ) ;

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

	engine.setTextEncondig( opts ) ;

	opts.append( url ) ;

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

	auto _add = [ & ]( const tableWidget::entry& e ){

		if( e.url.isEmpty() ){

			return ;
		}

		auto obj = e.uiJson ;

		auto title = obj.value( "title" ).toString() ;

		if( !title.isEmpty() ){

			auto url = obj.value( "url" ).toString() ;

			if( title == url ){

				obj.remove( "title" ) ;
			}
		}

		if( !e.downloadingOptions.isEmpty() ){

			obj.insert( "downloadOptions",e.downloadingOptions ) ;
		}

		if( !e.engineName.isEmpty() ){

			obj.insert( "engineName",e.engineName ) ;
		}

		if( !e.extraDownloadingOptions.isEmpty() ){

			obj.insert( "downloadExtraOptions",e.extraDownloadingOptions ) ;
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
	m.setToolTipsVisible( true ) ;

	auto dialogTxt = QObject::tr( "Save List To File" ) ;
	auto toolTip = QObject::tr( "Filename with \".txt\" Extension Will Save Urls Only" ) ;

	auto ac = m.addAction( dialogTxt ) ;

	ac->setToolTip( toolTip ) ;

	QObject::connect( ac,&QAction::triggered,[ &ctx,&tableWidget,pld,toolTip ](){

		QString filePath ;

		if( pld && tableWidget.rowCount() > 1 ){

			auto uploader = tableWidget.entryAt( 1 ).uiJson.value( "uploader" ).toString() ;

			if( !uploader.isEmpty() ){

				filePath = utility::homePath() + "/MediaDowloaderList-" + uploader + ".json" ;
			}else{
				filePath = utility::homePath() + "/MediaDowloaderList.json" ;
			}
		}else{
			filePath = utility::homePath() + "/MediaDowloaderList.json" ;
		}

		auto s = QFileDialog::getSaveFileName( &ctx.mainWidget(),toolTip,filePath ) ;
		if( !s.isEmpty() ){

			const auto e = _saveDownloadList( tableWidget,false ) ;

			if( s.endsWith( ".json" ) ){

				auto m = QJsonDocument( e ).toJson( QJsonDocument::Indented ) ;

				engines::file( s,ctx.logger() ).write( m ) ;
			}else{
				QByteArray m ;

				for( const auto& it : e ){

					auto obj = it.toObject() ;

					auto title = obj.value( "title" ).toString().toUtf8() ;
					auto url   = obj.value( "url" ).toString().toUtf8() ;

					m.append( "#" + title + "\n" + url + "\n\n" ) ;
				}

				engines::file( s,ctx.logger() ).write( m ) ;
			}
		}
	} ) ;
}

bool utility::isRelativePath( const QString& e )
{
	return QDir::isRelativePath( e ) ;
}

static QString _stringValue( QJsonObject& obj,const char * key )
{
	return obj.value( key ).toString().replace( "\"NA\"","NA" ) ;
}

static QString _intValue( QJsonObject& obj,const char * key )
{
	return QString::number( obj.value( key ).toInt() ) ;
}

utility::MediaEntry::MediaEntry( const QJsonDocument& doc ) : m_json( doc )
{
	this->parseJson() ;
}

utility::MediaEntry::MediaEntry( const engines::engine& engine,const QByteArray& data ) :
	m_json( engine.parsePlayListData( data ) )
{
	if( m_json ){

		this->parseJson() ;
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
	obj.insert( "intDuration",m_intDuration ) ;
	obj.insert( "upload_date",u ) ;
	obj.insert( "uploader",m_uploader ) ;

	return obj ;
}

void utility::MediaEntry::parseJson()
{
	auto object = m_json.doc().object() ;

	m_formats              = object.value( "formats" ).toArray() ;

	m_title                = _stringValue( object,"title" ) ;
	m_url                  = _stringValue( object,"webpage_url" ) ;
	m_uploadDate           = _stringValue( object,"upload_date" ) ;
	m_id                   = _stringValue( object,"id" ) ;
	m_thumbnailUrl         = _stringValue( object,"thumbnail" ) ;
	m_uploader             = _stringValue( object,"uploader" ) ;
	m_playlist             = _stringValue( object,"playlist" ) ;
	m_playlist_id          = _stringValue( object,"playlist_id" ) ;
	m_playlist_title       = _stringValue( object,"playlist_title" ) ;
	m_playlist_uploader    = _stringValue( object,"playlist_uploader" ) ;
	m_playlist_uploader_id = _stringValue( object,"playlist_uploader_id" ) ;

	m_n_entries            = _intValue( object,"n_entries" ) ;
	m_playlist_count       = _intValue( object,"playlist_count" ) ;

	if( m_uploadDate.size() == 8 ){

		auto year  = m_uploadDate.mid( 0,4 ).toInt() ;
		auto month = m_uploadDate.mid( 4,2 ).toInt() ;
		auto day   = m_uploadDate.mid( 6,2 ).toInt() ;

		QDate d ;

		if( d.setDate( year,month,day ) ){

			m_uploadDate = d.toString() ;
		}
	}

	if( !m_uploadDate.isEmpty() ){

		m_uploadDate = utility::stringConstants::uploadDate() + " " + m_uploadDate ;
	}

	auto duration = object.value( "duration" ) ;

	if( duration.isDouble() ){

		m_intDuration = static_cast< int >( duration.toDouble() ) ;
	}else{
		m_intDuration = duration.toInt() ;
	}

	if( m_intDuration != 0 ){

		auto s = engines::engine::baseEngine::timer::duration( m_intDuration * 1000 ) ;
		m_duration = utility::stringConstants::duration() + " " + s ;
	}
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

utility::downLoadOptions utility::setDownloadOptions( const engines::engine& engine,
						      tableWidget& table,
						      int row,
						      const QString& downloadOpts )
{
	utility::downLoadOptions opts ;

	auto m = table.subTitle( row ) ;

	if( !m.isEmpty() ){

		auto s = util::split( m,' ',true ) ;

		if( s.size() > 1 ){

			auto e = engine.defaultSubtitleDownloadOptions().join( " " ) ;

			if( s.at( 0 ) == "ac:" ){

				m = " " + e + " --write-auto-subs --sub-langs " + s.at( 1 ) ;
			}else{
				m = " " + e + " --sub-langs " + s.at( 1 ) ;
			}
		}
	}

	auto z = table.timeInterval( row ) ;

	if( !z.isEmpty() ){

		m += " --download-sections *" + z ;
	}

	z = table.chapters( row ) ;

	if( !z.isEmpty() ){

		const auto mm = util::split( z,',',true ) ;

		for( const auto& it : mm  ){

			m += " --download-sections \"" + it + "\"" ;
		}
	}

	if( table.splitByChapters( row ) ){

		m += " --split-chapters" ;
	}

	auto zz = table.extraDownloadOptions( row ) ;

	if( !zz.isEmpty() ){

		opts.hasExtraOptions = true ;

		m += " " + zz + " " ;
	}

	opts.downloadOptions = [ & ](){

		auto u = table.downloadingOptions( row ) ;

		if( u.isEmpty() ){

			if( downloadOpts.isEmpty() ){

				return m ;
			}else{
				return downloadOpts + m ;
			}

		}else if( downloadOpts.isEmpty() ){

			return u + m ;

		}else if( u == downloadOpts ){

			return u + m ;
		}else{
			return u + m + " " + downloadOpts ;
		}
	}() ;

	return opts ;
}

void utility::setDefaultEngine( const Context& ctx,const QString& name )
{
	if( !name.isEmpty() ){

		ctx.Engines().setDefaultEngine( name ) ;

		ctx.TabManager().setDefaultEngines() ;
	}
}

bool utility::onlyWantedVersionInfo( const utility::cliArguments& args )
{
	if( args.contains( "--version" ) ){

		std::cout << utility::compileTimeVersion().toUtf8().constData() << std::endl ;

		return true ;
	}else{
		return false ;
	}
}

static util::version _get_process_version( const QString& path,
					   const QString& cmd,
					   const QProcessEnvironment& env )
{
	auto e = path + "/version_info.txt" ;

	QFile file( e ) ;

	if( file.exists() ){

		if( file.open( QIODevice::ReadOnly ) ){

			util::version m = file.readAll().trimmed() ;

			if( m.valid() ){

				return m ;
			}

			file.close() ;
		}

		file.remove() ;
	}

	QProcess exe ;

	exe.setProgram( cmd ) ;
	exe.setArguments( { "--version" } ) ;
	exe.setProcessEnvironment( env ) ;

	exe.start() ;

	exe.waitForFinished() ;

	util::version m = exe.readAllStandardOutput().trimmed() ;

	if( m.valid() ){

		QFile file( e ) ;

		if( file.open( QIODevice::WriteOnly | QIODevice::Truncate ) ){

			file.write( m.toString().toUtf8() ) ;
		}
	}

	return m ;
}

static bool _start_updated( QProcess& exe )
{
#if QT_VERSION >= QT_VERSION_CHECK( 5,10,0 )
	return exe.startDetached() ;
#else
	exe.start() ;
	exe.waitForFinished( -1 ) ;
	return true ;
#endif
}

bool utility::startedUpdatedVersion( settings& s,const utility::cliArguments& cargs )
{
	const auto& cpath = s.configPaths() ;

	auto ew = cpath.endsWith( "/" ) ;

	const auto m  = ew ? cpath + "update_new" : cpath + "/update_new" ;
	const auto mm = ew ? cpath + "update" : cpath + "/update" ;

	if( QFile::exists( m ) ){

		QDir dir( mm ) ;

		dir.removeRecursively() ;
		dir.rename( m,mm ) ;
	}

	QString exePath = mm + "/media-downloader.exe" ;

	if( QFile::exists( exePath ) && !cargs.runningUpdated() ){

		auto env = QProcessEnvironment::systemEnvironment() ;

		auto exeDirPath = utility::windowsApplicationDirPath() ;

		if( !QFile::exists( mm + "/platforms" ) ){

			env.insert( "PATH",exeDirPath + ";" + env.value( "PATH" ) ) ;
			env.insert( "QT_PLUGIN_PATH",exeDirPath ) ;
		}

		util::version uv = _get_process_version( mm,exePath,env ) ;

		util::version cv = utility::runningVersionOfMediaDownloader() ;

		if( uv.valid() ){

			if( cv < uv ){

				auto args = cargs.arguments( cpath,exeDirPath,s.portableVersion() ) ;

				QProcess exe ;

				exe.setProgram( exePath ) ;
				exe.setArguments( args ) ;
				exe.setProcessEnvironment( env ) ;

				return _start_updated( exe ) ;
			}else{
				QDir( mm ).removeRecursively() ;
			}
		}
	}

	return false ;
}

bool utility::platformIsLikeWindows()
{
	return utility::platformIsWindows() || utility::platformisOS2() ;
}

class runTimeVersionInfo
{
public:
	void setInstanceVersion( const QString& e )
	{
		m_instanceVersion = e ;
	}
	void setAboutInstanceVersion( const QString& e )
	{
		m_aboutInstanceVersion = e ;
	}
	const QString& instanceVersion() const
	{
		return m_instanceVersion ;
	}
	const QString& aboutInstanceVersion() const
	{
		return m_aboutInstanceVersion ;
	}
private:
	QString m_instanceVersion ;
	QString m_aboutInstanceVersion ;
} ;

static runTimeVersionInfo& _runTimeVersions()
{
	static runTimeVersionInfo m ;

	return m ;
}

QString utility::aboutVersionInfo()
{
	const auto& e = _runTimeVersions().aboutInstanceVersion() ;

	if( e.isEmpty() ){

		return utility::runningVersionOfMediaDownloader() ;
	}else{
		return e ;
	}
}

QString utility::compileTimeVersion()
{
	QString m = VERSION ;
	m.replace( ".git_tag","" ) ;

	auto s = util::split( m,"." ) ;

	QString e ;

	if( s.size() ){

		auto max = s.size() >= 4 ? 4 : s.size() ;

		e = s[ 0 ] ;

		for( int i = 1 ; i < max ; i++ ){

			e += "." + s[ i ] ;
		}
	}

	return e ;
}

bool utility::runningGitVersion()
{
	auto m = utility::runningVersionOfMediaDownloader() ;

	return util::split( m,"." ).size() > 3 ;
}

QString utility::runningVersionOfMediaDownloader()
{
	const auto& e = _runTimeVersions().instanceVersion() ;

	if( e.isEmpty() ){

		return utility::compileTimeVersion() ;
	}else{
		return e ;
	}
}

void utility::setRunningVersionOfMediaDownloader( const QString& e )
{
	_runTimeVersions().setInstanceVersion( e ) ;
}

void utility::setHelpVersionOfMediaDownloader( const QString& e )
{
	_runTimeVersions().setAboutInstanceVersion( e ) ;
}

static QStringList _parseOptions( const QString& e,const engines::engine& engine )
{
	auto m = util::splitPreserveQuotes( e ) ;

	if( m.isEmpty() ){

		return {} ;
	}

	const auto& q = engine.optionsArgument() ;

	if( q.isEmpty() ){

		return m ;
	}

	if( !m[ 0 ].startsWith( '-' ) ){

		if( m[ 0 ].compare( "default",Qt::CaseInsensitive ) ){

			m.insert( 0,q ) ;
		}
	}

	QStringList opts ;

	for( int i = 0 ; i < m.size() ; i++ ){

		const auto& s = m[ i ] ;

		if( s == q && i + 1 < m.size() ){

			const auto& ss = m[ i + 1 ] ;

			if( !ss.startsWith( '-' ) ){

				if( ss.compare( "default",Qt::CaseInsensitive ) ){

					opts.append( q ) ;

					opts.append( ss ) ;
				}
			}

			i++ ;
		}else{
			opts.append( s ) ;
		}
	}

	return opts ;
}

utility::args::args( const QString& uiOptions,const QString& otherOptions,const engines::engine& engine )
{
	m_uiDownloadOptions = _parseOptions( uiOptions,engine ) ;
	m_otherOptions      = _parseOptions( otherOptions,engine ) ;
	m_credentials       = engine.setCredentials( m_uiDownloadOptions,m_otherOptions ) ;
}

QStringList utility::args::options() const
{
	return m_otherOptions + m_uiDownloadOptions ;
}

QString utility::uiIndex::toString( bool pad,const QStringList& e ) const
{
	auto start = [ & ](){

		for( int m = 0 ; m < e.size() ; m++ ){

			if( e[ m ] == "--autonumber-start" ){

				if( m + 1 < e.size() ){

					return e[ m + 1 ].toInt() - 1 ;
				}
			}
		}

		return 0 ;
	}() ;

	if( pad ){

		return this->toString( start + m_index ) ;
	}else{
		return QString::number( start + m_index ) ;
	}
}

QString utility::uiIndex::toString( int index ) const
{
	auto s = QString::number( index ) ;

	auto m = QString::number( m_total ) ;

	while( s.size() < m.size() ){

		s.insert( 0,'0' ) ;
	}

	return s ;
}

void utility::setPermissions( QFile& qfile )
{
	if( !QFileInfo( qfile ).isExecutable() ){

		qfile.setPermissions( qfile.permissions() | QFileDevice::ExeOwner ) ;
	}
}

void utility::setPermissions( const QString& e )
{
	QFile s( e ) ;

	utility::setPermissions( s ) ;
}

void utility::networkReply::getData( const Context& ctx,const utils::network::reply& reply )
{
	if( reply.success() ){

		m_data = reply.data() ;

	}else if( reply.timeOut() ){

		QString m = "Network Error: Network Request Timed Out" ;

		ctx.logger().add( m,utility::sequentialID() ) ;
	}else{
		ctx.logger().add( "Network Error: " + reply.errorString(),utility::sequentialID() ) ;
	}
}

utility::cliArguments::cliArguments( int argc,char ** argv )
{
	for( int i = 0 ; i < argc ; i++ ){

		m_args.append( argv[ i ] ) ;
	}

	if( this->runningUpdated() ){

		utility::setRunningVersionOfMediaDownloader( this->value( "--fake-updated-version" ) ) ;
	}else{
		utility::setRunningVersionOfMediaDownloader( this->value( "--fake-version" ) ) ;
	}
}

bool utility::cliArguments::contains( const char * m ) const
{
	return m_args.contains( m ) ;
}

bool utility::cliArguments::runningUpdated() const
{
	return this->contains( "--running-updated" ) ;
}

bool utility::cliArguments::portable() const
{
	return this->contains( "--portable" ) ;
}

QString utility::cliArguments::dataPath() const
{
	return this->value( "--dataPath" ) ;
}

QString utility::cliArguments::originalPath() const
{
	return this->value( "--exe-org-path" ) ;
}

QString utility::cliArguments::originalVersion() const
{
	return this->value( "--running-version" ) ;
}

QString utility::cliArguments::value( const char * m ) const
{
	for( auto it = m_args.begin() ; it != m_args.end() ; it++ ){

		if( *it == m ){

			auto xt = it + 1 ;

			if( xt != m_args.end() ){

				return *xt ;
			}
		}
	}

	return {} ;
}

QStringList utility::cliArguments::arguments( const QString& cpath,
					      const QString& exeDirPath,
					      bool portableVersion ) const
{
	auto args = m_args ;

	args.append( "--running-updated" ) ;

	args.append( "--dataPath" ) ;

	args.append( cpath ) ;

	args.append( "--running-version" ) ;

	args.append( utility::runningVersionOfMediaDownloader() ) ;

	if( portableVersion ){

		args.append( "--portable" ) ;
	}

	args.append( "--exe-org-path" ) ;
	args.append( exeDirPath ) ;

	return args ;
}

const QStringList& utility::cliArguments::arguments() const
{
	return m_args ;
}

bool utility::pathIsFolderAndExists( const QString& e )
{
	QFileInfo m( e ) ;

	return m.exists() && m.isDir() ;
}

QByteArray utility::barLine()
{
	return "*************************************************************" ;
}

utility::printOutPut::printOutPut( const utility::cliArguments& args )
{
	if( args.contains( "--qDebug" ) || args.contains( "--qdebug" ) ){

		m_status = utility::printOutPut::status::qdebug ;

	}else if( args.contains( "--debug" ) ){

		m_status = utility::printOutPut::status::debug ;
	}else{
		auto m = args.value( "--log-to-file" ) ;

		if( !m.isEmpty() ){

			m_outPutFile.setFileName( m ) ;

			m_outPutFile.open( QIODevice::WriteOnly | QIODevice::Append ) ;
		}
	}
}

void utility::printOutPut::operator()( int id,const QByteArray& e )
{
	if( m_outPutFile.isOpen() ){

		m_outPutFile.write( e ) ;
	}

	if( m_status == utility::printOutPut::status::qdebug ){

		qDebug() << "id: " + QString::number( id ) ;
		qDebug() << e ;
		qDebug() << "--------------------------------" ;

	}else if( m_status == utility::printOutPut::status::debug ){

		auto m = "id: " + QString::number( id ).toUtf8() ;

		std::cout << m.constData() << std::endl ;
		std::cout << e.constData() << std::endl ;
		std::cout << "--------------------------------" << std::endl ;
	}
}

utility::printOutPut::operator bool() const
{
	return m_status != utility::printOutPut::status::notSet ;
}

void utility::failedToParseJsonData( Logger& logger,const QJsonParseError& error )
{
	auto id = utility::sequentialID() ;

	logger.add( "Failed To Parse Json Data:" + error.errorString(),id ) ;
}

void utility::hideUnhideEntries( QMenu& m,tableWidget& table,int row,bool showHide )
{
	if( showHide ){

		auto ac = m.addAction( QObject::tr( "Hide Row" ) ) ;

		QObject::connect( ac,&QAction::triggered,[ &table,row ](){

			table.hideRow( row ) ;
		} ) ;
	}

	if( table.containsHiddenRows() ){

		auto ac = m.addAction( QObject::tr( "Unhide All Hidden Rows" ) ) ;

		QObject::connect( ac,&QAction::triggered,[ &table ](){

			auto& t = table.get() ;

			for( int row = 0 ; row < table.rowCount() ; row++ ){

				if( t.isRowHidden( row ) ){

					t.showRow( row ) ;
				}
			}
		} ) ;
	}
}

static QStringList _listOptionsFromDownloadOptions( const QString& e )
{
	QStringList m ;

	auto ee = util::splitPreserveQuotes( e ) ;

	for( auto it = ee.begin() ; it != ee.end() ; it++ ){

		const auto& s = *it ;

		if( s == "\"--proxy\"" || s == "--proxy" ){

			auto xt = it + 1 ;

			if( xt != ee.end() ){

				m.append( "--proxy" ) ;
				m.append( *xt ) ;
			}

			break ;
		}
	}

	return m ;
}

void utility::addToListOptionsFromsDownload( QStringList& args,
					     const QString& downLoadOptions,
					     const Context& ctx,
					     const engines::engine& engine )
{
	auto m = ctx.TabManager().Configure().engineDefaultDownloadOptions( engine.name() ) ;

	auto ee = _listOptionsFromDownloadOptions( m ) ;

	const auto& mm = ctx.Engines().networkProxy() ;

	if( mm.isSet() ){

		engine.setProxySetting( args,mm.networkProxyString() ) ;
	}

	if( !ee.isEmpty() ){

		args = args + ee ;
	}

	auto ss = args + _listOptionsFromDownloadOptions( downLoadOptions ) ;

	for( int i = ss.size() - 2 ; i > -1 ; i-- ){

		if( ss[ i ] == "--proxy" ){

			return mm.setApplicationProxy( ss[ i + 1 ] ) ;
		}
	}

	mm.setDefaultProxy() ;
}

bool utility::copyFile( const QString& s,const QString& d,bool setExePermssion )
{
	QFile src( s ) ;

	if( src.open( QIODevice::ReadOnly ) ){

		QFile dst( d ) ;

		if( dst.open( QIODevice::WriteOnly | QIODevice::Truncate ) ){

			std::array< char,1024 > buffer ;

			while( true ){

				auto m = src.read( buffer.data(),buffer.size() ) ;

				if( m > 0 ){

					dst.write( buffer.data(),m ) ;
				}else{
					if( src.size() == dst.size() ){

						if( setExePermssion ){

							auto s = dst.permissions() | QFileDevice::ExeOwner ;

							dst.setPermissions( s ) ;
						}

						return true ;
					}else{
						dst.remove() ;

						return false ;
					}
				}
			}
		}
	}

	return false ;
}

bool utility::addData( const QByteArray& e )
{
	auto s = "\r                                                      \r" ;

	if( e == "\r\r" || e == s || e.contains( "[download] " ) ){

		return false ;
	}else{
		return true ;
	}
}

void utility::contextMenuForDirectUrl( const QJsonObject& obj,const Context& ctx )
{	
	auto arr = obj.value( "urls" ).toArray() ;

	QMenu m ;

	auto mediaPlayer = ctx.Settings().openWith( ctx.logger() ) ;

	if( arr.size() == 0 ){

		m.addAction( QObject::tr( "Copy Url" ) )->setEnabled( false ) ;

		if( mediaPlayer.valid() ){

			for( const auto& e : mediaPlayer.opts() ){

				auto s = QObject::tr( "Open Url With %1" ).arg( e.name ) ;

				m.addAction( s )->setEnabled( false ) ;
			}
		}
	}else{
		auto clipBoard = QApplication::clipboard() ;

		auto act = &QAction::triggered ;

		if( clipBoard ){

			if( arr.size() == 1 ){

				auto url = arr[ 0 ].toString() ;

				auto ee = m.addAction( QObject::tr( "Copy Url" ) ) ;

				QObject::connect( ee,act,[ clipBoard,url ](){

					clipBoard->setText( url ) ;
				} ) ;
			}else{
				for( int i = 0 ; i < arr.size() ; i++ ){

					auto e = QString::number( i + 1 ) ;

					auto s = QObject::tr( "Copy Url %1" ).arg( e ) ;

					auto url = arr[ i ].toString() ;

					auto ee = m.addAction( s ) ;

					QObject::connect( ee,act,[ clipBoard,url ](){

						clipBoard->setText( url ) ;
					} ) ;
				}
			}
		}

		if( mediaPlayer.valid() ){

			const auto& adp = ctx.Settings().appDataPath() ;

			if( arr.size() == 1 ){

				for( const auto& e : mediaPlayer.opts() ){

					auto s = QObject::tr( "Open Url With %1" ).arg( e.name ) ;

					auto ee = m.addAction( s ) ;

					auto ac = mediaPlayer.ac( arr[ 0 ].toString(),e,adp,obj ) ;

					QObject::connect( ee,act,ac.move() ) ;
				}

			}else{
				for( int i = 0 ; i < arr.size() ; i++ ){

					auto e = QString::number( i + 1 ) ;

					for( const auto& a : mediaPlayer.opts() ){

						auto s = QObject::tr( "Open Url %1 With %2" ).arg( e,a.name ) ;

						auto ee = m.addAction( s ) ;

						auto ac = mediaPlayer.ac( arr[ i ].toString(),a,adp,obj ) ;

						QObject::connect( ee,act,ac.move() ) ;
					}
				}
			}
		}
	}

	m.exec( QCursor::pos() ) ;
}

void utility::deleteTmpFiles( const QString& df,std::vector< QByteArray > files )
{
	utils::qthread::run( [ df,files = std::move( files ) ](){

		for( const auto& it : files ){

			auto m = df + "/" + it ;

			QFile::remove( m + ".part" ) ;
			QFile::remove( m ) ;
		}
	} ) ;
}

bool utility::Qt6Version()
{
#if QT_VERSION > QT_VERSION_CHECK( 6,0,0 )
	return true ;
#else
	return false ;
#endif
}

QString utility::OSXApplicationDirPath()
{
	return QCoreApplication::applicationDirPath() ;
}

QString utility::OSXtranslationFilesPath()
{
	return utility::OSXApplicationDirPath() + "/../Resources/translations" ;
}

QString utility::OSX3rdPartyDirPath()
{
	return utility::OSXApplicationDirPath() + "/extra" ;
}

bool utility::platformisFlatPak()
{
#if FLATPAK
	return true ;
#else
	return false ;
#endif
}

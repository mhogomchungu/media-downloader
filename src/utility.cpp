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
#include "reportFinished.h"
#include "tableWidget.h"
#include "tabmanager.h"
#include "version.h"
#include "windows.h"

#include <QEventLoop>
#include <QDesktopServices>
#include <QClipboard>
#include <QMimeData>
#include <QFileDialog>
#include <QSysInfo>
#include <QFile>
#include <QJsonDocument>
#include <QOperatingSystemVersion>

#include <ctime>
#include <cstring>

namespace utility
{
namespace local
{
	class PretendPlatform
	{
	public:
		PretendPlatform( const QStringList& m )
		{
			if( utility::platformIsWindows() ){

				m_pretend32Bit = m.contains( "--pretend-x86" ) ;

				m_pretendWindows7 = m.contains( "--pretend-win7" ) ;

				m_pretendLegacyWindows = m.contains( "--pretend-winLegacy" ) ;
			}
		}
		bool isWindows7() const
		{
			return m_pretendWindows7 ;
		}
		bool is32Bit() const
		{
			return m_pretend32Bit ;
		}
		bool isLegacyWindows() const
		{
			return m_pretendLegacyWindows ;
		}
	private:
		bool m_pretend32Bit    = false ;
		bool m_pretendWindows7 = false ;
		bool m_pretendLegacyWindows = false ;
	} ;

	static std::unique_ptr< PretendPlatform > pretendPlatformImpl ;

	void setPretendPlatform( const QStringList& s )
	{
		pretendPlatformImpl = std::make_unique< PretendPlatform >( s ) ;
	}

	const PretendPlatform& pretendPlatform()
	{
		if( pretendPlatformImpl ){

			return *pretendPlatformImpl ;
		}else{
			qDebug() << "ERROR: utility::local::PretendPlatform Is Not Set As Expected" ;

			utility::local::setPretendPlatform( {} ) ;
			return *pretendPlatformImpl ;
		}
	}

	class SysPlatForm
	{
	public:
		SysPlatForm()
		{
			auto m            = QOperatingSystemVersion::current() ;

			m_isWin7          = this->Win7( m ) ;
			m_isLegacyWindows = this->LegacyWindows( m ) ;
		}
		bool isWin7() const
		{
			return m_isWin7 ;
		}
		bool isLegacyWindows() const
		{
			return m_isLegacyWindows ;
		}
		QString errorMessage() const
		{
			#ifdef Q_OS_WIN
				return windows::errorMessage() ;
			#else
				return strerror( errno ) ;
			#endif
		}
		bool isOs2() const
		{
			#if defined(__OS2__) || defined(OS2) || defined(_OS2)
				return true ;
			#else
				return false ;
			#endif
		}
		bool isWindows() const
		{
			#ifdef Q_OS_WIN
				return true ;
			#else
				return false ;
			#endif
		}
		bool isLinux() const
		{
			#ifdef Q_OS_LINUX
				return true ;
			#else
				return false ;
			#endif
		}
		bool isMacOs() const
		{
			#ifdef Q_OS_MACOS
				return true ;
			#else
				return false ;
			#endif
		}
	private:
		bool Win7( const QOperatingSystemVersion& system )
		{
			if( this->isWindows() ){

				if( utility::local::pretendPlatform().isWindows7() ){

					return true ;
				}else{
					return system < QOperatingSystemVersion::Windows8 ;
				}
			}else{
				return false ;
			}
		}
		bool LegacyWindows( const QOperatingSystemVersion& system )
		{
			if( this->isWindows() ){

				const auto& m = utility::local::pretendPlatform() ;

				if( m.isLegacyWindows() || m.isWindows7() ){

					return true ;
				}else{
					if( system < QOperatingSystemVersion::Windows10 ){

						return true ;

					}else if( system > QOperatingSystemVersion::Windows10 ){

						return false ;
					}else{
						/*
						 * Windows 10 (1903)       10.0.18362
						 * Windows 10 (1809)       10.0.17763
						 * Windows 10 (1803)       10.0.17134
						 * Windows 10 (1709)       10.0.16299
						 * Windows 10 (1703)       10.0.15063
						 * Windows 10 (1607)       10.0.14393
						 * Windows 10 (1511)       10.0.10586
						 * Windows 10              10.0.10240
						 */
						return system.microVersion() < 16299 ;
					}
				}
			}else{
				return false ;
			}
		}
		bool m_isWin7 ;
		bool m_isLegacyWindows ;
	} ;

	const SysPlatForm& Platform()
	{
		static SysPlatForm m ;

		return m ;
	}
}
}

bool utility::platformisOS2()
{
	return utility::local::Platform().isOs2() ;
}

bool utility::platformIsLinux()
{
	return utility::local::Platform().isLinux() ;
}

bool utility::platformIsOSX()
{
	return utility::local::Platform().isMacOs() ;
}

bool utility::platformIsWindows()
{
	return utility::local::Platform().isWindows() ;
}

bool utility::platformIsWindows7()
{
	return utility::local::Platform().isWin7() ;
}

bool utility::platformisLegacyWindows()
{
	return utility::local::Platform().isLegacyWindows() ;
}

QString utility::errorMessage()
{
	return utility::local::Platform().errorMessage() ;
}

void utility::checkPermissions::enable()
{
	if( utility::platformIsWindows() ){

		windows::enableCheckPermissions() ;
	}
}

void utility::checkPermissions::disable()
{
	if( utility::platformIsWindows() ){

		windows::disableCheckPermissions() ;
	}
}

class fileRename
{
public:
	fileRename( const QString& oldPath,const QString& newPath ) :
		m_oldPath( oldPath ),m_newPath( newPath )
	{
	}
	bool exec() const
	{
		return QFile::rename( m_oldPath,m_newPath ) ;
	}
	QString errorString() const
	{
		return utility::errorMessage() ;
	}
	const QString& oldPath() const
	{
		return m_oldPath ;
	}
	const QString& newPath() const
	{
		return m_newPath ;
	}
private:
	const QString& m_oldPath ;
	const QString& m_newPath ;
} ;

class fileRemove
{
public:
	fileRemove( const QString& s ) : m_src( s )
	{
	}
	bool exec() const
	{
		if( QFile::exists( m_src ) ){

			return QFile::remove( m_src ) ;
		}else{
			return true ;
		}
	}
	QString errorString() const
	{
		return utility::errorMessage() ;
	}
private:
	const QString& m_src ;
} ;

class dirRemove
{
public:
	dirRemove( const QString& s ) : m_src( s )
	{
	}
	bool exec() const
	{
		QDir dir( m_src ) ;

		if( dir.exists() ){

			return dir.removeRecursively() ;
		}else{
			return true ;
		}
	}
	QString errorString() const
	{
		return utility::errorMessage() ;
	}
private:
	const QString& m_src ;
} ;

std::vector< utility::PlayerOpts > utility::getMediaPlayers()
{
	if( utility::platformIsWindows() ){

		return windows::getMediaPlayers() ;
	}

	std::vector< utility::PlayerOpts > m ;

	if( utility::platformisFlatPak() ){

		m.emplace_back( "",QObject::tr( "Default Player" ) ) ;
		m.emplace_back( "vlc","VLC" ) ;
	}else{
		class appList
		{
		public:
			struct app
			{
				app( const char * u,const char * e ) : uiName( u ),exeName( e )
				{
				}
				const char * uiName ;
				const char * exeName ;
			} ;
			appList()
			{
				m_appList.emplace_back( "VLC","vlc" ) ;
				m_appList.emplace_back( "SMPlayer","smplayer" ) ;
				m_appList.emplace_back( "MPV","mpv" ) ;
			}
			auto begin() const
			{
				return m_appList.begin() ;
			}
			auto end() const
			{
				return m_appList.end() ;
			}
		private:
			std::vector< app > m_appList ;
		} apps ;

		for( const auto& it : apps ){

			auto s = QStandardPaths::findExecutable( it.exeName ) ;

			if( !s.isEmpty() ){

				m.emplace_back( s,it.uiName ) ;
			}
		}
	}

	return m ;
}

void utility::windowsSetDarkModeTitleBar( const Context& ctx )
{
	return windows::setDarkModeTitleBar( ctx ) ;
}

QString utility::windowsGetClipBoardText( const ContextWinId& e )
{
	return windows::getClipBoardText( e ) ;
}

QString utility::windowsApplicationDirPath()
{
	return windows::applicationDirPath() ;
}

QString utility::windowsGateWayAddress()
{
	return windows::gateWayAddress() ;
}

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

			QStringList args{ "/F","/T","/PID",QString::number( exe.processId() ) } ;

			QProcess::startDetached( "taskkill",args ) ;
		}

	}else if( utility::platformIsLinux() ){

		class meaw
		{
		public:
			meaw( QProcess& exe ) : m_exe( exe )
			{
			}
			void bg()
			{
				this->terminate( QString::number( m_exe.processId() ) ) ;
			}
			void fg()
			{
				m_exe.terminate() ;
			}
		private:
			void terminate( const QString& id )
			{
				auto path = QString( "/proc/%1/task/" ).arg( id ) ;

				auto filter = QDir::Filter::Dirs | QDir::Filter::NoDotAndDotDot ;

				const auto ff = QDir( path ).entryList( filter ) ;

				for( const auto& it : ff ){

					QFile file( path + it + "/children" ) ;

					if( file.open( QIODevice::ReadOnly ) ){

						const auto pids = util::split( file.readAll(),' ',true ) ;

						for( const auto& it : pids ){

							this->terminate( it ) ;

							QProcess exe ;
							exe.start( "kill",{ "-s","SIGTERM",it } ) ;
							exe.waitForFinished( -1 ) ;
						}
					}
				}
			}
			QProcess& m_exe ;
		} ;

		utils::qthread::run( meaw( exe ) ) ;
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

		auto m = utility::selectedAction::clearScreenText() ;
		translator::entry sx( QObject::tr( "Clear" ),m,m ) ;

		tr.addAction( menu,sx.move() ) ;
	}

	if( addOpenFolder ){

		menu->addSeparator() ;

		auto m = utility::selectedAction::openFolderText() ;
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

namespace utility
{
static QString downloadPath()
{
#if QT_VERSION >= QT_VERSION_CHECK( 5,6,0 )

	auto s = QStandardPaths::standardLocations( QStandardPaths::DownloadLocation ) ;

	if( s.isEmpty() ){

		return QDir::homePath() + "/Downloads" ;
	}else{
		return s.first() ;
	}
#else
	return QDir::homePath() + "/Downloads" ;
#endif
}
}

QString utility::homePath()
{
	if( utility::platformIsWindows() ){

		return QDir::homePath() + "/Desktop" ;

	}else if( utility::platformisFlatPak() ){

		return utility::downloadPath() ;
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
	const QString& downloadPath    = settings.downloadFolder() ;

	const utility::uiIndex& uiIndex       = s.uiIndex;
	const utility::downLoadOptions& dopts = s.dopts ;

	QStringList opts ;

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

	auto url = urls ;

	engines::engine::baseEngine::updateOpts ups( args,ent,uiIndex,url,opts ) ;

	engine.updateDownLoadCmdOptions( ups,settings.downloadOptionsAsLast() ) ;

	utility::setCookieOption( opts,settings,engine ) ;

	for( auto& it : opts ){

		it.replace( utility::stringConstants::mediaDownloaderDataPath(),ep.dataPath() ) ;
		it.replace( utility::stringConstants::mediaDownloaderDefaultDownloadPath(),downloadPath ) ;
		it.replace( utility::stringConstants::mediaDownloaderCWD(),QDir::currentPath() ) ;
	}

	if( settings.useInternalArchiveFile() ){

		opts.append( ep.archiveFileArgument( engine ) ) ;
	}

	engine.setTextEncondig( opts ) ;

	opts.append( url ) ;

	return opts ;
}

void utility::initDone()
{
}

int utility::loggerID()
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

namespace utility
{
static QJsonArray saveDownloadList( tableWidget& tableWidget,bool noFinishedSuccess )
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

			using gg = reportFinished::finishedStatus ;

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

		auto arr = utility::saveDownloadList( tableWidget,true ) ;

		auto e = ctx.Engines().engineDirPaths().dataPath( "autoSavedList.json" ) ;

		if( QFile::exists( e ) ){

			auto m = engines::file( e,ctx.logger() ).readAll() ;

			QFile::remove( e ) ;

			const auto rr = QJsonDocument::fromJson( m ).array() ;

			for( const auto it : rr ){

				arr.append( it ) ;
			}
		}

		if( arr.size() ){

			auto m = QJsonDocument( arr ).toJson( QJsonDocument::Indented ) ;

			engines::file( e,ctx.logger() ).write( m ) ;
		}
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

			const auto e = utility::saveDownloadList( tableWidget,false ) ;

			if( s.endsWith( ".json" ) ){

				auto m = QJsonDocument( e ).toJson( QJsonDocument::Indented ) ;

				engines::file( s,ctx.logger() ).write( m ) ;
			}else{
				QByteArray m ;

				for( const auto it : e ){

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

utility::MediaEntry::MediaEntry( const QJsonDocument& doc ) : m_json( doc )
{
	this->parseJson() ;
}

utility::MediaEntry::MediaEntry( const QString& url,const engines::engine& engine,const QByteArray& data ) :
	m_json( engine.parsePlayListData( url,data ) )
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
	obj.insert( "id",m_id ) ;

	return obj ;
}

void utility::MediaEntry::parseJson()
{
	auto object = m_json.toObject() ;

	m_formats              = object.value( "formats" ).toArray() ;

	m_title                = this->stringValue( object,"title" ) ;
	m_url                  = this->stringValue( object,"webpage_url" ) ;
	m_uploadDate           = this->stringValue( object,"upload_date" ) ;
	m_id                   = this->stringValue( object,"id" ) ;
	m_thumbnailUrl         = this->stringValue( object,"thumbnail" ) ;
	m_uploader             = this->stringValue( object,"uploader" ) ;
	m_playlist             = this->stringValue( object,"playlist" ) ;
	m_playlist_id          = this->stringValue( object,"playlist_id" ) ;
	m_playlist_title       = this->stringValue( object,"playlist_title" ) ;
	m_playlist_uploader    = this->stringValue( object,"playlist_uploader" ) ;
	m_playlist_uploader_id = this->stringValue( object,"playlist_uploader_id" ) ;

	m_n_entries            = this->intValue( object,"n_entries" ) ;
	m_playlist_count       = this->intValue( object,"playlist_count" ) ;

	auto headers = object.value( "http_headers" ).toObject() ;
	m_referer    = headers.value( "Referer" ).toString() ;
	m_userAgent  = headers.value( "User-Agent" ).toString() ;

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

void utility::addJsonCmd::add( const utility::addJsonCmd::entry& e )
{
	QJsonObject s ;

	for( const auto& it : e.platformData ){

		QJsonObject a ;

		a.insert( "Name",it.exeName() ) ;

		QJsonArray arr ;

		for( const auto& xt : it.exeArgs() ){

			arr.append( xt ) ;
		}

		a.insert( "Args",arr ) ;

		s.insert( it.archName(),a ) ;
	}

	m_obj.insert( e.platform,s ) ;
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

namespace utility
{
static util::version get_process_version( const QString& path,
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

static bool start_updated( QProcess& exe )
{
#if QT_VERSION >= QT_VERSION_CHECK( 5,10,0 )
	return exe.startDetached() ;
#else
	exe.start() ;
	exe.waitForFinished( -1 ) ;
	return true ;
#endif
}

}

bool utility::startedUpdatedVersion( settings& s,const utility::cliArguments& cargs )
{
	if( utility::platformIsNOTWindows() ){

		return false ;
	}

	const auto& cpath = s.configPaths() ;

	auto ew = cpath.endsWith( "/" ) ;

	const auto update_new  = ew ? cpath + "update_new" : cpath + "/update_new" ;
	const auto update      = ew ? cpath + "update" : cpath + "/update" ;

	QString updated_old ;

	if( QFile::exists( update_new ) ){

		QDir dir ;

		if( QFile::exists( update ) ){

			while( true ){

				auto m = utility::simpleRandomNumber() ;

				updated_old = update + "-" + QString::number( m ) ;

				if( QFileInfo::exists( updated_old ) ){

					QThread::currentThread()->sleep( 1 ) ;
				}else{
					break ;
				}
			}

			dir.rename( update,updated_old ) ;
		}

		dir.rename( update_new,update ) ;
	}

	QString exePath = update + "/media-downloader.exe" ;

	if( QFile::exists( exePath ) && !cargs.runningUpdated() ){

		auto env = QProcessEnvironment::systemEnvironment() ;

		auto exeDirPath = utility::windowsApplicationDirPath() ;

		if( !QFile::exists( update + "/platforms" ) ){

			env.insert( "PATH",exeDirPath + ";" + env.value( "PATH" ) ) ;
			env.insert( "QT_PLUGIN_PATH",exeDirPath ) ;
		}

		util::version uv = utility::get_process_version( update,exePath,env ) ;

		util::version cv = utility::runningVersionOfMediaDownloader() ;

		if( uv.valid() ){

			if( cv < uv ){

				auto e = s.portableVersion() ;
				auto args = cargs.arguments( cpath,exeDirPath,updated_old,e ) ;

				QProcess exe ;

				exe.setProgram( exePath ) ;
				exe.setArguments( args ) ;
				exe.setProcessEnvironment( env ) ;

				return utility::start_updated( exe ) ;
			}else{
				utils::qthread::run( [ update ]{ QDir( update ).removeRecursively() ; } ) ;
			}
		}
	}

	return false ;
}

bool utility::platformIsLikeWindows()
{
	return utility::platformIsWindows() || utility::platformisOS2() ;
}

namespace utility
{
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

static runTimeVersionInfo& runTimeVersions()
{
	static runTimeVersionInfo m ;

	return m ;
}

}

QString utility::aboutVersionInfo()
{
	const auto& e = utility::runTimeVersions().aboutInstanceVersion() ;

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

	return utility::runningGitVersion( m ) ;
}

bool utility::runningGitVersion( const QString& m )
{
	return util::split( m,"." ).size() > 3 ;
}

const QString& utility::fakeRunningVersionOfMediaDownloader()
{
	return utility::runTimeVersions().instanceVersion() ;
}

QString utility::runningVersionOfMediaDownloader()
{
	const auto& e = utility::fakeRunningVersionOfMediaDownloader() ;

	if( e.isEmpty() ){

		return utility::compileTimeVersion() ;
	}else{
		return e ;
	}
}

QString utility::parseVersionInfo( const utils::qprocess::outPut& r )
{
	if( r.success() ){

		if( utility::containsLinkerWarning( r.stdOut ) ){

			const auto m = util::split( r.stdOut,'\n' ) ;

			QStringList s ;

			for( const auto& it : m ){

				if( !utility::containsLinkerWarning( it ) ){

					s.append( it ) ;
				}
			}

			return s.join( '\n' ).toUtf8() ;
		}else{
			return r.stdOut ;
		}
	}else{
		return {} ;
	}
}

void utility::setRunningVersionOfMediaDownloader( const QString& e )
{
	utility::runTimeVersions().setInstanceVersion( e ) ;
}

void utility::setHelpVersionOfMediaDownloader( const QString& e )
{
	utility::runTimeVersions().setAboutInstanceVersion( e ) ;
}

QStringList utility::args::parseOptions( const QString& e,const engines::engine& engine )
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
	m_uiDownloadOptions = this->parseOptions( uiOptions,engine ) ;
	m_otherOptions      = this->parseOptions( otherOptions,engine ) ;
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

		if( m_data.isEmpty() ){

			QString m = "Network Error: Request Succeeded But No Data Received" ;

			ctx.logger().add( m,utility::loggerID() ) ;
		}
	}else if( reply.timeOut() ){

		QString m = "Network Error: Network Request Timed Out" ;

		ctx.logger().add( m,utility::loggerID() ) ;
	}else{
		ctx.logger().add( "Network Error: " + reply.errorString(),utility::loggerID() ) ;
	}
}

namespace utility
{
	static bool useFakeHash ;
	static bool cliArguments_debug ;
}

bool utility::cliArguments::useFakeMdHash()
{
	return utility::useFakeHash ;
}

bool utility::cliArguments::debug()
{
	return utility::cliArguments_debug ;
}

utility::cliArguments::cliArguments( int argc,char ** argv )
{
	for( int i = 0 ; i < argc ; i++ ){

		m_args.append( argv[ i ] ) ;
	}

	if( m_args.contains( "--qDebug" ) || m_args.contains( "--qdebug" ) || m_args.contains( "--debug" ) ){

		utility::cliArguments_debug = true ;
	}else{
		utility::cliArguments_debug = false ;
	}

	utility::useFakeHash = this->contains( "--fake-hash" ) ;

	utility::local::setPretendPlatform( m_args ) ;

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

bool utility::cliArguments::printMediaPlayers() const
{
	return this->contains( "--show-media-players" ) ;
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

QString utility::cliArguments::pathToOldUpdatedVersion() const
{
	return this->value( "--path-to-old-updated-version" ) ;
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
					      const QString& oldVersionPath,
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

	if( !oldVersionPath.isEmpty() ){

		args.append( "--path-to-old-updated-version" ) ;
		args.append( oldVersionPath ) ;
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
	if( e.isEmpty() ){

		return false ;
	}else{
		QFileInfo m( e ) ;

		return m.exists() && m.isDir() ;
	}
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

			auto m = m_outPutFile.open( QIODevice::WriteOnly | QIODevice::Append ) ;

			Q_UNUSED( m )
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

bool utility::printOutPut::valid() const
{
	return m_status != utility::printOutPut::status::notSet ;
}

void utility::failedToParseJsonData( Logger& logger,const QJsonParseError& error )
{
	auto id = utility::loggerID() ;

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

namespace utility
{
static QStringList listOptionsFromDownloadOptions( const QString& e )
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

}

void utility::addToListOptionsFromsDownload( QStringList& args,
					     const QString& downLoadOptions,
					     const Context& ctx,
					     const engines::engine& engine )
{
	auto m = ctx.TabManager().Configure().engineDefaultDownloadOptions( engine.name() ) ;

	auto ee = utility::listOptionsFromDownloadOptions( m ) ;

	const auto& mm = ctx.Engines().networkProxy() ;

	if( mm.isSet() ){

		engines::engine::baseEngine::optionsEnvironment m ;

		engine.setProxySetting( m,args,mm.networkProxyString() ) ;
	}

	if( !ee.isEmpty() ){

		args = args + ee ;
	}

	auto ss = args + utility::listOptionsFromDownloadOptions( downLoadOptions ) ;

	for( int i = static_cast< int >( ss.size() ) - 2 ; i > -1 ; i-- ){

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
	if( e.trimmed().isEmpty() || e.contains( "[download] " ) ){

		return false ;
	}else{
		return true ;
	}
}

void utility::contextMenuForDirectUrl( std::vector< UrlLinks > links,
				      QMenu& m,
				      const QJsonObject& obj,
				      const Context& ctx )
{	
	class mediaPlayerActions
	{
	public:
		mediaPlayerActions( std::vector< utility::UrlLinks > l,
				   const QJsonObject& obj,
				   QMenu& m,
				   const Context& ctx ) :
			m_obj( obj ),
			m_links( std::move( l ) ),
			m_menu( m ),
			m_mediaPlayer( ctx.Settings().openWith( ctx.logger() ) ),
			m_ctx( ctx )
		{
			if( m_links.size() == 0 ){

				this->disable() ;
			}else{
				this->setClipboard() ;
				this->setOpenUrl() ;
			}
		}
		void disable()
		{
			auto ac = m_menu.addAction( QObject::tr( "Copy Url" ) ) ;

			ac->setEnabled( false ) ;

			auto mm = QObject::tr( "Open Url With %1" ) ;

			for( const auto& e : m_mediaPlayer.opts() ){

				auto s = mm.arg( e.name ) ;

				m_menu.addAction( s )->setEnabled( false ) ;
			}
		}
		void setClipboard()
		{
			auto cpb = QApplication::clipboard() ;

			if( !cpb ){

				return ;
			}

			class cb
			{
			public:
				cb( const QString& m,QClipboard& cb ) :
					m_url( m ),
					m_clipboard( cb )
				{
				}
				void operator()()
				{
					m_clipboard.setText( m_url ) ;
				}
			private:
				QString m_url ;
				QClipboard& m_clipboard ;
			} ;

			auto act = &QAction::triggered ;

			if( m_links.size() == 1 ){

				auto ee = m_menu.addAction( QObject::tr( "Copy Url" ) ) ;

				auto ss = m_links[ 0 ].toList().join( " " ) ;

				QObject::connect( ee,act,cb( ss,*cpb ) ) ;
			}else{
				for( size_t i = 0 ; i < m_links.size() ; i++ ){

					auto e = QString::number( i + 1 ) ;

					auto s = QObject::tr( "Copy Url %1" ).arg( e ) ;

					auto ee = m_menu.addAction( s ) ;

					auto mm = m_links[ i ].toList().join( " " ) ;

					QObject::connect( ee,act,cb( mm,*cpb ) ) ;
				}
			}
		}
		void setOpenUrl()
		{
			auto act = &QAction::triggered ;

			const auto& adp = m_ctx.Settings().appDataPath() ;

			if( m_links.size() == 1 ){

				auto mm = QObject::tr( "Open Url With %1" ) ;

				for( const auto& e : m_mediaPlayer.opts() ){

					auto s = mm.arg( e.name ) ;

					auto ee = m_menu.addAction( s ) ;

					this->setEnabled( ee,e ) ;

					auto ss = m_links[ 0 ].toList() ;

					auto ac = m_mediaPlayer.ac( ss,e,adp,m_obj ) ;

					QObject::connect( ee,act,ac.move() ) ;
				}
			}else{
				for( const auto& a : m_mediaPlayer.opts() ){

					auto mm = QObject::tr( "Open Urls With %2" ) ;

					auto s = mm.arg( a.name ) ;

					auto ee = m_menu.addAction( s ) ;

					this->setEnabled( ee,a ) ;

					QStringList ss ;

					for( size_t i = 0 ; i < m_links.size() ; i++ ){

						ss.append( m_links[ i ].toList() ) ;
					}

					auto ac = m_mediaPlayer.ac( ss,a,adp,m_obj ) ;

					QObject::connect( ee,act,ac.move() ) ;
				}
			}
		}
	private:
		void setEnabled( QAction * ac,const settings::mediaPlayer::PlayerOpts& e )
		{
			if( utility::platformisFlatPak() ){

				if( e.name == "VLC" ){

					auto m = m_ctx.Settings().flatpakIntance().getVLC().valid() ;
					ac->setEnabled( m ) ;
				}
			}
		}
		const QJsonObject& m_obj ;
		std::vector< utility::UrlLinks > m_links ;
		QMenu& m_menu ;
		settings::mediaPlayer m_mediaPlayer ;
		const Context& m_ctx ;
	} ;

	mediaPlayerActions( std::move( links ),obj,m,ctx ) ;
}

void utility::deleteTmpFiles( const QString& df,std::vector< QByteArray > files )
{
	class meaw
	{
	public:
		meaw( const QString& df,std::vector< QByteArray > files ) :
			m_df( df ),m_files( std::move( files ) )
		{
		}
		void operator()()
		{
			for( const auto& it : m_files ){

				auto m = m_df + "/" + it ;

				QFile::remove( m + ".part" ) ;
				QFile::remove( m ) ;
			}
		}
	private:
		QString m_df ;
		std::vector< QByteArray > m_files ;
	} ;

	utils::qthread::run( meaw( df,std::move( files ) ) ) ;
}

bool utility::Qt6Version()
{
#if QT_VERSION < QT_VERSION_CHECK( 6,0,0 )
	return false ;
#else
	return true ;
#endif
}

bool utility::Qt5Version()
{
	return !utility::Qt6Version() ;
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
	static auto m = !QProcessEnvironment::systemEnvironment().value( "FLATPAK_ID" ).isEmpty() ;

	return m ;
}

bool utility::platformIsAppImage()
{
	static auto m = !QProcessEnvironment::systemEnvironment().value( "APPIMAGE" ).isEmpty() ;

	return m ;
}

quint64 utility::simpleRandomNumber()
{
	return static_cast< quint64 >( time( nullptr ) ) ;
}

QString utility::rename( const Context& ctx,
			QTableWidgetItem& item,
			const QString& cwd,
			const QString& newName,
			const QString& oldName )
{
	Logger& logger = ctx.logger() ;

	auto oldPath = cwd + "/" + oldName ;
	auto newPath = cwd + "/" + newName ;

	auto id = utility::loggerID() ;

	fileRename rename( oldPath,newPath ) ;

	auto e = QObject::tr( "Renaming \"%1\" to \"%2\"" ) ;

	auto bar = utility::barLine() ;

	logger.add( bar,id ) ;

	logger.add( e.arg( rename.oldPath(),rename.newPath() ),id ) ;

	if( rename.exec() ){

		auto txt = item.text() ;

		txt.replace( oldName,newName ) ;

		item.setText( txt ) ;

		logger.add( bar,id ) ;

		return newName ;
	}else{
		auto s = rename.errorString() ;

		ctx.logger().add( QObject::tr( "Renaming Failed: %3" ).arg( s ),id ) ;

		logger.add( bar,id ) ;

		return {} ;
	}
}

template< typename Type,typename ... Args >
QString FileSystemOperation( Args&& ... args )
{
	Type m( std::forward< Args >( args ) ... ) ;

	if( m.exec() ){

		return {} ;
	}else{
		return m.errorString() ;
	}
}

QString utility::rename( const QString& oldName,const QString& newName )
{
	return FileSystemOperation< fileRename >( oldName,newName ) ;
}

QString utility::removeFile( const QString& e )
{
	return FileSystemOperation< fileRemove >( e ) ;
}

QString utility::removeFolder( const QString& e )
{
	return FileSystemOperation< dirRemove >( e ) ;
}

bool utility::containsLinkerWarning( const QByteArray& e )
{
	return e.contains( "ERROR: ld.so: object" ) || e.contains( "RequestsDependencyWarning:" ) ;
}

void utility::copyToClipboardUrls( tableWidget& table )
{
	QStringList list ;

	auto lastColumn = table.columnCount() - 1 ;

	for( int row = 0 ; row < table.rowCount() ; row++ ){

		if( table.item( row,lastColumn ).isSelected() ){

			list.append( table.url( row ) ) ;
		}
	}

	auto m = QApplication::clipboard() ;

	if( m ){

		m->setText( list.join( "\n" ) ) ;
	}
}

bool utility::fileIsInvalidForGettingThumbnail( const QByteArray& e )
{
	return e.endsWith( ".mp4" ) || e.endsWith( ".webm" ) || e.endsWith( ".avi" ) ;
}

utility::CPU::CPU() : m_cpu( utility::CPU::getCPU() )
{
}

bool utility::CPU::x86_32() const
{
	if( utility::local::pretendPlatform().is32Bit() ){

		return true ;
	}else{
		return m_cpu == "i386" || m_cpu == "x86_32" ;
	}
}

bool utility::CPU::x86_64() const
{
	return m_cpu == "x86_64" ;
}

bool utility::CPU::aarch64() const
{
	return m_cpu == "arm64" || m_cpu == "aarch64";
}

bool utility::CPU::aarch32() const
{
	return m_cpu == "arm" || m_cpu == "aarch32" ;
}

const QString& utility::CPU::getCPU()
{
#if QT_VERSION >= QT_VERSION_CHECK( 5,4,0 )
	static QString m = QSysInfo::currentCpuArchitecture() ;
	return m ;
#else
	static QString m = utility::CPU::getCPUNatively() ;
	return m ;
#endif
}

QByteArray utility::CPU::getCPUNatively()
{
	if( utility::platformIsLinux() ){

		QFile file( "/proc/sys/kernel/arch" ) ;

		if( file.open( QIODevice::ReadOnly ) ){

			return file.readAll().trimmed() ;
		}
	}

	return QByteArray() ;
}

void utility::setCookieOption( QStringList& opts,settings& s,const engines::engine& engine )
{
	const auto& ca = engine.cookieArgument() ;
	const auto& cv = s.cookieBrowserName( engine.name() ) ;

	if( !ca.isEmpty() && !cv.isEmpty() ){

		opts.append( ca ) ;
		opts.append( cv ) ;
	}

	auto cookieFile = s.cookieBrowserTextFilePath( engine.name() ) ;
	const auto& caa = engine.cookieTextFileArgument() ;

	if( !cookieFile.isEmpty() && !caa.isEmpty() ){

		opts.append( caa ) ;
		opts.append( cookieFile ) ;
	}
}

void utility::impl::qJsonArrJoin( QJsonArray& )
{
}

void utility::archiveData::addToHistory( QJsonObject obj )
{
	class meaw
	{
	public:
		meaw( const Context& ctx,QJsonObject obj ) : m_ctx( ctx ),m_obj( std::move( obj ) )
		{
		}
		void bg()
		{
			utility::archiveData::guardHistoryFile() ;
			this->updateHistory() ;
			utility::archiveData::unGuardHistoryFile() ;
		}
		void fg()
		{
		}
	private:
		bool historyFound( const QString& path,const QString& url )
		{
			QFile file( path ) ;

			if( file.open( QIODevice::ReadOnly ) ){

				auto m = file.map( 0,file.size() ) ;

				if( m ){

					auto t = reinterpret_cast< char * >( m ) ;

					auto s = QByteArray::fromRawData( t,file.size() ) ;

					bool e = s.contains( url.toUtf8() ) ;

					file.unmap( m ) ;

					return e ;
				}else{
					return false ;
				}
			}else{
				return false ;
			}
		}
		void updateHistory()
		{
			const auto& e = m_ctx.Engines().engineDirPaths().downloadHistoryFilePath() ;

			auto url = m_obj.value( "Url" ).toString() ;

			if( !this->historyFound( e,url ) ){

				QFile file( e ) ;

				if( file.open( QIODevice::WriteOnly | QIODevice::Append ) ){

					auto s = QJsonDocument::JsonFormat::Indented ;

					auto m = QJsonDocument( m_obj ).toJson( s ) ;

					file.write( m ) ;
				}
			}
		}
		const Context& m_ctx ;
		QJsonObject m_obj ;
	} ;

	utils::qthread::run( meaw( m_ctx,std::move( obj ) ) ) ;
}

QByteArray utility::archiveData::logHistoryData( const Context& ctx )
{
	QFile file( ctx.Engines().engineDirPaths().downloadHistoryFilePath() ) ;

	utility::archiveData::guardHistoryFile() ;

	QByteArray data ;

	if( file.open( QIODevice::ReadOnly ) ){

		data = file.readAll() ;

		file.close() ;
	}

	utility::archiveData::unGuardHistoryFile() ;

	return data ;
}

utility::archiveData::archiveData( QStringList opts,const engines::engine& engine,const Context& ctx ) :
	m_options( std::move( opts ) ),
	m_ctx( ctx )
{
	auto downloadArchivePath = m_ctx.Engines().engineDirPaths().archiveFilePath( engine ) ;

	utility::arguments Opts( m_options ) ;

	m_maxMediaLength = Opts.hasValue( "--max-media-length",true ) ;
	m_minMediaLength = Opts.hasValue( "--min-media-length",true ) ;

	m_breakOnExisting = Opts.hasOption( "--break-on-existing",true ) ;
	m_skipOnExisting  = Opts.hasOption( "--skip-on-existing",true ) ;

	if( m_breakOnExisting || m_skipOnExisting ){

		if( ctx.Settings().useInternalArchiveFile() ){

			QFile file( downloadArchivePath ) ;

			if( file.open( QIODevice::ReadOnly ) ){

				m_archiveFileData = file.readAll() ;
			}
		}

		auto s = ctx.TabManager().Configure().engineDefaultDownloadOptions( engine.name() ) ;

		auto ss = util::splitPreserveQuotes( s ) ;

		auto mm = utility::arguments( ss ).hasValue( "--download-archive" ) ;

		if( !mm.isEmpty() && QFile::exists( mm ) ){

			QFile file( mm ) ;

			if( file.open( QIODevice::ReadOnly ) ){

				m_archiveFileData.append( "\n" + file.readAll() ) ;
			}
		}
	}
}

bool utility::archiveData::contains( const QString& e ) const
{
	if( m_archiveFileData.isEmpty() ){

		return false ;
	}else{
		return m_archiveFileData.contains( e.toUtf8() ) ;
	}
}

QMutex utility::archiveData::m_mutex ;

void utility::archiveData::guardHistoryFile()
{
	utility::archiveData::m_mutex.lock() ;
}

void utility::archiveData::unGuardHistoryFile()
{
	utility::archiveData::m_mutex.unlock() ;
}

void utility::addtoHistory( const engines::engine& engine,
			    const Context& ctx,
			    const QString& tabName,
			    const tableWidget::entry& e )
{
	if( ctx.Settings().saveDownloadHistory() ){

		QJsonObject obj ;

		obj.insert( "EngineName",engine.name() ) ;
		obj.insert( "TabName",tabName ) ;
		obj.insert( "Url",e.url ) ;
		obj.insert( "Id",e.videoId ) ;
		obj.insert( "Title",e.title ) ;
		obj.insert( "DownloadDate",QDateTime::currentDateTime().toString() ) ;

		if( e.videoId.isEmpty() ) {

			auto yt = "https://www.youtube.com/watch?v=" ;

			if( e.url.startsWith( yt ) ){

				auto m = e.url ;
				obj.insert( "Id",m.replace( yt,"" ) ) ;
			}
		}

		utility::archiveData( {},engine,ctx ).addToHistory( std::move( obj ) ) ;
	}
}

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

#include <QEventLoop>
#include <QDesktopServices>

const char * utility::selectedAction::CLEAROPTIONS = "Clear Options" ;
const char * utility::selectedAction::CLEARSCREEN  = "Clear Screen" ;
const char * utility::selectedAction::OPENFOLDER   = "Open Download Folder" ;

QStringList utility::split( const QString& e,char token,bool skipEmptyParts )
{
	if( skipEmptyParts ){
		#if QT_VERSION < QT_VERSION_CHECK( 5,15,0 )
			return e.split( token,QString::SkipEmptyParts ) ;
		#else
			return e.split( token,Qt::SkipEmptyParts ) ;
		#endif
	}else{
		return e.split( token ) ;
	}
}

QStringList utility::split( const QString& e,const char * token )
{
#if QT_VERSION < QT_VERSION_CHECK( 5,15,0 )
	return e.split( token,QString::SkipEmptyParts ) ;
#else
	return e.split( token,Qt::SkipEmptyParts ) ;
#endif
}

QList< QByteArray > utility::split( const QByteArray& e,char token )
{
	return e.split( token ) ;
}

QList<QByteArray> utility::split( const QByteArray& e,QChar token )
{
	return e.split( token.toLatin1() ) ;
}

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

int utility::terminateProcess( unsigned long )
{
	return 0 ;
}

QString utility::python3Path()
{
	return QStandardPaths::findExecutable( "python3" ) ;
}

bool utility::platformIs32BitWindows()
{
	return false ;
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

int utility::terminateProcess( unsigned long )
{
	return 0 ;
}

bool utility::platformIs32BitWindows()
{
	return false ;
}

#endif

#ifdef Q_OS_WIN

#include <windows.h>

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

int utility::terminateProcess( unsigned long pid )
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

bool utility::platformIsNOTWindows()
{
	return !utility::platformIsWindows() ;
}

static void _add( QMenu * menu,const QStringList& args )
{
	for( const auto& it : args ){

		auto a = it ;

		a.replace( "Best-audiovideo(",QObject::tr( "Best-audiovideo" ) + "(" ) ;
		a.replace( "Best-audio(",QObject::tr( "Best-audio" ) + "(" ) ;

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
}

QMenu * utility::details::sMo( const Context& ctx,
			       const QStringList& opts,
			       bool addClear,
			       QPushButton * w )
{
	auto m = w->menu() ;

	if( m ){

		m->deleteLater() ;
	}

	auto menu = new QMenu( w ) ;

	auto& translator = ctx.Translator() ;
	auto& settings = ctx.Settings() ;

	translator::entry ss( QObject::tr( "Preset Options" ),"Preset Options","Preset Options" ) ;
	auto ac = translator.addAction( menu,std::move( ss ) ) ;

	ac->setEnabled( false ) ;

	menu->addSeparator() ;

	_add( menu,settings.presetOptionsList() ) ;

	if( !opts.empty() ){

		menu->addSeparator() ;

		translator::entry ss( QObject::tr( "Found Options" ),"Found Options","Found Options" ) ;

		_add( translator.addMenu( menu,std::move( ss ) ),opts ) ;
	}

	if( addClear ){

		menu->addSeparator() ;

		translator::entry ss( QObject::tr( "Clear Options" ),
						   utility::selectedAction::CLEAROPTIONS,
						   utility::selectedAction::CLEAROPTIONS ) ;

		translator.addAction( menu,std::move( ss ) ) ;

		translator::entry sx( QObject::tr( "Clear Screen" ),
						   utility::selectedAction::CLEARSCREEN,
						   utility::selectedAction::CLEARSCREEN ) ;

		translator.addAction( menu,std::move( sx ) ) ;

		translator::entry mm( QObject::tr( "Open Download Folder" ),
						   utility::selectedAction::OPENFOLDER,
						   utility::selectedAction::OPENFOLDER ) ;

		translator.addAction( menu,std::move( mm ) ) ;
	}

	w->setMenu( menu ) ;

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
	utility::wait( 1 ) ;
}

void utility::wait( int time )
{
	QEventLoop e ;

	utility::Timer( 1,[ & ]( int counter ){

		if( counter == time ){

			e.exit() ;
			return true ;
		}else{
			return false ;
		}
	} ) ;

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

void utility::terminateProcess( QProcess& exe )
{
	if( utility::platformIsWindows() ){

		QStringList args{ "-T",QString::number( exe.processId() ) } ;

		QProcess::startDetached( "media-downloader.exe",args ) ;
	}else{
		exe.terminate() ;
	}
}

QStringList utility::updateOptions( const engines::engine& engine,
				    const utility::args& args,
				    const QStringList& urls )
{
	auto opts = engine.defaultDownLoadCmdOptions() ;

	for( const auto& it : args.otherOptions ){

		opts.append( it ) ;
	}

	auto url = urls ;

	engine.updateDownLoadCmdOptions( args.quality,args.otherOptions,url,opts ) ;

	opts.append( url ) ;

	return opts ;
}

void utility::setTableWidget( QTableWidget& m,const utility::tableWidgetOptions& s )
{
	m.verticalHeader()->setSectionResizeMode( QHeaderView::ResizeToContents ) ;

	m.verticalHeader()->setMinimumSectionSize( 30 ) ;

	m.horizontalHeader()->setStretchLastSection( true ) ;

	m.setMouseTracking( s.mouseTracking ) ;

	m.setContextMenuPolicy( s.customContextPolicy ) ;

	m.setEditTriggers( s.editTrigger ) ;
	m.setFocusPolicy( s.focusPolicy ) ;
	m.setSelectionMode( s.selectionMode ) ;
}

int utility::concurrentID()
{
	static int id = -1 ;

	id++ ;

	return id ;
}

void utility::addItem( QTableWidget& table,const QString& text,const QFont& font,int alignment )
{
	utility::addItem( table,QStringList{ text },font,alignment ) ;
}

void utility::clear( QTableWidget& table )
{
	int m = table.rowCount() ;

	for( int i = 0 ; i < m ; i++ ){

		table.removeRow( 0 ) ;
	}
}

void utility::addItem( QTableWidget& table,const QStringList& text,const QFont& font,int alignment )
{
	auto row = table.rowCount() ;
	auto columns = table.columnCount() ;

	table.insertRow( row ) ;

	for( int it = 0 ; it < columns ; it++ ){

		auto item = new QTableWidgetItem() ;

		item->setText( text.at( it ) ) ;
		item->setTextAlignment( alignment ) ;
		item->setFont( font ) ;

		table.setItem( row,it,item ) ;
	}
}

/*
 *
 *  Copyright (c) 2026
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

#include "windows.h"

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
#include <QDir>

QString windows::applicationDirPath()
{
	std::array< wchar_t,4096 > buffer ;

	auto e = GetModuleFileNameW( nullptr,buffer.data(),static_cast< DWORD >( buffer.size() ) ) ;

	if( e > 0 ){

		auto a = QString::fromWCharArray( buffer.data(),e ) ;

		auto m = QDir::fromNativeSeparators( a ) ;
		auto s = m.lastIndexOf( '/' ) ;

		if( s != -1 ){

			m.truncate( s ) ;
		}

		return m ;
	}else{
		return {} ;
	}
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

QString windows::gateWayAddress()
{
	return adaptorInfo().address() ;
}

QString windows::getClipBoardText( const ContextWinId& wId )
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

	auto format = utility::Qt6Version() ? CF_UNICODETEXT : CF_TEXT ;

	if( IsClipboardFormatAvailable( format ) ){

		if( OpenClipboard( wId.value() ) ){

			auto hglb = GetClipboardData( format ) ;

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

void windows::setDarkModeTitleBar( const Context& ctx )
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

namespace windows
{
namespace local
{
std::vector< utility::PlayerOpts > getMediaPlayers( REGSAM wow )
{
	class buffer
	{
	public:
		buffer()
		{
			m_buffer[ 0 ] = L'\0' ;
		}
		DWORD * size()
		{
			return &m_size ;
		}
		bool valid() const
		{
			if( m_buffer[ 0 ] == L'\0' ){

				return false ;

			}else if( this->equal( "potplayer" ) ){

				return true ;

			}if( this->equal( ".mp4" ) || this->equal( ".MP4" ) ){

				return false ;
			}else{
				return this->endsWith( ".mp4" ) || this->endsWith( ".MP4" ) ;
			}
		}
		wchar_t * data()
		{
			return m_buffer.data() ;
		}
		const wchar_t * data() const
		{
			return m_buffer.data() ;
		}
		QString qdata() const
		{
			return this->string() ;
		}
	private:
		bool equal( const QString& e ) const
		{
			return this->string() == e ;
		}
		bool endsWith( const QString& e ) const
		{
			return this->string().endsWith( e ) ;
		}
		QString string() const
		{
			auto m = static_cast< qsizetype >( m_size ) ;
			return QString::fromWCharArray( m_buffer.data(),m ) ;
		}
		std::array< wchar_t,4096 > m_buffer ;
		DWORD m_size = 4096 ;
	} ;

	class Hkey
	{
	public:
		Hkey( Hkey& hkey,const buffer& subKey ) :
		    m_regSam( hkey.regSam() ),
		    m_status( this->open( hkey,subKey.data() ) )
		{
		}
		Hkey( REGSAM r ) :
		    m_regSam( r ),
		    m_status( this->open( HKEY_CLASSES_ROOT,nullptr ) )
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

			auto st = RegQueryInfoKeyW( m_key,N,N,N,&keyCount,N,N,N,N,N,N,N ) ;

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

			auto path = L"shell\\open\\command" ;

			auto st = RegGetValueW( m_key,path,N,RRF_RT_REG_SZ,N,subKey.data(),subKey.size() ) ;

			if( st == ERROR_SUCCESS ){

				return subKey.qdata() ;
			}else{
				return {} ;
			}
		}
		buffer getSubKey( DWORD i )
		{
			auto N = nullptr ;

			buffer subKey ;

			auto st = RegEnumKeyExW( m_key,i,subKey.data(),subKey.size(),N,N,N,N ) ;

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
		LSTATUS open( HKEY hkey,const wchar_t * subKey )
		{
			DWORD x = 0 ;

			return RegOpenKeyExW( hkey,subKey,x,m_regSam,&m_key ) ;
		}
		REGSAM m_regSam ;
		HKEY m_key = nullptr ;
		LSTATUS m_status ;
	} ;

	Hkey rootKey( wow ) ;

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

		auto ss = key.getExePath() ;

		if( ss.isEmpty() ){

			continue ;
		}

		QStringList p ;

		if( ss.startsWith( "\"" ) ){

			p = util::splitPreserveQuotes( ss ) ;
		}else{
			auto e = ss.indexOf( ".exe" ) ;

			if( e != -1 ){

				auto m = ss.mid( 0,e + 4 ) ;

				if( QFile::exists( m ) ){

					p.append( m ) ;
				}
			}
		}

		if( p.size() ){

			auto m = p.first() ;

			if( m.endsWith( "wmplayer.exe" ) ){

				s.emplace_back( m,"Windows Media Player" ) ;
			}else{
				auto na = util::split( subKey.qdata(),"." ) ;

				auto e = na.first() ;

				if( e.size() ){

					if( !e[ 0 ].isUpper() ){

						e[ 0 ] = e[ 0 ].toUpper() ;
					}
				}

				s.emplace_back( m,e ) ;
			}
		}
	}

	return s ;
}

static void add_entry( std::vector< utility::PlayerOpts >& a,utility::PlayerOpts& b )
{
	for( const auto& it : a ){

		if( it.name == b.name ){

			return ;
		}
	}

	a.emplace_back( std::move( b ) ) ;
}

}
}

std::vector< utility::PlayerOpts > windows::getMediaPlayers()
{
	auto a = windows::local::getMediaPlayers( KEY_READ | KEY_WOW64_64KEY ) ;
	auto b = windows::local::getMediaPlayers( KEY_READ | KEY_WOW64_32KEY ) ;

	for( auto& it : b ){

		windows::local::add_entry( a,it ) ;
	}

	return a ;
}

#if QT_VERSION >= QT_VERSION_CHECK( 6,6,0 )

void windows::enableCheckenableCheckPermissions()
{
}

void windows::disabdisableCheckPermissions()
{
}

#else

extern Q_CORE_EXPORT int qt_ntfs_permission_lookup ;

void windows::enableCheckPermissions()
{
	qt_ntfs_permission_lookup++ ;
}

void windows::disableCheckPermissions()
{
	qt_ntfs_permission_lookup-- ;
}

#endif

QString windows::errorMessage()
{
	struct meaw
	{
		char * s = nullptr ;
		~meaw()
		{
			LocalFree( s ) ;
		}
	} m ;

	auto a = FORMAT_MESSAGE_FROM_SYSTEM ;
	auto b = FORMAT_MESSAGE_IGNORE_INSERTS ;
	auto c = FORMAT_MESSAGE_ALLOCATE_BUFFER ;

	auto flags = a | b | c ;

	auto le = GetLastError() ;
	auto lg = MAKELANGID( LANG_NEUTRAL,SUBLANG_DEFAULT ) ;

	FormatMessageA( flags,nullptr,le,lg,reinterpret_cast< char * >( &m.s ),0,nullptr ) ;

	return m.s ;
}

#else

QString windows::applicationDirPath()
{
	return {} ;
}

QString windows::gateWayAddress()
{
	return {} ;
}

QString windows::getClipBoardText( const ContextWinId& )
{
	return {} ;
}

void windows::setDarkModeTitleBar( const Context& )
{
}

std::vector<utility::PlayerOpts> windows::getMediaPlayers()
{
	return {} ;
}

QString windows::errorMessage()
{
	return {} ;
}

void windows::enableCheckPermissions()
{
}

void windows::disableCheckPermissions()
{
}

#endif

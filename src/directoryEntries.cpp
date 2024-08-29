/*
 *
 *  Copyright (c) 2023
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

#include "directoryEntries.h"

#include "utils/miscellaneous.hpp"

#include <QDir>

#include <cstring>
#include <cwchar>

#ifdef Q_OS_WIN

#include <windows.h>

class dManager
{
public:
	dManager( const QString& path,std::atomic_bool& c ) :
		m_path( this->setPath( path ) ),m_continue( c )
	{
		m_continue = true ;
	}
	void removeDirectoryContents()
	{
		this->removeDirectory( m_path,false ) ;
	}
	void removeDirectory()
	{
		this->removeDirectory( m_path,true ) ;
	}
	directoryEntries readAll()
	{
		handle h( m_path ) ;

		if( h.valid() ){

			directoryEntries entries ;

			this->add( entries,h.data() ) ;

			while( m_continue && this->read( entries,h ) ){}

			return entries ;
		}else{
			return {} ;
		}
	}
private:
	std::wstring setPath( const QString& path )
	{
		auto e = QDir::cleanPath( path.startsWith( "\\\\?\\" ) ? path : "\\\\?\\" + path ) ;
		auto m = QDir::toNativeSeparators( e ) ;
		return m.toStdWString() ;
	}
	class handle
	{
	public:
		handle( std::wstring s )
		{
			if( *s.rbegin() == L'\\' ){

				s += L"*" ;
			}else{
				s += L"\\*" ;
			}

			m_handle = FindFirstFileW( s.data(),&m_data ) ;
		}
		bool valid()
		{
			return m_handle != INVALID_HANDLE_VALUE ;
		}
		bool findNext()
		{
			return FindNextFileW( m_handle,&m_data ) != 0 ;
		}
		HANDLE get()
		{
			return m_handle ;
		}
		const WIN32_FIND_DATAW& data()
		{
			return m_data ;
		}
		~handle()
		{
			FindClose( m_handle ) ;
		}
	private:
		WIN32_FIND_DATAW m_data ;
		HANDLE m_handle ;
	};
	void removePath( const std::wstring& w,const wchar_t * name,const WIN32_FIND_DATAW& data )
	{
		if( std::wcscmp( name,L"." ) != 0 && std::wcscmp( name,L".." ) != 0 ){

			auto m = w + L'\\' + name ;

			if( data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ){

				this->removeDirectory( m,true ) ;
			}else{
				DeleteFileW( m.data() ) ;
			}
		}
	}
	void removeDirectory( const std::wstring& w,bool removeDirectory )
	{
		handle h( w ) ;

		if( h.valid() ){

			const auto& mm = h.data() ;

			this->removePath( w,mm.cFileName,mm ) ;

			while( m_continue ){

				if( h.findNext() ){

					const auto& m = h.data() ;

					this->removePath( w,m.cFileName,m ) ;
				}else{
					break ;
				}
			}

			if( removeDirectory ){

				RemoveDirectoryW( w.data() ) ;
			}
		}
	}
	void add( directoryEntries& entries,const WIN32_FIND_DATAW& data )
	{
		auto m = data.cFileName ;

		if( entries.valid( m ) ){

			LARGE_INTEGER filesize ;

			filesize.LowPart = data.ftCreationTime.dwLowDateTime ;
			filesize.HighPart = data.ftCreationTime.dwHighDateTime ;

			if( data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ){

				entries.addFolder( filesize.QuadPart,QString::fromWCharArray( m ) ) ;
			}else{
				entries.addFile( filesize.QuadPart,QString::fromWCharArray( m ) ) ;
			}
		}
	}
	bool read( directoryEntries& entries,handle& h )
	{
		if( h.findNext() ){

			this->add( entries,h.data() ) ;

			return true ;
		}else{
			return false ;
		}
	}
	std::wstring m_path ;
	std::atomic_bool& m_continue ;
} ;

#else

#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cstring>
#include <string>
#include <limits.h>

class dManager
{
public:
	dManager( const QString& path,std::atomic_bool& c ) :
		m_path( path.toUtf8().constData() ),
		m_continue( c )
	{
		m_continue = true ;
	}
	directoryEntries readAll()
	{
		auto handle = utils::misc::unique_rsc( opendir,closedir,m_path.data() ) ;

		if( handle ){

			directoryEntries entries ;

			while( m_continue && this->read( entries,m_path,handle.get() ) ){}

			return entries ;
		}else{
			return {} ;
		}
	}
	void removeDirectoryContents()
	{
		this->removeDirectory( m_path,false ) ;
	}
	void removeDirectory()
	{
		this->removeDirectory( m_path,true ) ;
	}
private:
	void removeDirectory( const std::string& pm,bool removeFolder )
	{
		auto handle = utils::misc::unique_rsc( opendir,closedir,pm.data() ) ;

		if( handle ){

			while( m_continue ){

				auto e = readdir( handle.get() ) ;

				if( e ){

					this->removePath( pm,e->d_name ) ;
				}else{
					break ;
				}
			}

			if( removeFolder ){

				rmdir( pm.data() ) ;
			}
		}
	}
	void removePath( const std::string& pm,const char * name )
	{
		if( std::strcmp( name,"." ) != 0 && std::strcmp( name,".." ) != 0 ){

			struct stat m ;

			auto pp = pm + "/" + name ;

			if( lstat( pp.data(),&m ) == 0 ){

				if( S_ISDIR( m.st_mode ) ){

					this->removeDirectory( pp,true ) ;
				}else{
					unlink( pp.data() ) ;
				}
			}
		}
	}
	bool read( directoryEntries& entries,const std::string& mm,DIR * dir )
	{
		auto e = readdir( dir ) ;

		if( e ){

			const auto name = e->d_name ;

			if( entries.valid( name ) ){

				struct stat m ;

				auto s = mm + '/' + name ;

				if( stat( s.data(),&m ) == 0 ){

					if( S_ISREG( m.st_mode ) ){

						entries.addFile( m.st_ctime,name ) ;

					}else if( S_ISDIR( m.st_mode ) ){

						entries.addFolder( m.st_ctime,name ) ;
					}
				}
			}

			return true ;
		}else{
			return false ;
		}
	}
	std::string m_path ;
	std::atomic_bool& m_continue ;
} ;

#endif

bool directoryEntries::valid( const char * e )
{
	if( std::strcmp( e,".." ) == 0 || std::strcmp( e,"." ) == 0 ){

		return false ;
	}

	if( std::strncmp( e,"info_",5 ) == 0 ){

		return false ;
	}

	return true ;
}

bool directoryEntries::valid( const wchar_t * s )
{
	if( std::wcscmp( s,L".." ) == 0 || std::wcscmp( s,L"." ) == 0 ){

		return false ;
	}

	if( std::wcsncmp( s,L"info_",5 ) == 0 ){

		return false ;
	}

	return true ;
}

directoryEntries directoryManager::readAll( const QString& e )
{
	std::atomic_bool s ;

	return directoryManager::readAll( e,s ) ;
}

directoryEntries directoryManager::readAll( const QString& e,std::atomic_bool& s )
{
	return dManager( e,s ).readAll() ;
}

void directoryManager::removeDirectoryContents( const QString& e,std::atomic_bool& s )
{
	return dManager( e,s ).removeDirectoryContents() ;
}

void directoryManager::removeDirectory( const QString& e,std::atomic_bool& s )
{
	return dManager( e,s ).removeDirectory() ;
}


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

#ifndef DIRECTORY_ENTRIES_H
#define DIRECTORY_ENTRIES_H

#include <QString>
#include <QPushButton>
#include <QObject>
#include <QDir>

#include <atomic>
#include <vector>

#include <array>
#include <cstdio>
#include <cwchar>
#include <cstring>

#include <limits.h>

#include "utils/miscellaneous.hpp"

class directoryEntries
{
private:
	struct entry
	{
		entry( qint64 d,QString p ) :
			dateCreated( d ),
			path( std::move( p ) )
		{
		}
		qint64 dateCreated ;
		QString path ;
	} ;

	std::vector< entry > m_folders ;
	std::vector< entry > m_files ;
public:
	bool valid( const char * ) ;
	bool valid( const wchar_t * ) ;

	bool valid( const QString& ) ;
	void sort()
	{
		std::sort( m_folders.begin(),m_folders.end(),[]( const entry& lhs,const entry& rhs ){

			return rhs.dateCreated < lhs.dateCreated ;
		} ) ;

		std::sort( m_files.begin(),m_files.end(),[]( const entry& lhs,const entry& rhs ){

			return rhs.dateCreated < lhs.dateCreated ;
		} ) ;
	}
	void addFile( qint64 dateCreated,QString path )
	{
		m_files.emplace_back( dateCreated,std::move( path ) ) ;
	}
	void addFolder( qint64 dateCreated,QString path )
	{
		m_folders.emplace_back( dateCreated,std::move( path ) ) ;
	}

	class iter
	{
	public:
		iter()
		{
		}
		iter( const std::vector< directoryEntries::entry >& e ) :
			m_entries( &e )
		{
		}
		bool hasNext() const
		{
			return m_position < m_entries->size() ;
		}
		const QString& value() const
		{
			return m_entries->data()[ m_position ].path ;
		}
		const QString& valueWithNext()
		{
			return m_entries->data()[ m_position++ ].path ;
		}
		iter next() const
		{
			auto m = *this ;
			m.m_position++ ;
			return m ;
		}
	private:
		size_t m_position = 0 ;
		const std::vector< directoryEntries::entry > * m_entries = nullptr ;
	} ;

	directoryEntries::iter directoryIter()
	{
		return { m_folders } ;
	}
	directoryEntries::iter fileIter()
	{
		return { m_files } ;
	}
} ;

Q_DECLARE_METATYPE( directoryEntries::iter )

#ifdef Q_OS_LINUX

#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cstring>

class directoryManager
{
public:
	directoryManager( const QString& path,
			  std::atomic_bool& c,
			  QDir::Filters = QDir::Filter::Files | QDir::Filter::Dirs | QDir::Filter::NoDotAndDotDot ) :
		m_path( path ),
		m_continue( &c )
	{
		*m_continue = true ;
	}
	directoryManager( const QString& path,
			  QDir::Filters = QDir::Filter::Files | QDir::Filter::Dirs | QDir::Filter::NoDotAndDotDot ) :
		m_path( path )
	{
	}
	static bool supportsCancel()
	{
		return true ;
	}
	directoryEntries readAll()
	{
		auto handle = utils::misc::unique_rsc( opendir,closedir,m_path.toUtf8().constData() ) ;

		if( handle ){

			pathManager mm( m_path ) ;

			if( m_continue ){

				while( *m_continue && this->read( mm,handle.get() ) ){}
			}else{
				while( this->read( mm,handle.get() ) ){}
			}

			m_entries.sort() ;
		}

		return std::move( m_entries ) ;
	}
	void removeDirectoryContents()
	{
		this->removeDirectory( m_path,[](){} ) ;
	}
	void removeDirectory()
	{
		this->removeDirectory( m_path,[ & ](){ rmdir( m_path.toUtf8().constData() ) ; } ) ;
	}
private:
	class pathManager
	{
	public:
		pathManager( const char * path )
		{
			this->init( path ) ;
		}
		pathManager( const QString& path )
		{
			this->init( path ) ;
		}
		const char * setPath( const char * path )
		{
			this->append( m_basePathLocation,path ) ;

			return m_buffer.data() ;
		}
		const char * path()
		{
			return m_buffer.data() ;
		}
	private:
		void init( const QString& path )
		{
			if( path.endsWith( "/" ) ){

				this->append( 0,path.toUtf8() ) ;
				m_basePathLocation = path.size() ;
			}else{
				this->append( 0,path.toUtf8() + "/" ) ;
				m_basePathLocation = path.size() + 1 ;
			}
		}
		void append( std::size_t s,const char * data )
		{
			std::snprintf( m_buffer.data() + s,m_buffer.size() - s,"%s",data ) ;
		}
		std::size_t m_basePathLocation ;
		std::array< char,PATH_MAX > m_buffer ;
	} ;
	template< typename Function >
	void removeDirectory( pathManager pm,Function function )
	{
		auto handle = utils::misc::unique_rsc( opendir,closedir,pm.path() ) ;

		if( handle ){

			if( m_continue ){

				while( *m_continue ){

					auto e = readdir( handle.get() ) ;

					if( e ){

						this->removePath( pm,e->d_name ) ;
					}else{
						break ;
					}
				}
			}else{
				while( true ){

					auto e = readdir( handle.get() ) ;

					if( e ){

						this->removePath( pm,e->d_name ) ;
					}else{
						break ;
					}
				}
			}

			function() ;
		}
	}
	void removePath( pathManager& pm,const char * name )
	{
		if( std::strcmp( name,"." ) != 0 && std::strcmp( name,".." ) != 0 ){

			struct stat m ;

			auto pp = pm.setPath( name ) ;

			if( stat( pp,&m ) == 0 ){

				if( S_ISREG( m.st_mode ) ){

					unlink( pp ) ;

				}else if( S_ISDIR( m.st_mode ) ){

					this->removeDirectory( pp,[ pp ](){ rmdir( pp ) ; } ) ;
				}
			}
		}
	}
	bool read( pathManager& mm,DIR * dir )
	{
		auto e = readdir( dir ) ;

		if( e ){

			const auto name = e->d_name ;

			if( m_entries.valid( name ) ){

				struct stat m ;

				if( stat( mm.setPath( name ),&m ) == 0 ){

					if( S_ISREG( m.st_mode ) ){

						m_entries.addFile( m.st_ctime,name ) ;

					}else if( S_ISDIR( m.st_mode ) ){

						m_entries.addFolder( m.st_ctime,name ) ;
					}
				}
			}

			return true ;
		}else{
			return false ;
		}
	}
	QString m_path ;
	std::atomic_bool * m_continue = nullptr ;
	directoryEntries m_entries ;
} ;

#else

#ifdef Q_OS_WIN

#include <windows.h>

class directoryManager
{
public:
	directoryManager( const QString& path,
			  std::atomic_bool& m,
			  QDir::Filters = QDir::Filter::Files | QDir::Filter::Dirs | QDir::Filter::NoDotAndDotDot ) :
		m_path( path ),
		m_continue( &m )
	{
		*m_continue = true ;
	}
	directoryManager( const QString& path,
			  QDir::Filters = QDir::Filter::Files | QDir::Filter::Dirs | QDir::Filter::NoDotAndDotDot ) :
		m_path( path )
	{
	}
	static bool supportsCancel()
	{
		return true ;
	}
	void removeDirectoryContents()
	{
		this->removeDirectory( m_path,[](){} ) ;
	}
	void removeDirectory()
	{
		this->removeDirectory( m_path,[ & ](){ RemoveDirectoryW( toWChar( m_path ) ) ; } ) ;
	}
	directoryEntries readAll()
	{
		auto _read = [ this ]( handle& h ){

			if( h.findNext() ){

				this->add( h.data() ) ;

				return true ;
			}else{
				return false ;
			}
		} ;

		handle h( m_path ) ;

		if( h.valid() ){

			this->add( h.data() ) ;

			if( m_continue ){

				while( *m_continue && _read( h ) ){}
			}else{
				while( _read( h ) ){}
			}

			m_entries.sort() ;
		}

		return std::move( m_entries ) ;
	}
	~directoryManager()
	{
	}
private:
	class toWChar
	{
	public:
		toWChar( const QString& e )
		{
			std::fill( m_buffer.begin(),m_buffer.end(),L'\0' ) ;

			if( e.size() < int( m_buffer.size() - 1 ) ){

				e.toWCharArray( m_buffer.data() ) ;
			}else{
				auto m = e.mid( 0,m_buffer.size() - 1 ) ;

				m.toWCharArray( m_buffer.data() ) ;
			}
		}
		operator const wchar_t *()
		{
			return m_buffer.data() ;
		}
	private:
		std::array< wchar_t,PATH_MAX > m_buffer ;
	} ;
	class pathManager
	{
	public:
		pathManager( const wchar_t * p )
		{
			auto len = std::wcslen( p ) ;

			if( p[ len - 1 ] == L'/' ){

				this->append( 0,p ) ;
				m_basePathLocation = len ;
			}else{
				std::wstring path( p ) ;

				path += L"/" ;
				this->append( 0,path.data() ) ;
				m_basePathLocation = len + 1 ;
			}
		}
		pathManager( const QString& path )
		{
			if( path.endsWith( "/" ) ){

				this->append( 0,toWChar( path ) ) ;
				m_basePathLocation = path.size() ;
			}else{
				this->append( 0,toWChar( path + "/" ) ) ;
				m_basePathLocation = path.size() + 1 ;
			}
		}
		const wchar_t * setPath( const wchar_t * path )
		{
			this->append( m_basePathLocation,path ) ;

			return m_buffer.data() ;
		}
		const wchar_t * path()
		{
			return m_buffer.data() ;
		}
	private:
		void append( std::size_t s,const wchar_t * data )
		{
			std::swprintf( m_buffer.data() + s,m_buffer.size() - s,L"%ls",data ) ;
		}
		std::size_t m_basePathLocation ;
		std::array< wchar_t,PATH_MAX > m_buffer ;
	} ;
	class handle
	{
	public:
		handle( const wchar_t * s )
		{
			auto m = std::wstring( s ) ;

			if( m[ m.size() - 1 ] == L'/' ){

				m.erase( m.end() - 1 ) ;
			}

			m += L"\\*" ;

			this->init( m.data() ) ;
		}
		handle( const QString& s )
		{
			auto m = s ;

			if( m.endsWith( "/" ) ){

				m.truncate( m.size() - 1 ) ;
			}

			m += "\\*" ;

			this->init( toWChar( m ) ) ;
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
		void init( const wchar_t * s )
		{
			m_handle = FindFirstFileW( s,&m_data ) ;
		}
		WIN32_FIND_DATAW m_data ;
		HANDLE m_handle ;
	};
	void removePath( pathManager& pm,const wchar_t * name,const WIN32_FIND_DATAW& data )
	{
		if( std::wcscmp( name,L"." ) != 0 && std::wcscmp( name,L".." ) != 0 ){

			auto m = pm.setPath( name ) ;

			if( data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ){

				this->removeDirectory( m,[ &m ](){ RemoveDirectoryW( m ) ; } ) ;
			}else{
				DeleteFileW( m ) ;
			}
		}
	}
	template< typename Function >
	void removeDirectory( pathManager pm,Function function )
	{
		handle h( pm.path() ) ;

		if( h.valid() ){

			const auto& mm = h.data() ;

			this->removePath( pm,mm.cFileName,mm ) ;

			if( m_continue ){

				while( *m_continue ){

					if( h.findNext() ){

						const auto& m = h.data() ;

						this->removePath( pm,m.cFileName,m ) ;
					}else{
						break ;
					}
				}
			}else{
				while( true ){

					if( h.findNext() ){

						const auto& m = h.data() ;

						this->removePath( pm,m.cFileName,m ) ;
					}else{
						break ;
					}
				}
			}

			function() ;
		}
	}
	void add( const WIN32_FIND_DATAW& data )
	{
		auto m = data.cFileName ;

		if( m_entries.valid( m ) ){

			LARGE_INTEGER filesize ;

			filesize.LowPart = data.ftCreationTime.dwLowDateTime ;
			filesize.HighPart = data.ftCreationTime.dwHighDateTime ;

			if( data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ){

				m_entries.addFolder( filesize.QuadPart,QString::fromWCharArray( m ) ) ;
			}else{
				m_entries.addFile( filesize.QuadPart,QString::fromWCharArray( m ) ) ;
			}
		}
	}
	QString m_path ;
	directoryEntries m_entries ;
	std::atomic_bool * m_continue = nullptr ;
} ;

#else

#include <QDateTime>
#include <QString>
#include <QFileInfo>
#include <QDir>

class directoryManager
{
public:
	directoryManager( const QString& path,
			  std::atomic_bool&,
			  QDir::Filters f = QDir::Filter::Files | QDir::Filter::Dirs | QDir::Filter::NoDotAndDotDot ) :
		m_path( path ),
		m_list( QDir( m_path ).entryList( f ) )
	{
	}
	directoryManager( const QString& path,
			  QDir::Filters f = QDir::Filter::Files | QDir::Filter::Dirs | QDir::Filter::NoDotAndDotDot ) :
		m_path( path ),
		m_list( QDir( m_path ).entryList( f ) )
	{
	}
	void removeDirectory()
	{
		QDir( m_path ).removeRecursively() ;
	}
	void removeDirectoryContents()
	{
		QDir::Filters f = QDir::Filter::Files | QDir::Filter::Dirs | QDir::Filter::NoDotAndDotDot ;

		QFileInfo m ;

		QString s ;

		for( const auto& it : QDir( m_path ).entryList( f ) ){

			s = m_path + "/" + it ;

			m.setFile( s ) ;

			if( m.isFile() ){

				QFile::remove( s ) ;
			}else{
				QDir( s ).removeRecursively() ;
			}
		}
	}
	static bool supportsCancel()
	{
		return false ;
	}
	directoryEntries readAll()
	{
		while( this->read() ){}

		return std::move( m_entries ) ;
	}
private:
	bool read()
	{
		if( m_counter < m_list.size() ){

			const auto& m = m_list[ m_counter++ ] ;

			if( m_entries.valid( m ) ){

				auto w = QDir::fromNativeSeparators( m ) ;

				m_fileInfo.setFile( m_path + "/" + m ) ;

				if( m_fileInfo.isFile() ){

					m_entries.addFile( this->createdTime( m_fileInfo ),w ) ;

				}else if( m_fileInfo.isDir() ){

					m_entries.addFolder( this->createdTime( m_fileInfo ),w ) ;
				}
			}

			return true ;
		}else{
			return false ;
		}
	}
	qint64 createdTime( QFileInfo& e )
	{
	#if QT_VERSION >= QT_VERSION_CHECK( 5,10,0 )
		return e.birthTime().toMSecsSinceEpoch() ;
	#else
		return e.created().toMSecsSinceEpoch() ;
	#endif
	}
	qint64 m_counter = 0 ;
	QString m_path ;
	QStringList m_list ;
	QFileInfo m_fileInfo ;
	directoryEntries m_entries ;
} ;

#endif

#endif

#endif


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

	friend class iter ;

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

#include <array>

#include <cstdio>

#include <limits.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

class directoryManager
{
public:
	directoryManager( const QString& path,
			  std::atomic_bool& c,
			  QDir::Filters = QDir::Filter::Files | QDir::Filter::Dirs | QDir::Filter::NoDotAndDotDot ) :
		m_pathManager( path + "/" ),
		m_handle( opendir( path.toUtf8() ) ),
		m_continue( &c )
	{
	}
	directoryManager( const QString& path,
			  QDir::Filters = QDir::Filter::Files | QDir::Filter::Dirs | QDir::Filter::NoDotAndDotDot ) :
		m_pathManager( path + "/" ),
		m_handle( opendir( path.toUtf8() ) )
	{
	}
	bool valid()
	{
		return m_handle != nullptr ;
	}
	static bool supportsCancel()
	{
		return true ;
	}
	bool readFirst()
	{
		return !this->read() ;
	}
	bool Continue()
	{
		return *m_continue ;
	}
	directoryEntries readAll()
	{
		if( this->valid() ){

			while( !this->read() ){}
		}

		return this->entries() ;
	}
	directoryEntries entries()
	{
		m_entries.sort() ;
		return std::move( m_entries ) ;
	}
	bool read()
	{
		auto e = readdir( m_handle ) ;

		if( e ){

			const auto name = e->d_name ;

			if( m_entries.valid( name ) ){

				struct stat m ;

				if( stat( m_pathManager.setPath( name ),&m ) == 0 ){

					if( S_ISREG( m.st_mode ) ){

						m_entries.addFile( m.st_mtime,name ) ;

					}else if( S_ISDIR( m.st_mode ) ){

						m_entries.addFolder( m.st_mtime,name ) ;
					}
				}
			}

			return false ;
		}else{
			return true ;
		}
	}
	~directoryManager()
	{
		closedir( m_handle ) ;
	}
private:
	class pathManager
	{
	public:
		pathManager( const QString& path ) :
			m_basePathLocation( path.size() )
		{
			this->append( 0,path.toUtf8() ) ;
		}
		const char * setPath( const char * path )
		{
			this->append( m_basePathLocation,path ) ;

			return m_buffer.data() ;
		}
	private:
		void append( std::size_t s,const char * data )
		{
			std::snprintf( m_buffer.data() + s,m_buffer.size() - s,"%s",data ) ;
		}
		std::size_t m_basePathLocation ;
		std::array< char,PATH_MAX > m_buffer ;
	} ;

	pathManager m_pathManager ;
	DIR * m_handle ;
	std::atomic_bool * m_continue ;
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
		m_path( QDir::fromNativeSeparators( path ) + "\\*" ),
		m_continue( &m )
	{
	}
	directoryManager( const QString& path,
			  QDir::Filters = QDir::Filter::Files | QDir::Filter::Dirs | QDir::Filter::NoDotAndDotDot ) :
		m_path( QDir::fromNativeSeparators( path ) + "\\*" )
	{
	}
	bool valid()
	{
		return true ;
	}
	static bool supportsCancel()
	{
		return true ;
	}
	bool Continue()
	{
		return *m_continue ;
	}
	directoryEntries readAll()
	{
		if( this->readFirst() ){

			while( !this->read() ){}
		}

		return this->entries() ;
	}
	bool readFirst()
	{
		m_handle = FindFirstFileA( m_path.toUtf8(),&m_data ) ;

		if( m_handle == INVALID_HANDLE_VALUE ){

			return false ;
		}else{
			this->add() ;
			return true ;
		}
	}
	directoryEntries entries()
	{
		m_entries.sort() ;
		return std::move( m_entries ) ;
	}
	bool read()
	{
		if( FindNextFileA( m_handle,&m_data ) != 0 ){

			this->add() ;

			return false ;
		}else{
			return true ;
		}
	}
	~directoryManager()
	{
		FindClose( m_handle ) ;
	}
private:
	void add()
	{
		auto m = m_data.cFileName ;

		if( m_entries.valid( m ) ){

			m_filesize.LowPart = m_data.ftCreationTime.dwLowDateTime ;
			m_filesize.HighPart = m_data.ftCreationTime.dwHighDateTime ;

			if( m_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ){

				m_entries.addFolder( m_filesize.QuadPart,m ) ;
			}else{
				m_entries.addFile( m_filesize.QuadPart,m ) ;
			}
		}
	}
	QString m_path ;
	directoryEntries m_entries ;
	std::atomic_bool * m_continue ;

	WIN32_FIND_DATA m_data ;
	LARGE_INTEGER m_filesize ;
	HANDLE m_handle ;
} ;

#else

#include <QDateTime>
#include <QString>
#include <QFileInfo>

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
	bool valid()
	{
		return true ;
	}
	static bool supportsCancel()
	{
		return false ;
	}
	bool Continue()
	{
		return true ;
	}
	bool readFirst()
	{
		return !this->read() ;
	}
	directoryEntries entries()
	{
		m_entries.sort() ;
		return std::move( m_entries ) ;
	}
	directoryEntries readAll()
	{
		while( !this->read() ){}

		return this->entries() ;
	}
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

			return false ;
		}else{
			return true ;
		}
	}
private:
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

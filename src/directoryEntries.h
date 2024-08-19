
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
#include <QObject>

#include <atomic>
#include <vector>

class directoryEntries
{
private:
	class entry
	{
	public:
		entry( qint64 d,QString p,bool f ) :
			m_dateCreated( d ),
			m_path( std::move( p ) ),
			m_folder( f )
		{
		}
		bool isFolder() const
		{
			return m_folder ;
		}
		const QString& path() const
		{
			return m_path ;
		}
		qint64 dateCreated() const
		{
			return m_dateCreated ;
		}
	private:
		qint64 m_dateCreated ;
		QString m_path ;
		bool m_folder ;
	} ;
	class wrapper
	{
	public:
		wrapper( const directoryEntries::entry& it ) : m_entry( &it )
		{
		}
		const directoryEntries::entry * operator->() const
		{
			return m_entry ;
		}
	private:
		const directoryEntries::entry * m_entry ;
	} ;
	std::vector< directoryEntries::entry > m_folders ;
	std::vector< directoryEntries::entry > m_files ;
	std::vector< directoryEntries::wrapper > m_joined ;
public:
	template< typename Function >
	void forEachFile( Function function )
	{
		for( const auto& it : m_files ){

			function( it.path() ) ;
		}
	}
	enum class ICON{ FILE,FOLDER } ;
	directoryEntries move()
	{
		return std::move( *this ) ;
	}
	bool valid( const char * ) ;
	bool valid( const wchar_t * ) ;

	void clear()
	{
		m_folders.clear() ;
		m_files.clear() ;
	}
	void sortByDateAscending()
	{
		struct meaw
		{
			bool operator()( const entry& lhs,const entry& rhs )
			{
				return lhs.dateCreated() < rhs.dateCreated() ;
			}
		} ;

		this->sort( meaw() ) ;
	}
	void sortByDateDescending()
	{
		struct meaw
		{
			bool operator()( const entry& lhs,const entry& rhs )
			{
				return lhs.dateCreated() > rhs.dateCreated() ;
			}
		} ;

		this->sort( meaw() ) ;
	}
	void sortByNameAscending()
	{
		struct meaw
		{
			bool operator()( const entry& lhs,const entry& rhs )
			{
				return lhs.path().toLower() < rhs.path().toLower() ;
			}
		} ;

		this->sort( meaw() ) ;
	}
	void sortByNameDescending()
	{
		struct meaw
		{
			bool operator()( const entry& lhs,const entry& rhs )
			{
				return lhs.path().toLower() > rhs.path().toLower() ;
			}
		} ;

		this->sort( meaw() ) ;
	}
	template< typename sorter>
	void sort( sorter s )
	{
		std::sort( m_folders.begin(),m_folders.end(),s ) ;
		std::sort( m_files.begin(),m_files.end(),s ) ;
	}
	void addFile( qint64 dateCreated,QString path )
	{
		m_files.emplace_back( dateCreated,std::move( path ),false ) ;
	}
	void addFolder( qint64 dateCreated,QString path )
	{
		m_folders.emplace_back( dateCreated,std::move( path ),true ) ;
	}

	class iter
	{
	public:
		iter()
		{
		}
		iter( const std::vector< directoryEntries::wrapper >& e ) :
			m_entries( &e )
		{
		}
		bool hasNext() const
		{
			return m_position < m_entries->size() ;
		}
		const QString& value() const
		{
			return m_entries->data()[ m_position ]->path() ;
		}
		directoryEntries::ICON icon() const
		{
			if( m_entries->data()[ m_position ]->isFolder() ){

				return directoryEntries::ICON::FOLDER ;
			}else{
				return directoryEntries::ICON::FILE ;
			}
		}
		iter next() const
		{
			auto m = *this ;
			m.m_position++ ;
			return m ;
		}
	private:
		size_t m_position = 0 ;
		const std::vector< directoryEntries::wrapper > * m_entries = nullptr ;
	} ;

	void join( bool folderFirst )
	{
		m_joined.clear() ;

		if( folderFirst ){

			for( const auto& it : m_folders ){

				m_joined.emplace_back( it ) ;
			}
			for( const auto& it : m_files ){

				m_joined.emplace_back( it ) ;
			}
		}else{
			for( const auto& it : m_files ){

				m_joined.emplace_back( it ) ;
			}
			for( const auto& it : m_folders ){

				m_joined.emplace_back( it ) ;
			}
		}
	}

	directoryEntries::iter Iter()
	{
		return { m_joined } ;
	}
} ;

Q_DECLARE_METATYPE( directoryEntries::iter )

namespace directoryManager
{
	directoryEntries readAll( const QString&,std::atomic_bool& ) ;

	directoryEntries readAll( const QString& ) ;

	void removeDirectoryContents( const QString&,std::atomic_bool& ) ;

	void removeDirectory( const QString&,std::atomic_bool& ) ;
}

#endif

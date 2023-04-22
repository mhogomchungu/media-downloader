
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
	void clear()
	{
		m_folders.clear() ;
		m_files.clear() ;
	}
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

namespace directoryManager
{
	bool supportsCancel() ;

	directoryEntries readAll( const QString& ) ;
	directoryEntries readAll( const QString&,std::atomic_bool& ) ;

	void removeDirectoryContents( const QString&,std::atomic_bool& ) ;
	void removeDirectoryContents( const QString& ) ;

	void removeDirectory( const QString&,std::atomic_bool& ) ;
	void removeDirectory( const QString& ) ;
}

#endif


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

#include <QString>
#include <QPushButton>
#include <QObject>

class directoryEntries
{
public:
	bool valid( const char * e ) ;
	void sort() ;
	void addFile( qint64 dateCreated,QString path )
	{
		m_files.emplace_back( dateCreated,std::move( path ) ) ;
	}
	void addFolder( qint64 dateCreated,QString path )
	{
		m_folders.emplace_back( dateCreated,std::move( path ) ) ;
	}
	template< typename Folders,typename Files >
	void addToList( Folders folders,Files files ) const
	{
		for( const auto& it : m_folders ){

			folders( it.path ) ;
		}
		for( const auto& it : m_files ){

			files( it.path ) ;
		}
	}
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
	};

	std::vector< entry > m_folders ;
	std::vector< entry > m_files ;
} ;

#ifdef Q_OS_LINUX

#include <array>

#include <cstdio>

#include <limits.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <QDir>

class directoryManager
{
public:
	directoryManager( const QString& path,QDir::Filters ) :
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
	bool getFirst( directoryEntries& e )
	{
		return !this->get( e ) ;
	}
	bool get( directoryEntries& s )
	{
		auto e = readdir( m_handle ) ;

		if( e ){

			const auto name = e->d_name ;

			if( s.valid( name ) ){

				struct stat m ;

				if( stat( m_pathManager.setPath( name ),&m ) == 0 ){

					if( S_ISREG( m.st_mode ) ){

						s.addFile( m.st_mtime,name ) ;

					}else if( S_ISDIR( m.st_mode ) ){

						s.addFolder( m.st_mtime,name ) ;
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
} ;

#else

#include <QDir>
#include <QDateTime>
#include <QString>
#include <QFileInfo>

class directoryManager
{
public:
	directoryManager( const QString& path,QDir::Filters f ) :
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
	bool valid( const QString& m )
	{
		if( m.startsWith( "info_" ) || m.endsWith( ".log" ) ){

			return false ;
		}else{
			return true ;
		}
	}
	bool getFirst( directoryEntries& entrs )
	{
		return !this->get( entrs ) ;
	}
	bool get( directoryEntries& entrs )
	{
		if( m_counter < m_list.size() ){

			const auto& m = m_list[ m_counter++ ] ;

			if( this->valid( m ) ){

				auto w = QDir::fromNativeSeparators( m ) ;

				m_fileInfo.setFile( m_path + "/" + m ) ;

				if( m_fileInfo.isFile() ){

					entrs.addFile( this->createdTime( m_fileInfo ),w ) ;

				}else if( m_fileInfo.isDir() ){

					entrs.addFolder( this->createdTime( m_fileInfo ),w ) ;
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
} ;

#endif

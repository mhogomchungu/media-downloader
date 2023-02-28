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
#include <cstring>

bool directoryEntries::valid( const char * e )
{
	if( std::strcmp( e,".." ) == 0 ){

		return false ;
	}

	if( std::strncmp( e,"info_",5 ) == 0 ){

		return false ;
	}

	if( *e == '.' ){

		return false ;
	}

	return true ;
}

bool directoryEntries::valid( const QString& m )
{
	if( m == ".." || m.startsWith( "." ) || m.startsWith( "info_" ) ){

		return false ;
	}else{
		return true ;
	}
}

void directoryEntries::sort()
{
	std::sort( m_folders.begin(),m_folders.end(),[]( const entry& lhs,const entry& rhs ){

		return lhs.dateCreated < rhs.dateCreated ;
	} ) ;

	std::sort( m_files.begin(),m_files.end(),[]( const entry& lhs,const entry& rhs ){

		return lhs.dateCreated < rhs.dateCreated ;
	} ) ;
}

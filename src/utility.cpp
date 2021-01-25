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

QList< QByteArray > utility::split( const QByteArray& e,char token )
{
	return e.split( token ) ;
}

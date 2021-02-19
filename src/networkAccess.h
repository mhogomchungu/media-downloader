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
#ifndef NETWORK_ACCESS_H
#define NETWORK_ACCESS_H

#include "network_support.h"

#include "context.hpp"

#if MD_NETWORK_SUPPORT

#include <QtNetwork/QNetworkAccessManager>
#include <QFile>
#include <QStringList>

#include "context.hpp"
#include "settings.h"

class basicdownloader ;

class networkAccess
{
public:
	networkAccess( const Context& ) ;
	void download( const engines::engine& ) ;
	static bool hasNetworkSupport()
	{
		return true ;
	}
private:
	struct metadata
	{
		qint64 size ;
		QString url ;
		QString sha256 ;
	};
	void download( const metadata&,const engines::engine& ) ;
	void post( const engines::engine&,const QString& ) ;
	const Context& m_ctx ;
	QNetworkAccessManager m_accessManager ;
	QFile m_file ;
	QStringList m_data ;
	basicdownloader& m_basicdownloader ;
	tabManager& m_tabManager ;
};

#else

class networkAccess
{
public:
	static bool hasNetworkSupport()
	{
		return false ;
	}
	networkAccess( const Context& )
	{
	}
	void download( const engines::engine& )
	{
	}
private:

};

#endif

#endif


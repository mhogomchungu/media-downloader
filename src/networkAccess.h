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

#include <QFile>
#include <QStringList>

#include "context.hpp"
#include "engines.h"
#include "context.hpp"
#include "settings.h"
#include "engines.h"
#include "utils/network_access_manager.hpp"

class basicdownloader ;

class networkAccess
{
public:
	networkAccess( const Context& ) ;
	void download( const engines::Iterator&,const QString& setDefaultENgine = QString() ) ;
	static bool hasNetworkSupport()
	{
		#if QT_VERSION >= QT_VERSION_CHECK( 5,6,0 )
			return true ;
		#else
			return false ;
		#endif
	}

	template< typename Function >
	void getResource( const QString& url,Function function )
	{
		m_network.get( this->networkRequest( url ),[ function = std::move( function ) ]( const utils::NetworkAccessManager::reply& reply ){

			function( reply.data() ) ;
		} ) ;
	}
private:
	QNetworkRequest networkRequest( const QString& url ) ;
	struct metadata
	{
		qint64 size ;
		QString url ;
		QString fileName ;
	};

	struct Opts
	{
		Opts( networkAccess::metadata m,
		      engines::Iterator itr,
		      const QString& exePath,
		      const QString& efp,
		      const QString& sde,
		      int xd ) :
			iter( std::move( itr ) ),
			exeBinPath( exePath ),
			metadata( std::move( m ) ),
			archiveExtractionPath( efp ),
			defaultEngine( sde ),
			id( xd )
		{
			filePath = archiveExtractionPath + "/" + metadata.fileName ;

			isArchive = filePath.endsWith( ".zip" ) || filePath.contains( ".tar." ) ;

			if( !isArchive ){

				filePath += ".tmp" ;
			}
		}
		engines::Iterator iter ;
		QString exeBinPath ;
		networkAccess::metadata metadata ;
		QString filePath ;
		QString archiveExtractionPath ;
		QString defaultEngine ;
		QString networkError ;
		bool isArchive ;
		int id ;
	};

	void download( networkAccess::Opts ) ;

	void download( const QByteArray&,const engines::engine&,networkAccess::Opts ) ;

	void finished( networkAccess::Opts ) ;

	void post( const engines::engine&,const QString&,int ) ;

	const Context& m_ctx ;
	utils::NetworkAccessManager m_network ;
	QFile m_file ;
	basicdownloader& m_basicdownloader ;
	tabManager& m_tabManager ;
};

#endif

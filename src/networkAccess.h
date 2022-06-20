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

#include "context.hpp"

#include "engines.h"

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>

#include <QFile>
#include <QStringList>

#include "context.hpp"
#include "settings.h"
#include "engines.h"

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
		auto networkReply = m_accessManager.get( this->networkRequest( url ) ) ;

		QObject::connect( networkReply,&QNetworkReply::finished,[ networkReply,function = std::move( function ) ](){

			function( networkReply->readAll() ) ;
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
			engine( itr.engine() ),
			iter( std::move( itr ) ),
			exeBinPath( exePath ),
			metadata( std::move( m ) ),
			exeFolderPath( efp ),
			defaultEngine( sde ),
			id( xd )
		{
			if( metadata.fileName.endsWith( ".zip" ) ){

				filePath = exeBinPath + ".tmp.zip" ;

			}else if( metadata.fileName.endsWith( ".tar.gz" ) ){

				filePath = exeBinPath + ".tmp.tar.gz" ;
			}else{
				filePath = exeBinPath + ".tmp" ;
			}
		}
		QNetworkReply * networkReply ;
		const engines::engine& engine ;
		engines::Iterator iter ;
		QString exeBinPath ;
		networkAccess::metadata metadata ;
		QString filePath ;
		QString exeFolderPath ;
		QString defaultEngine ;
		int id ;
	};

	void download( networkAccess::Opts ) ;

	void finished( networkAccess::Opts ) ;

	void post( const engines::engine&,const QString&,int ) ;

	const Context& m_ctx ;
	QNetworkAccessManager m_accessManager ;
	QFile m_file ;
	basicdownloader& m_basicdownloader ;
	tabManager& m_tabManager ;
};

#endif

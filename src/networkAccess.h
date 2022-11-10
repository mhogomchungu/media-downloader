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

#include <memory>

#include <QFile>
#include <QStringList>

#include "engines.h"
#include "utils/network_access_manager.hpp"

class basicdownloader ;
class Context ;
class tabManager ;

class networkAccess
{
public:
	networkAccess( const Context& ) ;

	static bool hasNetworkSupport()
	{
		#if QT_VERSION >= QT_VERSION_CHECK( 5,6,0 )
			return true ;
		#else
			return false ;
		#endif
	}
	struct showVersionInfo
	{
		bool show ;
		bool setAfterDownloading ;
	};
	void download( const engines::Iterator& iter,
		       networkAccess::showVersionInfo showVinfo,
		       const QString& setDefaultENgine = QString() ) const
	{
		const_cast< networkAccess * >( this )->download( iter,showVinfo,setDefaultENgine ) ;
	}

	template< typename Function >
	void get( const QString& url,Function function ) const
	{
		const_cast< networkAccess * >( this )->get( url,std::move( function ) ) ;
	}
private:
	void download( const engines::Iterator&,
		       networkAccess::showVersionInfo,
		       const QString& setDefaultENgine = QString() ) ;

	template< typename Function >
	void get( const QString& url,Function&& function )
	{
		m_network.get( this->networkRequest( url ),[ function = std::move( function ) ]( const utils::network::reply& reply ){

			if( reply.success() ){

				function( reply.data() ) ;
			}else{
				function( {} ) ;
			}
		} ) ;
	}
	QNetworkRequest networkRequest( const QString& url ) ;
	struct metadata
	{
		qint64 size ;
		QString url ;
		QString fileName ;
	};

	struct Opts
	{
		Opts( engines::Iterator itr,
		      const QString& exePath,
		      const QString& efp,
		      const QString& sde,
		      int xd,
		      networkAccess::showVersionInfo svf ) :
			iter( std::move( itr ) ),
			exeBinPath( exePath ),
			tempPath( efp ),
			defaultEngine( sde ),
			showVinfo( svf ),
			id( xd )
		{
		}
		void add( networkAccess::metadata&& m )
		{
			metadata = std::move( m ) ;

			filePath = tempPath + "/" + metadata.fileName ;

			isArchive = filePath.endsWith( ".zip" ) || filePath.contains( ".tar." ) ;

			if( !isArchive ){

				filePath += ".tmp" ;
			}
		}
		void openFile()
		{
			m_file = std::make_shared< QFile >( this->filePath ) ;
			m_file->remove() ;
			m_file->open( QIODevice::WriteOnly ) ;
		}
		QFile& file()
		{
			return *m_file ;
		}
		engines::Iterator iter ;
		QString exeBinPath ;
		networkAccess::metadata metadata ;
		QString filePath ;
		QString tempPath ;
		QString defaultEngine ;
		QString networkError ;
		/*
		 * We are using make_shared because old versions of gcc do not work with
		 * unique_ptr when moving the class to lambda capture area
		 */
		std::shared_ptr< QFile > m_file ;
		bool isArchive ;
		networkAccess::showVersionInfo showVinfo ;
		int id ;
	};

	QString downloadFailed() ;

	void extractArchive( const engines::engine&,networkAccess::Opts ) ;

	void download( networkAccess::Opts ) ;

	void download( const QByteArray&,const engines::engine&,networkAccess::Opts ) ;

	void finished( networkAccess::Opts ) ;

	void post( const engines::engine&,const QString&,int ) ;

	const Context& m_ctx ;
	utils::network::manager m_network ;
	basicdownloader& m_basicdownloader ;
	tabManager& m_tabManager ;
};

#endif

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

#include "networkAccess.h"

#if MD_NETWORK_SUPPORT

#include "networkAccess.h"
#include "tabmanager.h"
#include "basicdownloader.h"
#include "engines.h"

#include <QtNetwork/QNetworkReply>
#include <QFile>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDir>

networkAccess::networkAccess( const Context& ctx ) :
	m_ctx( ctx ),
	m_basicdownloader( m_ctx.TabManager().basicDownloader() ),
	m_tabManager( m_ctx.TabManager() )
{
}

void networkAccess::download( const engines::engine& engine )
{
	QDir dir ;

	auto path = engine.exeFolderPath() ;

	if( !dir.exists( path ) ){

		if( !dir.mkpath( path ) ){

			this->post( engine,QObject::tr( "Failed to download, Following path can not be created: " ) + path ) ;

			return ;
		}
	}


	this->post( engine,QObject::tr( "Start Downloading" ) + " " + engine.name() + " ..." ) ;

	m_tabManager.disableAll() ;

	m_basicdownloader.setAsActive().enableQuit() ;

	QString url( engine.downloadUrl() ) ;

	QNetworkRequest networkRequest( url ) ;

	networkRequest.setAttribute( QNetworkRequest::FollowRedirectsAttribute,true ) ;

	auto networkReply = m_accessManager.get( networkRequest ) ;

	QObject::connect( networkReply,&QNetworkReply::downloadProgress,[ this,&engine ]( qint64 received,qint64 total ){

		Q_UNUSED( received )
		Q_UNUSED( total )

		this->post( engine,"..." ) ;
	} ) ;

	QObject::connect( networkReply,&QNetworkReply::finished,[ this,networkReply,&engine ](){

		networkReply->deleteLater() ;

		if( networkReply->error() != QNetworkReply::NetworkError::NoError ){

			this->post( engine,QObject::tr( "Download Failed" ) + ": " + networkReply->errorString() ) ;
			m_tabManager.enableAll() ;
			return ;
		}

		metadata metadata ;

		engines::Json json( networkReply->readAll() ) ;

		if( !json ){

			this->post( engine,QObject::tr( "Failed to parse json file from github" ) + ": " + json.errorString() ) ;
			m_tabManager.enableAll() ;
			return ;
		}

		auto object = json.doc().object() ;

		auto value = object.value( "assets" ) ;

		const auto array = value.toArray() ;

		for( const auto& it : array ){

			const auto object = it.toObject() ;

			const auto value = object.value( "name" ) ;

			auto entry = value.toString() ;

			if( entry == engine.commandName() ){

				auto value = object.value( "browser_download_url" ) ;

				metadata.url = value.toString() ;

				value = object.value( "size" ) ;

				metadata.size = value.toInt() ;

				value = object.value( "browser_download_url" ) ;

			}else if( entry == "SHA2-256SUMS" ){

				auto value = object.value( "browser_download_url" ) ;

				metadata.sha256 = value.toString() ;
			}
		}

		this->download( metadata,engine ) ;
	} ) ;
}

void networkAccess::download( const metadata& metadata,const engines::engine& engine )
{
	QString filePath = engine.exePath().exe() ;

	m_file.setFileName( filePath ) ;

	m_file.remove() ;

	m_file.open( QIODevice::WriteOnly ) ;

	this->post( engine,QObject::tr( "Downloading" ) + ": " + metadata.url ) ;

	this->post( engine,QObject::tr( "Destination" ) + ": " + filePath ) ;

	QNetworkRequest networkRequest( metadata.url ) ;
	networkRequest.setAttribute( QNetworkRequest::FollowRedirectsAttribute,true ) ;

	auto networkReply = m_accessManager.get( networkRequest ) ;

	QObject::connect( networkReply,&QNetworkReply::finished,[ this,networkReply,&engine ](){

		networkReply->deleteLater() ;

		if( networkReply->error() != QNetworkReply::NetworkError::NoError ){

			this->post( engine,QObject::tr( "Download Failed" ) + ": " + networkReply->errorString() ) ;

			m_tabManager.enableAll() ;
		}else{
			m_file.close() ;

			this->post( engine,QObject::tr( "Download complete" ) ) ;

			m_file.setPermissions( m_file.permissions() | QFileDevice::ExeOwner ) ;

			m_tabManager.basicDownloader().checkAndPrintInstalledVersion( engine ) ;
		}
	} ) ;

	QObject::connect( networkReply,&QNetworkReply::downloadProgress,
			  [ this,metadata,networkReply,&engine ]( qint64 received,qint64 total ){

		Q_UNUSED( total )

		m_file.write( networkReply->readAll() ) ;

		auto perc = double( received )  * 100 / double( metadata.size ) ;
		auto totalSize = QString::number( metadata.size ) ;
		auto current   = QString::number( received ) ;
		auto percentage = QString::number( perc,'f',2 ) ;

		auto m = QString( "%1/%2 bytes(%3%)" ).arg( current,totalSize,percentage ) ;

		this->post( engine,QObject::tr( "Downloading" ) + " " + engine.name() + ": " + m ) ;
	} ) ;
}

void networkAccess::post( const engines::engine& engine,const QString& e )
{
	m_ctx.logger().add( [ &engine,&e ]( QStringList& s ){

		auto prefix = QObject::tr( "Downloading" ) + " " + engine.name() ;

		if( s.isEmpty() ){

			s.append( "[media-downloader] " + e ) ;

		}else if( e == "..." ){

			s.last() = s.last() + " ..." ;

		}else if( e.startsWith( prefix ) ){

			if( s.last().startsWith( "[media-downloader] " + prefix ) ){

				s.removeLast() ;
			}

			s.append( "[media-downloader] " + e ) ;
		}else{
			s.append( "[media-downloader] " + e ) ;
		}
	} ) ;
}

#endif

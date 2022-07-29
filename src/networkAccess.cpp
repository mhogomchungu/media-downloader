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

#if QT_VERSION >= QT_VERSION_CHECK( 5,4,0 )
static QString _sslLibraryVersionString()
{
	return QSslSocket::sslLibraryBuildVersionString() ;
}
#else
static QString _sslLibraryVersionString()
{
	return {} ;
}
#endif

networkAccess::networkAccess( const Context& ctx ) :
	m_ctx( ctx ),
	m_basicdownloader( m_ctx.TabManager().basicDownloader() ),
	m_tabManager( m_ctx.TabManager() )
{
	if( utility::platformIsWindows() && m_ctx.Settings().showVersionInfoWhenStarting() ){

		auto& e = m_ctx.logger() ;
		auto s = QSslSocket::sslLibraryVersionString() ;

		auto id = utility::sequentialID() ;

		e.add( QObject::tr( "Checking installed version of" ) + " OpenSSL",id ) ;

		if( s.isEmpty() ){

			auto q = _sslLibraryVersionString() ;
			auto m = QObject::tr( "Failed to find version information, make sure \"%1\" is installed and works properly" ).arg( q ) ;
			e.add( m,id ) ;
		}else{
			e.add( QObject::tr( "Found version" ) + ": " + s,id ) ;
		}
	}
}

QNetworkRequest networkAccess::networkRequest( const QString& url )
{
	QNetworkRequest networkRequest( url ) ;
#if QT_VERSION >= QT_VERSION_CHECK( 5,9,0 )
	networkRequest.setAttribute( QNetworkRequest::RedirectPolicyAttribute,QNetworkRequest::NoLessSafeRedirectPolicy ) ;
#else
	#if QT_VERSION >= QT_VERSION_CHECK( 5,6,0 )
		networkRequest.setAttribute( QNetworkRequest::FollowRedirectsAttribute,true ) ;
	#endif
#endif
		return networkRequest ;
}

void networkAccess::download( const engines::Iterator& iter,const QString& setDefaultEngine )
{
	const auto& engine = iter.engine() ;

	auto exePath = engine.exePath().realExe() ;

	auto exeFolderPath = m_ctx.Engines().engineDirPaths().binPath() ;

	auto internalBinPath = QDir::fromNativeSeparators( exeFolderPath ) ;

	if( !exeFolderPath.startsWith( internalBinPath ) ){

		auto m = QDir::fromNativeSeparators( exePath ) ;

		auto a = m.lastIndexOf( '/' ) ;

		if( a == -1 ){

			exePath = internalBinPath + "/" + exePath ;
		}else{
			exePath = internalBinPath + "/" + m.mid(( a + 1 ) ) ;
		}
	}

	QDir dir ;

	auto path = engine.exeFolderPath() ;

	int id = utility::sequentialID() ;

	if( !dir.exists( path ) ){

		if( !dir.mkpath( path ) ){

			this->post( engine,QObject::tr( "Failed to download, Following path can not be created: " ) + path,id ) ;

			return ;
		}
	}

	this->post( engine,QObject::tr( "Start Downloading" ) + " " + engine.name() + " ...",id ) ;

	m_tabManager.disableAll() ;

	m_basicdownloader.setAsActive().enableQuit() ;

	auto networkReply = m_accessManager.get( this->networkRequest( engine.downloadUrl() ) ) ;

	QObject::connect( networkReply,&QNetworkReply::downloadProgress,[ id,this,&engine ]( qint64 received,qint64 total ){

		Q_UNUSED( received )
		Q_UNUSED( total )

		this->post( engine,"...",id ) ;
	} ) ;

	QObject::connect( networkReply,&QNetworkReply::finished,
			  [ id,this,networkReply,&engine,iter,exePath,exeFolderPath,setDefaultEngine ](){

		networkReply->deleteLater() ;

		if( networkReply->error() != QNetworkReply::NetworkError::NoError ){

			this->post( engine,QObject::tr( "Download Failed" ) + ": " + networkReply->errorString(),id ) ;

			m_tabManager.enableAll() ;

			if( iter.hasNext() ){

				m_ctx.versionInfo().check( iter.next() ) ;
			}

			return ;
		}

		networkAccess::metadata metadata ;

		util::Json json( networkReply->readAll() ) ;

		if( !json ){

			this->post( engine,QObject::tr( "Failed to parse json file from github" ) + ": " + json.errorString(),id ) ;

			m_tabManager.enableAll() ;

			if( iter.hasNext() ){

				m_ctx.versionInfo().check( iter.next() ) ;
			}

			return ;
		}

		auto object = json.doc().object() ;

		auto value = object.value( "assets" ) ;

		const auto array = value.toArray() ;

		for( const auto& it : array ){

			const auto object = it.toObject() ;

			const auto value = object.value( "name" ) ;

			auto entry = value.toString() ;

			if( engine.foundNetworkUrl( entry ) ){

				auto value = object.value( "browser_download_url" ) ;

				metadata.url = value.toString() ;

				value = object.value( "size" ) ;

				metadata.size = value.toInt() ;

				metadata.fileName = entry ;

				break ;
			}
		}

		this->download( { metadata,iter,exePath,exeFolderPath,setDefaultEngine,id } ) ;
	} ) ;
}

void networkAccess::download( networkAccess::Opts opts )
{
	const auto& engine = opts.iter.engine() ;

	engine.updateEnginePaths( m_ctx,opts.filePath,opts.exeBinPath,opts.archiveExtractionPath ) ;

	m_file.setFileName( opts.filePath ) ;

	m_file.remove() ;

	m_file.open( QIODevice::WriteOnly ) ;

	this->post( engine,QObject::tr( "Downloading" ) + ": " + opts.metadata.url,opts.id ) ;

	this->post( engine,QObject::tr( "Destination" ) + ": " + opts.filePath,opts.id ) ;

	opts.networkReply = m_accessManager.get( this->networkRequest( opts.metadata.url ) ) ;

	QObject::connect( opts.networkReply,&QNetworkReply::downloadProgress,
			  [ id = opts.id,this,metadata = opts.metadata,networkReply = opts.networkReply,&engine,locale = utility::locale() ]( qint64 received,qint64 total ){

		Q_UNUSED( total )

		m_file.write( networkReply->readAll() ) ;

		auto perc = double( received )  * 100 / double( metadata.size ) ;
		auto totalSize = locale.formattedDataSize( metadata.size ) ;
		auto current   = locale.formattedDataSize( received ) ;
		auto percentage = QString::number( perc,'f',2 ) ;

		auto m = QString( "%1 / %2 (%3%)" ).arg( current,totalSize,percentage ) ;

		this->post( engine,QObject::tr( "Downloading" ) + " " + engine.name() + ": " + m,id ) ;
	} ) ;

	QObject::connect( opts.networkReply,&QNetworkReply::finished,[ this,opts = std::move( opts ) ]()mutable{

		this->finished( std::move( opts ) ) ;
	} ) ;
}

void networkAccess::finished( networkAccess::Opts str )
{
	str.networkReply->deleteLater() ;

	const auto& engine = str.iter.engine() ;

	if( str.networkReply->error() != QNetworkReply::NetworkError::NoError ){

		this->post( engine,QObject::tr( "Download Failed" ) + ": " + str.networkReply->errorString(),str.id ) ;

		m_tabManager.enableAll() ;

		if( str.iter.hasNext() ){

			m_ctx.versionInfo().check( str.iter.next() ) ;
		}
	}else{
		if( m_file.size() != str.metadata.size ){

			//????

			if( str.iter.hasNext() ){

				m_ctx.versionInfo().check( str.iter.next() ) ;
			}

			return ;
		}

		m_file.close() ;

		this->post( engine,QObject::tr( "Download complete" ),str.id ) ;

		if( str.isArchive ){

			this->post( engine,QObject::tr( "Extracting archive: " ) + str.filePath,str.id ) ;

			if( engine.archiveContainsFolder() ){

				auto m = str.archiveExtractionPath + "/" + engine.name() ;
				//this->post( str.engine,QObject::tr( "Removing Folder: " ) + m,str.id ) ;

				QDir( m ).removeRecursively() ;
			}else{
				QFile::remove( str.exeBinPath ) ;
			}

			auto exe = m_ctx.Engines().findExecutable( utility::platformIsWindows() ? "bsdtar.exe" : "tar" ) ;

			auto args = QStringList{ "-x","-f",str.filePath,"-C",str.archiveExtractionPath } ;

			util::run( exe,args,[ this,str = std::move( str ) ]( const util::run_result& s ){

				const auto& engine = str.iter.engine() ;

				QFile::remove( str.filePath ) ;

				if( s.success() ){

					const auto& engine = str.iter.engine() ;

					if( engine.archiveContainsFolder() ){

						engine.renameArchiveFolder( str.archiveExtractionPath ) ;

						const auto& name = engine.name() ;

						auto exe = str.archiveExtractionPath + "/" + name + "/" + name ;

						engine.updateCmdPath( exe ) ;

						QFile f( exe ) ;

						f.setPermissions( f.permissions() | QFileDevice::ExeOwner ) ;
					}else{
						QFile f( str.exeBinPath ) ;

						f.setPermissions( f.permissions() | QFileDevice::ExeOwner ) ;
					}

					utility::setDefaultEngine( m_ctx,str.defaultEngine ) ;

					m_ctx.versionInfo().check( str.iter ) ;
				}else{
					this->post( engine,s.stdError,str.id ) ;

					if( str.iter.hasNext() ){

						m_ctx.versionInfo().check( str.iter.next() ) ;
					}
				}
			} ) ;
		}else{
			this->post( engine,QObject::tr( "Renaming file to: " ) + str.exeBinPath,str.id ) ;

			QFile::remove( str.exeBinPath ) ;

			m_file.rename( str.exeBinPath ) ;

			m_file.setPermissions( m_file.permissions() | QFileDevice::ExeOwner ) ;

			utility::setDefaultEngine( m_ctx,str.defaultEngine ) ;

			m_ctx.versionInfo().check( str.iter ) ;
		}
	}
}

void networkAccess::post( const engines::engine& engine,const QString& m,int id )
{	
	m_ctx.logger().add( [ &engine,&m ]( Logger::Data& s,int id,bool ){

		auto e = m.toUtf8() ;

		auto p = QObject::tr( "Downloading" ) + " " + engine.name() ;

		auto prefix = p.toUtf8() ;

		if( s.isEmpty() ){

			s.add( "[media-downloader] " + e,id ) ;

		}else if( e == "..." ){

			auto m = s.getData( id ) ;

			if( m ){

				m.replaceLast( m.lastText() + " ..." ) ;
			}

		}else if( e.startsWith( prefix ) ){

			auto m = s.getData( id ) ;

			if( m ){

				if( m.lastText().startsWith( "[media-downloader] " + prefix ) ){

					m.removeLast() ;
				}
			}

			s.add( "[media-downloader] " + e,id ) ;
		}else{
			s.add( "[media-downloader] " + e,id ) ;
		}
	},id ) ;
}

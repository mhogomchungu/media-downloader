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

#include "generic.h"
#include "../utility.h"

generic::generic( const engines& e,const engines::engine& s,QJsonObject& ) :
	engines::engine::functions( e.Settings(),s,e.processEnvironment() )
{
}

generic::~generic()
{
}

media_downloader::~media_downloader()
{
}

media_downloader::media_downloader( const engines& e,const engines::engine& s,QJsonObject & ) :
	engines::engine::functions( e.Settings(),s,e.processEnvironment() )
{
}

static QString _lastComponent( const QString& e )
{
	auto m = QDir::fromNativeSeparators( e ) ;

	auto s = m.lastIndexOf( '/' ) ;

	if( s == - 1 ){

		return m ;
	}else{
		return e.mid( s + 1 ) ;
	}
}

void media_downloader::updateEnginePaths( const Context& ctx,
					  QString& filePath,
					  QString& exeBinPath,
					  QString& archiveExtractionPath )
{
	const auto& e = ctx.Engines().engineDirPaths() ;
	const auto& m = e.updatePath() ;

	filePath      = e.basePath() + "/tmp/" + _lastComponent( filePath ) ;
	exeBinPath    = m + "/bin/" + _lastComponent( exeBinPath ) ;
	archiveExtractionPath = e.updatePath() ;

	QDir().mkpath( e.basePath() + "/tmp" ) ;
	QDir().mkpath( archiveExtractionPath ) ;
}

bool media_downloader::foundNetworkUrl( const QString& s )
{
	return s.startsWith( "updates" ) ;
}

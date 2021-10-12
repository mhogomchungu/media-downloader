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

#include "gallery-dl.h"
#include "../settings.h"
#include "../downloadmanager.h"

gallery_dl::gallery_dl( const engines& engines,const engines::engine& engine,QJsonObject& object ) :
	engines::engine::functions( engines.Settings(),engine )
{
	if( !object.contains( "CookieArgument" ) ){

		object.insert( "CookieArgument","--cookies" ) ;
	}
}

gallery_dl::~gallery_dl()
{
}

void gallery_dl::updateDownLoadCmdOptions( const engines::engine::functions::updateOpts& s )
{	
	const auto& engine = engines::engine::functions::engine() ;

	if( !engine.optionsArgument().isEmpty() ){

		s.ourOptions.append( engine.optionsArgument() ) ;
	}

	if( !s.quality.isEmpty() ){

		s.ourOptions.append( s.quality ) ;
	}
}

engines::engine::functions::DataFilter gallery_dl::Filter( const QString& e )
{
	auto& s = engines::engine::functions::Settings() ;
	const auto& engine = engines::engine::functions::engine() ;

	return { util::types::type_identity< gallery_dl::gallery_dlFilter >(),e,s,engine } ;
}

void gallery_dl::runCommandOnDownloadedFile( const QString& e,const QString& )
{
	auto& settings = engines::engine::functions::Settings() ;

	auto a = settings.commandOnSuccessfulDownload() ;

	if( !a.isEmpty() && !e.isEmpty() ){

		auto args = util::split( a,' ',true ) ;
		auto exe = args.takeAt( 0 ) ;
		args.append( "bla bla bla" ) ;
		auto pos = args.size() - 1 ;

		for( const auto& it : util::split( e,'\n',true ) ){

			auto b = settings.downloadFolder() + "/gallery-dl/" + it ;

			if( QFile::exists( b ) ){

				args.replace( pos,b ) ;

				QProcess::startDetached( exe,args ) ;
			}
		}
	}
}

QString gallery_dl::updateTextOnCompleteDownlod( const QString& uiText,
						 const QString& bkText,
						 const engines::engine::functions::finishedState& f )
{
	if( f.success() ){

		return engines::engine::functions::updateTextOnCompleteDownlod( uiText,f ) ;
	}else{
		return engines::engine::functions::updateTextOnCompleteDownlod( bkText,f ) ;
	}
}

gallery_dl::gallery_dlFilter::gallery_dlFilter( const QString& e,settings&,const engines::engine& engine ) :
	engines::engine::functions::filter( e,engine )
{
}

const QString& gallery_dl::gallery_dlFilter::operator()( const Logger::Data& s )
{
	const auto data = s.toStringList() ;

	QStringList m ;

	for( const auto& e : data ){

		auto u = QDir::fromNativeSeparators( e ) ;

		if( u.contains( "/gallery-dl/" ) ){

			m.append( u.mid( u.indexOf( "/gallery-dl/" ) + 12 ) ) ;
		}
	}

	if( !m.isEmpty() ){

		m_tmp = m.join( "\n" ) ;
	}else{
		const auto& s = data.last() ;

		if( s.startsWith( "[media-downloader] cmd:" ) ){

			auto m = engines::engine::functions::timer::startTimerText() ;

			m_tmp = QObject::tr( "Downloading" ) + "\n" + m ;
		}else{
			m_tmp = QObject::tr( "Downloading" ) + "\n" + s ;
		}

		return m_tmp ;
	}

	return m_tmp ;
}

gallery_dl::gallery_dlFilter::~gallery_dlFilter()
{
}

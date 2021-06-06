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

gallery_dl::gallery_dl( settings& s ) :
	engines::engine::functions( s )
{
}

gallery_dl::~gallery_dl()
{
}

void gallery_dl::updateDownLoadCmdOptions( const engines::engine& engine,
					   const QString& quality,
					   const QStringList& userOptions,
					   QStringList& urls,
					   QStringList& ourOptions )
{
	Q_UNUSED( userOptions )
	Q_UNUSED( urls )

	if( !engine.optionsArgument().isEmpty() ){

		ourOptions.append( engine.optionsArgument() ) ;
	}

	if( !quality.isEmpty() ){

		ourOptions.append( quality ) ;
	}
}

std::unique_ptr< engines::engine::functions::filter> gallery_dl::Filter( const QString& e )
{
	auto& s = engines::engine::functions::Settings() ;

	return std::make_unique< gallery_dl::gallery_dlFilter >( e,s ) ;
}

void gallery_dl::updateOptions( QJsonObject& object,settings& )
{
	if( !object.contains( "CookieArgument" ) ){

		object.insert( "CookieArgument","--cookies" ) ;
	}
}

void gallery_dl::runCommandOnDownloadedFile( const QString& e,const QString& )
{
	auto& settings = engines::engine::functions::Settings() ;

	auto a = settings.commandOnSuccessfulDownload() ;

	if( !a.isEmpty() && !e.isEmpty() ){

		auto args = utility::split( a,' ',true ) ;
		auto exe = args.takeAt( 0 ) ;
		args.append( "bla bla bla" ) ;
		auto pos = args.size() - 1 ;

		for( const auto& it : utility::split( e,'\n',true ) ){

			auto b = settings.downloadFolder() + "/gallery-dl/" + it ;

			if( QFile::exists( b ) ){

				args.replace( pos,b ) ;

				QProcess::startDetached( exe,args ) ;
			}
		}
	}
}

QString gallery_dl::updateTextOnCompleteDownlod( const engines::engine&,
						 const QString& uiText,
						 const QString& bkText,
						 const engines::engine::functions::finishedState& f )
{
	auto m = engines::engine::functions::processCompleteStateText( f ) ;
	auto e = engines::engine::functions::timer::stringElapsedTime( f.duration() ) ;

	if( f.success() ){

		return engines::engine::functions::updateTextOnCompleteDownlod( uiText,f ) ;
	}else{
		return engines::engine::functions::updateTextOnCompleteDownlod( bkText,f ) ;
	}
}

gallery_dl::gallery_dlFilter::gallery_dlFilter( const QString& e,settings& ) :
	engines::engine::functions::filter( e )
{
}

const QString& gallery_dl::gallery_dlFilter::operator()( const engines::engine&,
							 const Logger::Data& s )
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

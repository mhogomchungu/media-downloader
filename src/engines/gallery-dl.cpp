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
#include "../concurrentdownloadmanager.hpp"

gallery_dl::gallery_dl( settings& s ) :
	m_settings( s )
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
	return std::make_unique< gallery_dl::gallery_dlFilter >( e,m_settings ) ;
}

void gallery_dl::updateOptions( QJsonObject& object,settings& )
{
	if( !object.contains( "CookieArgument" ) ){

		object.insert( "CookieArgument","--cookies" ) ;
	}
}

QString gallery_dl::updateTextOnCompleteDownlod( const engines::engine&,
						 const QString& uiText,
						 const QString& bkText,
						 const concurrentDownloadManagerFinishedStatus& f )
{
	auto m = engines::engine::functions::processCompleteStateText( f ) ;

	if( f.exitState.success() ){

		return uiText + "\n" + m ;
	}else{
		return bkText + "\n" + m ;
	}
}

gallery_dl::gallery_dlFilter::gallery_dlFilter( const QString& e,settings& s ) :
	engines::engine::functions::filter( e ),
	m_settings( s )
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

	if( QDir::fromNativeSeparators( data.last() ).contains( "/gallery-dl/" ) ){

		m_tmp = m.join( "\n" ) ;
	}else{
		if( m.isEmpty() ){

			return m_processing.text() ;
		}else{
			m_tmp = m.join( "\n" ) + "\n" + m_processing.text() ;
		}
	}

	return m_tmp ;
}

gallery_dl::gallery_dlFilter::~gallery_dlFilter()
{
}

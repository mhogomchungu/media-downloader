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

#include "safaribooks.h"

#include "../utility.h"

engines::Json safaribooks::config( Logger&,const engines::enginePaths& ) const
{
	return QJsonParseError() ;
}

std::unique_ptr< engines::engine::functions > safaribooks::Functions() const
{
	return std::make_unique< safaribooks::functions >( m_settings ) ;
}

safaribooks::functions::~functions()
{
}

void safaribooks::functions::sendCredentials( const engines::engine& engine,
					      const QString& credentials,
					      QProcess& exe )
{
	if( credentials.isEmpty() ){

		exe.write( engine.userName().toUtf8() + "\n" ) ;
		exe.write( engine.password().toUtf8() + "\n") ;
	}else{
		auto m = utility::split( credentials,':',true ) ;

		if( m.size() > 1 ){

			exe.write( m.at( 0 ).toUtf8() + "\n" ) ;
			exe.write( m.at( 1 ).toUtf8() + "\n") ;
		}else{
			exe.write( "woof@bar.com\nmeaw\n" ) ;
		}
	}

	exe.closeWriteChannel() ;
}

void safaribooks::functions::processData( QStringList& outPut,QByteArray data )
{
	data.replace( "\033[0m","" ) ;
	data.replace( "\033[33m","" ) ;
	data.replace( "\033[41m","" ) ;
	data.replace( "\033[43m","" ) ;

	for( const auto& m : utility::split( data,'\r' ) ){

		for( const auto& e : utility::split( m,'\n' ) ){

			if( e.isEmpty() ){

				continue ;

			}else if( e.endsWith( "%" ) ){

				auto& s = outPut.last() ;

				if( s.endsWith( "%" ) ){

					s = e ;
				}else{
					outPut.append( e ) ;
				}
			}else{
				outPut.append( e ) ;
			}
		}
	}
}

void safaribooks::functions::updateDownLoadCmdOptions( const engines::engine& engine,
						       const QString& quality,
						       const QStringList& userOptions,
						       QStringList& urls,
						       QStringList& ourOptions )
{
	Q_UNUSED( userOptions )
	Q_UNUSED( engine )
	Q_UNUSED( quality )

	if( urls.size() > 0 ){

		urls[ 0 ] = utility::split( urls[ 0 ],'/',true ).last() ;
	}

	ourOptions.append( "--login" ) ;
	ourOptions.append( "--destination" ) ;
	ourOptions.append( m_settings.downloadFolder() ) ;
}

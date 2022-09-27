/*
 *
 *  Copyright (c) 2022
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

#include "wget.h"
#include "../utility.h"

wget::wget( const engines& e,const engines::engine& s,QJsonObject& ) :
	engines::engine::functions( e.Settings(),s,e.processEnvironment() )
{
}

void wget::updateDownLoadCmdOptions( const engines::engine::functions::updateOpts& s )
{
	engines::engine::functions::updateDownLoadCmdOptions( s ) ;

	if( !s.ourOptions.contains( "--progress=bar:force" ) ){

		s.ourOptions.append( "--progress=bar:force" ) ;
	}
}

engines::engine::functions::DataFilter wget::Filter( int id,const QString& e )
{
	const auto& engine = engines::engine::functions::engine() ;

	return { util::types::type_identity< wget::wgetFilter >(),e,engine,id } ;
}

QString wget::updateTextOnCompleteDownlod( const QString& uiText,
					   const QString& bkText,
					   const QString& dopts,
					   const finishedState& f )
{
	if( f.cancelled() ){

		return engines::engine::functions::updateTextOnCompleteDownlod( bkText,dopts,f ) ;

	}else if( f.success() ){

		const auto& m = [ & ](){

			if( uiText.isEmpty() ){

				return bkText ;
			}else{
				return uiText ;
			}
		}() ;

		return engines::engine::functions::updateTextOnCompleteDownlod( m,dopts,f ) ;
	}else{
		auto m = engines::engine::functions::processCompleteStateText( f ) ;
		return m + "\n" + bkText ;
	}
}

wget::~wget()
{
}

wget::wgetFilter::wgetFilter( const QString& e,const engines::engine& engine,int id ) :
	engines::engine::functions::filter( e,engine,id )
{
}

const QByteArray& wget::wgetFilter::operator()( const Logger::Data& e )
{
	if( e.doneDownloading() ){

		return m_title ;
	}

	auto line = e.toString() ;

	if( !line.contains( "Saving to: " ) ){

		return m_preProcessing.text() ;
	}

	if( m_title.isEmpty() ){

		for( const auto& it : util::split( line,'\n' ) ){

			if( it.startsWith( "Saving to: " ) ){

				m_title = it.mid( 11 ) ;
				m_title.replace( "‘","" ) ;
				m_title.replace( "’","" ) ;
				m_title.replace( "'","" ) ;
				m_title.replace( "'","" ) ;

				break ;
			}
		}
	}

	if( e.lastLineIsProgressLine() ){

		if( m_title.isEmpty() ){

			return e.lastText() ;
		}else{
			const auto m = e.lastText() ;

			auto s = m.indexOf( "%[" ) ;

			if( s != -1 ){

				auto l = m.mid( s ) ;

				for( int i = s - 1 ; i >= 0 ; i-- ){

					if( m[ i ] != ' ' ){

						l.prepend( m[ i ] ) ;
					}else{
						break ;
					}
				}

				auto a = l.indexOf( '[' ) ;

				auto b = l.indexOf( ']' ) ;

				if( a != -1 && b != -1 ){

					auto aa = l.mid( 0,a ) ;

					auto bb = l.mid( b + 1 ) ;

					m_tmp = m_title + "\n" + aa + " " + bb ;
				}else{
					m_tmp = m_title + "\n" + m_preProcessing.text() ;
				}
			}else{
				auto b = m.indexOf( ']' ) ;

				if( b != -1 ){

					m_tmp = m_title + "\n" + m.mid( b + 1 ) ;
				}else{
					return m_preProcessing.text() ;
				}
			}

			return m_tmp ;
		}
	}else{
		if( m_title.isEmpty() ){

			return m_preProcessing.text() ;
		}else{
			m_tmp = m_title + "\n" + m_preProcessing.text() ;
			return m_tmp ;
		}
	}
}

wget::wgetFilter::~wgetFilter()
{
}

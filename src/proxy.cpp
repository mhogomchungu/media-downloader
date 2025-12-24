/*
 *
 *  Copyright (c) 2023
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

#include "proxy.h"
#include "utils/qprocess.hpp"
#include "utility.h"

static QString _find_proxy( const QProcessEnvironment& )
{
	return {} ;
}

template< typename ... Args >
static QString _find_proxy( const QProcessEnvironment& env,const char * first,Args&& ... rest )
{
	auto m = env.value( first ) ;

	if( m.isEmpty() ){

		return _find_proxy( env,std::forward< Args >( rest ) ... ) ;
	}else{
		return m ;
	}
}

static QString _proxy_find( const Context& ctx )
{
	const auto& env = ctx.Engines().processEnvironment() ;

	return _find_proxy( env,"all_proxy","ALL_PROXY","https_proxy","http_proxy","HTTPS_PROXY","HTTP_PROXY" ) ;
}

static QByteArray _hex_to_decimal( const QByteArray& mm )
{
	auto m = mm.toLower() ;

	auto a = m[ 0 ] ;
	auto b = m[ 1 ] ;

	int r ;

	if( a < 'a' ){

		r = 16 * ( a - '0' ) ;
	}else{
		r = 16 * ( a - 'a' + 10 ) ;
	}

	if( b < 'a' ){

		r += b - '0' ;
	}else{
		r += b - 'a' + 10 ;
	}

	return QByteArray::number( r ) ;
}

static QByteArray _ip_address( const QByteArray& e )
{
	auto a = _hex_to_decimal( e.mid( 0,2 ) ) ;
	auto b = _hex_to_decimal( e.mid( 2,2 ) ) ;
	auto c = _hex_to_decimal( e.mid( 4,2 ) ) ;
	auto d = _hex_to_decimal( e.mid( 6,2 ) ) ;

	return d + "." + c + "." + b + "." + a ;
}

static void _get_proxy_from_gateway_linux( Context& ctx,const QByteArray& addr,bool firstTime )
{
	QFile file( "/proc/net/route" ) ;

	if( !file.open( QIODevice::ReadOnly ) ){

		return ;
	}

	const auto mm = util::split( file.readAll(),'\n' ) ;

	for( const auto& it : mm ){

		auto s = util::split( it,'\t' ) ;

		if( s.size() > 1 && s[ 1 ] == "00000000" ){

			const auto& m = s[ 2 ] ;

			if( m.size() == 8 ){

				QString s = addr ;

				s.replace( "${gateway}",_ip_address( m ) ) ;

				ctx.setNetworkProxy( s,firstTime ) ;
			}else{
				ctx.setNetworkProxy( firstTime ) ;
			}

			return ;
		}
	}

	ctx.setNetworkProxy( firstTime ) ;
}

static void _get_proxy_from_gateway_win( Context& ctx,const QByteArray& addr,bool firstTime )
{
	auto m = utility::windowsGateWayAddress() ;

	if( m.isEmpty() ){

		ctx.setNetworkProxy( firstTime ) ;
	}else{
		QString s = addr ;

		s.replace( "${gateway}",m ) ;

		ctx.setNetworkProxy( s,firstTime ) ;
	}
}

using mm = settings::proxySettings ;

void proxy::set( Context& ctx,bool firstTime,const QByteArray& proxyAddress,const mm::type& m )
{
	if( utility::platformIsWindows() && m.system() ){

		class meaw
		{
		public:
			meaw( Context& ctx,bool firstTime ) :
				m_ctx( ctx ),m_firstTime( firstTime )
			{
			}
			QList< QNetworkProxy > bg()
			{
				return QNetworkProxyFactory::systemProxyForQuery() ;
			}
			void fg( const QList< QNetworkProxy >& m )
			{
				for( const auto& it : m ){

					if( !it.hostName().isEmpty() ){

						return m_ctx.setNetworkProxy( it,m_firstTime ) ;
					}
				}

				m_ctx.setNetworkProxy( m_firstTime ) ;
			}
		private:
			Context& m_ctx ;
			bool m_firstTime ;
		} ;

		utils::qthread::run( meaw( ctx,firstTime ) ) ;

	}else if( m.none() ){

		ctx.setNetworkProxy( firstTime ) ;

	}else if( m.env() ){

		ctx.setNetworkProxy( _proxy_find( ctx ),firstTime ) ;

	}else if( proxyAddress.contains( "${gateway}" ) ){

		if( utility::platformIsLinux() ){

			_get_proxy_from_gateway_linux( ctx,proxyAddress,firstTime ) ;

		}else if( utility::platformIsWindows() ){

			_get_proxy_from_gateway_win( ctx,proxyAddress,firstTime ) ;
		}else{
			ctx.setNetworkProxy( firstTime ) ;
		}
	}else{
		ctx.setNetworkProxy( proxyAddress,firstTime ) ;
	}
}

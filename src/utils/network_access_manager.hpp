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

#ifndef NetworkAccessManager_H
#define NetworkAccessManager_H

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>

#include <QMutex>
#include <QTimer>

#include <memory>

namespace utils
{
	namespace network
	{
		namespace details
		{
			#if __cplusplus >= 201703L
				template<typename Function,typename ... Args>
				using result_of = std::invoke_result_t<Function,Args ...> ;
			#else
				template<typename Function,typename ... Args>
				using result_of = std::result_of_t<Function(Args ...)> ;
			#endif

			class NetworkReply
			{
			public:
				NetworkReply( QNetworkReply& n,bool f,bool t ) :
					m_networkReply( n ),m_finished( f ),m_timeOut( t )
				{
				}
				bool finished() const
				{
					return m_finished ;
				}
				bool success() const
				{
					return m_networkReply.error() == QNetworkReply::NoError && !m_timeOut ;
				}
				bool timeOut() const
				{
					return m_timeOut ;
				}
				QByteArray data() const
				{
					return m_networkReply.readAll() ;
				}
				QNetworkReply::NetworkError error() const
				{
					return m_networkReply.error() ;
				}
				QString errorString() const
				{
					return m_networkReply.errorString() ;
				}
				QNetworkReply& networkReply() const
				{
					return m_networkReply ;
				}
				bool retry() const
				{
					auto error = this->error() ;

					return error == QNetworkReply::TemporaryNetworkFailureError ||
					       error == QNetworkReply::NetworkSessionFailedError ;
				}
			private:
				QNetworkReply& m_networkReply ;
				bool m_finished ;
				bool m_timeOut ;
			};
		}
		class progress : public details::NetworkReply
		{
		public:
			progress( bool finished,bool timeOut,QNetworkReply& networkReply,qint64 r,qint64 t ) :
				details::NetworkReply( networkReply,finished,timeOut ),
				m_received( r ),
				m_total( t )
			{
			}
			qint64 received() const
			{
				return m_received ;
			}
			qint64 total() const
			{
				return m_total ;
			}
		private:
			qint64 m_received ;
			qint64 m_total ;
		} ;
		class reply : public details::NetworkReply
		{
		public:
			reply( QNetworkReply& n,bool timeOut ) : details::NetworkReply( n,true,timeOut )
			{
			}
		} ;
		class manager
		{
		public:
			manager( int timeOut ) : m_timeOut( timeOut )
			{
			}
			QNetworkAccessManager& QtNAM() const
			{
				return m_manager ;
			}
			template< typename Reply,
				  typename std::enable_if< std::is_void< details::result_of< Reply,network::reply > >::value,int >::type = 0 >
			void get( const QNetworkRequest& r,Reply reply ) const
			{
				this->setupReply( m_manager.get( r ),std::move( reply ) ) ;
			}
			template< typename Progress,
				  typename std::enable_if< std::is_void< details::result_of< Progress,network::progress > >::value,int >::type = 0 >
			void get( const QNetworkRequest& r,Progress progress ) const
			{
				auto reply = []( const network::reply& ){} ;

				this->setupReply( m_manager.get( r ),std::move( reply ),std::move( progress ) ) ;
			}
			template< typename Reply,typename Data >
			void post( const QNetworkRequest& r,const Data& e,Reply reply ) const
			{
				this->setupReply( m_manager.post( r,e ),std::move( reply ) ) ;
			}
			template< typename Reply >
			void head( const QNetworkRequest& r,Reply reply ) const
			{
				this->setupReply( m_manager.head( r ),std::move( reply ) ) ;
			}
		private:
			template< typename Reply,typename Progress >
			class handle
			{
			public:
				handle( Reply&& r,Progress&& p,QNetworkReply& n,QMutex& m ) :
					m_reply( std::move( r ) ),
					m_progress( std::move( p ) ),
					m_mutex( m ),
					m_networkReply( n )
				{
				}
				void result( bool timeOut )
				{
					QObject::disconnect( m_networkConn ) ;
					QObject::disconnect( m_timerConn ) ;
					m_timer.stop() ;
					m_reply( { m_networkReply,timeOut } ) ;
					m_progress( { true,timeOut,m_networkReply,0,0 } ) ;
				}
				bool firstSeen()
				{
					QMutexLocker m( &m_mutex ) ;

					auto s = m_firstSeen ;

					if( m_firstSeen ){

						m_firstSeen = false ;
					}

					return s ;
				}
				void start( int timeOut,QMetaObject::Connection&& nc,QMetaObject::Connection&& tc )
				{
					m_networkConn = std::move( nc ) ;
					m_timerConn = std::move( tc ) ;
					m_timer.start( timeOut ) ;
				}
				QNetworkReply * networkReply()
				{
					return &m_networkReply ;
				}
				QTimer * timer()
				{
					return &m_timer ;
				}
				void stopTimer()
				{
					if( m_stopTimer ){

						m_timer.stop() ;
						m_stopTimer = false ;
					}
				}
				void progress( qint64 r,qint64 t )
				{
					if( r != 0 ){

						m_progress( { false,false,m_networkReply,r,t } ) ;
					}
				}
				~handle()
				{
					m_networkReply.deleteLater() ;
				}
			private:
				bool m_firstSeen = true ;
				bool m_stopTimer = true ;
				QTimer m_timer ;
				Reply m_reply ;
				Progress m_progress ;
				QMutex& m_mutex ;
				QNetworkReply& m_networkReply ;
				QMetaObject::Connection m_networkConn ;
				QMetaObject::Connection m_timerConn ;
			} ;
			template< typename Reply,typename Progress,typename Function >
			void setupReply( QNetworkReply * s,Reply&& reply,Progress&& progress,Function&& function ) const
			{
				auto hdl = std::make_shared< handle< Reply,Progress > >( std::move( reply ),std::move( progress ),*s,m_mutex ) ;

				QObject::connect( s,&QNetworkReply::downloadProgress,[ &h = *hdl,function = std::move( function ) ]( qint64 r,qint64 t ){

					h.stopTimer() ;

					function( h,r,t ) ;
				} ) ;

				hdl->start( m_timeOut,QObject::connect( hdl->networkReply(),&QNetworkReply::finished,[ hdl ](){

					if( hdl->firstSeen() ){

						hdl->result( false ) ;
					}

				} ),QObject::connect( hdl->timer(),&QTimer::timeout,[ hdl ](){

					if( hdl->firstSeen() ){

						hdl->result( true ) ;

						hdl->networkReply()->abort() ;
					}
				} ) ) ;
			}
			template< typename Reply,typename Progress >
			void setupReply( QNetworkReply * s,Reply&& reply,Progress&& progress ) const
			{
				using handle_t = handle< Reply,Progress > ;

				this->setupReply( s,std::move( reply ),std::move( progress ),[]( handle_t& h,qint64 r,qint64 t ){

					h.progress( r,t ) ;
				} ) ;
			}
			template< typename Reply >
			void setupReply( QNetworkReply * s,Reply&& reply ) const
			{
				struct progress
				{
					void operator()( const network::progress& ){}
				} ;

				using handle_t = handle< Reply,progress > ;

				this->setupReply( s,std::move( reply ),progress(),[]( handle_t&,qint64,qint64 ){} ) ;
			}

			mutable QNetworkAccessManager m_manager ;
			int m_timeOut ;
			mutable QMutex m_mutex ;
		} ;
	}
}

#endif

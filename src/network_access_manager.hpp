/*
 * copyright: 2023
 * name : Francis Banyikwa
 * email: mhogomchungu@gmail.com
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE
 * COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef NetworkAccessManager_H
#define NetworkAccessManager_H

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>

#include <QMutex>
#include <QTimer>

#include <memory>

class NetworkAccessManager
{
#if __cplusplus >= 201703L
	template<typename Function,typename ... Args>
	using result_of = std::invoke_result_t<Function,Args ...> ;
#else
	template<typename Function,typename ... Args>
	using result_of = std::result_of_t<Function(Args ...)> ;
#endif
public:
	class progress
	{
	public:
		progress( bool finished,bool timeOut,QNetworkReply& networkReply,qint64 r,qint64 t,QByteArray&& data ) :
			m_finished( finished ),
			m_timeOut( timeOut ),
			m_networkReply( networkReply ),
			m_received( r ),
			m_total( t ),
			m_data( std::move( data ) )
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
			return std::move( const_cast< progress * >( this )->m_data ) ;
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
		qint64 received() const
		{
			return m_received ;
		}
		qint64 total() const
		{
			return m_total ;
		}
	private:
		bool m_finished ;
		bool m_timeOut ;
		QNetworkReply& m_networkReply ;
		qint64 m_received ;
		qint64 m_total ;
		QByteArray m_data ;
	} ;
	class reply
	{
	public:
		reply( QNetworkReply& n,bool t,QByteArray&& d ) :
			m_data( std::move( d ) ),
			m_networkReply( n ),
			m_timeOut( t )
		{
		}
		bool finished() const
		{
			return true ;
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
			return std::move( const_cast< reply * >( this )->m_data ) ;
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
	private:
		QByteArray m_data ;
		QNetworkReply& m_networkReply ;
		bool m_timeOut ;
	} ;
	NetworkAccessManager( int timeOut ) : m_timeOut( timeOut )
	{
	}
	QNetworkAccessManager& QtNAM()
	{
		return m_manager ;
	}
	template< typename Reply,
		  typename std::enable_if< std::is_void< result_of< Reply,NetworkAccessManager::reply > >::value,int >::type = 0 >
	void get( const QNetworkRequest& r,Reply reply )
	{
		this->setupReply( m_manager.get( r ),std::move( reply ) ) ;
	}
	template< typename Progress,
		  typename std::enable_if< std::is_void< result_of< Progress,NetworkAccessManager::progress > >::value,int >::type = 0 >
	void get( const QNetworkRequest& r,Progress progress )
	{
		auto reply = []( const NetworkAccessManager::reply& ){} ;

		this->setupReply( m_manager.get( r ),std::move( reply ),std::move( progress ) ) ;
	}
	template< typename Reply,typename Data >
	void post( const QNetworkRequest& r,const Data& e,Reply reply )
	{
		this->setupReply( m_manager.post( r,e ),std::move( reply ) ) ;
	}
	template< typename Reply >
	void head( const QNetworkRequest& r,Reply reply )
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
			m_reply( NetworkAccessManager::reply( m_networkReply,timeOut,std::move( m_data ) ) ) ;
			m_progress( NetworkAccessManager::progress( true,timeOut,m_networkReply,0,0,{} ) ) ;
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
		void addData()
		{
			this->stopTimer() ;

			m_data += m_networkReply.readAll() ;
		}
		void progress( qint64 r,qint64 t )
		{
			this->stopTimer() ;

			m_progress( NetworkAccessManager::progress( false,false,m_networkReply,r,t,m_networkReply.readAll() ) ) ;
		}
		~handle()
		{
			m_networkReply.deleteLater() ;
		}
	private:
		void stopTimer()
		{
			if( m_stopTimer ){

				m_timer.stop() ;
				m_stopTimer = false ;
			}
		}
		QByteArray m_data ;
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
	void setupReply( QNetworkReply * s,Reply&& reply,Progress&& progress,Function&& function )
	{
		auto hdl = std::make_shared< handle< Reply,Progress > >( std::move( reply ),std::move( progress ),*s,m_mutex ) ;

		QObject::connect( s,&QNetworkReply::downloadProgress,[ &h = *hdl,function = std::move( function ) ]( qint64 r,qint64 t ){

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
	void setupReply( QNetworkReply * s,Reply&& reply,Progress&& progress )
	{
		using handle_t = handle< Reply,Progress > ;

		this->setupReply( s,std::move( reply ),std::move( progress ),[]( handle_t& h,qint64 r,qint64 t ){

			h.progress( r,t ) ;
		} ) ;
	}
	template< typename Reply >
	void setupReply( QNetworkReply * s,Reply&& reply )
	{
		struct progress
		{
			void operator()( const NetworkAccessManager::progress& ){}
		} ;

		using handle_t = handle< Reply,progress > ;

		this->setupReply( s,std::move( reply ),progress(),[]( handle_t& h,qint64,qint64 ){

			h.addData() ;
		} ) ;
	}

	QNetworkAccessManager m_manager ;
	int m_timeOut ;
	QMutex m_mutex ;
} ;

#endif

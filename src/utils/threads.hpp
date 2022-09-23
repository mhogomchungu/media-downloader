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

#include <QThread>
#include <QEventLoop>

#include <type_traits>

namespace utils
{
	namespace qthread
	{
		namespace details{
			#if __cplusplus >= 201703L
				template<typename Function>
				using result_of = std::invoke_result_t<Function> ;
			#else
				template<typename Function>
				using result_of = std::result_of_t< Function() > ;
			#endif
		}

		template< typename BackGroundTask,
			  typename UiThreadResult,
			  typename std::enable_if< !std::is_void< details::result_of< BackGroundTask > >::value,int >::type = 0 >
		void run( BackGroundTask bgt,UiThreadResult fgt )
		{
			using bgt_t = details::result_of< BackGroundTask > ;

			class Thread : public QThread
			{
			public:
				Thread( BackGroundTask&& bgt,UiThreadResult&& fgt ) :
					m_bgt( std::move( bgt ) ),
					m_fgt( std::move( fgt ) )
				{
					connect( this,&QThread::finished,this,&Thread::then,Qt::QueuedConnection ) ;

					this->start() ;
				}
				void run() override
				{
					m_pointer = new ( &m_storage ) bgt_t( m_bgt() ) ;
				}
			private:
				void then()
				{
					m_fgt( std::move( *m_pointer ) ) ;

					m_pointer->~bgt_t() ;

					this->deleteLater() ;
				}
				BackGroundTask m_bgt ;
				UiThreadResult m_fgt ;

		#if __cplusplus >= 201703L
				alignas( bgt_t ) std::byte m_storage[ sizeof( bgt_t ) ] ;
		#else
				typename std::aligned_storage< sizeof( bgt_t ),alignof( bgt_t ) >::type m_storage ;
		#endif
				bgt_t * m_pointer ;
			};

			new Thread( std::move( bgt ),std::move( fgt ) ) ;
		}

		template< typename BackGroundTask,
			  typename UiThreadResult,
			  typename std::enable_if< std::is_void< details::result_of< BackGroundTask > >::value,int >::type = 0 >
		void run( BackGroundTask bgt,UiThreadResult fgt )
		{
			run( [ bgt = std::move( bgt ) ](){

				bgt() ;

				return 0 ;

			},[ fgt = std::move( fgt ) ]( int ){

				fgt() ;
			} ) ;
		}

		template< typename BackGroundTask >
		void run( BackGroundTask bgt )
		{
			run( [ bgt = std::move( bgt ) ](){

				bgt() ;

				return 0 ;

			},[]( int ){} ) ;
		}

		template< typename BackGroundTask,
			  typename std::enable_if< !std::is_void< details::result_of< BackGroundTask > >::value,int >::type = 0 >
		details::result_of< BackGroundTask > await( BackGroundTask bgt )
		{
			using bgt_t = details::result_of< BackGroundTask > ;

			QEventLoop loop ;

			class Handle
			{
			public:
				void set( bgt_t&& h )
				{
					m_pointer = new ( &m_storage ) bgt_t( std::move( h ) ) ;
				}
				~Handle()
				{
					m_pointer->~bgt_t() ;
				}
				bgt_t value()
				{
					return std::move( *m_pointer ) ;
				}
			private:
				bgt_t * m_pointer ;
				#if __cplusplus >= 201703L
					alignas( bgt_t ) std::byte m_storage[ sizeof( bgt_t ) ] ;
				#else
					typename std::aligned_storage< sizeof( bgt_t ),alignof( bgt_t ) >::type m_storage ;
				#endif
			} handle ;

			run( std::move( bgt ),[ & ]( bgt_t&& r ){

				handle.set( std::move( r ) ) ;
				loop.quit() ;
			} ) ;

			loop.exec() ;

			return handle.value() ;
		}
	}
}

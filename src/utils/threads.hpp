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

#pragma once

namespace utils
{
	namespace qthread
	{
		namespace details{
			#if __cplusplus >= 201703L
				template< typename Function,typename ... Args >
				using result_of = std::invoke_result_t< Function,Args ... > ;
			#else
				template< typename Function,typename ... Args >
				using result_of = std::result_of_t< Function( Args ... ) > ;
			#endif
		}

		template< typename T,
			  typename std::enable_if< !std::is_void< decltype( std::declval< T >().bg() ) >::value,int >::type = 0 >
		void run( T bgt )
		{
			using bgt_t = decltype( std::declval< T >().bg() ) ;

			class Thread : public QThread
			{
			public:
				Thread( T&& task ) : m_task( std::move( task ) )
				{
					connect( this,&QThread::finished,this,&Thread::then,Qt::QueuedConnection ) ;

					this->start() ;
				}
				void run() override
				{
					m_pointer = new ( &m_storage ) bgt_t( m_task.bg() ) ;
				}
			private:
				void then()
				{
					m_task.fg( std::move( *m_pointer ) ) ;

					m_pointer->~bgt_t() ;

					this->deleteLater() ;
				}

				T m_task ;

				#if __cplusplus >= 201703L
					alignas( bgt_t ) std::byte m_storage[ sizeof( bgt_t ) ] ;
				#else
					typename std::aligned_storage< sizeof( bgt_t ),alignof( bgt_t ) >::type m_storage ;
				#endif

				bgt_t * m_pointer ;
			};

			new Thread( std::move( bgt ) ) ;
		}

		template< typename T,
			 typename std::enable_if< std::is_void< decltype( std::declval< T >().bg() ) >::value,int >::type = 0 >
		void run( T bgt )
		{
			struct meaw
			{
				T m_task ;

				int bg()
				{
					m_task.bg() ;
					return 0 ;
				}
				void fg( int )
				{
					m_task.fg() ;
				}
			} ;

			run( meaw{ std::move( bgt ) } ) ;
		}

		template< typename T,
			 typename std::enable_if< std::is_same< decltype( std::declval< T >().operator()() ),decltype( std::declval< T >().operator()() ) >::value,int >::type = 0 >
		void run( T bgt )
		{
			struct meaw
			{
				T m_bg ;

				void bg()
				{
					m_bg() ;
				}
				void fg()
				{
				}
			} ;

			run( meaw{ std::move( bgt ) } ) ;
		}

		template< typename BackGroundTask,
			 typename UiThreadResult,
			 typename std::enable_if< !std::is_void< details::result_of< BackGroundTask > >::value,int >::type = 0 >
		void run( BackGroundTask bgt,UiThreadResult fgt )
		{
			using bgt_t = details::result_of< BackGroundTask > ;

			struct meaw
			{
				BackGroundTask m_bg ;
				UiThreadResult m_fg ;

				bgt_t bg()
				{
					return m_bg() ;
				}
				void fg( bgt_t&& s )
				{
					m_fg( std::move( s ) ) ;
				}
			} ;

			run( meaw{ std::move( bgt ),std::move( fgt ) } ) ;
		}

		template< typename BackGroundTask,
			  typename UiThreadResult,
			  typename std::enable_if< std::is_void< details::result_of< BackGroundTask > >::value,int >::type = 0 >
		void run( BackGroundTask bgt,UiThreadResult fgt )
		{
			struct meaw
			{
				BackGroundTask m_bg ;
				UiThreadResult m_fg ;

				void bg()
				{
					m_bg() ;
				}
				void fg()
				{
					m_fg() ;
				}
			} ;

			run( meaw{ std::move( bgt ),std::move( fgt ) } ) ;
		}

		template< typename Object,
			  typename Method,
			  typename std::enable_if< std::is_pointer< Object >::value,int >::type = 0,
			  typename std::enable_if< std::is_member_function_pointer< Method >::value,int >::type = 0 >
		void run( Object obj,Method method )
		{			
			struct meaw
			{
				Object obj ;
				Method method ;

				void bg()
				{
					( obj->*method )() ;
				}
				void fg()
				{
				}
			} ;

			run( meaw{ obj,method } ) ;
		}

		namespace details
		{
			template< typename Obj,typename M >
			auto function( Obj o,M m )
			{
				return ( o->*m )() ;
			}
			template< typename Function,
				  typename Obj1,
				  typename M1,
				  typename Obj2,
				  typename M2,
				  typename std::enable_if< std::is_void< details::result_of< Function,Obj1,M1 > >::value,int >::type = 0 >
			void run1( Function,Obj1 obj,M1 method,Obj2 obj1,M2 method1 )
			{
				struct meaw
				{
					Obj1 obj ;
					M1 method ;
					Obj2 obj1 ;
					M2 method1 ;

					void bg()
					{
						( obj->*method )() ;
					}
					void fg()
					{
						( obj1->*method1 )() ;
					}
				} ;

				run( meaw{ obj,method,obj1,method1 } ) ;
			}
			template< typename Function,
				  typename Obj1,
				  typename M1,
				  typename Obj2,
				  typename M2,
				  typename std::enable_if< !std::is_void< details::result_of< Function,Obj1,M1 > >::value,int >::type = 0 >
			void run1( Function,Obj1 obj,M1 method,Obj2 obj1,M2 method1 )
			{
				struct meaw
				{
					Obj1 obj ;
					M1 method ;
					Obj2 obj1 ;
					M2 method1 ;

					details::result_of< Function,Obj1,M1 > bg()
					{
						return ( obj->*method )() ;
					}
					void fg( details::result_of< Function,Obj1,M1 >&& value  )
					{
						( obj1->*method1 )( std::move( value ) ) ;
					}
				} ;

				run( meaw{ obj,method,obj1,method1 } ) ;
			}
		}

		template< typename ObjectUiThread,
			  typename MethodUiThread,
			  typename ObjectBGThread,
			  typename MethodBGThread,
			  typename std::enable_if< std::is_pointer< ObjectUiThread >::value,int >::type = 0,
			  typename std::enable_if< std::is_member_function_pointer< MethodUiThread >::value,int >::type = 0,
			  typename std::enable_if< std::is_pointer< ObjectBGThread >::value,int >::type = 0,
			  typename std::enable_if< std::is_member_function_pointer< MethodBGThread >::value,int >::type = 0 >
		void run( ObjectUiThread obj,MethodUiThread method,ObjectBGThread obj1,MethodBGThread method1 )
		{
			details::run1( details::function< ObjectUiThread,MethodUiThread >,obj,method,obj1,method1 ) ;
		}

		template< typename ObjectUiThread,
			  typename MethodUiThread,
			  typename MethodBGThread,
			  typename std::enable_if< std::is_pointer< ObjectUiThread >::value,int >::type = 0,
			  typename std::enable_if< std::is_member_function_pointer< MethodUiThread >::value,int >::type = 0,
			  typename std::enable_if< std::is_member_function_pointer< MethodBGThread >::value,int >::type = 0 >
		void run( ObjectUiThread obj,MethodUiThread method,MethodBGThread method1 )
		{
			details::run1( details::function< ObjectUiThread,MethodUiThread >,obj,method,obj,method1 ) ;
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

			run( std::move( bgt ),[ & ]( bgt_t&& r )mutable{

				handle.set( std::move( r ) ) ;
				loop.quit() ;
			} ) ;

			loop.exec() ;

			return handle.value() ;
		}
	}
}

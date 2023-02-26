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

#pragma once

#include <memory>
#include <QtGlobal>

namespace utils{
	namespace misc{

		template< typename Value,typename Opt >
		bool containsAny( const Value& v,const Opt& opt )
		{
			return v.contains( opt ) ;
		}

		template< typename Value,typename Opt,typename ... Opts >
		bool containsAny( const Value& v,const Opt& opt,Opts&& ... opts )
		{
			if( v.contains( opt ) ){

				return true ;
			}else{
				return misc::containsAny( v,std::forward< Opts >( opts ) ... ) ;
			}
		}

		template< typename Value,typename Opt >
		bool containsAll( const Value& v,const Opt& opt )
		{
			return v.contains( opt ) ;
		}

		template< typename Value,typename Opt,typename ... Opts >
		bool containsAll( const Value& v,const Opt& opt,Opts&& ... opts )
		{
			if( !v.contains( opt ) ){

				return false ;
			}else{
				return misc::containsAll( v,std::forward< Opts >( opts ) ... ) ;
			}
		}

		template< typename Value,typename Opt >
		bool startsWithAny( const Value& v,const Opt& opt )
		{
			return v.startsWith( opt ) ;
		}

		template< typename Value,typename Opt,typename ... Opts >
		bool startsWithAny( const Value& v,const Opt& opt,Opts&& ... opts )
		{
			if( v.startsWith( opt ) ){

				return true ;
			}else{
				return misc::startsWithAny( v,std::forward< Opts >( opts ) ... ) ;
			}
		}
		/*
		 * This method takes a function that returns a resource,a function that deletes
		 * the resource and arguments that are to be passed to the function that returns a
		 * resource.
		 *
		 * example usecase of a function:
		 *
		 * auto woof = utility2::unique_rsc( ::fopen,::fclose,"/woof/foo/bar","r" ) ;
		 */
		template< typename Function,typename Deleter,typename ... Arguments >
		auto unique_rsc( Function&& function,Deleter&& deleter,Arguments&& ... args )
		{
			using A = std::remove_pointer_t< std::result_of_t< Function( Arguments&& ... ) > > ;
			using B = std::decay_t< Deleter > ;

			return std::unique_ptr< A,B >( function( std::forward< Arguments >( args ) ... ),
						       std::forward< Deleter >( deleter ) ) ;
		}
		template< typename T >
		class unique_ptr
		{
		public:
			template< typename U >
			unique_ptr( U&& u ) : m_handle( std::forward< U >( u ) )
			{
			}
			unique_ptr()
			{
			}
			T& operator*() const
			{
				return m_handle.operator*() ;
			}
			T * operator->() const
			{
				return m_handle.operator->() ;
			}
			T * get() const
			{
				return m_handle.get() ;
			}
		private:
		#if QT_VERSION < QT_VERSION_CHECK( 5,10,0 )
			//Old version of Qt seems to not like unique_ptr in signals
			std::shared_ptr< T > m_handle ;
		#else
			std::unique_ptr< T > m_handle ;
		#endif
		};
	}
}

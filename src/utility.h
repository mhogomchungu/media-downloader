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

#ifndef UTILITY_H
#define UTILITY_H

#include <QStringList>
#include <QString>
#include <QProcess>

#include <type_traits>

namespace utility
{
	namespace types
	{
		#if __cplusplus >= 201703L
			template<typename Function,typename ... Args>
			using result_of = std::invoke_result_t<Function,Args ...> ;
		#else
			template<typename Function,typename ... Args>
			using result_of = std::result_of_t<Function(Args ...)> ;
		#endif

		template<typename ReturnType,typename Function,typename ... Args>
		using imp = std::enable_if_t<std::is_same<result_of<Function,Args...>,ReturnType>::value,int> ;

		template<typename Function,typename ... Args>
		using has_argument = imp<result_of<Function,Args...>,Function,Args...> ;

		template<typename Function>
		using has_no_argument = imp<result_of<Function>,Function> ;
	}

	template< typename T >
	typename std::add_const<T>::type& asConst( T& t )
	{
		return t ;
	}

	template< typename T >
	void asConst( const T&& ) = delete ;

	QStringList split( const QString& e,char token = '\n' ) ;

	enum class readChannel{ stdOut,stdError } ;

	template< typename WhenCreated,
		  typename WhenDone,
		  typename WithData,
		  utility::types::has_argument< WhenDone,int,QProcess::ExitStatus > = 0 >
	void run( const QString& cmd,
		  const QStringList& args,
		  readChannel r,
		  WhenCreated whenCreated,
		  WhenDone whenDone,
		  WithData withData )
	{
		auto exe = new QProcess() ;

		whenCreated( *exe ) ;

		if( r == readChannel::stdOut ){

			QObject::connect( exe,&QProcess::readyReadStandardOutput,
					  [ exe,withData = std::move( withData ) ](){

				withData( exe->readAllStandardOutput() ) ;
			} ) ;
		}else{
			QObject::connect( exe,&QProcess::readyReadStandardError,
					  [ exe,withData = std::move( withData ) ](){

				withData( exe->readAllStandardError() ) ;
			} ) ;
		}

		auto s = static_cast< void( QProcess::* )( int,QProcess::ExitStatus ) >( &QProcess::finished ) ;

		QObject::connect( exe,s,[ exe,whenDone = std::move( whenDone ) ]( int e,QProcess::ExitStatus ss ){

			whenDone( e,ss ) ;

			exe->deleteLater() ;
		} ) ;

		exe->start( cmd,args ) ;
	}

	template< typename WhenCreated,
		  typename WhenDone,
		  typename WithData,
		  utility::types::has_argument< WhenDone > = 0 >
	void run( const QString& cmd,
		  const QStringList& args,
		  readChannel r,
		  WhenCreated whenCreated,
		  WhenDone whenDone,
		  WithData withData )
	{
		auto m = [ whenDone = std::move( whenDone ) ]( int,QProcess::ExitStatus ){ whenDone() ; } ;

		utility::run( cmd,args,r,std::move( whenCreated ),std::move( m ),std::move( withData ) ) ;
	}

	template< typename WhenDone,typename WithData >
	void run( const QString& cmd,const QStringList& args,WhenDone w,WithData p )
	{
		utility::run( cmd,args,[]( QProcess& exe ){ Q_UNUSED( exe ) },std::move( w ),std::move( p ) ) ;
	}

	template< typename Value,typename Arg >
	bool startsWith( Value& v,const Arg& arg )
	{
		return v.startsWith( arg ) ;
	}

	template< typename Value,typename Arg,typename ... Args >
	bool startsWith( Value& v,const Arg& arg,const Args& ... args )
	{
		if( utility::startsWith( v,arg ) ){

			return true ;
		}else{
			return utility::startsWith( v,args ... ) ;
		}
	}
}

#endif

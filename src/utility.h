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
#include <QMenu>
#include <QPushButton>
#include <QTimer>

#include <type_traits>
#include <memory>

#include "translator.h"

#include "ui_mainwindow.h"

class Context ;

namespace Ui
{
	class MainWindow ;
}

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

		template<typename ReturnType,typename Function,typename ... Args>
		using has_same_return_type = std::enable_if_t<std::is_same<result_of<Function,Args...>,ReturnType>::value,int> ;

		template<typename Function,typename ... Args>
		using has_argument = imp<result_of<Function,Args...>,Function,Args...> ;

		template<typename Function>
		using has_no_argument = imp<result_of<Function>,Function> ;

		template<typename Function,typename ... Args>
		using has_void_return_type = has_same_return_type<void,Function,Args...> ;

		template<typename Function,typename ... Args>
		using has_bool_return_type = has_same_return_type<bool,Function,Args...> ;

		template<typename Function,typename ... Args>
		using has_non_void_return_type = std::enable_if_t<!std::is_void<result_of<Function,Args...>>::value,int> ;
	}

	template< typename T >
	typename std::add_const<T>::type& asConst( T& t )
	{
		return t ;
	}

	template< typename T >
	void asConst( const T&& ) = delete ;

	QStringList split( const QString& e,char token,bool skipEmptyParts ) ;
	QList< QByteArray > split( const QByteArray& e,char token = '\n' ) ;

	class selectedAction
	{
	public:
		selectedAction( QAction * ac ) : m_ac( ac )
		{
		}
		bool clearOptions()
		{
			return m_ac->objectName() == translator::CLEAROPTIONS ;
		}
		bool clearScreen()
		{
			return m_ac->objectName() == translator::CLEARSCREEN ;
		}
		QString text()
		{
			return m_ac->text() ;
		}
		QString objectName()
		{
			return m_ac->objectName() ;
		}
	private:
		QAction * m_ac ;
	};

	struct args
	{
		args( const QString& e )
		{
			if( !e.isEmpty() ){

				otherOptions = utility::split( e,' ',true ) ;

				if( !otherOptions.isEmpty() ){

					quality = otherOptions.takeFirst() ;
				}
			}
		}
		QString quality ;
		QStringList otherOptions ;
	} ;

	namespace details
	{
		QMenu * sMo( const Context&,
			     const QStringList& opts,
			     bool addClear,
			     QPushButton * w ) ;
	}

	template< typename Function >
	void setMenuOptions( const Context& ctx,
			     const QStringList& opts,
			     bool addClear,
			     QPushButton * w,
			     Function function )
	{
		auto menu = details::sMo( ctx,opts,addClear,w ) ;
		QObject::connect( menu,&QMenu::triggered,std::move( function ) ) ;
	}

	template< typename WhenCreated,
		  typename WhenStarted,
		  typename WhenDone,
		  typename WithData,
		  utility::types::has_non_void_return_type< WhenCreated,QProcess& > = 0 >
	void run( const QString& cmd,
		  const QStringList& args,
		  WhenCreated whenCreated,
		  WhenStarted whenStarted,
		  WhenDone whenDone,
		  WithData withData )
	{
		auto exe = new QProcess() ;

		using type = utility::types::result_of< WhenCreated,QProcess& > ;

		auto data = std::make_shared< type >( whenCreated( *exe ) ) ;

		QObject::connect( exe,&QProcess::readyReadStandardOutput,
				  [ exe,data,withData = std::move( withData ) ](){

			withData( QProcess::ProcessChannel::StandardOutput,
				  exe->readAllStandardOutput(),*data ) ;
		} ) ;

		QObject::connect( exe,&QProcess::readyReadStandardError,
				  [ exe,data,withData = std::move( withData ) ](){

			withData( QProcess::ProcessChannel::StandardError,
				  exe->readAllStandardError(),*data ) ;
		} ) ;

		using process = void( QProcess::* )( int,QProcess::ExitStatus ) ;

		auto s = static_cast< process >( &QProcess::finished ) ;

		QObject::connect( exe,s,[ data,exe,whenDone = std::move( whenDone ) ]
				  ( int e,QProcess::ExitStatus ss ){

			whenDone( e,ss,*data ) ;

			exe->deleteLater() ;
		} ) ;

		QObject::connect( exe,&QProcess::started,
				  [ exe,whenStarted = std::move( whenStarted ) ](){

			whenStarted( *exe ) ;
		} ) ;

		exe->start( cmd,args ) ;		
	}

	template< typename WhenCreated,
		  typename WhenStarted,
		  typename WhenDone,
		  typename WithData,
		  utility::types::has_void_return_type< WhenCreated,QProcess& > = 0 >
	void run( const QString& cmd,
		  const QStringList& args,
		  WhenCreated whenCreated,
		  WhenStarted whenStarted,
		  WhenDone whenDone,
		  WithData withData )
	{
		auto exe = new QProcess() ;

		whenCreated( *exe ) ;

		QObject::connect( exe,&QProcess::readyReadStandardOutput,
				  [ exe,withData = std::move( withData ) ](){

			withData( QProcess::ProcessChannel::StandardOutput,
				  exe->readAllStandardOutput() ) ;
		} ) ;

		QObject::connect( exe,&QProcess::readyReadStandardError,
				  [ exe,withData = std::move( withData ) ](){

			withData( QProcess::ProcessChannel::StandardError,
				  exe->readAllStandardError() ) ;
		} ) ;

		using type = void( QProcess::* )( int,QProcess::ExitStatus ) ;

		auto s = static_cast< type >( &QProcess::finished ) ;

		QObject::connect( exe,s,[ exe,whenDone = std::move( whenDone ) ]
				  ( int e,QProcess::ExitStatus ss ){

			whenDone( e,ss ) ;

			exe->deleteLater() ;
		} ) ;

		QObject::connect( exe,&QProcess::started,
				  [ exe,whenStarted = std::move( whenStarted ) ](){

			whenStarted( *exe ) ;
		} ) ;

		exe->start( cmd,args ) ;
	}

	template< typename WhenDone,typename WithData >
	void run( const QString& cmd,const QStringList& args,WhenDone w,WithData p )
	{
		utility::run( cmd,args,[]( QProcess& ){},std::move( w ),std::move( p ) ) ;
	}

	/*
	 * Function must take an int and must return bool
	 */
	template< typename Function,utility::types::has_bool_return_type<Function,int > = 0 >
	void Timer( int interval,Function&& function )
	{
		class Timer{
		public:
			Timer( int interval,Function&& function ) :
				m_function( std::forward< Function >( function ) )
			{
				auto timer = new QTimer() ;

				QObject::connect( timer,&QTimer::timeout,[ timer,this ](){

					m_counter++ ;

					if( m_function( m_counter ) ){

						timer->stop() ;

						timer->deleteLater() ;

						delete this ;
					}
				} ) ;

				timer->start( interval ) ;
			}
		private:
			int m_counter = 0 ;
			Function m_function ;
		} ;

		new Timer( interval,std::forward< Function >( function ) ) ;
	}

	/*
	 * Function must takes no argument and will be called once when the interval pass
	 */
	template< typename Function,utility::types::has_no_argument< Function > = 0 >
	void Timer( int interval,Function&& function )
	{
		utility::Timer( interval,[ function = std::forward< Function >( function ) ]( int s ){

			Q_UNUSED( s )

			function() ;

			return true ;
		} ) ;
	}

	template< typename T >
	class result_ref
	{
	public:
		result_ref() : m_value( nullptr )
		{
		}
		result_ref( T e ) : m_value( std::addressof( e ) )
		{
		}
		typename std::remove_reference< T >::type * operator->() const
		{
			return m_value ;
		}
		T& value() const
		{
			return *m_value ;
		}
		T& operator*() const
		{
			return this->value() ;
		}
		bool has_value() const
		{
			return m_value ;
		}
		operator bool() const
		{
			return this->has_value() ;
		}
	private:
		typename std::remove_reference< T >::type * m_value ;
	} ;

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

	template< typename Value,typename Arg >
	bool contains( Value& v,const Arg& arg )
	{
		return v.contains( arg ) ;
	}

	template< typename Value,typename Arg,typename ... Args >
	bool contains( Value& v,const Arg& arg,const Args& ... args )
	{
		if( utility::contains( v,arg ) ){

			return true ;
		}else{
			return utility::contains( v,args ... ) ;
		}
	}

	void wait( int time ) ;
	void waitForOneSecond() ;
	QString homePath() ;
	int terminateProcess( unsigned long pid ) ;
	bool platformIsWindows() ;
	bool platformIsLinux() ;
	bool platformIsOSX() ;
	bool platformIsNOTWindows() ;

	bool hasDigitsOnly( const QString& e ) ;
}

#endif

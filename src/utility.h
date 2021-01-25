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

#include <type_traits>
#include <memory>

#include "settings.h"

#include "ui_mainwindow.h"

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
		static const QString& bestText()
		{
			static QString m( "best" ) ;
			return m ;
		}
		static const QString& clearOptionTextTr()
		{
			static QString m( QObject::tr( "Clear Options" ) ) ;

			return m ;
		}
		static const QString& clearScreenTextTr()
		{
			static QString m( QObject::tr( "Clear Screen" ) ) ;

			return m ;
		}
		static const QString& clearOptionText()
		{
			static QString m( "Clear Options" ) ;

			return m ;
		}
		static const QString& clearScreenText()
		{
			static QString m( "Clear Screen" ) ;

			return m ;
		}
		selectedAction( QAction * ac ) :
			m_objectName( ac->objectName() ),
			m_text( ac->text() )
		{
		}
		bool clearOptions()
		{
			return this->objectName() == clearOptionText() ;
		}
		bool clearScreen()
		{
			return this->objectName() == clearScreenText() ;
		}
		bool best()
		{
			return this->text() == "Best" ;
		}
		const QString& text()
		{
			return m_text ;
		}
		const QString& objectName()
		{
			return m_objectName ;
		}
	private:
		QString m_objectName ;
		QString m_text ;
	};

	struct args
	{
		args( const QString& e )
		{
			if( e.isEmpty() ){

				quality = utility::selectedAction::bestText() ;
			}else{
				otherOptions = utility::split( e,' ',true ) ;

				if( otherOptions.isEmpty() ){

					quality = utility::selectedAction::bestText() ;
				}else{
					quality = otherOptions.takeFirst() ;
				}
			}
		}
		QString quality ;
		QStringList otherOptions ;
	} ;

	template< typename Function >
	void setMenuOptions( settings * settings,bool addClear,QPushButton * w,Function function )
	{
		auto m = w->menu() ;

		if( m ){

			m->deleteLater() ;
		}

		const auto entries = settings->presetOptionsList() ;

		auto menu = new QMenu( w ) ;

		for( const auto& it : entries ){

			auto a = it ;

			auto b = a.lastIndexOf( '(' ) ;

			if( b != -1 ){

				auto m = a.mid( 0,b ) ;
				auto mm = a.mid( b + 1 ) ;
				mm.truncate( mm.size() - 1 ) ;
				menu->addAction( m )->setObjectName( mm ) ;
			}else{
				menu->addAction( it )->setObjectName( it ) ;
			}
		}

		if( addClear ){

			menu->addSeparator() ;

			const auto& cotr = selectedAction::clearOptionTextTr() ;
			const auto& co = selectedAction::clearOptionTextTr() ;

			const auto& cstr = selectedAction::clearScreenTextTr() ;
			const auto& cs = selectedAction::clearScreenText() ;

			menu->addAction( cotr )->setObjectName( co ) ;
			menu->addAction( cstr )->setObjectName( cs ) ;
		}

		QObject::connect( menu,&QMenu::triggered,std::move( function ) ) ;

		w->setMenu( menu ) ;
	}

	template< typename WhenCreated,
		  typename WhenDone,
		  typename WithData,
		  utility::types::has_non_void_return_type< WhenCreated,QProcess& > = 0 >
	void run( const QString& cmd,
		  const QStringList& args,
		  WhenCreated whenCreated,
		  WhenDone whenDone,
		  WithData withData )
	{
		auto exe = new QProcess() ;

		using type = utility::types::result_of< WhenCreated,QProcess& > ;

		auto data = std::make_shared< type >( whenCreated( *exe ) ) ;

		QObject::connect( exe,&QProcess::readyReadStandardOutput,
				  [ exe,data,withData = std::move( withData ) ](){

			withData( QProcess::ProcessChannel::StandardOutput,exe->readAllStandardOutput(),*data ) ;
		} ) ;

		QObject::connect( exe,&QProcess::readyReadStandardError,
				  [ exe,data,withData = std::move( withData ) ](){

			withData( QProcess::ProcessChannel::StandardError,exe->readAllStandardError(),*data ) ;
		} ) ;

		auto s = static_cast< void( QProcess::* )( int,QProcess::ExitStatus ) >( &QProcess::finished ) ;

		QObject::connect( exe,s,[ data,exe,whenDone = std::move( whenDone ) ]( int e,QProcess::ExitStatus ss ){

			whenDone( e,ss,*data ) ;

			exe->deleteLater() ;
		} ) ;

		exe->start( cmd,args ) ;
	}

	template< typename WhenCreated,
		  typename WhenDone,
		  typename WithData,
		  utility::types::has_void_return_type< WhenCreated,QProcess& > = 0 >
	void run( const QString& cmd,
		  const QStringList& args,
		  WhenCreated whenCreated,
		  WhenDone whenDone,
		  WithData withData )
	{
		auto exe = new QProcess() ;

		whenCreated( *exe ) ;

		QObject::connect( exe,&QProcess::readyReadStandardOutput,
				  [ exe,withData = std::move( withData ) ](){

			withData( QProcess::ProcessChannel::StandardOutput,exe->readAllStandardOutput() ) ;
		} ) ;

		QObject::connect( exe,&QProcess::readyReadStandardError,
				  [ exe,withData = std::move( withData ) ](){

			withData( QProcess::ProcessChannel::StandardError,exe->readAllStandardError() ) ;
		} ) ;

		auto s = static_cast< void( QProcess::* )( int,QProcess::ExitStatus ) >( &QProcess::finished ) ;

		QObject::connect( exe,s,[ exe,whenDone = std::move( whenDone ) ]( int e,QProcess::ExitStatus ss ){

			whenDone( e,ss ) ;

			exe->deleteLater() ;
		} ) ;

		exe->start( cmd,args ) ;
	}

	template< typename WhenDone,typename WithData >
	void run( const QString& cmd,const QStringList& args,WhenDone w,WithData p )
	{
		utility::run( cmd,args,[]( QProcess& ){},std::move( w ),std::move( p ) ) ;
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

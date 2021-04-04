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

#include "engines.h"

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
	QStringList split( const QString& e,const char * token ) ;
	QList< QByteArray > split( const QByteArray& e,char token = '\n' ) ;
	QList< QByteArray > split( const QByteArray& e,QChar token = '\n' ) ;

	class selectedAction
	{
	public:
		static const char * CLEARSCREEN ;
		static const char * CLEAROPTIONS ;
		static const char * OPENFOLDER ;

		selectedAction( QAction * ac ) : m_ac( ac )
		{
		}
		bool clearOptions() const
		{
			return m_ac->objectName() == utility::selectedAction::CLEAROPTIONS ;
		}
		bool clearScreen() const
		{
			return m_ac->objectName() == utility::selectedAction::CLEARSCREEN ;
		}
		bool openFolderPath() const
		{
			return m_ac->objectName() == utility::selectedAction::OPENFOLDER ;
		}
		QString text() const
		{
			return m_ac->text() ;
		}
		QString objectName() const
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

	template< typename Object,
		  typename ObjectMemberFunction,
		  typename Function >
	struct Conn
	{
		Conn( Object obj,ObjectMemberFunction pointer,Function function ) :
			obj( obj ),
			pointer( pointer ),
			function( std::move( function ) )
		{
		}
		Object obj ;
		ObjectMemberFunction pointer ;
		Function function ;
	};

	template< typename Object,
		  typename ObjectMemberFunction,
		  typename Function >
	static auto make_conn( Object obj,ObjectMemberFunction memFunction,Function function )
	{
		return Conn< Object,ObjectMemberFunction,Function >( obj,memFunction,std::move( function ) ) ;
	}
	template< typename Tlogger,
		  typename Options >
	class context
	{
	public:
		context( const engines::engine& engine,
			 Tlogger logger,
			 Options options,
			 bool list_requested ) :
			m_engine( engine ),
			m_list_requested( list_requested ),
			m_logger( std::move( logger ) ),
			m_postData( true ),
			m_options( std::move( options ) )
		{
		}
		void setCancelConnection( QMetaObject::Connection conn )
		{
			m_conn = std::move( conn ) ;
		}
		void stopReceivingData()
		{
			m_postData = false ;
		}
		void postData( QByteArray data )
		{
			if( m_postData ){

				m_data += data ;

				m_logger.add( [ this,data = std::move( data ) ]( QStringList& e ){

					m_engine.processData( e,std::move( data ) ) ;
				} ) ;
			}
		}
		template< typename Function >
		void listRequested( Function function )
		{
			if( m_list_requested ){

				function( utility::split( m_data,'\n' ) ) ;
			}
		}
		void disconnect()
		{
			QObject::disconnect( m_conn ) ;
		}
		Options& options()
		{
			return m_options ;
		}
	private:
		const engines::engine& m_engine ;
		bool m_list_requested ;
		QMetaObject::Connection m_conn ;
		Tlogger m_logger ;
		QByteArray m_data ;
		bool m_postData ;
		Options m_options ;
	} ;

	template< typename Connection,
		  typename Tlogger,
		  typename Options >
	void run( const engines::engine& engine,
		  const QStringList& args,
		  const QString& quality,
		  bool list_requested,
		  Options options,
		  Tlogger logger,
		  Connection conn )
	{
		options.tabManagerEnableAll( false ) ;

		engines::engine::exeArgs::cmd cmd( engine.exePath(),args ) ;

		utility::run( cmd.exe(),cmd.args(),[ &,logger = std::move( logger ),options = std::move( options ) ]( QProcess& exe )mutable{

			exe.setProcessEnvironment( options.processEnvironment() ) ;

			logger.add( "cmd: " + engine.commandString( cmd ) ) ;

			exe.setWorkingDirectory( options.downloadFolder() ) ;

			exe.setProcessChannelMode( QProcess::ProcessChannelMode::MergedChannels ) ;

			auto ctx = std::make_shared< utility::context< Tlogger,Options > >( engine,std::move( logger ),std::move( options ),list_requested ) ;

			ctx->setCancelConnection( QObject::connect( conn.obj,conn.pointer,
					[ &exe,ctx,function = std::move( conn.function ) ](){

				ctx->stopReceivingData() ;

				function( exe ) ;
			} ) ) ;

			return ctx ;

		},[ &options,&engine,quality ]( QProcess& exe ){

			engine.sendCredentials( quality,exe ) ;

		},[]( int,QProcess::ExitStatus,std::shared_ptr< utility::context< Tlogger,Options > >& ctx ){

			ctx->disconnect() ;

			ctx->listRequested( [ & ]( const QList< QByteArray >& e ){

				ctx->options().listRequested( e ) ;
			} ) ;

			ctx->options().done() ;

		},[]( QProcess::ProcessChannel,QByteArray data,std::shared_ptr< utility::context< Tlogger,Options > >& ctx ){

			if( ctx->options().debug() ){

				qDebug() << data ;
				qDebug() << "------------------------" ;
			}

			ctx->postData( std::move( data ) ) ;
		} ) ;
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

	void wait( int time ) ;
	void waitForOneSecond() ;
	void openDownloadFolderPath( const QString& ) ;
	QString homePath() ;
	QString python3Path() ;
	int terminateProcess( unsigned long pid ) ;
	void terminateProcess( QProcess& ) ;
	bool platformIsWindows() ;
	bool platformIsLinux() ;
	bool platformIsOSX() ;
	bool platformIsNOTWindows() ;

	bool hasDigitsOnly( const QString& e ) ;

	template< typename Object,
		  typename ObjectMemberFunction >
	static auto make_term_conn( Object obj,ObjectMemberFunction memFunction )
	{
		auto s = static_cast< void( * )( QProcess& ) >( utility::terminateProcess ) ;

		return make_conn( obj,memFunction,s ) ;
	}
}

#endif

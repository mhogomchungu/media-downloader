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
#include <QThread>

#include <type_traits>
#include <memory>

#include "translator.h"

#include "ui_mainwindow.h"

#include "engines.h"

class Context ;

struct concurrentDownloadManagerFinishedStatus ;

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
	QString failedToFindExecutableString( const QString& cmd ) ;

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

	class ProcessExitState
	{
	public:
		ProcessExitState( bool c,int s,QProcess::ExitStatus e ) :
			m_cancelled( c ),
			m_exitCode( s ),
			m_exitStatus( e )
		{
		}
		int exitCode() const
		{
			return m_exitCode ;
		}
		QProcess::ExitStatus exitStatus() const
		{
			return m_exitStatus ;
		}
		bool cancelled() const
		{
			return m_cancelled ;
		}
		bool success() const
		{
			return m_exitCode == 0 && m_exitStatus == QProcess::ExitStatus::NormalExit ;
		}
	private:
		bool m_cancelled = false ;
		int m_exitCode ;
		QProcess::ExitStatus m_exitStatus ;
	};

	class ProcessOutputChannels
	{
	public:
		ProcessOutputChannels() :
			m_channelMode( QProcess::ProcessChannelMode::MergedChannels )
		{
		}
		ProcessOutputChannels( QProcess::ProcessChannel c ) :
			m_channelMode( QProcess::ProcessChannelMode::SeparateChannels ),
			m_channel( c )
		{
		}
		QProcess::ProcessChannelMode channelMode() const
		{
			return m_channelMode ;
		}
		QProcess::ProcessChannel channel() const
		{
			return m_channel ;
		}
	private:
		QProcess::ProcessChannelMode m_channelMode ;
		QProcess::ProcessChannel m_channel ;
	} ;

	template< typename Tlogger,
		  typename Options >
	class context
	{
	public:
		context( const engines::engine& engine,
			 Tlogger logger,
			 Options options,
			 ProcessOutputChannels channels ) :
			m_engine( engine ),
			m_logger( std::move( logger ) ),
			m_cancelled( false ),
			m_options( std::move( options ) ),
			m_channels( channels )
		{
		}
		void setCancelConnection( QMetaObject::Connection conn )
		{
			m_conn = std::move( conn ) ;
		}
		bool cancelled()
		{
			return m_cancelled ;
		}
		void cancel()
		{
			m_cancelled = true ;
		}
		void postData( QByteArray data )
		{
			if( !m_cancelled ){

				m_data += data ;

				m_logger.add( [ this,data = std::move( data ) ]( Logger::Data& e,int id ){

					m_engine.processData( e,std::move( data ),id ) ;
				} ) ;
			}
		}
		template< typename Function >
		void listRequested( Function function )
		{
			if( m_options.listRequested() ){

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
		const ProcessOutputChannels& outputChannels()
		{
			return m_channels ;
		}
	private:
		const engines::engine& m_engine ;
		bool m_list_requested ;
		QMetaObject::Connection m_conn ;
		Tlogger m_logger ;
		QByteArray m_data ;
		bool m_cancelled ;
		Options m_options ;
		ProcessOutputChannels m_channels ;
	} ;

	template< typename Connection,
		  typename Tlogger,
		  typename Options >
	void run( const engines::engine& engine,
		  const QStringList& args,
		  const QString& quality,
		  Options options,
		  Tlogger logger,
		  Connection conn,
		  ProcessOutputChannels channels = ProcessOutputChannels() )
	{
		engines::engine::exeArgs::cmd cmd( engine.exePath(),args ) ;

		const auto& exe = cmd.exe() ;

		if( !QFile::exists( exe ) ){

			logger.add( utility::failedToFindExecutableString( exe ) ) ;

			options.done( ProcessExitState( false,-1,QProcess::ExitStatus::NormalExit ) ) ;

			return ;
		}

		options.tabManagerEnableAll( false ) ;

		utility::run( exe,cmd.args(),[ &,logger = std::move( logger ),options = std::move( options ) ]( QProcess& exe )mutable{

			exe.setProcessEnvironment( options.processEnvironment() ) ;

			logger.add( "cmd: " + engine.commandString( cmd ) ) ;

			const auto& df = options.downloadFolder() ;

			if( !QFile::exists( df ) ){

				QDir().mkpath( df ) ;
			}

			exe.setWorkingDirectory( df ) ;

			exe.setProcessChannelMode( channels.channelMode() ) ;

			using ctx_t = utility::context< Tlogger,Options > ;

			auto ctx = std::make_shared< ctx_t >( engine,std::move( logger ),std::move( options ),channels ) ;

			ctx->setCancelConnection( QObject::connect( conn.obj,conn.pointer,
					[ &engine,&exe,ctx,function = std::move( conn.function ) ](){

				ctx->cancel() ;

				function( engine,exe ) ;
			} ) ) ;

			return ctx ;

		},[ &engine,quality ]( QProcess& exe ){

			engine.sendCredentials( quality,exe ) ;

		},[]( int s,QProcess::ExitStatus e,std::shared_ptr< utility::context< Tlogger,Options > >& ctx ){

			ctx->disconnect() ;

			ctx->listRequested( [ & ]( const QList< QByteArray >& e ){

				ctx->options().listRequested( e ) ;
			} ) ;

			ctx->options().done( ProcessExitState( ctx->cancelled(),s,e ) ) ;

		},[]( QProcess::ProcessChannel channel,QByteArray data,std::shared_ptr< utility::context< Tlogger,Options > >& ctx ){

			if( ctx->options().debug() ){

				qDebug() << data ;
				qDebug() << "------------------------" ;
			}

			const auto& channels = ctx->outputChannels() ;

			if( channels.channelMode() == QProcess::ProcessChannelMode::MergedChannels ){

				ctx->postData( std::move( data ) ) ;

			}else if( channels.channelMode() == QProcess::ProcessChannelMode::SeparateChannels ){

				auto c = channels.channel() ;

				if( c == QProcess::ProcessChannel::StandardOutput && channel == c ){

					ctx->postData( std::move( data ) ) ;

				}else if( c == QProcess::ProcessChannel::StandardError && channel == c ){

					ctx->postData( std::move( data ) ) ;
				}
			}
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

	struct tableWidgetOptions
	{
		QFlags< QAbstractItemView::EditTrigger > editTrigger = QAbstractItemView::NoEditTriggers ;
		Qt::ContextMenuPolicy customContextPolicy = Qt::CustomContextMenu ;
		Qt::FocusPolicy focusPolicy = Qt::NoFocus ;
		QAbstractItemView::SelectionMode selectionMode = QAbstractItemView::NoSelection ;
		bool mouseTracking = true ;
	};

	template< typename List,
		  std::enable_if_t< std::is_lvalue_reference< List >::value,int > = 0 >
	class reverseIterator
	{
	public:
		typedef typename std::remove_reference_t< std::remove_cv_t< List > > ::value_type value_type ;
		typedef typename std::remove_reference_t< std::remove_cv_t< List > > ::size_type size_type ;

	        reverseIterator( List s ) :
		        m_list( s ),
			m_index( m_list.size() - 1 )
		{
		}
		bool hasNext() const
		{
			return m_index > -1 ;
		}
		void reset()
		{
			m_index = m_list.size() - 1 ;
		}
		auto& next()
		{
			auto s = static_cast< typename reverseIterator< List >::size_type >( m_index-- ) ;

			return m_list[ s ] ;
		}
		template< typename Function,
			  utility::types::has_bool_return_type< Function,typename reverseIterator< List >::value_type > = 0 >
		void forEach( Function function )
		{
			while( this->hasNext() ){

				if( function( this->next() ) ){

					break ;
				}
			}
		}
		template< typename Function,
			  utility::types::has_void_return_type< Function,typename reverseIterator< List >::value_type > = 0 >
		void forEach( Function function )
		{
			while( this->hasNext() ){

				function( this->next() ) ;
			}
		}
	private:
		List m_list ;
		int m_index ;
	} ;

	template< typename List >
	auto make_reverseIterator( List&& l )
	{
		return reverseIterator< decltype( l ) >( std::forward< List >( l ) ) ;
	}

	template< typename BackGroundTask,
		  typename UiThreadResult >
	class Thread : public QThread
	{
	public:
		Thread( BackGroundTask bgt,UiThreadResult fgt ) :
			m_bgt( std::move( bgt ) ),
			m_fgt( std::move( fgt ) )
		{
			connect( this,&QThread::finished,this,&Thread::then,Qt::QueuedConnection ) ;

			this->start() ;
		}
		void run() override
		{
			m_storage.create( m_bgt() ) ;
		}
		void then()
		{
			m_fgt( std::move( m_storage.get() ) ) ;

			this->deleteLater() ;
		}
	private:
		BackGroundTask m_bgt ;
		UiThreadResult m_fgt ;

		template< typename S >
		class storage
		{
		public:
			template< typename ... T >
			void create( T&& ... t )
			{
				m_pointer = new ( &m_storage ) S( std::forward< T >( t ) ... ) ;
			}
			S& get()
			{
				return *m_pointer ;
			}
			~storage()
			{
				if( m_pointer ){

					m_pointer->~S() ;
				}
			}
			bool created()
			{
				return m_pointer ;
			}
		private:
			S * m_pointer = nullptr ;
			#if __cplusplus >= 201703L
				alignas( S ) std::byte m_storage[ sizeof( S ) ] ;
			#else
				typename std::aligned_storage< sizeof( S ),alignof( S ) >::type m_storage ;
			#endif
		};

		storage< utility::types::result_of< BackGroundTask > > m_storage ;
	};

	template< typename BackGroundTask,
		  typename UiThreadResult,
		  utility::types::has_non_void_return_type< BackGroundTask > = 0 >
	void runInBgThread( BackGroundTask bgt,UiThreadResult fgt )
	{
		new Thread< BackGroundTask,UiThreadResult >( std::move( bgt ),std::move( fgt ) ) ;
	}

	template< typename BackGroundTask,
		  typename UiThreadResult,
		  utility::types::has_void_return_type< BackGroundTask > = 0 >
	void runInBgThread( BackGroundTask bgt,UiThreadResult fgt )
	{
		return utility::runInBgThread( [ bgt = std::move( bgt ) ](){

			bgt() ;

			return 0 ;

		},[ fgt = std::move( fgt ) ]( int ){

			fgt() ;
		} ) ;
	}

	template< typename BackGroundTask >
	void runInBgThread( BackGroundTask bgt )
	{
		return utility::runInBgThread( [ bgt = std::move( bgt ) ](){

			bgt() ;

			return 0 ;

		},[]( int ){} ) ;
	}

	void updateFinishedState( const engines::engine& engine,
				  settings& settings,
				  QTableWidget& table,
				  const concurrentDownloadManagerFinishedStatus& f ) ;
	int concurrentID() ;

	void setTableWidget( QTableWidget&,const tableWidgetOptions& = tableWidgetOptions() ) ;
	void addItem( QTableWidget&,const QStringList&,const QFont&,int alignment = Qt::AlignCenter ) ;
	void addItem( QTableWidget&,const QString&,const QFont&,int alignment = Qt::AlignCenter ) ;
	void selectRow( QTableWidgetItem * current,QTableWidgetItem * previous,int firstColumnNumber = 0 ) ;

	void clear( QTableWidget& ) ;
	void wait( int time ) ;
	void waitForOneSecond() ;
	void openDownloadFolderPath( const QString& ) ;
	QString homePath() ;
	QString python3Path() ;
	QString clipboardText() ;
	int terminateProcess( unsigned long pid ) ;
	void terminateProcess( const engines::engine&,QProcess& ) ;
	bool platformIsWindows() ;
	bool platformIs32BitWindows() ;
	bool platformIsLinux() ;
	bool platformIsOSX() ;
	bool platformIsNOTWindows() ;

	QStringList updateOptions( const engines::engine& engine,
				   settings&,
				   const utility::args& args,
				   const QStringList& urls ) ;

	bool hasDigitsOnly( const QString& e ) ;

	template< typename Object,
		  typename ObjectMemberFunction >
	static auto make_term_conn( Object obj,ObjectMemberFunction memFunction )
	{
		auto s = static_cast< void( * )( const engines::engine&,QProcess& ) >( utility::terminateProcess ) ;

		return utility::make_conn( obj,memFunction,s ) ;
	}
}

#endif

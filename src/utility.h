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

#include "tableWidget.h"

class Context ;

class tabManager ;

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

	QStringList splitPreserveQuotes( const QString& ) ;
	QStringList split( const QString& e,char token,bool skipEmptyParts ) ;
	QStringList split( const QString& e,const char * token ) ;
	QList< QByteArray > split( const QByteArray& e,char token = '\n' ) ;
	QList< QByteArray > split( const QByteArray& e,QChar token = '\n' ) ;

	class args
	{
	public:
		args( const QString& e )
		{
			if( !e.isEmpty() ){

				m_otherOptions = utility::splitPreserveQuotes( e ) ;

				if( !m_otherOptions.isEmpty() ){

					m_quality = m_otherOptions.takeFirst() ;
				}
			}
		}
		const QString& quality() const
		{
			return m_quality ;
		}
		const QStringList& otherOptions() const
		{
			return m_otherOptions ;
		}
	private:
		QString m_quality ;
		QStringList m_otherOptions ;
	} ;

	QString failedToFindExecutableString( const QString& cmd ) ;
	int concurrentID() ;

	void wait( int time ) ;
	void waitForOneSecond() ;
	void openDownloadFolderPath( const QString& ) ;
	QString homePath() ;
	QString python3Path() ;
	QString clipboardText() ;
	bool platformIsWindows() ;
	bool platformIs32BitWindows() ;
	bool platformIsLinux() ;
	bool platformIsOSX() ;
	bool platformIsNOTWindows() ;
	QString downloadFolder( const Context& ctx ) ;
	const QProcessEnvironment& processEnvironment( const Context& ctx ) ;

	QStringList updateOptions( const engines::engine& engine,
				   settings&,
				   const utility::args& args,
				   const QStringList& urls ) ;

	bool hasDigitsOnly( const QString& e ) ;

	class contextState
	{
	public:
		contextState() :
			m_noneAreRunning( true ),
			m_finishedSuccess( false )
		{
		}
		contextState( bool r ) :
			m_noneAreRunning( r ),
			m_finishedSuccess( false )
		{
		}
		contextState( bool r,bool f ) :
			m_noneAreRunning( r ),
			m_finishedSuccess( f )
		{
		}
		bool noneAreRunning() const
		{
			return m_noneAreRunning ;
		}
		bool finishedSuccess() const
		{
			return m_finishedSuccess ;
		}
		bool showLogWindow() const
		{
			return m_showLogWindow ;
		}
		void setShowLogWindow()
		{
			m_showLogWindow = true ;
		}
		bool clear() const
		{
			return m_clear ;
		}
		void setClear()
		{
			m_clear = true ;
		}
	private:
		bool m_noneAreRunning ;
		bool m_finishedSuccess ;
		bool m_showLogWindow = false ;
		bool m_clear = false ;
	};

	template< typename Function >
	void appendContextMenu( QMenu& m,utility::contextState c,Function function )
	{
		auto ac = m.addAction( QObject::tr( "Show Log Window" ) ) ;

		QObject::connect( ac,&QAction::triggered,[ &function,&c ](){

			c.setShowLogWindow() ;

			function( c ) ;
		} ) ;

		ac = m.addAction( QObject::tr( "Clear" ) ) ;

		ac->setEnabled( c.noneAreRunning() ) ;

		QObject::connect( ac,&QAction::triggered,[ &function,&c ](){

			c.setClear() ;

			function( c ) ;
		} ) ;

		m.exec( QCursor::pos() ) ;
	}

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

	QMenu * setUpMenu( const Context& ctx,
			   const QStringList&,
			   bool addClear,
			   bool addOpenFolder,
			   bool combineText,
			   QWidget * parent ) ;

	template< typename Function >
	void setMenuOptions( const Context& ctx,
			     const QStringList& opts,
			     bool addClear,
			     bool addOpenFolder,
			     QPushButton * w,
			     Function function )
	{
		auto m = w->menu() ;

		if( m ){

			m->deleteLater() ;
		}

		auto menu = utility::setUpMenu( ctx,opts,addClear,addOpenFolder,false,w ) ;

		w->setMenu( menu ) ;

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

	template< typename Function,typename FunctionConnect >
	class Conn
	{
	public:
		typedef Function function_type ;
		Conn( Function function,
		      FunctionConnect functionConnect ) :
			m_function( std::move( function ) ),
			m_functionConnect( std::move( functionConnect ) )
		{
		}
		template< typename Fnt >
		void connect( Fnt function )
		{
			auto fnt = [ this,function = std::move( function ) ]( int index ){

				function( m_function,index ) ;
			} ;

			m_conn = m_functionConnect( std::move( fnt ) ) ;
		}
		void disconnect()
		{
			QObject::disconnect( m_conn ) ;
		}
	private:
		Function m_function ;
		FunctionConnect m_functionConnect ;
		QMetaObject::Connection m_conn ;
	};

	template< typename Function,typename FunctionConnect >
	utility::Conn< Function,FunctionConnect > make_conn( Function f,FunctionConnect c )
	{
		return utility::Conn< Function,FunctionConnect >( std::move( f ),std::move( c ) ) ;
	}

	class Terminator : public QObject
	{
		Q_OBJECT
	public:
		static int terminateProcess( unsigned long pid ) ;

		template< typename Object,typename Member >
		auto setUp( Object obj,Member member,int idx )
		{
			return utility::make_conn( []( const engines::engine& engine,QProcess& exe,int index,int idx ){

				return utility::Terminator::terminateProcess( engine,exe,index,idx ) ;

			},[ idx,obj,member,this ]( auto function ){

				Q_UNUSED( this ) //Older version of gcc seems to require capturing "this".

				return QObject::connect( obj,member,[ idx,function = std::move( function ) ](){

					function( idx ) ;
				} ) ;
			} ) ;
		}
		auto setUp()
		{
			return utility::make_conn( []( const engines::engine& engine,QProcess& exe,int index,int idx ){

				return utility::Terminator::terminateProcess( engine,exe,index,idx ) ;

			},[ this ]( auto function ){

				return QObject::connect( this,&utility::Terminator::terminate,
							 [ function = std::move( function ) ]( int index ){

					function( index ) ;
				} ) ;
			} ) ;
		}
		void terminateAll( QTableWidget& t )
		{
			for( int i = 0 ; i < t.rowCount() ; i++ ){

				this->terminate( i ) ;
			}
		}
	signals :
		void terminate( int index ) ;
	private:
		static bool terminateProcess( const engines::engine&,QProcess& exe,int index,int idx )
		{
			if( index == idx ){

				if( utility::platformIsWindows() ){

					QStringList args{ "-T",QString::number( exe.processId() ) } ;

					QProcess::startDetached( "media-downloader.exe",args ) ;
				}else{
					exe.terminate() ;
				}

				return true ;
			}else{
				return false ;
			}
		}
	};

	class ProcessExitState
	{
	public:
		ProcessExitState( bool c,int s,int d,QProcess::ExitStatus e ) :
			m_cancelled( c ),
			m_exitCode( s ),
			m_duration( d ),
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
		int duration() const
		{
			return m_duration ;
		}
	private:
		bool m_cancelled = false ;
		int m_exitCode ;
		int m_duration ;
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
		  typename Options,
		  typename Connection >
	class context
	{
	public:
		context( const engines::engine& engine,
			 QProcess& exe,
			 ProcessOutputChannels channels,
			 Tlogger logger,
			 Options options,
			 Connection conn ) :
			m_engine( engine ),
			m_logger( std::move( logger ) ),
			m_options( std::move( options ) ),
			m_conn( std::move( conn ) ),
			m_channels( channels ),
			m_cancelled( false )
		{
			if( m_engine.replaceOutputWithProgressReport() ){

				QObject::connect( &m_timer,&QTimer::timeout,[ this ]{

					m_logger.add( [ this ]( Logger::Data& e,int id ){

						m_engine.processData( e,m_timeCounter.stringElapsedTime(),id ) ;
					} ) ;
				} ) ;

				m_timer.start( 1000 ) ;
			}

			m_conn.connect( [ this,&exe ]( typename Connection::function_type& function,int index ){

				auto m = function( m_engine,exe,m_options.index(),index ) ;

				if( m ){

					m_cancelled = true ;
				}

				return m ;
			} ) ;
		}
		void postData( QByteArray data )
		{
			m_timer.stop() ;

			if( !m_cancelled ){

				if( m_options.listRequested() ){

					m_data += data ;
				}

				m_logger.add( [ this,data = std::move( data ) ]( Logger::Data& e,int id ){

					m_engine.processData( e,std::move( data ),id ) ;
				} ) ;
			}
		}
		bool debug()
		{
			return m_options.debug() ;
		}
		void done( int s,QProcess::ExitStatus e )
		{
			m_conn.disconnect() ;

			m_timer.stop() ;

			if( m_options.listRequested() ){

				m_options.listRequested( utility::split( m_data,'\n' ) ) ;
			}

			auto m = m_timeCounter.elapsedTime() ;
			m_options.done( ProcessExitState( m_cancelled,s,m,std::move( e ) ) ) ;
		}
		const ProcessOutputChannels& outputChannels()
		{
			return m_channels ;
		}
	private:
		const engines::engine& m_engine ;
		Tlogger m_logger ;
		Options m_options ;
		Connection m_conn ;
		ProcessOutputChannels m_channels ;
		QTimer m_timer ;
		engines::engine::functions::timer m_timeCounter ;
		QByteArray m_data ;
		bool m_cancelled ;
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
		engines::engine::exeArgs::cmd cmd( engine.exePath(),args,options.downloadFolder() ) ;

		const auto& exe = cmd.exe() ;

		if( !QFile::exists( exe ) ){

			logger.add( utility::failedToFindExecutableString( exe ) ) ;

			options.done( ProcessExitState( false,-1,-1,QProcess::ExitStatus::NormalExit ) ) ;

			return ;
		}

		options.disableAll() ;

		using ctx_t = utility::context< Tlogger,Options,Connection > ;

		using unique_ptr_ctx_t = std::unique_ptr< ctx_t > ;

		utility::run( exe,cmd.args(),[ &,logger = std::move( logger ),options = std::move( options ) ]( QProcess& exe )mutable{

			exe.setProcessEnvironment( options.processEnvironment() ) ;

			logger.add( "cmd: " + engine.commandString( cmd ) ) ;

			const auto& df = options.downloadFolder() ;

			if( !QFile::exists( df ) ){

				QDir().mkpath( df ) ;
			}

			exe.setWorkingDirectory( df ) ;

			exe.setProcessChannelMode( channels.channelMode() ) ;

			return std::make_unique< ctx_t >( engine,
							  exe,
							  channels,
							  std::move( logger ),
							  std::move( options ),
							  std::move( conn ) ) ;

		},[ &engine,quality ]( QProcess& exe ){

			engine.sendCredentials( quality,exe ) ;

		},[]( int s,QProcess::ExitStatus e,unique_ptr_ctx_t& ctx ){

			ctx->done( s,std::move( e ) ) ;

		},[]( QProcess::ProcessChannel channel,QByteArray data,unique_ptr_ctx_t& ctx ){

			if( ctx->debug() ){

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

	template< typename S >
	class storage
	{
	public:
		storage()
		{
		}
		storage( const storage& s )
		{
			this->set( s.get() ) ;
		}
		storage( storage& s )
		{
			this->set( s.get() ) ;
		}
		storage( storage&& s )
		{
			this->set( std::move( s.get() ) ) ;
		}
		template< typename ... T >
		storage( T&& ... t )
		{
			this->set( std::forward< T >( t ) ... ) ;
		}
		storage& operator=( const storage& s )
		{
			return this->set( s.get() ) ;
		}
		storage& operator=( storage& s )
		{
			return this->set( s.get() ) ;
		}
		storage& operator=( storage&& s )
		{
			return this->set( std::move( s.get() ) ) ;
		}
		template< typename T >
		storage& operator=( T&& t )
		{
			return this->set( std::forward< T >( t ) ) ;
		}
		template< typename ... T >
		S& put( T&& ... t )
		{
			return this->set( std::forward< T >( t ) ... ).get() ;
		}
		S& get() const
		{
			return *m_pointer ;
		}
		S * operator->() const
		{
			return m_pointer ;
		}
		~storage()
		{
			if( m_pointer ){

				m_pointer->~S() ;
			}
		}
		bool created() const
		{
			return m_pointer ;
		}
	private:
		template< typename ... T >
		storage& set( T&& ... t )
		{
			if( m_pointer ){

				m_pointer->~S() ;
			}

			m_pointer = new ( &m_storage ) S( std::forward< T >( t ) ... ) ;
			return *this ;
		}
		S * m_pointer = nullptr ;
		#if __cplusplus >= 201703L
			alignas( S ) std::byte m_storage[ sizeof( S ) ] ;
		#else
			typename std::aligned_storage< sizeof( S ),alignof( S ) >::type m_storage ;
		#endif
	};

	class MediaEntry
	{
	public:
		MediaEntry( const QString& url ) :
			m_url( url ),
			m_json( QByteArray() )
		{
		}
		MediaEntry( const QString& uiText,const QString& url ) :
			m_title( uiText ),
			m_url( url ),
			m_json( QByteArray() )
		{
		}
		MediaEntry( const QByteArray& data ) : m_json( data )
		{
			if( m_json ){

				auto object = m_json.doc().object() ;

				m_title        = object.value( "title" ).toString() ;
				m_thumbnailUrl = object.value( "thumbnail" ).toString() ;
				m_url          = object.value( "webpage_url" ).toString() ;
				m_uploadDate   = object.value( "upload_date" ).toString() ;

				if( !m_uploadDate.isEmpty() ){

					m_uploadDate = QObject::tr( "Upload Date:" ) + " " + m_uploadDate ;
				}

				auto d = object.value( "duration" ).toInt() ;

				if( d != 0 ){

					auto s = engines::engine::functions::timer::duration( d * 1000 ) ;
					m_duration = QObject::tr( "Duration:" ) + " " + s ;
				}
			}
		}
		const QString& thumbnailUrl() const
		{
			return m_thumbnailUrl ;
		}
		const QString& title() const
		{
			return m_title ;
		}
		const QString& url() const
		{
			return m_url ;
		}
		QString uiText() const
		{
			auto title = [ & ](){

				if( m_title.isEmpty() || m_title == "\n" ){

					return m_url ;
				}else{
					return m_title ;
				}
			}() ;

			if( m_duration.isEmpty() ){

				if( m_uploadDate.isEmpty() ){

					return title ;
				}else{
					return m_uploadDate + "\n" + title ;
				}
			}else{
				if( m_uploadDate.isEmpty() ){

					return m_duration + "\n" + title ;
				}else{
					return m_duration + ", " + m_uploadDate + "\n" + title ;
				}
			}
		}
		const QString& uploadDate() const
		{
			return m_uploadDate ;
		}
		bool valid() const
		{
			return m_json ;
		}
		const QJsonDocument& doc() const
		{
			return m_json.doc() ;
		}
		QString errorString() const
		{
			return m_json.errorString() ;
		}
		const QString& duration() const
		{
			return m_duration ;
		}
	private:
		QString m_thumbnailUrl ;
		QString m_title ;
		QString m_uploadDate ;
		QString m_url ;
		QString m_duration ;
		engines::Json m_json ;
	};

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
			m_storage = m_bgt() ;
		}
		void then()
		{
			m_fgt( std::move( m_storage.get() ) ) ;

			this->deleteLater() ;
		}
	private:
		BackGroundTask m_bgt ;
		UiThreadResult m_fgt ;

		utility::storage< utility::types::result_of< BackGroundTask > > m_storage ;
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

	template< typename FinishedState >
	void updateFinishedState( const engines::engine& engine,
				  settings& s,
				  tableWidget& table,
				  const FinishedState& f )
	{
		const auto& index = f.index() ;
		const auto& es = f.exitState() ;

		f.setState( table.runningStateItem( index ) ) ;

		const auto backUpUrl = table.url( index ) ;

		auto& item = table.uiTextItem( index ) ;

		auto a = item.text() ;

		item.setText( engine.updateTextOnCompleteDownlod( a,backUpUrl,es ) ) ;

		if( !es.cancelled() ){

			if( es.success() ){

				engine.runCommandOnDownloadedFile( a,backUpUrl ) ;
			}

			if( f.allFinished() ){

				auto a = s.commandWhenAllFinished() ;

				if( !a.isEmpty() ){

					auto args = utility::split( a,' ',true ) ;

					auto exe = args.takeAt( 0 ) ;

					QProcess::startDetached( exe,args ) ;
				}
			}
		}
	}

	struct opts
	{
		const Context& ctx ;
		const engines::engine& engine ;
		QTableWidget& table ;
		bool debug ;
		bool listRequested ;
		QString downloadFolder() const ;
		const QProcessEnvironment& processEnvironment() const ;
	} ;

	template< typename Opts,typename Functions >
	class options
	{
	public:
		options( Opts opts,Functions functions ) :
			m_opts( std::move( opts ) ),
			m_functions( std::move( functions ) )
		{
		}
		void done( utility::ProcessExitState e )
		{
			m_functions.done( std::move( e ),m_opts ) ;
		}
		void listRequested( const QList< QByteArray >& e )
		{
			m_functions.list( e ) ;
		}
		bool listRequested()
		{
			return m_opts.listRequested ;
		}
		int index()
		{
			return m_opts.index ;
		}
		bool debug()
		{
			return m_opts.debug ;
		}
		void disableAll()
		{
			m_functions.disableAll( m_opts ) ;
		}
		QString downloadFolder() const
		{
			return utility::downloadFolder( m_opts.ctx ) ;
		}
		const QProcessEnvironment& processEnvironment() const
		{
			return utility::processEnvironment( m_opts.ctx ) ;
		}
	private:
		Opts m_opts ;
		Functions m_functions ;
	} ;

	template< typename List,typename DisableAll,typename Done >
	struct Functions
	{
		List list ;
		DisableAll disableAll ;
		Done done ;
	} ;

	template< typename List,typename DisableAll,typename Done >
	Functions< List,DisableAll,Done > OptionsFunctions( List list,DisableAll disableAll,Done done )
	{
		return { std::move( list ),std::move( disableAll ),std::move( done ) } ;
	}

	template< typename DisableAll,typename Done >
	auto OptionsFunctions( DisableAll disableAll,Done done )
	{
		auto aa = []( const QList< QByteArray >& ){} ;

		using type = Functions< decltype( aa ),DisableAll,Done > ;

		return type{ std::move( aa ),std::move( disableAll ),std::move( done ) } ;
	}
}

#endif

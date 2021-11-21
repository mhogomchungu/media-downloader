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
#include <QDebug>

#include <type_traits>
#include <memory>
#include <iostream>

#include "translator.h"

#include "ui_mainwindow.h"

#include "engines.h"

#include "tableWidget.h"

#include "util.hpp"

#include "networkAccess.h"

class Context ;

class tabManager ;

namespace Ui
{
	class MainWindow ;
}

namespace utility
{
	class debug
	{
	public:
		debug( const QString& e = QString() ) :
			m_switch( e )
		{
		}
		template< typename T >
		debug& operator<<( const T& e )
		{
			return _print( e ) ;
		}
		debug& operator<<( const QString& e ) ;
		debug& operator<<( const QByteArray& e ) ;
		debug& operator<<( const QStringList& e ) ;
	private:
		template< typename T >
		debug& _print( const T& e )
		{
			if( !m_switch.isEmpty() ){

				if( m_switch == "--debug" ){

					std::cout << e << std::endl ;

				}else if( m_switch == "--qdebug" ){

					qDebug() << e ;
				}
			}
			return *this ;
		}
		QString m_switch ;
	};

	class stringConstants
	{
	public:
		static QString mediaDownloaderDataPath()
		{
			return "{MediaDownloaderDataPath}" ;
		}
		static QString mediaDownloaderDefaultDownloadPath()
		{
			return "{MediaDownloaderDefaultDownloadPath}" ;
		}
		static QString mediaDownloaderCWD()
		{
			return "{MediaDownloaderCWD}" ;
		}
		static QString defaultPath()
		{
			return "${default}" ;
		}
		static QString backendPath()
		{
			return "${BackendPath}" ;
		}
		static QString commandName()
		{
			return "${CommandName}" ;
		}
	private:
	};

	class args
	{
	public:
		args( const QString& e )
		{
			if( !e.isEmpty() ){

				m_otherOptions = util::splitPreserveQuotes( e ) ;

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
	class arguments
	{
	public:
		arguments( QStringList& args ) : m_args( args )
		{
		}
		template< typename T >
		bool hasOption( const T& opt,bool remove = false ) const
		{
			for( int i = 0 ; i < m_args.size() ; i++ ){

				if( m_args[ i ] == opt ){

					if( remove ){

						m_args.removeAt( i ) ;
					}

					return true ;
				}
			}

			return false ;
		}
		template< typename T >
		void removeOptionWithArgument( const T& opt ) const
		{
			this->hasValue( opt,true ) ;
		}
		template< typename T >
		void removeOption( const T& opt ) const
		{
			this->hasOption( opt,true ) ;
		}
		template< typename T >
		QString hasValue( const T& opt,bool remove = false ) const
		{
			QString result ;

			for( int i = 0 ; i < m_args.size() ; i++ ){

				if( m_args[ i ] == opt ){

					if( i + 1 < m_args.size() ){

						result = m_args[ i + 1 ] ;

						if( remove ){

							m_args.removeAt( i + 1 ) ;
						}
					}

					if( remove ){

						m_args.removeAt( i ) ;
					}
				}
			}

			return result ;
		}
	private:
		QStringList& m_args ;
	};

	template< typename T >
	void removeArgument( QStringList& s,const T& e )
	{
		s.removeAll( e ) ;
	}

	template< typename T >
	void removeArgumentWithOption( QStringList& s,const T& e )
	{
		for( int i = 0 ; i < s.size() ; i++ ){

			if( s[ i ] == e ){

				if( i + 1 < s.size() ){

					s.removeAt( i + 1 ) ;
				}

				s.removeAt( i ) ;

				break ;
			}
		}
	}

	const engines::engine& resolveEngine( const tableWidget&,
					      const engines::engine&,
					      const engines& engines,
					      int row ) ;

	QString failedToFindExecutableString( const QString& cmd ) ;
	int concurrentID() ;
	void saveDownloadList( const Context&,QMenu&,tableWidget& ) ;
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
	bool isRelativePath( const QString& ) ;
	QString downloadFolder( const Context& ctx ) ;
	const QProcessEnvironment& processEnvironment( const Context& ctx ) ;

	class locale
	{
	public:
		QString formattedDataSize( qint64 ) const ;
	private:
		QLocale m_locale ;
	};

	struct updateOptionsStruct
	{
		const engines::engine& engine ;
		const engines::enginePaths& enginePaths ;
		settings& stts;
		const utility::args& args ;
		const QString& indexAsString ;
		bool forceDownload ;
		const QStringList& urls ;
	};

	QStringList updateOptions( const updateOptionsStruct& ) ;

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

	enum class PlayListButtonName{ DownloadRange,PlaylistUrl,None } ;
	template< typename Settings,typename TabName,typename Function >
	bool showHistory( QLineEdit& lineEdit,
			  const QStringList& history,
			  Settings& settings,
			  TabName tabName,
			  Function function,
			  PlayListButtonName pbn = PlayListButtonName::None )
	{
		if( history.isEmpty() ){

			return false ;
		}else{
			bool s = false ;

			QMenu m ;

			QObject::connect( &m,&QMenu::triggered,[ & ]( QAction * ac ){

				auto m = ac->objectName() ;

				if( m == "Clear" ){

					if( pbn == utility::PlayListButtonName::None ){

						settings.clearOptionsHistory( tabName ) ;

					}else if( pbn == utility::PlayListButtonName::DownloadRange ){

						settings.clearPlaylistRangeHistory() ;
					}else{
						settings.clearPlaylistUrlHistory() ;
					}
				}else{
					s = true ;

					lineEdit.setText( ac->objectName() ) ;
				}
			} ) ;

			for( const auto& it : history ){

				auto ss = settings.stringTruncationSize() ;

				if( it.size() < ss ){

					m.addAction( it )->setObjectName( it ) ;
				}else{
					auto sss = ss / 2 ;

					auto a = it.mid( 0,sss ) ;
					auto b = it.mid( it.size() - sss ) ;
					auto ac = m.addAction( a + "..." + b ) ;
					ac->setObjectName( it ) ;
					ac->setToolTip( it ) ;
				}
			}

			m.addSeparator() ;

			function( m ) ;

			m.addSeparator() ;

			m.addAction( QObject::tr( "Clear" ) )->setObjectName( "Clear" ) ;

			m.exec( QCursor::pos() ) ;

			return s ;
		}
	}
	template< typename Settings,typename TabName >
	bool showHistory( QLineEdit& lineEdit,
			  const QStringList& history,
			  Settings& settings,
			  TabName tabName,
			  PlayListButtonName pbn = PlayListButtonName::None )
	{
		return utility::showHistory( lineEdit,history,settings,tabName,[]( QMenu& ){},pbn ) ;
	}

	template< typename Function,typename AddAction >
	void addDownloadContextMenu( bool running,
				     bool finishSuccess,
				     QMenu& m,
				     int row,
				     Function function,
				     AddAction addAction )
	{
		bool forceDownload = false ;

		QAction * ac ;

		if( finishSuccess ){

			if( running ){

				ac = m.addAction( QObject::tr( "Download" ) ) ;
				ac->setEnabled( false ) ;
			}else{
				if( function( row ) ){

					ac = m.addAction( QObject::tr( "Force Download" ) ) ;
					forceDownload = true ;
				}else{
					ac = m.addAction( QObject::tr( "Download" ) ) ;
					ac->setEnabled( !running && !finishSuccess ) ;
				}
			}
		}else{
			ac = m.addAction( QObject::tr( "Download" ) ) ;
			ac->setEnabled( !running && !finishSuccess ) ;
		}

		addAction( ac,forceDownload,row ) ;
	}

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

	template< typename WhenDone,typename WithData >
	void run( const QString& cmd,const QStringList& args,WhenDone w,WithData p )
	{
		util::run( cmd,args,[]( QProcess& ){},std::move( w ),std::move( p ) ) ;
	}

	template< typename Function,typename FunctionConnect >
	class Conn
	{
	public:
		Conn( Function function,FunctionConnect functionConnect ) :
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
		static util::result< int > terminate( int argc,char ** argv ) ;

		template< typename Object,typename Member >
		auto setUp( Object obj,Member member,int idx )
		{
			return utility::make_conn( []( const engines::engine& engine,QProcess& exe,int index,int idx ){

				return utility::Terminator::terminate( engine,exe,index,idx ) ;

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

				return utility::Terminator::terminate( engine,exe,index,idx ) ;

			},[ this ]( auto function ){

				using e = void( Terminator::* )( int ) ;

				auto m = static_cast< e >( &utility::Terminator::terminate ) ;

				return QObject::connect( this,m,[ function = std::move( function ) ]( int index ){

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
		static bool terminate( QProcess& ) ;
		static bool terminate( const engines::engine&,QProcess& exe,int index,int idx )
		{
			if( index == idx ){

				return utility::Terminator::terminate( exe ) ;
			}else{
				return false ;
			}
		}
	};

	template< typename Function >
	void setUpdefaultEngine( QComboBox& comboBox,
				 const QString& defaultEngine,
				 Function function )
	{
		for( int s = 0 ; s < comboBox.count() ; s++ ){

			if( comboBox.itemText( s ) == defaultEngine ){

				comboBox.setCurrentIndex( s ) ;

				return ;
			}
		}

		if( comboBox.count() > 0 ){

			comboBox.setCurrentIndex( 0 ) ;
			function( comboBox.itemText( 0 ) ) ;
		}
	}

	class versionInfo : public QObject
	{
		Q_OBJECT
	public:
		~versionInfo() override ;

		versionInfo( Ui::MainWindow& ui ) : m_ui( ui )
		{
		}
		void setContext( const Context& ctx )
		{
			m_ctx = &ctx ;
			m_networkAccess = ctx ;
		}
		template< typename Then >
		void setVersion( const engines::engine& engine,Then then ){

			engines::engine::exeArgs::cmd cmd( engine.exePath(),{ engine.versionArgument() } ) ;

			util::run( cmd.exe(),cmd.args(),[ &engine,then = std::move( then ) ]( const util::run_result& r ){

				if( r.success() ){

					engine.versionString( r.stdOut ) ;
				}

				then( r.success() ) ;

			},QProcess::ProcessChannelMode::MergedChannels ) ;
		}
		void check( const engines::Iterator& iter ) ;
		networkAccess& network()
		{
			return m_networkAccess.get() ;
		}
	signals:
		void vinfoDone() ;
	private:
		void printEngineVersionInfo( const engines::Iterator& iter ) ;
		const Context * m_ctx ;
		util::storage< networkAccess > m_networkAccess ;
		Ui::MainWindow& m_ui ;
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
			 ProcessOutputChannels channels,
			 Tlogger&& logger,
			 Options&& options,
			 Connection&& conn ) :
			m_engine( engine ),
			m_logger( std::move( logger ) ),
			m_options( std::move( options ) ),
			m_conn( std::move( conn ) ),
			m_channels( channels ),
			m_timer( std::make_unique< QTimer >() ),
			m_cancelled( false )
		{
		}
		void whenCreated( QProcess& exe,const engines::engine::exeArgs::cmd& cmd )
		{
			m_options.disableAll() ;

			exe.setProcessEnvironment( m_options.processEnvironment() ) ;

			m_logger.add( "cmd: " + m_engine.commandString( cmd ) ) ;

			const auto& df = m_options.downloadFolder() ;

			if( !QFile::exists( df ) ){

				QDir().mkpath( df ) ;
			}

			exe.setWorkingDirectory( df ) ;

			exe.setProcessChannelMode( m_channels.channelMode() ) ;
		}
		void whenStarted( QProcess& exe,const QString& credentials )
		{
			m_conn.connect( [ this,&exe ]( auto& function,int index ){

				auto m = function( m_engine,exe,m_options.index(),index ) ;

				if( m ){

					m_cancelled = true ;
				}

				return m ;
			} ) ;

			if( m_engine.replaceOutputWithProgressReport() ){

				QObject::connect( m_timer.get(),&QTimer::timeout,[ this ]{

					m_logger.add( [ this ]( Logger::Data& e,int id ){

						m_engine.processData( e,m_timeCounter.stringElapsedTime(),id ) ;
					} ) ;
				} ) ;

				m_timer->start( 1000 ) ;
			}

			m_engine.sendCredentials( credentials,exe ) ;
		}
		void whenDone( int s,QProcess::ExitStatus e )
		{
			m_conn.disconnect() ;

			m_timer->stop() ;

			if( m_options.listRequested() ){

				m_options.listRequested( std::move( m_data ) ) ;
			}

			auto m = m_timeCounter.elapsedTime() ;
			m_options.done( ProcessExitState( m_cancelled,s,m,std::move( e ) ) ) ;
		}
		void withData( QProcess::ProcessChannel channel,const QByteArray& data )
		{
			auto _withData = [ & ]( const QByteArray& data ){

				utility::debug( m_options.debug() ) << data ;
				utility::debug( m_options.debug() ) << "-------------------------------" ;

				m_timer->stop() ;

				if( !m_cancelled ){

					if( m_options.listRequested() ){

						m_data += data ;
					}

					m_logger.add( [ this,&data ]( Logger::Data& e,int id ){

						m_engine.processData( e,data,id ) ;
					} ) ;
				}
			} ;

			auto mode = m_channels.channelMode() ;

			if( mode == QProcess::ProcessChannelMode::MergedChannels ){

				_withData( data ) ;

			}else if( mode == QProcess::ProcessChannelMode::SeparateChannels ){

				auto c = m_channels.channel() ;

				if( channel == c ){

					_withData( data ) ;
				}
			}
		}
		engines::engine::exeArgs::cmd cmd( const QStringList& args )
		{
			return { m_engine.exePath(),args } ;
		}
		void logError( const engines::engine::exeArgs::cmd& exe )
		{
			m_logger.add( utility::failedToFindExecutableString( exe.exe() ) ) ;

			m_options.done( ProcessExitState( false,-1,-1,QProcess::ExitStatus::NormalExit ) ) ;
		}
	private:
		const engines::engine& m_engine ;
		Tlogger m_logger ;
		Options m_options ;
		Connection m_conn ;
		ProcessOutputChannels m_channels ;
		std::unique_ptr< QTimer > m_timer ;
		engines::engine::functions::timer m_timeCounter ;
		QByteArray m_data ;
		bool m_cancelled ;
	} ;

	template< typename Connection,
		  typename Tlogger,
		  typename Options >
	auto make_ctx( const engines::engine& engine,
		       Options options,
		       Tlogger logger,
		       Connection conn,
		       utility::ProcessOutputChannels channels = utility::ProcessOutputChannels() )
	{
		using ctx = utility::context< Tlogger,Options,Connection > ;

		return ctx( engine,channels,std::move( logger ),std::move( options ),std::move( conn ) ) ;
	}

	template< typename Ctx >
	void run( const QStringList& args,const QString& credentials,Ctx ctx )
	{
		auto cmd = ctx.cmd( args ) ;

		if( cmd.valid() ){

			util::run( cmd.exe(),cmd.args(),[ &cmd,ctx = std::move( ctx ) ]( QProcess& exe )mutable{

				ctx.whenCreated( exe,cmd ) ;

				return std::move( ctx ) ;

			},[ credentials ]( QProcess& exe,auto& ctx ){

				ctx.whenStarted( exe,credentials ) ;

			},[]( int s,QProcess::ExitStatus e,auto& ctx ){

				ctx.whenDone( s,std::move( e ) ) ;

			},[]( QProcess::ProcessChannel channel,const QByteArray& data,auto& ctx ){

				ctx.withData( channel,data ) ;
			} ) ;
		}else{
			ctx.logError( cmd ) ;
		}
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
			  util::types::has_bool_return_type< Function,typename reverseIterator< List >::value_type > = 0 >
		void forEach( Function function )
		{
			while( this->hasNext() ){

				if( function( this->next() ) ){

					break ;
				}
			}
		}
		template< typename Function,
			  util::types::has_void_return_type< Function,typename reverseIterator< List >::value_type > = 0 >
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

		MediaEntry( const QByteArray& data ) ;
		QString uiText() const ;

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
		const QString& id() const
		{
			return m_id ;
		}
		int intDuration() const
		{
			return m_intDuration ;
		}
	private:
		QString m_thumbnailUrl ;
		QString m_title ;
		QString m_uploadDate ;
		QString m_url ;
		QString m_duration ;
		QString m_id ;
		int m_intDuration ;
		util::Json m_json ;
	};

	template< typename FinishedState >
	void updateFinishedState( const engines::engine& engine,
				  settings& s,
				  tableWidget& table,
				  const FinishedState& f )
	{
		const auto& index = f.index() ;
		const auto& es = f.exitState() ;

		table.setRunningState( f.setState(),index ) ;

		const auto backUpUrl = table.url( index ) ;

		auto a = table.uiText( index ) ;

		table.setUiText( engine.updateTextOnCompleteDownlod( a,backUpUrl,es ),index ) ;

		if( !es.cancelled() ){

			if( es.success() ){

				engine.runCommandOnDownloadedFile( a,backUpUrl ) ;
			}

			if( f.allFinished() ){

				auto a = s.commandWhenAllFinished() ;

				if( !a.isEmpty() ){

					auto args = util::split( a,' ',true ) ;

					auto exe = args.takeAt( 0 ) ;

					QProcess::startDetached( exe,args ) ;
				}
			}
		}
	}

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
		void listRequested( QByteArray e )
		{
			m_functions.list( std::move( e ) ) ;
		}
		bool listRequested()
		{
			return m_opts.listRequested ;
		}
		int index()
		{
			return m_opts.index ;
		}
		const QString& debug()
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
		auto aa = []( QByteArray ){} ;

		using type = Functions< decltype( aa ),DisableAll,Done > ;

		return type{ std::move( aa ),std::move( disableAll ),std::move( done ) } ;
	}
}

#endif

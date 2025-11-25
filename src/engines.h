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
#ifndef ENGINES_H
#define ENGINES_H

#include <QString>
#include <QStringList>
#include <QStandardPaths>
#include <QPlainTextEdit>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QDir>
#include <QProcess>
#include <QDateTime>
#include <QNetworkProxy>

#include <vector>
#include <functional>
#include <memory>

#include "logger.h"
#include "util.hpp"
#include "utils/threads.hpp"
#include "utils/qprocess.hpp"

class tableWidget ;
class settings ;
class Context ;
class networkAccess ;

namespace utility {
	class uiIndex ;
}

class engines
{
public:
	static bool filePathIsValid( const QFileInfo& ) ;

	class file
	{
	public:
		file( const QString& path,Logger& logger ) :
			m_filePath( path ),m_file( m_filePath ),m_logger( logger )
		{
		}
		void write( const QString& ) ;
		void write( const QJsonDocument&,
			    QJsonDocument::JsonFormat = QJsonDocument::Indented ) ;
		void write( const QJsonObject&,
			    QJsonDocument::JsonFormat = QJsonDocument::Indented ) ;
		QByteArray readAll() ;
		QStringList readAllAsLines() ;
		template< typename Function >
		static void readAll( const QString& filePath,Logger& logger,Function function )
		{
			struct result
			{
				bool success ;
				QByteArray data ;
			};

			utils::qthread::run( [ filePath ]()->result{

				QFile f( filePath ) ;

				if( f.open( QIODevice::ReadOnly ) ){

					return { true,f.readAll() } ;
				}else{
					return { false,{} } ;
				}

			},[ &logger,filePath,function = std::move( function ) ]( result r ){

				if( r.success ){

					function( true,r.data ) ;
				}else{
					engines::file( filePath,logger ).failToOpenForReading() ;
					function( false,r.data ) ;
				}
			} ) ;
		}
	private:
		void failToOpenForReading() ;
		void failToOpenForWriting() ;
		QString m_filePath ;
		QFile m_file ;
		Logger& m_logger ;
	} ;

	class ProcessExitState
	{
	public:
		enum class ExitStatus{ FailedToStart,NormalExit,Crashed } ;

		ProcessExitState()
		{
		}
		ProcessExitState( bool c,int s,qint64 d,ProcessExitState::ExitStatus e ) :
			m_cancelled( c ),
			m_exitCode( s ),
			m_duration( d ),
			m_exitStatus( e )
		{
		}
		ProcessExitState( bool c,int s,qint64 d,QProcess::ExitStatus e ) :
			m_cancelled( c ),
			m_exitCode( s ),
			m_duration( d )
		{
			if( e == QProcess::ExitStatus::NormalExit ){

				m_exitStatus = ProcessExitState::ExitStatus::NormalExit ;

			}else if( e == QProcess::ExitStatus::CrashExit ){

				m_exitStatus = ProcessExitState::ExitStatus::Crashed ;
			}else{
				m_exitStatus = ProcessExitState::ExitStatus::FailedToStart ;
			}
		}
		int exitCode() const
		{
			return m_exitCode ;
		}
		const ProcessExitState::ExitStatus& exitStatus() const
		{
			return m_exitStatus ;
		}
		bool cancelled() const
		{
			return m_cancelled ;
		}
		bool success() const
		{
			return m_exitCode == 0 && m_exitStatus == ProcessExitState::ExitStatus::NormalExit ;
		}
		qint64 duration() const
		{
			return m_duration ;
		}
		ProcessExitState move()
		{
			return std::move( *this ) ;
		}
	private:
		bool m_cancelled = false ;
		int m_exitCode = 255 ;
		qint64 m_duration = 0 ;
		ProcessExitState::ExitStatus m_exitStatus = ProcessExitState::ExitStatus::NormalExit ;
	};

	class enginePaths
	{
	public:
		enginePaths( settings& ) ;

		const QString& basePath() const
		{
			return m_basePath ;
		}
		const QString& binPath() const
		{
			return m_binPath ;
		}
		const QString& enginePath() const
		{
			return m_enginePath ;
		}
		const QString& dataPath() const
		{
			return m_dataPath ;
		}
		const QString& updatePath() const
		{
			return m_updatePath ;
		}
		const QString& tmp() const
		{
			return m_tmp ;
		}
		const QString updateNewPath() const
		{
			return m_updateNewPath ;
		}
		QString tmp( const QString& e ) const
		{
			return _add( m_tmp,e ) ;
		}
		QString updatePath( const QString& e ) const
		{
			return _add( m_updatePath,e ) ;
		}
		QString dataPath( const QString& e ) const
		{
			return _add( m_dataPath,e ) ;
		}
		QString subscriptionsArchiveFilePath() const
		{
			return _add( m_dataPath,"subscriptions_archive_file.txt" ) ;
		}
		QString binPath( const QString& e ) const
		{
			return _add( m_binPath,e ) ;
		}
		QString themePath() const
		{
			return _add( m_dataPath,"themes" ) ;
		}
		QString enginePath( const QString& e ) const
		{
			return _add( m_enginePath,e ) ;
		}
		QString socketPath() ;
		void confirmPaths( Logger& ) const ;
	private:
		QString _add( const QString& basePath,const QString& toAdd ) const
		{
			if( basePath.endsWith( "/" ) ){

				return basePath + toAdd ;
			}else{
				return basePath + "/" + toAdd ;
			}
		}
		QString m_binPath ;
		QString m_enginePath ;
		QString m_basePath ;
		QString m_dataPath ;
		QString m_updatePath ;
		QString m_updateNewPath ;
		QString m_tmp ;
	} ;

	class metadata
	{
	public:
		metadata( const QJsonObject& obj ) :
			m_url( obj.value( "browser_download_url" ).toString() ),
			m_size( obj.value( "size" ).toInt() ),
			m_fileName( obj.value( "name" ).toString() ),
			m_hash( obj.value( "digest" ).toString() )
		{
		}
		metadata()
		{
		}
		metadata move()
		{
			return std::move( *this ) ;
		}
		const QString& url() const
		{
			return m_url ;
		}
		const QString& fileName() const
		{
			return m_fileName ;
		}
		const QString& hash() const
		{
			return m_hash ;
		}
		qint64 size() const
		{
			return m_size ;
		}
	private:
		QString m_url ;
		qint64 m_size = 0 ;
		QString m_fileName ;
		QString m_hash ;
	} ;

	class engine
	{
	public:
		class exeArgs
		{
		public:
			class cmd
			{
			public:
				cmd()
				{
				}
				cmd( const engines::engine::exeArgs& exeArgs,
				     const QStringList& args ) :
					m_args( exeArgs.exe() ),
					m_exe( m_args.takeAt( 0 ) )
				{
					m_args.append( exeArgs.args() ) ;
					m_args.append( args ) ;
				}
				const QString& exe() const
				{
					return m_exe ;
				}
				const QStringList& args() const
				{
					return m_args ;
				}
				bool valid()
				{
					QFileInfo info( m_exe ) ;

					return engines::filePathIsValid( info ) ;
				}
			private:
				QStringList m_args ;
				QString m_exe ;
			} ;

			exeArgs() = default;
			exeArgs( const QString& e ) :
				m_exe( e ),m_realExe( e )
			{
			}
			exeArgs( const QString& e,const QString& r,const QStringList& s ) :
				m_exe( e ),m_realExe( r ),m_options( s )
			{
			}
			exeArgs( const QStringList& e,const QString& r,const QStringList& s ) :
				m_exe( e ),m_realExe( r ),m_options( s )
			{
			}
			bool isEmpty() const
			{
				return m_realExe.isEmpty() ;
			}
			const QStringList& exe() const
			{
				return m_exe ;
			}
			const QStringList& args() const
			{
				return m_options ;
			}
			const QString& realExe() const
			{
				return m_realExe ;
			}
			void updateRealExe( const QString& e )
			{
				if( m_exe.at( 0 ) == m_realExe ){

					m_exe[ 0 ] = e ;
					m_realExe  = e ;
				}else{
					m_realExe = e ;
				}
			}
		private:
			QStringList m_exe ;
			QString m_realExe ;
			QStringList m_options ;
		} ;

		class baseEngine
		{
		public:
			class finishedState
			{
			public:
				template< typename Args >
				finishedState( const Args& a ) :
				        m_success( a.success() ),
				        m_cancelled( a.cancelled() ),
					m_duration( a.duration() ),
					m_errorCode( a.exitCode() ),
					m_exitStatus( a.exitStatus() )
				{
				}
				bool success() const
				{
					return m_success ;
				}
				bool cancelled() const
				{
				        return m_cancelled ;
				}
				qint64 duration() const
				{
				        return m_duration ;
				}
				int errorCode() const
				{
					return m_errorCode ;
				}
				engines::ProcessExitState::ExitStatus exitStatus() const
				{
					return m_exitStatus ;
				}
			private:
				bool m_success ;
				bool m_cancelled ;
				qint64 m_duration ;
				int m_errorCode ;
				engines::ProcessExitState::ExitStatus m_exitStatus ;
			};

			enum class errors{ runtimeError,unknownUrl,notSupportedUrl,noNetwork,unknownFormat,logInRequired } ;
			static QString errorString( const engine::engine::baseEngine::finishedState&,
						    engines::engine::baseEngine::errors,
						    const QString& ) ;
			static QString processCompleteStateText( const engine::engine::baseEngine::finishedState& ) ;

			class timer
			{
			public:
				static bool timerText( const QString& e ) ;
				static QString timerText() ;
				static QString startTimerText() ;
				static QString stringElapsedTime( qint64 ) ;
				static QString duration( qint64 ) ;
				static int toSeconds( const QString& ) ;
				static qint64 currentTime() ;
				qint64 elapsedTime() ;
				QString stringElapsedTime() ;
				void reset() ;
			private:
				qint64 m_startTime = engines::engine::baseEngine::timer::currentTime() ;
			};

			class preProcessing
			{
			public:
				preProcessing() ;
				preProcessing( const QByteArray&,int = 16 ) ;

				static QByteArray processingText() ;
				void reset() ;
				const QByteArray& text() ;
				const QByteArray& text( const QByteArray& ) ;
			private:
				int m_maxCounter = 16 ;
				int m_counter = 0 ;
				QByteArray m_counterDots ;
				QByteArray m_txt ;
				QByteArray m_processingDefaultText ;
			};

			class postProcessing
			{
			public:
				static QByteArray processingText() ;
				postProcessing() ;
				postProcessing( const QByteArray& ) ;

				const QByteArray& text( const QByteArray& ) ;
			private:
				int m_counter = 0 ;
				QByteArray m_counterDots ;
				QByteArray m_txt ;
				QByteArray m_processingDefaultText ;
			};

			class filter
			{
			public:
				filter( const engines::engine& engine,int ) ;
				virtual const QByteArray& operator()( Logger::Data& e ) ;
				virtual ~filter() ;
				const engines::engine& engine() const ;
			private:
				engines::engine::baseEngine::preProcessing m_processing ;
				const engines::engine& m_engine ;
				QByteArray m_tmp ;
				int m_processId ;
			} ;

			class DataFilter
			{
			public:
				template< typename Type,typename ... Args >
				DataFilter( Type,Args&& ... args ) :
					m_filter( std::make_unique< typename Type::type >( std::forward< Args >( args ) ... ) )
				{
				}
				const QByteArray& operator()( Logger::Data& e )
				{
					return ( *m_filter )( e ) ;
				}
				DataFilter move()
				{
					return std::move( *this ) ;
				}
			private:
				std::unique_ptr< engines::engine::baseEngine::filter > m_filter ;
			};

			class filterOutPut
			{
			public:
				struct args
				{
					const Logger::locale& locale ;
					Logger::Data& data ;
					const QByteArray& outPut ;
				} ;
				class meetCondition
				{
				public:
					meetCondition( bool( *m )( const engines::engine&,const QByteArray& ),
						       const engines::engine& engine ) :
						m_function( m ),m_engine( engine )
					{
					}
					bool operator()( const QByteArray& e ) const
					{
						return m_function( m_engine,e ) ;
					}
				private:
					bool( *m_function )( const engines::engine&,const QByteArray& ) ;
					const engines::engine& m_engine ;
				} ;
				class result
				{
				public:
					result( const QByteArray& p,
						const engines::engine& e,
						bool( *m )( const engines::engine&,const QByteArray& ) ) :
						m_progress( p ),m_meetCondition( m,e )
					{
					}
					const QByteArray& progress()
					{
						return m_progress ;
					}
					const filterOutPut::meetCondition& meetCondition()
					{
						return m_meetCondition ;
					}
				private:
					const QByteArray& m_progress ;
					filterOutPut::meetCondition m_meetCondition ;
				} ;
				virtual result formatOutput( const filterOutPut::args& ) const = 0 ;
				virtual bool meetCondition( const filterOutPut::args& ) const = 0 ;
				virtual const engines::engine& engine() const = 0 ;
				virtual ~filterOutPut() ;
			} ;

			static bool meetCondition( const engines::engine&,const QByteArray& ) ;

			class FilterOutPut
			{
			public:
				template< typename Type,typename ... Args >
				FilterOutPut( Type,const engines::engine& engine,Args&& ... args ) :
					m_filterOutPut( std::make_unique< typename Type::type >( engine,std::forward< Args >( args ) ... ) )
				{
				}
				engines::engine::baseEngine::filterOutPut::result
				formatOutput( const Logger::locale& l,Logger::Data& d,const QByteArray& e ) const
				{
					return m_filterOutPut->formatOutput( { l,d,e } ) ;
				}
				bool meetCondition( const Logger::locale& l,Logger::Data& d,const QByteArray& e ) const
				{
					return m_filterOutPut->meetCondition( { l,d,e } ) ;
				}
			private:
				std::unique_ptr< engines::engine::baseEngine::filterOutPut > m_filterOutPut ;
			};

			virtual FilterOutPut filterOutput() ;

			virtual ~baseEngine() ;

			virtual const QProcessEnvironment& processEnvironment() const ;

			class mediaInfo
			{
			public:
				mediaInfo( const QStringList& u,
					   const QString& i,
					   const QString& e,
					   const QString& r,
					   const QString& f,
					   const QString& ff,
					   const QString& n,
					   const QString& d,
					   const QString& t ) :
					m_url( u ),
					m_id( i ),
					m_extension( e ),
					m_resolution( r ),
					m_fileSize( f ),
					m_fileSizeRaw( ff ),
					m_info( n ),
					m_duration( d ),
					m_title( t )
				{
				}
				mediaInfo( const QString& i,
					   const QString& e,
					   const QString& r,
					   const QString& f,
					   const QString& ff,
					   const QString& n,
					   const QString& d,
					   const QString& t ) :
					m_id( i ),
					m_extension( e ),
					m_resolution( r ),
					m_fileSize( f ),
					m_fileSizeRaw( ff ),
					m_info( n ),
					m_duration( d ),
					m_title( t )
				{
				}
				QJsonObject toqJsonObject() const
				{
					QJsonObject obj ;

					QJsonArray arr ;

					for( const auto& it : m_url ){

						arr.append( it ) ;
					}

					obj.insert( "urls",arr ) ;
					obj.insert( "id",m_id ) ;
					obj.insert( "extension",m_extension ) ;
					obj.insert( "resolution",m_resolution ) ;
					obj.insert( "filesize",m_fileSize ) ;
					obj.insert( "filesizeRaw",m_fileSizeRaw ) ;
					obj.insert( "info",m_info ) ;
					obj.insert( "duration",m_duration ) ;
					obj.insert( "title",m_title ) ;

					return obj ;
				}
				static QString fileSizeRaw( const QJsonObject& obj )
				{
					return obj.value( "filesizeRaw" ).toString() ;
				}
				static QString id( const QJsonObject& obj )
				{
					return obj.value( "id" ).toString() ;
				}
				template< typename Function >
				static void fromQJobject( const QJsonObject& obj,const Function& function )
				{
					auto a = obj.value( "id" ).toString() ;
					auto b = obj.value( "extension" ).toString() ;
					auto c = obj.value( "resolution" ).toString() ;
					auto d = obj.value( "filesize" ).toString() ;
					auto e = obj.value( "info" ).toString() ;

					function( a,b,c,d,e ) ;
				}
				const QString& id() const
				{
					return m_id ;
				}
				const QString& ext() const
				{
					return m_extension ;
				}
				const QString& resolution() const
				{
					return m_resolution ;
				}
				const QString& fileSize() const
				{
					return m_fileSize ;
				}
				const QString& fileSizeRaw() const
				{
					return m_fileSizeRaw ;
				}
				const QString& info() const
				{
					return m_info ;
				}
				const QString& title() const
				{
					return m_title ;
				}
				const QString& duration() const
				{
					return m_duration ;
				}
				mediaInfo move()
				{
					return std::move( *this ) ;
				}
			private:
				QStringList m_url ;
				QString m_id ;
				QString m_extension ;
				QString m_resolution ;
				QString m_fileSize ;
				QString m_fileSizeRaw ;
				QString m_info ;
				QString m_duration ;
				QString m_title ;
			} ;

			virtual engines::metadata parseJsonDataFromGitHub( const QJsonDocument& ) ;

			virtual std::vector< engines::engine::baseEngine::mediaInfo > mediaProperties( Logger&,const QByteArray& ) ;

			virtual std::vector< engines::engine::baseEngine::mediaInfo > mediaProperties( Logger&,const QJsonArray& ) ;

			virtual void updateOutPutChannel( QProcess::ProcessChannel& ) const ;

			virtual bool breakShowListIfContains( const QStringList& ) ;

			virtual bool supportsShowingComments() ;

			virtual bool updateVersionInfo() ;

			virtual QByteArray parseError( const QByteArray& ) ;

			virtual void setTextEncondig( const QString&,QStringList& opts ) ;

			virtual QJsonObject parseJson( const QString&,const QByteArray& ) ;

			virtual std::vector< QByteArray > parseJsonData( QByteArray& ) ;

			virtual QString updateCmdPath( const QString& ) ;

			virtual engines::engine::baseEngine::DataFilter Filter( int ) ;

			virtual QString deleteEngineBinFolder( const QString& ) ;

			virtual void runCommandOnDownloadedFile( const std::vector< QByteArray >& ) ;

			virtual QString commandString( const engines::engine::exeArgs::cmd& ) ;

			virtual QStringList horizontalHeaderLabels() const ;

			virtual void updateEnginePaths( const Context&,QString& filePath,QString& exeBinPath,QString& exeFolderPath ) ;

			virtual QString parseVersionInfo( const utils::qprocess::outPut& ) ;

			virtual void updateLocalOptions( QStringList& ) ;

			class removeFilesStatus
			{
			public:
				class pair
				{
				public:
					pair( QString s,QString e ) :
						m_src( std::move( s ) ),m_err( std::move( e ) )
					{
					}
					const QString& src() const
					{
						return m_src ;
					}
					const QString& err() const
					{
						return m_err ;
					}
				private:
					QString m_src ;
					QString m_err ;
				} ;
				removeFilesStatus( QString s,QString e )
				{
					m_entries.emplace_back( std::move( s ),std::move( e ) ) ;
				}
				removeFilesStatus()
				{
				}
				auto begin() const
				{
					return m_entries.begin() ;
				}
				auto end() const
				{
					return m_entries.end() ;
				}
				auto size() const
				{
					return m_entries.size() ;
				}
				template< typename ... Args >
				void add( Args&& ... args )
				{
					m_entries.emplace_back( std::forward< Args >( args ) ... ) ;
				}
			private:
				std::vector< pair > m_entries ;
			} ;

			virtual engines::engine::baseEngine::removeFilesStatus removeFiles( const QStringList&,const QString& ) ;

			class optionsEnvironment
			{
			public:
				optionsEnvironment()
				{
				}
				optionsEnvironment( QString key,QString value )
				{
					m_pairs.emplace_back( std::move( key ),std::move( value ) ) ;
				}
				void add( QString key,QString value )
				{
					m_pairs.emplace_back( std::move( key ),std::move( value ) ) ;
				}
				bool isEmpty() const
				{
					return m_pairs.size() ;
				}
				QProcessEnvironment update( const QProcessEnvironment& ) const ;
			private:
				struct pair
				{
					pair( QString k,QString v ) :
						key( std::move( k ) ),value( std::move( v ) )
					{
					}
					QString key ;
					QString value ;
				} ;
				std::vector< pair > m_pairs ;
			} ;
			virtual optionsEnvironment setProxySetting( QStringList&,const QString& ) ;

			virtual QString setCredentials( QStringList&,QStringList & ) ;

			virtual util::Json parsePlayListData( const QString&,const QByteArray& ) ;

			virtual QString downloadFolder( const QString& ) ;

			struct localFile
			{
				const QString& uiText ;
				const QString& downloadFolder ;
				const std::vector< QByteArray >& fileNames ;
			};

			virtual void openLocalFile( const engines::engine::baseEngine::localFile& ) ;

			struct onlineVersion
			{
				QString stringVersion ;
				util::version version ;
				onlineVersion move()
				{
					return std::move( *this ) ;
				}
			};

			virtual engines::engine::baseEngine::onlineVersion versionInfoFromGithub( const QByteArray& ) ;

			virtual bool foundNetworkUrl( const QString& ) ;

			class renameArchiveFolderStatus
			{
			public:
				renameArchiveFolderStatus( const QString& s,const QString& d,const QString& e ) :
					m_src( s ),m_dst( d ),m_err( e )
				{
				}
				renameArchiveFolderStatus()
				{
				}
				const QString& src() const
				{
					return m_src ;
				}
				const QString& dst() const
				{
					return m_dst ;
				}
				const QString& err() const
				{
					return m_err ;
				}
				bool success() const
				{
					return m_err.isEmpty() ;
				}
			private:
				QString m_src ;
				QString m_dst ;
				QString m_err ;
			} ;
			virtual renameArchiveFolderStatus renameArchiveFolder( const QString&,const QString& ) ;

			QString updateTextOnCompleteDownlod( const QString& uiText,
							     const QString& downloadingOptions,
							     const QString& tabName,
							     const engine::engine::baseEngine::finishedState& ) ;

			virtual QString updateTextOnCompleteDownlod( const QString& uiText,
								     const QString& bkText,
								     const QString& downloadingOptions,
								     const QString& tabName,
								     const engine::engine::baseEngine::finishedState& ) ;

			virtual void sendCredentials( const QString&,QProcess& ) ;

			virtual void processData( Logger::Data&,const QByteArray&,int id,bool readableJson ) ;

			virtual void processData( Logger::Data&,const QString&,int id,bool readableJson ) ;

			struct updateOpts
			{
				template< typename Args,typename Ent >
				updateOpts( const Args& args,
					    const Ent& ent,
					    const utility::uiIndex& ui,
					    QStringList& u,
					    QStringList& o ) :
					uiOptions( args.uiDownloadOptions() ),
					userOptions( args.otherOptions() ),
					uiIndex( ui ),
					credentials( args.credentials() ),
					playlist( ent.playlist ),
					playlist_count( ent.playlist_count ),
					playlist_id( ent.playlist_id ),
					playlist_title( ent.playlist_title ),
					playlist_uploader( ent.playlist_uploader ),
					playlist_uploader_id( ent.playlist_uploader_id ),
					n_entries( ent.n_entries ),
					urls( u ),
					ourOptions( o )
				{
				}
				const QStringList& uiOptions ;
				const QStringList& userOptions ;
				const utility::uiIndex& uiIndex ;
				const QString& credentials ;
				const QString& playlist ;
				const QString& playlist_count ;
				const QString& playlist_id ;
				const QString& playlist_title ;
				const QString& playlist_uploader ;
				const QString& playlist_uploader_id ;
				const QString& n_entries ;
				QStringList& urls ;
				QStringList& ourOptions ;
			};

			virtual void updateDownLoadCmdOptions( const engines::engine::baseEngine::updateOpts&,
							       bool,
							       const QStringList& ) ;

			virtual void updateGetPlaylistCmdOptions( QStringList& ) ;

			virtual void updateCmdOptions( QStringList& ) ;

			baseEngine( settings&,const engines::engine&,const QProcessEnvironment& e ) ;
			settings& Settings() const ;
			const engines::engine& engine() const ;
		private:
			settings& m_settings ;
			const engines::engine& m_engine ;
			const QProcessEnvironment& m_processEnvironment ;
			static bool meetExtraCondition( const QByteArray&,const QJsonObject& ) ;
		} ;

		engine()
		{
		}
		engine( const engines& engines,
			Logger& logger,
			const QString& name,
			const QString& versionArgument,
			int line,
			int position,
			int id ) ;

		engine( Logger& logger,
			const enginePaths& ePaths,
			const util::Json& json,
			const engines& engines,
			int id ) ;

		static QString mediaAlreadInArchiveText()
		{
			return QObject::tr( "Media Already In Archive" ) ;
		}

		const QString& name() const
		{
			return m_name ;
		}
		const QString& configFileVersion() const
		{
			return m_configVersion ;
		}
		bool forTesting() const
		{
			return this->name().endsWith( "-test" ) ;
		}
		void updateLocalOptions( QStringList& opts ) const
		{
			m_engine->updateLocalOptions( opts ) ;
		}

		template< typename Context,typename Function >
		class uvic
		{
		public:
			uvic( const engines::engine& engine,
			      const Context& ctx,
			      Function function ) :
				m_engine( engine ),
				m_ctx( ctx ),
				m_function( std::move( function ) )
			{
			}
			void operator()( const utils::qprocess::outPut& e )
			{
				if( e.success() ){

					m_engine.setVersionString( e.stdOut ) ;
				}

				m_ctx.TabManager().enableAll() ;

				m_function() ;
			}
			uvic< Context,Function > move()
			{
				return std::move( *this ) ;
			}
		private:
			const engines::engine& m_engine ;
			const Context& m_ctx ;
			Function m_function ;
		} ;

		template< typename Context,typename Function >
		void updateVersionInfo( const Context& ctx,Function ff ) const
		{
			if( m_engine->updateVersionInfo() ){

				const auto& engine = *this ;

				if( engine.versionInfo().valid() ){

					ff() ;
				}else{
					ctx.TabManager().disableAll() ;

					const auto& exe = engine.exePath() ;
					QStringList args{ engine.versionArgument() } ;

					engines::engine::exeArgs::cmd cmd( exe,args ) ;

					this->setPermissions( cmd.exe() ) ;

					uvic< Context,Function > meaw( engine,ctx,std::move( ff ) ) ;

					auto m = QProcess::SeparateChannels ;

					utils::qprocess::run( cmd.exe(),cmd.args(),m,meaw.move() ) ;
				}
			}else{
				ff() ;
			}
		}

		template< typename backend,typename ... Args >
		void setBackend( const engines& engines,Args&& ... args )
		{
			m_engine = std::make_unique< backend >( engines,
								*this,
								m_jsonObject,
								std::forward< Args >( args ) ... ) ;

			this->updateOptions() ;
		}

		QString updateCmdPath( Logger&,const QString& e ) const ;

		const QString& commandName() const ;

		bool breakShowListIfContains( const QStringList& e ) const ;

		const QString& versionArgument() const
		{
			return m_versionArgument ;
		}

		QString setVersionString( const QString& data ) const ;
		QString versionString( const QString& data ) const ;

		const util::version& versionInfo() const
		{
			return m_version ;
		}
		const QString& optionsArgument() const
		{
			return m_optionsArgument ;
		}
		const QString& cookieTextFileArgument() const
		{
			return m_cookieTextFileArgument ;
		}
		const QString& downloadUrl() const
		{
			return m_downloadUrl ;
		}
		bool validDownloadUrl() const ;
		bool supportsTextEnconding() const
		{
			return !m_encodingArgument.isEmpty() ;
		}
		void setTextEncondig( QStringList& opts ) const
		{
			m_engine->setTextEncondig( m_encodingArgument,opts ) ;
		}
		const QProcessEnvironment& processEnvironment() const
		{
			return m_engine->processEnvironment() ;
		}
		void processData( Logger::Data& outPut,const QByteArray& data,int id,bool readableJson ) const
		{
			m_engine->processData( outPut,data,id,readableJson ) ;
		}
		void processData( Logger::Data& outPut,const QString& data,int id,bool readableJson ) const
		{
			m_engine->processData( outPut,data,id,readableJson ) ;
		}
		engines::engine::baseEngine::renameArchiveFolderStatus renameArchiveFolder( const QString& s,const QString& e ) const
		{
			return m_engine->renameArchiveFolder( s,e ) ;
		}
		QString commandString( const engines::engine::exeArgs::cmd& cmd ) const
		{
			return m_engine->commandString( cmd ) ;
		}
		void runCommandOnDownloadedFile( const std::vector< QByteArray >& fileNames ) const
		{
			m_engine->runCommandOnDownloadedFile( fileNames ) ;
		}
		const QStringList& defaultDownLoadCmdOptions() const
		{
			return m_defaultDownLoadCmdOptions ;
		}
		QString setCredentials( QStringList& e,QStringList& s ) const
		{
			return m_engine->setCredentials( e,s ) ;
		}
		enum class tab{ basic,batch,playlist } ;
		QStringList dumpJsonArguments( engines::engine::tab ) const ;

		engines::engine::baseEngine::DataFilter filter( int processId ) const
		{
			return m_engine->Filter( processId ) ;
		}
		QString updateTextOnCompleteDownlod( const QString& uiText,
						     const QString& bkText,
						     const QString& dopts,
						     const QString& tabName,
						     const engine::engine::baseEngine::finishedState& f ) const
		{
			return m_engine->updateTextOnCompleteDownlod( uiText,bkText,dopts,tabName,f ) ;
		}
		void updateDownLoadCmdOptions( const engines::engine::baseEngine::updateOpts& u,bool e ) const
		{
			m_engine->updateDownLoadCmdOptions( u,e,this->extraArguments() ) ;
		}
		void sendCredentials( const QString& credentials,QProcess& exe ) const
		{
			m_engine->sendCredentials( credentials,exe ) ;
		}
		std::vector< engines::engine::baseEngine::mediaInfo > mediaProperties( Logger& l,const QByteArray& e ) const
		{
			return m_engine->mediaProperties( l,e ) ;
		}
		void updateGetPlaylistCmdOptions( QStringList& e ) const
		{
			m_engine->updateGetPlaylistCmdOptions( e ) ;

			e.append( this->extraArguments() ) ;
		}
		void updateCmdOptions( QStringList& e ) const
		{
			m_engine->updateCmdOptions( e ) ;

			e.append( this->extraArguments() ) ;
		}
		util::Json parsePlayListData( const QString& m,const QByteArray& e ) const
		{
			return m_engine->parsePlayListData( m,e ) ;
		}
		bool archiveContainsFolder() const
		{
			return m_archiveContainsFolder ;
		}
		QString deleteEngineBinFolder( const QString& e ) const
		{
			return m_engine->deleteEngineBinFolder( e ) ;
		}
		engines::metadata parseJsonDataFromGitHub( const QJsonDocument& e ) const
		{
			return m_engine->parseJsonDataFromGitHub( e ) ;
		}
		engines::engine::baseEngine::FilterOutPut filterOutput() const
		{
			return m_engine->filterOutput() ;
		}
		bool foundNetworkUrl( const QString& s ) const
		{
			return m_engine->foundNetworkUrl( s ) ;
		}
		engines::engine::baseEngine::onlineVersion versionInfoFromGithub( const QByteArray& e ) const
		{
			return m_engine->versionInfoFromGithub( e ) ;
		}
		QString downloadFolder( const QString& e ) const
		{
			return m_engine->downloadFolder( e ) ;
		}
		engines::engine::baseEngine::optionsEnvironment setProxySetting( QStringList& e,const QString& s ) const
		{
			return m_engine->setProxySetting( e,s ) ;
		}
		std::vector< QByteArray > parseJsonData( QByteArray& data ) const
		{
			return m_engine->parseJsonData( data ) ;
		}
		QStringList horizontalHeaderLabels() const
		{
			return m_engine->horizontalHeaderLabels() ;
		}
		QJsonObject parseJson( const QString& url,const QByteArray& e ) const
		{
			return m_engine->parseJson( url,e ) ;
		}
		QString parseVersionInfo( const utils::qprocess::outPut& e ) const
		{
			return m_engine->parseVersionInfo( e ) ;
		}
		void openLocalFile( const engines::engine::baseEngine::localFile& s ) const
		{
			m_engine->openLocalFile( s ) ;
		}
		engines::engine::baseEngine::removeFilesStatus removeFiles( const QStringList& e,const QString& s ) const
		{
			return m_engine->removeFiles( e,s ) ;
		}
		QByteArray parseError( const QByteArray& e ) const
		{
			return m_engine->parseError( e ) ;
		}
		void updateOutPutChannel( QProcess::ProcessChannel& s ) const
		{
			m_engine->updateOutPutChannel( s ) ;
		}
		std::vector< engines::engine::baseEngine::mediaInfo > mediaProperties( Logger& l,const QJsonArray& e ) const
		{
			return m_engine->mediaProperties( l,e ) ;
		}
		void updateEnginePaths( const Context& ctx,QString& filePath,QString& exeBinPath,QString& exeFolderPath ) const
		{
			m_engine->updateEnginePaths( ctx,filePath,exeBinPath,exeFolderPath ) ;
		}
		const QStringList& defaultListCmdOptions() const
		{
			return m_defaultListCmdOptions ;
		}
		const QStringList& defaultCommentsCmdOptions() const
		{
			return m_defaultCommentsCmdOptions ;
		}
		const QStringList& defaultSubstitlesCmdOptions() const
		{
			return m_defaultSubstitlesCmdOptions ;
		}
		const QStringList& defaultSubtitleDownloadOptions() const
		{
			return m_defaultSubtitleDownloadOptions ;
		}
		const QStringList& skiptLineWithText() const
		{
			return m_skiptLineWithText ;
		}
		const QStringList& removeText() const
		{
			return m_removeText ;
		}
		const QStringList& splitLinesBy() const
		{
			return m_splitLinesBy ;
		}
		const exeArgs& exePath() const
		{
			return m_exePath ;
		}
		const QString& batchFileArgument() const
		{
			return m_batchFileArgument ;
		}
		const QString& exeFolderPath() const
		{
			return m_exeFolderPath ;
		}
		const QString& userName() const
		{
		        return m_userName ;
		}
		const QString& password() const
		{
		        return m_password ;
		}		
		const QString& playListUrlPrefix() const
		{
			return m_playListUrlPrefix ;
		}
		const QString& playlistItemsArgument() const
		{
			return m_playlistItemsArgument ;
		}
		const QString& cookieArgument() const
		{
			return m_cookieArgument ;
		}
		const QStringList extraArguments() const
		{
			return m_extraArguments ;
		}
		const QJsonObject& controlStructure() const
		{
			return m_controlStructure ;
		}
		bool canDownloadMediaPart() const
		{
			return this->likeYtDlp() ;
		}
		bool valid() const
		{
			return m_valid ;
		}
		bool supportShowingComments() const
		{
			return m_engine->supportsShowingComments() ;
		}
		bool canDownloadPlaylist() const
		{
			return m_canDownloadPlaylist ;
		}
		bool likeYtDlp() const
		{
			return m_likeYtDlp ;
		}
		bool isGalleryDl() const
		{
			return this->name() == "gallery-dl" ;
		}
		bool canShowMetaData() const
		{
			return m_likeYtDlp || this->isGalleryDl() ;
		}
		bool backendExists() const
		{
			QFileInfo m( m_exePath.realExe() ) ;
			return m.exists() && m.isFile() ;
		}
		bool autoUpdate() const
		{
			return m_autoUpdate ;
		}
		bool supportingEngine() const
		{
			return m_supportingEngine ;
		}
		bool replaceOutputWithProgressReport() const
		{
			return m_replaceOutputWithProgressReport ;
		}
		void setBroken() const
		{
			m_broken = true ;
		}
		bool broken() const
		{
			return m_broken ;
		}
	private:
		void setPermissions( const QString& ) const ;
		void updateOptions() ;
		QStringList toStringList( const QJsonValue&,bool = false ) const ;
		QJsonObject getCmd( const QJsonObject& ) ;

		struct cmd
		{
			QString name ;
			QStringList args ;
			bool noCheckArgs ;
		} ;

		engines::engine::cmd getCommands( const QJsonObject& ) ;

		void parseMultipleCmdArgs( Logger& logger,const engines& engines,const enginePaths&,int ) ;

		void parseMultipleCmdArgs( QStringList&,
					   const QString&,
					   Logger& logger,
					   const enginePaths& ePaths,
					   const engines& engines,
					   int ) ;

		mutable util::version m_version ;
		QJsonObject m_jsonObject ;
		std::unique_ptr< engines::engine::baseEngine > m_engine ;
		int m_line ;
		int m_position ;
		bool m_valid ;
		bool m_likeYtDlp ;
		bool m_autoUpdate ;
		bool m_canDownloadPlaylist ;
		bool m_supportingEngine ;
		bool m_archiveContainsFolder = false ;
		bool m_replaceOutputWithProgressReport ;
		mutable bool m_broken = false ;
		QString m_versionArgument ;
		QString m_name ;
		QString m_configVersion ;
		QString m_commandName ;
		QString m_userName ;
		QString m_password ;
		QString m_exeFolderPath ;
		QString m_optionsArgument ;
		QString m_downloadUrl ;
		QString m_playListUrlPrefix ;
		QString m_playlistItemsArgument ;
		QString m_batchFileArgument ;
		QString m_cookieArgument ;
		QString m_cookieTextFileArgument ;
		QString m_encodingArgument ;
		QStringList m_extraArguments ;
		QStringList m_dumpJsonArguments ;
		QStringList m_splitLinesBy ;
		QStringList m_removeText ;
		QStringList m_skiptLineWithText ;
		QStringList m_defaultDownLoadCmdOptions ;
		QStringList m_defaultListCmdOptions ;
		QStringList m_defaultCommentsCmdOptions ;
		QStringList m_defaultSubstitlesCmdOptions ;
		QStringList m_defaultSubtitleDownloadOptions ;

		QJsonObject m_controlStructure ;

		mutable engines::engine::exeArgs m_exePath ;
	};
	settings& Settings() const;
	QString findExecutable( const QString& exeName ) const ;
	const QProcessEnvironment& processEnvironment() const ;
	QString addEngine( const QByteArray& data,const QString& path,int ) ;
	void removeEngine( const QString& name,int ) ;
	QStringList enginesList() const ;
	const engine& defaultEngine( const QString&,int ) const ;
	util::result_ref< const engines::engine& > getEngineByName( const QString& name ) const ;
	const enginePaths& engineDirPaths() const ;
	engines( Logger&,const engines::enginePaths&,settings&,int ) ;
	void openUrls( tableWidget&,int row,const engines::engine& ) const ;
	void openUrls( const QString& path ) const ;
	const QString& defaultEngineName() const ;
	class Iterator
	{
	public:
		Iterator( const std::vector< engines::engine >& engines,int id ) :
			m_maxCounter( engines.size() ),
			m_engines( &engines ),
			m_id( id )
		{
		}
		Iterator( const engines::engine& engine,int id ) :
			m_maxCounter( 1 ),
			m_engine( &engine ),
			m_id( id )
		{
		}
		size_t size() const
		{
			return m_maxCounter ;
		}
		bool hasNext() const
		{
			return m_counter + 1 < m_maxCounter ;
		}
		engines::Iterator next() const
		{
			auto m = *this ;
			m.m_counter++ ;
			return m ;
		}
		const engines::engine& engine() const
		{
			if( m_engine ){

				return *m_engine ;
			}else{
				return ( *m_engines )[ m_counter ] ;
			}
		}
		int id() const
		{
			return m_id ;
		}
		Iterator move()
		{
			return std::move( *this ) ;
		}
	private:
		size_t m_counter = 0 ;
		size_t m_maxCounter ;
		const engines::engine * m_engine = nullptr ;
		const std::vector< engines::engine > * m_engines = nullptr ;
		int m_id ;
	} ;

	const std::vector< engine >& getEngines() const ;
	engines::Iterator getEnginesIterator() const ;
	void setDefaultEngine( const QString& ) ;
	void showBanner() ;
	class proxySettings
	{
	public:
		proxySettings()
		{
		}
		proxySettings( const QByteArray& e ) :
			m_networkProxyString( e ),
			m_networkProxy( this->toQNetworkProxy( m_networkProxyString ) )
		{
		}
		proxySettings( const QString& e ) :
			m_networkProxyString( e ),
			m_networkProxy( this->toQNetworkProxy( m_networkProxyString ) )
		{
			this->setDefaultProxy() ;
		}
		proxySettings( const QNetworkProxy& e ) :
			m_networkProxyString( this->toString( e ) ),
			m_networkProxy( e )
		{
			this->setDefaultProxy() ;
		}
		const QString& networkProxyString() const
		{
			return m_networkProxyString ;
		}
		const QNetworkProxy& networkProxy() const
		{
			return m_networkProxy ;
		}
		bool isSet() const
		{
			return !m_networkProxyString.isEmpty() ;
		}
		proxySettings move()
		{
			return std::move( *this ) ;
		}
		bool operator!=( const proxySettings &other ) const ;
		QNetworkProxy toQNetworkProxy( const QString& e ) const ;
		void setApplicationProxy( const QString& ) const ;
		void setDefaultProxy() const ;
	private:
		QString toString( const QNetworkProxy& ) const ;
		QString m_networkProxyString ;
		mutable QString m_currentProxyString ;
		QNetworkProxy m_networkProxy ;
	};
	const engines::proxySettings& networkProxy() const
	{
		return m_networkProxy ;
	}
	void setNetworkProxy( engines::proxySettings,bool,networkAccess& ) ;
private:
	void updateEngines( bool,int ) ;
	util::result< engines::engine > getEngineByPath( const QString& ) const ;
	util::result_ref< const engines::engine& > getCompleteEngineByPath( const QString& ) const ;
	void engineAdd( const QString&,util::result< engines::engine >,int ) ;
	QString findExecutable( const QString&,const QStringList&,QFileInfo& ) const ;
	QProcessEnvironment getEnvPaths() const ;
	QStringList dirEntries( const QString& ) const ;
	Logger& m_logger ;
	settings& m_settings ;
	std::vector< engine > m_backends ;
	const engines::enginePaths& m_enginePaths ;
	QProcessEnvironment m_processEnvironment ;
	engines::proxySettings m_networkProxy ;
	int m_bannerId ;
	class configDefaultEngine
	{
	public:
		configDefaultEngine( Logger& logger,const enginePaths& enginePath ) ;

		const QString& name() const
		{
			return m_name ;
		}
		const QString& configFileName() const
		{
			return m_configFileName ;
		}
	private:
		QString m_name ;
		QString m_configFileName ;
	} ;

	engines::configDefaultEngine m_defaultEngine ;
};

#endif

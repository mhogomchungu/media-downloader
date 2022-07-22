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
#include <QJsonDocument>
#include <QDir>
#include <QProcess>
#include <QDateTime>

#include <vector>
#include <functional>
#include <memory>

#include "logger.h"
#include "util.hpp"

class tableWidget ;
class settings ;
class Context ;

class engines{
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

			util::runInBgThread( [ filePath ]()->result{

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
		QString updatePath( const QString& e ) const
		{
			return m_updatePath + "/" +  e ;
		}
		QString dataPath( const QString& e ) const
		{
			return m_dataPath + "/" + e ;
		}
		QString binPath( const QString& e ) const
		{
			return m_binPath + "/" + e ;
		}
		QString themePath() const
		{
			return m_basePath + "/themes" ;
		}
		QString enginePath( const QString& e ) const
		{
			return m_enginePath + "/" + e ;
		}
		QString socketPath() ;
	private:
		QString m_binPath ;
		QString m_enginePath ;
		QString m_basePath ;
		QString m_dataPath ;
		QString m_updatePath ;
	};

	class engine
	{
	public:
		class exeArgs
		{
		public:
			class cmd
			{
			public:
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
		private:
			QStringList m_exe ;
			QString m_realExe ;
			QStringList m_options ;
		} ;

		class functions
		{
		public:
			class finishedState
			{
			public:
				template< typename Args >
				finishedState( const Args& a ) :
				        m_success( a.success() ),
				        m_cancelled( a.cancelled() ),
				        m_duration( a.duration() )
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
				int duration() const
				{
				        return m_duration ;
				}
			private:
				bool m_success ;
				bool m_cancelled ;
				int m_duration ;
			};

			static QString processCompleteStateText( const engine::engine::functions::finishedState& ) ;

			class timer
			{
			public:
				static bool timerText( const QString& e ) ;
				static QString timerText() ;
				static QString startTimerText() ;
				static QString stringElapsedTime( int ) ;
				static QString duration( int ) ;
				static int toSeconds( const QString& ) ;
				int elapsedTime() ;
				QString stringElapsedTime() ;
			private:
				qint64 m_startTime = QDateTime().currentMSecsSinceEpoch() ;
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

			class filter{
			public:
				filter( const QString& quality,const engines::engine& engine,int ) ;
				virtual const QByteArray& operator()( const Logger::Data& e ) ;
				virtual ~filter() ;
				const engines::engine& engine() const ;
			protected:
				const QString& quality() const ;
			private:
				engines::engine::functions::preProcessing m_processing ;
				QString m_quality ;
				const engines::engine& m_engine ;
				QByteArray m_tmp ;
				int m_processId ;
			} ;

			class DataFilter{
			public:
				template< typename Type,typename ... Args >
				DataFilter( Type,Args&& ... args ) :
					m_filter( std::make_unique< typename Type::type >( std::forward< Args >( args ) ... ) )
				{
				}
				const QByteArray& operator()( const Logger::Data& e )
				{
					return ( *m_filter )( e ) ;
				}
			private:
				std::unique_ptr< engines::engine::functions::filter > m_filter ;
			};

			virtual ~functions() ;

			virtual const QProcessEnvironment& processEnvironment() const ;

			virtual std::vector< QStringList > mediaProperties( const QByteArray& ) ;

			virtual std::vector< QStringList > mediaProperties( const QJsonArray& ) ;

			virtual void updateOutPutChannel( QProcess::ProcessChannel& ) const ;

			virtual bool breakShowListIfContains( const QStringList& ) ;

			virtual bool supportsShowingComments() ;

			virtual bool updateVersionInfo() ;

			virtual engines::engine::functions::DataFilter Filter( int,const QString& ) ;

			virtual void runCommandOnDownloadedFile( const QString&,const QString& ) ;

			virtual QString commandString( const engines::engine::exeArgs::cmd& ) ;

			virtual QStringList dumpJsonArguments() ;

			virtual QStringList horizontalHeaderLabels() const ;

			virtual void updateEnginePaths( const Context&,QString& filePath,QString& exeBinPath,QString& exeFolderPath ) ;

			virtual bool parseOutput( Logger::Data&,const QByteArray&,int,bool ) ;

			virtual bool foundNetworkUrl( const QString& ) ;

			QString updateTextOnCompleteDownlod( const QString& uiText,
							     const QString& downloadingOptions,
							     const engine::engine::functions::finishedState& ) ;

			virtual QString updateTextOnCompleteDownlod( const QString& uiText,
								     const QString& bkText,
								     const QString& downloadingOptions,
								     const engine::engine::functions::finishedState& ) ;

			virtual void sendCredentials( const QString&,QProcess& ) ;

			virtual void processData( Logger::Data&,const QByteArray&,int id,bool readableJson ) ;

			virtual void processData( Logger::Data&,const QString&,int id,bool readableJson ) ;

			struct updateOpts
			{
				const QString& quality ;
				const QStringList& userOptions ;
				const QString& indexAsString ;
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

			virtual void updateDownLoadCmdOptions( const engines::engine::functions::updateOpts& ) ;

			virtual void updateGetPlaylistCmdOptions( QStringList& ) ;

			functions( settings&,const engines::engine&,const QProcessEnvironment& e ) ;
			settings& Settings() const ;
			const engines::engine& engine() const ;
		private:
			settings& m_settings ;
			const engines::engine& m_engine ;
			const QProcessEnvironment& m_processEnvironment ;
		} ;

		engine( const Context& ) ;

		engine( Logger& l ) ;

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

		template< typename Function >
		void updateVersionInfo( Function function ) const
		{
			if( m_functions->updateVersionInfo() ){

				const auto& engine = *this ;

				if( engine.versionInfo().valid() ){

					function() ;
				}else{
					const auto& exe = engine.exePath() ;
					QStringList args{ engine.versionArgument() } ;

					engines::engine::exeArgs::cmd cmd( exe,args ) ;

					util::run( cmd.exe(),cmd.args(),[ &engine,function = std::move( function ) ]( const util::run_result& e ){

						if( e.success() ){

							engine.setVersionString( e.stdOut ) ;
						}

						function() ;
					} ) ;
				}
			}else{
				function() ;
			}
		}

		template< typename backend,typename ... Args >
		void setBackend( const engines& engines,Args&& ... args )
		{
			m_functions = std::make_unique< backend >( engines,
								   *this,
								   m_jsonObject,
								   std::forward< Args >( args ) ... ) ;

			this->updateOptions() ;
		}
		const QString& commandName() const ;

		bool breakShowListIfContains( const QStringList& e ) const ;

		const QString& versionArgument() const
		{
			return m_versionArgument ;
		}

		QString setVersionString( const QString& data ) const ;

		const util::version& versionInfo() const
		{
			return m_version ;
		}
		const QString& optionsArgument() const
		{
			return m_optionsArgument ;
		}
		const QString& downloadUrl() const
		{
			return m_downloadUrl ;
		}
		bool validDownloadUrl() const
		{
			return m_downloadUrl.startsWith( "https://api.github.com" ) ;
		}
		const QProcessEnvironment& processEnvironment() const
		{
			return m_functions->processEnvironment() ;
		}
		QStringList dumpJsonArguments() const
		{
			return m_functions->dumpJsonArguments() ;
		}
		void processData( Logger::Data& outPut,const QByteArray& data,int id,bool readableJson ) const
		{
			m_functions->processData( outPut,data,id,readableJson ) ;
		}
		void processData( Logger::Data& outPut,const QString& data,int id,bool readableJson ) const
		{
			m_functions->processData( outPut,data,id,readableJson ) ;
		}
		QString commandString( const engines::engine::exeArgs::cmd& cmd ) const
		{
			return m_functions->commandString( cmd ) ;
		}
		void runCommandOnDownloadedFile( const QString& e,const QString& s ) const
		{
			m_functions->runCommandOnDownloadedFile( e,s ) ;
		}
		const QStringList& defaultDownLoadCmdOptions() const
		{
			return m_defaultDownLoadCmdOptions ;
		}
		engines::engine::functions::DataFilter filter( int processId,const QString& quality ) const
		{
			return m_functions->Filter( processId,quality ) ;
		}
		QString updateTextOnCompleteDownlod( const QString& uiText,
						     const QString& bkText,
						     const QString& dopts,
						     const engine::engine::functions::finishedState& f ) const
		{
			return m_functions->updateTextOnCompleteDownlod( uiText,bkText,dopts,f ) ;
		}
		void updateDownLoadCmdOptions( const engines::engine::functions::updateOpts& u ) const
		{
			m_functions->updateDownLoadCmdOptions( u ) ;
		}
		void sendCredentials( const QString& credentials,QProcess& exe ) const
		{
			m_functions->sendCredentials( credentials,exe ) ;
		}
		std::vector< QStringList > mediaProperties( const QByteArray& e ) const
		{
			return m_functions->mediaProperties( e ) ;
		}
		void updateGetPlaylistCmdOptions( QStringList& e ) const
		{
			m_functions->updateGetPlaylistCmdOptions( e ) ;
		}
		bool parseOutput( Logger::Data& e,const QByteArray& s,int id,bool m ) const
		{
			return m_functions->parseOutput( e,s,id,m ) ;
		}
		bool foundNetworkUrl( const QString& s ) const
		{
			return m_functions->foundNetworkUrl( s ) ;
		}
		QStringList horizontalHeaderLabels() const
		{
			return m_functions->horizontalHeaderLabels() ;
		}
		void updateOutPutChannel( QProcess::ProcessChannel& s ) const
		{
			m_functions->updateOutPutChannel( s ) ;
		}
		std::vector< QStringList > mediaProperties( const QJsonArray& e ) const
		{
			return m_functions->mediaProperties( e ) ;
		}
		void updateEnginePaths( const Context& ctx,QString& filePath,QString& exeBinPath,QString& exeFolderPath ) const
		{
			m_functions->updateEnginePaths( ctx,filePath,exeBinPath,exeFolderPath ) ;
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
		const QJsonObject& controlStructure() const
		{
			return m_controlStructure ;
		}
		bool usingPrivateBackend() const
		{
			return m_usingPrivateBackend ;
		}
		bool valid() const
		{
			return m_valid ;
		}
		bool supportShowingComments() const
		{
			return m_functions->supportsShowingComments() ;
		}
		bool canDownloadPlaylist() const
		{
			return m_canDownloadPlaylist ;
		}
		bool likeYoutubeDl() const
		{
			return m_likeYoutubeDl ;
		}
		bool backendExists() const
		{
			return QFile::exists( m_exePath.realExe() ) ;
		}
		bool mainEngine() const
		{
			return m_mainEngine ;
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
		void updateOptions() ;

		void parseMultipleCmdArgs( Logger& logger,const engines& engines,int ) ;

		void parseMultipleCmdArgs( QStringList&,
					   const QString&,
					   Logger& logger,
					   const enginePaths& ePaths,
					   const engines& engines,
					   int ) ;

		mutable util::version m_version ;
		QJsonObject m_jsonObject ;
		std::unique_ptr< engines::engine::functions > m_functions ;
		int m_line ;
		int m_position ;
		bool m_valid ;
		bool m_usingPrivateBackend = false ;
		bool m_canDownloadPlaylist ;
		bool m_likeYoutubeDl ;
		bool m_mainEngine ;
		bool m_replaceOutputWithProgressReport ;
		mutable bool m_broken = false ;
		QString m_versionArgument ;
		QString m_name ;
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
		QStringList m_splitLinesBy ;
		QStringList m_removeText ;
		QStringList m_skiptLineWithText ;
		QStringList m_defaultDownLoadCmdOptions ;
		QStringList m_defaultListCmdOptions ;
		QStringList m_defaultCommentsCmdOptions ;
		QStringList m_defaultSubstitlesCmdOptions ;
		QStringList m_defaultSubtitleDownloadOptions ;

		QJsonObject m_controlStructure ;

		exeArgs m_exePath ;
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
	engines( Logger&,settings&,int ) ;
	void openUrls( tableWidget&,int row ) const ;
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
private:
	void updateEngines( bool,int ) ;
	Logger& m_logger ;
	settings& m_settings ;
	std::vector< engine > m_backends ;
	enginePaths m_enginePaths ;
	QProcessEnvironment m_processEnvironment ;

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

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

class engines{
public:
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
	private:
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
		QString dataPath( const QString& e ) const
		{
			return m_dataPath + "/" + e ;
		}
		QString binPath( const QString& e ) const
		{
			return m_binPath + "/" + e ;
		}
		QString enginePath( const QString& e ) const
		{
			return m_enginePath + "/" + e ;
		}
	private:
		QString m_binPath ;
		QString m_enginePath ;
		QString m_basePath ;
		QString m_dataPath ;
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
					return QFile::exists( m_exe ) ;
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
				preProcessing( const QString& ) ;
				static QString processingText() ;
				const QString& text() ;
			private:
				int m_counter = 0 ;
				QString m_counterDots ;
				QString m_txt ;
				QString m_processingDefaultText ;
			};

			class postProcessing
			{
			public:
				static QString processingText() ;
				postProcessing() ;
				postProcessing( const QString& ) ;

				const QString& text( const QString& ) ;
			private:
				int m_counter = 0 ;
				QString m_counterDots ;
				QString m_txt ;
				QString m_processingDefaultText ;
			};

			class filter{
			public:
				filter( const QString& quality,const engines::engine& engine ) ;
				virtual const QString& operator()( const Logger::Data& e ) ;
				virtual ~filter() ;
				const engines::engine& engine() const ;
			protected:
				const QString& quality() const ;
			private:
				engines::engine::functions::preProcessing m_processing ;
				QString m_quality ;
				const engines::engine& m_engine ;
			} ;

			class DataFilter{
			public:
				template< typename Type,typename ... Args >
				DataFilter( Type,Args&& ... args ) :
					m_filter( std::make_unique< typename Type::type >( std::forward< Args >( args ) ... ) )
				{
				}
				const QString& operator()( const Logger::Data& e )
				{
					return ( *m_filter )( e ) ;
				}
			private:
				std::unique_ptr< engines::engine::functions::filter > m_filter ;
			};

			virtual ~functions() ;

			virtual bool breakShowListIfContains( const QStringList& ) ;

			virtual DataFilter Filter( const QString& ) ;

			virtual void runCommandOnDownloadedFile( const QString&,const QString& ) ;

			virtual QString commandString( const engines::engine::exeArgs::cmd& ) ;

			QString updateTextOnCompleteDownlod( const QString& uiText,
							     const engine::engine::functions::finishedState& ) ;

			virtual QString updateTextOnCompleteDownlod( const QString& uiText,
								     const QString& bkText,
								     const engine::engine::functions::finishedState& ) ;

			virtual void sendCredentials( const QString&,QProcess& ) ;

			virtual void processData( Logger::Data&,const QByteArray&,int id ) ;

			virtual void processData( Logger::Data&,const QString&,int id ) ;

			struct updateOpts
			{
				const QString& quality ;
				const QStringList& userOptions ;
				const QString& indexAsString ;
				QStringList& urls ;
				QStringList& ourOptions ;
			};

			virtual void updateDownLoadCmdOptions( const engines::engine::functions::updateOpts& ) ;

			functions( settings&,const engines::engine& ) ;
			settings& Settings() const ;
			const engines::engine& engine() const ;
		private:
			settings& m_settings ;
			const engines::engine& m_engine ;
		} ;

		engine( Logger& l ) ;

		engine( const engines& engines,
			Logger& logger,
			const QString& name,
			const QString& versionArgument,
			int line,
			int position ) ;

		engine( Logger& logger,
			const enginePaths& ePaths,
			const util::Json& json,
			const engines& engines ) ;

		static QString mediaAlreadInArchiveText()
		{
			return QObject::tr( "Media Already In Archive" ) ;
		}

		const QString& name() const
		{
			return m_name ;
		}

		template< typename backend,typename ... Args >
		void setBackend( const engines& engines,Args&& ... args )
		{
			m_functions = std::make_unique< backend >( engines,
								   *this,
								   m_jsonObject,
								   std::forward< Args >( args ) ... ) ;
		}
		const QString& commandName() const ;

		bool breakShowListIfContains( const QStringList& e ) const ;

		const QString& versionArgument() const
		{
			return m_versionArgument ;
		}

		QString versionString( const QString& data ) const ;

		const QString& optionsArgument() const
		{
			return m_optionsArgument ;
		}
		const QString& downloadUrl() const
		{
			return m_downloadUrl ;
		}
		void processData( Logger::Data& outPut,const QByteArray& data,int id ) const
		{
			m_functions->processData( outPut,data,id ) ;
		}
		void processData( Logger::Data& outPut,const QString& data,int id ) const
		{
			m_functions->processData( outPut,data,id ) ;
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
		engines::engine::functions::DataFilter filter( const QString& quality ) const
		{
			return m_functions->Filter( quality ) ;
		}
		QString updateTextOnCompleteDownlod( const QString& uiText,
						     const QString& bkText,
						     const engine::engine::functions::finishedState& f ) const
		{
			return m_functions->updateTextOnCompleteDownlod( uiText,bkText,f ) ;
		}
		void updateDownLoadCmdOptions( const engines::engine::functions::updateOpts& u ) const
		{
			m_functions->updateDownLoadCmdOptions( u ) ;
		}
		void sendCredentials( const QString& credentials,QProcess& exe ) const
		{
			m_functions->sendCredentials( credentials,exe ) ;
		}
		const QStringList& defaultListCmdOptions() const
		{
			return m_defaultListCmdOptions ;
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
		const QStringList& playListIdArguments() const
		{
			return m_playListIdArguments ;
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
	private:
		QJsonObject m_jsonObject ;
		std::unique_ptr< engines::engine::functions > m_functions ;
		int m_line ;
		int m_position ;
		bool m_valid ;
		bool m_usingPrivateBackend ;
		bool m_canDownloadPlaylist ;
		bool m_likeYoutubeDl ;
		bool m_mainEngine ;
		bool m_replaceOutputWithProgressReport ;
		QString m_name ;
		QString m_commandName ;
		QString m_commandNameWindows ;
		QString m_userName ;
		QString m_password ;
		QString m_exeFolderPath ;
		QString m_versionArgument ;
		QString m_optionsArgument ;
		QString m_downloadUrl ;
		QString m_playListUrlPrefix ;
		QString m_playlistItemsArgument ;
		QString m_batchFileArgument ;
		QString m_cookieArgument ;
		QStringList m_playListIdArguments ;
		QStringList m_splitLinesBy ;
		QStringList m_removeText ;
		QStringList m_skiptLineWithText ;
		QStringList m_defaultDownLoadCmdOptions ;
		QStringList m_defaultListCmdOptions ;
		QJsonObject m_controlStructure ;

		exeArgs m_exePath ;
	};
	settings& Settings() const;
	QString findExecutable( const QString& exeName ) const ;
	const QProcessEnvironment& processEnvironment() const ;
	bool addEngine( const QByteArray& data,const QString& path ) ;
	void removeEngine( const QString& name ) ;
	QStringList enginesList() const ;
	const std::vector< engine >& getEngines() const ;
	const engine& defaultEngine( const QString& ) const ;
	util::result_ref< const engines::engine& > getEngineByName( const QString& name ) const ;
	const enginePaths& engineDirPaths() const ;
	engines( Logger&,settings& ) ;
	void openUrls( tableWidget&,int row,const QString& engineName = QString() ) const ;
	void openUrls( const QString& path ) const ;
private:
	void updateEngines( bool ) ;
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

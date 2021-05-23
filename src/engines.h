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

#include <vector>
#include <functional>
#include <memory>

#include "logger.h"

class settings ;
class concurrentDownloadManagerFinishedStatus ;

class engines{
public:
	template< typename T,std::enable_if_t< std::is_reference< T >::value,int > = 0 >
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

	class Json
	{
	public:
		Json( const QByteArray& data ) :
			m_doc( QJsonDocument::fromJson( data,&m_error ) )
		{
		}
		Json( const QJsonObject& obj ) :
			m_doc( obj )
		{
			m_error.error = QJsonParseError::NoError ;
		}
		Json( QJsonParseError error ) :
			m_error( std::move( error ) )
		{
		}
		Json( QJsonDocument doc ) : m_doc( std::move( doc ) )
		{
			m_error.error = QJsonParseError::NoError ;
		}
		const QJsonDocument& doc() const
		{
			return m_doc ;
		}
		QString errorString() const
		{
			return m_error.errorString() ;
		}
		operator bool() const
		{
			return m_error.error == QJsonParseError::NoError ;
		}
	private:
		QJsonParseError m_error ;
		QJsonDocument m_doc ;
	} ;

	class file
	{
	public:
	        file( const QString& path,Logger& logger ) :
		        m_filePath( path ),m_file( m_filePath ),m_logger( logger )
		{
		}
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
		const QString& configPath() const
		{
			return m_configPath ;
		}
		QString binPath( const QString& e ) const
		{
			return m_binPath + "/" + e ;
		}
		QString configPath( const QString& e ) const
		{
			return m_configPath + "/" + e ;
		}
	private:
		QString m_binPath ;
		QString m_configPath ;
		QString m_basePath ;
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
				cmd( const engines::engine::exeArgs& exeArgs,const QStringList& args ) :
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
			private:
				QStringList m_args ;
				QString m_exe ;
			} ;

			exeArgs()
			{
			}
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

		struct functions
		{
			static QString processCompleteStateText( const concurrentDownloadManagerFinishedStatus& ) ;

			class preProcessing
			{
			public:
				preProcessing() ;
				static QString processingText() ;
				const QString& text() ;
			private:
				int m_counter = 0 ;
				QString m_txt ;
			};

			class postProcessing
			{
			public:
				static QString processingText() ;
				postProcessing() ;
				const QString& text( const QString& ) ;
			private:
				int m_counter = 0 ;
				QString m_txt ;
				QString m_tmp ;
			};

			class filter{
			public:
				filter( const QString& quality ) ;
				virtual const QString& operator()( const engines::engine&,const Logger::Data& e ) ;
				virtual ~filter() ;
			protected:
				const QString& quality() ;
			private:
				engines::engine::functions::preProcessing m_processing ;
				QString m_quality ;
			} ;

			virtual ~functions() ;

			virtual std::unique_ptr< engines::engine::functions::filter > Filter( const QString& ) ;

			virtual void updateOptions( QJsonObject&,settings& ) ;

			virtual QString commandString( const engines::engine::exeArgs::cmd& ) ;

			virtual QString updateTextOnCompleteDownlod( const engines::engine&,
								     const QString& uiText,
								     const QString& bkText,
								     const concurrentDownloadManagerFinishedStatus& ) ;

		        virtual void sendCredentials( const engines::engine&,
		                                      const QString&,
						      QProcess& ) ;

			virtual void processData( const engines::engine& engine,
						  Logger::Data&,
						  QByteArray,
						  int id ) ;

			virtual void updateDownLoadCmdOptions( const engines::engine& engine,
							       const QString& quality,
							       const QStringList& userOptions,
		                                               QStringList& urls,
							       QStringList& ourOptions ) ;

		} ;

		engine() : m_valid( false )
		{
		}

		engine( const engines& engines,
			Logger& logger,
			const QString& name,
			const QString& versionArgument,
			int line,
			int position ) ;

		engine( Logger& logger,
			const enginePaths& ePaths,
		        const engines::Json& json,
			const engines& engines,
			std::unique_ptr< engines::engine::functions > ) ;

		const QString& name() const
		{
			return m_name ;
		}
		const QString& commandName() const ;
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
		void processData( Logger::Data& outPut,QByteArray data,int id ) const
		{
			m_functions->processData( *this,outPut,std::move( data ),id ) ;
		}
		QString commandString( const engines::engine::exeArgs::cmd& cmd ) const
		{
			return m_functions->commandString( cmd ) ;
		}
		const QStringList& defaultDownLoadCmdOptions() const
		{
			return m_defaultDownLoadCmdOptions ;
		}
		std::unique_ptr< engines::engine::functions::filter > filter( const QString& quality ) const
		{
			return m_functions->Filter( quality ) ;
		}
		QString updateTextOnCompleteDownlod( const QString& uiText,
						     const QString& bkText,
						     const concurrentDownloadManagerFinishedStatus& f ) const
		{
			return m_functions->updateTextOnCompleteDownlod( *this,uiText,bkText,f ) ;
		}
		void updateDownLoadCmdOptions( const QString& quality,
					       const QStringList& userOptions,
		                               QStringList& urls,
					       QStringList& ourOptions ) const
		{
		        m_functions->updateDownLoadCmdOptions( *this,quality,userOptions,urls,ourOptions ) ;
		}
		void sendCredentials( const QString& credentials,QProcess& exe ) const
		{
			m_functions->sendCredentials( *this,credentials,exe ) ;
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
		QStringList m_playListIdArguments ;
		QStringList m_splitLinesBy ;
		QStringList m_removeText ;
		QStringList m_skiptLineWithText ;
		QStringList m_defaultDownLoadCmdOptions ;
		QStringList m_defaultListCmdOptions ;
		QJsonObject m_controlStructure ;
		exeArgs m_exePath ;
	};
	QString findExecutable( const QString& exeName ) const ;
	const QProcessEnvironment& processEnvironment() const ;
	void addEngine( const QByteArray& data,const QString& path ) ;
	void removeEngine( const QString& name ) ;
	QStringList enginesList() const ;
	const std::vector< engine >& getEngines() const ;
	const engine& defaultEngine( const QString& ) const ;
	engines::result_ref< const engines::engine& > getEngineByName( const QString& name ) const ;
	engines::engine getEngineByPath( const QString& path ) const ;
	engines( Logger&,settings& ) ;
	void openUrls( QTableWidgetItem&,const QString& engineName = QString() ) const ;
	void openUrls( const QString& path ) const ;
private:
	void updateEngines( bool ) ;
	Logger& m_logger ;
	settings& m_settings ;
	std::vector< engine > m_backends ;
	enginePaths m_enginePaths ;
	QProcessEnvironment m_processEnvironment ;
};

#endif


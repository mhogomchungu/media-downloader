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

#include <vector>
#include <functional>

#include "utility.h"
#include "logger.h"

class engines{
public:
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
		enginePaths() ;

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
	private:
		QString m_binPath ;
		QString m_configPath ;
		QString m_basePath ;
	};

	void addEngine( const QByteArray& data,const QString& path ) ;
	void removeEngine( const QString& name ) ;
	QStringList enginesList() const ;

	class engine
	{
	public:
		class exeArgs
		{
		public:
			exeArgs()
			{
			}
			exeArgs( const QString& e ) : m_exe( e )
			{
			}
			exeArgs( const QString& e,const QStringList& s ) : m_exe( e ),m_options( s )
			{
			}
			bool isEmpty() const
			{
				return m_exe.isEmpty() ;
			}
			const QString& exe() const
			{
				return m_exe ;
			}
			const QStringList& args() const
			{
				return m_options ;
			}
		private:
			QString m_exe ;
			QStringList m_options ;
		} ;

		struct functions
		{
			virtual ~functions() ;

			virtual void processData( QStringList&,const QByteArray& ) = 0 ;
			virtual void updateDownLoadCmdOptions( const engines::engine& engine,
							       const QString& quality,
							       const QStringList& userOptions,
							       QStringList& ourOptions ) = 0 ;
		} ;

		engine() : m_valid( false )
		{
		}

		engine( const engines::Json& json,
			std::unique_ptr< engines::engine::functions > ) ;

		const QString& name() const
		{
			return m_name ;
		}
		const QString& commandName() const
		{
			return m_commandName ;
		}
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
		void processData( QStringList& outPut,const QByteArray& data ) const
		{
			m_functions->processData( outPut,data ) ;
		}
		const QStringList& defaultDownLoadCmdOptions() const
		{
			return m_defaultDownLoadCmdOptions ;
		}
		void updateDownLoadCmdOptions( const QString& quality,
					       const QStringList& userOptions,
					       QStringList& ourOptions ) const
		{
			m_functions->updateDownLoadCmdOptions( *this,quality,userOptions,ourOptions ) ;
		}
		const QStringList& defaultListCmdOptions() const
		{
			return m_defaultListCmdOptions ;
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
	private:
		QJsonObject m_jsonObject ;
		std::unique_ptr< engines::engine::functions > m_functions ;
		int m_line ;
		int m_position ;
		bool m_valid ;
		bool m_usingPrivateBackend ;
		bool m_canDownloadPlaylist ;
		bool m_likeYoutubeDl ;
		QString m_name ;
		QString m_commandName ;
		QString m_exeFolderPath ;
		QString m_versionArgument ;
		QString m_optionsArgument ;
		QString m_downloadUrl ;
		QString m_batchFileArgument ;
		QStringList m_defaultDownLoadCmdOptions ;
		QStringList m_defaultListCmdOptions ;
		exeArgs m_exePath ;
	};

	const std::vector< engine >& getEngines() const ;
	const engine& defaultEngine() const ;
	utility::result_ref< const engines::engine& > getEngineByName( const QString& name ) const ;
	void setDefaultEngine( const QString& name ) ;
	void setDefaultEngine( const engines::engine& engine ) ;
	engines( Logger&,settings& ) ;
private:
	void updateEngines() ;
	Logger& m_logger ;
	settings& m_settings ;
	std::vector< engine > m_backends ;
	enginePaths m_enginePaths ;
};

#endif


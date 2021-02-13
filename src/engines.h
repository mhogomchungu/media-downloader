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

class engines{
public:
	class log
	{
	public:
		log( QPlainTextEdit& e ) : m_textEdit( e )
		{
		}
		void add( const QString& s ) ;
	private:
		QPlainTextEdit& m_textEdit ;
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

	class engine
	{
	public:
		struct functions
		{
			std::function< void( QStringList&,const QByteArray& ) > processData ;

			std::function< void( const engines::engine& engine,
					     const QString& quality,
					     const QStringList& userOptions,
					     QStringList& ourOptions ) > updateDownLoadCmdOptions ;
		};

		engine() : m_valid( false )
		{
		}

		engine( const QJsonDocument& json,engine::functions ) ;

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
		const QString& downloadPath() const
		{
			return m_downloadPath ;
		}
		void processData( QStringList& outPut,const QByteArray& data ) const
		{
			m_functions.processData( outPut,data ) ;
		}
		const QStringList& defaultDownLoadCmdOptions() const
		{
			return m_defaultDownLoadCmdOptions ;
		}
		void updateDownLoadCmdOptions( const QString& quality,
					       const QStringList& userOptions,
					       QStringList& ourOptions ) const
		{
			m_functions.updateDownLoadCmdOptions( *this,quality,userOptions,ourOptions ) ;
		}
		const QStringList& defaultListCmdOptions() const
		{
			return m_defaultListCmdOptions ;
		}
		const QString& exePath() const
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
	private:
		QJsonObject m_jsonObject ;
		functions m_functions ;
		int m_line ;
		int m_position ;
		bool m_valid ;
		bool m_usingPrivateBackend ;
		bool m_canDownloadPlaylist ;
		QString m_name ;
		QString m_commandName ;
		QString m_exeFolderPath ;
		QString m_exePath ;
		QString m_versionArgument ;
		QString m_optionsArgument ;
		QString m_downloadPath ;
		QString m_batchFileArgument ;
		QStringList m_defaultDownLoadCmdOptions ;
		QStringList m_defaultListCmdOptions ;
	};

	const std::vector< engine >& getEngines() ;
	const engine& defaultEngine() ;
	const engine& getEngineByName( const QString& name ) ;
	void setDefaultEngine( const QString& name ) ;
	engines( QPlainTextEdit& ) ;
private:
	log m_log ;
	QString m_defaultEngine ;
	std::vector< engine > m_backends ;
};

#endif


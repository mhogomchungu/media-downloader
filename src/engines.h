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

#include <vector>
#include <functional>

class engines{
public:
	class engine
	{
	public:
		struct functions
		{
			std::function< void( QStringList&,const QByteArray& ) > processData ;

			std::function< bool( const QString& ) > hasSupport ;
			std::function< void( const engines::engine& engine,
					     const QString& quality,
					     const QStringList& userOptions,
					     QStringList& ourOptions ) > updateDownLoadCmdOptions ;
		};
		engine() : m_valid( false )
		{
		}
		engine( functions f,
			int line,
			int position,
			bool usingPrivateBackend,
			const QString& name,
			const QString& exeFolderPath,
			const QString& versionArgument,
			const QString& optionsArgument,
			const QString& dp,
			const QStringList& defaultDownLoadCmdOptions,
			const QStringList& defaultListCmdOptions ) :
			m_functions( std::move( f ) ),
			m_line( line ),
			m_position( position ),
			m_valid( true ),
			m_usingPrivateBackend( usingPrivateBackend ),
			m_name( name ),
			m_exeFolderPath( exeFolderPath ),
			m_versionArgument( versionArgument ),
			m_optionsArgument( optionsArgument ),
			m_downloadPath( dp ),
			m_defaultDownLoadCmdOptions( defaultDownLoadCmdOptions ),
			m_defaultListCmdOptions( defaultListCmdOptions )
		{
			if( this->usingPrivateBackend() ){

				m_exePath = m_exeFolderPath + "/" + m_name ;
			}else{
				m_exePath = QStandardPaths::findExecutable( m_name ) ;
			}
		}
		const QString& name() const
		{
			return m_name ;
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
		bool hasSupport( const QString& url ) const
		{
			return m_functions.hasSupport( url ) ;
		}
		const QStringList& defaultListCmdOptions() const
		{
			return m_defaultListCmdOptions ;
		}
		const QString& exePath() const
		{
			return m_exePath ;
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
	private:
		functions m_functions ;
		int m_line ;
		int m_position ;
		bool m_valid ;
		bool m_usingPrivateBackend ;
		QString m_name ;
		QString m_exeFolderPath ;
		QString m_exePath ;
		QString m_versionArgument ;
		QString m_optionsArgument ;
		QString m_downloadPath ;
		QStringList m_defaultDownLoadCmdOptions ;
		QStringList m_defaultListCmdOptions ;
	};

	const std::vector< engine >& getEngines() ;
	const engine& defaultEngine() ;
	const engine& getEngine( const QString& url ) ;

	engines() ;
private:
	std::vector< engine > m_backends ;
};

#endif


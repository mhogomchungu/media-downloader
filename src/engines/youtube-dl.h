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

#include <QByteArray>

#include "../engines.h"

class youtube_dl : public engines::engine::functions
{
public:
	~youtube_dl() override ;

	void updateOptions( QJsonObject&,settings& ) override ;

	class youtube_dlFilter : public engines::engine::functions::filter
	{
	public:
		youtube_dlFilter( const QString& ) ;

		const QString& operator()( const engines::engine&,const Logger::Data& e ) override ;

		virtual ~youtube_dlFilter() override ;
	private:
		engines::engine::functions::preProcessing m_preProcessing ;
		engines::engine::functions::postProcessing m_postProcessing ;
		QString m_tmp ;
		QString m_fileName ;
	} ;

	std::unique_ptr< engines::engine::functions::filter > Filter( const QString& ) override ;

	QString updateTextOnCompleteDownlod( const engines::engine& engine,
					     const QString& uiText,
					     const QString& bkText,
					     const concurrentDownloadManagerFinishedStatus& ) override ;

	void updateDownLoadCmdOptions( const engines::engine& engine,
				       const QString& quality,
				       const QStringList& userOptions,
				       QStringList& urls,
				       QStringList& ourOptions ) override ;

	static void init( Logger& logger,const engines::enginePaths& enginePath ) ;
	youtube_dl() ;
	static QJsonObject defaultControlStructure() ;
private:
};

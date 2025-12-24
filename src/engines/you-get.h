/*
 *
 *  Copyright (c) 2022
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

class settings ;

class you_get : public engines::engine::baseEngine
{
public:
	~you_get() override ;
	you_get( const engines&,const engines::engine&,QJsonObject& ) ;

	class you_getFilter : public engines::engine::baseEngine::filter
	{
	public:
		you_getFilter( settings&,const engines::engine&,int ) ;

		const QByteArray& operator()( Logger::Data& e ) override ;

		~you_getFilter() override ;
	private:
		QByteArray m_title ;
		QByteArray m_tmp ;
		engines::engine::baseEngine::preProcessing m_preProcessing ;
		int m_processId ;
	} ;

	QString updateCmdPath( const QString& ) override ;

	bool foundNetworkUrl( const QString& ) override ;

	engines::engine::baseEngine::optionsEnvironment setProxySetting( QStringList&,const QString& ) override ;

	renameArchiveFolderStatus renameArchiveFolder( const QString&,const QString& ) override ;

	std::vector< engines::engine::baseEngine::mediaInfo > mediaProperties( Logger&,const QByteArray& ) override ;

	engines::engine::baseEngine::DataFilter Filter( int ) override ;

	QString updateTextOnCompleteDownlod( const QString& uiText,
					     const QString& bkText,
					     const QString& downloadingOptions,
					     const QString& tabName,
					     const engines::engine::baseEngine::finishedState& ) override ;
private:
	QString archiveExtension()
	{
		return ".tar.gz" ;
	}
};

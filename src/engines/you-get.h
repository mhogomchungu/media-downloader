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

class you_get : public engines::engine::functions
{
public:
	~you_get() override ;
	you_get( const engines&,const engines::engine&,QJsonObject& ) ;

	class you_getFilter : public engines::engine::functions::filter
	{
	public:
		you_getFilter( settings&,const engines::engine&,int ) ;

		const QByteArray& operator()( const Logger::Data& e ) override ;

		~you_getFilter() override ;
	private:
		QByteArray m_title ;
		QByteArray m_tmp ;
		engines::engine::functions::preProcessing m_preProcessing ;
		int m_processId ;
	} ;

	QString updateCmdPath( const QString& ) override ;

	bool foundNetworkUrl( const QString& ) override ;

	void renameArchiveFolder( const QString& ) override ;

	std::vector< engines::engine::functions::mediaInfo > mediaProperties( Logger&,const QByteArray& ) override ;

	engines::engine::functions::DataFilter Filter( int ) override ;

	QString updateTextOnCompleteDownlod( const QString& uiText,
					     const QString& bkText,
					     const QString& downloadingOptions,
					     const engines::engine::functions::finishedState& ) override ;
private:
};

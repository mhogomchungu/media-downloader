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

#include "../engines.h"

class aria2c : public engines::engine::functions
{
public:
	static void init( const QString& name,
			  const QString& configFileName,
			  Logger& logger,
			  const engines::enginePaths& enginePath ) ;

	static void trimProgressLine( QString& ) ;

	~aria2c() override ;

	aria2c( const engines&,const engines::engine&,QJsonObject& ) ;

	class aria2c_dlFilter : public engines::engine::functions::filter
	{
	public:
		aria2c_dlFilter( const QString&,settings&,const engines::engine& ) ;

		const QString& operator()( const Logger::Data& e ) override ;

		virtual ~aria2c_dlFilter() override ;
	private:
		engines::engine::functions::preProcessing m_preProcessing ;
		QString m_tmp ;
		QString m_fileName ;
	} ;

	engines::engine::functions::DataFilter Filter( const QString& ) override ;

	QString updateTextOnCompleteDownlod( const QString& uiText,
					     const QString& bkText,
					     const engines::engine::functions::finishedState& ) override ;

	void updateDownLoadCmdOptions( const engines::engine::functions::updateOpts& ) override ;
private:
	const engines& m_engines ;
};

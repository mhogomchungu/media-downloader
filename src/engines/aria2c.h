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

class aria2c : public engines::engine::baseEngine
{
public:
	static void init( const QString& name,
			  const QString& configFileName,
			  Logger& logger,
			  const engines::enginePaths& enginePath ) ;

	static QByteArray& trimProgressLine( QByteArray& ) ;

	static bool meetCondition( const engines::engine&,const QByteArray& e ) ;

	~aria2c() override ;

	aria2c( const engines&,const engines::engine&,QJsonObject& ) ;

	class aria2c_dlFilter : public engines::engine::baseEngine::filter
	{
	public:
		aria2c_dlFilter( settings&,const engines::engine&,int ) ;

		const QByteArray& operator()( Logger::Data& e ) override ;

		~aria2c_dlFilter() override ;
	private:
		engines::engine::baseEngine::preProcessing m_preProcessing ;
		QByteArray m_tmp ;
		QByteArray m_fileName ;
		int m_processId ;
	} ;

	engines::engine::baseEngine::FilterOutPut filterOutput( int ) override ;

	engines::engine::baseEngine::DataFilter Filter( int ) override ;

	QString updateTextOnCompleteDownlod( const QString& uiText,
					     const QString& bkText,
					     const QString& downloadingOptions,
					     const QString& tabName,
					     const engines::engine::baseEngine::finishedState& ) override ;

	void updateDownLoadCmdOptions( const engines::engine::baseEngine::updateOpts&,
				       bool,
				       const QStringList& ) override ;
private:
	const engines& m_engines ;
};

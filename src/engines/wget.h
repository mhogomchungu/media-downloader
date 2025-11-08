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

class wget : public engines::engine::baseEngine
{
public:
	static const char * testData() ;
	static void init( const QString& name,
			  const QString& configFileName,
			  Logger& logger,
			  const engines::enginePaths& enginePath ) ;
	~wget() override ;
	wget( const engines& e,const engines::engine& s,QJsonObject& ) ;
	void updateDownLoadCmdOptions( const engines::engine::baseEngine::updateOpts&,
				       bool,
				       const QStringList& ) override ;

	engines::engine::baseEngine::DataFilter Filter( int ) override ;

	engines::engine::baseEngine::optionsEnvironment setProxySetting( QStringList&,const QString& ) override ;

	QString updateTextOnCompleteDownlod( const QString& uiText,
					     const QString& bkText,
					     const QString& downloadingOptions,
					     const QString& tabName,
					     const engines::engine::baseEngine::finishedState& ) override ;

	class wgetFilter : public engines::engine::baseEngine::filter
	{
	public:
		wgetFilter( const engines::engine&,int ) ;

		const QByteArray& operator()( Logger::Data& e ) override ;

		~wgetFilter() override ;
	private:
		QByteArray m_title ;
		QByteArray m_length ;
		QByteArray m_tmp ;
		engines::engine::baseEngine::preProcessing m_preProcessing ;
	} ;
private:
};

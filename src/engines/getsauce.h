/*
 *
 *  Copyright (c) 2025
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

class getsauce : public engines::engine::baseEngine
{
public:
	static const char * testData() ;

	class getsauce_dlFilter : public engines::engine::baseEngine::filter
	{
	public:
		getsauce_dlFilter( const engines::engine&,int,QByteArray ) ;

		const QByteArray& operator()( Logger::Data& e ) override ;

		~getsauce_dlFilter() override ;
	private:
		bool progressLine( const Logger::Data& e ) ;
		QByteArray m_banner ;
		QByteArray m_tmp ;
		engines::engine::baseEngine::preProcessing m_progress ;
		QByteArray m_downloadFolder ;
	} ;
	~getsauce() override ;
	getsauce( const engines& e,const engines::engine& s,QJsonObject& ) ;

	engines::engine::baseEngine::optionsEnvironment setProxySetting( QStringList&,const QString& ) override ;

	QString updateTextOnCompleteDownlod( const QString& uiText,
					     const QString& bkText,
					     const QString& downloadingOptions,
					     const QString& tabName,
					     const engines::engine::baseEngine::finishedState& ) override ;

	bool skipCondition( const QByteArray& ) override ;

	engines::engine::baseEngine::DataFilter Filter( int ) override ;
	engines::engine::baseEngine::FilterOutPut filterOutput( int ) override ;

	std::vector< engines::engine::baseEngine::mediaInfo > mediaProperties( Logger&,const QByteArray& ) override ;

	std::vector< engines::engine::baseEngine::mediaInfo > mediaProperties( Logger&,const QJsonArray& ) override ;

	bool foundNetworkUrl( const QString& s ) override ;
private:
	const engines::engine& m_engine ;
	QString m_downloadFolder ;
} ;

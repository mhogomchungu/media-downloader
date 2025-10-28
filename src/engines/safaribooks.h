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
#include "../settings.h"

class safaribooks : public engines::engine::baseEngine
{
public:
	static const char * testData() ;

	safaribooks( const engines&,const engines::engine&,QJsonObject& ) ;
	~safaribooks() override ;
	QString commandString( const engines::engine::exeArgs::cmd& ) override ;
	void sendCredentials( const QString&,QProcess& ) override ;
	QString setCredentials( QStringList& e,QStringList& s ) override ;
	void updateDownLoadCmdOptions( const engines::engine::baseEngine::updateOpts&,
				       bool,
				       const QStringList& ) override ;
private:
	const engines::engine& m_engine ;
};

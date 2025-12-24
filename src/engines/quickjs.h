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

#include "../engines.h"

class quickjs : public engines::engine::baseEngine
{
public:
	static QJsonObject init( const QString& name,
				 const QString& configFileName,
				 Logger& logger,
				 const engines::enginePaths& enginePath ) ;
	~quickjs() ;
	engines::metadata parseJsonDataFromGitHub( const QJsonDocument& ) override ;
	engines::engine::baseEngine::removeFilesStatus removeFiles( const QStringList&,const QString& ) override ;
	bool foundNetworkUrl( const QString& s ) override ;
	QString parseVersionInfo( const utils::qprocess::outPut& ) override ;
	quickjs( const engines&,const engines::engine&,QJsonObject& ) ;
private:
};

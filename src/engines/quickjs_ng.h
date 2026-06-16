/*
 *
 *  Copyright (c) 2026
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
#include "../utils/miscellaneous.hpp"

class quickjs_ng : public engines::engine::baseEngine
{
public:
	static void init( Logger& logger,const engines::enginePaths& enginePath ) ;
	static void remove( Logger& logger,const engines::enginePaths& enginePath ) ;
	QString parseVersionInfo( const utils::qprocess::outPut& ) override ;

	class nameAndExe
	{
	public:
		nameAndExe( const utils::misc::string& e ) : m_name( "quickjs" ),m_exe( e )
		{
		}
		const utils::misc::string& name() const
		{
			return m_name ;
		}
		const utils::misc::string& exe() const
		{
			return m_exe ;
		}
	private:
		utils::misc::string m_name ;
		utils::misc::string m_exe ;
	} ;
	static nameAndExe getNameAndExe() ;
	~quickjs_ng() ;
	quickjs_ng( const engines&,const engines::engine&,QJsonObject& ) ;
};

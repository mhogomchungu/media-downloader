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

#ifndef WINDOWS_H
#define WINDOWS_H

#include <QString>
#include "context.hpp"
#include "utility.h"

namespace windows
{
	class OsVersion
	{
	public:
		template< typename T >
		OsVersion( const T& t ) :
			m_major( t.majorVersion() ),
			m_minor( t.minorVersion() ),
			m_micro( t.microVersion() )
		{
		}
		OsVersion()
		{
		}
		int majorVersion() const
		{
			return m_major ;
		}
		int minorVersion() const
		{
			return m_minor ;
		}
		int microVersion() const
		{
			return m_micro ;
		}
	private:
		int m_major = 0 ;
		int m_minor = 0 ;
		int m_micro = 0 ;
	};

	windows::OsVersion OperatingSystemVersion() ;
	QString applicationDirPath() ;
	QString gateWayAddress() ;
	QString getClipBoardText( const ContextWinId& wId ) ;
	void setDarkModeTitleBar( const Context& ctx ) ;
	std::vector< utility::PlayerOpts > getMediaPlayers() ;
	QString errorMessage() ;
	void enableCheckPermissions() ;
	void disableCheckPermissions() ;
}

#endif
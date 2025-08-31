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

#ifndef REPORT_FINISHED_H
#define REPORT_FINISHED_H

#include "engines.h"
#include "tableWidget.h"

class reportFinished
{
public:
	class finishedStatus
	{
	public:
		static QString notStarted()
		{
			return "Not Started" ;
		}
		static QString running()
		{
			return "Running" ;
		}
		static QString finishedCancelled()
		{
			return "FinishedCancelled" ;
		}
		static QString finishedWithError()
		{
			return "FinishedWithError" ;
		}
		static QString finishedWithSuccess()
		{
			return "FinishedWithSuccess" ;
		}
		static bool notStarted( const QString& e )
		{
			return notStarted() == e ;
		}
		static bool running( const QString& e )
		{
			return running() == e ;
		}
		static bool finishedCancelled( const QString& e )
		{
			return finishedCancelled() == e ;
		}
		static bool finishedWithError( const QString& e )
		{
			return finishedWithError() == e ;
		}
		static bool finishedWithSuccess( const QString& e )
		{
			return finishedWithSuccess() == e  ;
		}
		static bool finishedCancelled( const tableWidget& e,int row )
		{
			return finishedCancelled( e.runningState( row ) ) ;
		}
		static bool finishedWithError( const tableWidget& e,int row )
		{
			return finishedWithError( e.runningState( row ) ) ;
		}
		static bool finishedWithSuccess( const tableWidget& e,int row )
		{
			return finishedWithSuccess( e.runningState( row ) ) ;
		}
		static bool isRunning( const tableWidget& e,int row )
		{
			return running( e.runningState( row ) ) ;
		}
		QString setState() const
		{
			if( this->exitState().cancelled()){

				return finishedCancelled() ;

			}else if( this->exitState().success() ){

				return finishedWithSuccess() ;
			}else{
				return finishedWithError() ;
			}
		}
		int index() const
		{
			return m_index ;
		}
		enum class state{ cancelled,done,running } ;
		bool continuing() const
		{
			return m_state == state::running ;
		}
		bool cancelled() const
		{
			return m_state == state::cancelled ;
		}
		bool done() const
		{
			return m_state == state::done ;
		}
		finishedStatus move()
		{
			return std::move( *this ) ;
		}
		const engines::ProcessExitState& exitState() const
		{
			return m_exitState ;
		}
		finishedStatus( int i,state s,engines::ProcessExitState e ) :
			m_index( i ),
			m_state( s ),
			m_exitState( std::move( e ) )
		{
		}
		finishedStatus( state s,engines::ProcessExitState e ) :
			m_index( 0 ),
			m_state( s ),
			m_exitState( std::move( e ) )
		{
		}
		finishedStatus()
		{
		}
	private:
		int m_index ;
		state m_state ;
		engines::ProcessExitState m_exitState ;
	} ;
	reportFinished()
	{
	}
	reportFinished( const engines::engine& engine,reportFinished::finishedStatus status ) :
		m_engine( &engine ),
		m_status( std::move( status ) )
	{
	}
	const engines::engine& engine() const
	{
		return *m_engine ;
	}
	const reportFinished::finishedStatus& status() const
	{
		return m_status ;
	}
	reportFinished move()
	{
		return std::move( *this ) ;
	}
private:
	const engines::engine * m_engine = nullptr ;
	reportFinished::finishedStatus m_status ;
};

Q_DECLARE_METATYPE( reportFinished )

#endif

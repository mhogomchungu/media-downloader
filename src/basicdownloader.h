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

#ifndef BASIC_DOWNLOADER_H
#define BASIC_DOWNLOADER_H

#include <QString>
#include <QStringList>
#include <QMenu>

#include "settings.h"
#include "utility.h"
#include "context.hpp"

class basicdownloader : public QObject
{
	Q_OBJECT
public:
	basicdownloader( const Context& ) ;
	void init_done() ;
	void updateEngines() ;
	void downloadDefaultEngine() ;
	void tabEntered() ;
	void tabExited() ;
	void enableQuit() ;
	void enableAll() ;
	void disableAll() ;
	void appQuit() ;
	void resetMenu( const QStringList& = {} ) ;
	void retranslateUi() ;
	void checkAndPrintInstalledVersion( const engines::engine& ) ;
	void printEngineVersionInfo() ;
	basicdownloader& setAsActive() ;
private:
	class options
	{
	public:
		options( QPushButton& p,
			 const Context& ctx,
			 const engines::engine& engine,
			 QTableWidget& table,
			 bool d,
			 bool l ) :
			m_button( p ),
			m_ctx( ctx ),
			m_engine( engine ),
			m_table( table ),
			m_debug( d ),
			m_listRequested( l )
		{
		}

		void done( utility::ProcessExitState ) ;

		basicdownloader::options& tabManagerEnableAll( bool e ) ;
		basicdownloader::options& listRequested( const QList< QByteArray >& e ) ;

		basicdownloader::options& enableCancel( bool e )
		{
			m_button.setEnabled( e ) ;

			return *this ;
		}
		bool listRequested()
		{
			return m_listRequested ;
		}
		bool debug()
		{
			return m_debug ;
		}
		QString downloadFolder() const
		{
			return m_ctx.Settings().downloadFolder() ;
		}
		const QProcessEnvironment& processEnvironment() const
		{
			return m_ctx.Engines().processEnvironment() ;
		}
	private:
		QPushButton& m_button ;
		const Context& m_ctx ;
		const engines::engine& m_engine ;
		QTableWidget& m_table ;
		bool m_debug ;
		bool m_listRequested ;
	} ;

	size_t m_counter = 0 ;
	const Context& m_ctx ;
	settings& m_settings ;
	bool m_debug ;
	Ui::MainWindow& m_ui ;
	tabManager& m_tabManager ;
	QStringList m_optionsList ;
	QTableWidget m_bogusTable ;

	void setDefaultEngine() ;

	void run( const engines::engine& engine,
		  const QStringList& args,
		  const QString& quality,
		  bool list_requested ) ;

	void tabManagerEnableAll( bool ) ;
	void listRequested( const QList< QByteArray >& ) ;
	void list() ;
	void download( const engines::engine&,
		       const utility::args&,
		       const QString& urls,
		       bool = true ) ;
	void download( const engines::engine&,
		       const utility::args&,
		       const QStringList& urls,
		       bool = true ) ;
	void download( const QString& ) ;
	void exit() ;
	void printEngineVersionInfo( const engines::engine& ) ;
} ;

#endif

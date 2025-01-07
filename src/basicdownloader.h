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
#include "tableWidget.h"
#include "networkAccess.h"

class basicdownloader : public QObject
{
	Q_OBJECT
public:
	basicdownloader( const Context& ) ;
	void keyPressed( utility::mainWindowKeyCombo ) ;
	void init_done() ;
	void tabEntered() ;
	void tabExited() ;
	void enableQuit() ;
	void enableAll() ;
	void disableAll() ;
	void exiting() ;
	void textAlignmentChanged( Qt::LayoutDirection ) ;
	void gotEvent( const QJsonObject& e ) ;
	void resetMenu( const QStringList& = {} ) ;
	void retranslateUi() ;
	void updateEnginesList( const QStringList& ) ;
	void clipboardData( const QString& ) ;

	QStringList enginesList() ;
	basicdownloader& setAsActive() ;
	basicdownloader& hideTableList() ;
private:
	QString defaultEngineName() ;
	struct engine
	{
		const engines::engine& engine ;
		int id ;
	};

	basicdownloader::engine defaultEngine() ;

	const Context& m_ctx ;
	settings& m_settings ;
	Ui::MainWindow& m_ui ;
	tabManager& m_tabManager ;
	tableMiniWidget< engines::engine::baseEngine::mediaInfo,5 > m_tableList ;
	QStringList m_optionsList ;
	QTableWidget m_bogusTableOriginal ;
	tableWidget m_hiddenTable ;
	utility::downLoadOptions m_extraOptions ;
	utility::Terminator m_terminator ;

	void run( const basicdownloader::engine& engine,
		  const QStringList& args,
		  const QString& credentials,
		  bool list_requested ) ;

	void changeDefaultEngine( int index ) ;
	void listRequested( const QByteArray&,int ) ;
	void list() ;
	void download( const basicdownloader::engine&,
		       const utility::args&,
		       const QString& urls,
		       bool = true ) ;
	void download( const basicdownloader::engine&,
		       const utility::args&,
		       const QStringList& urls,
		       bool = true ) ;
	void download( const QString& ) ;
	void exit() ;
} ;

#endif

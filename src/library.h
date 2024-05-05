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

#ifndef LIBRARY_H
#define LIBRARY_H

#include "context.hpp"
#include <QString>
#include <QStringList>
#include <QDir>

#include "settings.h"
#include "utility.h"
#include "tableWidget.h"
#include "directoryEntries.h"

class tabManager ;

#include <QObject>

class library : public QObject
{
	Q_OBJECT
public:
	library( const Context& ) ;
	void init_done() ;
	void enableAll() ;
	void disableAll() ;
	void resetMenu() ;
	void exiting() ;
	void retranslateUi() ;
	void tabEntered() ;
	void tabExited() ;
	void textAlignmentChanged( Qt::LayoutDirection ) ;
private slots:
	void addEntry( const directoryEntries::iter& ) ;
private:
	void arrangeAndShow() ;
	void arrangeEntries( int ) ;
	void enableAll( bool ) ;
	void disableAll( bool ) ;
	void internalEnableAll() ;
	void internalDisableAll() ;
	void showContents( const QString&,bool disableUi = true ) ;
	void moveUp() ;
	enum class ICON{ FILE,FOLDER } ;
	void addItem( const QString& text,library::ICON ) ;
	const Context& m_ctx ;
	bool m_enableGlobalUiChanges ;
	settings& m_settings ;
	bool m_enabled ;
	bool m_disableUi ;
	std::atomic_bool m_continue ;
	Ui::MainWindow& m_ui ;
	tableMiniWidget< ICON,2 > m_table ;
	QString m_downloadFolder ;
	QString m_currentPath ;
	QPixmap m_folderIcon ;
	QPixmap m_videoIcon ;
	directoryEntries m_directoryEntries ;
};

#endif

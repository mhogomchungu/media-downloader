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
#ifndef BATCH_DOWNLOADER_URL_H
#define BATCH_DOWNLOADER_URL_H

#include <QString>
#include <QStringList>
#include <QMenu>

#include "settings.h"
#include "utility.h"
#include "context.hpp"
#include "downloadmanager.h"
#include "tableWidget.h"

class tabManager ;

class batchdownloader : public QObject
{
	Q_OBJECT
public:
	template< typename Function >
	void setUpdefaultEngine( QComboBox& comboBox,
				 const QString& defaultEngine,
				 Function function )
	{
		for( int s = 0 ; s < comboBox.count() ; s++ ){

			if( comboBox.itemText( s ) == defaultEngine ){

				comboBox.setCurrentIndex( s ) ;

				return ;
			}
		}

		if( comboBox.count() > 0 ){

			comboBox.setCurrentIndex( 0 ) ;
			function( comboBox.itemText( 0 ) ) ;
		}
	}

	batchdownloader( const Context& ) ;
	void init_done() ;
	void enableAll() ;
	void disableAll() ;
	void resetMenu() ;
	void retranslateUi() ;
	void tabEntered() ;
	void tabExited() ;
	void updateEnginesList( const QStringList& ) ;
	void download( const engines::engine&,
		       const QString& opts,
		       const QStringList&,
		       bool doNotGetTitle ) ;
private:
	void clearScreen() ;
	void showList() ;
	void addToList( const QString&,bool ) ;
	void download( const engines::engine&,downloadManager::index ) ;
	void download( const engines::engine& ) ;
	void download( const engines::engine&,int ) ;
	void addItem( const QString& url,const QString& thumbnail ) ;

	const Context& m_ctx ;
	settings& m_settings ;
	Ui::MainWindow& m_ui ;
	QWidget& m_mainWindow ;
	tabManager& m_tabManager ;
	bool m_showThumbnails ;
	tableWidget m_table ;
	tableWidget m_tableWidgetBDList ;
	bool m_running ;
	bool m_debug ;
	int m_networkRunning = false ;

	QStringList m_optionsList ;
	QLineEdit m_lineEdit ;

	utility::Terminator m_terminator ;

	downloadManager m_ccmd ;

	struct opts
	{
		const Context& ctx ;
		bool debug ;
		bool listRequested ;
		int index ;
	} ;

	template< typename Functions >

	auto make_options( batchdownloader::opts opts,Functions f )
	{
		return utility::options< batchdownloader::opts,Functions >( std::move( opts ),std::move( f ) ) ;
	}
};

#endif

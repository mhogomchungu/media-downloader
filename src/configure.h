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
#ifndef CONFIGURE_H
#define CONFIGURE_H

#include "settings.h"
#include "utility.h"
#include "context.hpp"
#include "networkAccess.h"
#include "tableWidget.h"

#include <QMenu>

class tabManager ;

class configure : public QObject
{
        Q_OBJECT
public:
	configure( const Context& ) ;
	void init_done() ;
	void enableAll() ;
	void disableAll() ;
	void resetMenu() ;
	void retranslateUi() ;
	void downloadFromGitHub( const engines::Iterator& ) ;
	void tabEntered() ;
	void tabExited() ;
	void updateEnginesList( const QStringList& e ) ;
	template< typename Function >
	void presetOptionsForEach( const Function& function )
	{
		m_presetOptions.forEach( function ) ;
	}
private:
	class presetOptions
	{
	public:
		presetOptions( const Context&,settings& ) ;
		~presetOptions() ;
		void clear() ;
		void setDefaults() ;
		void add( const QString& uiName,const QString& options ) ;
		template< typename Function >
		void forEach( const Function& function )
		{
			for( const auto& it : util::asConst( m_array ) ){

				auto obj = it.toObject() ;

				if( !obj.isEmpty() ){

					auto a = obj.value( "uiName" ).toString() ;
					auto b = obj.value( "options" ).toString() ;

					if( !a.isEmpty() && !b.isEmpty() ){

						function( a,b ) ;
					}
				}
			}
		}
	private:
		QByteArray defaultData() ;
		QString m_path ;
		QJsonArray m_array ;
	};
	void saveOptions() ;
	void setEngineOptions( const QString& ) ;
	void savePresetOptions() ;
	void showOptions() ;

	const Context& m_ctx ;
	settings& m_settings ;
	Ui::MainWindow& m_ui ;
	QWidget& m_mainWindow ;
	tabManager& m_tabManager ;
	engines& m_engines ;
	tableMiniWidget< int > m_tablePresetOptions ;
	QMenu m_menu ;
	presetOptions m_presetOptions ;
};

#endif


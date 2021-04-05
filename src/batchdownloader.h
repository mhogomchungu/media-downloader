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

class tabManager ;

class batchdownloader : public QObject
{
	Q_OBJECT
public:
	batchdownloader( const Context& ) ;
	void init_done() ;
	void enableAll() ;
	void disableAll() ;
	void resetMenu() ;
	void retranslateUi() ;
	void tabEntered() ;
private slots:
	void monitorForFinished() ;
private:
	void download() ;
	const Context& m_ctx ;
	settings& m_settings ;
	Ui::MainWindow& m_ui ;
	QWidget& m_mainWindow ;
	tabManager& m_tabManager ;
	int counter ;
	bool m_running ;
	bool m_cancelled ;
	bool m_debug ;

	QStringList m_downloadList ;

	template< typename Function >
	class options
	{
	public:
		options( QPushButton& p,const Context& ctx,bool d,Function function ) :
			m_button( p ),
			m_ctx( ctx ),
			m_debug( d ),
			m_done( std::move( function ) )
		{
		}
		void done()
		{
			m_done() ;
		}
		options& tabManagerEnableAll( bool )
		{
			return *this ;
		}
		options& listRequested( const QList< QByteArray >& )
		{
			return *this ;
		}
		options& enableCancel( bool e )
		{
			Q_UNUSED( e )
			//m_button.setEnabled( e ) ;

			return *this ;
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
		bool m_debug ;
		Function m_done ;
	} ;

	template< typename Function >
	auto make_options( QPushButton& p,const Context& ctx,bool d,Function function )
	{
		return batchdownloader::options< Function >( p,ctx,d,std::move( function ) ) ;
	}
};

#endif

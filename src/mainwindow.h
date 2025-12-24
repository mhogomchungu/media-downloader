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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCloseEvent>
#include <QSystemTrayIcon>
#include <QString>
#include <QStringList>
#include <QMenu>
#include <QShortcut>

#include "tabmanager.h"
#include "engines.h"
#include "logger.h"
#include "logwindow.h"

#include <QApplication>

namespace Ui
{
	class MainWindow ;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT
public:
	MainWindow( QApplication&,settings&,translator&,const engines::enginePaths&,const utility::cliArguments& ) ;
	void retranslateUi() ;
	void setTitle( const QString& m ) ;
	void resetTitle() ;
	void Show() ;
	void processEvent( const QByteArray& e ) ;
	void quitApp() ;
	void saveData() ;
	void notifyOnDownloadComplete( const QString& = {} ) ;
	void notifyOnAllDownloadComplete( const QString& ) ;
	void showTrayIcon( bool ) ;
	void keyPressEvent( QKeyEvent * ) override ;
	~MainWindow() override ;
private:
	signals:
	void processEventSignal( const QByteArray& ) ;
private:
	void processEventSlot( const QByteArray& ) ;

	static void signalHandler( int ) ;
	static void setUpSignal( int ) ;
	template< typename Int,typename ... INTS >
	static void setUpSignal( Int sig,INTS ... sigs )
	{
		MainWindow::setUpSignal( sig ) ;
		MainWindow::setUpSignal( sigs ... ) ;
	}
	static void setUpSignals( MainWindow * ) ;

	class setUpMainWindow
	{
	public:
		setUpMainWindow( QMainWindow * mw )
		{
			m_ui.setupUi( mw ) ;
		}
		Ui::MainWindow& get()
		{
			return m_ui ;
		}
		QPlainTextEdit& plainTextEditLogger()
		{
			return *m_ui.plainTextEditLogger ;
		}
		int currentIndex()
		{
			return m_ui.tabWidget->currentIndex() ;
		}
		void retranslateUi( QMainWindow * mw )
		{
			m_ui.retranslateUi( mw ) ;
		}
	private:
		Ui::MainWindow m_ui ;
	} ;

	bool m_dataNotSaved = true ;
	QSystemTrayIcon m_trayIcon ;
	QApplication& m_qApp ;
	QString m_appName ;
	setUpMainWindow m_ui ;
	Logger m_logger ;
	engines m_engines ;
	utility::printOutPut m_printOutPut ;
	tabManager m_tabManager ;
	settings& m_settings ;
	static MainWindow * m_mainWindow ;
	bool m_showTrayIcon ;
	QShortcut m_shortcut ;
	void closeEvent( QCloseEvent * ) override ;
};

#endif // MAINWINDOW_H

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCloseEvent>
#include <QSystemTrayIcon>
#include <QString>
#include <QSettings>
#include <QStringList>
#include <QMenu>

namespace Ui {
class MainWindow;
}

namespace settings{
	static const QString EnabledHighDpiScalingFactor = "EnabledHighDpiScalingFactor" ;
	static const QString PresetOptions = "PresetOptions" ;
	static const QString DownloadFolder = "DownloadFolder" ;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT
public:
	explicit MainWindow( QSettings& ) ;
	~MainWindow() ;
private:
	QSettings& m_settings ;
	QString m_downloadFolder ;
	QSystemTrayIcon m_trayIcon ;
	Ui::MainWindow * m_ui ;
	QMenu * m_menu ;
	QStringList m_tmp ;
	void run( const QString& cmd,const QStringList& args ) ;
	void list() ;
	void download() ;
	void exit() ;
	void enableAll() ;
	void disableAll() ;
	void closeEvent( QCloseEvent * ) ;
	QMenu * setMenu() ;
};

#endif // MAINWINDOW_H

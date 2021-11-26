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

#ifndef LOGWINDOW_H
#define LOGWINDOW_H

#include <QWidget>
#include <QPlainTextEdit>
#include <QCloseEvent>

class settings ;
class Logger ;

namespace Ui {
class logWindow;
}

class logWindow : public QWidget
{
	Q_OBJECT
public:
	logWindow( QWidget * parent,settings&,Logger& ) ;
	~logWindow() override ;
	void update( const QString& ) ;
	void setText( const QString& ) ;
	template< typename LoggerData >
	void update( const LoggerData& e )
	{
		if( this->isVisible() ){

			this->setText( e.toString() ) ;
		}
	}
	void Hide() ;
	void Show() ;
	void clear() ;
private:
	void closeEvent( QCloseEvent * ) override ;
	Ui::logWindow * m_ui ;
	settings& m_settings ;
};

#endif // LOGWINDOW_H

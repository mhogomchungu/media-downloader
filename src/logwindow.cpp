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

#include "logwindow.h"
#include "ui_logwindow.h"

#include "settings.h"
#include "utility.h"
#include "logger.h"

logWindow::logWindow( QWidget * parent,settings& s,Logger& logger ) :
	QWidget( parent ),
	m_ui( new Ui::logWindow ),
	m_settings( s ),
	m_logger( logger )
{
	m_ui->setupUi( this ) ;

	m_ui->plainTextEdit->setReadOnly( true ) ;

	m_ui->plainTextEdit->installEventFilter( this ) ;

	connect( m_ui->pbClose,&QPushButton::clicked,[ this ](){		

		this->Hide() ;
	} ) ;

	connect( m_ui->pbShowAll,&QPushButton::clicked,[ this ](){

		m_logger.showAllLogs() ;
	} ) ;
}

logWindow::~logWindow()
{
	delete m_ui ;
}

void logWindow::setText( const QByteArray& e )
{
	m_ui->plainTextEdit->setPlainText( e ) ;
	m_ui->plainTextEdit->moveCursor( QTextCursor::End ) ;
}

void logWindow::update( const QByteArray& e )
{
	this->setText( e ) ;
}

void logWindow::Hide()
{
	const auto& r = this->window()->geometry() ;

	auto x = QString::number( r.x() ) ;
	auto y = QString::number( r.y() ) ;
	auto w = QString::number( r.width() ) ;
	auto h = QString::number( r.height() ) ;

	m_settings.setWindowDimensions( "LogWindow",x + "-" + y + "-" + w + "-" + h ) ;

	this->hide() ;
	this->clear() ;
}

void logWindow::Show()
{
	auto w = m_settings.windowsDimensions( "LogWindow" ) ;

	if( !w.isEmpty() ){

		auto m = util::split( w,'-',true ) ;

		if( m.size() == 4 ){

			QRect r ;

			r.setX( m.at( 0 ).toInt() ) ;
			r.setY( m.at( 1 ).toInt() ) ;
			r.setWidth( m.at( 2 ).toInt() ) ;
			r.setHeight( m.at( 3 ).toInt() ) ;

			this->window()->setGeometry( r ) ;
		}
	}

	this->show() ;
}

void logWindow::clear()
{
	m_ui->plainTextEdit->clear() ;
}

void logWindow::retranslateUi()
{
	m_ui->retranslateUi( this ) ;
}

bool logWindow::eventFilter( QObject * obj,QEvent * event )
{
	return utility::showContextMenuLogWidget( obj,event,m_ui->plainTextEdit,[ this ](){

		m_logger.clear() ;
	} ) ;
}

void logWindow::closeEvent( QCloseEvent * e )
{
	e->ignore() ;
	this->Hide() ;
}

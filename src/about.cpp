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

#include "about.h"
#include "version.h"
#include "utility.h"

#include <QDesktopServices>

void about::enableAll()
{
}

void about::disableAll()
{
}

void about::resetMenu()
{
}

about::about( const Context& ctx ) : m_ctx( ctx )
{
	this->retranslateUi() ;

	auto m = m_ctx.Ui().textBrowser ;

	QObject::connect( m,&QTextBrowser::anchorClicked,[]( const QUrl& e ){

		QDesktopServices::openUrl( e ) ;
	} ) ;

	auto p = m->palette() ;
	p.setColor( QPalette::Base,m->palette().color( QPalette::Window ) ) ;

	m->setPalette( p ) ;
}

void about::retranslateUi()
{
	auto url = ": <a href=\"https://github.com/mhogomchungu/media-downloader\">https://github.com/mhogomchungu/media-downloader</a>" ;
	auto version   = QObject::tr( "Version" ) ;
	auto website   = QObject::tr( "Project Page" ) + url ;
	auto copyright = QObject::tr( "Copyright" ) ;
	auto license   = QObject::tr( "License" ) ;
	auto email     = QObject::tr( "Email" ) ;

	auto QtVersion = [ & ](){

		if( utility::platformIsLikeWindows() ){

			return QObject::tr( "Qt Version" ) + ": " QTVERSION "<br><br>" ;
		}else{
			return QString() ;
		}
	}() ;

	auto banner1 = QObject::tr( "This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version." ) ;
	auto banner2 = QObject::tr( "This program is distributed in the hope that it will be useful,but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details." ) ;

	auto about = QString( "<br><br><br><br><center>%1: %2<br><br>%3%4: %5, Francis Banyikwa<br><br>%6: mhogomchungu@gmail.com<br><br>%7<br><br>%8: GPLv2+<br><br>" + banner1 + "<br><br>" + banner2 + "</center>" ) ;

	auto vv = utility::aboutVersionInfo() ;

	auto m = about.arg( version,vv,QtVersion,copyright,COPYRIGHT,email,website,license ) ;

	m_ctx.Ui().textBrowser->setText( m ) ;
}

void about::tabEntered()
{
}

void about::tabExited()
{
}

void about::exiting()
{
}

void about::textAlignmentChanged( Qt::LayoutDirection )
{
}

void about::keyPressed( utility::mainWindowKeyCombo )
{
}

void about::init_done()
{
}

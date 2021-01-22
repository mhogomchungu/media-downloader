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

#include "ui_mainwindow.h"

about::about()
{
}

void about::init( Ui::MainWindow * m_ui,QWidget * )
{
	auto about = QString( "Version: %1\n\n\
Copyright: %2, Francis Banyikwa\n\nEmail: mhogomchungu@gmail.com\n\n\
License: GPLv2+\n\nThis program is free software; you can redistribute \
it and/or modify it under the terms of the GNU General Public License \
as published by the Free Software Foundation; either version 2 of the \
License, or (at your option) any later version.\n\n\
This program is distributed in the hope that it will be useful,\
but WITHOUT ANY WARRANTY; without even the implied warranty of \
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the \
GNU General Public License for more details." ) ;

	m_ui->TextLabelAbout->setText( about.arg( VERSION,COPYRIGHT ) ) ;
}
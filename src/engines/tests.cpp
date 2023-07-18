/*
 *
 *
 *  Copyright (c) 2022
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
#include "tests.h"
#include "../utils/single_instance.hpp"

#include "wget.h"
#include "yt-dlp.h"
#include "safaribooks.h"
#include "gallery-dl.h"
#include "svtplay-dl.h"

#include "../util.hpp"
#include <iostream>

#include <QString>
#include <QEventLoop>

static bool _run_test( const QStringList& args )
{
	for( const auto& it : args ){

		if( it.startsWith( "--media-downloader-test-engine" ) ){

			return true ;
		}
	}

	return false ;
}

bool tests::test_engine( const QStringList& args,QApplication& app )
{
	if( _run_test( args ) ){

		class myApp
		{
		public:
			struct args
			{
				const QStringList& args ;
				QApplication& app ;
			} ;
			myApp( const myApp::args& args ) : m_args( args )
			{
			}
			void start( const QByteArray& )
			{
				for( const auto& it : m_args.args ){

					if( it == "--media-downloader-test-engine-safaribooks" ){

						return this->testEngine( safaribooks::testData() ) ;

					}else if( it == "--media-downloader-test-engine-yt-dlp" ){

						return this->testEngine( yt_dlp::testYtDlp() ) ;

					}else if( it == "--media-downloader-test-engine-yt-dlp-ffmpeg" ){

						return this->testEngine( yt_dlp::testFfmpeg() ) ;

					}else if( it == "--media-downloader-test-engine-wget" ){

						return this->testEngine( wget::testData() ) ;

					}else if( it == "--media-downloader-test-engine-gallery-dl" ){

						return this->testEngine( gallery_dl::testData() ) ;

					}else if( it == "--media-downloader-test-engine-svtplay-dl" ){

						return this->testEngine( svtplay_dl::testData() ) ;
					}
				}

				this->done() ;
			}
			void done()
			{
				m_args.app.quit() ;
			}
			void testEngine( const char * output )
			{
				m_list = util::split( output,'\n' ) ;

				util::Timer( 500,[ this ]( int ){

					if( m_counter < m_list.size() ){

						std::cout << m_list.at( m_counter ).constData() << std::endl ;

						m_counter++ ;

						return false ;
					}else{
						this->done() ;

						return true ;
					}
				} ) ;
			}
		private:
			QList< QByteArray > m_list ;
			myApp::args m_args ;
			int m_counter = 0 ;
		};

		utils::app::runMultiInstances( utils::app::appInfo< myApp,myApp::args >( { args,app },"",app,"" ) ) ;

		return true ;
	}else{
		return false ;
	}
}

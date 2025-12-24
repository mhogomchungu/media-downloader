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
#include "lux.h"
#include "getsauce.h"

#include "../util.hpp"

#include <iostream>
#include <array>

#include <QString>
#include <QEventLoop>

#define TEST_ENGINE_PREFIX "--media-downloader-test-engine"

#if QT_VERSION >= QT_VERSION_CHECK( 5,10,0 )

#include <QRandomGenerator>

static int _getInterval( int x,int y )
{
	return QRandomGenerator::global()->bounded( x,y ) ;
}

#else

#include <random>

static int _getInterval( int x,int y )
{
	std::random_device rd ;
	std::mt19937 gen( rd() ) ;
	std::uniform_real_distribution<> dis( x,y ) ;
	return dis( gen ) ;
}

#endif


class Tests
{
public:
	struct engines
	{
		const char * arg ;
		const char *( *data )( void ) ;
	} ;
	auto begin() const
	{
		return m_engines.begin() ;
	}
	auto end() const
	{
		return m_engines.end() ;
	}
private:
	std::array< engines,10 > m_engines = { {
		{ TEST_ENGINE_PREFIX"-safaribooks",&safaribooks::testData },
		{ TEST_ENGINE_PREFIX"-yt-dlp",&yt_dlp::testYtDlp },
		{ TEST_ENGINE_PREFIX"-yt-dlp-playlist",&yt_dlp::testYtDlpPlayList },
		{ TEST_ENGINE_PREFIX"-yt-dlp-metadata",&yt_dlp::testYtDlpMetadata },
		{ TEST_ENGINE_PREFIX"-yt-dlp-ffmpeg",&yt_dlp::testFfmpeg },
		{ TEST_ENGINE_PREFIX"-wget",&wget::testData },
		{ TEST_ENGINE_PREFIX"-gallery-dl",&gallery_dl::testData },
		{ TEST_ENGINE_PREFIX"-getsauce",&getsauce::testData },
		{ TEST_ENGINE_PREFIX"-svtplay-dl",&svtplay_dl::testData },
		{ TEST_ENGINE_PREFIX"-lux",&lux::testData } } } ;
} ;

class testing
{
public:
	struct args
	{
		const QStringList& args ;
		QApplication& app ;
	} ;
	testing( const testing::args& args ) : m_args( args )
	{
	}
	void start( const QByteArray& )
	{
		Tests tests ;

		QString s ;

		for( const auto& it : m_args.args ){

			for( const auto& xt : tests ){

				auto m = it.indexOf( "0xdeadbeef" ) ;

				if( m == -1 ){

					s = it ;
				}else{
					s = it.mid( 0,m ) ;
				}

				if( s == xt.arg ){

					return this->testEngine( xt.data() ) ;
				}
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

		util::Timer( _getInterval( 100,600 ),[ this ]( int ){

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
	testing::args m_args ;
	int m_counter = 0 ;
} ;

static bool _run_test( const QStringList& args )
{
	for( const auto& it : args ){

		if( it.startsWith( TEST_ENGINE_PREFIX ) ){

			return true ;
		}
	}

	return false ;
}

bool tests::test_engine( const QStringList& args,QApplication& app )
{
	if( _run_test( args ) ){

		utils::app::appInfo< testing,testing::args > mm( { args,app },"",app,"" ) ;

		utils::app::runMultiInstances( std::move( mm ) ) ;

		return true ;
	}else{
		return false ;
	}
}

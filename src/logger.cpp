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

#include "logger.h"

#include "engines.h"

#include "utility.h"

#include "engines/yt-dlp.h"

Logger::Logger( QPlainTextEdit& e,QWidget *,settings& s ) :
	m_logWindow( nullptr,s,*this ),
	m_textEdit( e ),
	m_processOutPuts( true ),
	m_settings( s )
{
	m_textEdit.setReadOnly( true ) ;
}

void Logger::registerDone( int id )
{
	if( m_processOutPuts.registerDone( id ) ){

		this->update() ;
	}
}

void Logger::add( const QByteArray& s,int id )
{
	if( s.startsWith( "[media-downloader]" ) ){

		m_processOutPuts.add( s,id ) ;
	}else{
		m_processOutPuts.add( "[media-downloader] " + s,id ) ;
	}

	this->update() ;
}

void Logger::clear()
{
	m_processOutPuts.clear() ;
	m_textEdit.clear() ;
	m_logWindow.clear() ;
}

void Logger::setMaxProcessLog( int s )
{
	m_processOutPuts.removeExtraLogs() ;
	m_maxProcessLog = s ;
}

void Logger::showLogWindow()
{
	m_logWindow.setText( m_processOutPuts.toString() ) ;
	m_logWindow.Show() ;
}

void Logger::reTranslateLogWindow()
{
	m_logWindow.retranslateUi() ;
}

void Logger::updateView( bool e )
{
	m_updateView = e ;
	this->update() ;
}

void Logger::update()
{
	auto s = m_settings.maxLoggerProcesses() ;

	auto e = m_maxProcessLog > s ? m_maxProcessLog : s ;

	while( true ){

		int current_amount = static_cast< int >( m_processOutPuts.size() ) ;

		if( current_amount > e ){

			if( !m_processOutPuts.removeFirstFinished() ){

				break ;
			}
		}else{
			break ;
		}
	}

	auto m = m_processOutPuts.toString() ;

	if( m_updateView ){

		m_textEdit.setPlainText( m ) ;
		m_textEdit.moveCursor( QTextCursor::End ) ;
	}

	m_logWindow.update( m ) ;
}

bool Logger::Data::registerDone( int id )
{
	for( auto& it : m_processOutputs ){

		if( it.processId() == id ){

			it.setProcessAsFinished() ;

			return true ;
		}
	}

	return false ;
}

void Logger::Data::removeExtraLogs()
{
	auto& v = m_processOutputs ;
	auto m = Logger::Data::processOutput::IdLessThanZero() ;
	v.erase( std::remove( v.begin(),v.end(),m ),v.end() ) ;
}

bool Logger::Data::removeFirstFinished()
{
	for( auto it = m_processOutputs.begin() ; it != m_processOutputs.end() ; it++ ){

		if( it->processFinished() ){

			m_processOutputs.erase( it ) ;

			return true ;
		}
	}

	return false ;
}

bool Logger::Data::doneDownloadingText( const QByteArray& data )
{
	return utility::stringConstants::doneDownloadingText( data ) ;
}

void Logger::updateLogger::run( bool humanReadableJson,const QByteArray& data )
{
	if( data.isEmpty() ){

		return ;
	}

	if( m_args.likeYoutubeDl && humanReadableJson ){

		if( data.startsWith( '[' ) || data.startsWith( '{' ) ){

			QJsonParseError err ;

			auto json = QJsonDocument::fromJson( data,&err ) ;

			if( err.error == QJsonParseError::NoError ){

				auto s = json.toJson( QJsonDocument::JsonFormat::Indented ) ;

				m_outPut.add( s,m_id ) ;

				return ;
			}
		}
	}

	const auto& sp = m_args.splitLinesBy ;

	if( sp.size() == 1 && sp[ 0 ].size() > 0 ){

		this->add( data,sp[ 0 ][ 0 ] ) ;

	}else if( sp.size() == 2 && sp[ 0 ].size() > 0 && sp[ 1 ].size() > 0 ){

		for( const auto& m : util::split( data,sp[ 0 ][ 0 ] ) ){

			this->add( m,sp[ 1 ][ 0 ] ) ;
		}
	}else{
		for( const auto& m : util::split( data,'\r' ) ){

			this->add( m,'\n' ) ;
		}
	}
}

bool Logger::updateLogger::meetCondition( const QByteArray& l,const QJsonObject& obj ) const
{
	const QString line = l ;

	if( obj.contains( "startsWith" ) ){

		return line.startsWith( obj.value( "startsWith" ).toString() ) ;
	}

	if( obj.contains( "endsWith" ) ){

		return line.endsWith( obj.value( "endsWith" ).toString() ) ;
	}

	if( obj.contains( "contains" ) ){

		return line.contains( obj.value( "contains" ).toString() ) ;
	}

	if( obj.contains( "containsAny" ) ){

		const auto arr = obj.value( "containsAny" ).toArray() ;

		for( const auto& it : arr ){

			if( line.contains( it.toString() ) ) {

				return true ;
			}
		}

		return false ;
	}

	if( obj.contains( "containsAll" ) ){

		const auto arr = obj.value( "containsAll" ).toArray() ;

		for( const auto& it : arr ){

			if( !line.contains( it.toString() ) ) {

				return false ;
			}
		}

		return true ;
	}

	return false ;
}

bool Logger::updateLogger::meetCondition( const QByteArray& line ) const
{
	const auto& obj = m_args.controlStructure ;

	auto connector = obj.value( "Connector" ).toString() ;

	if( connector.isEmpty() ){

		auto oo = obj.value( "lhs" ) ;

		if( oo.isObject() ){

			return this->meetCondition( line,oo.toObject() ) ;
		}else{
			return false ;
		}
	}else{
		auto obj1 = obj.value( "lhs" ) ;
		auto obj2 = obj.value( "rhs" ) ;

		if( obj1.isObject() && obj2.isObject() ){

			auto a = this->meetCondition( line,obj1.toObject() ) ;
			auto b = this->meetCondition( line,obj2.toObject() ) ;

			if( connector == "&&" ){

				return a && b ;

			}else if( connector == "||" ){

				return a || b ;
			}else{
				return false ;
			}
		}else{
			return false ;
		}
	}
}

bool Logger::updateLogger::skipLine( const QByteArray& line ) const
{
	if( line.isEmpty() ){

		return true ;
	}else{
		for( const auto& it : m_args.skipLinesWithText ){

			if( line.contains( it.toUtf8() ) ){

				return true ;
			}
		}

		return false ;
	}
}

template< typename MeetCondition,typename AddReplace >
static void _add_or_replace( Logger::Data& outPut,
			     int id,
			     const QByteArray& e,
			     MeetCondition meetCondition,
			     AddReplace addReplace )
{
	outPut.replaceOrAdd( e,id,[ & ]( const QByteArray& e ){

		return meetCondition( e ) ;

	},[ & ]( const QByteArray& e ){

		return addReplace( e ) ;
	} ) ;
}

static bool _false( const QByteArray& )
{
	return false ;
}

template< typename Function,typename Filter >
static void _ytDlp( const Logger::locale& locale,
		    const QByteArray& e,
		    Logger::Data& outPut,
		    int id,
		    Function function,
		    Filter filter )
{
	auto mm = function( locale,e ) ;

	if( outPut.mainLogger() ){

		if( !mm.progress.isEmpty() ){

			_add_or_replace( outPut,id,mm.QByteArrayProgress(),filter,_false ) ;
		}
	}else{
		QJsonObject obj ;

		obj.insert( "filename",mm.filename ) ;
		obj.insert( "progress",mm.QStringProgress().mid( 11 ) ) ;

		auto m = QJsonDocument( obj ).toJson( QJsonDocument::JsonFormat::Compact ) ;

		_add_or_replace( outPut,id,m,[ & ]( const QByteArray& ){

			return filter( e ) ;

		},_false ) ;
	}
}

static bool _ffmpeg( const QByteArray& e )
{
	return e.startsWith( "frame=" ) || e.startsWith( "size=" ) ;
}

static bool _aria2c( const QByteArray& e )
{
	return e.startsWith( "[DL:" ) || e.startsWith( "[#" ) ;
}

struct result
{
	const QByteArray& progress ;
	QString filename ;

	const QByteArray& QByteArrayProgress()
	{
		return progress ;
	}
	QString QStringProgress()
	{
		return progress ;
	}
};

static result _reflect( const Logger::locale&,const QByteArray& e )
{
	return { e,"" } ;
}

void Logger::updateLogger::add( const QByteArray& data,QChar token ) const
{
	for( const auto& e : util::split( data,token ) ){

		if( this->skipLine( e ) ){

			continue ;

		}else if( ( m_like_yt_dlp || m_ytdl ) && _ffmpeg( e ) ){

			/*
			 * youtube-dl or yt-dlp and they decided to use ffmpeg as an external downloader
			 */

			_ytDlp( m_locale,e,m_outPut,m_id,_reflect,_ffmpeg ) ;

		}else if( ( m_yt_dlp_aria2c || m_yt_dlp_ffmpeg ) && yt_dlp::meetCondition( e ) ){

			/*
			 * yt-dlp-aria2c/yt-dlp-ffmpeg and yt-dlp decided to use internal downloader
			 */

			_ytDlp( m_locale,e,m_outPut,m_id,yt_dlp::formatYdDlpOutput,yt_dlp::meetCondition ) ;

		}else if( m_aria2c && _aria2c( e ) ){

			/*
			 * aria2c
			 */

			_add_or_replace( m_outPut,m_id,e,_aria2c,_false ) ;

		}else if( m_yt_dlp_aria2c && _aria2c( e ) ){

			/*
			 * yt-dlp-aria2c
			 */
			_ytDlp( m_locale,e,m_outPut,m_id,_reflect,_aria2c ) ;

		}else if( m_yt_dlp && yt_dlp::meetCondition( e ) ){

			_ytDlp( m_locale,e,m_outPut,m_id,yt_dlp::formatYdDlpOutput,yt_dlp::meetCondition ) ;

		}else if( m_ytdl && yt_dlp::meetCondition( e ) ){

			/*
			 * youtube-dl
			 */
			_ytDlp( m_locale,e,m_outPut,m_id,_reflect,yt_dlp::meetCondition ) ;

		}else if( this->meetCondition( e ) ){

			_add_or_replace( m_outPut,m_id,e,[ this ]( const QByteArray& s ){

				return this->meetCondition( s )  ;

			},_false ) ;
		}else{
			m_outPut.add( e,m_id ) ;
		}
	}
}

QString Logger::locale::locale::formattedDataSize( qint64 s ) const
{
#if QT_VERSION >= QT_VERSION_CHECK( 5,14,0 )
	return m_locale.formattedDataSize( s ) ;
#else
	std::array< const char *,7 > sizes = { "EiB", "PiB", "TiB", "GiB", "MiB", "KiB", "B" } ;

	qint64  multiplier = 1024ULL * 1024ULL * 1024ULL * 1024ULL * 1024ULL * 1024ULL ;

	QString result ;

	for( size_t i = 0 ; i < sizes.size() ; i++,multiplier /= 1024 ){

		if( s < multiplier ){

			continue ;
		}

		if( s % multiplier == 0 ){

			auto a = QString::number( s / multiplier ) ;
			auto b = sizes[ i ] ;

			result = QString( "%1 %2" ).arg( a,b ) ;
		}else{
			auto a = static_cast< double >( s ) / static_cast< double >( multiplier ) ;
			auto b = sizes[ i ] ;
			auto c = QString::number( a,'f',2 ) ;

			result = QString( "%1 %2" ).arg( c,b ) ;
		}

		return result ;
	}

	return {} ;
#endif
}

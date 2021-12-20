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

Logger::Logger( QPlainTextEdit& e,QWidget *,settings& s ) :
	m_logWindow( nullptr,s,*this ),
	m_textEdit( e )
{
	m_textEdit.setReadOnly( true ) ;
}

void Logger::add( const QByteArray& s,int id )
{
	if( s.startsWith( "[media-downloader]" ) ){

		m_lines.add( s,id ) ;
	}else{
		m_lines.add( "[media-downloader] " + s,id ) ;
	}

	this->update() ;
}

void Logger::clear()
{
	m_lines.clear() ;
	m_textEdit.clear() ;
	m_logWindow.clear() ;
}

void Logger::showLogWindow()
{
	m_logWindow.setText( m_lines.toString() ) ;
	m_logWindow.Show() ;
}

void Logger::updateView( bool e )
{
	m_updateView = e ;
	this->update() ;
}

void Logger::update()
{	
	auto m = m_lines.toString() ;

	if( m_updateView ){

		m_textEdit.setPlainText( m ) ;
		m_textEdit.moveCursor( QTextCursor::End ) ;
	}

	m_logWindow.update( m ) ;
}

QList< QByteArray > Logger::Data::toStringList() const
{
	return util::split( this->toString(),'\n' ) ;
}

bool Logger::Data::postProcessText( const QByteArray& data )
{
	return utility::stringConstants::postProcessMarker( data ) ;
}

void Logger::updateLogger::run( bool humanReadableJson,const QByteArray& data )
{
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
	if( id == -1 ){

		if( outPut.isEmpty() ){

			outPut.add( e ) ;
		}else{
			if( meetCondition( outPut.lastText() ) ){

				outPut.replaceLast( e ) ;
			}else{
				outPut.add( e ) ;
			}
		}
	}else{
		outPut.replaceOrAdd( e,id,[ & ]( const QByteArray& e ){

			return meetCondition( e ) ;

		},[ & ]( const QByteArray& e ){

			return addReplace( e ) ;
		} ) ;
	}
}

void Logger::updateLogger::add( const QByteArray& data,QChar token ) const
{
	bool aria2c = m_args.name.contains( "aria2c" ) ;
	bool ffmpeg = m_args.name.contains( "ffmpeg" ) ;
	bool yt_dlp = m_args.name == "yt-dlp" ;
	bool ytdl   = m_args.name == "youtube-dl" ;

	for( const auto& e : util::split( data,token ) ){

		if( this->skipLine( e ) ){

			continue ;

		}else if( ( yt_dlp || ytdl ) && ( e.startsWith( "frame=" ) || e.startsWith( "size=" ) ) ){

			/*
			 * youtube-dl or yt-dlp and they decided to use ffmpeg as an external downloader
			 */

			_add_or_replace( m_outPut,m_id,e,[]( const QByteArray& s ){

				return s.startsWith( "frame=" ) || s.startsWith( "size=" ) ;

			},[]( const QByteArray& ){

				return false ;
			} ) ;

		}else if( ( aria2c || ffmpeg ) && e.startsWith( "[download]" ) && e.contains( "ETA" ) ){

			/*
			 * yt-dlp-aria2c/yt-dlp-ffmpeg and yt-dlp decided to use internal downloader
			 */

			_add_or_replace( m_outPut,m_id,e,[]( const QByteArray& s ){

				return s.startsWith( "[download]" ) && s.contains( "ETA" ) ;

			},[]( const QByteArray& ){

				return false ;
			} ) ;

		}else if( aria2c && e.startsWith( "[DL:" ) ){

			/*
			 * aria2c when doing concurrent downloads
			 */

			_add_or_replace( m_outPut,m_id,e,[]( const QByteArray& s ){

				return s.startsWith( "[DL:" ) ;

			},[]( const QByteArray& ){

				return false ;
			} ) ;

		}else if( this->meetCondition( e ) ){

			_add_or_replace( m_outPut,m_id,e,[ this ]( const QByteArray& s ){

				return this->meetCondition( s )  ;

			},[ this ]( const QByteArray& e ){

				if( m_args.likeYoutubeDl ){

					return e.startsWith( "[download] 100.0%" ) ;
				}else{
					return false ;
				}
			} ) ;
		}else{
			m_outPut.add( e,m_id ) ;
		}
	}
}

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

#include "gallery-dl.h"
#include "../settings.h"

#include <QDesktopServices>

const char * gallery_dl::testData()
{
	return R"R(  ./gallery-dl/directlink/2chen.moe_ass…66e7972cebece1d016342b7fb985eb23f1a.webm
 16%   1.01MB 337.19kB/s
 16%   1.04MB 264.48kB/s
 17%   1.08MB 248.24kB/s
 18%   1.11MB 255.74kB/s
 18%   1.14MB 263.25kB/s
 19%   1.17MB 270.77kB/s
 19%   1.21MB 278.28kB/s
 20%   1.24MB 284.61kB/s
 20%   1.27MB 286.94kB/s
 21%   1.31MB 289.93kB/s
 21%   1.34MB 292.43kB/s
 22%   1.37MB 294.17kB/s
 22%   1.40MB 294.08kB/s
 23%   1.44MB 292.42kB/s
 23%   1.47MB 283.18kB/s
 24%   1.50MB 278.01kB/s
 24%   1.54MB 284.03kB/s
 25%   1.57MB 290.07kB/s
 26%   1.60MB 294.82kB/s
 26%   1.63MB 297.96kB/s
 27%   1.67MB 298.45kB/s
 27%   1.70MB 300.55kB/s
 28%   1.73MB 301.72kB/s
 28%   1.76MB 303.21kB/s
 29%   1.80MB 303.46kB/s
 29%   1.83MB 305.10kB/s
 30%   1.86MB 306.53kB/s
 30%   1.90MB 308.32kB/s
 31%   1.93MB 308.32kB/s
 31%   1.96MB 308.59kB/s
 32%   1.99MB 308.73kB/s
 32%   2.03MB 309.23kB/s
 33%   2.06MB 310.40kB/s
 33%   2.09MB 312.53kB/s
 34%   2.12MB 314.56kB/s
 35%   2.16MB 315.66kB/s
 35%   2.19MB 317.34kB/s
 36%   2.22MB 318.94kB/s
 36%   2.26MB 319.17kB/s
 37%   2.29MB 319.42kB/s
 37%   2.32MB 318.24kB/s
 38%   2.35MB 317.41kB/s
 38%   2.39MB 312.11kB/s
 39%   2.42MB 312.11kB/s
 39%   2.45MB 312.10kB/s
 40%   2.49MB 312.14kB/s
 40%   2.52MB 312.48kB/s
 41%   2.55MB 312.33kB/s
 41%   2.58MB 312.41kB/s
 42%   2.62MB 312.78kB/s
 42%   2.65MB 314.36kB/s
 43%   2.68MB 316.42kB/s
 44%   2.71MB 317.93kB/s
 44%   2.75MB 319.48kB/s
 45%   2.78MB 320.58kB/s
 45%   2.81MB 321.11kB/s
 46%   2.85MB 321.38kB/s
 46%   2.88MB 322.04kB/s
 47%   2.91MB 321.11kB/s
 47%   2.94MB 318.67kB/s
 48%   2.98MB 318.77kB/s
 48%   3.01MB 319.02kB/s
 49%   3.04MB 319.91kB/s
 49%   3.08MB 320.48kB/s
 50%   3.11MB 319.72kB/s
 50%   3.14MB 316.85kB/s
 51%   3.17MB 317.65kB/s
 52%   3.21MB 318.64kB/s
 52%   3.24MB 311.89kB/s
 53%   3.27MB 312.65kB/s
 53%   3.30MB 312.89kB/s
 54%   3.34MB 312.53kB/s
 54%   3.37MB 312.46kB/s
 55%   3.40MB 311.90kB/s
 55%   3.44MB 311.87kB/s
 56%   3.47MB 312.54kB/s
 56%   3.50MB 313.18kB/s
 57%   3.53MB 313.11kB/s
 57%   3.57MB 313.15kB/s
 58%   3.60MB 311.26kB/s
 58%   3.63MB 310.74kB/s
 59%   3.67MB 310.29kB/s
 59%   3.70MB 310.22kB/s
 60%   3.73MB 310.50kB/s
 61%   3.76MB 310.56kB/s
 61%   3.80MB 310.39kB/s
 62%   3.83MB 311.13kB/s
 62%   3.86MB 311.40kB/s
 63%   3.89MB 311.63kB/s
 63%   3.93MB 312.28kB/s
 64%   3.96MB 312.53kB/s
 64%   3.99MB 312.74kB/s
 65%   4.03MB 313.64kB/s
 65%   4.06MB 314.19kB/s
 66%   4.09MB 315.01kB/s
 66%   4.12MB 315.31kB/s
 67%   4.16MB 315.73kB/s
 67%   4.19MB 316.01kB/s
 68%   4.22MB 316.72kB/s
 69%   4.25MB 316.69kB/s
 69%   4.29MB 316.53kB/s
 70%   4.32MB 316.45kB/s
 70%   4.35MB 316.36kB/s
 71%   4.39MB 315.89kB/s
 71%   4.42MB 315.66kB/s
 72%   4.45MB 315.91kB/s
 72%   4.48MB 316.38kB/s
 73%   4.52MB 316.77kB/s
 73%   4.55MB 317.03kB/s
 74%   4.58MB 316.19kB/s
 74%   4.62MB 316.29kB/s
 75%   4.65MB 316.68kB/s
 75%   4.68MB 316.62kB/s
 76%   4.71MB 316.60kB/s
 76%   4.75MB 315.82kB/s
 77%   4.78MB 314.73kB/s
 78%   4.81MB 314.21kB/s
 78%   4.84MB 313.50kB/s
 79%   4.88MB 313.99kB/s
 79%   4.91MB 314.29kB/s
 80%   4.94MB 313.69kB/s
 80%   4.98MB 312.42kB/s
 81%   5.01MB 311.82kB/s
 81%   5.04MB 311.36kB/s
 82%   5.07MB 311.20kB/s
 82%   5.11MB 310.34kB/s
 83%   5.14MB 309.44kB/s
 83%   5.17MB 308.69kB/s
 84%   5.21MB 308.29kB/s
 84%   5.24MB 307.36kB/s
 85%   5.27MB 307.13kB/s
 85%   5.30MB 307.20kB/s
 86%   5.34MB 307.29kB/s
 87%   5.37MB 307.42kB/s
 87%   5.40MB 307.72kB/s
 88%   5.43MB 308.42kB/s
 88%   5.47MB 308.92kB/s
 89%   5.50MB 309.34kB/s
 89%   5.53MB 309.45kB/s
 90%   5.57MB 310.02kB/s
 90%   5.60MB 310.38kB/s
 91%   5.63MB 310.69kB/s
 91%   5.66MB 309.98kB/s
 92%   5.70MB 309.58kB/s
 92%   5.73MB 309.10kB/s
 93%   5.76MB 309.06kB/s
 93%   5.79MB 308.92kB/s
 94%   5.83MB 308.86kB/s
 95%   5.86MB 308.58kB/s
 95%   5.89MB 308.47kB/s
 96%   5.93MB 308.71kB/s
 96%   5.96MB 308.37kB/s
 97%   5.99MB 306.31kB/s
 97%   6.02MB 304.24kB/s
 98%   6.06MB 304.41kB/s
 98%   6.09MB 305.09kB/s
 99%   6.12MB 305.66kB/s
 99%   6.16MB 306.61kB/s
100%   6.17MB 307.06kB/s
✔ ./gallery-dl/directlink/2chen.moe_ass…66e7972cebece1d016342b7fb985eb23f1a.webm)R" ;
}

gallery_dl::gallery_dl( const engines& engines,const engines::engine& engine,QJsonObject& object ) :
	engines::engine::baseEngine( engines.Settings(),engine,engines.processEnvironment() )
{
	if( !object.contains( "CookieArgument" ) ){

		object.insert( "CookieArgument","--cookies-from-browser" ) ;
	}

	if( !object.contains( "CookieArgumentTextFile" ) ){

		object.insert( "CookieArgumentTextFile","--cookies" ) ;
	}

	object.insert( "ReplaceOutputWithProgressReport",false ) ;

	object.insert( "ControlJsonStructure",[](){

		QJsonObject obj ;

		obj.insert( "Connector","||" ) ;

		obj.insert( "lhs",[](){

			QJsonObject obj ;

			obj.insert( "startsWith","[gallery-dl]" ) ;

			return obj ;
		}() ) ;

		obj.insert( "rhs",[](){

			QJsonObject obj ;

			obj.insert( "contains","% " ) ;

			return obj ;
		}() ) ;

		return obj ;
	}() ) ;

	object.insert( "CanDownloadPlaylist",true ) ;

	QJsonArray arr ;

	arr.append( "--no-skip" ) ;
	arr.append( "--no-download" ) ;
	arr.append( "--quiet" ) ;
	arr.append( "--postprocessor" ) ;
	arr.append( "metadata" ) ;
	arr.append( "--postprocessor-option" ) ;
	arr.append( "event=prepare" ) ;
	arr.append( "--postprocessor-option" ) ;
	arr.append( "filename=-" ) ;

	object.insert( "DumptJsonArguments",arr ) ;
}

bool gallery_dl::parse( const int& s,std::vector< QByteArray >& mm,QByteArray& data )
{
	int counter = 0 ;

	int ss = s - 1 ;

	while( true ){

		ss++ ;

		auto m = data[ ss ] ;

		if( m == '{' ){

			counter++ ;

		}else if( m == '}' ){

			counter-- ;
		}

		if( counter == 0 ){

			mm.emplace_back( data.mid( s,ss + 1 ) ) ;

			data = data.mid( ss + 1 ) ;

			return false ;

		}else if( ss >= data.size() ){

			return true ;
		}
	}
}

std::vector< QByteArray > gallery_dl::parseJsonData( QByteArray& data )
{
	std::vector< QByteArray > mm ;

	while( true ){

		auto s = data.indexOf( "{" ) ;

		if( s != -1 ){

			if( this->parse( s,mm,data ) ){

				break ;
			}
		}else{
			break ;
		}
	}

	return mm ;
}

class galleryDlparseData
{
public:
	galleryDlparseData( const QString& url,const QJsonDocument& doc ) :
		m_obj( doc.object() ),m_wurl( url )
	{
	}
	QJsonObject toObj()
	{
		if( !m_fileName.isEmpty() ){

			if( !m_extension.isEmpty() ){

				m_fileName += "." + m_extension ;
			}
		}

		if( m_title.isEmpty() ){

			m_title = m_fileName ;
		}else{
			if( !m_fileName.isEmpty() ){

				m_title = m_title + "\n" + m_fileName ;
			}
		}

		if( m_url.isEmpty() ){

			m_url = m_wurl + "/" + m_fileName ;
		}

		QJsonObject obj ;

		obj.insert( "direct_url",m_url ) ;
		obj.insert( "fileName",m_fileName ) ;
		obj.insert( "title",m_title ) ;
		obj.insert( "thumbnail",m_thumbnail ) ;

		return obj ;
	}
	QJsonObject parseArtstation()
	{
		m_title     = m_obj.value( "title" ).toString() ;
		m_fileName  = m_obj.value( "filename" ).toString() ;
		m_extension = m_obj.value( "extension" ).toString() ;
		m_url       = m_obj.value( "asset" ).toObject().value( "image_url" ).toString() ;
		m_thumbnail = m_obj.value( "cover_url" ).toString() ;

		return this->toObj() ;
	}
	QJsonObject parseDeviantart()
	{
		m_title     = m_obj.value( "title" ).toString() ;
		m_fileName  = m_obj.value( "filename" ).toString() ;
		m_extension = m_obj.value( "extension" ).toString() ;
		m_url       = m_obj.value( "url" ).toString() ;

		auto thumbnails = m_obj.value( "thumbs" ).toArray() ;

		if( thumbnails.size() ){

			m_thumbnail = thumbnails[ 0 ].toObject().value( "src" ).toString() ;
		}

		return this->toObj() ;
	}
	QJsonObject parseGeneric()
	{
		m_url       = m_obj.value( "url" ).toString() ;
		m_thumbnail = m_obj.value( "thumbnail" ).toString() ;
		m_fileName  = m_obj.value( "filename" ).toString() ;
		m_extension = m_obj.value( "extension" ).toString() ;
		m_title     = this->getValue( "title","seo_alt_text","auto_alt_text" ).toString() ;

		auto imageList = m_obj.value( "images" ) ;

		if( imageList.isUndefined() ){

			auto s = m_obj.value( "media_asset" ).toObject() ;

			if( !s.isEmpty() ){

				this->setUrls( s.value( "variants" ).toArray() ) ;
			}
		}else{
			this->setUrls( imageList.toVariant().toMap() ) ;
		}

		return this->toObj() ;
	}
	void setUrls( const QMap< QString,QVariant >& m )
	{
		if( m.size() ){

			auto a = m.first().toJsonObject().value( "url" ).toString() ;
			auto b = m.last().toJsonObject().value( "url" ).toString() ;

			this->setUrls( a,b ) ;
		}
	}

	void setUrls( const QJsonArray& m )
	{
		if( m.size() ){

			auto a = m.first().toObject().value( "url" ).toString() ;
			auto b = m.last().toObject().value( "url" ).toString() ;

			this->setUrls( a,b ) ;
		}
	}
	void setUrls( const QString& thumbnailUrl,const QString& url )
	{
		if( m_thumbnail.isEmpty() ){

			m_thumbnail = thumbnailUrl ;
		}

		if( m_url.isEmpty() ){

			m_url = url ;
		}
	}
private:
	template< typename Arg >
	QJsonValue getValue( const Arg& arg )
	{
		return m_obj.value( arg ) ;
	}
	template< typename Arg,typename ... Args >
	QJsonValue getValue( const Arg& arg,Args&& ... args )
	{
		auto m = m_obj.value( arg ) ;

		if( m.isUndefined() ){

			return this->getValue( std::forward< Args >( args ) ... ) ;
		}else{
			return m ;
		}
	}

	QJsonObject m_obj ;
	QString m_wurl ;
	QString m_thumbnail ;
	QString m_url ;
	QString m_title ;
	QString m_fileName ;
	QString m_extension ;
} ;

QJsonObject gallery_dl::parseJson( const QString& url,const QByteArray& e )
{
	QJsonParseError err ;

	auto doc = QJsonDocument::fromJson( e,&err ) ;

	if( err.error == QJsonParseError::NoError ){

		galleryDlparseData parser( url,doc ) ;

		if( url.contains( "artstation.com" ) ){

			return parser.parseArtstation() ;

		}else if( url.contains( "deviantart.com" ) ){

			return parser.parseDeviantart() ;
		}else{
			return parser.parseGeneric() ;
		}
	}else{
		return {} ;
	}
}

util::Json gallery_dl::parsePlayListData( const QString& url,const QByteArray& e )
{
	return this->parseJson( url,e ) ;
}

gallery_dl::~gallery_dl()
{
}

engines::engine::baseEngine::DataFilter gallery_dl::Filter( int id )
{
	auto& s = engines::engine::baseEngine::Settings() ;
	const auto& engine = engines::engine::baseEngine::engine() ;

	return { util::types::type_identity< gallery_dl::gallery_dlFilter >(),s,engine,id } ;
}

QString gallery_dl::downloadFolder( const QString& e )
{
	return e + "/gallery-dl" ;
}

void gallery_dl::openLocalFile( const engines::engine::baseEngine::localFile& s )
{
	if( s.fileNames.size() ){

		const auto& e = s.downloadFolder ;
		auto m = QUrl::fromLocalFile( e + "/gallery-dl/" + s.fileNames.back() ) ;

		QDesktopServices::openUrl( m ) ;
	}
}

void gallery_dl::runCommandOnDownloadedFile( const std::vector< QByteArray >& e )
{
	auto& s = engines::engine::baseEngine::Settings() ;
	auto df = s.downloadFolder() + "/" + this->engine().name() + "/" ;
	s.runCommandOnSuccessfulDownload( this->engine().name(),df,e ) ;
}

engines::engine::baseEngine::optionsEnvironment gallery_dl::setProxySetting( QStringList& e,const QString& s )
{
	e.append( "--proxy" ) ;
	e.append( s ) ;

	return {} ;
}

void gallery_dl::updateDownLoadCmdOptions( const engines::engine::baseEngine::updateOpts& opts,
					   bool s,
					   const QStringList& extraOpts )
{
	auto _not_contains = []( const engines::engine::baseEngine::updateOpts& opts,const char * e ){

		for( const auto& it : util::asConst( opts.ourOptions ) ){

			if( it == e ){

				return false ;
			}
		}

		return true ;
	} ;

	if( _not_contains( opts,"-D" ) && _not_contains( opts,"-d" ) ){

		const auto& s = engines::engine::baseEngine::Settings().downloadFolder() ;

		opts.ourOptions.prepend( s + "/gallery-dl" ) ;
		opts.ourOptions.prepend( "-d" ) ;
	}

	opts.ourOptions.prepend( "--no-colors" ) ;

	opts.ourOptions.prepend( "output.shorten=false" ) ;
	opts.ourOptions.prepend( "-o" ) ;

	opts.ourOptions.prepend( "output.mode=terminal" ) ;
	opts.ourOptions.prepend( "-o" ) ;

	engines::engine::baseEngine::updateDownLoadCmdOptions( opts,s,extraOpts ) ;
}

QString gallery_dl::updateTextOnCompleteDownlod( const QString& uiText,
						 const QString& bkText,
						 const QString& dopts,
						 const QString& tabName,
						 const engines::engine::baseEngine::finishedState& f )
{
	if( f.success() ){

		if( uiText.isEmpty() ){

			auto m = QObject::tr( "Warning, Nothing Was Downloaded" ) ;

			return m + "\n" + engines::engine::baseEngine::updateTextOnCompleteDownlod( bkText,dopts,tabName,f ) ;
		}else{
			return engines::engine::baseEngine::updateTextOnCompleteDownlod( uiText,dopts,tabName,f ) ;
		}

	}else if( f.cancelled() ){

		return engines::engine::baseEngine::updateTextOnCompleteDownlod( bkText,dopts,tabName,f ) ;

	}else if( uiText.contains( "Name or service not known" ) ){

		auto m = engines::engine::baseEngine::errors::unknownUrl ;
		return engines::engine::baseEngine::errorString( f,m,bkText ) ;

	}else if( uiText.contains( "Temporary failure in name resolution" ) ){

		auto m = engines::engine::baseEngine::errors::noNetwork ;
		return engines::engine::baseEngine::errorString( f,m,bkText ) ;
	}else{
		auto m = engines::engine::baseEngine::processCompleteStateText( f ) ;
		return m + "\n" + bkText ;
	}
}

gallery_dl::gallery_dlFilter::gallery_dlFilter( settings&,const engines::engine& engine,int id ) :
	engines::engine::baseEngine::filter( engine,id ),
	m_speed( QObject::tr( "Speed:" ) ),
	m_downloaded( QObject::tr( "Downloaded" ) )
{
}

const QByteArray& gallery_dl::gallery_dlFilter::operator()( Logger::Data& s )
{
	if( s.doneDownloading() ){

		auto m = s.toLine() ;

		if( m.contains( "Unsupported URL" ) ){

			m_tmp = "Name or service not known" ;

		}else if( m.contains( "Temporary failure in name resolution" ) ){

			m_tmp = "Temporary failure in name resolution" ;

		}else if( m.contains( "Name or service not known" ) ){

			m_tmp = "Name or service not known" ;
		}

		return m_tmp ;
	}

	const auto data = s.toStringList() ;

	if( m_dir.isEmpty() && data.size() ){

		const QByteArray& e = *data.begin() ;

		if( e.startsWith( "[media-downloader] cmd:" ) ){

			auto m = util::splitPreserveQuotes( e ) ;

			for( int i = 0 ; i < m.size() ; i++ ){

				if( m[ i ] == "-d" && i + 1 < m.size() ){

					m_dir = QDir::fromNativeSeparators( m[ i + 1 ] ).toUtf8() ;

					break ;

				}else if( m[ i ] == "-D" && i + 1 < m.size() ){

					m_dir = QDir::fromNativeSeparators( m[ i + 1 ] ).toUtf8() ;

					break ;
				}
			}
		}
	}

	for( const auto& e : data ){

		auto u = QDir::fromNativeSeparators( e ) ;

		auto n = u.indexOf( m_dir ) ;

		if( n != -1 ){

			auto ss = u.mid( n + m_dir.size() + 1 ).trimmed() ;

			if( !u.startsWith( "[media-downloader] cmd:" ) ){

				s.addFileName( ss.toUtf8() ) ;
			}
		}else{
			n = u.indexOf( "./gallery-dl" ) ;

			if( n != -1 ){

				auto ss = u.mid( n + 13 ).trimmed() ;

				if( !u.startsWith( "[media-downloader] cmd:" ) ){

					s.addFileName( ss.toUtf8() ) ;
				}
			}
		}
	}

	const auto& m = s.fileNames() ;

	if( m.size() ){

		if( m.size() < 4 ){

			auto begin = m.begin() ;

			m_tmp = *begin ;

			for( auto it = begin + 1 ; it != m.end() ; it++ ){
				m_tmp += "\n" + *it ;
			}
		}else{
			auto a = QObject::tr( "%1 Already Downloaded" ).arg( QString::number( m.size() - 1 ) ) ;
			m_tmp = a.toUtf8() + "\n" + m.back() ;
		}

		if( s.lastLineIsProgressLine() ){

			auto m = util::split( s.lastText(),' ',true ) ;

			m.removeAll( "\r" ) ;

			if( m.size() > 2 ){

				const auto& p = m[ 0 ] ;
				const auto& sp = m[ 1 ] ;
				const auto& ss = m[ 2 ] ;

				auto e = QString( "\n%1 %2, %3: %4 (%5)" ).arg( m_speed,ss,m_downloaded,sp,p ) ;

				m_tmp += e.toUtf8() ;
			}else{
				m_tmp += "\n" + s.lastText() ;
			}
		}
	}else{
		const auto& s = data.last() ;

		if( s.startsWith( "[media-downloader] cmd:" ) ){

			return m_preProcessing.text() ;
		}else{
			m_tmp = s ;

			return m_tmp ;
		}
	}

	return m_tmp ;
}

gallery_dl::gallery_dlFilter::~gallery_dlFilter()
{
}

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

#ifndef UTIL_H
#define UTIL_H

#include <QString>
#include <QByteArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QProcess>
#include <QThread>
#include <QTimer>
#include <QFile>
#include <QApplication>

#include <QtNetwork/QLocalServer>
#include <QtNetwork/QLocalSocket>

#include <functional>
#include <memory>
#include <type_traits>

namespace util {

namespace types
{
	#if __cplusplus >= 201703L
		template<typename Function,typename ... Args>
		using result_of = std::invoke_result_t<Function,Args ...> ;
	#else
		template<typename Function,typename ... Args>
		using result_of = std::result_of_t<Function(Args ...)> ;
	#endif

	template< typename T >
	struct type_identity{
		using type = T ;
	} ;

	template< typename T >
	using type_identity_t = typename type_identity< T >::type ;

	template<typename ReturnType,typename Function,typename ... Args>
	using imp = std::enable_if_t<std::is_same<result_of<Function,Args...>,ReturnType>::value,int> ;

	template<typename ReturnType,typename Function,typename ... Args>
	using has_same_return_type = std::enable_if_t<std::is_same<result_of<Function,Args...>,ReturnType>::value,int> ;

	template<typename Function,typename ... Args>
	using has_argument = imp<result_of<Function,Args...>,Function,Args...> ;

	template<typename Function>
	using has_no_argument = imp<result_of<Function>,Function> ;

	template<typename Function,typename ... Args>
	using has_void_return_type = has_same_return_type<void,Function,Args...> ;

	template<typename Function,typename ... Args>
	using has_bool_return_type = has_same_return_type<bool,Function,Args...> ;

	template<typename Function,typename ... Args>
	using has_non_void_return_type = std::enable_if_t<!std::is_void<result_of<Function,Args...>>::value,int> ;
}

template< typename T >
typename std::add_const<T>::type& asConst( T& t )
{
	return t ;
}

template< typename T >
void asConst( const T&& ) = delete ;

static inline QStringList splitPreserveQuotes( const QString& e )
{
#if QT_VERSION < QT_VERSION_CHECK( 5,15,0 )
	QStringList args ;
	QString tmp ;
	int quoteCount = 0 ;
	bool inQuote = false ;

	for( int i = 0 ; i < e.size() ; ++i ) {

		const auto& s = e.at( i ) ;

		if( s == '"' ){

			quoteCount++ ;

			if( quoteCount == 3 ) {

				quoteCount = 0 ;
				tmp.append( s ) ;
			}

			continue ;
		}

		if( quoteCount ){

			if( quoteCount == 1 ){

				inQuote = !inQuote ;
			}

			quoteCount = 0 ;
		}

		if( !inQuote && s.isSpace() ){

			if( !tmp.isEmpty() ){

				args.append( tmp ) ;
				tmp.clear() ;
			}
		}else{
			tmp.append( s ) ;
		}
	}

	if( !tmp.isEmpty() ){

		args.append( tmp ) ;
	}

	return args ;
#else
	return QProcess::splitCommand( e ) ;
#endif
}

static inline QStringList split( const QString& e,char token,bool skipEmptyParts )
{
	if( skipEmptyParts ){
		#if QT_VERSION < QT_VERSION_CHECK( 5,15,0 )
			return e.split( token,QString::SkipEmptyParts ) ;
		#else
			return e.split( token,Qt::SkipEmptyParts ) ;
		#endif
	}else{
		return e.split( token ) ;
	}
}

static inline QStringList split( const QString& e,const char * token )
{
#if QT_VERSION < QT_VERSION_CHECK( 5,15,0 )
	return e.split( token,QString::SkipEmptyParts ) ;
#else
	return e.split( token,Qt::SkipEmptyParts ) ;
#endif
}

static inline QList< QByteArray > split( const QByteArray& e,char token )
{
	auto m = e.split( token ) ;

	m.removeAll( "" ) ;

	return m ;
}

static inline QList< QByteArray > split( const QByteArray& e,QChar token )
{
	auto m = e.split( token.toLatin1() ) ;

	m.removeAll( "" ) ;

	return m ;
}

template< typename List,typename Joiner >
static inline auto join( const List& list,typename List::size_type startPosition,const Joiner& m )
{
	auto s = list[ startPosition ] ;

	for( auto i = startPosition + 1 ; i < list.size() ; i++ ){

		s += m + list[ i ] ;
	}

	return s ;
}

class Json
{
public:
	Json()
	{
	}
	Json( const QByteArray& data ) :
		m_doc( QJsonDocument::fromJson( data,&m_error ) )
	{
	}
	Json( const QJsonObject& obj ) :
		m_doc( obj )
	{
		m_error.error = QJsonParseError::NoError ;
	}
	QByteArray toData() const
	{
		return m_doc.toJson() ;
	}
	Json( QJsonParseError error ) :
		m_error( std::move( error ) )
	{
	}
	Json( QJsonDocument doc ) : m_doc( std::move( doc ) )
	{
		m_error.error = QJsonParseError::NoError ;
	}
	const QJsonDocument& doc() const
	{
		return m_doc ;
	}
	QString errorString() const
	{
		return m_error.errorString() ;
	}
	operator bool() const
	{
		return m_error.error == QJsonParseError::NoError ;
	}
private:
	QJsonParseError m_error ;
	QJsonDocument m_doc ;
} ;

template< typename S >
class storage
{
public:
	storage() = default;
	storage( const storage& s )
	{
		this->set( s.get() ) ;
	}
	storage( storage& s )
	{
		this->set( s.get() ) ;
	}
	storage( storage&& s )
	{
		this->set( std::move( s.get() ) ) ;
	}
	template< typename ... T >
	storage( T&& ... t )
	{
		this->set( std::forward< T >( t ) ... ) ;
	}
	storage& operator=( const storage& s )
	{
		return this->set( s.get() ) ;
	}
	storage& operator=( storage& s )
	{
		return this->set( s.get() ) ;
	}
	storage& operator=( storage&& s )
	{
		return this->set( std::move( s.get() ) ) ;
	}
	template< typename T >
	storage& operator=( T&& t )
	{
		return this->set( std::forward< T >( t ) ) ;
	}
	template< typename ... T >
	S& put( T&& ... t )
	{
		return this->set( std::forward< T >( t ) ... ).get() ;
	}
	S& get()
	{
		return *m_pointer ;
	}
	S * operator->()
	{
		return m_pointer ;
	}
	const S& get() const
	{
		return *m_pointer ;
	}
	const S * operator->() const
	{
		return m_pointer ;
	}
	~storage()
	{
		if( m_pointer ){

			m_pointer->~S() ;
			m_pointer = nullptr ;
		}
	}
	bool created() const
	{
		return m_pointer ;
	}
private:
	template< typename ... T >
	storage& set( T&& ... t )
	{
		if( m_pointer ){

			m_pointer->~S() ;
		}

		m_pointer = new ( &m_storage ) S( std::forward< T >( t ) ... ) ;
		return *this ;
	}
	S * m_pointer = nullptr ;
	#if __cplusplus >= 201703L
		alignas( S ) std::byte m_storage[ sizeof( S ) ] ;
	#else
		typename std::aligned_storage< sizeof( S ),alignof( S ) >::type m_storage ;
	#endif
};

template< typename T >
class result
{
public:
	result() = default;
	template< typename ... S >
	result( S&& ... e ) : m_value( std::forward< S >( e ) ... )
	{
	}
	const T * operator->() const
	{
		return m_value.operator->() ;
	}
	const T& value() const
	{
		return m_value.get() ;
	}
	const T& operator*() const
	{
		return this->value() ;
	}
	template< typename ... S >
	void set( S&& ... s )
	{
		m_value.put( std::forward< S >( s ) ... ) ;
	}
	T * operator->()
	{
		return m_value.operator->() ;
	}
	T& value()
	{
		return m_value.get() ;
	}
	T& operator*()
	{
		return this->value() ;
	}
	bool has_value() const
	{
		return m_value.created() ;
	}
	operator bool() const
	{
		return this->has_value() ;
	}
private:
	util::storage< T > m_value ;
} ;

template< typename T,std::enable_if_t< std::is_reference< T >::value,int > = 0 >
class result_ref
{
public:
	result_ref() : m_value( nullptr )
	{
	}
	result_ref( T e ) : m_value( std::addressof( e ) )
	{
	}
	typename std::remove_reference< T >::type * operator->() const
	{
		return m_value ;
	}
	T& value() const
	{
		return *m_value ;
	}
	T& operator*() const
	{
		return this->value() ;
	}
	bool has_value() const
	{
		return m_value ;
	}
	operator bool() const
	{
		return this->has_value() ;
	}
private:
	typename std::remove_reference< T >::type * m_value ;
} ;

/*
 * Function must take an int and must return bool
 */
template< typename Function,util::types::has_bool_return_type<Function,int > = 0 >
void Timer( int interval,Function&& function )
{
	class Timer{
	public:
		Timer( int interval,Function&& function ) :
			m_function( std::forward< Function >( function ) )
		{
			auto timer = new QTimer() ;

			QObject::connect( timer,&QTimer::timeout,[ timer,this ](){

				m_counter++ ;

				if( m_function( m_counter ) ){

					timer->stop() ;

					timer->deleteLater() ;

					delete this ;
				}
			} ) ;

			timer->start( interval ) ;
		}
	private:
		int m_counter = 0 ;
		Function m_function ;
	} ;

	new Timer( interval,std::forward< Function >( function ) ) ;
}

/*
 * Function must takes no argument and will be called once when the interval pass
 */
template< typename Function,util::types::has_no_argument< Function > = 0 >
void Timer( int interval,Function&& function )
{
	util::Timer( interval,[ function = std::forward< Function >( function ) ]( int s ){

		Q_UNUSED( s )

		function() ;

		return true ;
	} ) ;
}

class version
{
public:
	version()
	{
	}
	template< typename T >
	version( const T& e )
	{
		auto s = util::split( e,'.',true ) ;

		auto m = s.size() ;

		try{
			if( m == 1 ){

				m_major = this->fromString( s.at( 0 ) ) ;

			}else if( m == 2 ){

				m_major = this->fromString( s.at( 0 ) ) ;
				m_minor = this->fromString( s.at( 1 ) ) ;

			}else if( m == 3 ){

				m_major = this->fromString( s.at( 0 ) ) ;
				m_minor = this->fromString( s.at( 1 ) ) ;
				m_patch = this->fromString( s.at( 2 ) ) ;

			}else if( m > 3 ){

				m_major   = this->fromString( s.at( 0 ) ) ;
				m_minor   = this->fromString( s.at( 1 ) ) ;
				m_patch   = this->fromString( s.at( 2 ) ) ;
				m_gitDate = this->fromString( s.at( 3 ) ) ;
			}

			m_valid = true ;

		}catch( ... ){

			m_valid = false ;
		}
	}
	bool valid() const
	{
		return m_valid ;
	}
	bool operator==( const version& other ) const
	{
		return m_major == other.m_major && m_minor == other.m_minor && m_patch == other.m_patch && m_gitDate == other.m_gitDate ;
	}
	bool operator<( const version& other ) const
	{
		if( m_major < other.m_major ){

			return true ;

		}else if( m_major == other.m_major ){

			if( m_minor < other.m_minor ){

				return true ;

			}else if( m_minor == other.m_minor ){

				if( m_patch < other.m_patch ){

					return true ;

				}else if( m_patch == other.m_patch ){

					return m_gitDate < other.m_gitDate ;
				}
			}
		}

		return false ;
	}
	/*
	 * a != b equal to !(a == b)
	 * a <= b equal to (a < b) || (a == b)
	 * a >= b equal to !(a < b)
	 * a > b  equal to !(a <= b)
	 */
	bool operator>=( const version& other ) const
	{
		return !( *this < other ) ;
	}
	bool operator<=( const version& other ) const
	{
		return ( *this < other ) || ( *this == other ) ;
	}
	bool operator!=( const version& other ) const
	{
		return !( *this == other ) ;
	}
	bool operator>( const version& other ) const
	{
		return !( *this <= other ) ;
	}
	QString toString() const
	{
		auto a = QString::number( m_major ) ;
		auto b = QString::number( m_minor ) ;
		auto c = QString::number( m_patch ) ;
		auto d = QString::number( m_gitDate ) ;

		if( d == "0" ){

			return a + "." + b + "." + c ;
		}else{
			return a + "." + b + "." + c + "." + d ;
		}
	}
	version move()
	{
		return std::move( *this ) ;
	}
private:
	qulonglong fromString( const QString& e )
	{
		bool ok ;

		auto m = e.toULongLong( &ok ) ;

		if( !ok ){

			throw -1 ;
		}

		return m ;
	}
	bool m_valid = false ;
	qulonglong m_major = 0 ;
	qulonglong m_minor = 0 ;
	qulonglong m_patch = 0 ;
	qulonglong m_gitDate = 0 ;
};
}

#endif

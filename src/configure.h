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
#ifndef CONFIGURE_H
#define CONFIGURE_H

#include "settings.h"
#include "utility.h"
#include "context.hpp"
#include "networkAccess.h"
#include "tableWidget.h"

#include <QMenu>

class tabManager ;

class configure : public QObject
{
        Q_OBJECT
public:
	static QString defaultDownloadOption() ;
	configure( const Context& ) ;
	void keyPressed( utility::mainWindowKeyCombo ) ;
	void init_done() ;
	void enableAll() ;
	void textAlignmentChanged( Qt::LayoutDirection ) ;
	void disableAll() ;
	void resetMenu() ;
	void exiting() ;
	void retranslateUi() ;
	void setUpdateMenu() ;
	void downloadFromGitHub( const engines::Iterator& ) ;
	void tabEntered() ;
	void tabExited() ;
	void setVisibilityEditConfigFeature( bool ) ;
	void updateEnginesList( const QStringList& e ) ;
	QString engineDefaultDownloadOptions( const QString& ) ;
	void engineSetDefaultDownloadOptions( const engines::engine& ) ;
	QString optionsTranslated( const QString& ) ;
	QString getEngineNameFromUrlManager( const QString& ) ;
	void setDownloadOptions( int row,tableWidget& table ) ;
	struct presetEntry
	{
		presetEntry( const QString& ui,const QString& op,const QString& wb ) ;
		const QString& uiName ;
		const QString& options ;
		const QString& website ;
		QString websiteTranslated ;
		QString uiNameTranslated ;
	} ;
	template< typename Function >
	void presetOptionsForEach( const Function& function )
	{
		m_presetOptions.forEach( function ) ;
	}
private:
	class presetOptions
	{
	public:
		presetOptions( const Context&,settings& ) ;
		~presetOptions() ;
		void clear() ;
		void setDefaults() ;
		QString optionsTranslated( const QString& ) ;
		void add( const QString& uiName,const QString& options,const QString& website ) ;
		template< typename Function >
		void forEach( const Function& function )
		{
			for( const auto& it : util::asConst( m_array ) ){

				auto obj = it.toObject() ;

				if( !obj.isEmpty() ){

					auto a = obj.value( "uiName" ).toString() ;
					auto b = obj.value( "options" ).toString() ;
					auto c = obj.value( "website" ).toString() ;

					if( !a.isEmpty() && !b.isEmpty() ){

						function( configure::presetEntry( a,b,c ) ) ;
					}
				}
			}
		}
	private:
		QByteArray defaultData() ;
		QString m_path ;
		QJsonArray m_array ;
	};
	class downloadDefaultOptions
	{
	public:
		struct opts
		{
			const QString& downloadOptions ;
			const QString& engine ;
			const QString& url ;
		} ;
		class qOpts
		{
		public:
			qOpts( QJsonObject e ) : m_qJsonObject( std::move( e ) )
			{
			}
			qOpts( const QJsonArray& e,int m ) :
				m_qJsonObject( e[ m ].toObject() )
			{
			}
			QString engineName() const
			{
				return this->engineName( this->toJson() ) ;
			}
			QString opts() const
			{
				return this->options( this->toJson() ) ;
			}
			bool inUse() const
			{
				return this->inUse( this->toJson() ) ;
			}
			const char * inUseStr() const
			{
				return this->inUse() ? "yes" : "no" ;
			}
			const QJsonObject& toJson() const
			{
				return m_qJsonObject ;
			}
			QJsonObject setAsDefault( const QString& e ) const
			{
				auto o = this->toJson() ;

				o.insert( "engineName",this->engineName() ) ;
				o.insert( "options",this->opts() ) ;
				o.insert( "default",e ) ;

				return o ;
			}
			QJsonObject replaceOptions( const QString& e ) const
			{
				auto o = this->toJson() ;

				o.insert( "engineName",this->engineName() ) ;
				o.insert( "options",e ) ;
				o.insert( "default",this->inUseStr() ) ;

				return o ;
			}
			static qOpts obj( const QString& options,const QString& engineName )
			{
				QJsonObject obj ;

				obj.insert( "default","yes" ) ;
				obj.insert( "options",options ) ;
				obj.insert( "engineName",engineName ) ;

				return obj ;
			}
			static QString engineName( const QJsonObject& m )
			{
				return m.value( "engineName" ).toString() ;
			}
			static QString options( const QJsonObject& m )
			{
				return m.value( "options" ).toString() ;
			}
			static bool inUse( const QJsonObject& m )
			{
				return m.value( "default" ).toString() == "yes" ;
			}
			qOpts move()
			{
				return std::move( *this ) ;
			}
		private:
			QJsonObject m_qJsonObject ;
		} ;
		downloadDefaultOptions( const Context&,const QString& ) ;
		void save() ;
		bool isEmpty( const QString& ) ;
		void replace( const QString& engineName,const QString& oldOptions,const QString& newOptions ) ;
		QJsonObject addOpt( const QString& engineName,const QString& options ) ;
		QJsonObject add( const QString& url,const QString& opts,const QString& engineName ) ;
		QJsonObject add( const configure::downloadDefaultOptions::opts& ) ;
		void remove( const QJsonObject& ) ;
		void removeDownloadDefaultOptions( const QJsonObject& ) ;
		void removeAll( const QString& ) ;
		void setAsDefault( const QJsonObject& ) ;

		template< typename Function,
			  typename std::enable_if<std::is_same<util::types::result_of<Function,qOpts>,bool>::value,int >::type = 0 >
		void forEach( const Function& function )
		{
			for( const auto& it : util::asConst( m_array ) ){

				auto obj = it.toObject() ;

				if( !obj.isEmpty() ){

					if( function( std::move( obj ) ) ){

						break ;
					}
				}
			}
		}

		template< typename Function,
			  typename std::enable_if<std::is_same<util::types::result_of<Function,const configure::downloadDefaultOptions::opts&,QJsonObject>,bool>::value,int >::type = 0 >
		void forEach( const Function& function )
		{
			for( const auto& it : util::asConst( m_array ) ){

				auto obj = it.toObject() ;

				if( !obj.isEmpty() ){

					auto a = obj.value( "downloadOption" ).toString() ;
					auto b = obj.value( "engine" ).toString() ;
					auto c = obj.value( "url" ).toString() ;

					if( function( { a,b,c },std::move( obj ) ) ){

						break ;
					}
				}
			}
		}
		class urlType
		{
		public:
			enum class type{ contain = 1,startsWith = 2,endsWith = 3 } ;
			urlType( const QString& ) ;
			type getType() const
			{
				return m_type ;
			}
			QString getUrl() const
			{
				return m_url ;
			}
		private:
			type m_type ;
			QString m_url ;
		} ;
		QString engineNameFromUrl( const QString& u )
		{
			QString engineName ;

			this->forEach( [ & ]( const opts& e,const QJsonObject& ){

				urlType uu( e.url ) ;

				auto t = uu.getType() ;

				const auto& url = uu.getUrl() ;

				if( t == urlType::type::startsWith && u.startsWith( url ) ){

					engineName = e.engine ;

					return true ;

				}else if( t == urlType::type::endsWith && u.endsWith( url ) ){

					engineName = e.engine ;

					return true ;

				}else if( t == urlType::type::contain && u.contains( url ) ){

					engineName = e.engine ;

					return true ;
				}else {
					return false ;
				}
			} ) ;

			return engineName ;
		}
		void setDownloadOptions( int row,tableWidget& table )
		{
			this->forEach( [ & ]( const opts& e,const QJsonObject& ){

				const auto& rowUrl = table.url( row ) ;

				urlType uu( e.url ) ;

				auto t          = uu.getType() ;
				const auto& url = uu.getUrl() ;
				const auto& eng = e.engine ;
				const auto& opt = e.downloadOptions ;

				auto dlo = tableWidget::type::DownloadOptions ;
				auto en  = tableWidget::type::EngineName ;

				if( t == urlType::type::contain && rowUrl.contains( url ) ){

					if( !opt.isEmpty() ){

						table.setDownloadingOptions( dlo,row,opt ) ;
					}

					if( !eng.isEmpty() ){

						table.setDownloadingOptions( en,row,eng ) ;
					}

					return true ;

				}else if( t == urlType::type::startsWith && rowUrl.startsWith( url ) ){

					if( !opt.isEmpty() ){

						table.setDownloadingOptions( dlo,row,opt ) ;
					}

					if( !eng.isEmpty() ){

						table.setDownloadingOptions( en,row,eng ) ;
					}

					return true ;

				}else if( t == urlType::type::endsWith && rowUrl.endsWith( url ) ){

					if( !opt.isEmpty() ){

						table.setDownloadingOptions( dlo,row,opt ) ;
					}

					if( !eng.isEmpty() ){

						table.setDownloadingOptions( en,row,eng ) ;
					}

					return true ;
				}else{
					return false ;
				}
			} ) ;
		}
	private:
		QString m_path ;
		QJsonArray m_array ;
	};

	QMenu * addExtenion() ;
	QMenu * removeExtenion() ;
	void addEngine( const QByteArray&,const QString& ) ;
	void saveOptions() ;
	enum class engineOptions{ url,options,both } ;
	void setEngineOptions( const QString&,engineOptions ) ;
	void savePresetOptions() ;
	void showOptions() ;
	void populateOptionsTable( const engines::engine&,int = -1 ) ;
	void updateProxySettings( settings::proxySettings::Type ) ;
	void confirmResetMakeVisible( bool ) ;
	void setCookieSourceLabel( bool ) ;
	void downloadExtension( const QString& ) ;
	QString setUrl( const QString& e );
	const Context& m_ctx ;
	settings& m_settings ;
	Ui::MainWindow& m_ui ;
	QWidget& m_mainWindow ;
	tabManager& m_tabManager ;
	engines& m_engines ;

	class String
	{
	public:
		String()
		{
		}
		String( QString e ) : m_string( std::move( e ) )
		{
		}
		const QString& value() const
		{
			return m_string ;
		}
		QString& value()
		{
			return m_string ;
		}
	private:
		QString m_string ;
	};

	tableMiniWidget< String,3 > m_tablePresetOptions ;
	tableMiniWidget< QJsonObject,2 > m_tableUrlToDefaultEngine ;
	tableMiniWidget< QJsonObject,2 > m_tableDefaultDownloadOptions ;
	QMenu m_menu ;
	presetOptions m_presetOptions ;
	downloadDefaultOptions m_downloadDefaultOptions ;
	downloadDefaultOptions m_downloadEngineDefaultOptions ;
};

#endif

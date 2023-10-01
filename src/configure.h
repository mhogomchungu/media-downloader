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
	void init_done() ;
	void enableAll() ;
	void disableAll() ;
	void resetMenu() ;
	void exiting() ;
	void retranslateUi() ;
	void downloadFromGitHub( const engines::Iterator& ) ;
	void tabEntered() ;
	void tabExited() ;
	void updateEnginesList( const QStringList& e ) ;
	QString engineDefaultDownloadOptions( const QString& ) ;
	template< typename Function >
	void engineDefaultDownloadOptions( const QString& engineName,Function function )
	{
		QStringList options ;

		using mm = configure::downloadDefaultOptions::optsEngines ;

		m_downloadEngineDefaultOptions.forEach( [ & ]( const mm& opts,const QJsonObject& ){

			if( opts.engine == engineName ){

				options.append( opts.options ) ;
			}

			return false ;
		} ) ;

		QMenu m ;

		for( const auto& it : options ){

			m.addAction( it ) ;
		}

		connect( &m,&QMenu::triggered,[ &function ]( QAction * ac ){

			function( ac->text() ) ;
		} ) ;

		m.exec( QCursor::pos() ) ;
	}
	QString optionsTranslated( const QString& ) ;
	void setDownloadOptions( int row,tableWidget& table ) ;
	struct presetEntry
	{
		presetEntry( const QString& ui,const QString& op,const QString& wb ) ;
		const QString& uiName ;
		const QString& options ;
		const QString& website ;
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
			const QString& comparator ;
			const QString& downloadOptions ;
			const QString& engine ;
			const QString& url ;
		} ;
		struct optsEngines
		{
			const QString& inuse ;
			const QString& engine ;
			const QString& options ;
		} ;
		downloadDefaultOptions( const Context&,const QString& ) ;
		void save() ;
		bool isEmpty( const QString& ) ;
		QJsonObject addOpt( const QString&,const QString& engineName,const QString& options ) ;
		QJsonObject add( const QString& url,const QString& opts,const QString& engineName ) ;
		QJsonObject add( const configure::downloadDefaultOptions::opts& ) ;
		void remove( const QJsonObject& ) ;
		void removeAll( const QString& ) ;
		QJsonObject setAsDefault( const QJsonObject& ) ;

		template< typename Function,
			  typename std::enable_if<std::is_same<util::types::result_of<Function,const configure::downloadDefaultOptions::optsEngines&,QJsonObject>,bool>::value,int >::type = 0 >
		void forEach( const Function& function )
		{
			for( const auto& it : util::asConst( m_array ) ){

				auto obj = it.toObject() ;

				if( !obj.isEmpty() ){

					auto a = obj.value( "default" ).toString() ;
					auto b = obj.value( "engineName" ).toString() ;
					auto c = obj.value( "options" ).toString() ;

					if( function( { a,b,c },std::move( obj ) ) ){

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

					auto a = obj.value( "comparator" ).toString() ;
					auto b = obj.value( "downloadOption" ).toString() ;
					auto c = obj.value( "engine" ).toString() ;
					auto d = obj.value( "url" ).toString() ;

					if( function( { a,b,c,d },std::move( obj ) ) ){

						break ;
					}
				}
			}
		}
		void setDownloadOptions( int row,tableWidget& table )
		{
			this->forEach( [ & ]( const opts& e,const QJsonObject& ){

				const auto& rowUrl = table.url( row ) ;

				const auto& cmp = e.comparator ;
				const auto& url = e.url ;
				const auto& eng = e.engine ;
				const auto& opt = e.downloadOptions ;

				auto dlo = tableWidget::type::DownloadOptions ;
				auto en  = tableWidget::type::EngineName ;

				if( cmp == "contains" && rowUrl.contains( url ) ){

					if( !opt.isEmpty() ){

						table.setDownloadingOptions( dlo,row,opt ) ;
					}

					if( !eng.isEmpty() ){

						table.setDownloadingOptions( en,row,eng ) ;
					}

					return true ;

				}else if( cmp == "startsWith" && rowUrl.startsWith( url ) ){

					if( !opt.isEmpty() ){

						table.setDownloadingOptions( dlo,row,opt ) ;
					}

					if( !eng.isEmpty() ){

						table.setDownloadingOptions( en,row,eng ) ;
					}

					return true ;

				}else if( cmp == "endsWith" && rowUrl.endsWith( url ) ){

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

	void saveOptions() ;
	enum class engineOptions{ url,options,both } ;
	void setEngineOptions( const QString&,engineOptions ) ;
	void savePresetOptions() ;
	void showOptions() ;
	void populateOptionsTable( const util::result_ref< const engines::engine& >& ) ;
	void updateProxySettings( settings::proxySettings::Type ) ;
	const Context& m_ctx ;
	settings& m_settings ;
	Ui::MainWindow& m_ui ;
	QWidget& m_mainWindow ;
	tabManager& m_tabManager ;
	engines& m_engines ;

	tableMiniWidget< QString > m_tablePresetOptions ;
	tableMiniWidget< QJsonObject > m_tableUrlToDefaultEngine ;
	tableMiniWidget< QJsonObject > m_tableDefaultDownloadOptions ;
	QMenu m_menu ;
	presetOptions m_presetOptions ;
	downloadDefaultOptions m_downloadDefaultOptions ;
	downloadDefaultOptions m_downloadEngineDefaultOptions ;
};

#endif

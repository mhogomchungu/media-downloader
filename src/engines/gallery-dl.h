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

#include <QByteArray>

#include "../engines.h"

class settings ;

class gallery_dl : public engines::engine::baseEngine
{
public:
	static const char * testData() ;

	~gallery_dl() override ;
	gallery_dl( const engines&,const engines::engine&,QJsonObject& ) ;

	class gallery_dlFilter : public engines::engine::baseEngine::filter
	{
	public:
		gallery_dlFilter( settings&,const engines::engine&,int ) ;

		const QByteArray& operator()( Logger::Data& e ) override ;

		~gallery_dlFilter() override ;
	private:
		QString m_speed ;
		QString m_downloaded ;
		QByteArray m_tmp ;
		QByteArray m_dir ;
		engines::engine::baseEngine::preProcessing m_preProcessing ;
	} ;

	util::Json parsePlayListData( const QByteArray& ) override ;

	engines::engine::baseEngine::DataFilter Filter( int ) override ;

	QString downloadFolder( const QString& ) override ;

	void openLocalFile( const engines::engine::baseEngine::localFile& ) override ;

	void runCommandOnDownloadedFile( const std::vector< QByteArray >& ) override ;

	void setProxySetting( QStringList&,const QString& ) override ;

	void updateDownLoadCmdOptions( const engines::engine::baseEngine::updateOpts&,bool ) override ;

	QString updateTextOnCompleteDownlod( const QString& uiText,
					     const QString& bkText,
					     const QString& downloadingOptions,
					     const engines::engine::baseEngine::finishedState& ) override ;
private:
};

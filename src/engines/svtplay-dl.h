/*
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

#include <QByteArray>

#include "../engines.h"

class settings ;

class svtplay_dl : public engines::engine::baseEngine
{
public:
	static const char * testData() ;

	~svtplay_dl() override ;
	svtplay_dl( const engines&,const engines::engine&,QJsonObject& ) ;

	class svtplay_dlFilter : public engines::engine::baseEngine::filter
	{
	public:
		svtplay_dlFilter( settings&,const engines::engine&,int ) ;

		const QByteArray& operator()( Logger::Data& e ) override ;

		~svtplay_dlFilter() override ;
	private:
		QByteArray m_tmp ;
		QByteArray m_fileName ;
		engines::engine::baseEngine::preProcessing m_preProcessing ;
	} ;

	engines::engine::baseEngine::FilterOutPut filterOutput( int ) override ;

	QString updateCmdPath( const QString& ) override ;

	engines::engine::baseEngine::onlineVersion versionInfoFromGithub( const QByteArray& e ) override ;

	engines::metadata parseJsonDataFromGitHub( const QJsonDocument& ) override ;

	static QString downloadUrl() ;

	void updateOutPutChannel( QProcess::ProcessChannel& ) const override ;

	void updateDownLoadCmdOptions( const engines::engine::baseEngine::updateOpts&,
				       bool,
				       const QStringList& ) override ;

	QStringList horizontalHeaderLabels() const override ;

	engines::engine::baseEngine::optionsEnvironment setProxySetting( QStringList&,const QString& ) override ;

	std::vector< engines::engine::baseEngine::mediaInfo > mediaProperties( Logger&,const QByteArray& ) override ;

	const QProcessEnvironment& processEnvironment() const override ;

	engines::engine::baseEngine::DataFilter Filter( int ) override ;

	QString updateTextOnCompleteDownlod( const QString& uiText,
					     const QString& bkText,
					     const QString& downloadingOptions,
					     const QString& tabName,
					     const engines::engine::baseEngine::finishedState& ) override ;
private:
	QProcessEnvironment m_processEnvironment ;
};

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

class svtplay_dl : public engines::engine::functions
{
public:
	~svtplay_dl() override ;
	svtplay_dl( const engines&,const engines::engine&,QJsonObject& ) ;

	class svtplay_dlFilter : public engines::engine::functions::filter
	{
	public:
		svtplay_dlFilter( const QString&,settings&,const engines::engine&,int ) ;

		const QByteArray& operator()( const Logger::Data& e ) override ;

		~svtplay_dlFilter() override ;
	private:
		QByteArray m_tmp ;
		engines::engine::functions::preProcessing m_preProcessing ;
		int m_processId ;
	} ;

	void updateOutPutChannel( QProcess::ProcessChannel& ) const override ;

	QStringList horizontalHeaderLabels() const override ;

	std::vector< QStringList > mediaProperties( const QByteArray& ) override ;

	const QProcessEnvironment& processEnvironment() const override ;

	engines::engine::functions::DataFilter Filter( int,const QString& ) override ;

	QString updateTextOnCompleteDownlod( const QString& uiText,
					     const QString& bkText,
					     const QString& downloadingOptions,
					     const engines::engine::functions::finishedState& ) override ;

	void updateDownLoadCmdOptions( const engines::engine::functions::updateOpts& ) override ;
private:
	QProcessEnvironment m_processEnvironment ;
};

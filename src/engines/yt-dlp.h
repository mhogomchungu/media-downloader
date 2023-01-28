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
#include <QJsonArray>

#include "../engines.h"

class yt_dlp : public engines::engine::functions
{
public:
	static const char * testData() ;

	~yt_dlp() override ;

	class youtube_dlFilter : public engines::engine::functions::filter
	{
	public:
		youtube_dlFilter( int processId,const QString&,const engines::engine&,bool ) ;

		const QByteArray& operator()( const Logger::Data& e ) override ;

		~youtube_dlFilter() override ;
	private:
		const QByteArray& youtubedlOutput( const Logger::Data& ) ;
		const QByteArray& ytdlpOutput( const Logger::Data& ) ;
		bool m_likeYtdlp ;
		engines::engine::functions::preProcessing m_preProcessing ;
		engines::engine::functions::postProcessing m_postProcessing ;
		QByteArray m_tmp ;
		QByteArray m_fileName ;
	} ;

	std::vector< engines::engine::functions::mediaInfo > mediaProperties( const QByteArray& ) override ;

	std::vector< engines::engine::functions::mediaInfo > mediaProperties( const QJsonArray& ) override ;

	bool breakShowListIfContains( const QStringList& ) override ;

	bool supportsShowingComments() override ;

	bool updateVersionInfo() override ;

	bool likeYtdlp() override ;

	void updateLocalOptions( QStringList& ) override ;

	void setTextEncondig( const QString&,QStringList& ) override ;

	engines::engine::functions::DataFilter Filter( int,const QString& ) override ;

	void runCommandOnDownloadedFile( const QString&,const QString& ) override ;

	QString updateTextOnCompleteDownlod( const QString& uiText,
					     const QString& bkText,
					     const QString& downloadingOptions,
					     const engines::engine::functions::finishedState& ) override ;

	void updateDownLoadCmdOptions( const engines::engine::functions::updateOpts& ) override ;

	void updateGetPlaylistCmdOptions( QStringList& ) override ;

	void updateCmdOptions( QStringList& ) override ;

	static QJsonObject init( const QString& name,
				 const QString& configFileName,
				 Logger& logger,
				 const engines::enginePaths& enginePath ) ;

	yt_dlp( const engines&,
		const engines::engine&,
		QJsonObject&,
		Logger& logger,
		const engines::enginePaths&,
		const util::version& ) ;
private:
	const engines::engine& m_engine ;
	QJsonArray m_objs ;
	const util::version& m_version ;
	bool m_likeYtdlp ;
	const util::version m_supportsLazyPlaylist ;
	const util::version m_supportsCompactOption ;
};

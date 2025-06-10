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

class settings ;

class yt_dlp : public engines::engine::baseEngine
{
public:
	static const char * testYtDlpMetadata() ;
	static const char * testYtDlpPlayList() ;
	static const char * testYtDlp() ;
	static const char * testFfmpeg() ;
	static void checkIfBinaryExist( const QString&,const QString& ) ;

	static QStringList jsonNoFormatsArgumentList() ;

	~yt_dlp() override ;

	class yt_dlplFilter : public engines::engine::baseEngine::filter
	{
	public:
		yt_dlplFilter( int,const engines::engine&,yt_dlp& ) ;

		const QByteArray& operator()( Logger::Data& e ) override ;

		~yt_dlplFilter() override ;
	private:
		QByteArray fileName() ;

		const char * compatYear() ;

		const QByteArray& parseOutput( const Logger::Data::QByteArrayList& ) ;

		void setFileName( const QByteArray& ) ;
		engines::engine::baseEngine::preProcessing m_preProcessing ;
		engines::engine::baseEngine::postProcessing m_postProcessing ;

		const engines::engine& m_engine ;
		QByteArray m_tmp ;
		bool m_mergeSeen = false ;
		std::vector< QByteArray > m_fileNames ;
		yt_dlp& m_parent ;
	} ;

	engines::engine::baseEngine::FilterOutPut filterOutput() override ;

	std::vector< engines::engine::baseEngine::mediaInfo > mediaProperties( Logger&,const QByteArray& ) override ;

	std::vector< engines::engine::baseEngine::mediaInfo > mediaProperties( Logger&,const QJsonArray& ) override ;

	bool supportsShowingComments() override ;

	bool updateVersionInfo() override ;

	void updateLocalOptions( QStringList& ) override ;

	void setProxySetting( QStringList&,const QString& ) override ;

	void setTextEncondig( const QString&,QStringList& ) override ;

	const QProcessEnvironment& processEnvironment() const override ;

	engines::engine::baseEngine::DataFilter Filter( int ) override ;

	QString updateTextOnCompleteDownlod( const QString& uiText,
					     const QString& bkText,
					     const QString& downloadingOptions,
					     const engines::engine::baseEngine::finishedState& ) override ;

	void updateDownLoadCmdOptions( const engines::engine::baseEngine::updateOpts&,bool ) override ;

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
		settings& ) ;
private:
	std::vector< engines::engine::baseEngine::mediaInfo >
	mediaProperties( Logger&,const QJsonArray&,const QJsonObject& ) ;

	void appendCompatOption( QStringList& ) ;
	const engines::engine& m_engine ;
	QJsonArray m_objs ;
	settings * m_settings ;
	QProcessEnvironment m_processEnvironment ;
};

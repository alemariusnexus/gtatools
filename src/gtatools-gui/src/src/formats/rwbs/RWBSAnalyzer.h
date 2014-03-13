/*
	Copyright 2010-2014 David "Alemarius Nexus" Lerch

	This file is part of gtatools-gui.

	gtatools-gui is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	gtatools-gui is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with gtatools-gui.  If not, see <http://www.gnu.org/licenses/>.

	Additional permissions are granted, which are listed in the file
	GPLADDITIONS.
 */

#ifndef RWBSANALYZER_H_
#define RWBSANALYZER_H_

#include <QtCore/QObject>
#include <QtGui/QWidget>
#include <QtGui/QTextEdit>
#include <gtaformats/rwbs/RWSection.h>
#include <gta/script/luasys.h>



class RWBSAnalyzer : public QObject
{
	Q_OBJECT

public:
	static RWBSAnalyzer* load(const File& scriptFile);

public:
	QString getName() const { return name; }
	void notifySectionChanged(RWSection* sect);
	void notifyContentsChanged(QList<RWSection*> rootSects);
	QString getAnalysisResults();
	void notifyCurrentSectionChanged(RWSection* sect);

private:
	RWBSAnalyzer(lua_State* lua, const QString& name);

private:
	lua_State* lua;
	QString name;
	QString lastResults;
	bool resultsValid;
};

#endif /* RWBSANALYZER_H_ */

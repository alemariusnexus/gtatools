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

#ifndef RWBSANALYZERWIDGET_H_
#define RWBSANALYZERWIDGET_H_

#include <ui_RWBSAnalyzerWidget.h>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QTextEdit>
#include "RWBSAnalyzer.h"
#include "../../System.h"



class RWBSAnalyzerWidget : public QWidget
{
	Q_OBJECT

private:
	struct AnalyzerEntry
	{
		QTextEdit* textEdit;
	};

public:
	RWBSAnalyzerWidget(QWidget* parent = NULL);
	void setRootSections(QList<RWSection*> rootSects);
	void updateSection(RWSection* sect);
	void setCurrentSection(RWSection* sect);

private:
	void reloadReport(RWBSAnalyzer* analyzer);

private slots:
	void systemQuerySent(const SystemQuery& query, QList<SystemQueryResult>& results);

private:
	Ui_RWBSAnalyzerWidget ui;
	QList<RWBSAnalyzer*> analyzers;
	QMap<RWBSAnalyzer*, AnalyzerEntry> analyzerEntries;
};

#endif /* RWBSANALYZERWIDGET_H_ */

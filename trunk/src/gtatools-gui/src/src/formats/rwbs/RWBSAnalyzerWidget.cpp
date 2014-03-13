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

#include "RWBSAnalyzerWidget.h"
#include "../../System.h"



RWBSAnalyzerWidget::RWBSAnalyzerWidget(QWidget* parent)
		: QWidget(parent)
{
	ui.setupUi(this);

	File scriptRoot("/home/alemariusnexus/luatest");

	for (File child : scriptRoot.getChildren()) {
		if (child.getPath().getExtension() == CString("lua")) {
			RWBSAnalyzer* analyzer = RWBSAnalyzer::load(child);

			if (analyzer) {
				analyzers << analyzer;
			}
		}
	}

	for (QList<RWBSAnalyzer*>::iterator it = analyzers.begin() ; it != analyzers.end() ; it++) {
		RWBSAnalyzer* ana = *it;

		AnalyzerEntry entry;
		entry.textEdit = new QTextEdit(ui.tabber);
		entry.textEdit->setReadOnly(true);

		ui.tabber->addTab(entry.textEdit, ana->getName());

		analyzerEntries[ana] = entry;
	}

	System* sys = System::getInstance();

	connect(sys, SIGNAL(systemQuerySent(const SystemQuery&, QList<SystemQueryResult>&)), this,
			SLOT(systemQuerySent(const SystemQuery&, QList<SystemQueryResult>&)));
}


void RWBSAnalyzerWidget::setRootSections(QList<RWSection*> rootSects)
{
	for (QList<RWBSAnalyzer*>::iterator it = analyzers.begin() ; it != analyzers.end() ; it++) {
		RWBSAnalyzer* analyzer = *it;
		analyzer->notifyContentsChanged(rootSects);
		reloadReport(analyzer);
	}
}


void RWBSAnalyzerWidget::updateSection(RWSection* sect)
{
	for (QList<RWBSAnalyzer*>::iterator it = analyzers.begin() ; it != analyzers.end() ; it++) {
		RWBSAnalyzer* analyzer = *it;
		analyzer->notifySectionChanged(sect);
		reloadReport(analyzer);
	}
}


void RWBSAnalyzerWidget::setCurrentSection(RWSection* sect)
{
	for (QList<RWBSAnalyzer*>::iterator it = analyzers.begin() ; it != analyzers.end() ; it++) {
		RWBSAnalyzer* analyzer = *it;
		analyzer->notifyCurrentSectionChanged(sect);
		reloadReport(analyzer);
	}
}


void RWBSAnalyzerWidget::systemQuerySent(const SystemQuery& query, QList<SystemQueryResult>& results)
{
	if (query.getName() == "RWBSAnalyzerSwitchRootSections") {
		uint nrs = query.getProperty("rootSectCount").toUInt();
		QList<RWSection*> rootSects;

		for (uint i = 0 ; i < nrs ; i++) {
			rootSects << (RWSection*) query.getProperty(QString("rootSect%1").arg(i)).value<void*>();
		}

		setRootSections(rootSects);
	} else if (query.getName() == "RWBSAnalyzerUpdateSection") {
		RWSection* sect = (RWSection*) query.getProperty("sect").value<void*>();
		updateSection(sect);
	} else if (query.getName() == "RWBSAnalyzerSetCurrentSection") {
		RWSection* sect = (RWSection*) query.getProperty("sect").value<void*>();
		setCurrentSection(sect);
	}
}


void RWBSAnalyzerWidget::reloadReport(RWBSAnalyzer* analyzer)
{
	AnalyzerEntry entry = analyzerEntries[analyzer];

	QString res = analyzer->getAnalysisResults();
	entry.textEdit->setHtml(res);
}

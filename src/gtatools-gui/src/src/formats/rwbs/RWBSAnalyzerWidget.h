#ifndef RWBSANALYZERWIDGET_H_
#define RWBSANALYZERWIDGET_H_

#include <ui_RWBSAnalyzerWidget.h>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtGui/QTextEdit>
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

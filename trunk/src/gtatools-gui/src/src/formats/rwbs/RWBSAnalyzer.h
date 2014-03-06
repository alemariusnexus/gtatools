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

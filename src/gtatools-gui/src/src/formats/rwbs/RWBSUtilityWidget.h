#ifndef RWBSUTILITYWIDGET_H_
#define RWBSUTILITYWIDGET_H_

#include "RWBSWidget.h"
#include "../../System.h"
#include "../../DisplayedFile.h"
#include "../../DefaultDisplayedEntityHandler.h"
#include <QtGui/QWidget>
#include <QtGui/QStackedWidget>
#include <QtCore/QMap>



class RWBSUtilityWidget : public QStackedWidget
{
	Q_OBJECT

private:
	struct DisplayedFileEntry
	{
		bool writable;
		DefaultDisplayedEntityHandler* handler;
		bool hasChanges;
		RWBSWidget* rwbsWidget;
	};

public:
	RWBSUtilityWidget(QWidget* parent = NULL);

private slots:
	void systemQuerySent(const SystemQuery& query, QList<SystemQueryResult>& results);
	void currentDataChanged();
	void updateCurrentFileEditableStatus();
	void fileSaveRequested(bool& success);
	void fileClosed();
	void currentEntityChanged(DisplayedEntity* oldEnt, DisplayedEntity* newEnt);
	void entityHasUnsavedChangesStateChanged(bool hasChanges);
	void entityUndoStackCleanStateChanged(bool clean);
	void sectionInserted(RWSection* sect);
	void sectionRemoved(RWSection* sect, RWSection* oldParent);
	void sectionUpdated(RWSection* sect, uint32_t oldID, uint32_t oldVersion, bool oldContainer);
	void sectionDataChanged(RWSection* sect);
	void currentSectionChanged(RWSection* oldSect, RWSection* newSect);

private:
	//void rememberChanges();
	bool applyChanges(DisplayedFile* dfile);
	void setCurrentFile(DisplayedFile* dfile);
	void registerFile(DisplayedFile* dfile, bool writable);
	bool save(DisplayedFile* dfile);

private:
	//RWBSWidget* rwbsWidget;
	QMap<DisplayedFile*, DisplayedFileEntry> openFiles;
	DisplayedFile* currentFile;
};

#endif /* RWBSUTILITYWIDGET_H_ */

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

#ifndef RWBSUTILITYWIDGET_H_
#define RWBSUTILITYWIDGET_H_

#include "RWBSWidget.h"
#include "../../System.h"
#include "../../DisplayedFile.h"
#include "../../DefaultDisplayedEntityHandler.h"
#include <QWidget>
#include <QStackedWidget>
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

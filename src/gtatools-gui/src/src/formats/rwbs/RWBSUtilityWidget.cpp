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

#include "RWBSUtilityWidget.h"
#include "RWBSGUIModule.h"
#include <gtaformats/img/IMGArchive.h>
#include <QVBoxLayout>
#include "../../System.h"



RWBSUtilityWidget::RWBSUtilityWidget(QWidget* parent)
		: QStackedWidget(parent), currentFile(NULL)
{
	System* sys = System::getInstance();

	// Widget 0 is a dummy widget when no RWBS file is currently displayed.
	addWidget(new QWidget(this));
	setCurrentIndex(0);

	connect(sys, SIGNAL(systemQuerySent(const SystemQuery&, QList<SystemQueryResult>&)), this,
			SLOT(systemQuerySent(const SystemQuery&, QList<SystemQueryResult>&)));
	connect(sys, SIGNAL(currentEntityChanged(DisplayedEntity*, DisplayedEntity*)), this,
			SLOT(currentEntityChanged(DisplayedEntity*, DisplayedEntity*)));
}


void RWBSUtilityWidget::systemQuerySent(const SystemQuery& query, QList<SystemQueryResult>& results)
{
	System* sys = System::getInstance();

	if (query.getName() == "RWBSRegisterFile") {
		DisplayedFile* dfile = (DisplayedFile*) query.getProperty("dfile").value<void*>();

		bool writable = query.getProperty("writable").toBool();

		registerFile(dfile, writable);
	} else if (query.getName() == "RWBSSaveFile") {
		DisplayedFile* dfile = (DisplayedFile*) query.getProperty("dfile").value<void*>();

		save(dfile);
	} else if (query.getName() == "RWBSSaveAllFiles") {
		for (QMap<DisplayedFile*, DisplayedFileEntry>::iterator it = openFiles.begin() ; it != openFiles.end() ; it++) {
			DisplayedFile* dfile = it.key();
			save(dfile);
		}
	}
}


void RWBSUtilityWidget::currentEntityChanged(DisplayedEntity* oldEnt, DisplayedEntity* newEnt)
{
	DisplayedFile* dfile = dynamic_cast<DisplayedFile*>(newEnt);

	if (!dfile)
		return;

	if (openFiles.contains(dfile))
		setCurrentFile(dfile);
	else
		setCurrentFile(NULL);
}


void RWBSUtilityWidget::setCurrentFile(DisplayedFile* dfile)
{
	currentFile = dfile;

	RWBSGUIModule* guiModule = RWBSGUIModule::getInstance();

	QList<RWSection*> rootSects;

	if (dfile) {
		DisplayedFileEntry entry = openFiles[dfile];

		for (RWBSWidget::SectIterator it = entry.rwbsWidget->getRootSectionBegin() ; it != entry.rwbsWidget->getRootSectionEnd() ; it++) {
			RWSection* sect = *it;
			rootSects << sect;
		}

		setCurrentWidget(entry.rwbsWidget);

		updateCurrentFileEditableStatus();
	} else {
		setCurrentIndex(0);
	}
}


void RWBSUtilityWidget::registerFile(DisplayedFile* dfile, bool writable)
{
	System* sys = System::getInstance();

	DisplayedFileEntry entry;
	entry.writable = writable;
	entry.handler = new DefaultDisplayedEntityHandler(dfile);
	entry.hasChanges = false;
	entry.rwbsWidget = new RWBSWidget(this);

	addWidget(entry.rwbsWidget);

	entry.rwbsWidget->loadFile(dfile->getFile());
	entry.rwbsWidget->setUndoStack(entry.handler->getUndoStack());

	entry.handler->getUndoStack()->setClean();
	connect(entry.handler->getUndoStack(), SIGNAL(cleanChanged(bool)), this, SLOT(entityUndoStackCleanStateChanged(bool)));

	connect(entry.rwbsWidget, SIGNAL(sectionDataChanged(RWSection*)), this, SLOT(currentDataChanged()));
	connect(entry.rwbsWidget, SIGNAL(sectionUpdated(RWSection*, uint32_t, uint32_t, bool)), this, SLOT(currentDataChanged()));
	connect(entry.rwbsWidget, SIGNAL(sectionInserted(RWSection*)), this, SLOT(currentDataChanged()));
	connect(entry.rwbsWidget, SIGNAL(sectionRemoved(RWSection*, RWSection*)), this, SLOT(currentDataChanged()));

	connect(entry.rwbsWidget, SIGNAL(sectionInserted(RWSection*)), this, SLOT(sectionInserted(RWSection*)));
	connect(entry.rwbsWidget, SIGNAL(sectionRemoved(RWSection*, RWSection*)), this, SLOT(sectionRemoved(RWSection*, RWSection*)));
	connect(entry.rwbsWidget, SIGNAL(sectionUpdated(RWSection*, uint32_t, uint32_t, bool)), this,
			SLOT(sectionUpdated(RWSection*, uint32_t, uint32_t, bool)));
	connect(entry.rwbsWidget, SIGNAL(sectionDataChanged(RWSection*)), this, SLOT(sectionDataChanged(RWSection*)));
	connect(entry.rwbsWidget, SIGNAL(currentSectionChanged(RWSection*, RWSection*)), this,
			SLOT(currentSectionChanged(RWSection*, RWSection*)));

	if (writable) {
		connect(entry.handler, SIGNAL(editLockAcquired()), this, SLOT(updateCurrentFileEditableStatus()));
		connect(entry.handler, SIGNAL(editLockAcquisitionLost()), this, SLOT(updateCurrentFileEditableStatus()));
		connect(entry.handler, SIGNAL(handleSaveChangesRequest(bool&)), this, SLOT(fileSaveRequested(bool&)));
		connect(entry.handler, SIGNAL(hasUnsavedChangesStateChanged(bool)), this, SLOT(entityHasUnsavedChangesStateChanged(bool)));
	}

	connect(dfile, SIGNAL(closed()), this, SLOT(fileClosed()));

	openFiles.insert(dfile, entry);

	if (dfile == sys->getCurrentEntity()) {
		setCurrentFile(dfile);
	}

	updateCurrentFileEditableStatus();
}


void RWBSUtilityWidget::updateCurrentFileEditableStatus()
{
	if (!currentFile)
		return;

	DisplayedFileEntry entry = openFiles[currentFile];

	if (entry.writable) {
		if (!entry.handler->hasEditLock()  &&  !entry.handler->canAcquireEditLock()) {
			if (entry.rwbsWidget->isEditable())
				entry.rwbsWidget->setEditable(false);
		} else {
			if (!entry.rwbsWidget->isEditable())
				entry.rwbsWidget->setEditable(true);
		}
	} else {
		if (entry.rwbsWidget->isEditable())
			entry.rwbsWidget->setEditable(false);
	}
}


void RWBSUtilityWidget::currentDataChanged()
{
	DisplayedFileEntry& entry = openFiles[currentFile];

	entry.handler->acquireEditLock();
	entry.handler->setHasUnsavedChanges(true);

	updateCurrentFileEditableStatus();

	entry.hasChanges = true;
}


bool RWBSUtilityWidget::applyChanges(DisplayedFile* dfile)
{
	DisplayedFileEntry& entry = openFiles[currentFile];

	if (!entry.writable)
		return false;

	if (entry.hasChanges) {
		entry.rwbsWidget->applyChanges();

		File file = dfile->getFile();
		File pfile = file.getParent();

		if (pfile.isArchiveDirectory()) {
			IMGArchive img(pfile, IMGArchive::ReadWrite);

			IMGArchive::EntryIterator eit = img.getEntryByName(file.getPath().getFileName().get());

			if (eit != img.getEntryEnd()) {
				size_t size = 0;

				RWBSWidget::SectIterator it;
				for (it = entry.rwbsWidget->getRootSectionBegin() ; it != entry.rwbsWidget->getRootSectionEnd() ; it++) {
					RWSection* sect = *it;
					size += sect->getSize() + 12;
				}

				img.resizeEntry(eit, IMG_BYTES2BLOCKS(size));

				iostream* stream = (iostream*) img.gotoEntry(eit);

				for (it = entry.rwbsWidget->getRootSectionBegin() ; it != entry.rwbsWidget->getRootSectionEnd() ; it++) {
					RWSection* sect = *it;
					sect->write(stream);
				}

				delete stream;

				img.sync();
			}
		} else {
			ostream* stream = file.openOutputStream(ostream::binary);

			RWBSWidget::SectIterator it;
			for (it = entry.rwbsWidget->getRootSectionBegin() ; it != entry.rwbsWidget->getRootSectionEnd() ; it++) {
				RWSection* sect = *it;
				sect->write(stream);
			}

			delete stream;
		}

		entry.hasChanges = false;
	}

	return true;
}


bool RWBSUtilityWidget::save(DisplayedFile* dfile)
{
	if (!openFiles[dfile].writable)
		return false;

	return applyChanges(dfile);
}


void RWBSUtilityWidget::fileSaveRequested(bool& success)
{
	if (success)
		return;

	DisplayedEntityHandler* handler = (DisplayedEntityHandler*) sender();
	DisplayedFile* dfile = dynamic_cast<DisplayedFile*>(handler->getDisplayedEntity());
	success = save(dfile);
}


void RWBSUtilityWidget::fileClosed()
{
	DisplayedFile* dfile = (DisplayedFile*) sender();

	if (currentFile == dfile) {
		setCurrentFile(NULL);
	}

	QMap<DisplayedFile*, DisplayedFileEntry>::iterator it = openFiles.find(dfile);

	DisplayedFileEntry& entry = it.value();
	delete entry.handler;
	removeWidget(entry.rwbsWidget);
	delete entry.rwbsWidget;

	openFiles.erase(it);
}


void RWBSUtilityWidget::entityHasUnsavedChangesStateChanged(bool hasChanges)
{
	DisplayedEntityHandler* handler = (DisplayedEntityHandler*) sender();
	DisplayedFile* dfile = dynamic_cast<DisplayedFile*>(handler->getDisplayedEntity());

	if (!hasChanges) {
		DisplayedFileEntry& entry = openFiles[dfile];

		// This is needed, because we are currently in a slot that is called from handler's signal, and
		// when we release handler's edit lock right now, the signal gets deconnected, which would mean
		// that all slots following this one wouldn't get called.
		QTimer::singleShot(0, entry.handler, SLOT(releaseEditLock()));
	}
}


void RWBSUtilityWidget::entityUndoStackCleanStateChanged(bool clean)
{
	QUndoStack* undoStack = (QUndoStack*) sender();

	for (QMap<DisplayedFile*, DisplayedFileEntry>::iterator it = openFiles.begin() ; it != openFiles.end() ; it++) {
		DisplayedFile* dfile = it.key();

		if (dfile->getCurrentEditHandler()  &&  dfile->getCurrentEditHandler()->getUndoStack() == undoStack) {
			DisplayedFileEntry& entry = it.value();

			if (clean) {
				entry.hasChanges = false;
				entry.handler->setHasUnsavedChanges(false);
			} else {
				currentDataChanged();
			}

			break;
		}
	}
}


void RWBSUtilityWidget::sectionInserted(RWSection* sect)
{
	RWBSGUIModule* guiModule = RWBSGUIModule::getInstance();

	if (!sect->getParent()) {
		DisplayedFileEntry entry = openFiles[currentFile];

		QList<RWSection*> rootSects;

		for (RWBSWidget::SectIterator it = entry.rwbsWidget->getRootSectionBegin() ; it != entry.rwbsWidget->getRootSectionEnd() ; it++) {
			RWSection* sect = *it;
			rootSects << sect;
		}
	}
}


void RWBSUtilityWidget::sectionRemoved(RWSection* sect, RWSection* oldParent)
{
	RWBSGUIModule* guiModule = RWBSGUIModule::getInstance();
}


void RWBSUtilityWidget::sectionUpdated(RWSection* sect, uint32_t oldID, uint32_t oldVersion, bool oldContainer)
{
	RWBSGUIModule* guiModule = RWBSGUIModule::getInstance();
}


void RWBSUtilityWidget::sectionDataChanged(RWSection* sect)
{
	RWBSGUIModule* guiModule = RWBSGUIModule::getInstance();
}


void RWBSUtilityWidget::currentSectionChanged(RWSection* oldSect, RWSection* newSect)
{
	if (oldSect == newSect)
		return;

	RWBSGUIModule* guiModule = RWBSGUIModule::getInstance();
}

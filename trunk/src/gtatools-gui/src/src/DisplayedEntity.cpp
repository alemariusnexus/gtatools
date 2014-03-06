/*
	Copyright 2010-2013 David "Alemarius Nexus" Lerch

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

#include "DisplayedEntity.h"
#include "System.h"
#include "DisplayedEntityHandler.h"



DisplayedEntity::DisplayedEntity()
		: currentEditHandler(NULL)
{
	System* sys = System::getInstance();

	connect(sys, SIGNAL(entityClosed(DisplayedEntity*)), this, SLOT(entityClosed(DisplayedEntity*)));
	connect(sys, SIGNAL(currentEntityChanged(DisplayedEntity*, DisplayedEntity*)), this,
			SLOT(currentEntityChanged(DisplayedEntity*, DisplayedEntity*)));
}


void DisplayedEntity::entityClosed(DisplayedEntity* ent)
{
	if (ent == this)
		emit closed();
}


void DisplayedEntity::currentEntityChanged(DisplayedEntity* prev, DisplayedEntity* current)
{
	if (current == this  &&  prev != this)
		emit madeCurrent();
	else if (prev == this  &&  current != this)
		emit lostCurrent(current);
}


void DisplayedEntity::registerHandler(DisplayedEntityHandler* handler)
{
	handlers << handler;
}


void DisplayedEntity::unregisterHandler(DisplayedEntityHandler* handler)
{
	handlers.removeOne(handler);
}


bool DisplayedEntity::canAcquireEditLock(const DisplayedEntityHandler* handler) const
{
	return currentEditHandler == handler  ||  currentEditHandler == NULL;
}


bool DisplayedEntity::acquireEditLock(DisplayedEntityHandler* handler)
{
	if (!canAcquireEditLock(handler))
		return false;

	DisplayedEntityHandler* oldHandler = currentEditHandler;

	currentEditHandler = handler;

	if (handler != oldHandler) {
		connect(handler, SIGNAL(hasUnsavedChangesStateChanged(bool)), this, SIGNAL(hasUnsavedChangesStateChanged(bool)));

		emit currentEditHandlerChanged(oldHandler, handler);
	}

	return true;
}


bool DisplayedEntity::releaseEditLock(DisplayedEntityHandler* handler)
{
	if (currentEditHandler == NULL)
		return true;
	if (currentEditHandler != handler)
		return false;

	currentEditHandler = NULL;

	disconnect(handler, SIGNAL(hasUnsavedChangesStateChanged(bool)), this, SIGNAL(hasUnsavedChangesStateChanged(bool)));

	emit currentEditHandlerChanged(handler, NULL);

	for (QLinkedList<DisplayedEntityHandler*>::iterator it = handlers.begin() ; it != handlers.end() ; it++) {
		DisplayedEntityHandler* h = *it;

		if (h != handler) {
			h->editLockAcquisitionReleasedExternally();
		}
	}

	return true;
}


bool DisplayedEntity::saveChanges(DisplayedEntityHandler* handler, bool letUserChooseFile)
{
	if (currentEditHandler != handler)
		return false;

	if (doSaveChanges(letUserChooseFile)) {
		for (QLinkedList<DisplayedEntityHandler*>::iterator it = handlers.begin() ; it != handlers.end() ; it++) {
			DisplayedEntityHandler* h = *it;

			if (h != handler) {
				h->changesSavedExternally();
			}
		}

		return true;
	}

	return false;
}


bool DisplayedEntity::doSaveChanges(bool letUserChooseFile)
{
	if (!currentEditHandler)
		return false;

	return currentEditHandler->doSaveChanges();
}


bool DisplayedEntity::hasUnsavedChanges() const
{
	return currentEditHandler != NULL  &&  currentEditHandler->hasUnsavedChanges();
}


bool DisplayedEntity::saveChanges(bool letUserChooseFile)
{
	if (!currentEditHandler)
		return false;

	return currentEditHandler->saveChanges(letUserChooseFile);
}


bool DisplayedEntity::canSave()
{
	return currentEditHandler != NULL;
}


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

#include "DisplayedEntityHandler.h"



DisplayedEntityHandler::DisplayedEntityHandler(DisplayedEntity* entity)
		: entity(entity), undoStack(new QUndoStack), unsavedChanges(false)
{
	entity->registerHandler(this);
}


DisplayedEntityHandler::~DisplayedEntityHandler()
{
	entity->unregisterHandler(this);
	delete undoStack;
}


bool DisplayedEntityHandler::canAcquireEditLock() const
{
	return entity->canAcquireEditLock(this);
}


bool DisplayedEntityHandler::acquireEditLock()
{
	bool acq = entity->acquireEditLock(this);

	if (acq)
		emit editLockAcquired();

	return acq;
}


bool DisplayedEntityHandler::hasEditLock() const
{
	return entity->currentEditHandler == this;
}


bool DisplayedEntityHandler::releaseEditLock()
{
	return entity->releaseEditLock(this);
}


void DisplayedEntityHandler::changesSavedExternally()
{
	emit entityChanged();
}


void DisplayedEntityHandler::editLockAcquisitionReleasedExternally()
{
	emit editLockAcquisitionReleased();
}


bool DisplayedEntityHandler::saveChanges(bool letUserChooseFile)
{
	bool success = entity->saveChanges(this, letUserChooseFile);

	if (success) {
		setHasUnsavedChanges(false);
	}

	return success;
}


void DisplayedEntityHandler::setHasUnsavedChanges(bool hasChanges)
{
	if (!hasEditLock())
		return;

	bool oldState = unsavedChanges;
	unsavedChanges = hasChanges;

	if (hasChanges != oldState) {
		emit hasUnsavedChangesStateChanged(hasChanges);
	}
}

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

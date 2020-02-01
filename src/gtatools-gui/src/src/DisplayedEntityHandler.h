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

#ifndef DISPLAYEDENTITYHANDLER_H_
#define DISPLAYEDENTITYHANDLER_H_

#include <QObject>
#include <QUndoStack>
#include "DisplayedEntity.h"



/**
 *	An access interface for a single module that is interested in accessing a DisplayedEntity.
 *
 *	Access to DisplayedEntities, especially to actions which modify their content, or access to notification of
 *	changes to them, are managed through this class. Every module/subsystem that has an interest to modify a
 *	particular DisplayedEntity, or to be notified when other modules make changes to them, creates its own
 *	DisplayedEntityHandler, through which it gains access to these features.
 *
 *	If such a module wants to make any changes to the DisplayedEntity, it first has to acquire an edit lock.
 *	Only one DisplayedEntityHandler can have an active edit lock at any point in time. This ensures that
 *	no two modules modify the same DisplayedEntity at the same time, which would mean that if both modules
 *	want to save their changes, these changes would have to be merged, which isn't always possible.
 *
 *	For managing changes to the entity, each DisplayedEntityHandler has its own QUndoStack. Note that by
 *	default, none of the QUndoStack signals is connected to a slot here, so if e.g. the QUndoStack becomes
 *	clean, this does not automatically release the edit lock, although this behavior might be useful.
 *
 *	This class (or subclasses of it) is also responsible to actually save whatever changes were made to
 *	the DisplayedEntity. When changes are to be saved, the method doSaveChanges() is called.
 */
class DisplayedEntityHandler : public QObject
{
	Q_OBJECT

public:
	/**
	 * Create a new DisplayedEntityHandler for the given DisplayedEntity.
	 *
	 * @param entity The entity to be handled
	 */
	DisplayedEntityHandler(DisplayedEntity* entity);


	/**
	 * Destroys this DisplayedEntityHandler.
	 *
	 * This deletes the QUndoStack and unregisters this handler from its DisplayedEntity. It does, however,
	 * _not_ release the edit lock, if it is acquired.
	 */
	virtual ~DisplayedEntityHandler();


	/**
	 * @return The DisplayedEntity this object handles.
	 */
	DisplayedEntity* getDisplayedEntity() { return entity; }


	/**
	 * @return This DisplayEntityHandler's own QUndoStack.
	 */
	QUndoStack* getUndoStack() { return undoStack; }


	/**
	 * Determine whether an edit lock can be acquired. This is only true if no other DisplayedEntityHandler has
	 * currently acquired a lock. It is also true if this handler already has acquired the edit lock.
	 *
	 * @return true if an edit lock can be or is currently acquired, false otherwise.
	 */
	bool canAcquireEditLock() const;


	/**
	 * @return true if this handler currently holds the edit lock, false otherwise.
	 */
	bool hasEditLock() const;

	bool saveChanges(bool letUserChooseFile = false);
	void setHasUnsavedChanges(bool hasChanges);
	bool hasUnsavedChanges() const { return unsavedChanges; }

public slots:
	bool acquireEditLock();
	bool releaseEditLock();

protected:
	virtual bool doSaveChanges() { return false; }

private:
	void changesSavedExternally();
	void editLockAcquisitionReleasedExternally();

signals:
	void editLockAcquired();
	void editLockAcquisitionLost();
	void editLockAcquisitionReleased();
	void entityChanged();
	void hasUnsavedChangesStateChanged(bool hasChanges);

private:
	DisplayedEntity* entity;
	QUndoStack* undoStack;
	bool unsavedChanges;


	friend class DisplayedEntity;
};

#endif /* DISPLAYEDENTITYHANDLER_H_ */

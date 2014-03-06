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

#ifndef DISPLAYEDENTITY_H_
#define DISPLAYEDENTITY_H_

#include <QtCore/QString>
#include <QtCore/QLinkedList>
#include <QtGui/QUndoStack>
#include <QtGui/QWidget>

class DisplayedEntityHandler;



/**
 * Represents an opened entity.
 *
 * A DisplayedEntity is an abstract concept of something that is perceived as a single unit by the user and the system.
 * Each DisplayedEntity has its own tab widget in the main tabber, is separately handled by a EntityHandler, has its
 * own 'changed' state, can be locked or unlocked if changes are made, etc.
 * The most basic and most often used example of a DisplayedEntity is a DisplayedFile. A DisplayedFile represents a
 * single file for which gtatools-gui provides a means of viewing and editing the contents. This file is seen as a
 * whole and it is handled separately from any other DisplayedFiles.
 * The abstract concept of DisplayedEntity however is not limited to a single file. Imagine you want to add a simple
 * map viewer to gtatools-gui. Such a map viewer would display the contents of many different files like meshes,
 * textures, and others, but it would still be perceived as a single unit by the user, and has a single tab widget.
 * Therefore, such a map viewer would be a custom DisplayedEntity.
 *
 *	There are a bunch of abstract concepts that are common to all DisplayedEntities.
 *
 *	Every DisplayedEntity has a name that describes it. For a simple DisplayedFile, this is usually the file name, but
 *	it can be anything that describes the DisplayedEntity. It is displayed as a tab widget title, for example.
 *
 *	Each DisplayedEntity has a unique signature. A signature is a bunch of bytes that uniquely identifies the
 *	DisplayedEntity, and is used to find out whether a DisplayedEntity is already opened, among other uses. For a
 *	simple DisplayedFile, this might be the fully qualified file path. Signatures are compared by value, and can
 *	contain any data that is useful to build a unique identifier.
 *
 *	If you want to modify the contents of a DisplayedEntity, or if you just want to be notified of changes to it, you
 *	have to create a DisplayedEntityHandler for it.
 */
class DisplayedEntity : public QObject {
	Q_OBJECT

public:
	DisplayedEntity();
	virtual QWidget* getWidget() const = 0;
	virtual QString getName() const = 0;
	virtual QByteArray getSignature() const { return QByteArray(); }
	virtual bool hasUnsavedChanges() const;
	virtual bool saveChanges(bool letUserChooseFile = false);
	virtual bool canSave();
	virtual DisplayedEntityHandler* getCurrentEditHandler() { return currentEditHandler; }

private:
	void registerHandler(DisplayedEntityHandler* handler);
	void unregisterHandler(DisplayedEntityHandler* handler);
	bool canAcquireEditLock(const DisplayedEntityHandler* handler) const;
	bool acquireEditLock(DisplayedEntityHandler* handler);
	bool releaseEditLock(DisplayedEntityHandler* handler);
	bool saveChanges(DisplayedEntityHandler* handler, bool letUserChooseFile);

protected:
	virtual bool doSaveChanges(bool letUserChooseFile);

signals:
	void closed();
	void madeCurrent();
	void lostCurrent(DisplayedEntity* newCurrent);
	void hasUnsavedChangesStateChanged(bool hasChanges);
	void currentEditHandlerChanged(DisplayedEntityHandler* oldHandler, DisplayedEntityHandler* newHandler);

private slots:
	void entityClosed(DisplayedEntity* file);
	void currentEntityChanged(DisplayedEntity* prev, DisplayedEntity* current);

private:
	QLinkedList<DisplayedEntityHandler*> handlers;
	DisplayedEntityHandler* currentEditHandler;

private:
	friend class DisplayedEntityHandler;
};

#endif /* DISPLAYEDENTITY_H_ */

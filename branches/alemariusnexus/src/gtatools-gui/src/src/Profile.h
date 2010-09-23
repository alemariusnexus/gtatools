/*
	Copyright 2010 David "Alemarius Nexus" Lerch

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
 */

#ifndef PROFILE_H_
#define PROFILE_H_

#include <QtCore/qlinkedlist.h>
#include <QtCore/qlist.h>
#include <QtCore/qstring.h>
#include <QtCore/qfile.h>
#include <gtaformats/engine/ResourceIndex.h>
#include <qmetatype.h>
#include <gtaformats/util/File.h>
#include "ProfileInitializer.h"
#include "Task.h"


class Profile : public QObject {
	Q_OBJECT

public:
	typedef QLinkedList<File*>::iterator ResourceIterator;

public:
	Profile(const QString& name);

	void addResource(const File& resource);
	ResourceIterator getResourceBegin();
	ResourceIterator getResourceEnd();
	int getResourceCount() { return resources.size(); }
	QString getName() const { return name; }
	ResourceIterator removeResource(ResourceIterator it);
	void clearResources();
	void setName(const QString& name) { this->name = name; }
	void synchronize();
	bool containsFile(const File& file);
	ResourceIndex* getResourceManager() { return resourceIndex; };
	bool isResourceIndexInitialized() { return resourceIdxInitialized; }

private:
	void loadResourceIndex();

public slots:
	void currentProfileChanged(Profile* oldProfile, Profile* newProfile);

private slots:
	void resourcesInitialized();
	void selfChanged();

signals:
	void changed();
	void resourceIndexInitialized();

private:
	QLinkedList<File*> resources;
	QString name;
	ResourceIndex* resourceIndex;
	ProfileInitializer* currentInitializer;
	bool resourceIdxInitialized;
	Task* resourceInitTask;




public:
	/**	\brief Just here for compatibility with Qt's meta type system and QVariant.
	 *
	 * 	NEVER EVER CALL THIS CONSTRUCTOR!
	 */
	Profile();

	/**	\brief Just here for compatibility with Qt's meta type system and QVariant.
	 *
	 * 	NEVER EVER CALL THIS CONSTRUCTOR!
	 */
	Profile(const Profile& other);
};

//Q_DECLARE_METATYPE(Profile)

#endif /* PROFILE_H_ */

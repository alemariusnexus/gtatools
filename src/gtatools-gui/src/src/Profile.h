/*
	Copyright 2010-2011 David "Alemarius Nexus" Lerch

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

#ifndef PROFILE_H_
#define PROFILE_H_

#include <QtCore/QLinkedList>
#include <QtCore/QList>
#include <QtCore/QString>
#include <QtCore/QFile>
#include <QtCore/QMetaType>
#include <gtaformats/util/File.h>
#include <gta/resource/ResourceObserver.h>
#include <gta/Engine.h>
#include "Task.h"
#include "SystemQuery.h"
#include "SystemQueryResult.h"
#include <map>

using std::multimap;



class Profile : public QObject, public ResourceObserver
{
	Q_OBJECT

public:
	typedef QLinkedList<File*>::iterator ResourceIterator;

private:
	typedef multimap<hash_t, char*> MeshTexMap;

public:
	Profile(const QString& name);

	void addResource(const File& resource);
	void addDATFile(const File& file);
	bool setResources(const QLinkedList<File>& resources);
	bool setDATFiles(const QLinkedList<File>& files);
	ResourceIterator getResourceBegin();
	ResourceIterator getResourceEnd();
	ResourceIterator getDATFilesBegin() { return datFiles.begin(); }
	ResourceIterator getDATFilesEnd() { return datFiles.end(); }
	QLinkedList<File*>& getResources() { return resources; }
	int getResourceCount() const { return resources.size(); }
	int getDATFileCount() const { return datFiles.size(); }
	QString getDATRootDirectory() const { return datRoot; }
	void setDATRootDirectory(const QString& dir);
	QLinkedList<File*> getResources() const { return resources; }
	QString getName() const { return name; }
	ResourceIterator removeResource(ResourceIterator it);
	ResourceIterator removeDATFile(ResourceIterator it);
	void clearResources();
	void setName(const QString& name);
	void synchronize();
	bool containsFile(const File& file);
	int findTexturesForMesh(hash_t meshName, char**& textures);
	void updateResourceIndex() { if (isCurrent()) loadResourceIndex(); }
	bool isCurrent() const;
	virtual void resourceAdded(const File& file);

private:
	void loadResourceIndex();
	void addResourceRecurse(const File& file);
	void removeResourceRecurse(const File& file);

private slots:
	void currentProfileChanged(Profile* oldProfile, Profile* newProfile);
	void resourcesInitialized();
	void systemQuerySent(const SystemQuery& query, SystemQueryResult& result);

signals:
	//void changed();
	void nameChanged(const QString& oldStr, const QString& newStr);
	void datRootChanged(const QString& oldRoot, const QString& newRoot);
	void datFileAdded(const File& file);
	void datFileRemoved(const File& file);
	void profileResourceAdded(const File& file);
	void profileResourceRemoved(const File& file);
	void resourceIndexInitialized();

private:
	QLinkedList<File*> resources;
	QString name;
	MeshTexMap meshTextures;
	QString datRoot;
	QLinkedList<File*> datFiles;



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

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

#ifndef PROFILE_H_
#define PROFILE_H_

#include <QLinkedList>
#include <QList>
#include <QString>
#include <QFile>
#include <QMetaType>
#include <QQueue>
#include <nxcommon/file/File.h>
#include <gta/resource/ResourceObserver.h>
#include <gta/Engine.h>
#include <gta/scene/Scene.h>
#include "Task.h"
#include "SystemQuery.h"
#include "SystemQueryResult.h"
#include <map>

using std::multimap;



class Profile : public QObject, public ResourceObserver
{
	Q_OBJECT

public:
	typedef QLinkedList<File>::iterator ResourceIterator;

private:
	typedef multimap<hash_t, CString> MeshTexMap;

public:
	Profile(const QString& name);
	~Profile();

	void addResource(const File& resource);
	void addSearchResource(const File& resource);
	void addDATFile(const File& file);
	bool setResources(const QLinkedList<File>& resources);
	bool setSearchResources(const QLinkedList<File>& resources);
	bool setDATFiles(const QLinkedList<File>& files);
	ResourceIterator getResourceBegin();
	ResourceIterator getResourceEnd();
	ResourceIterator getSearchResourceBegin() { return searchResources.begin(); }
	ResourceIterator getSearchResourceEnd() { return searchResources.end(); }
	ResourceIterator getDATFilesBegin() { return datFiles.begin(); }
	ResourceIterator getDATFilesEnd() { return datFiles.end(); }
	QLinkedList<File>& getResources() { return resources; }
	QLinkedList<File>& getSearchResources() { return searchResources; }
	int getResourceCount() const { return resources.size(); }
	int getSearchResourceCount() const { return searchResources.size(); }
	int getDATFileCount() const { return datFiles.size(); }
	void setGameInfo(const GameInfo& info);
	GameInfo getGameInfo() { return gameInfo; }
	QLinkedList<File> getResources() const { return resources; }
	QString getName() const { return name; }
	ResourceIterator removeResource(ResourceIterator it);
	ResourceIterator removeSearchResource(ResourceIterator it);
	ResourceIterator removeDATFile(ResourceIterator it);
	void clearResources();
	void setName(const QString& name);
	void synchronize();
	bool containsFile(const File& file);
	QLinkedList<CString> findTexturesForMesh(hash_t meshName);
	void updateResourceIndex() { if (isCurrent()) loadResourceIndex(); }
	bool isCurrent() const;
	bool isResourceIndexFunctional() const { return resourceIndexFunctional; }
	virtual void resourceAdded(const File& file);

private:
	void loadResourceIndex();
	void addResourceRecurse(const File& file);
	void removeResourceRecurse(const File& file);
	void loadResourceRecurse(const File& file);
	void interruptResourceLoading();

private slots:
	void doLoadResourceIndex();
	void loadSingleResource();
	void loadSingleDAT();
	void currentProfileChanged(Profile* oldProfile, Profile* newProfile);
	void resourcesInitialized();
	void systemQuerySent(const SystemQuery& query, QList<SystemQueryResult>& results);

signals:
	//void changed();
	void nameChanged(const QString& oldStr, const QString& newStr);
	void datRootChanged(const QString& oldRoot, const QString& newRoot);
	void datFileAdded(const File& file);
	void datFileRemoved(const File& file);
	void engineResourceAdded(const File& file);
	void engineResourceRemoved(const File& file);
	void resourceIndexInitialized();
	void searchResourceAdded(const File& file);
	void searchResourceRemoved(const File& file);

private:
	QLinkedList<File> searchResources;
	QLinkedList<File> resources;
	QString name;
	MeshTexMap meshTextures;
	GameInfo gameInfo;
	QLinkedList<File> datFiles;
	QQueue<File> resourceLoadingQueue;
	QQueue<File> datLoadingQueue;
	Task* resourceLoadingTask;
	Task* datLoadingTask;
	bool stopResourceLoading;
	bool resourceIndexFunctional;
};

//Q_DECLARE_METATYPE(Profile)

#endif /* PROFILE_H_ */

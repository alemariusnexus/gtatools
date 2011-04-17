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
 */

#include "Profile.h"
#include <cstdio>
#include <cstdlib>
#include "ProfileManager.h"
#include "System.h"
#include <utility>
#include <gtaformats/gtaide.h>
#include <gtaformats/util/strutil.h>

using std::pair;
using std::distance;



void _UiUpdateCallback()
{
	qApp->processEvents();
}



Profile::Profile(const QString& name)
		: name(QString(name))
{
	connect(ProfileManager::getInstance(), SIGNAL(currentProfileChanged(Profile*, Profile*)), this,
			SLOT(currentProfileChanged(Profile*, Profile*)));
}


bool Profile::isCurrent() const
{
	return ProfileManager::getInstance()->getCurrentProfile() == this;
}


void Profile::addResource(const File& resource)
{
	resources << new File(resource);
	addResourceRecurse(resource);
}


void Profile::addResourceRecurse(const File& file)
{
	if (file.isDirectory()  ||  file.isArchiveFile()) {
		FileIterator* it = file.getIterator();
		File* child;

		while ((child = it->next())  !=  NULL) {
			addResourceRecurse(*child);
			delete child;
		}

		delete it;
	}

	emit profileResourceAdded(file);
}


void Profile::addDATFile(const File& file)
{
	datFiles << new File(file);
	emit datFileAdded(file);
}


Profile::ResourceIterator Profile::getResourceBegin()
{
	return resources.begin();
}


Profile::ResourceIterator Profile::getResourceEnd()
{
	return resources.end();
}


void Profile::loadResourceIndex()
{
	Engine* engine = Engine::getInstance();
	System* sys = System::getInstance();

	ResourceIterator it;

	Task* task = sys->createTask();
	task->start(tr("Building resource index..."));

	engine->clearResources();

	for (it = resources.begin() ; it != resources.end() ; it++) {
		File* resFile = *it;
		engine->addResource(*resFile, _UiUpdateCallback);
	}

	delete task;
}


void Profile::currentProfileChanged(Profile* oldProfile, Profile* newProfile)
{
	Engine* engine = Engine::getInstance();

	if (oldProfile == this  &&  newProfile != this) {
		//disconnect(this, SIGNAL(resourceAdded(const File&)), this, SLOT(resourceAddedSlot(const File&)));
		engine->removeResourceObserver(this);
	} else if (oldProfile != this  &&  newProfile == this) {
		//connect(this, SIGNAL(resourceAdded(const File&)), this, SLOT(resourceAddedSlot(const File&)));
		engine->addResourceObserver(this);
		loadResourceIndex();
	}
}


Profile::ResourceIterator Profile::removeResource(ResourceIterator it)
{
	File* file = *it;
	ResourceIterator next = resources.erase(it);
	removeResourceRecurse(*file);
	delete file;
	return next;
}


void Profile::removeResourceRecurse(const File& file)
{
	if (file.isDirectory()  ||  file.isArchiveFile()) {
		FileIterator* it = file.getIterator();
		File* child;

		while ((child = it->next())  !=  NULL) {
			removeResourceRecurse(*child);
			delete child;
		}

		delete it;
	}

	emit profileResourceRemoved(file);
}


Profile::ResourceIterator Profile::removeDATFile(ResourceIterator it)
{
	File* file = *it;
	ResourceIterator next = datFiles.erase(it);
	emit datFileRemoved(*file);
	delete file;
	return next;
}


void Profile::clearResources()
{
	ResourceIterator it;

	for (it = getResourceBegin() ; it != getResourceEnd() ;) {
		it = removeResource(it);
	}
}


void Profile::setDATRootDirectory(const QString& dir)
{
	QString oldStr = datRoot;
	datRoot = dir;
	emit datRootChanged(oldStr, dir);
}


void Profile::setName(const QString& name)
{
	QString oldName = this->name;
	this->name = name;
	emit nameChanged(oldName, name);
}


void Profile::synchronize()
{
}


bool Profile::containsFile(const File& file)
{
	ResourceIterator it;
	for (it = getResourceBegin() ; it != getResourceEnd() ; it++) {
		File* res = *it;

		if (file.isChildOf(*res)) {
			return true;
		}
	}

	return false;
}


void Profile::resourcesInitialized()
{
}


void Profile::resourceAdded(const File& file)
{
	if (file.guessContentType() == CONTENT_TYPE_IDE) {
		IDEReader ide(file);

		IDEStatement* stmt;

		while ((stmt = ide.readStatement())  !=  NULL) {
			if (stmt->getType() == IDETypeStaticObject  ||  stmt->getType() == IDETypeTimedObject) {
				IDEStaticObject* sobj = (IDEStaticObject*) stmt;
				char* lMeshName = new char[strlen(sobj->getModelName())+1];
				strtolower(lMeshName, sobj->getModelName());
				char* lTexName = new char[strlen(sobj->getTextureName())+1];
				strtolower(lTexName, sobj->getTextureName());

				meshTextures.insert(pair<hash_t, char*>(Hash(lMeshName), lTexName));

				delete[] lMeshName;
			}

			delete stmt;
		}
	}
}


int Profile::findTexturesForMesh(hash_t meshName, char**& textures)
{
	pair<MeshTexMap::iterator, MeshTexMap::iterator> range = meshTextures.equal_range(meshName);

	int numElements = distance(range.first, range.second);

	textures = new char*[numElements];

	MeshTexMap::iterator it;
	int i = 0;
	for (it = range.first ; it != range.second ; it++, i++) {
		textures[i] = it->second;
	}

	return numElements;
}


bool Profile::setResources(const QLinkedList<File>& resources)
{
	bool hasChanges = false;

	QLinkedList<File> oldResources;
	ResourceIterator rit;
	for (rit = this->resources.begin() ; rit != this->resources.end() ; rit++) {
		oldResources << **rit;
	}

	// First, search for removed files
	for (rit = this->resources.begin() ; rit != this->resources.end() ;) {
		if (!resources.contains(**rit)) {
			rit = removeResource(rit);
			hasChanges = true;
		} else {
			rit++;
		}
	}

	// Then find new files
	QLinkedList<File>::const_iterator newIt;
	for (newIt = resources.begin() ; newIt != resources.end() ; newIt++) {
		if (!oldResources.contains(*newIt)) {
			addResource(*newIt);
			hasChanges = true;
		}
	}

	return hasChanges;
}


bool Profile::setDATFiles(const QLinkedList<File>& files)
{
	bool hasChanges = false;

	QLinkedList<File> oldFiles;
	ResourceIterator rit;
	for (rit = this->datFiles.begin() ; rit != this->datFiles.end() ; rit++) {
		oldFiles << **rit;
	}

	// First, search for removed files
	for (rit = this->datFiles.begin() ; rit != this->datFiles.end() ;) {
		if (!files.contains(**rit)) {
			rit = removeDATFile(rit);
			hasChanges = true;
		} else {
			rit++;
		}
	}

	// Then find new files
	QLinkedList<File>::const_iterator newIt;
	for (newIt = files.begin() ; newIt != files.end() ; newIt++) {
		if (!oldFiles.contains(*newIt)) {
			addDATFile(*newIt);
			hasChanges = true;
		}
	}

	return hasChanges;
}
















Profile::Profile()
{
	fprintf(stderr, "UUUUUUUAAAAAARRRGGGHHH! Profile::Profile() should NEVER EVER be called. It's just here "
			"for compatibility with QVariant and Qt's meta type system! Program will exit now!");
	exit(1);
}


Profile::Profile(const Profile& other)
{
	fprintf(stderr, "UUUUUUUAAAAAARRRGGGHHH! Profile::Profile(const Profile&) should NEVER EVER be called. It's "
				"just here for compatibility with QVariant and Qt's meta type system! Program will exit now!");
	exit(1);
}

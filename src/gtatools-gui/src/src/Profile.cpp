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


/*void profileInitialized()
{
	System* sys = System::getInstance();
	sys->updateTaskValue(1);
	sys->endTask();
}*/



Profile::Profile(const QString& name)
		: name(QString(name))
{
	connect(ProfileManager::getInstance(), SIGNAL(currentProfileChanged(Profile*, Profile*)), this,
			SLOT(currentProfileChanged(Profile*, Profile*)));
	connect(this, SIGNAL(changed()), this, SLOT(selfChanged()));
}


void Profile::addResource(const File& resource)
{
	resources << new File(resource);
}


void Profile::addDATFile(const File& file)
{
	datFiles << new File(file);
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
	/*if (resourceManager) {
		delete resourceManager;
	}

	resourceIdxInitialized = false;
	resourceManager = new ExtendedResourceManager;
	resourceManager->resizeMeshCache(2 * 1000000); // 2MB
	resourceManager->resizeTextureCache(10 * 1000000); // 10MB

	Engine::getInstance()->setResourceManager(resourceManager);

	currentInitializer = new ProfileInitializer(this);
	connect(currentInitializer, SIGNAL(finished()), this, SLOT(resourcesInitialized()));
	currentInitializer->start();*/

	Engine* engine = Engine::getInstance();

	ResourceIterator it;

	for (it = resources.begin() ; it != resources.end() ; it++) {
		File* resFile = *it;
		engine->addResource(*resFile);
	}
}


void Profile::currentProfileChanged(Profile* oldProfile, Profile* newProfile)
{
	Engine* engine = Engine::getInstance();

	if (oldProfile == this  &&  newProfile != this) {
		engine->clearResources();
		engine->removeResourceObserver(this);
	} else if (oldProfile != this  &&  newProfile == this) {
		engine->addResourceObserver(this);
		loadResourceIndex();
	}
}


Profile::ResourceIterator Profile::removeResource(ResourceIterator it)
{
	File* file = *it;
	ResourceIterator next = resources.erase(it);
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


void Profile::synchronize()
{
	emit changed();
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


void Profile::selfChanged()
{
	if (ProfileManager::getInstance()->getCurrentProfile() == this) {
		loadResourceIndex();
	}
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

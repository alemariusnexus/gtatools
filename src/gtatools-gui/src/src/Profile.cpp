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

#include "Profile.h"
#include <cstdio>
#include <cstdlib>
#include "ProfileManager.h"
#include "System.h"
#include <utility>
#include <gtaformats/gtaide.h>
#include <nxcommon/strutil.h>
#include <gtaformats/dat/ResourceDATLoader.h>
#include <gta/render/DefaultRenderer.h>
#include <gta/scene/visibility/PVSDatabase.h>
#include <QtCore/QTimer>

using std::pair;
using std::distance;





Profile::Profile(const QString& name)
		: name(QString(name)), stopResourceLoading(false)
{
	connect(ProfileManager::getInstance(), SIGNAL(currentProfileChanged(Profile*, Profile*)), this,
			SLOT(currentProfileChanged(Profile*, Profile*)));
}


Profile::~Profile()
{
}


bool Profile::isCurrent() const
{
	return ProfileManager::getInstance()->getCurrentProfile() == this;
}


void Profile::addResource(const File& resource)
{
	resources << resource;
	//addResourceRecurse(resource);
	emit engineResourceAdded(resource);
}


void Profile::addSearchResource(const File& resource)
{
	searchResources << resource;
	emit searchResourceAdded(resource);
}


void Profile::addDATFile(const File& file)
{
	datFiles << file;
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


void Profile::setGameInfo(const GameInfo& info)
{
	gameInfo = info;

	if (isCurrent()) {
		Engine::getInstance()->setDefaultGameInfo(info);
	}
}


void Profile::interruptResourceLoading()
{
	stopResourceLoading = true;

	resourceLoadingQueue.clear();
}


void Profile::loadResourceIndex()
{
	stopResourceLoading = false;

	System* sys = System::getInstance();

	if (sys->isInitializing()) {
		connect(sys, SIGNAL(initializationDone()), this, SLOT(doLoadResourceIndex()));
	} else {
		doLoadResourceIndex();
	}
}


void Profile::doLoadResourceIndex()
{
	Engine* engine = Engine::getInstance();
	System* sys = System::getInstance();

	ResourceIterator it;

	engine->clearResources();

	for (it = resources.begin() ; it != resources.end()  &&  !stopResourceLoading ; it++) {
		File resFile = *it;
		loadResourceRecurse(resFile);
	}

	for (it = datFiles.begin() ; it != datFiles.end() ; it++) {
		File file = *it;
		datLoadingQueue.enqueue(file);
	}


	resourceLoadingTask = sys->createTask();
	resourceLoadingTask->start(0, resourceLoadingQueue.size(), tr("Building resource index..."));

	QTimer::singleShot(0, this, SLOT(loadSingleResource()));
}


void Profile::loadResourceRecurse(const File& file)
{
	Engine* engine = Engine::getInstance();
	System* sys = System::getInstance();

	if (file.isDirectoryOrArchiveDirectory()) {
		for (File child : file.getChildren()) {
			if (stopResourceLoading)
				break;

			if (child.guessContentType() != CONTENT_TYPE_DIR) {
				loadResourceRecurse(child);
			}
		}
	} else {
		try {
			resourceLoadingQueue.enqueue(file);
		} catch (Exception& ex) {
			sys->log(LogEntry::warning(tr("Error loading resource file %1: %2. The resource file will not be "
					"used for certain operations.").arg(file.getPath().toString().get()).arg(ex.getMessage().get()),
					&ex));
		}
	}
}


void Profile::loadSingleResource()
{
	System* sys = System::getInstance();

	if (sys->isShuttingDown()) {
		return;
	}

	if (resourceLoadingQueue.size() != 0) {
		File file = resourceLoadingQueue.dequeue();
		Engine* engine = Engine::getInstance();

		try {
			engine->addResource(file);
		} catch (Exception& ex) {
			sys->log(LogEntry::warning(tr("Error loading resource file %1: %2. The resource file will not be "
					"used for certain operations.").arg(file.getPath().toString().get()).arg(ex.getMessage().get()),
					&ex));
		}

		resourceLoadingTask->update(resourceLoadingTask->getValue() + 1);

		QTimer::singleShot(0, this, SLOT(loadSingleResource()));
	} else {
		delete resourceLoadingTask;

		datLoadingTask = sys->createTask();
		datLoadingTask->start("Loading DAT files...");

		QTimer::singleShot(0, this, SLOT(loadSingleDAT()));
	}
}


void Profile::loadSingleDAT()
{
	if (!datLoadingQueue.empty()) {
		File file = datLoadingQueue.dequeue();
		Engine* engine = Engine::getInstance();

		istream* in = file.openInputStream(istream::in);
		ResourceDATLoader dat(gameInfo.getRootDirectory());

		ResourceDATLoader::Entry e;
		while (dat.loadEntry(in, e)) {
			if (e.type == ResourceDATLoader::IDE  ||  e.type == ResourceDATLoader::IMG  ||  e.type == ResourceDATLoader::TXD) {
				engine->addResource(e.resolvedFile);
			}
		}

		/*Scene* oldScene = engine->getScene();
		if (engine->getScene() != scene)
			engine->setScene(scene);

		engine->loadDAT(*file, gameInfo.getRootDirectory());

		if (oldScene != scene)
			engine->setScene(oldScene);*/

		QTimer::singleShot(0, this, SLOT(loadSingleDAT()));
	} else {
		delete datLoadingTask;
	}
}


void Profile::currentProfileChanged(Profile* oldProfile, Profile* newProfile)
{
	Engine* engine = Engine::getInstance();

	if (oldProfile == this  &&  newProfile != this) {
		interruptResourceLoading();
		engine->clearResources();
		disconnect(System::getInstance(), SIGNAL(systemQuerySent(const SystemQuery&, QList<SystemQueryResult>&)),
				this, SLOT(systemQuerySent(const SystemQuery&, QList<SystemQueryResult>&)));
		engine->removeResourceObserver(this);
	} else if (oldProfile != this  &&  newProfile == this) {
		//connect(this, SIGNAL(resourceAdded(const File&)), this, SLOT(resourceAddedSlot(const File&)));
		connect(System::getInstance(), SIGNAL(systemQuerySent(const SystemQuery&, QList<SystemQueryResult>&)),
				this, SLOT(systemQuerySent(const SystemQuery&, QList<SystemQueryResult>&)));
		engine->setDefaultGameInfo(gameInfo);
		engine->addResourceObserver(this);
		loadResourceIndex();
	}
}


Profile::ResourceIterator Profile::removeResource(ResourceIterator it)
{
	File file = *it;
	ResourceIterator next = resources.erase(it);
	emit engineResourceRemoved(file);
	return next;
}


Profile::ResourceIterator Profile::removeSearchResource(ResourceIterator it)
{
	File file = *it;
	ResourceIterator next = searchResources.erase(it);
	emit searchResourceRemoved(file);
	return next;
}


Profile::ResourceIterator Profile::removeDATFile(ResourceIterator it)
{
	File file = *it;
	ResourceIterator next = datFiles.erase(it);
	emit datFileRemoved(file);
	return next;
}


void Profile::clearResources()
{
	ResourceIterator it;

	for (it = getResourceBegin() ; it != getResourceEnd() ;) {
		it = removeResource(it);
	}
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
		File res = *it;

		if (file.isChildOf(res, true)) {
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
			const char* meshName;
			const char* texName;

			idetype_t type = stmt->getType();

			if (type == IDETypeStaticObject  ||  type == IDETypeTimedObject) {
				IDEStaticObject* sobj = (IDEStaticObject*) stmt;
				meshName = sobj->getModelName().get();
				texName = sobj->getTXDArchiveName().get();
			} else if (type == IDETypeAnimation) {
				IDEAnimation* anim = (IDEAnimation*) stmt;
				meshName = anim->getModelName().get();
				texName = anim->getTXDArchiveName().get();
			} else if (type == IDETypePedestrian) {
				IDEPedestrian* ped = (IDEPedestrian*) stmt;
				meshName = ped->getModelName().get();
				texName = ped->getTXDArchiveName().get();
			} else if (type == IDETypeWeapon) {
				IDEWeapon* weap = (IDEWeapon*) stmt;
				meshName = weap->getModelName().get();
				texName = weap->getTXDArchiveName().get();
			}

			char* lMeshName = new char[strlen(meshName)+1];
			strtolower(lMeshName, meshName);
			char* lTexName = new char[strlen(texName)+1];
			strtolower(lTexName, texName);

			meshTextures.insert(pair<hash_t, char*>(Hash(lMeshName), lTexName));

			delete[] lMeshName;

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
		oldResources << *rit;
	}

	// First, search for removed files
	for (rit = this->resources.begin() ; rit != this->resources.end() ;) {
		if (!resources.contains(*rit)) {
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


bool Profile::setSearchResources(const QLinkedList<File>& resources)
{
	bool hasChanges = false;

	QLinkedList<File> oldResources;
	ResourceIterator rit;
	for (rit = this->searchResources.begin() ; rit != this->searchResources.end() ; rit++) {
		oldResources << *rit;
	}

	// First, search for removed files
	for (rit = this->searchResources.begin() ; rit != this->searchResources.end() ;) {
		if (!resources.contains(*rit)) {
			rit = removeSearchResource(rit);
			hasChanges = true;
		} else {
			rit++;
		}
	}

	// Then find new files
	QLinkedList<File>::const_iterator newIt;
	for (newIt = resources.begin() ; newIt != resources.end() ; newIt++) {
		if (!oldResources.contains(*newIt)) {
			addSearchResource(*newIt);
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
		oldFiles << *rit;
	}

	// First, search for removed files
	for (rit = this->datFiles.begin() ; rit != this->datFiles.end() ;) {
		if (!files.contains(*rit)) {
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


void Profile::systemQuerySent(const SystemQuery& query, QList<SystemQueryResult>& results)
{
	if (query.getName() == "FindMeshTextures") {
		QString meshName = query["meshName"].toString();
		char** textures;
		int numTexes = findTexturesForMesh(LowerHash(meshName.toLocal8Bit().constData()), textures);
		QStringList texNames;

		for (int i = 0 ; i < numTexes ; i++) {
			texNames << textures[i];
		}

		delete[] textures;

		SystemQueryResult res;
		res["textures"] = QVariant(texNames);
		results << res;
	}
}





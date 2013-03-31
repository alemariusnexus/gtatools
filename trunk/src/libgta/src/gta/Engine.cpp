/*
	Copyright 2010-2012 David "Alemarius Nexus" Lerch

	This file is part of libgta.

	libgta is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	libgta is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with libgta.  If not, see <http://www.gnu.org/licenses/>.

	Additional permissions are granted, which are listed in the file
	GPLADDITIONS.
 */

#include "Engine.h"
#include "resource/ResourceObserver.h"
#include "resource/ResourceCache.h"
#include "resource/mesh/MeshCacheLoader.h"
#include "resource/mesh/MeshIndexer.h"
#include "resource/texture/TextureCacheLoader.h"
#include "resource/texture/TextureIndexer.h"
#include "resource/collision/CollisionMeshCacheLoader.h"
#include "resource/collision/CollisionMeshIndexer.h"
#include "resource/animation/AnimationCacheLoader.h"
#include "resource/animation/AnimationIndexer.h"
#include "ItemManager.h"
#include "scene/Scene.h"
#include "scene/SceneObjectDefinitionInfo.h"
#include "scene/objects/MapSceneObject.h"
#include "GLException.h"
#include "EngineException.h"
#include <gtaformats/util/File.h>
#include <gtaformats/util/strutil.h>
#include <gtaformats/util/util.h>
#include <gtaformats/ipl/IPLReader.h>
#include <gtaformats/ipl/IPLInstance.h>
#include "MapItemDefinition.h"
#include <cstdio>
#include <utility>
#include <algorithm>
#include <set>
#include <map>
#include <queue>

using std::streamoff;
using std::find;
using std::set;
using std::map;
using std::multimap;
using std::queue;


Engine* Engine::instance = NULL;





Engine* Engine::getInstance()
{
	if (!instance)
		instance = new Engine;

	return instance;
}


void Engine::destroy()
{
	if (instance) {
		delete instance;
		instance = NULL;
	}
}


Engine::Engine()
		: defGameInfo(NULL), camera(NULL), scene(NULL), gameHours(8), gameMinutes(0), viewWidth(-1),
		  viewHeight(-1), testMem(0), freezeVisibility(false)
{
	meshIndexer = new MeshIndexer;
	texIndexer = TextureIndexer::getInstance();
	colIndexer = new CollisionMeshIndexer;
	animIndexer = new AnimationIndexer;

	meshCacheLoader = new MeshCacheLoader(meshIndexer);
	texCacheLoader = new TextureCacheLoader(texIndexer);
	colCacheLoader = new CollisionMeshCacheLoader(colIndexer);
	animCacheLoader = new AnimationCacheLoader(animIndexer);

	meshCache = new StringResourceCache(meshCacheLoader, 0);
	texCache = new StringResourceCache(texCacheLoader, 0);
	colCache = new StringResourceCache(colCacheLoader, 0);
	animCache = new StringResourceCache(animCacheLoader, 0);

	addResourceObserver(meshIndexer);
	addResourceObserver(texIndexer);
	addResourceObserver(colIndexer);
	addResourceObserver(animIndexer);
	addResourceObserver(ItemManager::getInstance());
}


void Engine::addResource(const File& file, void (*callback)())
{
	if (file.isDirectory()  ||  file.isArchiveFile()) {
		FileIterator* it = file.getIterator();
		File* child;

		while ((child = it->next())  !=  NULL) {
			// Don't load DIR files. Otherwise, addResource would be called both for the IMG and the DIR file.
			// Because the IMGArchive automatically handles both of them, this would be a double-loading, and
			// so we don't load DIRs, but only IMG files (a DIR file without an IMG doesn't make sense, so
			// this is safe).
			if (child->guessContentType() != CONTENT_TYPE_DIR) {
				addResource(*child, callback);
			}
			delete child;
		}

		delete it;
	} else {
		vector<ResourceObserver*>::iterator it;

		for (it = resObservers.begin() ; it != resObservers.end() ; it++) {
			ResourceObserver* observer = *it;
			observer->resourceAdded(file);
		}

		if (callback) {
			callback();
		}
	}
}


void Engine::clearResources()
{
	vector<ResourceObserver*>::iterator it;

	for (it = resObservers.begin() ; it != resObservers.end() ; it++) {
		ResourceObserver* observer = *it;
		observer->resourcesCleared();
	}

	meshCache->clear();
	texCache->clear();
	colCache->clear();
	animCache->clear();
}


void Engine::addResourceObserver(ResourceObserver* observer)
{
	resObservers.push_back(observer);
}


void Engine::removeResourceObserver(ResourceObserver* observer)
{
	vector<ResourceObserver*>::iterator it = find(resObservers.begin(), resObservers.end(), observer);
	resObservers.erase(it);
}


void Engine::addViewportObserver(ViewportObserver* observer)
{
	vpObservers.push_back(observer);
}


void Engine::removeViewportObserver(ViewportObserver* observer)
{
	vector<ViewportObserver*>::iterator it = find(vpObservers.begin(), vpObservers.end(), observer);
	vpObservers.erase(it);
}


void Engine::setViewportSize(int w, int h)
{
	viewWidth = w;
	viewHeight = h;

	for (vector<ViewportObserver*>::iterator it = vpObservers.begin() ; it != vpObservers.end() ; it++) {
		ViewportObserver* obsv = *it;
		obsv->viewportChanged(w, h);
	}
}


void Engine::advanceFrame(uint64_t advanceTime)
{
	if (!scene) {
		throw EngineException("Attempt to render without a Scene!", __FILE__, __LINE__);
	}
	if (!camera) {
		throw EngineException("Attempt to render without a Camera!", __FILE__, __LINE__);
	}
	if (viewWidth == -1  ||  viewHeight == -1) {
		throw EngineException("Attempt to render with invalid viewport dimensions!", __FILE__, __LINE__);
	}


	if (!freezeVisibility)
		scene->updateVisibility();

	scene->update(advanceTime);
}


void Engine::renderFrame()
{
	if (!scene) {
		throw EngineException("Attempt to render without a Scene!", __FILE__, __LINE__);
	}
	if (!camera) {
		throw EngineException("Attempt to render without a Camera!", __FILE__, __LINE__);
	}
	if (viewWidth == -1  ||  viewHeight == -1) {
		throw EngineException("Attempt to render with invalid viewport dimensions!", __FILE__, __LINE__);
	}

	scene->present();
}


void Engine::loadDAT(const File& file, const File& rootDir, const GameInfo* gameInfo)
{
	if (!gameInfo)
		gameInfo = defGameInfo;

	istream* dat = file.openInputStream();
	char line[1024];

	while (!dat->eof()) {
		dat->getline(line, sizeof(line));
		streamoff numChars = dat->gcount();

		if (numChars >= 2  &&  line[numChars-2] == '\r') {
			line[numChars-2] = '\0';
		}


		if (line[0] == '\0') {
			continue;
		}

		if (strncmp(line, "IDE", 3) == 0) {
			FilePath path(*rootDir.getPath(), line+4, FilePath::BackslashAsSeparator | FilePath::CorrectCase);
			File ide(&path, false);
			addResource(ide);
		} else if (strncmp(line, "IPL", 3) == 0) {
			FilePath path(*rootDir.getPath(), line+4, FilePath::BackslashAsSeparator | FilePath::CorrectCase);
			File ipl(&path, false);
			iplRecurse(&ipl, rootDir, gameInfo);
		} else if (strncmp(line, "IMG", 3) == 0) {
			FilePath path(*rootDir.getPath(), line+4, FilePath::BackslashAsSeparator | FilePath::CorrectCase);
			File img(&path, false);
			addResource(img);
		} else if (strncmp(line, "TEXDICTION", 10) == 0) {
			FilePath path(*rootDir.getPath(), line+11, FilePath::BackslashAsSeparator | FilePath::CorrectCase);
			File txd(&path, false);
			addResource(txd);
		}
	}

	delete dat;
}


void Engine::iplRecurse(File* file, const File& rootDir, const GameInfo* gameInfo)
{
	if (!gameInfo)
		gameInfo = defGameInfo;

	File gta3img(rootDir, "/models/gta3.img");

	if (file->isDirectory()) {
		FileIterator* it = file->getIterator();
		File* child;

		while ((child = it->next())  !=  NULL) {
			iplRecurse(child, rootDir);
			delete child;
		}

		delete it;
	} else {
		Scene::ObjectList objs;
		iplLoader.load(*file, objs, gameInfo);

		scene->addSceneObjects(objs.begin(), objs.end());
	}
}


void Engine::advanceGameTime(int8_t h, int8_t m)
{
	int16_t absTime = ((gameHours*60 + gameMinutes) + (h*60 + m)) % (24*60);
	gameHours = absTime / 60;
	gameMinutes = absTime % 60;
}

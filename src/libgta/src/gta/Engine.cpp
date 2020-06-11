/*
	Copyright 2010-2014 David "Alemarius Nexus" Lerch

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
#include <nxcommon/ResourceCache.h>
#include <nxcommon/file/FileSystem.h>
#include "resource/mesh/MeshCacheLoader.h"
#include "resource/mesh/MeshIndexer.h"
#include "resource/texture/TextureCacheLoader.h"
#include "resource/texture/TextureIndexer.h"
#include "resource/collision/CollisionMeshCacheLoader.h"
#include "resource/collision/CollisionMeshIndexer.h"
#include "resource/animation/AnimationCacheLoader.h"
#include "resource/animation/AnimationIndexer.h"
#include "resource/physics/PhysicsCacheLoader.h"
#include "ItemManager.h"
#include "scene/Scene.h"
#include "scene/SceneObjectDefinitionInfo.h"
#include "scene/objects/MapSceneObject.h"
#include "GLException.h"
#include "EngineException.h"
#include <nxcommon/file/File.h>
#include <nxcommon/strutil.h>
#include <nxcommon/util.h>
#include <gtaformats/ipl/IPLReader.h>
#include <gtaformats/ipl/IPLInstance.h>
#include <gtaformats/dat/ResourceDATLoader.h>
#include <gtaformats/img/IMGArchiveHandler.h>
#include "MapItemDefinition.h"
#include "render/DefaultShaderPluginAPI.h"
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





void _EngineGLDebugMessageCallback (
		GLenum source,
		GLenum type,
		GLuint id,
		GLenum severity,
		GLsizei length,
		const GLchar* message,
		const void* userParam
) {
	Engine* engine = (Engine*) userParam;

	engine->onGLDebugMessage(source, type, id, severity, length, message, userParam);
}




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
		: meshIndexer(new MeshIndexer),
		  texIndexer(new TextureIndexer),
		  colIndexer(new CollisionMeshIndexer),
		  animIndexer(new AnimationIndexer),

		  meshCacheLoader(new MeshCacheLoader(meshIndexer.get(), colIndexer.get())),
		  texCacheLoader(new TextureCacheLoader(texIndexer.get())),
		  colCacheLoader(new CollisionMeshCacheLoader(colIndexer.get())),
		  animCacheLoader(new AnimationCacheLoader(animIndexer.get())),
		  physicsCacheLoader(new PhysicsCacheLoader),

		  meshCache(new StringResourceCache(meshCacheLoader, 0)),
		  texCache(new StringResourceCache(texCacheLoader, 0)),
		  colCache(new StringResourceCache(colCacheLoader, 0)),
		  animCache(new StringResourceCache(animCacheLoader, 0)),
		  physicsCache(new StringResourceCache(physicsCacheLoader, 0)),

		  itemManager(new ItemManager),

		  defGameInfo(GameInfo()), camera(NULL), scene(NULL), gameHours(8), gameMinutes(0), viewWidth(-1),
		  viewHeight(-1), testMem(0), freezeVisibility(false), shaderPluginAPI(new DefaultShaderPluginAPI)
{
	FileSystem::getInstance()->registerArchiveHandler(IMGArchiveHandler::getInstance());

	/*meshIndexer = new MeshIndexer;
	texIndexer = TextureIndexer::getInstance();
	colIndexer = new CollisionMeshIndexer;
	animIndexer = new AnimationIndexer;

	meshCacheLoader = new MeshCacheLoader(meshIndexer, colIndexer);
	texCacheLoader = new TextureCacheLoader(texIndexer);
	colCacheLoader = new CollisionMeshCacheLoader(colIndexer);
	animCacheLoader = new AnimationCacheLoader(animIndexer);
	physicsCacheLoader = new PhysicsCacheLoader;

	meshCache = new StringResourceCache(meshCacheLoader, 0);
	texCache = new StringResourceCache(texCacheLoader, 0);
	colCache = new StringResourceCache(colCacheLoader, 0);
	animCache = new StringResourceCache(animCacheLoader, 0);
	physicsCache = new StringResourceCache(physicsCacheLoader, 0);*/

#ifndef NDEBUG
	oglDebugLogLevel = GL_DEBUG_SEVERITY_NOTIFICATION;
#else
	oglDebugLogLevel = GL_DEBUG_SEVERITY_MEDIUM;
#endif

	addResourceObserver(meshIndexer.get());
	addResourceObserver(texIndexer.get());
	addResourceObserver(colIndexer.get());
	addResourceObserver(animIndexer.get());
	addResourceObserver(itemManager.get());
}


Engine::~Engine()
{
	delete shaderPluginAPI;

	/*delete meshIndexer;
	delete colIndexer;
	delete animIndexer;

	delete meshCache;
	delete texCache;
	delete colCache;
	delete animCache;
	delete physicsCache;*/
}


void Engine::addResource(const File& file, void (*callback)())
{
	if (file.isDirectoryOrArchiveDirectory()) {
		for (File child : file.getChildren()) {
			// Don't load DIR files. Otherwise, addResource would be called both for the IMG and the DIR file.
			// Because the IMGArchive automatically handles both of them, this would be a double-loading, and
			// so we don't load DIRs, but only IMG files (a DIR file without an IMG doesn't make sense, so
			// this is safe).
			if (child.guessContentType() != CONTENT_TYPE_DIR) {
				addResource(child, callback);
			}
		}
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


void Engine::loadDAT(const File& file, const File& rootDir, GameInfo gameInfo)
{
	if (!gameInfo.isValid())
		gameInfo = defGameInfo;

	istream* stream = file.openInputStream();

	ResourceDATLoader dat(rootDir);

	ResourceDATLoader::Entry e;
	while (dat.loadEntry(stream, e)) {
		if (e.type == ResourceDATLoader::IPL) {
			iplRecurse(&e.resolvedFile, rootDir, gameInfo);
		} else {
			addResource(e.resolvedFile);
		}
	}

	delete stream;
}


void Engine::iplRecurse(File* file, const File& rootDir, GameInfo gameInfo)
{
	if (!gameInfo.isValid())
		gameInfo = defGameInfo;

	File gta3img(rootDir, "/models/gta3.img");

	if (file->isDirectory()) {
		for (File child : file->getChildren()) {
			iplRecurse(&child, rootDir);
		}
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


void Engine::setupDebug()
{
	if (gtaglIsVersionSupported(4, 3)) {
		glDebugMessageCallback(&_EngineGLDebugMessageCallback, this);
	}
}


void Engine::onGLDebugMessage (
		GLenum source,
		GLenum type,
		GLuint id,
		GLenum severity,
		GLsizei length,
		const GLchar* message,
		const void* userParam
) {
	if (oglDebugLogLevel == GL_INVALID_ENUM) {
		return;
	}
	if (oglDebugLogLevel == GL_DEBUG_SEVERITY_HIGH) {
		if (severity != GL_DEBUG_SEVERITY_HIGH) {
			return;
		}
	}
	if (oglDebugLogLevel == GL_DEBUG_SEVERITY_MEDIUM) {
		if (severity != GL_DEBUG_SEVERITY_HIGH  &&  severity != GL_DEBUG_SEVERITY_MEDIUM) {
			return;
		}
	}
	if (oglDebugLogLevel == GL_DEBUG_SEVERITY_LOW) {
		if (severity == GL_DEBUG_SEVERITY_NOTIFICATION) {
			return;
		}
	}

	const char* srcStr = "???";
	const char* typeStr = "???";
	const char* sevStr = "???";

	switch (source) {
	case GL_DEBUG_SOURCE_API:
		srcStr = "API";
		break;
	case GL_DEBUG_SOURCE_APPLICATION:
		srcStr = "APP";
		break;
	case GL_DEBUG_SOURCE_OTHER:
		srcStr = "OTH";
		break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER:
		srcStr = "SHC";
		break;
	case GL_DEBUG_SOURCE_THIRD_PARTY:
		srcStr = "THP";
		break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
		srcStr = "WSY";
		break;
	}

	switch (type) {
	case GL_DEBUG_TYPE_ERROR:
		typeStr = "ERR";
		break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
		typeStr = "DEP";
		break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
		typeStr = "UND";
		break;
	case GL_DEBUG_TYPE_PORTABILITY:
		typeStr = "POR";
		break;
	case GL_DEBUG_TYPE_PERFORMANCE:
		typeStr = "PRF";
		break;
	case GL_DEBUG_TYPE_MARKER:
		typeStr = "MRK";
		break;
	case GL_DEBUG_TYPE_PUSH_GROUP:
		typeStr = "PSH";
		break;
	case GL_DEBUG_TYPE_POP_GROUP:
		typeStr = "POP";
		break;
	case GL_DEBUG_TYPE_OTHER:
		typeStr = "OTH";
		break;
	}

	switch (severity) {
	case GL_DEBUG_SEVERITY_LOW:
		sevStr = "LOW";
		break;
	case GL_DEBUG_SEVERITY_MEDIUM:
		sevStr = "MED";
		break;
	case GL_DEBUG_SEVERITY_HIGH:
		sevStr = "HIG";
		break;
	case GL_DEBUG_SEVERITY_NOTIFICATION:
		sevStr = "NOT";
		break;
	}

	if (length < 0) {
		length = strlen(message);
	}

	printf("GLDEBUG [%s][%s][%s](%u) - %.s\n", sevStr, srcStr, typeStr, (unsigned int) id, (char*) message, (int) length);
}

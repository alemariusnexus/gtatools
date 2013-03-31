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

#ifndef ENGINE_H_
#define ENGINE_H_

#include <gta/config.h>
#include "Camera.h"
#include "GameInfo.h"
#include "resource/ResourceCache.h"
#include "EngineIPLLoader.h"
#include "scene/parts/VisualSceneObject.h"
#include "ViewportObserver.h"
#include <locale>
#include <cstring>
#include <vector>
#include <string>
#include <gtaformats/gta.h>
#include <gtaformats/util/math/Matrix4.h>
#include <gtaformats/util/strutil.h>
#include <gtaformats/util/File.h>
#include <gtaformats/util/cxx0xhash.h>
#include <gtaformats/util/CString.h>
#include <btBulletDynamicsCommon.h>
#include <functional>
#include <boost/shared_array.hpp>

using std::locale;
using std::collate;
using std::use_facet;
using std::vector;
using std::string;
using std::less;
using std::equal_to;
using boost::shared_array;


class ResourceObserver;
class MeshCacheLoader;
class TextureCacheLoader;
class CollisionMeshCacheLoader;
class AnimationCacheLoader;
class MeshIndexer;
class TextureIndexer;
class CollisionMeshIndexer;
class AnimationIndexer;
class Scene;
class SceneObjectHandler;
class StaticSceneObject;
template<class K, class Compare, class MapHash, class KeyEqual> class ResourceCache;





class Engine {
public:
	typedef ResourceCache<CString, StringComparator, CXX0XHash<CString>, StringEqual> StringResourceCache;

public:
	static Engine* getInstance();
	static void destroy();

public:
	void addResource(const File& file, void (*callback)() = NULL);
	void clearResources();
	void addResourceObserver(ResourceObserver* observer);
	void removeResourceObserver(ResourceObserver* observer);

	void addViewportObserver(ViewportObserver* observer);
	void removeViewportObserver(ViewportObserver* observer);

	StringResourceCache* getMeshCache() { return meshCache; }
	StringResourceCache* getTextureCache() { return texCache; }
	StringResourceCache* getCollisionMeshCache() { return colCache; }
	StringResourceCache* getAnimationCache() { return animCache; }

	MeshIndexer* getMeshIndexer() { return meshIndexer; }
	TextureIndexer* getTextureIndexer() { return texIndexer; }
	CollisionMeshIndexer* getCollisionMeshIndexer() { return colIndexer; }
	AnimationIndexer* getAnimationIndexer() { return animIndexer; }

	void setCamera(Camera* cam) { camera = cam; }
	Camera* getCamera() { return camera; }

	const Matrix4& getProjectionMatrix() const { return camera->getFrustum().getProjectionMatrix(); }

	void setScene(Scene* scene) { this->scene = scene; }
	Scene* getScene() { return scene; }

	void advanceFrame(uint64_t advanceTime);
	void renderFrame();

	void loadDAT(const File& file, const File& rootDir, const GameInfo* gameInfo = NULL);

	void setGameTime(int8_t h, int8_t m) { gameHours = h; gameMinutes = m; }
	int8_t getGameHours() const { return gameHours; }
	int8_t getGameMinutes() const { return gameMinutes; }
	void getGameTime(int8_t& h, int8_t& m) const { h = gameHours; m = gameMinutes; }
	void advanceGameTime(int8_t h, int8_t m);

	void setViewportSize(int w, int h);
	int getViewportWidth() const { return viewWidth; }
	int getViewportHeight() const { return viewHeight; }

	void setDefaultGameInfo(GameInfo* info) { defGameInfo = info; }
	GameInfo* getDefaultGameInfo() { return defGameInfo; }

	EngineIPLLoader* getIPLLoader() { return &iplLoader; }

	void increaseTestMem(unsigned int m, const char* file, unsigned int line) { testMem += m; }
	unsigned int getTestMem() const { return testMem; }

	void setFreezeVisibility(bool fv) { freezeVisibility = fv; scene->setFreezeVisibility(fv); }
	bool isVisibilityFrozen() const { return freezeVisibility; }

private:
	Engine();
	void iplRecurse(File* file, const File& rootDir, const GameInfo* gameInfo = NULL);

private:
	static Engine* instance;

private:
	GameInfo* defGameInfo;

	vector<ResourceObserver*> resObservers;
	vector<ViewportObserver*> vpObservers;

	Camera* camera;
	Matrix4 projectionMatrix;
	Scene* scene;

	MeshIndexer* meshIndexer;
	TextureIndexer* texIndexer;
	CollisionMeshIndexer* colIndexer;
	AnimationIndexer* animIndexer;

	MeshCacheLoader* meshCacheLoader;
	TextureCacheLoader* texCacheLoader;
	CollisionMeshCacheLoader* colCacheLoader;
	AnimationCacheLoader* animCacheLoader;

	StringResourceCache* meshCache;
	StringResourceCache* texCache;
	StringResourceCache* colCache;
	StringResourceCache* animCache;

	int8_t gameHours, gameMinutes;

	int viewWidth, viewHeight;

	EngineIPLLoader iplLoader;

	unsigned int testMem;

	bool freezeVisibility;
};

#endif /* ENGINE_H_ */

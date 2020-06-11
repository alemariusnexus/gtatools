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

#ifndef ENGINE_H_
#define ENGINE_H_

#include <gta/config.h>
#include "Camera.h"
#include "GameInfo.h"
#include "ItemManager.h"
#include <nxcommon/ResourceCache.h>
#include "EngineIPLLoader.h"
#include "scene/parts/VisualSceneObject.h"
#include "ViewportObserver.h"
#include "render/ShaderPluginAPI.h"
#include <locale>
#include <cstring>
#include <vector>
#include <string>
#include <gtaformats/gta.h>
#include <nxcommon/math/Matrix4.h>
#include <nxcommon/strutil.h>
#include <nxcommon/file/File.h>
#include <nxcommon/cxx11hash.h>
#include <nxcommon/CString.h>
#include <btBulletDynamicsCommon.h>
#include <functional>
#include <memory>

using std::locale;
using std::collate;
using std::use_facet;
using std::vector;
using std::string;
using std::less;
using std::equal_to;
using std::shared_ptr;
using std::unique_ptr;

using gta::Camera;


class ResourceObserver;
class MeshCacheLoader;
class TextureCacheLoader;
class CollisionMeshCacheLoader;
class AnimationCacheLoader;
class PhysicsCacheLoader;
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
	typedef ResourceCache<CString, StringComparator, CXX11Hash<CString>, StringEqual> StringResourceCache;

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

	StringResourceCache* getMeshCache() { return meshCache.get(); }
	StringResourceCache* getTextureCache() { return texCache.get(); }
	StringResourceCache* getCollisionMeshCache() { return colCache.get(); }
	StringResourceCache* getAnimationCache() { return animCache.get(); }
	StringResourceCache* getPhysicsCache() { return physicsCache.get(); }

	MeshIndexer* getMeshIndexer() { return meshIndexer.get(); }
	TextureIndexer* getTextureIndexer() { return texIndexer.get(); }
	CollisionMeshIndexer* getCollisionMeshIndexer() { return colIndexer.get(); }
	AnimationIndexer* getAnimationIndexer() { return animIndexer.get(); }

	ItemManager* getItemManager() { return itemManager.get(); }

	void setCamera(Camera* cam) { camera = cam; }
	Camera* getCamera() { return camera; }

	const Matrix4& getProjectionMatrix() const { return camera->getFrustum().getProjectionMatrix(); }

	void setScene(Scene* scene) { this->scene = scene; }
	Scene* getScene() { return scene; }

	void advanceFrame(uint64_t advanceTime);
	void renderFrame();

	void loadDAT(const File& file, const File& rootDir, GameInfo gameInfo = GameInfo());

	void setGameTime(int8_t h, int8_t m) { gameHours = h; gameMinutes = m; }
	int8_t getGameHours() const { return gameHours; }
	int8_t getGameMinutes() const { return gameMinutes; }
	void getGameTime(int8_t& h, int8_t& m) const { h = gameHours; m = gameMinutes; }
	void advanceGameTime(int8_t h, int8_t m);

	void setViewportSize(int w, int h);
	int getViewportWidth() const { return viewWidth; }
	int getViewportHeight() const { return viewHeight; }

	void setDefaultGameInfo(const GameInfo& info) { defGameInfo = info; }
	GameInfo getDefaultGameInfo() { return defGameInfo; }

	EngineIPLLoader* getIPLLoader() { return &iplLoader; }

	void increaseTestMem(unsigned int m, const char* file, unsigned int line) { testMem += m; }
	unsigned int getTestMem() const { return testMem; }

	void setFreezeVisibility(bool fv) { freezeVisibility = fv; scene->setFreezeVisibility(fv); }
	bool isVisibilityFrozen() const { return freezeVisibility; }

	ShaderPluginAPI* getShaderPluginAPI() { return shaderPluginAPI; }
	void setShaderPluginAPI(ShaderPluginAPI* api) { shaderPluginAPI = api; }

	void setupDebug();

private:
	Engine();
	~Engine();
	void iplRecurse(File* file, const File& rootDir, GameInfo gameInfo = GameInfo());

public:
	void onGLDebugMessage (
			GLenum source,
			GLenum type,
			GLuint id,
			GLenum severity,
			GLsizei length,
			const GLchar* message,
			const void* userParam
			);

private:
	static Engine* instance;

private:
	GameInfo defGameInfo;

	unique_ptr<MeshIndexer> meshIndexer;
	unique_ptr<TextureIndexer> texIndexer;
	unique_ptr<CollisionMeshIndexer> colIndexer;
	unique_ptr<AnimationIndexer> animIndexer;

	// These are owned by the ResourceCaches below
	MeshCacheLoader* meshCacheLoader;
	TextureCacheLoader* texCacheLoader;
	CollisionMeshCacheLoader* colCacheLoader;
	AnimationCacheLoader* animCacheLoader;
	PhysicsCacheLoader* physicsCacheLoader;

	unique_ptr<StringResourceCache> meshCache;
	unique_ptr<StringResourceCache> texCache;
	unique_ptr<StringResourceCache> colCache;
	unique_ptr<StringResourceCache> animCache;
	unique_ptr<StringResourceCache> physicsCache;

	unique_ptr<ItemManager> itemManager;

	Camera* camera;
	Scene* scene;

	Matrix4 projectionMatrix;

	vector<ResourceObserver*> resObservers;
	vector<ViewportObserver*> vpObservers;

	int8_t gameHours, gameMinutes;

	int viewWidth, viewHeight;

	EngineIPLLoader iplLoader;

	unsigned int testMem;

	bool freezeVisibility;

	ShaderPluginAPI* shaderPluginAPI;

	GLenum oglDebugLogLevel;
};

#endif /* ENGINE_H_ */

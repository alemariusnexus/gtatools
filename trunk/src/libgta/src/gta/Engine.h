/*
	Copyright 2010-2011 David "Alemarius Nexus" Lerch

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
#include "ShaderProgram.h"
#include "Camera.h"
#include <locale>
#include <cstring>
#include <vector>
#include <gtaformats/util/math/Matrix4.h>
#include <gtaformats/util/strutil.h>
#include <btBulletDynamicsCommon.h>

using std::locale;
using std::collate;
using std::use_facet;
using std::vector;


class ResourceCache;
class ResourceObserver;
class MeshCacheLoader;
class TextureCacheLoader;
class CollisionMeshCacheLoader;
class MeshIndexer;
class TextureIndexer;
class CollisionMeshIndexer;
class Scene;




class Engine {
public:
	enum VersionMode
	{
		GTAIII,
		GTAVC,
		GTASA
	};

public:
	static Engine* getInstance();
	static void destroy();

public:
	void addResource(const File& file, void (*callback)() = NULL);
	void clearResources();
	ShaderProgram* getCurrentShaderProgram() { return currentShader; }
	void setCurrentShaderProgram(ShaderProgram* program);
	ResourceCache* getMeshCache();
	ResourceCache* getTextureCache();
	ResourceCache* getCollisionMeshCache() { return colCache; }
	MeshIndexer* getMeshIndexer() { return meshIndexer; }
	TextureIndexer* getTextureIndexer() { return texIndexer; }
	CollisionMeshIndexer* getCollisionMeshIndexer() { return colIndexer; }
	void addResourceObserver(ResourceObserver* observer);
	void removeResourceObserver(ResourceObserver* observer);
	void setCamera(Camera* cam) { camera = cam; }
	Camera* getCamera() { return camera; }
	void setProjectionMatrix(const Matrix4& mat) { projectionMatrix = mat; }
	const Matrix4& getProjectionMatrix() const { return projectionMatrix; }
	Matrix4& getProjectionMatrix() { return projectionMatrix; }
	void setScene(Scene* scene) { this->scene = scene; }
	Scene* getScene() { return scene; }
	void render();
	void loadDAT(const File& file, const File& rootDir);
	void setPhysicsWorld(btDiscreteDynamicsWorld* world) { physicsWorld = world; }
	btDiscreteDynamicsWorld* getPhysicsWorld() { return physicsWorld; }
	void switchDrawing() { enableDrawing = !enableDrawing; }
	bool isDrawingEnabled() { return enableDrawing; }
	void setGameTime(int8_t h, int8_t m) { gameHours = h; gameMinutes = m; }
	int8_t getGameHours() const { return gameHours; }
	int8_t getGameMinutes() const { return gameMinutes; }
	void getGameTime(int8_t& h, int8_t& m) const { h = gameHours; m = gameMinutes; }
	void advanceGameTime(int8_t h, int8_t m);
	void setVersionMode(VersionMode mode) { verMode = mode; }
	VersionMode getVersionMode() const { return verMode; }

private:
	Engine();
	void iplRecurse(File* file, const File& rootDir);

private:
	static Engine* instance;

private:
	VersionMode verMode;

	ShaderProgram* currentShader;
	vector<ResourceObserver*> resObservers;
	Camera* camera;
	Matrix4 projectionMatrix;
	Scene* scene;

	MeshIndexer* meshIndexer;
	TextureIndexer* texIndexer;
	CollisionMeshIndexer* colIndexer;

	MeshCacheLoader* meshCacheLoader;
	TextureCacheLoader* texCacheLoader;
	CollisionMeshCacheLoader* colCacheLoader;

	ResourceCache* meshCache;
	ResourceCache* texCache;
	ResourceCache* colCache;

	btDiscreteDynamicsWorld* physicsWorld;

	bool enableDrawing;

	int8_t gameHours, gameMinutes;
};

#endif /* ENGINE_H_ */

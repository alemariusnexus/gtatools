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
 */

#ifndef ENGINE_H_
#define ENGINE_H_

#include "config.h"
#include "ShaderProgram.h"
#include "Camera.h"
#include <locale>
#include <cstring>
#include <vector>
#include <gtaformats/util/math/Matrix4.h>

using std::locale;
using std::collate;
using std::use_facet;
using std::vector;


class ResourceCache;
class ResourceObserver;
class MeshCacheLoader;
class TextureCacheLoader;
class MeshIndexer;
class TextureIndexer;
class Scene;



typedef long hash_t;

inline hash_t Hash(const char* str)
{
	return use_facet< collate<char> >(locale()).hash(str, str+strlen(str));
}


class Engine {
public:
	static Engine* getInstance();

public:
	void addResource(const File& file, void (*callback)() = NULL);
	void clearResources();
	ShaderProgram* getCurrentShaderProgram() { return currentShader; }
	void setCurrentShaderProgram(ShaderProgram* program);
	ResourceCache* getMeshCache();
	ResourceCache* getTextureCache();
	MeshIndexer* getMeshIndexer() { return meshIndexer; }
	TextureIndexer* getTextureIndexer() { return texIndexer; }
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

private:
	Engine();
	void iplRecurse(File* file, const File& rootDir);

private:
	ShaderProgram* currentShader;
	vector<ResourceObserver*> resObservers;
	Camera* camera;
	Matrix4 projectionMatrix;
	Scene* scene;

	MeshIndexer* meshIndexer;
	TextureIndexer* texIndexer;

	MeshCacheLoader* meshCacheLoader;
	TextureCacheLoader* texCacheLoader;

	ResourceCache* meshCache;
	ResourceCache* texCache;
};

#endif /* ENGINE_H_ */

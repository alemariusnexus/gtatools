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

#include "Engine.h"
#include "resource/ResourceObserver.h"
#include "resource/ResourceCache.h"
#include "resource/mesh/MeshCacheLoader.h"
#include "resource/mesh/MeshIndexer.h"
#include "resource/texture/TextureCacheLoader.h"
#include "resource/texture/TextureIndexer.h"
#include "ItemManager.h"
#include "scene/Scene.h"
#include "GLException.h"
#include <cstdio>



Engine* Engine::getInstance()
{
	static Engine inst;
	return &inst;
}


Engine::Engine()
		: currentShader(NULL)
{
	meshIndexer = new MeshIndexer;
	texIndexer = new TextureIndexer;

	meshCacheLoader = new MeshCacheLoader(meshIndexer);
	texCacheLoader = new TextureCacheLoader(texIndexer);

	meshCache = new ResourceCache(meshCacheLoader, 0);
	texCache = new ResourceCache(texCacheLoader, 0);

	addResourceObserver(meshIndexer);
	addResourceObserver(texIndexer);
	addResourceObserver(ItemManager::getInstance());
}


void Engine::addResource(const File& file)
{
	if (file.isDirectory()  ||  file.isArchiveFile()) {
		FileIterator* it = file.getIterator();
		File* child;

		while ((child = it->next())  !=  NULL) {
			addResource(*child);
			delete child;
		}

		delete child;
	} else {
		vector<ResourceObserver*>::iterator it;

		for (it = resObservers.begin() ; it != resObservers.end() ; it++) {
			ResourceObserver* observer = *it;
			observer->resourceAdded(file);
		}
	}
}


void Engine::setCurrentShaderProgram(ShaderProgram* program)
{
	currentShader = program;

	if (program) {
		program->makeCurrent();
	} else {
		ShaderProgram::disableShaders();
	}
}


void Engine::addResourceObserver(ResourceObserver* observer)
{
	resObservers.push_back(observer);
}


ResourceCache* Engine::getMeshCache()
{
	return meshCache;
}


void Engine::render()
{
	GLException::checkError();

	Matrix4 mvpMatrix = projectionMatrix;
	mvpMatrix *= Matrix4::lookAt(camera->getTarget(), camera->getUp());
	const Vector3& cpos = camera->getPosition();
	mvpMatrix.translate(-cpos);

	Scene::ObjectList visibleObjects;
	scene->buildVisibleSceneObjectList(visibleObjects);

	GLuint mvpMatrixUniform = currentShader->getUniformLocation("MVPMatrix");

	Scene::ObjectIterator it;

	for (it = visibleObjects.begin() ; it != visibleObjects.end() ; it++) {
		DefaultSceneObject* obj = *it;
		ItemDefinition* def = obj->getDefinition();

		Matrix4 mat = mvpMatrix * obj->getModelMatrix();
		const float* r = mat.toArray();
		glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, r);
		def->render();
	}
}


ResourceCache* Engine::getTextureCache()
{
	return texCache;
}

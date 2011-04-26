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
#include <gtaformats/util/File.h>
#include <gtaformats/util/strutil.h>
#include <gtaformats/util/util.h>
#include <cstdio>
#include <utility>
#include <algorithm>

using std::streamoff;
using std::find;


struct IndexedSceneObject
{
	DefaultSceneObject* obj;
	int32_t lodIndex;
	bool topLevel;
};



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


void Engine::addResource(const File& file, void (*callback)())
{
	if (file.isDirectory()  ||  file.isArchiveFile()) {
		FileIterator* it = file.getIterator();
		File* child;

		while ((child = it->next())  !=  NULL) {
			addResource(*child, callback);
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


void Engine::removeResourceObserver(ResourceObserver* observer)
{
	vector<ResourceObserver*>::iterator it = find(resObservers.begin(), resObservers.end(), observer);
	resObservers.erase(it);
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
	Scene::ObjectIterator begin = visibleObjects.begin();
	Scene::ObjectIterator end = visibleObjects.end();

	for (it = begin ; it != end ; it++) {
		DefaultSceneObject* obj = *it;
		ItemDefinition* def = obj->getDefinition();

		Matrix4& modelMat = obj->getModelMatrix();
		Matrix4 mat = mvpMatrix * modelMat;
		const float* r = mat.toArray();
		GLException::checkError();
		glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, r);
		GLException::checkError();
		def->render();
	}
}


ResourceCache* Engine::getTextureCache()
{
	return texCache;
}


void Engine::loadDAT(const File& file, const File& rootDir)
{
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

		//printf("Processing DAT line: %s\n", line);

		Engine* engine = Engine::getInstance();

		if (strncmp(line, "IDE", 3) == 0) {
			FilePath path(*rootDir.getPath(), line+4, FilePath::BackslashAsSeparator | FilePath::CorrectCase);
			File ide(&path, false);
			addResource(ide);
		} else if (strncmp(line, "IPL", 3) == 0) {
			FilePath path(*rootDir.getPath(), line+4, FilePath::BackslashAsSeparator | FilePath::CorrectCase);
			File ipl(&path, false);
			iplRecurse(&ipl, rootDir);
		} else if (strncmp(line, "IMG", 3) == 0) {
			FilePath path(*rootDir.getPath(), line+4, FilePath::BackslashAsSeparator | FilePath::CorrectCase);
			File img(&path, false);
			addResource(img);
		}
	}

	delete dat;
}


void Engine::iplRecurse(File* file, const File& rootDir)
{
	//printf("IPL recurse %s\n", file->getPath()->toString());

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

		if (file->guessContentType() == CONTENT_TYPE_IPL) {
			queue<File*> streamingFiles;
			streamingFiles.push(new File(*file));

			const char* fname = file->getPath()->getFileName();

			int baseLen = strlen(fname)-4+7;
			char* baseName = new char[baseLen+1];
			strtolower(baseName, fname);
			char* baseEnd = strchr(baseName, '.');
			strcpy(baseEnd, "_stream");

			for (int i = 0 ; true ; i++) {
				char* sfname = new char[strlen(baseName)+12];
				sprintf(sfname, "%s%d.ipl", baseName, i);
				File* sfile = new File(gta3img, sfname);

				if (sfile->exists()) {
					//printf("  -> %s depends on %s\n", file->getPath()->getFileName(), sfname);
					delete[] sfname;
					streamingFiles.push(sfile);
				} else {
					delete[] sfname;
					delete sfile;
					break;
				}
			}

			delete[] baseName;

			vector<IndexedSceneObject*> localObjs;
			int iplIdx = 0;

			while (!streamingFiles.empty()) {
				File* sfile = streamingFiles.front();
				streamingFiles.pop();

				IPLReader ipl(*sfile);
				IPLStatement* iplStmt;

				delete sfile;

				while ((iplStmt = ipl.readStatement())  !=  NULL) {
					switch (iplStmt->getType()) {
					case IPL_TYPE_INSTANCE:
						IPLInstance* inst = (IPLInstance*) iplStmt;
						int32_t id = inst->getID();
						ItemManager* itemMgr = ItemManager::getInstance();
						ItemDefinition* def = itemMgr->getItem(id);

						if (def) {
							float x, y, z;
							float rx, ry, rz, rw;
							inst->getPosition(x, y, z);
							inst->getRotation(rx, ry, rz, rw);

							float x2, y2, z2, xy, xz, yz, wx, wy, wz;
							x2 = rx*rx;
							y2 = ry*ry;
							z2 = rz*rz;
							xy = rx*ry;
							xz = rx*rz;
							yz = ry*rz;
							wx = rw*rx;
							wy = rw*ry;
							wz = rw*rz;

							Matrix4 modelMatrix (
									1.0f - 2.0f * (y2+z2),	2.0f * (xy-wz),			2.0f * (xz + wy),		0.0f,
									2.0f * (xy+wz),			1.0f - 2.0f * (x2+z2),	2.0f * (yz-wx),			0.0f,
									2.0f * (xz-wy),			2.0f * (yz+wx),			1.0f - 2.0f * (x2+y2),	0.0f,
									x,						y,						z,						1.0f
							);

							IndexedSceneObject* iobj = new IndexedSceneObject;
							iobj->topLevel = true;
							DefaultSceneObject* obj = new DefaultSceneObject(def, modelMatrix, NULL);
							iobj->obj = obj;
							iobj->lodIndex = inst->getLOD();

							localObjs.push_back(iobj);
						} else {
							printf("WARNING: Object with ID %d (%s) not found!\n", id, inst->getModelName());
							localObjs.push_back(NULL);
						}
						iplIdx++;
						break;
					}

					delete iplStmt;
				}
			}

			vector<IndexedSceneObject*>::iterator it;

			for (it = localObjs.begin() ; it != localObjs.end() ; it++) {
				IndexedSceneObject* iobj = *it;

				if (iobj) {
					DefaultSceneObject* obj = iobj->obj;

					if (iobj->lodIndex != -1) {
						if ((uint32_t) iobj->lodIndex >= localObjs.size()) {
							printf("ERROR: LOD index out of bounds: %d\n", iobj->lodIndex);
						}

						IndexedSceneObject* lodParent = localObjs[iobj->lodIndex];

						if (lodParent) {
							lodParent->topLevel = false;
							obj->setLODParent(lodParent->obj);
						}
					}
				}
			}

			for (it = localObjs.begin() ; it != localObjs.end() ; it++) {
				IndexedSceneObject* iobj = *it;

				if (iobj) {
					if (iobj->topLevel) {
						scene->addSceneObject(iobj->obj);
					}

					delete iobj;
				}
			}
		}
	}
}

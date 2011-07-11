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
#include "resource/collision/CollisionMeshCacheLoader.h"
#include "resource/collision/CollisionMeshIndexer.h"
#include "ItemManager.h"
#include "scene/Scene.h"
#include "GLException.h"
#include "EngineException.h"
#include <gtaformats/util/File.h>
#include <gtaformats/util/strutil.h>
#include <gtaformats/util/util.h>
#include <cstdio>
#include <utility>
#include <algorithm>
#include <set>
#include <map>

using std::streamoff;
using std::find;
using std::set;
using std::map;


Engine* Engine::instance = NULL;



struct IndexedSceneObject
{
	DefaultSceneObject* obj;
	bool topLevel;
	int32_t saLodIndex;
	char* vcModelName;
};



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
		: verMode(GTASA), currentShader(NULL), enableDrawing(true), gameHours(8), gameMinutes(0)
{
	meshIndexer = new MeshIndexer;
	texIndexer = TextureIndexer::getInstance();
	colIndexer = new CollisionMeshIndexer;

	meshCacheLoader = new MeshCacheLoader(meshIndexer);
	texCacheLoader = new TextureCacheLoader(texIndexer);
	colCacheLoader = new CollisionMeshCacheLoader(colIndexer);

	meshCache = new ResourceCache(meshCacheLoader, 0);
	texCache = new ResourceCache(texCacheLoader, 0);
	colCache = new ResourceCache(colCacheLoader, 0);

	addResourceObserver(meshIndexer);
	addResourceObserver(texIndexer);
	addResourceObserver(colIndexer);
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

		if (obj->isVisible()) {
			obj->render(mvpMatrix, mvpMatrixUniform);
			physicsWorld->addRigidBody(obj->getRigidBody());
		}
	}

	physicsWorld->stepSimulation(1.0f/60.0f, 10);
	physicsWorld->debugDrawWorld();

	for (it = begin ; it != end ; it++) {
		DefaultSceneObject* obj = *it;

		if (obj->isVisible()) {
			physicsWorld->removeRigidBody(obj->getRigidBody());
		}
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
		} else if (strncmp(line, "TEXDICTION", 10) == 0) {
			FilePath path(*rootDir.getPath(), line+11, FilePath::BackslashAsSeparator | FilePath::CorrectCase);
			File txd(&path, false);
			addResource(txd);
		}
	}

	delete dat;
}


void Engine::iplRecurse(File* file, const File& rootDir)
{
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

			if (verMode == GTASA) {
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
						delete[] sfname;
						streamingFiles.push(sfile);
					} else {
						delete[] sfname;
						delete sfile;
						break;
					}
				}

				delete[] baseName;
			}

			map<hash_t, IndexedSceneObject*> vcLocalObjs;
			vector<IndexedSceneObject*> saLocalObjs;
			int iplIdx = 0;

			while (!streamingFiles.empty()) {
				File* sfile = streamingFiles.front();
				streamingFiles.pop();

				IPLReader ipl(*sfile);
				IPLStatement* iplStmt;

				int localIplIdx = 0;

				while ((iplStmt = ipl.readStatement())  !=  NULL) {
					switch (iplStmt->getType()) {
					case IPL_TYPE_INSTANCE:
						IPLInstance* inst = (IPLInstance*) iplStmt;
						int32_t id = inst->getID();
						ItemManager* itemMgr = ItemManager::getInstance();
						MapItemDefinition* def = (MapItemDefinition*) itemMgr->getItem(id);

						if (def) {
							char* lModelName = NULL; // Needed and created only for GTAVC.

							if (verMode == GTAVC) {
								lModelName = new char[strlen(inst->getModelName()) + 1];
								strtolower(lModelName, inst->getModelName());
							}

							if (	verMode != GTAVC
									||	strncmp(lModelName, "islandlod", 9) != 0
							) {
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

								if (verMode == GTASA) {
									iobj->saLodIndex = inst->getLOD();
									saLocalObjs.push_back(iobj);
								} else if (verMode == GTAVC) {
									iobj->vcModelName = lModelName;
									vcLocalObjs[Hash(iobj->vcModelName)] = iobj;
									if (
												iobj->vcModelName[0] == 'l'
											&&  iobj->vcModelName[1] == 'o'
											&&  iobj->vcModelName[2] == 'd'
									) {
										iobj->topLevel = false;
									}
								} else {
									iobj->saLodIndex = -1;
									saLocalObjs.push_back(iobj);
								}
							}
						} else {
							printf("WARNING: Object with ID %d (%s) not found!\n", id, inst->getModelName());

							if (verMode == GTASA) {
								saLocalObjs.push_back(NULL);
							}
						}
						iplIdx++;
						localIplIdx++;
						break;
					}

					delete iplStmt;
				}

				delete sfile;
			}

			if (verMode == GTASA) {
				vector<IndexedSceneObject*>::iterator it;

				for (it = saLocalObjs.begin() ; it != saLocalObjs.end() ; it++) {
					IndexedSceneObject* iobj = *it;

					if (iobj) {
						DefaultSceneObject* obj = iobj->obj;
						if (iobj->saLodIndex != -1) {
							if ((uint32_t) iobj->saLodIndex >= saLocalObjs.size()) {
								printf("ERROR: LOD index out of bounds: %d\n", iobj->saLodIndex);
							}

							IndexedSceneObject* lodParent = saLocalObjs[iobj->saLodIndex];

							if (lodParent) {
								if (!lodParent->topLevel) {
									// It is possible that two IPL objects point to the same (yes, the SAME, not
									// two equal ones) LOD parent. We'll handle this by making a copy of the
									// parent (having the same object rendered twice confuses the engine).
									DefaultSceneObject* lodParentCopy = new DefaultSceneObject(*lodParent->obj);
									lodParentCopy->setModelMatrix(obj->getModelMatrix());
									obj->setLODParent(lodParentCopy);
								} else {
									lodParent->topLevel = false;
									obj->setLODParent(lodParent->obj);
								}
							}
						}
					}
				}

				for (it = saLocalObjs.begin() ; it != saLocalObjs.end() ; it++) {
					IndexedSceneObject* iobj = *it;

					if (iobj) {
						if (iobj->topLevel) {
							scene->addSceneObject(iobj->obj);
						}

						delete iobj;
					}
				}
			} else if (verMode == GTAVC) {
				map<hash_t, IndexedSceneObject*>::iterator it;

				for (it = vcLocalObjs.begin() ; it != vcLocalObjs.end() ; it++) {
					IndexedSceneObject* iobj = it->second;

					if (iobj  &&  iobj->topLevel) {
						DefaultSceneObject* obj = iobj->obj;
						iobj->vcModelName[0] = 'l';
						iobj->vcModelName[1] = 'o';
						iobj->vcModelName[2] = 'd';

						map<hash_t, IndexedSceneObject*>::iterator parentIt
								= vcLocalObjs.find(Hash(iobj->vcModelName));

						if (parentIt != vcLocalObjs.end()) {
							IndexedSceneObject* lodParent = parentIt->second;

							if (lodParent) {
								if (!lodParent->topLevel) {
									// It is possible that two IPL objects point to the same (yes, the SAME, not
									// two equal ones) LOD parent. We'll handle this by making a copy of the
									// parent (having the same object rendered twice confuses the engine).
									DefaultSceneObject* lodParentCopy = new DefaultSceneObject(*lodParent->obj);
									lodParentCopy->setModelMatrix(obj->getModelMatrix());
									obj->setLODParent(lodParentCopy);
								} else {
									obj->setLODParent(lodParent->obj);
								}
							}
						}

						delete[] iobj->vcModelName;
					}
				}

				for (it = vcLocalObjs.begin() ; it != vcLocalObjs.end() ; it++) {
					IndexedSceneObject* iobj = it->second;

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
}


void Engine::advanceGameTime(int8_t h, int8_t m)
{
	int16_t absTime = ((gameHours*60 + gameMinutes) + (h*60 + m)) % (24*60);
	gameHours = absTime / 60;
	gameMinutes = absTime % 60;
}

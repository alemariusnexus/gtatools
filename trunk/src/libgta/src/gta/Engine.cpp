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
#include "scene/AnimatedSceneObject.h"
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
using std::multimap;


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
		: defGameInfo(NULL), enableDrawing(true), gameHours(8), gameMinutes(0), viewWidth(-1), viewHeight(-1),
		  updateTime(0)
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


void Engine::render()
{
	uint64_t passed;
	uint64_t time = GetTickcount();

	if (updateTime == 0)
		passed = 0;
	else
		passed = time - updateTime;

	updateTime = time;

	/*if (viewWidth < 0  ||  viewHeight < 0) {
		char* errmsg = new char[128];
		sprintf(errmsg, "Error: Viewport dimensions in Engine class are invalid: %dx%d!",
				viewWidth, viewHeight);
		EngineException ex(errmsg, __FILE__, __LINE__);
		delete[] errmsg;
		throw ex;
	}*/

	scene->update(passed);

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

		GameInfo::VersionMode ver = gameInfo->getVersionMode();

		if (file->guessContentType() == CONTENT_TYPE_IPL) {
			queue<File*> streamingFiles;
			streamingFiles.push(new File(*file));

			if (ver == GameInfo::GTASA) {
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

			multimap<const char*, IndexedSceneObject*, StringComparator> vcLocalObjs;
			vector<IndexedSceneObject*> saLocalObjs;
			int iplIdx = 0;

			unsigned int groupObjCount = 0;

			while (!streamingFiles.empty()) {
				File* sfile = streamingFiles.front();
				streamingFiles.pop();

				FilePath* relPath = sfile->getPath()->relativeTo(*rootDir.getPath());
				SceneObjectFileGroup* group = new SceneObjectFileGroup(relPath->toString());
				delete relPath;

				if (sfile->physicallyExists())
					group->setModifyTime(sfile->getModifyTime());
				else
					group->setModifyTime(0);

				scene->getDefinitionDatabase()->addFileGroup(group);

				IPLReader ipl(*sfile);
				IPLStatement* iplStmt;

				uint32_t localIplIdx = 0;

				while ((iplStmt = ipl.readStatement())  !=  NULL) {
					switch (iplStmt->getType()) {
					case IPL_TYPE_INSTANCE:
						IPLInstance* inst = (IPLInstance*) iplStmt;

						if (	inst->getInterior() != 0
								&&  inst->getInterior() != 13
								&&  (ver != GameInfo::GTASA  ||  inst->getInterior() < 256)
						) {
							if (ver == GameInfo::GTASA) {
								saLocalObjs.push_back(NULL);
							}
							break;
						}

						int32_t id = inst->getID();
						ItemManager* itemMgr = ItemManager::getInstance();
						MapItemDefinition* def = (MapItemDefinition*) itemMgr->getItem(id);

						if (def) {
							char* lModelName = NULL; // Needed and created only for GTAVC.

							if (ver == GameInfo::GTAVC) {
								lModelName = new char[strlen(inst->getModelName()) + 1];
								strtolower(lModelName, inst->getModelName());
							}

							if (	ver != GameInfo::GTAVC
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
								iobj->parent = NULL;
								VisualSceneObject* obj;

								/*StaticSceneObject* sobj = new StaticSceneObject(def, modelMatrix, NULL);
								obj = sobj;

								SceneObjectDefinitionInfo* defInfo = new SceneObjectDefinitionInfo(group,
										localIplIdx);
								sobj->setDefinitionInfo(defInfo);

								group->addSceneObject(sobj);*/

								int type = def->getType();

								if (type == ItemTypeStaticMapItem  ||  type == ItemTypeTimedMapItem) {
									StaticSceneObject* sobj = new StaticSceneObject(def, modelMatrix, NULL);
									obj = sobj;

									SceneObjectDefinitionInfo* defInfo = new SceneObjectDefinitionInfo(group,
											localIplIdx);
									sobj->setDefinitionInfo(defInfo);

									group->addSceneObject(sobj);
								} else if (type == ItemTypeAnimatedMapItem) {
									AnimatedMapItemDefinition* adef = (AnimatedMapItemDefinition*) def;
									AnimatedSceneObject* aobj = new AnimatedSceneObject(adef);
									aobj->setModelMatrix(modelMatrix);
									obj = aobj;

									aobj->setCurrentAnimation(adef->getDefaultAnimation());
								}

								iobj->obj = obj;

								if (ver == GameInfo::GTASA) {
									iobj->saLodIndex = inst->getLOD();
									saLocalObjs.push_back(iobj);
								} else if (ver == GameInfo::GTAVC) {
									iobj->vcModelName = lModelName;
									//vcLocalObjs[Hash(iobj->vcModelName)] = iobj;

									vcLocalObjs.insert(pair<const char*, IndexedSceneObject*>(
											iobj->vcModelName, iobj));

									if (
												iobj->vcModelName[0] == 'l'
											&&  iobj->vcModelName[1] == 'o'
											&&  iobj->vcModelName[2] == 'd'
									) {
										iobj->topLevel = false;
									} else {
										iobj->topLevel = true;
									}
								} else {
									iobj->saLodIndex = -1;
									saLocalObjs.push_back(iobj);
								}
							} else {
								if (ver == GameInfo::GTAVC) {
									delete[] lModelName;
								}
							}
						} else {
							printf("WARNING: Object with ID %d (%s) not found!\n", id, inst->getModelName());

							if (ver == GameInfo::GTASA) {
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

				groupObjCount += group->getObjectCount();
			}

			if (ver == GameInfo::GTASA) {
				vector<IndexedSceneObject*>::iterator it;

				for (it = saLocalObjs.begin() ; it != saLocalObjs.end() ; it++) {
					IndexedSceneObject* iobj = *it;

					if (iobj) {
						if (iobj->saLodIndex != -1) {
							if ((uint32_t) iobj->saLodIndex >= saLocalObjs.size()) {
								printf("ERROR: LOD index out of bounds: %d\n", iobj->saLodIndex);
							}

							IndexedSceneObject* lodParent = saLocalObjs[iobj->saLodIndex];

							if (lodParent) {
								iobj->parent = lodParent;

								if (iobj->obj == lodParent->obj) {
									if (iobj->obj->getType() == SceneObjectStatic) {
										printf("LOD parent is the object itself!\n");
									}
									exit(0);
								}

								iobj->obj->setLODParent(lodParent->obj);
								lodParent->children.push_back(iobj);
							}
						}
					}
				}

				uint32_t nextID = 4294967295;

				unsigned int addedCount = 0;

				vector<IndexedSceneObject*> newObjs;

				for (it = saLocalObjs.begin() ; it != saLocalObjs.end() ; it++) {
					IndexedSceneObject* iobj = *it;

					if (iobj) {
						if (iobj->children.empty()) {
							iplMakeUniqueLODHierarchy(iobj, nextID, newObjs);
							scene->addSceneObject(iobj->obj);

							if (iobj->obj->getType() == SceneObjectStatic)
								((StaticSceneObject*) iobj->obj)->getDefinitionInfo()->setLODLeaf(true);

							addedCount++;
						}
					}
				}

#ifndef NDEBUG
				for (it = saLocalObjs.begin() ; it != saLocalObjs.end() ; it++) {
					IndexedSceneObject* iobj = *it;

					if (iobj) {
						if (iobj->children.size() > 1) {
							char* errmsg = new char[256 + strlen(file->getPath()->toString())];
							sprintf(errmsg, "Failed to generate unique IPL LOD hierarchy: Encountered IPL "
									"object with multiple LOD children in '%s' (or one of it's streaming "
									"files).", file->getPath()->toString());
							EngineException ex(errmsg, __FILE__, __LINE__);
							delete[] errmsg;
							throw ex;
						}
					}
				}
#endif

				for (it = saLocalObjs.begin() ; it != saLocalObjs.end() ; it++) {
					IndexedSceneObject* iobj = *it;
					delete iobj;
				}
				for (it = newObjs.begin() ; it != newObjs.end() ; it++) {
					IndexedSceneObject* iobj = *it;
					delete iobj;
				}
			} else if (ver == GameInfo::GTAVC) {
				map<const char*, IndexedSceneObject*, StringComparator>::iterator it;

				for (it = vcLocalObjs.begin() ; it != vcLocalObjs.end() ; it++) {
					IndexedSceneObject* iobj = it->second;

					if (iobj  &&  iobj->topLevel) {
						if (strlen(iobj->vcModelName) >= 3) {
							VisualSceneObject* obj = iobj->obj;
							iobj->vcModelName[0] = 'l';
							iobj->vcModelName[1] = 'o';
							iobj->vcModelName[2] = 'd';

							pair<map<const char*, IndexedSceneObject*, StringComparator>::iterator,
									map<const char*, IndexedSceneObject*, StringComparator>::iterator >
									parentRange = vcLocalObjs.equal_range(iobj->vcModelName);

							map<const char*, IndexedSceneObject*, StringComparator>::iterator nearestParentIt
									= vcLocalObjs.end();
							map<const char*, IndexedSceneObject*, StringComparator>::iterator parentIt;

							if (parentRange.first != parentRange.second) {
								parentIt = parentRange.first;

								if (++parentIt == parentRange.second) {
									// Only one possible parent
									nearestParentIt = parentRange.first;
								} else {
									// Multiple possible parents. This happens when there are multiple IPL entries
									// using the same mesh name. We'll choose the right parent as being the one
									// which is nearest to the child.

									float nearestDist;
									parentIt = parentRange.first;
									for (; parentIt != parentRange.second ; parentIt++) {
										IndexedSceneObject* piobj = parentIt->second;
										const float* mmat = obj->getModelMatrix().toArray();
										const float* pmmat = piobj->obj->getModelMatrix().toArray();
										float xd = pmmat[12] - mmat[12];
										float yd = pmmat[13] - mmat[13];
										float zd = pmmat[14] - mmat[14];
										float dist = sqrtf(xd*xd + yd*yd + zd*zd);

										if (parentIt == parentRange.first  ||  dist < nearestDist) {
											nearestDist = dist;
											nearestParentIt = parentIt;
										}
									}
								}
							}

							parentIt = nearestParentIt;

							if (parentIt != vcLocalObjs.end()) {
								IndexedSceneObject* lodParent = parentIt->second;

								if (lodParent) {
									iobj->parent = lodParent;
									lodParent->children.push_back(iobj);
									obj->setLODParent(lodParent->obj);
								} else {
									iobj->parent = NULL;
								}
							} else {
								iobj->parent = NULL;
							}
						} else {
							iobj->parent = NULL;
						}

						delete[] iobj->vcModelName;
					}
				}

				uint32_t nextID = 4294967295;

				vector<IndexedSceneObject*> newObjs;

				for (it = vcLocalObjs.begin() ; it != vcLocalObjs.end() ; it++) {
					IndexedSceneObject* iobj = it->second;

					if (iobj) {
						if (iobj->topLevel) {
							iplMakeUniqueLODHierarchy(iobj, nextID, newObjs);
							scene->addSceneObject(iobj->obj);

							if (iobj->obj->getType() == SceneObjectStatic)
								((StaticSceneObject*) iobj->obj)->getDefinitionInfo()->setLODLeaf(true);
						}
					}
				}

				for (it = vcLocalObjs.begin() ; it != vcLocalObjs.end() ; it++) {
					IndexedSceneObject* iobj = it->second;
					delete iobj;
				}

				vector<IndexedSceneObject*>::iterator nit;
				for (nit = newObjs.begin() ; nit != newObjs.end() ; nit++) {
					IndexedSceneObject* iobj = *nit;
					delete iobj;
				}
			}
		}

		//scene->compareObjectSizes();
	}
}


void Engine::iplMakeUniqueLODHierarchy(IndexedSceneObject* node, uint32_t& nextID,
		vector<IndexedSceneObject*>& newObjs)
{
	IndexedSceneObject* parent = node->parent;

	if (parent) {
		if (parent->children.size() > 1) {
			// Copy the parent
			IndexedSceneObject* parentCpy = new IndexedSceneObject;
			//parentCpy->obj = new StaticSceneObject(*parent->obj);
			parentCpy->obj = (VisualSceneObject*) parent->obj->clone();
			parentCpy->obj->setModelMatrix(parent->obj->getModelMatrix());

			if (parentCpy->obj->getType() == SceneObjectStatic) {
				StaticSceneObject* sobjCpy = (StaticSceneObject*) parentCpy->obj;
				sobjCpy->getDefinitionInfo()->setID(nextID--);
				sobjCpy->getDefinitionInfo()->getFileGroup()->addSceneObject(sobjCpy);
			}
			parentCpy->parent = parent->parent;
			parentCpy->children.push_back(node);

			newObjs.push_back(parentCpy);

			if (parent->parent)
				parent->parent->children.push_back(parentCpy);

			vector<IndexedSceneObject*>::iterator it = find(parent->children.begin(), parent->children.end(), node);
			parent->children.erase(it);

			parent = parentCpy;

			node->parent = parentCpy;
			node->obj->setLODParent(parentCpy->obj);
		}

		iplMakeUniqueLODHierarchy(parent, nextID, newObjs);
	}
}


void Engine::advanceGameTime(int8_t h, int8_t m)
{
	int16_t absTime = ((gameHours*60 + gameMinutes) + (h*60 + m)) % (24*60);
	gameHours = absTime / 60;
	gameMinutes = absTime % 60;
}

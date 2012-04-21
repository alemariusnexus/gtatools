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

#include "EngineIPLLoader.h"
#include "Engine.h"
#include "ItemManager.h"
#include "MapItemDefinition.h"
#include "scene/objects/MapSceneObject.h"
#include "scene/objects/AnimatedMapSceneObject.h"
#include "EngineException.h"
#include <gtaformats/ipl/IPLReader.h>
#include <gtaformats/ipl/IPLInstance.h>
#include <queue>
#include <map>
#include <utility>
#include <algorithm>
#include <set>

using std::queue;
using std::multimap;
using std::pair;
using std::find;
using std::set;



void EngineIPLLoader::load(const File& file, Scene::ObjectList& objects, const GameInfo* info)
{
	if (!info)
		info = Engine::getInstance()->getDefaultGameInfo();

	GameInfo::VersionMode ver = info->getVersionMode();

	if (ver == GameInfo::GTAIII)
		ver = GameInfo::GTAVC;

	if (file.guessContentType() == CONTENT_TYPE_IPL) {
		IPLStreamingFileProvider::StreamingFileList sfiles;
		sfiles.push_back(file);

		if (sfProv) {
			sfProv->findStreamingFiles(file, sfiles);
		}

		queue<File, IPLStreamingFileProvider::StreamingFileList> streamingFiles(sfiles);

		multimap<const char*, IndexedSceneObject*, StringComparator> vcLocalObjs;
		vector<IndexedSceneObject*> saLocalObjs;
		int iplIdx = 0;

		while (!streamingFiles.empty()) {
			File sfile = streamingFiles.front();
			streamingFiles.pop();

			FilePath* relPath = sfile.getPath()->relativeTo(*info->getRootDirectory().getPath());
			SceneObjectFileGroup* group = new SceneObjectFileGroup(relPath->toString());
			delete relPath;

			group->setChecksum(sfile.crc32());

			IPLReader ipl(sfile);
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
							lModelName = new char[inst->getModelName().length() + 1];
							strtolower(lModelName, inst->getModelName().get());
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

							int type = def->getType();

							if (type == ItemTypeStaticMapItem  ||  type == ItemTypeTimedMapItem) {
								MapSceneObject* sobj = new MapSceneObject(def);
								sobj->setModelMatrix(modelMatrix);
								obj = sobj;

								SceneObjectDefinitionInfo* defInfo = new SceneObjectDefinitionInfo(group,
										localIplIdx);
								sobj->setDefinitionInfo(defInfo);
							} else if (type == ItemTypeAnimatedMapItem) {
								AnimatedMapItemDefinition* adef = (AnimatedMapItemDefinition*) def;
								AnimatedMapSceneObject* aobj = new AnimatedMapSceneObject(adef);
								aobj->setModelMatrix(modelMatrix);
								obj = aobj;

								SceneObjectDefinitionInfo* defInfo = new SceneObjectDefinitionInfo(group,
										localIplIdx);
								aobj->setDefinitionInfo(defInfo);

								aobj->setCurrentAnimation(adef->getDefaultAnimation());
							}

							iobj->obj = obj;

							if (ver == GameInfo::GTASA) {
								iobj->saLodIndex = inst->getLOD();
								saLocalObjs.push_back(iobj);
							} else if (ver == GameInfo::GTAVC) {
								iobj->vcModelName = lModelName;

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
						printf("WARNING: Object with ID %d (%s) not found!\n", id, inst->getModelName().get());

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
								printf("LOD parent is the object itself!\n");
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
						makeUniqueLODHierarchy(iobj, nextID, newObjs);

						if ((iobj->obj->getTypeFlags() & SceneObject::TypeFlagPVS)  !=  0) {
							if (dynamic_cast<PVSSceneObject*>(iobj->obj)->getDefinitionInfo()->isFixed()) {
								printf("Object already fixed!\n");
								exit(0);
							}
							dynamic_cast<PVSSceneObject*>(iobj->obj)->getDefinitionInfo()->setLODLeaf(true);
						}

						//scene->addSceneObject(iobj->obj);
						objects.push_back(iobj->obj);
						addedCount++;
					}
				}
			}

#ifndef NDEBUG
			for (it = saLocalObjs.begin() ; it != saLocalObjs.end() ; it++) {
				IndexedSceneObject* iobj = *it;

				if (iobj) {
					if (iobj->children.size() > 1) {
						char* errmsg = new char[256 + file.getPath()->toString().length()];
						sprintf(errmsg, "Failed to generate unique IPL LOD hierarchy: Encountered IPL "
								"object with multiple LOD children in '%s' (or one of it's streaming "
								"files).", file.getPath()->toString().get());
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
						char* lodModelName = new char[strlen(iobj->vcModelName)+1];
						lodModelName[0] = 'l';
						lodModelName[1] = 'o';
						lodModelName[2] = 'd';
						strcpy(lodModelName+3, iobj->vcModelName+3);

						VisualSceneObject* obj = iobj->obj;

						pair<map<const char*, IndexedSceneObject*, StringComparator>::iterator,
								map<const char*, IndexedSceneObject*, StringComparator>::iterator >
								parentRange = vcLocalObjs.equal_range(lodModelName);

						delete[] lodModelName;

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
								if (iobj->obj == lodParent->obj) {
									throw EngineException("LOD parent is the object itself!",
											__FILE__, __LINE__);
								}

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
				}
			}

			uint32_t nextID = 4294967295;

			vector<IndexedSceneObject*> newObjs;

			for (it = vcLocalObjs.begin() ; it != vcLocalObjs.end() ; it++) {
				IndexedSceneObject* iobj = it->second;

				if (iobj) {
					if (iobj->topLevel) {
						makeUniqueLODHierarchy(iobj, nextID, newObjs);

						if ((iobj->obj->getTypeFlags() & SceneObject::TypeFlagPVS)  !=  0)
							dynamic_cast<PVSSceneObject*>(iobj->obj)->getDefinitionInfo()->setLODLeaf(true);

						//scene->addSceneObject(iobj->obj);
						objects.push_back(iobj->obj);
					}
				}
			}

			for (it = vcLocalObjs.begin() ; it != vcLocalObjs.end() ; it++) {
				IndexedSceneObject* iobj = it->second;
				delete[] iobj->vcModelName;
				delete iobj;
			}

			vector<IndexedSceneObject*>::iterator nit;
			for (nit = newObjs.begin() ; nit != newObjs.end() ; nit++) {
				IndexedSceneObject* iobj = *nit;
				delete iobj;
			}
		}
	}

	set<SceneObject*> lodParents;

/*#ifndef NDEBUG
	for (Scene::ObjectIterator it = objects.begin() ; it != objects.end() ; it++) {
		SceneObject* obj = *it;

		if (!lodParents.insert(obj).second) {
			throw EngineException("IPL scene object has multiple LOD children. LOD hierarchy not unique!",
					__FILE__, __LINE__);
		}
	}
#endif*/
}


void EngineIPLLoader::makeUniqueLODHierarchy(IndexedSceneObject* iobj, uint32_t& nextID,
			vector<IndexedSceneObject*>& newObjs)
{
	IndexedSceneObject* iparent = iobj->parent;

	if (iparent) {
		if (iparent->children.size() > 1) {
			// Clone the parent
			VisualSceneObject* parentCopy = dynamic_cast<VisualSceneObject*>(iparent->obj->clone());

			if ((parentCopy->getTypeFlags() & SceneObject::TypeFlagPVS)  !=  0) {
				dynamic_cast<PVSSceneObject*>(parentCopy)->getDefinitionInfo()->setID(nextID--);
			}

			IndexedSceneObject* iparentCopy = new IndexedSceneObject;
			iparentCopy->obj = parentCopy;
			iparentCopy->parent = iparent->parent;

			iparentCopy->children.push_back(iobj);

			vector<IndexedSceneObject*>::iterator it = find(iparent->children.begin(),
					iparent->children.end(), iobj);
			iparent->children.erase(it);

			if (iparent->parent) {
				iparent->parent->children.push_back(iparent);
			}

			iobj->parent = iparentCopy;
			iobj->obj->setLODParent(parentCopy);

			newObjs.push_back(iparentCopy);
		}

		makeUniqueLODHierarchy(iparent, nextID, newObjs);
	}
}

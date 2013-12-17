/*
	Copyright 2010-2013 David "Alemarius Nexus" Lerch

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



int64_t tellHelper(istream* s)
{
	return s->tellg();
}


void EngineIPLLoader::load(const File& file, Scene::ObjectList& objects, GameInfo info)
{
	if (!info.isValid())
		info = Engine::getInstance()->getDefaultGameInfo();

	GameInfo::VersionMode ver = info.getVersionMode();

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

			FilePath* relPath = sfile.getPath()->relativeTo(*info.getRootDirectory().getPath());
			SceneObjectFileGroup* group = new SceneObjectFileGroup(relPath->toString());
			delete relPath;

			group->setChecksum(sfile.crc32());

			istream* stream = sfile.openInputStream(istream::binary);
			IPLReader ipl(stream);
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

							Vector3 pos(x, y, z);
							Quaternion rot(rw, rx, ry, rz);

							IndexedSceneObject* iobj = new IndexedSceneObject;
							iobj->isRoot = true;
							iobj->hasAssociatedParent = false;
							iobj->id = id;
							iobj->pos = pos;
							iobj->rot = rot;

							MapSceneObjectLODInstance* lodInst = new MapSceneObjectLODInstance(def);
							lodInst->debugID = id;

							iobj->lodInst = lodInst;

							if (ver == GameInfo::GTASA) {
								iobj->saLodChildIndex = inst->getLOD();
								iobj->isLeaf = (inst->getLOD() == -1);
								saLocalObjs.push_back(iobj);
							} else if (ver == GameInfo::GTAVC) {
								iobj->vcModelName = lModelName;

								vcLocalObjs.insert(pair<const char*, IndexedSceneObject*>(
										iobj->vcModelName, iobj));

								// TODO This seems to be the wrong way round.
								if (
											iobj->vcModelName[0] == 'l'
										&&  iobj->vcModelName[1] == 'o'
										&&  iobj->vcModelName[2] == 'd'
								) {
									iobj->isLeaf = true;
									iobj->isRoot = false;
								} else {
									iobj->isLeaf = false;
									iobj->isRoot = true;
								}
							} else {
								iobj->saLodChildIndex = -1;
								iobj->isLeaf = true;
								saLocalObjs.push_back(iobj);
							}

							// Actually, because the definition info is only used on a per MapSceneObject
							// basis, we only need this info for top-level objects. HOWEVER, it may be
							// possible that some objects are decoupled from their original LOD hierarchy and
							// therefor become top-level in the further process (see the hack below), so we
							// keep this info for all objects until we know exactly.
							iobj->defInfo = new SceneObjectDefinitionInfo(group, localIplIdx);
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





		// **********************************************************
		// *			BUILD LOD HIERARCHY FOR GTA SA				*
		// **********************************************************

		// Key is the IPL local index of the hierarchy's root object
		map<uint32_t, TemporaryLODHierarchy*> tmpHierarchies;

		if (ver == GameInfo::GTASA) {
			uint32_t nextUnusedIdx = saLocalObjs.size();

			uint32_t numObjs = saLocalObjs.size();


			// STEP 1: Find objects with multiple parents to fix them, and find out which objects
			// aren't actually root objects (at this point, all their isRoot flags are set).
			for (uint32_t saLocalIdx = 0 ; saLocalIdx < numObjs ; saLocalIdx++) {
				IndexedSceneObject* iobj = saLocalObjs[saLocalIdx];

				if (iobj  &&  !iobj->isLeaf) {
					IndexedSceneObject* ciobj = saLocalObjs[iobj->saLodChildIndex];

					if (ciobj->hasAssociatedParent) {
						// ciobj has multiple parents. We handle this by copying ciobj and all
						// its children to make them unique.

						// Loop through ciobj and all its children
						IndexedSceneObject* iobjOrig = ciobj;
						do {
							IndexedSceneObject* iobjCpy = new IndexedSceneObject;
							iobjCpy->lodInst = new MapSceneObjectLODInstance(*iobjOrig->lodInst);
							iobjCpy->saLodChildIndex = nextUnusedIdx+1;
							iobjCpy->isLeaf = iobjOrig->isLeaf;
							iobjCpy->isRoot = false;
							iobjCpy->hasAssociatedParent = true;
							iobjCpy->pos = iobjOrig->pos;
							iobjCpy->rot = iobjOrig->rot;
							iobjCpy->id = iobjOrig->id;

							SceneObjectDefinitionInfo* infoCpy =
									new SceneObjectDefinitionInfo(*iobjOrig->defInfo);
							infoCpy->setID(nextUnusedIdx);
							iobjCpy->defInfo = infoCpy;

							if (iobjOrig == ciobj) {
								iobj->saLodChildIndex = nextUnusedIdx;
							}

							saLocalObjs.push_back(iobjCpy);

							nextUnusedIdx++;

							if (iobjOrig->isLeaf)
								iobjOrig = NULL;
							else
								iobjOrig = saLocalObjs[iobjOrig->saLodChildIndex];
						} while (iobjOrig);
					} else {
						ciobj->isRoot = false;
						ciobj->hasAssociatedParent = true;
					}
				}
			}

			numObjs = saLocalObjs.size();

			// By now, there is at most a 1:1 relation between child and parent:
			//
			// A single object can never have multiple children, because there is only one child
			// LOD index for each object in IPL.
			// All cases of objects with multiple parents were resolved in STEP 1.
			//
			// This means that the data structure is basically not a tree anymore, but a number
			// of linked lists, each of which contains all LOD instances of an object.


			// STEP 2: Build a TemporaryLODHierarchy for each root object and add all its
			// children ("recursively") as LOD instances.
			for (uint32_t saLocalIdx = 0 ; saLocalIdx < numObjs ; saLocalIdx++) {
				IndexedSceneObject* iobj = saLocalObjs[saLocalIdx];

				if (iobj) {
					if (iobj->isRoot) {
						TemporaryLODHierarchy* h = new TemporaryLODHierarchy;
						h->rootInst = iobj;
						h->insts.push_back(iobj);
						MapItemDefinition* def = iobj->lodInst->getDefinition();

						if (def->getType() == ItemTypeAnimatedMapItem)
							h->animated = true;
						else
							h->animated = false;

						IndexedSceneObject* ciobj = iobj;
						while (!ciobj->isLeaf) {
							ciobj = saLocalObjs[ciobj->saLodChildIndex];
							h->insts.push_back(ciobj);
						}

						tmpHierarchies.insert(pair<uint32_t, TemporaryLODHierarchy*> (
								saLocalIdx, h));
					}
				}
			}


			// STEP 3: For each TemporaryLODHierarchy, create a single MapSceneObject and populate it
			// with the already created MapSceneObjectLODInstances. Also calculate the model matrices
			// of all the LOD instances relative to the root object's model matrix.
			map<uint32_t, TemporaryLODHierarchy*>::iterator hit;
			for (hit = tmpHierarchies.begin() ; hit != tmpHierarchies.end() ; hit++) {
				TemporaryLODHierarchy* h = hit->second;

				list<IndexedSceneObject*>::iterator iit;
				MapSceneObject* obj;

				Matrix4 modelMatrix = Matrix4::fromQuaternionVector(h->rootInst->pos,
						h->rootInst->rot);

				if (h->animated) {
					AnimatedMapSceneObject* aobj = new AnimatedMapSceneObject;
					aobj->setModelMatrix(modelMatrix);
					aobj->setAutoPickDefaultAnimation(true);
					obj = aobj;
				} else {
					obj = new MapSceneObject;
					obj->setModelMatrix(modelMatrix);
				}

				obj->setDefinitionInfo(h->rootInst->defInfo);

				Matrix4 invModelMatrix = modelMatrix.inverse();

				// Handle the LOD instances, creating relative model matrices etc.
				for (iit = h->insts.begin() ; iit != h->insts.end() ; iit++) {
					IndexedSceneObject* iobj = *iit;

					MapSceneObjectLODInstance* lodInst = iobj->lodInst;

					if (iobj != h->rootInst) {
						Matrix4 relMat = invModelMatrix * Matrix4::fromQuaternionVector (
								iobj->pos, iobj->rot);
						lodInst->setRelativeModelMatrix(relMat);
						delete iobj->defInfo;
					}

					obj->addLODInstance(lodInst);

					delete iobj;
				}

				objects.push_back(obj);
			}
		}





		// **********************************************************
		// *			BUILD LOD HIERARCHY FOR GTA VC				*
		// **********************************************************

		else if (ver == GameInfo::GTAVC) {
			multimap<const char*, IndexedSceneObject*, StringComparator>::iterator it;

			// For VC, the LOD hierarchy is built by looking at the model names of IPL objects:
			//
			// Every object is assumed to be a root object, unless its name starts with the
			// letters 'LOD'. For each root object, a single LOD child can be provided by
			// changing the first three letters of the root model name to 'LOD'. In case of
			// multiple LOD children with the same model name, we choose the one whose
			// coordinates are nearest to the root object.

			for (it = vcLocalObjs.begin() ; it != vcLocalObjs.end() ; it++) {
				IndexedSceneObject* iobj = it->second;

				if (iobj  &&  iobj->isRoot) {
					IndexedSceneObject* lodChild = NULL;

					if (strlen(iobj->vcModelName) >= 3) {
						char* lodModelName = new char[strlen(iobj->vcModelName)+1];
						lodModelName[0] = 'l';
						lodModelName[1] = 'o';
						lodModelName[2] = 'd';
						strcpy(lodModelName+3, iobj->vcModelName+3);

						// Find all possible LOD children.
						pair<multimap<const char*, IndexedSceneObject*, StringComparator>::iterator,
								multimap<const char*, IndexedSceneObject*, StringComparator>::iterator >
								childRange = vcLocalObjs.equal_range(lodModelName);

						delete[] lodModelName;

						multimap<const char*, IndexedSceneObject*, StringComparator>::iterator nearestChildIt
								= vcLocalObjs.end();
						multimap<const char*, IndexedSceneObject*, StringComparator>::iterator childIt;

						if (childRange.first != childRange.second) {
							childIt = childRange.first;

							if (++childIt == childRange.second) {
								if (!childRange.first->second->hasAssociatedParent) {
									// Only one possible child
									nearestChildIt = childRange.first;
								}
							} else {
								// Multiple possible children. This happens when there are multiple IPL entries
								// using the same mesh name. We'll choose the right child as being the one
								// which is nearest to the root object.

								float nearestDist;
								childIt = childRange.first;
								for (; childIt != childRange.second ; childIt++) {
									IndexedSceneObject* ciobj = childIt->second;

									if (ciobj->hasAssociatedParent)
										continue;

									Vector3 basePos = iobj->pos;
									Vector3 cPos = ciobj->pos;

									Vector3 distVec = cPos - basePos;
									float dist = distVec.length();

									if (childIt == childRange.first  ||  dist < nearestDist) {
										nearestDist = dist;
										nearestChildIt = childIt;
									}
								}
							}
						}

						childIt = nearestChildIt;

						if (childIt != vcLocalObjs.end()) {
							lodChild = childIt->second;
						}
					}

					MapSceneObject* obj;

					Matrix4 modelMatrix = Matrix4::fromQuaternionVector(iobj->pos, iobj->rot);

					bool animated = iobj->lodInst->getDefinition()->getType() == ItemTypeAnimatedMapItem
							||  (lodChild  &&  lodChild->lodInst->getDefinition()->getType() == ItemTypeAnimatedMapItem);

					if (animated) {
						AnimatedMapSceneObject* aobj = new AnimatedMapSceneObject;
						aobj->setModelMatrix(modelMatrix);
						aobj->setAutoPickDefaultAnimation(true);
						obj = aobj;
					} else {
						obj = new MapSceneObject;
						obj->setModelMatrix(modelMatrix);
					}

					obj->setDefinitionInfo(iobj->defInfo);

					Matrix4 invModelMatrix = modelMatrix.inverse();

					iobj->lodInst->setRelativeModelMatrix(Matrix4::Identity);
					obj->addLODInstance(iobj->lodInst);

					if (lodChild) {
						lodChild->hasAssociatedParent = true;

						Matrix4 relMat = invModelMatrix * Matrix4::fromQuaternionVector (
								lodChild->pos, lodChild->rot);
						lodChild->lodInst->setRelativeModelMatrix(relMat);
						delete lodChild->defInfo;

						obj->addLODInstance(lodChild->lodInst);
					}

					objects.push_back(obj);
				}
			}

			for (it = vcLocalObjs.begin() ; it != vcLocalObjs.end() ; it++) {
				IndexedSceneObject* iobj = it->second;
				delete[] iobj->vcModelName;
				delete iobj;
			}
		}
	}

	set<SceneObject*> lodParents;
}


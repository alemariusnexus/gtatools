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
								iobj->saLodIndex = inst->getLOD();
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
									iobj->isLeaf = false;
								} else {
									iobj->isLeaf = true;
								}
							} else {
								iobj->saLodIndex = -1;
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

			// Find the root objects
			uint32_t numObjs = saLocalObjs.size();

			for (uint32_t saLocalIdx = 0 ; saLocalIdx < numObjs ; saLocalIdx++) {
				IndexedSceneObject* iobj = *it;

				if (iobj  &&  !iobj->isLeaf) {
					IndexedSceneObject* ciobj = saLocalObjs[iobj->saLodIndex];

					if (ciobj->hasAssociatedParent) {
						// Copy ciobj and all its children.

						IndexedSceneObject* iobjOrig = ciobj;
						do {
							IndexedSceneObject* iobjCpy = new IndexedSceneObject;
							iobjCpy->lodInst = new MapSceneObjectLODInstance(*iobjOrig->lodInst);
							iobjCpy->saLodIndex = nextUsedIdx+1;

						} while (iobjOrig);
					} else {
						ciobj->isRoot = false;
						ciobj->hasAssociatedParent = true;
					}
				}
			}



			/*vector<IndexedSceneObject*>::iterator it;

			uint32_t nextUnusedIdx = saLocalObjs.size();

			uint32_t saLocalIdx = 0;
			for (it = saLocalObjs.begin() ; it != saLocalObjs.end() ; it++, saLocalIdx++) {
				IndexedSceneObject* iobj = *it;

				if (iobj) {
					uint32_t lodBaseIdx = saLocalIdx;
					IndexedSceneObject* lodBase;

					if (iobj->saLodIndex != -1) {
						if ((uint32_t) iobj->saLodIndex >= saLocalObjs.size()) {
							printf("ERROR: LOD index out of bounds: %d\n", iobj->saLodIndex);
						}

						IndexedSceneObject* lodParent = iobj;

						while (lodParent->saLodIndex != -1) {
							lodBaseIdx = lodParent->saLodIndex;
							lodParent = saLocalObjs[lodBaseIdx];
						}

						lodBase = lodParent;
					} else {
						lodBase = iobj;
					}

					pair<map<uint32_t, TemporaryLODHierarchy*>::iterator, bool> res
							= tmpHierarchies.insert(pair<uint32_t, TemporaryLODHierarchy*>(
									lodBaseIdx, NULL));
					TemporaryLODHierarchy* h = res.first->second;

					if (res.second) {
						res.first->second = new TemporaryLODHierarchy;
						h = res.first->second;
						h->animated = false;
					}

					h->insts.push_back(iobj);

					if (!iobj->topLevel) {
						// TODO This is a very, very dirty hack...

						// There are a few cases in San Andreas where multiple object instances share the
						// SAME LOD parent instance. libgta, however, does not support multiple LOD children
						// unless they all have exactly the same streaming distance. To support these cases
						// nonetheless, we just remove all but the last of these children from their parent
						// and place them as new top-level objects here.

						IndexedSceneObject* lodParent = saLocalObjs[iobj->saLodIndex];
						pair<map<IndexedSceneObject*, IndexedSceneObject*>::iterator, bool> ares
								= h->parentChildAssociations.insert(
										pair<IndexedSceneObject*, IndexedSceneObject*>(lodParent, iobj));

						if (!ares.second) {
							// Apparently, the correct way to do this is to remove all but the LAST of the LOD
							// children from their parent. Previously, all but the FIRST were removed, but this
							// lead to problems with certain objects (consider objects with IDs 11216, 10933
							// and 11229 in SFSe.ipl, but note that the objects' coordinates can be somewhat
							// misleading as to which object is the correct child).

#if 0
							IndexedSceneObject* prevChild = ares.first->second;
							ares.first->second = iobj;

							list<IndexedSceneObject*>::iterator iit;
							for (iit = h->insts.begin() ; iit != h->insts.end() ; iit++) {
								IndexedSceneObject* hiobj = *iit;
								if (hiobj == prevChild) {
									h->insts.erase(iit);
									break;
								}
							}

							uint32_t newIdx = nextUnusedIdx++;

							prevChild->saLodIndex = -1;
							prevChild->topLevel = true;
							prevChild->defInfo = new SceneObjectDefinitionInfo(*lodBase->defInfo);
							prevChild->defInfo->setID(newIdx);

							TemporaryLODHierarchy* fakeH = new TemporaryLODHierarchy;
							fakeH->animated = prevChild->lodInst->getDefinition()->getType() == ItemTypeAnimatedMapItem;
							fakeH->topLevelInst = prevChild;
							fakeH->insts.push_back(prevChild);
							tmpHierarchies.insert(pair<uint32_t, TemporaryLODHierarchy*>(newIdx, fakeH));

							continue;
#else
							list<IndexedSceneObject*>::iterator iit;
							for (iit = h->insts.begin() ; iit != h->insts.end() ; iit++) {
								IndexedSceneObject* hiobj = *iit;
								if (hiobj == iobj) {
									h->insts.erase(iit);
									break;
								}
							}

							uint32_t newIdx = nextUnusedIdx++;

							iobj->saLodIndex = -1;
							iobj->topLevel = true;
							iobj->defInfo = new SceneObjectDefinitionInfo(*lodBase->defInfo);
							iobj->defInfo->setID(newIdx);

							TemporaryLODHierarchy* fakeH = new TemporaryLODHierarchy;
							fakeH->animated = iobj->lodInst->getDefinition()->getType() == ItemTypeAnimatedMapItem;
							fakeH->topLevelInst = iobj;
							fakeH->insts.push_back(iobj);
							tmpHierarchies.insert(pair<uint32_t, TemporaryLODHierarchy*>(newIdx, fakeH));

							continue;
#endif
						}
					}

					MapItemDefinition* def = iobj->lodInst->getDefinition();
					if (!h->animated  &&  def->getType() == ItemTypeAnimatedMapItem) {
						h->animated = true;
					}

					if (iobj->topLevel) {
						h->topLevelInst = iobj;
					}
				}
			}

			map<uint32_t, TemporaryLODHierarchy*>::iterator hit;
			for (hit = tmpHierarchies.begin() ; hit != tmpHierarchies.end() ; hit++) {
				TemporaryLODHierarchy* h = hit->second;
				IndexedSceneObject* bottomIobj;

				list<IndexedSceneObject*>::iterator iit;

				for (iit = h->insts.begin() ; iit != h->insts.end() ; iit++) {
					IndexedSceneObject* iobj = *iit;

					if (h->parentChildAssociations.find(iobj) == h->parentChildAssociations.end()) {
						bottomIobj = iobj;
						break;
					}
				}

				MapSceneObject* obj;

				Matrix4 modelMatrix = Matrix4::fromQuaternionVector(bottomIobj->pos, bottomIobj->rot);

				if (h->animated) {
					AnimatedMapSceneObject* aobj = new AnimatedMapSceneObject;
					aobj->setModelMatrix(modelMatrix);
					aobj->setAutoPickDefaultAnimation(true);
					obj = aobj;
				} else {
					obj = new MapSceneObject;
					obj->setModelMatrix(modelMatrix);
				}

				obj->setDefinitionInfo(bottomIobj->defInfo);

				Matrix4 invModelMatrix = modelMatrix.inverse();

				if (h->topLevelInst->id == 3716) {
					obj->special = true;
				}

				for (iit = h->insts.begin() ; iit != h->insts.end() ; iit++) {
					IndexedSceneObject* iobj = *iit;

					MapSceneObjectLODInstance* lodInst = iobj->lodInst;

					if (lodInst->getSceneObject())
						lodInst = new MapSceneObjectLODInstance(*lodInst);

					if (iobj != bottomIobj) {
						Matrix4 relMat = invModelMatrix * Matrix4::fromQuaternionVector(iobj->pos, iobj->rot);
						lodInst->setRelativeModelMatrix(relMat);
						delete iobj->defInfo;
					}

					obj->addLODInstance(lodInst);

					delete iobj;
				}

				objects.push_back(obj);
			}*/
		}





		// **********************************************************
		// *			BUILD LOD HIERARCHY FOR GTA VC				*
		// **********************************************************

		else if (ver == GameInfo::GTAVC) {
			multimap<const char*, IndexedSceneObject*, StringComparator>::iterator it;

			for (it = vcLocalObjs.begin() ; it != vcLocalObjs.end() ; it++) {
				IndexedSceneObject* iobj = it->second;
				// 2279.0242, -1234.8101, 23.9911
				if (iobj  &&  iobj->topLevel) {
					if (strlen(iobj->vcModelName) >= 3) {
						char* lodModelName = new char[strlen(iobj->vcModelName)+1];
						lodModelName[0] = 'l';
						lodModelName[1] = 'o';
						lodModelName[2] = 'd';
						strcpy(lodModelName+3, iobj->vcModelName+3);

						pair<multimap<const char*, IndexedSceneObject*, StringComparator>::iterator,
								multimap<const char*, IndexedSceneObject*, StringComparator>::iterator >
								parentRange = vcLocalObjs.equal_range(lodModelName);

						delete[] lodModelName;

						multimap<const char*, IndexedSceneObject*, StringComparator>::iterator nearestParentIt
								= vcLocalObjs.end();
						multimap<const char*, IndexedSceneObject*, StringComparator>::iterator parentIt;

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
									Vector3 basePos = iobj->pos;
									Vector3 pPos = piobj->pos;

									Vector3 distVec = pPos - basePos;
									float dist = distVec.length();

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
								MapSceneObjectLODInstance* lodInst = lodParent->lodInst;

								if (lodInst->getSceneObject())
									lodInst = new MapSceneObjectLODInstance(*lodInst);

								//iobj->rootObj->addLODInstance(lodInst);
							}
						}
					}

					//objects.push_back(iobj->rootObj);
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


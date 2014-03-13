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

#ifndef ENGINEIPLLOADER_H_
#define ENGINEIPLLOADER_H_

#include "GameInfo.h"
#include "scene/Scene.h"
#include <nxcommon/file/File.h>
#include "IPLStreamingFileProvider.h"
#include "scene/parts/VisualSceneObject.h"
#include "scene/objects/MapSceneObjectLODInstance.h"
#include <vector>
#include <list>

using std::vector;
using std::list;



/**
 *
 *	LOD Hierarchies:
 *
 *	A LOD hierarchy is a bunch of objects with different streaming distances that represent different
 *	levels of detail (LOD) of the same object, and of which usually at most one is active, based on
 *	the distance of the camera to the object. Suppose you have three objects, OBJ100, OBJ250, OBJ500,
 *	forming a LOD hierarchy, where the number in each object's name describes its streaming distance.
 *	A LOD hierarchy of these objects typically looks like this:
 *
 *	OBJ100
 *	|
 *	----OBJ250
 *	    |
 *	    ----OBJ500
 *
 *	The object with the lowest streaming distance (which should be the most detailed one), OBJ100, is
 *	called the "LOD root", whereas the one with the highest streaming distance (which should be the
 *	least detailed one), OBJ500, is called a "LOD leaf". The "LOD parent" of a given object is the
 *	object with the next LOWER streaming distance. The parent of OBJ500 is therefore OBJ250, and the
 *	parent of OBJ250 is OBJ100.
 *
 *	In San Andreas, it is possible that a LOD hierarchy looks like this:
 *
 *	OBJ100_1
 *	|
 *	----OBJ250
 *	    |
 *	    ----OBJ500
 *
 *	OBJ100_2
 *	|
 *	----OBJ250
 *	    |
 *	    ----OBJ500
 *
 *	This means that a single child object (OBJ250) may have multiple parent objects (OBJ100_1 and
 *	OBJ100_2). This is not supported by gtatools. Therefore, whenever an object has multiple parents,
 *	it is copied along with all of its children for each parent but the first. The above hierarchy
 *	would be converted to the following:
 *
 *	OBJ100_1
 *	|
 *	----OBJ250
 *	    |
 *	    ----OBJ500
 *
 *	OBJ100_2
 *	|
 *	----OBJ250_2
 *	    |
 *	    ----OBJ500_2
 */
class EngineIPLLoader
{
private:
	struct IndexedSceneObject
	{
		MapSceneObjectLODInstance* lodInst;
		int32_t saLodChildIndex;
		bool isLeaf;
		bool isRoot;
		bool hasAssociatedParent;
		char* vcModelName;
		Vector3 pos;
		Quaternion rot;
		int32_t id;
		SceneObjectDefinitionInfo* defInfo;
	};
	struct TemporaryLODHierarchy
	{
		list<IndexedSceneObject*> insts;
		map<IndexedSceneObject*, IndexedSceneObject*> parentChildAssociations;
		IndexedSceneObject* rootInst;
		bool animated;
	};

public:
	EngineIPLLoader() : sfProv(NULL) {}
	void load(const File& file, Scene::ObjectList& objects, GameInfo info = GameInfo());
	void setStreamingFileProvider(IPLStreamingFileProvider* sfProv) { this->sfProv = sfProv; }
	IPLStreamingFileProvider* getStreamingFileProvider() { return sfProv; }

private:
	/*void makeUniqueLODHierarchy(IndexedSceneObject* iobj, uint32_t& nextID,
			vector<IndexedSceneObject*>& newObjs);*/

private:
	IPLStreamingFileProvider* sfProv;
};

#endif /* ENGINEIPLLOADER_H_ */

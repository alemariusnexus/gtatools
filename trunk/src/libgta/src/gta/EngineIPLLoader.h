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

#ifndef ENGINEIPLLOADER_H_
#define ENGINEIPLLOADER_H_

#include "GameInfo.h"
#include "scene/Scene.h"
#include <gtaformats/util/File.h>
#include "IPLStreamingFileProvider.h"
#include "scene/parts/VisualSceneObject.h"
#include "scene/objects/MapSceneObjectLODInstance.h"
#include <vector>
#include <list>

using std::vector;
using std::list;


class EngineIPLLoader
{
private:
	struct IndexedSceneObject
	{
		MapSceneObjectLODInstance* lodInst;
		//MapSceneObject* rootObj;
		int32_t saLodIndex;
		bool topLevel;
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
		IndexedSceneObject* topLevelInst;
		bool animated;
	};

public:
	EngineIPLLoader() : sfProv(NULL) {}
	void load(const File& file, Scene::ObjectList& objects, const GameInfo* info = NULL);
	void setStreamingFileProvider(IPLStreamingFileProvider* sfProv) { this->sfProv = sfProv; }
	IPLStreamingFileProvider* getStreamingFileProvider() { return sfProv; }

private:
	/*void makeUniqueLODHierarchy(IndexedSceneObject* iobj, uint32_t& nextID,
			vector<IndexedSceneObject*>& newObjs);*/

private:
	IPLStreamingFileProvider* sfProv;
};

#endif /* ENGINEIPLLOADER_H_ */

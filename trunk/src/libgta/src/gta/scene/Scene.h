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

#ifndef SCENE_H_
#define SCENE_H_

#include "SceneObjectDefinitionDatabase.h"
#include "Renderer.h"
#include <gtaformats/util/StringComparator.h>
#include "../ShaderProgram.h"
#include <list>
#include <map>

using std::list;
using std::map;


class PVSDatabase;



class Scene {
private:
	typedef map<const char*, SceneObjectFileGroup*, StringComparator> FileGroupMap;

public:
	typedef list<SceneObject*> ObjectList;
	typedef ObjectList::iterator ObjectIterator;
	typedef ObjectList::const_iterator ConstObjectIterator;

	typedef list<PVSSceneObject*> PVSObjectList;
	typedef PVSObjectList::iterator PVSObjectIterator;
	typedef PVSObjectList::const_iterator ConstPVSObjectIterator;

public:
	Scene();
	~Scene();

	template <class ItType>
	void addSceneObjects(ItType begin, ItType end);

	void addSceneObject(SceneObject* obj);
	void clear();
	ObjectIterator getSceneObjectBegin() { return objects.begin(); }
	ObjectIterator getSceneObjectEnd() { return objects.end(); }
	ConstObjectIterator getSceneObjectBegin() const { return objects.begin(); }
	ConstObjectIterator getSceneObjectEnd() const { return objects.end(); }
	ObjectList::size_type getSceneObjectCount() { return objects.size(); }
	void buildVisibleSceneObjectList(ObjectList& list);
	//DefaultSceneObject* getObjectByID(int id) { return objects[id]; }
	int getLastPotentiallyVisibleObjectCount() const { return pvObjCount; }
	int getLastVisibleObjectCount() const { return visibleObjCount; }
	void setDrawDistanceMultiplier(float ddm) { ddMultiplier = ddm; }
	float getDrawDistanceMultiplier() const { return ddMultiplier; }
	Renderer* getRenderer() { return renderer; }
	void setRenderer(Renderer* r) { renderer = r; }
	void update(uint64_t timePassed);
	void present();
	void setPVSDatabase(PVSDatabase* pvs) { this->pvs = pvs; }
	PVSDatabase* getPVSDatabase() { return pvs; }
	void setPVSEnabled(bool pe) { pvsEnabled = pe; }
	bool isPVSEnabled() const { return pvsEnabled; }

private:
	inline bool addIfVisible(SceneObject* obj, ObjectList& list, float cx, float cy, float cz)
	{
		Vector3 pos = obj->getPosition();

		float dx = cx - pos.getX();
		float dy = cy - pos.getY();
		float dz = cz - pos.getZ();

		float distSq = dx*dx + dy*dy + dz*dz;

		while (obj) {
			float dd = obj->getStreamingDistance();

			if (dd == 0.0f  ||  dd*dd - distSq > 0.0f) {
				list.push_back(obj);
				return true;
			} else {
				obj = obj->getLODParent();
			}
		}

		return false;
	}

private:
	ObjectList objects;
	ObjectList dynamicObjs;
	ObjectList animObjs;
	PVSDatabase* pvs;
	int pvObjCount, visibleObjCount;
	float ddMultiplier;
	Renderer* renderer;
	int nextStaticObjID;
	bool pvsEnabled;
};




template <class ItType>
void Scene::addSceneObjects(ItType begin, ItType end)
{
	for (ItType it = begin ; it != end ; it++) {
		SceneObject* obj = dynamic_cast<SceneObject*>(*it);
		addSceneObject(obj);
	}
}

#endif /* SCENE_H_ */

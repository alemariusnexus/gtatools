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

#include "Scene.h"
#include "AnimatedSceneObject.h"
#include "../Engine.h"
#include "visibility/PVSDatabase.h"
#include <fstream>
#include <utility>
#include <gtaformats/util/util.h>
#include <set>
#include <map>
#include <algorithm>

using std::ofstream;
using std::min;
using std::set;
using std::map;
using std::sort;
using std::copy;
using std::pair;


struct SortableSceneObject
{
	SortableSceneObject(SceneObject* obj, float dist) : obj(obj), dist(dist) {}
	bool operator<(const SortableSceneObject& other) const { return dist > other.dist; }
	SceneObject* obj;
	float dist;
};




Scene::Scene()
		: pvs(NULL), pvObjCount(0), visibleObjCount(0), ddMultiplier(1.0f)
{
}


Scene::~Scene()
{
	delete pvs;
}


void Scene::addSceneObject(SceneObject* obj)
{
	objects.push_back(obj);

	if (obj->getType() != SceneObjectStatic) {
		dynamicObjs.push_back(obj);

		if (obj->getType() == SceneObjectAnimated) {
			animObjs.push_back(obj);
		}
	}
}


void Scene::clear()
{
	objects.clear();
	dynamicObjs.clear();
	animObjs.clear();
}


void Scene::buildVisibleSceneObjectList(ObjectList& list)
{
	Engine* engine = Engine::getInstance();

	Camera* cam = engine->getCamera();
	const Vector3& cpos = cam->getPosition();

	float cx = cpos.getX();
	float cy = cpos.getY();
	float cz = cpos.getZ();

	ObjectList& pvObjects = objects;

	if (pvs) {
		ObjectList realPVObjects;
		pvObjects = realPVObjects;
		pvs->queryPVS(cx, cy, cz, pvObjects);
		pvObjects.insert(pvObjects.end(), dynamicObjs.begin(), dynamicObjs.end());
	}

	pvObjCount = pvObjects.size();

	ObjectIterator it;
	ObjectIterator begin = pvObjects.begin();
	ObjectIterator end = pvObjects.end();

	int actuallyRendered = 0;

	for (it = begin ; it != end ; it++) {
		SceneObject* obj = *it;

		Vector3 pos = obj->getPosition();

		float dx = cx - pos.getX();
		float dy = cy - pos.getY();
		float dz = cz - pos.getZ();

		float distSq = dx*dx + dy*dy + dz*dz;

		while (obj) {
			float dd = obj->getDrawDistance();
			float distDiff = dd*dd - distSq;

			if (distDiff > 0.0f) {
				list.push_back(obj);
				actuallyRendered++;
				break;
			} else {
				obj = obj->getLODParent();
			}
		}
	}

	visibleObjCount = actuallyRendered;
}


void Scene::update(uint64_t timePassed)
{
	for (ObjectIterator it = animObjs.begin() ; it != animObjs.end() ; it++) {
		SceneObject* obj = *it;
		AnimatedSceneObject* aobj = (AnimatedSceneObject*) obj;
		aobj->increaseAnimationTime(timePassed / 1000.0f);
	}
}


void Scene::present()
{
	ObjectList visObjs;
	buildVisibleSceneObjectList(visObjs);

	for (ObjectIterator it = visObjs.begin() ; it != visObjs.end() ; it++) {
		SceneObject* obj = *it;
		int type = obj->getType();

		if (type == SceneObjectStatic  ||  type == SceneObjectAnimated) {
			renderer->enqueueForRendering((VisualSceneObject*) obj);
		}
	}

	renderer->render();
}

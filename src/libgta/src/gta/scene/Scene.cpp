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
#include "parts/AnimatedSceneObject.h"
#include "parts/PVSSceneObject.h"
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
		: pvs(NULL), pvObjCount(0), visibleObjCount(0), ddMultiplier(1.0f), pvsEnabled(false)
{
}


Scene::~Scene()
{
	delete pvs;
}


void Scene::addSceneObject(SceneObject* obj)
{
	objects.push_back(obj);

	SceneObject::typeflags_t tf = obj->getTypeFlags();

	if ((tf & SceneObject::TypeFlagPVS)  ==  0) {
		dynamicObjs.push_back(obj);
	} else {
		pvs->addObject(dynamic_cast<PVSSceneObject*>(obj));
	}

	if ((tf & SceneObject::TypeFlagAnimated)  !=  0) {
		animObjs.push_back(obj);
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
	if (pvsEnabled)
		pvs->calculatePVS();

	Engine* engine = Engine::getInstance();

	Camera* cam = engine->getCamera();
	const Vector3& cpos = cam->getPosition();

	float cx = cpos.getX();
	float cy = cpos.getY();
	float cz = cpos.getZ();

	PVSObjectList pvObjs;
	ObjectList& nonPVObjs = objects;

	if (pvsEnabled) {
		nonPVObjs = dynamicObjs;
		pvs->queryPVS(cx, cy, cz, pvObjs);
	}

	pvObjCount = pvObjs.size();

	int actuallyRendered = 0;

	for (PVSObjectIterator it = pvObjs.begin() ; it != pvObjs.end() ; it++) {
		PVSSceneObject* obj = *it;

		if (obj->isEnabled())
			actuallyRendered += addIfVisible(obj, list, cx, cy, cz) ? 1 : 0;
	}

	for (ObjectIterator it = nonPVObjs.begin() ; it != nonPVObjs.end() ; it++) {
		SceneObject* obj = *it;

		if (obj->isEnabled())
			actuallyRendered += addIfVisible(obj, list, cx, cy, cz) ? 1 : 0;
	}

	visibleObjCount = actuallyRendered;
}


void Scene::update(uint64_t timePassed)
{
	for (ObjectIterator it = animObjs.begin() ; it != animObjs.end() ; it++) {
		SceneObject* obj = *it;
		AnimatedSceneObject* aobj = dynamic_cast<AnimatedSceneObject*>(obj);

		if (aobj->isAutoAnimationEnabled())
			aobj->increaseAnimationTime(timePassed / 1000.0f);
	}
}


void Scene::present()
{
	ObjectList visObjs;
	buildVisibleSceneObjectList(visObjs);

	for (ObjectIterator it = visObjs.begin() ; it != visObjs.end() ; it++) {
		SceneObject* obj = *it;
		//int type = obj->getType();

		SceneObject::typeflags_t tf = obj->getTypeFlags();

		//if (type == SceneObjectStatic  ||  type == SceneObjectAnimated) {
		if ((tf & SceneObject::TypeFlagVisual)  !=  0) {
			renderer->enqueueForRendering(dynamic_cast<VisualSceneObject*>(obj));
		} else if ((tf & SceneObject::TypeFlagLight)  !=  0) {
			renderer->enqueueForRendering(dynamic_cast<LightSource*>(obj));
		}
	}

	renderer->render();
}

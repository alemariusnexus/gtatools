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

#include "Scene.h"
#include "../Engine.h"
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


struct SortableSceneObject
{
	SortableSceneObject(StaticSceneObject* obj, float dist) : obj(obj), dist(dist) {}
	bool operator<(const SortableSceneObject& other) const { return dist > other.dist; }
	StaticSceneObject* obj;
	float dist;
};




Scene::Scene()
		: alphaObjBegin(objects.begin()), pvs(PVSData(this)), pvsValid(false), pvObjCount(0),
		  visibleObjCount(0), ddMultiplier(1.0f)
{
}


void Scene::addSceneObject(StaticSceneObject* obj)
{
	int id = objects.size();
	obj->setID(id);

	if (obj->getDefinition()->hasAlphaTransparency()) {
		objects.push_back(obj);
	} else {
		objects.insert(alphaObjBegin, obj);
	}

	if (id == 0)
		alphaObjBegin = objects.begin();

	pvsValid = false;
}


void Scene::buildVisibleSceneObjectList(ObjectList& list)
{
	if (!pvsValid) {
		pvs.build();
		pvsValid = true;
	}

	Engine* engine = Engine::getInstance();

	Camera* cam = engine->getCamera();
	const Vector3& cpos = cam->getPosition();

	float cx = cpos.getX();
	float cy = cpos.getY();
	float cz = cpos.getZ();

#ifdef GTA_VISIBILITY_PVS
	ObjectList pvObjects;
	pvs.queryPVS(cx, cy, cz, pvObjects);
#else
	ObjectList& pvObjects = objects;
#endif

	pvObjCount = pvObjects.size();

	ObjectIterator it;
	ObjectIterator begin = pvObjects.begin();
	ObjectIterator end = pvObjects.end();

	int actuallyRendered = 0;

	for (it = begin ; it != end ; it++) {
		StaticSceneObject* obj = *it;

		const float* mat = obj->getModelMatrix().toArray();
		float ox = mat[12];
		float oy = mat[13];
		float oz = mat[14];

		float dx = cx - ox;
		float dy = cy - oy;
		float dz = cz - oz;

		float distSq = dx*dx + dy*dy + dz*dz;

		while (obj) {
			MapItemDefinition* def = obj->getDefinition();
			float dd = def->getDrawDistance();
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

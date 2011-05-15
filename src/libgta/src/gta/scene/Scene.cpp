/*
 * Scene.cpp
 *
 *  Created on: 07.03.2011
 *      Author: alemariusnexus
 */

#include "Scene.h"
#include "../Engine.h"
#include <fstream>
#include <utility>
#include <gtaformats/util/util.h>
#include <set>
#include <map>

using std::ofstream;
using std::min;
using std::set;
using std::map;



Scene::Scene()
		: pvs(PVSData(this)), pvsValid(false), pvObjCount(0), visibleObjCount(0), ddMultiplier(1.0f)
{
}


void Scene::addSceneObject(DefaultSceneObject* obj)
{
	obj->setID(objects.size());
	objects.push_back(obj);
	pvsValid = false;
}


void Scene::buildVisibleSceneObjectList(ObjectList& list)
{
	if (!pvsValid) {
		File pvsFile("/home/alemariusnexus/pvs.pvs");

		if (!pvsFile.exists()  ||  !pvs.unserialize(pvsFile)) {
			pvs.build();
		}

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
		DefaultSceneObject* obj = *it;
		DefaultSceneObject* baseObj = obj;

		const float* mat = obj->getModelMatrix().toArray();
		float ox = mat[12];
		float oy = mat[13];
		float oz = mat[14];

		float dx = cx - ox;
		float dy = cy - oy;
		float dz = cz - oz;

		float distSq = dx*dx + dy*dy + dz*dz;

		while (obj) {
			ItemDefinition* def = obj->getDefinition();
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

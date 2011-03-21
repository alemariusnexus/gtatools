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

using std::ofstream;
using std::min;



Scene::Scene()
		: pvs(PVSData(this)), pvsValid(false)
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

		if (pvsFile.exists()) {
			pvs.unserialize(pvsFile);
		} else {
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

	ObjectList pvObjects;
	pvs.queryPVS(cx, cy, cz, pvObjects);

	printf("PVS algorithm chose %d of %d objects (%.2f%%) for narrow distance testing\n",
			pvObjects.size(), objects.size(), ((float) pvObjects.size() / (float) objects.size())*100.0f);

	ObjectIterator it;

	for (it = pvObjects.begin() ; it != pvObjects.end() ; it++) {
		DefaultSceneObject* obj = *it;

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
			float distDiff = def->getDrawDistanceSquarred() - distSq;

			if (distDiff > 0.0f) {
				list.push_back(obj);
				break;
			} else {
				obj = obj->getLODParent();
			}
		}
	}
}

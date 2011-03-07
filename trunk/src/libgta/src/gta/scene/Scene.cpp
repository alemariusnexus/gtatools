/*
 * Scene.cpp
 *
 *  Created on: 07.03.2011
 *      Author: alemariusnexus
 */

#include "Scene.h"
#include "../Engine.h"



void Scene::addSceneObject(DefaultSceneObject* obj)
{
	objects.push_back(obj);
}


void Scene::buildVisibleSceneObjectList(ObjectList& list)
{
	Engine* engine = Engine::getInstance();
	Camera* cam = engine->getCamera();
	const Vector3& cpos = cam->getPosition();

	float cx = cpos.getX();
	float cy = cpos.getY();
	float cz = cpos.getZ();

	ObjectList::iterator it;

	for (it = objects.begin() ; it != objects.end() ; it++) {
		DefaultSceneObject* obj = *it;
		const float* mat = obj->getModelMatrix().toArray();
		float ox = mat[12];
		float oy = mat[13];
		float oz = mat[14];

		float dx = cx - ox;
		float dy = cy - oy;
		float dz = cz - oz;

		float dist = sqrt(dx*dx + dy*dy + dz*dz);

		while (obj) {
			ItemDefinition* def = obj->getDefinition();
			float distDiff = def->getDrawDistance() - dist;

			if (distDiff > 0.0f) {
				list.push_back(obj);
				break;
			} else {
				obj = obj->getLODParent();
			}
		}
	}
}

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
#include "parts/RigidBodySceneObject.h"
#include "../Engine.h"
#include "visibility/PVSDatabase.h"
#include "../EngineException.h"
#include <fstream>
#include <utility>
#include <gtaformats/util/util.h>
#include <set>
#include <map>
#include <algorithm>
#include "objects/MapSceneObject.h"

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
		: renderer(NULL), reGenerator(new RenderingEntityGenerator), pvsEnabled(false), fcEnabled(true),
		  physicsWorld(NULL), freezeVisibility(false)
{
}


Scene::~Scene()
{
}


void Scene::addSceneObject(SceneObject* obj)
{
	objects.push_back(obj);

	SceneObject::typeflags_t tf = obj->getTypeFlags();

	if ((tf & SceneObject::TypeFlagPVS)  ==  0) {
		dynamicObjs.push_back(obj);
	} else {
		pvs.addObject(dynamic_cast<PVSSceneObject*>(obj));
	}

	if ((tf & SceneObject::TypeFlagAnimated)  !=  0) {
		animObjs.push_back(obj);
	}

	if ((tf & SceneObject::TypeFlagLight)  !=  0) {
		lightSources.push_back(dynamic_cast<LightSource*>(obj));
	}

	/*if ((tf & SceneObject::TypeFlagRigidBody)  !=  0) {
		RigidBodySceneObject* rbObj = dynamic_cast<RigidBodySceneObject*>(obj);

		btRigidBody* rb = rbObj->getRigidBody();

		if (rb) {
			physicsWorld->addRigidBody(rb);
			delete rb->getCollisionShape();
		}
	}*/
}


void Scene::clear()
{
	objects.clear();
	dynamicObjs.clear();
	animObjs.clear();
}


template <class ItType>
void Scene::buildVisibleSceneObjectList(StreamingViewpoint* svp, ItType beg, ItType end)
{
	int svpFlags = svp->getStreamingFlags();

	bool visSvp = (svpFlags & StreamingViewpoint::GraphicsStreaming) != 0;
	bool physSvp = (svpFlags & StreamingViewpoint::PhysicsStreaming) != 0;
	bool fcSvp = (svpFlags & StreamingViewpoint::FrustumCulling) != 0;

	Frustum frustum = svp->getCullingFrustum();

	float sdMul = svp->getStreamingDistanceMultiplier();
	Vector3 svpPos = svp->getStreamingViewpointPosition();

	float sx = svpPos.getX();
	float sy = svpPos.getY();
	float sz = svpPos.getZ();

	for (ItType it = beg ; it != end ; it++) {
		SceneObject* obj = dynamic_cast<SceneObject*>(*it);

		Vector3 dv = svpPos - obj->getPosition();
		float dist = sqrt(dv.dot(dv));

		float sd = obj->getStreamingDistance() * sdMul;

		if (sd == 0.0f  ||  sd - dist > 0.0f) {
			if (visSvp  &&  (obj->getTypeFlags() & SceneObject::TypeFlagVisual)  !=  0) {
				VisualSceneObject* vobj = dynamic_cast<VisualSceneObject*>(obj);

				if (fcEnabled  &&  fcSvp) {
					Vector3 bCenter;
					float bRadius;
					vobj->getBoundingSphere(bCenter, bRadius);

					if (frustum.computeSphere(bCenter, bRadius) == Frustum::Outside) {
						continue;
					}
				}

				vobj->updateRenderingDistance(dist, sdMul);

				if (!obj->sceneGraphicsVisible) {
					curVisObjs.push_back(vobj);
					obj->sceneGraphicsVisible = true;
				}
			}
			if (physSvp  &&  (obj->getTypeFlags() & SceneObject::TypeFlagRigidBody)  !=  0) {
				RigidBodySceneObject* rbobj = dynamic_cast<RigidBodySceneObject*>(obj);

				if (!obj->scenePhysicsVisible) {
					curRbObjs.push_back(rbobj);
					obj->scenePhysicsVisible = true;
				}
			}
		}
	}
}


void Scene::buildVisibleSceneObjectList(VisualObjectList& visObjs, RigidBodyObjectList& rbObjs)
{
	if (pvsEnabled)
		pvs.calculatePVS();

	Engine* engine = Engine::getInstance();

	if (pvsEnabled) {
		for (StreamingViewpointIterator it = svList.begin() ; it != svList.end() ; it++) {
			StreamingViewpoint* vp = *it;

			list<PVSSceneObject*> pvObjs;

			Vector3 pos = vp->getStreamingViewpointPosition();
			float sdMul = vp->getStreamingDistanceMultiplier();
			float chosenSdMul;

			if (pvs.queryPVS(pvObjs, pos, sdMul, &chosenSdMul)) {
				// PVS query was successful

				// Process the dynamic objects
				buildVisibleSceneObjectList(vp, dynamicObjs.begin(), dynamicObjs.end());

				// Then, process all objects delivered by PVS for this viewpoint
				buildVisibleSceneObjectList(vp, pvObjs.begin(), pvObjs.end());
			} else {
				// PVS query was NOT sucessful, so we manually process all objects here
				buildVisibleSceneObjectList(vp, objects.begin(), objects.end());
			}
		}
	} else {
		for (StreamingViewpointIterator it = svList.begin() ; it != svList.end() ; it++) {
			StreamingViewpoint* vp = *it;
			buildVisibleSceneObjectList(vp, objects.begin(), objects.end());
		}
	}
}


void Scene::updateVisibility()
{
	curVisObjs.clear();
	curRbObjs.clear();
	buildVisibleSceneObjectList(curVisObjs, curRbObjs);
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
	if (!renderer) {
		throw EngineException("Attempt to present a Scene without a Renderer specified!", __FILE__, __LINE__);
	}

	list<RenderingEntity*> rEntities;

	for (VisualObjectIterator it = curVisObjs.begin() ; it != curVisObjs.end() ; it++) {
		VisualSceneObject* obj = *it;

		obj->sceneGraphicsVisible = false;

		SceneObject::typeflags_t tf = obj->getTypeFlags();
	}

	reGenerator->generate(curVisObjs.begin(), curVisObjs.end(), rEntities);

	for (list<RenderingEntity*>::iterator it = rEntities.begin() ; it != rEntities.end() ; it++) {
		RenderingEntity* re = *it;
		renderer->enqueueForRendering(re);
	}

	for (LightSourceIterator it = lightSources.begin() ; it != lightSources.end() ; it++) {
		renderer->enqueueForRendering(*it);
	}

	renderer->render();

	if (!freezeVisibility) {
		for (VisualObjectIterator it = curVisObjs.begin() ; it != curVisObjs.end() ; it++) {
			VisualSceneObject* obj = *it;
			obj->resetRenderingDistance();
		}
	}
}

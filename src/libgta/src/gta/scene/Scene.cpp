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

#include "Scene.h"
#include "parts/AnimatedSceneObject.h"
#include "parts/PVSSceneObject.h"
#include "parts/RigidBodySceneObject.h"
#include "../Engine.h"
#include "visibility/PVSDatabase.h"
#include "../EngineException.h"
#include <fstream>
#include <utility>
#include <nxcommon/util.h>
#include <set>
#include <map>
#include <algorithm>
#include "objects/MapSceneObject.h"
#include "objects/Vehicle.h"

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
		: streamer(new StreamingManager()), renderer(NULL), reGenerator(new RenderingEntityGenerator),
		  pvsEnabled(false), fcEnabled(true), physicsWorld(NULL), freezeVisibility(false)
{
	btDefaultCollisionConfiguration* config = new btDefaultCollisionConfiguration();
	btCollisionDispatcher* dispatcher = new btCollisionDispatcher(config);
	btBroadphaseInterface* broadphase = new btDbvtBroadphase();
	btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver();
	physicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, config);

	physicsWorld->setGravity(btVector3(0.0f, 0.0f, -9.81f));

	streamer->addStreamingListener(this);

	testPlugin = new TestShaderPlugin(0);
	//testPlugin->addUniformBuffer(&testBuf);
	//testReg.installPlugin(testPlugin);

	timeVal = 0;
}


int64_t streamCount = 0;


Scene::~Scene()
{
	delete streamer;
	delete physicsWorld;
	delete reGenerator;

	for (SceneObject* obj : objects) {
		delete obj;
	}
}


void Scene::addSceneObject(SceneObject* obj)
{
	objects.push_back(obj);

	SceneObject::typeflags_t tf = obj->getTypeFlags();

	if ((tf & SceneObject::TypeFlagAnimated)  !=  0) {
		animObjs.push_back(obj);
	}

	if ((tf & SceneObject::TypeFlagLight)  !=  0) {
		lightSources.push_back(dynamic_cast<LightSource*>(obj));
	} else {
		streamer->addSceneObject(obj);
	}

	MapSceneObject* mobj = dynamic_cast<MapSceneObject*>(obj);


	if (mobj) {
		//ShaderPluginRegistry reg = testReg;
		//UniformBuffer* buf = new UniformBuffer;
		//buf->setUniformInt("TestIndex", objIndex);
		//reg.addUniformBuffer(buf);
		//mobj->setShaderPluginRegistry(reg);
	}
}


void Scene::addStreamingViewpoint(StreamingViewpoint* vp)
{
	svList.push_back(vp);
	streamer->addViewpoint(vp);
}


void Scene::streamed(SceneObject* obj, uint32_t inBuckets, uint32_t outBuckets)
{
	if ((inBuckets & StreamingManager::VisibleBucket)  !=  0) {
		curVisObjs.push_back(dynamic_cast<VisualSceneObject*>(obj));
		streamCount++;
	} else if ((outBuckets & StreamingManager::VisibleBucket)  !=  0) {
		curVisObjs.remove(dynamic_cast<VisualSceneObject*>(obj));
		streamCount--;
	}

	if ((inBuckets & StreamingManager::PhysicsBucket)  !=  0) {
		MapSceneObject* mobj = dynamic_cast<MapSceneObject*>(obj);
		SimpleDynamicSceneObject* sdObj = dynamic_cast<SimpleDynamicSceneObject*>(obj);
		Vehicle* veh = dynamic_cast<Vehicle*>(obj);

		if (mobj) {
			MapItemDefinition* def = mobj->getLODInstance()->getDefinition();
			PhysicsPointer* pptr = def->getPhysicsPointer();

			if (pptr) {
				btRigidBody* rb = mobj->getRigidBody();
				btCollisionShape* shape = pptr->get(true);

				rb->setCollisionShape(shape);

				physicsWorld->addRigidBody(rb);
			}
		} else if (sdObj) {
			btRigidBody* rb = sdObj->getRigidBody();

			PhysicsPointer* pptr = sdObj->getPhysicsPointer();

			btCollisionShape* shape = pptr->get(true);
			rb->setCollisionShape(shape);

			physicsWorld->addRigidBody(rb);
		} else if (veh) {
			veh->streamIn(physicsWorld);

			/*//btRigidBody* rb = veh->getRigidBody();

			PhysicsPointer* pptr = veh->getPhysicsPointer();

			btCollisionShape* shape = pptr->get(true);

			btVector3 inertia(0.0f, 0.0f, 0.0f);
			shape->calculateLocalInertia(1.0f, inertia);

			btRigidBody::btRigidBodyConstructionInfo info(veh->getMass(), veh, shape, inertia);
			btRigidBody* rb = new btRigidBody(info);
			rb->setRestitution(0.2f);
			rb->setCcdMotionThreshold(0.00001f);
			rb->setCcdSweptSphereRadius(5.0f);

			delete veh->getRigidBody();
			veh->setRigidBody(rb);

			physicsWorld->addRigidBody(rb);*/
		}
	} else if ((outBuckets & StreamingManager::PhysicsBucket)  !=  0) {
		MapSceneObject* mobj = dynamic_cast<MapSceneObject*>(obj);
		SimpleDynamicSceneObject* sdObj = dynamic_cast<SimpleDynamicSceneObject*>(obj);
		Vehicle* veh = dynamic_cast<Vehicle*>(obj);

		if (mobj) {
			MapItemDefinition* def = mobj->getLODInstance()->getDefinition();
			PhysicsPointer* pptr = def->getPhysicsPointer();

			if (pptr) {
				pptr->release();

				btRigidBody* rb = mobj->getRigidBody();
				rb->setCollisionShape(NULL);

				physicsWorld->removeRigidBody(rb);
			}
		} else if (sdObj) {
			btRigidBody* rb = sdObj->getRigidBody();

			PhysicsPointer* pptr = sdObj->getPhysicsPointer();
			pptr->release();

			rb->setCollisionShape(NULL);

			physicsWorld->removeRigidBody(rb);
		} else if (veh) {
			veh->streamOut(physicsWorld);

			/*btRigidBody* rb = veh->getRigidBody();

			PhysicsPointer* pptr = veh->getPhysicsPointer();
			pptr->release();

			rb->setCollisionShape(NULL);

			physicsWorld->removeRigidBody(rb);*/
		}
	}
}


void Scene::clear()
{
	streamer->clear();

	objects.clear();
	animObjs.clear();
}


void Scene::updateVisibility()
{
	if (!freezeVisibility)
		streamer->update();
}


void Scene::update(uint64_t timePassed)
{
	/*for (ObjectIterator it = animObjs.begin() ; it != animObjs.end() ; it++) {
		SceneObject* obj = *it;
		AnimatedSceneObject* aobj = dynamic_cast<AnimatedSceneObject*>(obj);

		if (aobj->isAutoAnimationEnabled()) {
			aobj->increaseAnimationTime(timePassed / 1000.0f);
		}
	}*/

	for (SceneObject* obj : curVisObjs) {
		AnimatedSceneObject* aobj = dynamic_cast<AnimatedSceneObject*>(obj);

		if (aobj  &&  aobj->isAutoAnimationEnabled()) {
			aobj->increaseAnimationTime(timePassed / 1000.0f);
		}
	}

	physicsWorld->stepSimulation(timePassed/1000.0f, 0);
	//testBuf.setUniformInt("Time", timeVal);

	timeVal += timePassed;
}


void Scene::present()
{
	uint64_t absS, absE, s, e;
	absS = GetTickcountMicroseconds();

	if (!renderer) {
		throw EngineException("Attempt to present a Scene without a Renderer specified!", __FILE__, __LINE__);
	}

	list<RenderingEntity*> rEntities;

	s = GetTickcountMicroseconds();

	reGenerator->generate(curVisObjs.begin(), curVisObjs.end(), rEntities);

	e = GetTickcountMicroseconds();
	uint64_t t1 = e-s;
	s = GetTickcountMicroseconds();

	renderer->enqueueForRendering(rEntities.begin(), rEntities.end());

	e = GetTickcountMicroseconds();
	uint64_t t2 = e-s;
	s = GetTickcountMicroseconds();

	for (LightSourceIterator it = lightSources.begin() ; it != lightSources.end() ; it++) {
		renderer->enqueueForRendering(*it);
	}

	e = GetTickcountMicroseconds();
	uint64_t t3 = e-s;
	s = GetTickcountMicroseconds();

	renderer->render();

	e = GetTickcountMicroseconds();
	uint64_t t4 = e-s;
	s = GetTickcountMicroseconds();

	if (!freezeVisibility) {
		for (VisualObjectIterator it = curVisObjs.begin() ; it != curVisObjs.end() ; it++) {
			VisualSceneObject* obj = *it;
			obj->resetRenderingDistance();
		}
	}

	e = GetTickcountMicroseconds();
	uint64_t t5 = e-s;

	absE = GetTickcountMicroseconds();

	/*printf("=== SCENE TIMING ===\n");
	printf("Absolute:       %uus\n", (unsigned int) (absE-absS));
	printf("T1:             %uus\n", (unsigned int) t1);
	printf("T2:             %uus\n", (unsigned int) t2);
	printf("T3:             %uus\n", (unsigned int) t3);
	printf("T4:             %uus\n", (unsigned int) t4);
	printf("T5:             %uus\n", (unsigned int) t5);

	printf("\n");*/
}

/*
	Copyright 2010-2013 David "Alemarius Nexus" Lerch

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
		: streamer(new StreamingManager()), renderer(NULL), reGenerator(new RenderingEntityGenerator),
		  pvsEnabled(false), fcEnabled(true), physicsWorld(NULL), freezeVisibility(false)
{
	streamer->addStreamingListener(this);
}


Scene::~Scene()
{
	delete streamer;
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
	} else if ((outBuckets & StreamingManager::VisibleBucket)  !=  0) {
		curVisObjs.remove(dynamic_cast<VisualSceneObject*>(obj));
	}
}


void Scene::clear()
{
	objects.clear();
	animObjs.clear();
}


void Scene::updateVisibility()
{
	streamer->update();
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

	reGenerator->generate(curVisObjs.begin(), curVisObjs.end(), rEntities);

	renderer->enqueueForRendering(rEntities.begin(), rEntities.end());

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

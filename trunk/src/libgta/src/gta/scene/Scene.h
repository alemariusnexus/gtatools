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

#ifndef SCENE_H_
#define SCENE_H_

#include "parts/RigidBodySceneObject.h"
#include "SceneObjectDefinitionDatabase.h"
#include "../render/Renderer.h"
#include "../render/RenderingEntityGenerator.h"
#include "objects/LightSource.h"
#include "StreamingViewpoint.h"
#include "StreamingManager.h"
#include "visibility/PVSDatabase.h"
#include <gtaformats/util/StringComparator.h>
#include "../ShaderProgram.h"
#include "../render/TestShaderPlugin.h"
#include "../render/ShaderPluginRegistry.h"
#include <list>
#include <map>
#include <btBulletDynamicsCommon.h>

using std::list;
using std::map;





class Scene : public StreamingManager::StreamingListener
{
private:
	typedef map<const char*, SceneObjectFileGroup*, StringComparator> FileGroupMap;

public:
	typedef list<SceneObject*> ObjectList;
	typedef ObjectList::iterator ObjectIterator;
	typedef ObjectList::const_iterator ConstObjectIterator;

	typedef list<VisualSceneObject*> VisualObjectList;
	typedef VisualObjectList::iterator VisualObjectIterator;
	typedef VisualObjectList::const_iterator ConstVisualObjectIterator;

	typedef list<RigidBodySceneObject*> RigidBodyObjectList;
	typedef RigidBodyObjectList::iterator RigidBodyObjectIterator;
	typedef RigidBodyObjectList::const_iterator ConstRigidBodyObjectIterator;

	typedef list<PVSSceneObject*> PVSObjectList;
	typedef PVSObjectList::iterator PVSObjectIterator;
	typedef PVSObjectList::const_iterator ConstPVSObjectIterator;

	typedef list<StreamingViewpoint*> StreamingViewpointList;
	typedef StreamingViewpointList::iterator StreamingViewpointIterator;
	typedef StreamingViewpointList::const_iterator ConstStreamingViewpointIterator;

	typedef list<LightSource*> LightSourceList;
	typedef LightSourceList::iterator LightSourceIterator;
	typedef LightSourceList::const_iterator ConstLightSourceIterator;

public:
	Scene();
	virtual ~Scene();

	template <class ItType>
	void addSceneObjects(ItType begin, ItType end);

	void addSceneObject(SceneObject* obj);
	void clear();
	ObjectIterator getSceneObjectBegin() { return objects.begin(); }
	ObjectIterator getSceneObjectEnd() { return objects.end(); }
	ConstObjectIterator getSceneObjectBegin() const { return objects.begin(); }
	ConstObjectIterator getSceneObjectEnd() const { return objects.end(); }
	ObjectList::size_type getSceneObjectCount() { return objects.size(); }
	void buildVisibleSceneObjectList(VisualObjectList& visObjs, RigidBodyObjectList& rbObjs);
	size_t getLastVisibleObjectCount() const { return curVisObjs.size(); }
	Renderer* getRenderer() { return renderer; }
	void setRenderer(Renderer* r) { renderer = r; }
	void updateVisibility();
	void update(uint64_t timePassed);
	void present();
	void setFrustumCullingEnabled(bool fc) { fcEnabled = fc; }
	bool isFrustumCullingEnabled() const { return fcEnabled; }
	btDiscreteDynamicsWorld* getPhysicsWorld() { return physicsWorld; }
	void setPhysicsWorld(btDiscreteDynamicsWorld* world) { physicsWorld = world; }
	void addStreamingViewpoint(StreamingViewpoint* vp);
	StreamingManager* getStreamingManager() { return streamer; }

	void setFreezeVisibility(bool fv) { freezeVisibility = fv; }
	bool isVisibilityFrozen() const { return freezeVisibility; }

	virtual void streamed(SceneObject* obj, uint32_t inBuckets, uint32_t outBuckets);

private:
	StreamingManager* streamer;
	ObjectList objects;
	ObjectList animObjs;
	LightSourceList lightSources;
	Renderer* renderer;
	RenderingEntityGenerator* reGenerator;
	int nextStaticObjID;
	bool pvsEnabled;
	bool fcEnabled;
	btDiscreteDynamicsWorld* physicsWorld;
	StreamingViewpointList svList;

	VisualObjectList curVisObjs;
	RigidBodyObjectList curRbObjs;

	bool freezeVisibility;

	TestShaderPlugin* testPlugin;
	ShaderPluginRegistry testReg;
	UniformBuffer testBuf;

	GLint timeVal;
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

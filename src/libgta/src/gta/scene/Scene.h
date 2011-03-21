/*
 * Scene.h
 *
 *  Created on: 07.03.2011
 *      Author: alemariusnexus
 */

#ifndef SCENE_H_
#define SCENE_H_

#include "DefaultSceneObject.h"
#include "../ShaderProgram.h"
#include "visibility/PVSData.h"
#include <vector>

using std::vector;


class Scene {
public:
	typedef vector<DefaultSceneObject*> ObjectList;
	typedef ObjectList::iterator ObjectIterator;
	typedef ObjectList::const_iterator ConstObjectIterator;

public:
	Scene();
	void addSceneObject(DefaultSceneObject* obj);
	ObjectIterator getSceneObjectBegin() { return objects.begin(); }
	ObjectIterator getSceneObjectEnd() { return objects.end(); }
	ConstObjectIterator getSceneObjectBegin() const { return objects.begin(); }
	ConstObjectIterator getSceneObjectEnd() const { return objects.end(); }
	size_type getSceneObjectCount() { return objects.size(); }
	void buildVisibleSceneObjectList(ObjectList& list);
	DefaultSceneObject* getObjectByID(int id) { return objects[id]; }

private:
	ObjectList objects;
	PVSData pvs;
	bool pvsValid;
};

#endif /* SCENE_H_ */

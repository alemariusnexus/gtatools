/*
 * Scene.h
 *
 *  Created on: 07.03.2011
 *      Author: alemariusnexus
 */

#ifndef SCENE_H_
#define SCENE_H_

#include "DefaultSceneObject.h"
#include <vector>

using std::vector;


class Scene {
public:
	typedef vector<DefaultSceneObject*> ObjectList;
	typedef ObjectList::iterator ObjectIterator;

public:
	void addSceneObject(DefaultSceneObject* obj);
	ObjectIterator getSceneObjectBegin() { return objects.begin(); }
	ObjectIterator getSceneObjectEnd() { return objects.end(); }
	void buildVisibleSceneObjectList(ObjectList& list);

private:
	ObjectList objects;
};

#endif /* SCENE_H_ */

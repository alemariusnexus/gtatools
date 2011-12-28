/*
 * SceneObjectFileGroup.cpp
 *
 *  Created on: 22.10.2011
 *      Author: alemariusnexus
 */

#include "SceneObjectFileGroup.h"
#include "StaticSceneObject.h"
#include "SceneObjectDefinitionInfo.h"
#include <utility>

using std::pair;



SceneObjectFileGroup::SceneObjectFileGroup(const CString& relPath)
		: relPath(relPath)
{
}


void SceneObjectFileGroup::addSceneObject(StaticSceneObject* obj)
{
	SceneObjectDefinitionInfo* info = obj->getDefinitionInfo();
	objs.insert(pair<uint32_t, StaticSceneObject*>(info->getID(), obj));
}


StaticSceneObject* SceneObjectFileGroup::getObject(uint32_t id)
{
	ObjectIterator it = objs.find(id);

	if (it == objs.end()) {
		return NULL;
	}

	return it->second;
}

/*
 * SceneObjectDefinitionDatabase.cpp
 *
 *  Created on: 22.10.2011
 *      Author: alemariusnexus
 */

#include "SceneObjectDefinitionDatabase.h"
#include <utility>

using std::pair;




void SceneObjectDefinitionDatabase::addFileGroup(SceneObjectFileGroup* group)
{
	fileGroups.insert(pair<CString, SceneObjectFileGroup*>(group->getRelativePath(), group));
}


SceneObjectFileGroup* SceneObjectDefinitionDatabase::getFileGroup(const CString& relPath)
{
	FileGroupMap::iterator it = fileGroups.find(relPath);

	if (it == fileGroups.end()) {
		return NULL;
	}

	return it->second;
}


void SceneObjectDefinitionDatabase::addGroupDependency(SceneObjectGroupDependency* dep)
{
	groupDeps.insert(pair<CString, SceneObjectGroupDependency*>(dep->getRelativePath(), dep));
}


SceneObjectGroupDependency* SceneObjectDefinitionDatabase::getGroupDependency(const CString& relPath)
{
	GroupDepIterator it = groupDeps.find(relPath);

	if (it == groupDeps.end())
		return NULL;

	return it->second;
}

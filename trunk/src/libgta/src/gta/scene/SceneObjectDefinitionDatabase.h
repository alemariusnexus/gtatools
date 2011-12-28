/*
 * SceneObjectDefinitionDatabase.h
 *
 *  Created on: 22.10.2011
 *      Author: alemariusnexus
 */

#ifndef SCENEOBJECTDEFINITIONDATABASE_H_
#define SCENEOBJECTDEFINITIONDATABASE_H_

#include <gta/config.h>
#include "SceneObjectFileGroup.h"
#include "SceneObjectGroupDependency.h"
#include <gtaformats/util/StringComparator.h>
#include <gtaformats/util/CString.h>
#include <map>
#include <utility>

using std::map;
using std::distance;



class SceneObjectDefinitionDatabase {
public:
	typedef map<CString, SceneObjectFileGroup*> FileGroupMap;
	typedef FileGroupMap::iterator FileGroupIterator;

	typedef map<CString, SceneObjectGroupDependency*> GroupDepMap;
	typedef GroupDepMap::iterator GroupDepIterator;
	typedef GroupDepMap::const_iterator ConstGroupDepIterator;

public:
	void addFileGroup(SceneObjectFileGroup* group);
	SceneObjectFileGroup* getFileGroup(const CString& relPath);
	size_t getFileGroupCount() const { return fileGroups.size(); }
	FileGroupIterator getFileGroupBegin() { return fileGroups.begin(); }
	FileGroupIterator getFileGroupEnd() { return fileGroups.end(); }
	void addGroupDependency(SceneObjectGroupDependency* dep);
	size_t getGroupDependencyCount() const { return groupDeps.size(); }
	GroupDepIterator getGroupDependencyBegin() { return groupDeps.begin(); }
	GroupDepIterator getGroupDependencyEnd() { return groupDeps.end(); }
	size_t indexOf(SceneObjectGroupDependency* dep) const
			{ ConstGroupDepIterator it = groupDeps.find(dep->getRelativePath()); return distance(it, groupDeps.begin()); }
	SceneObjectGroupDependency* getGroupDependency(const CString& relPath);

private:
	FileGroupMap fileGroups;
	GroupDepMap groupDeps;
};

#endif /* SCENEOBJECTDEFINITIONDATABASE_H_ */

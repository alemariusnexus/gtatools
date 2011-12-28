/*
 * SceneObjectFileGroup.h
 *
 *  Created on: 22.10.2011
 *      Author: alemariusnexus
 */

#ifndef SCENEOBJECTFILEGROUP_H_
#define SCENEOBJECTFILEGROUP_H_

#include <gta/config.h>
#include "StaticSceneObject.h"
#include "SceneObjectGroupDependency.h"
#include <map>
#include <vector>

using std::map;
using std::vector;


class StaticSceneObject;



class SceneObjectFileGroup {
public:
	typedef map<uint32_t, StaticSceneObject*> ObjectMap;
	typedef ObjectMap::iterator ObjectIterator;
	typedef ObjectMap::const_iterator ConstObjectIterator;

	typedef vector<SceneObjectGroupDependency*> DepList;
	typedef DepList::iterator DepIterator;
	typedef DepList::const_iterator ConstDepIterator;

public:
	SceneObjectFileGroup(const CString& relPath);
	CString getRelativePath() const { return relPath; }
	void addSceneObject(StaticSceneObject* obj);
	StaticSceneObject* getObject(uint32_t id);
	void setModifyTime(uint64_t mtime) { this->mtime = mtime; }
	uint64_t getModifyTime() const { return mtime; }
	ObjectIterator getObjectBegin() { return objs.begin(); }
	ObjectIterator getObjectEnd() { return objs.end(); }
	ConstObjectIterator getObjectBegin() const { return objs.begin(); }
	ConstObjectIterator getObjectEnd() const { return objs.end(); }
	size_t getObjectCount() const { return objs.size(); }
	void addDependency(SceneObjectGroupDependency* dep) { deps.push_back(dep); }
	size_t getDependencyCount() const { return deps.size(); }
	DepIterator getDependencyBegin() { return deps.begin(); }
	DepIterator getDependencyEnd() { return deps.end(); }
	ConstDepIterator getDependencyBegin() const { return deps.begin(); }
	ConstDepIterator getDependencyEnd() const { return deps.end(); }

private:
	CString relPath;
	ObjectMap objs;
	uint64_t mtime;
	DepList deps;
};

#endif /* SCENEOBJECTFILEGROUP_H_ */

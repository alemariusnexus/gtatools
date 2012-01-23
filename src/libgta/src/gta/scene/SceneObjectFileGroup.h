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
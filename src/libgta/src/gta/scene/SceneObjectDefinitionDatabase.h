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

#ifndef SCENEOBJECTDEFINITIONDATABASE_H_
#define SCENEOBJECTDEFINITIONDATABASE_H_

#include <gta/config.h>
#include "SceneObjectFileGroup.h"
#include "SceneObjectGroupDependency.h"
#include <nxcommon/StringComparator.h>
#include <nxcommon/CString.h>
#include <map>
#include <utility>

using std::map;
using std::distance;



class SceneObjectDefinitionDatabase {
public:
	typedef map<CString, SceneObjectGroupDependency*> GroupDepMap;
	typedef GroupDepMap::iterator GroupDepIterator;
	typedef GroupDepMap::const_iterator ConstGroupDepIterator;

public:
	void addGroupDependency(SceneObjectGroupDependency* dep);
	size_t getGroupDependencyCount() const { return groupDeps.size(); }
	GroupDepIterator getGroupDependencyBegin() { return groupDeps.begin(); }
	GroupDepIterator getGroupDependencyEnd() { return groupDeps.end(); }
	size_t indexOf(SceneObjectGroupDependency* dep) const
			{ ConstGroupDepIterator it = groupDeps.find(dep->getRelativePath()); return distance(it, groupDeps.begin()); }
	SceneObjectGroupDependency* getGroupDependency(const CString& relPath);

private:
	GroupDepMap groupDeps;
};

#endif /* SCENEOBJECTDEFINITIONDATABASE_H_ */

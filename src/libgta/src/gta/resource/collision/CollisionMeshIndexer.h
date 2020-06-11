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

#ifndef COLLISIONMESHINDEXER_H_
#define COLLISIONMESHINDEXER_H_

#include "../ResourceObserver.h"
#include <gta/config.h>
#include <nxcommon/strutil.h>
#include <nxcommon/CString.h>
#include <nxcommon/cxx11hash.h>

#include <unordered_map>
using std::unordered_map;



class CollisionMeshIndexer : public ResourceObserver {
public:
	struct CollisionMeshIndexEntry
	{
		~CollisionMeshIndexEntry() { delete file; }
		File* file;
		int index;
	};

private:
	typedef unordered_map<CString, CollisionMeshIndexEntry*, CXX11Hash<CString> > IndexMap;

public:
	virtual ~CollisionMeshIndexer();
	virtual void resourceAdded(const File& file);
	virtual void resourcesCleared();
	CollisionMeshIndexEntry* getCollisionMesh(CString name);
	CollisionMeshIndexEntry* operator[](CString name) { return getCollisionMesh(name); }

private:
	IndexMap index;
};

#endif /* COLLISIONMESHINDEXER_H_ */

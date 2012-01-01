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

#ifndef MESHCACHEENTRY_H_
#define MESHCACHEENTRY_H_

#include "../ResourceCache.h"
#include "MeshClump.h"


class MeshCacheEntry : public Engine::StringResourceCache::Entry {
public:
	MeshCacheEntry(MeshClump* mesh);
	virtual ~MeshCacheEntry();
	virtual cachesize_t getSize() const;
	MeshClump* getMesh() { return mesh; }
	MeshClump* operator*() { return mesh; }

private:
	MeshClump* mesh;
};

#endif /* MESHCACHEENTRY_H_ */

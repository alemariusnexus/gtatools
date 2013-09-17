/*
	Copyright 2010-2013 David "Alemarius Nexus" Lerch

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

#ifndef COLLISIONMESHCACHEENTRY_H_
#define COLLISIONMESHCACHEENTRY_H_

#include "../ResourceCache.h"
#include <gtaformats/util/strutil.h>
#include <btBulletDynamicsCommon.h>
#include "../smesh/ShadowMesh.h"
#include "../../Engine.h"


class CollisionMeshCacheEntry : public Engine::StringResourceCache::Entry {
public:
	CollisionMeshCacheEntry(btCollisionShape* shape, ShadowMesh* smesh, cachesize_t size)
			: shape(shape), size(size), smesh(smesh) {}
	virtual ~CollisionMeshCacheEntry() { delete shape; delete smesh; }
	virtual cachesize_t getSize() const { return size; }
	btCollisionShape* operator*() { return shape; }
	ShadowMesh* getShadowMesh() { return smesh; }

private:
	btCollisionShape* shape;
	cachesize_t size;
	ShadowMesh* smesh;
};

#endif /* COLLISIONMESHCACHEENTRY_H_ */

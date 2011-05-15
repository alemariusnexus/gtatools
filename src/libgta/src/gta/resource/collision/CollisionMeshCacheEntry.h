/*
 * CollisionMeshCacheEntry.h
 *
 *  Created on: 30.04.2011
 *      Author: alemariusnexus
 */

#ifndef COLLISIONMESHCACHEENTRY_H_
#define COLLISIONMESHCACHEENTRY_H_

#include "../CacheEntry.h"
#include <btBulletDynamicsCommon.h>


class CollisionMeshCacheEntry : public CacheEntry {
public:
	CollisionMeshCacheEntry(btCollisionShape* shape, cachesize_t size) : shape(shape), size(size) {}
	virtual ~CollisionMeshCacheEntry() { delete shape; }
	virtual cachesize_t getSize() const { return size; }
	btCollisionShape* operator*() { return shape; }

private:
	btCollisionShape* shape;
	cachesize_t size;
};

#endif /* COLLISIONMESHCACHEENTRY_H_ */

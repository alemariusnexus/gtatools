/*
 * CollisionMeshCacheLoader.h
 *
 *  Created on: 30.04.2011
 *      Author: alemariusnexus
 */

#ifndef COLLISIONMESHCACHELOADER_H_
#define COLLISIONMESHCACHELOADER_H_

#include "../CacheEntryLoader.h"
#include "CollisionMeshIndexer.h"



class CollisionMeshCacheLoader : public CacheEntryLoader {
public:
	CollisionMeshCacheLoader(CollisionMeshIndexer* indexer);
	virtual CacheEntry* load(hash_t name);

private:
	CollisionMeshIndexer* indexer;
};

#endif /* COLLISIONMESHCACHELOADER_H_ */

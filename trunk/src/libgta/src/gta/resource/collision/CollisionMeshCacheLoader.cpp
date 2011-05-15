/*
 * CollisionMeshCacheLoader.cpp
 *
 *  Created on: 30.04.2011
 *      Author: alemariusnexus
 */

#include "CollisionMeshCacheLoader.h"
#include "../../COLBulletConverter.h"
#include "CollisionMeshCacheEntry.h"




CollisionMeshCacheLoader::CollisionMeshCacheLoader(CollisionMeshIndexer* indexer)
		: indexer(indexer)
{
}


CacheEntry* CollisionMeshCacheLoader::load(hash_t name)
{
	CollisionMeshIndexer::CollisionMeshIndexEntry* entry = indexer->getCollisionMesh(name);

	if (!entry) {
		return NULL;
	}

	COLLoader col;
	istream* stream = entry->file->openInputStream(istream::binary);
	col.skip(stream, entry->index);
	COLModel* model = col.loadModel(stream);
	COLBulletConverter conv;
	btCollisionShape* shape = conv.convert(*model);
	delete model;
	delete stream;

	CollisionMeshCacheEntry* cacheEntry = new CollisionMeshCacheEntry(shape, 0);
	return cacheEntry;
}

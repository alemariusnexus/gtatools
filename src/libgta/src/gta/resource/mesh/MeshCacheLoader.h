/*
 * MeshCacheLoader.h
 *
 *  Created on: 06.03.2011
 *      Author: alemariusnexus
 */

#ifndef MESHCACHELOADER_H_
#define MESHCACHELOADER_H_

#include "../CacheEntryLoader.h"
#include "MeshIndexer.h"


class MeshCacheLoader : public CacheEntryLoader {
public:
	MeshCacheLoader(MeshIndexer* indexer) : indexer(indexer) {}
	virtual CacheEntry* load(hash_t key);

private:
	MeshIndexer* indexer;
};

#endif /* MESHCACHELOADER_H_ */

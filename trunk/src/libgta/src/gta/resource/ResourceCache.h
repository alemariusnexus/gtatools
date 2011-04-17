/*
 * ResourceCache.h
 *
 *  Created on: 06.03.2011
 *      Author: alemariusnexus
 */

#ifndef RESOURCECACHE_H_
#define RESOURCECACHE_H_

#include "../Engine.h"
#include "CacheEntry.h"
#include <gtaformats/util/Cache.h>
#include "CacheEntryLoader.h"
#include <map>




class ResourceCache {
private:
	typedef Cache<hash_t, CacheEntry> EntryCache;

public:
	ResourceCache(CacheEntryLoader* loader, cachesize_t capacity);
	~ResourceCache();
	CacheEntry* getEntry(hash_t key, bool lock = false);
	CacheEntry* getEntryIfCached(hash_t key, bool lock = false);
	bool uncacheEntry(hash_t key);
	CacheEntry* unlockEntry(hash_t key);
	cachesize_t getCapacity() const { return cache.getCapacity(); }
	cachesize_t getOccupiedSize() const { return cache.getOccupiedSize(); }
	void resize(cachesize_t capacity) { cache.resize(capacity); }
	bool isOverfilled() const { return getOccupiedSize() > getCapacity(); }
	bool clear();
	CacheEntryLoader* getEntryLoader() { return loader; }
#ifndef NDEBUG
	uint64_t getHitCount() const { return numHits; }
	uint64_t getMissCount() const { return numMisses; }
	void resetStatistics() { numHits = 0; numMisses = 0; }
	bool isStatisticsAvailable() const { return true; }
	float getEfficiency() const { return (float) numHits / (float) (numHits+numMisses); }
#else
	uint64_t getHitCount() const { return 0; }
	uint64_t getMissCount() const { return 0; }
	void resetStatistics() {}
	bool isStatisticsAvailable() const { return false; }
	float getEfficiency() const { return 0.0f; }
#endif

private:
	CacheEntry* doCache(hash_t key, bool lock = false);

private:
	EntryCache cache;
	CacheEntryLoader* loader;

#ifndef NDEBUG
	uint64_t numHits;
	uint64_t numMisses;
#endif
};

#endif /* RESOURCECACHE_H_ */

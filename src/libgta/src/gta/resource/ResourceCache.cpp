/*
 * ResourceCache.cpp
 *
 *  Created on: 06.03.2011
 *      Author: alemariusnexus
 */

#include "ResourceCache.h"
#include "CacheEntry.h"
#include <gtaformats/util/InvalidStateException.h>
#include <cstdio>
#include <gtaformats/util/util.h>




ResourceCache::ResourceCache(CacheEntryLoader* loader, cachesize_t capacity)
		: cache(EntryCache(capacity)), loader(loader)
#ifndef NDEBUG
		  , numHits(0), numMisses(0)
#endif
{
}


ResourceCache::~ResourceCache()
{
	if (!clear()) {
		throw InvalidStateException("At least one cache entry is still locked during the destruction of "
				"this ResourceCache!", __FILE__, __LINE__);
	}

	delete loader;
}


CacheEntry* ResourceCache::doCache(hash_t key, bool lock)
{
#ifndef NDEBUG
	numMisses++;
#endif
	CacheEntry* entry = loader->load(key);

	if (!cache.insert(key, entry, entry->getSize(), lock)) {
		return NULL;
	}

	return entry;
}


CacheEntry* ResourceCache::getEntryIfCached(hash_t key, bool lock)
{
	CacheEntry* entry = cache.lock(key, lock);
	return entry;
}


CacheEntry* ResourceCache::getEntry(hash_t key, bool lock)
{
	CacheEntry* entry = cache.lock(key, lock);

	if (!entry) {
		CacheEntry* entry = doCache(key, lock);
		return entry;
	}

#ifndef NDEBUG
	numHits++;
#endif

	return entry;
}


bool ResourceCache::uncacheEntry(hash_t key)
{
	return cache.remove(key);
}


CacheEntry* ResourceCache::unlockEntry(hash_t key)
{
	return cache.unlock(key);
}


bool ResourceCache::clear()
{
	return cache.clear();
}

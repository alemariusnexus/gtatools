/*
	Copyright 2010-2011 David "Alemarius Nexus" Lerch

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

#include "ResourceCache.h"
#include "CacheEntry.h"
#include <gtaformats/util/InvalidStateException.h>
#include <cstdio>
#include <gtaformats/util/util.h>
#include <algorithm>

using std::find;




ResourceCache::ResourceCache(CacheEntryLoader* loader, cachesize_t capacity)
		: cache(EntryCache(capacity)), loader(loader)
#ifndef NDEBUG
		  , numHits(0), numMisses(0), sizeUsed(0)
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

	if (!entry  ||  !cache.insert(key, entry, entry->getSize(), lock)) {
		return NULL;
	}

#ifndef NDEBUG
	entryAccessed(entry);
#endif

	return entry;
}


CacheEntry* ResourceCache::getEntryIfCached(hash_t key, bool lock)
{
	CacheEntry* entry = cache.lock(key, lock);
#ifndef NDEBUG
	entryAccessed(entry);
#endif
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
	entryAccessed(entry);
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


#ifndef NDEBUG
void ResourceCache::resetStatistics()
{
	numHits = 0;
	numMisses = 0;
	sizeUsed = 0;
	entriesUsed.clear();
}
#endif


#ifndef NDEBUG
void ResourceCache::entryAccessed(CacheEntry* entry)
{
	vector<CacheEntry*>::iterator it = find(entriesUsed.begin(), entriesUsed.end(), entry);

	if (it == entriesUsed.end()) {
		sizeUsed += entry->getSize();
		entriesUsed.push_back(entry);
	}
}
#endif

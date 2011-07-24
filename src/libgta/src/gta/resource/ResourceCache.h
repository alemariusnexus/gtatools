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

#ifndef RESOURCECACHE_H_
#define RESOURCECACHE_H_

#include "../Engine.h"
#include "CacheEntry.h"
#include <gtaformats/util/Cache.h>
#include "CacheEntryLoader.h"
#include <map>
#include <cstdio>

using std::map;

class SharedCachePointer;
class CachePointer;



class ResourceCache {
private:
	typedef Cache<hash_t, CacheEntry> EntryCache;
	typedef map<hash_t, SharedCachePointer*> SharedPtrMap;

public:
	ResourceCache(CacheEntryLoader* loader, cachesize_t capacity);
	~ResourceCache();
	CachePointer getEntryPointer(hash_t key);
	cachesize_t getCapacity() const { return cache.getCapacity(); }
	cachesize_t getOccupiedSize() const { return cache.getOccupiedSize(); }
	void resize(cachesize_t capacity) { cache.resize(capacity); }
	bool isOverfilled() const { return getOccupiedSize() > getCapacity(); }
	bool clear();
	CacheEntryLoader* getEntryLoader() { return loader; }

private:
	CacheEntry* doCache(hash_t key, bool lock = false);
	SharedCachePointer* getSharedPointer(hash_t key);
	void removeSharedPointer(hash_t key);
	CacheEntry* getEntry(hash_t key, bool lock = false);
	CacheEntry* getEntryIfCached(hash_t key, bool lock = false);
	bool uncacheEntry(hash_t key);
	CacheEntry* unlockEntry(hash_t key);

private:
	EntryCache cache;
	CacheEntryLoader* loader;
	SharedPtrMap sharedPtrs;

private:
	friend class SharedCachePointer;
};

#endif /* RESOURCECACHE_H_ */

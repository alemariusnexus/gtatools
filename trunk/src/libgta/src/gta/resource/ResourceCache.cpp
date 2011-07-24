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
#include "SharedCachePointer.h"
#include "CachePointer.h"
#include <gtaformats/util/InvalidStateException.h>
#include <cstdio>
#include <gtaformats/util/util.h>
#include <algorithm>

using std::find;




ResourceCache::ResourceCache(CacheEntryLoader* loader, cachesize_t capacity)
		: cache(EntryCache(capacity)), loader(loader)
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


CachePointer ResourceCache::getEntryPointer(hash_t key)
{
	return CachePointer(getSharedPointer(key));
}


SharedCachePointer* ResourceCache::getSharedPointer(hash_t key)
{
	pair<SharedPtrMap::iterator, bool> res = sharedPtrs.insert(pair<hash_t, SharedCachePointer*>(key, NULL));

	if (res.second) {
		res.first->second = new SharedCachePointer(this, key);
	}

	return res.first->second;
}


void ResourceCache::removeSharedPointer(hash_t key)
{
	sharedPtrs.erase(key);
}


CacheEntry* ResourceCache::doCache(hash_t key, bool lock)
{
	CacheEntry* entry = loader->load(key);

	if (!entry  ||  !cache.insert(key, entry, entry->getSize(), lock)) {
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

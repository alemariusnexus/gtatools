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
#include <gtaformats/util/Cache.h>
//#include "EntryLoader.h"
//#include "Pointer.h"
#include <map>
#include <cstdio>
#include <algorithm>
#include <gtaformats/util/InvalidStateException.h>
#include <gtaformats/util/util.h>

using std::find;
using std::map;



//template<class K, class Compare, class MapHash, class KeyEqual> class SharedPointer;


typedef unsigned int cachesize_t;



template<class K, class Compare = less<K>, class MapHash = CXX0XHash<K>, class KeyEqual = equal_to<K> >
class ResourceCache {
private:
	class SharedPointer;

public:
	class Entry {
	public:
		Entry() : sharedPtr(NULL) {}
		virtual ~Entry();
		virtual cachesize_t getSize() const = 0;

	private:
		void registerPointer(SharedPointer* pointer) { sharedPtr = pointer; }
		void unregisterPointer() { sharedPtr = NULL; }

	private:
		SharedPointer* sharedPtr;

		friend class ResourceCache;
		friend class SharedPointer;
	};

	class Pointer {
	public:
		Pointer() {}
		Pointer(const Pointer& other) : sharedPtr(other.sharedPtr) {}
		bool isValid() const { return sharedPtr; }
		Entry* getEntry() { return sharedPtr->getEntry(); }
		void release() { return sharedPtr->release(); }

	private:
		Pointer(SharedPointer* sharedPtr) : sharedPtr(sharedPtr) {}

	private:
		shared_ptr<SharedPointer> sharedPtr;

	private:
		friend class ResourceCache<K, Compare, MapHash, KeyEqual>;
	};

	class EntryLoader {
	public:
		virtual Entry* load(K key) = 0;
	};

private:
	class SharedPointer {
	public:
		~SharedPointer();
		Entry* getEntry();
		void release();

	private:
		SharedPointer(ResourceCache<K, Compare, MapHash, KeyEqual>* cache, K key)
				: cache(cache), key(key), entry(NULL) {}
		void entryDeleted();

	private:
		ResourceCache<K, Compare, MapHash, KeyEqual>* cache;
		K key;
		Entry* entry;

	private:
		friend class ResourceCache<K, Compare, MapHash, KeyEqual>;
		friend class Entry;
	};

private:
	typedef Cache<K, Entry, Compare, MapHash, KeyEqual> EntryCache;
	typedef map<K, SharedPointer*, Compare> SharedPtrMap;

public:
	ResourceCache(EntryLoader* loader, cachesize_t capacity);
	~ResourceCache();
	Pointer getEntryPointer(K key);
	cachesize_t getCapacity() const { return cache.getCapacity(); }
	cachesize_t getOccupiedSize() const { return cache.getOccupiedSize(); }
	void resize(cachesize_t capacity) { cache.resize(capacity); }
	bool isOverfilled() const { return getOccupiedSize() > getCapacity(); }
	bool clear();
	EntryLoader* getEntryLoader() { return loader; }

private:
	Entry* doCache(K key, bool lock = false);
	SharedPointer* getSharedPointer(K key);
	void removeSharedPointer(K key);
	Entry* getEntry(K key, bool lock = false);
	Entry* getEntryIfCached(K key, bool lock = false);
	bool uncacheEntry(K key);
	Entry* unlockEntry(K key);

private:
	EntryCache cache;
	EntryLoader* loader;
	SharedPtrMap sharedPtrs;

private:
	friend class SharedPointer;
};









template <class K, class Compare, class MapHash, class KeyEqual>
ResourceCache<K, Compare, MapHash, KeyEqual>::ResourceCache(EntryLoader* loader, cachesize_t capacity)
		: cache(EntryCache(capacity)), loader(loader)
{
}


template <class K, class Compare, class MapHash, class KeyEqual>
ResourceCache<K, Compare, MapHash, KeyEqual>::~ResourceCache()
{
	if (!clear()) {
		throw InvalidStateException("At least one cache entry is still locked during the destruction of "
				"this ResourceCache!", __FILE__, __LINE__);
	}

	delete loader;
}


template <class K, class Compare, class MapHash, class KeyEqual>
typename ResourceCache<K, Compare, MapHash, KeyEqual>::Pointer ResourceCache<K, Compare, MapHash, KeyEqual>::getEntryPointer(K key)
{
	return Pointer(getSharedPointer(key));
}


template <class K, class Compare, class MapHash, class KeyEqual>
typename ResourceCache<K, Compare, MapHash, KeyEqual>::SharedPointer* ResourceCache<K, Compare, MapHash, KeyEqual>::getSharedPointer(K key)
{
	pair<typename SharedPtrMap::iterator, bool> res
			= sharedPtrs.insert(pair<K, typename ResourceCache<K, Compare, MapHash, KeyEqual>::SharedPointer* >(key, NULL));

	if (res.second) {
		res.first->second = new SharedPointer(this, key);
	}

	return res.first->second;
}


template <class K, class Compare, class MapHash, class KeyEqual>
void ResourceCache<K, Compare, MapHash, KeyEqual>::removeSharedPointer(K key)
{
	sharedPtrs.erase(key);
}


template <class K, class Compare, class MapHash, class KeyEqual>
typename ResourceCache<K, Compare, MapHash, KeyEqual>::Entry* ResourceCache<K, Compare, MapHash, KeyEqual>::doCache(K key, bool lock)
{
	Entry* entry = loader->load(key);

	if (!entry  ||  !cache.insert(key, entry, entry->getSize(), lock)) {
		return NULL;
	}

	return entry;
}


template <class K, class Compare, class MapHash, class KeyEqual>
typename ResourceCache<K, Compare, MapHash, KeyEqual>::Entry* ResourceCache<K, Compare, MapHash, KeyEqual>::getEntryIfCached(K key, bool lock)
{
	Entry* entry = cache.lock(key, lock);
	return entry;
}


template <class K, class Compare, class MapHash, class KeyEqual>
typename ResourceCache<K, Compare, MapHash, KeyEqual>::Entry* ResourceCache<K, Compare, MapHash, KeyEqual>::getEntry(K key, bool lock)
{
	Entry* entry = cache.lock(key, lock);

	if (!entry) {
		Entry* entry = doCache(key, lock);
		return entry;
	}

	return entry;
}


template <class K, class Compare, class MapHash, class KeyEqual>
bool ResourceCache<K, Compare, MapHash, KeyEqual>::uncacheEntry(K key)
{
	return cache.remove(key);
}


template <class K, class Compare, class MapHash, class KeyEqual>
typename ResourceCache<K, Compare, MapHash, KeyEqual>::Entry* ResourceCache<K, Compare, MapHash, KeyEqual>::unlockEntry(K key)
{
	return cache.unlock(key);
}


template <class K, class Compare, class MapHash, class KeyEqual>
bool ResourceCache<K, Compare, MapHash, KeyEqual>::clear()
{
	return cache.clear();
}





template<class K, class Compare, class MapHash, class KeyEqual>
ResourceCache<K, Compare, MapHash, KeyEqual>::SharedPointer::~SharedPointer()
{
	cache->removeSharedPointer(key);

	if (entry)
		entry->unregisterPointer();
}


template<class K, class Compare, class MapHash, class KeyEqual>
typename ResourceCache<K, Compare, MapHash, KeyEqual>::Entry* ResourceCache<K, Compare, MapHash, KeyEqual>::SharedPointer::getEntry()
{
	if (entry)
		return entry;

	entry = cache->getEntry(key);

	if (entry) {
		entry->registerPointer(this);
	}

	return entry;
}


template<class K, class Compare, class MapHash, class KeyEqual>
void ResourceCache<K, Compare, MapHash, KeyEqual>::SharedPointer::release()
{

}


template<class K, class Compare, class MapHash, class KeyEqual>
void ResourceCache<K, Compare, MapHash, KeyEqual>::SharedPointer::entryDeleted()
{
	entry = NULL;
}


template<class K, class Compare, class MapHash, class KeyEqual>
ResourceCache<K, Compare, MapHash, KeyEqual>::Entry::~Entry()
{
	if (sharedPtr)
		sharedPtr->entryDeleted();
}

#endif /* RESOURCECACHE_H_ */

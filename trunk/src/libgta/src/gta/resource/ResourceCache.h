/*
	Copyright 2010-2013 David "Alemarius Nexus" Lerch

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

#include <gtaformats/util/Cache.h>
//#include "EntryLoader.h"
//#include "Pointer.h"
#include <map>
#include <cstdio>
#include <algorithm>
#include <gtaformats/util/InvalidStateException.h>
#include <gtaformats/util/util.h>
#include <boost/shared_ptr.hpp>

using std::find;
using std::map;
using boost::shared_ptr;



//template<class K, class Compare, class MapHash, class KeyEqual> class SharedPointer;


typedef unsigned int cachesize_t;



/**	\brief A resource cache that automatically loads resources that are not yet cached.
 *
 * 	This class is built upon the basic libgtaformats Cache class. Internally, it acts much in the same way,
 * 	but it's external interface is different. See documentation of the Cache class for information on how
 * 	this cache works in general.
 *
 *	What makes this class different from the basic Cache class is that it is based on entry pointers, and an
 *	entry can not be directly retrieved from or inserted into the cache. Instead, each ResourceCache has an
 *	associated EntryLoader which, given a key, tries to load the value for an entry from an external source.
 *	If the loader found such a value, a cache entry is created for it.
 *
 *	Accessing entries of the cache is done exclusively through the use of the Pointer class. At any time, a
 *	pointer to a cache entry can be obtained from the cache by giving in a key. The pointer may even be
 *	retrieved if the entry does not yet exist in the cache. A pointer once obtained from the cache stays
 *	valid throughout the lifetime of the ResourceCache, no matter how often the underlying entry is removed
 *	by the cache. Obtaining an entry pointer is different from accessing the real entry. To get the actual
 *	value of a cache entry, you have to <i>dereference</i> the entry pointer. When dereferenced, the entry
 *	pointer searches for the entry inside the cache, and if it is not found, it asks the EntryLoader to load
 *	it into the cache.
 *	Using this method, you can access entries without having to think about whether they are currently cached
 *	or not.
 *
 *	The Pointer class is optimized in that it tries its best to make as few map lookups as possible. For
 *	example, if the entry it points to is currently cached, it keeps a direct pointer to its value, so that
 *	no cache map lookup is needed when the entry pointer is dereferenced. In this case, getting a value from
 *	the cache is nearly a no-cost operation.
 *	This holds true even for two different entry pointers referring to the same entry. Because internally
 *	they use a SharedPointer object, if the entry is loaded into the cache through one pointer, it is
 *	instantly available to the other pointer without doing a cache map lookup.
 *
 *	This system effectively gives full responsibility for inserting and removing cache entries to this class.
 *	This means that entry values may never be deleted from anywhere outside this class and that memory
 *	management is completely handeled by this class, so you don't have to think about deleting your data at
 *	any time.
 *
 *	However, this also means that cache entries may become invalid whenever any method of the cache class or
 *	one of its associated classes (mainly the entry pointer class) is called, even when that method seems to
 *	be read-only (because with the entry loading system descibed above, accessing an entry might be a write
 *	operation). An entry is therefore guaranteed to be valid only until another method of this cache class
 *	or one of its associated classes is called. To keep an entry valid for a longer period of time, you can
 *	lock it through one of its entry pointers. The entry is then locked in much the same way as with the base
 *	Cache class. A locked entry is only unlocked if <b>all</b> pointers referring to it are unlocked
 *	(released). This is done by keeping a lock count in the SharedPointer.
 */
template<class K, class Compare = less<K>, class MapHash = CXX0XHash<K>, class KeyEqual = equal_to<K> >
class ResourceCache {
private:
	class SharedPointer;

public:
	/**	\brief Base class for entries stored inside a ResourceCache.
	 *
	 * 	Typically, a ResourceCache stores subclasses of Entry which provide the actual entry data.
	 */
	class Entry {
	public:
		/**	\brief Creates a new Entry.
		 */
		Entry() : sharedPtr(NULL) {}

		/**	\brief Destroys the entry.
		 */
		virtual ~Entry();

		/**	\brief Returns the size of the entry inside the cache.
		 *
		 * 	@return The entry size.
		 */
		virtual cachesize_t getSize() const = 0;

	private:
		/**	\brief Register a SharedPointer for this entry.
		 *
		 *	This is needed to inform the SharedPointer when the entry is deleted.
		 *
		 *	@param pointer The SharedPointer.
		 */
		void registerPointer(SharedPointer* pointer) { sharedPtr = pointer; }

		/**	\brief Unregister the pointer for this entry.
		 *
		 * 	This is usually done when the SharedPointer is deleted.
		 */
		void unregisterPointer() { sharedPtr = NULL; }

	private:
		SharedPointer* sharedPtr;

		friend class ResourceCache;
		friend class SharedPointer;
	};


	/**	\brief A pointer to an entry inside a ResourceCache (entry pointer).
	 *
	 * 	A pointer to an entry can exist even if the entry is not yet cached. When a pointer to an entry which
	 * 	is not cached is dereferenced, it tries to load the entry into the cache through the associated
	 * 	EntryLoader.
	 */
	class Pointer {
	public:
		/**	\brief Constructs an invalid entry pointer
		 *
		 * 	This pointer can not be dereferenced and is thus basically useless (kind of like a NULL pointer).
		 */
		Pointer() : locked(false) {}

		/**	\brief Copy constructor.
		 *
		 * 	Note that if a locked entry pointer is copied, the resulting copy will <b>not</b> be automatically
		 * 	locked.
		 *
		 * 	Copying an entry pointer is a very cheap operation, so you can copy pointers around as you wish.
		 *
		 * 	@param other The pointer to copy.
		 */
		Pointer(const Pointer& other) : sharedPtr(other.sharedPtr), locked(false) {}

		/**	\brief Determines whether the pointer is valid.
		 *
		 *	An invalid pointer is one which does not point to any entry. Note that even pointers to entries
		 *	which do not exist are valid. The only way to retrieve an invalid pointer is to call the
		 *	Pointer() constructor.
		 *
		 *	@return true if the pointer is valid, false otherwise.
		 */
		bool isValid() const { return sharedPtr; }

		/**	\brief Dereferences this pointer to obtain the entry it points to.
		 *
		 *	If the entry is cached, it is returned from the cache. Otherwise, the EntryLoader is asked to
		 *	load and cache the entry.
		 *
		 *	If the entry is not locked, it is guaranteed to be valid only up to the next call to a method of
		 *	the ResourceCache object this pointer belongs to or one of it's associated objects.
		 *
		 *	Note that this method can <b>not</b> be used to unlock an entry. Unlocking is done by calling
		 *	release().
		 *
		 *	@param lock If true, the entry will be locked so that it won't be deleted until release() is
		 *		called. If false, the locked status is kept unchanged.
		 *	@return The entry, or NULL if it was neither cached nor could be loaded by the EntryLoader.
		 */
		Entry* getEntry(bool lock = false);

		/**	\brief Release (unlock) the entry pointed to by this pointer.
		 *
		 *	This does not directly unlock the entry, but decreased its lock count by one. Only when the
		 *	lock count reaches 0, the entry is actually unlocked.
		 */
		void release() { if (locked) { sharedPtr->release(); locked = false; } }

	private:
		/**	\brief Creates a pointer with the given SharedPointer.
		 *
		 * 	@param sharedPtr The SharedPointer.
		 */
		Pointer(SharedPointer* sharedPtr) : sharedPtr(sharedPtr), locked(false) {}

	private:
		shared_ptr<SharedPointer> sharedPtr;
		bool locked;

	private:
		friend class ResourceCache<K, Compare, MapHash, KeyEqual>;
	};


	/**	\brief Base class responsible for loading entries into the cache from an external source.
	 */
	class EntryLoader {
	public:
		/**	\brief Load the entry with the specified key from an external source.
		 *
		 * 	@param key The entry's key.
		 * 	@return The loaded entry, or NULL if it could not be found or loaded.
		 */
		virtual Entry* load(K key) = 0;
	};

private:
	/**	\brief Internal shared pointer to an entry.
	 *
	 * 	This class is used to keep the lock counter and a direct reference to the entry if it is cached.
	 */
	class SharedPointer {
	public:
		/**	\brief Destroys the shared pointer.
		 *
		 * 	A SharedPointer is only deleted when all Pointer objects referring to it have been deleted. When
		 * 	a SharedPointer is destroyed, it removes itself from the ResourceCache's shared pointer list.
		 */
		~SharedPointer();

		Entry* getEntry(bool lock = false);

		void release();

	private:
		SharedPointer(ResourceCache<K, Compare, MapHash, KeyEqual>* cache, K key)
				: cache(cache), key(key), entry(NULL), lockCount(0) {}

		/**	\brief Callback method which is called by Entry::~Entry().
		 *
		 * 	It is used to invalidate the entry reference when the entry is deleted.
		 */
		void entryDeleted();

	private:
		ResourceCache<K, Compare, MapHash, KeyEqual>* cache;
		K key;
		Entry* entry;
		unsigned int lockCount;

	private:
		friend class ResourceCache<K, Compare, MapHash, KeyEqual>;
		friend class Entry;
	};

private:
	typedef Cache<K, Entry, Compare, MapHash, KeyEqual> EntryCache;
	typedef map<K, SharedPointer*, Compare> SharedPtrMap;

public:
	/**	\brief Create a new ResourceCache.
	 *
	 * 	@param loader The entry loader. Ownership of it is taken and it is automatically deleted when the
	 * 		ResourceCache destructor is called.
	 * 	@param capacity The initial capacity.
	 */
	ResourceCache(EntryLoader* loader, cachesize_t capacity);

	/**	\brief Destroys this ResourceCache and all its entries.
	 *
	 *	This invalidates all pointers obtained from this cache and destroys all cached entries. Note that all
	 *	locked entries must be manually unlocked before calling this method. If the cache is destroyed with
	 *	entries still locked, an InvalidStateException is thrown.
	 *
	 *	@throws InvalidStateException
	 */
	~ResourceCache();

	/**	\brief Obtain a pointer to an entry.
	 *
	 * 	The entry does not have to be cached or even exist in order to receive a valid pointer. The returned
	 * 	pointer stays valid until the ResourceCache is destroyed.
	 *
	 * 	@param key The entry's key.
	 * 	@return A pointer by which the entry can be referenced.
	 */
	Pointer getEntryPointer(K key);

	/**	\brief Return the cache's capacity (maximum size).
	 *
	 *	@return The capacity.
	 */
	cachesize_t getCapacity() const { return cache.getCapacity(); }

	/**	\brief Return the currently occupied size.
	 *
	 * 	@return The occupied size.
	 */
	cachesize_t getOccupiedSize() const { return cache.getOccupiedSize(); }

	/**	\brief Set a new cache capacity.
	 *
	 * 	If the new capacity is lower than the old, entries might be removed by this method.
	 *
	 * 	@param capacity The new capacity.
	 */
	void resize(cachesize_t capacity) { cache.resize(capacity); }

	/**	\brief Determine whether the cache is overfilled.
	 *
	 * 	This can happen if all entries inside the cache are locked.
	 *
	 * 	@return true if occupied size > capacity.
	 */
	bool isOverfilled() const { return getOccupiedSize() > getCapacity(); }

	/**	\brief Attempt to remove all entries from the cache.
	 *
	 *	It is not guaranteed that the cache is really empty afterwards, because there might be locked entries
	 *	which are not removed by this method.
	 *
	 *	@return true When the cache was completely cleared (i.e. there were no locked entries), false
	 *		otherwise.
	 */
	bool clear();

	/**	\brief Return the entry loader.
	 *
	 * 	@return The entry loader.
	 */
	EntryLoader* getEntryLoader() { return loader; }

private:
	/**	\brief Ask the EntryLoader to load the entry with the specified key and store it inside the cache.
	 *
	 * 	This method does not check whether the entry is already cached, so do not call it when you aren't
	 * 	sure it isn't already cached.
	 *
	 * 	@param key The entry's key.
	 * 	@param lock The initial lock status.
	 * 	@return The loaded entry or NULL if it could not be loaded or does not fit into the cache and wasn't
	 * 		locked.
	 */
	Entry* doCache(K key, bool lock = false);

	/**	\brief Find the SharedPointer responsible for the entry with the specified key.
	 *
	 * 	If it does not yet exist, it is created by this method.
	 *
	 * 	@param key The entry's key.
	 * 	@return The SharedPointer.
	 */
	SharedPointer* getSharedPointer(K key);

	/**	\brief Remove the SharedPointer for the specified entry.
	 *
	 * 	This method is called by SharedPointer::~SharedPointer().
	 *
	 *	@param The entry's key.
	 * 	@see SharedPointer::~SharedPointer().
	 */
	void removeSharedPointer(K key);

	/**	\brief Lock or unlock a cache entry.
	 *
	 * 	Unlike the lock/release methods of Pointer and SharedPointer, this method ignores any lock counters
	 * 	and directly locks or unlocks the entries.
	 *
	 * 	@param key The entry's key.
	 * 	@param lock The lock state.
	 * 	@return A pointer to the locked or unlocked entry. If it was unlocked in an overfilled cache, it will
	 * 		be instantly removed by this method and thus NULL will be returned.
	 * 	@see getEntry(K key)
	 */
	Entry* lock(K key, bool lock = false);

	/**	\brief Returns a cache entry with the specified key.
	 *
	 * 	When the entry is not yet cached, doCache() is called to laod it into the cache.
	 *
	 * 	@param key The entry's key.
	 * 	@return The entry or NULL if it was neither cached nor could be loaded.
	 */
	Entry* getEntry(K key);

	/**	\brief Returns a cache entry with the specified key only if it is currently cached.
	 *
	 * 	Unlike getEntry(), it does not attempt to load the entry when it is not cached, but returns NULL in
	 * 	this case.
	 *
	 * 	@param key The entry's key.
	 * 	@return The entry or NULL if it was not cached.
	 */
	Entry* getEntryIfCached(K key);

	/**	\brief Explicitly removes an entry from the cache.
	 *
	 *	If the entry is locked, false is returned and the entry is left untouched.
	 *
	 *	@param key The entry's key.
	 *	@return true if the entry was removed, false otherwise.
	 */
	bool uncacheEntry(K key);

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
typename ResourceCache<K, Compare, MapHash, KeyEqual>::Entry* ResourceCache<K, Compare, MapHash, KeyEqual>::getEntryIfCached(K key)
{
	Entry* entry = cache.getEntry(key);
	return entry;
}


template <class K, class Compare, class MapHash, class KeyEqual>
typename ResourceCache<K, Compare, MapHash, KeyEqual>::Entry* ResourceCache<K, Compare, MapHash, KeyEqual>::getEntry(K key)
{
	Entry* entry = cache.item(key);

	if (!entry) {
		Entry* entry = doCache(key);
		return entry;
	}

	return entry;
}


template <class K, class Compare, class MapHash, class KeyEqual>
typename ResourceCache<K, Compare, MapHash, KeyEqual>::Entry* ResourceCache<K, Compare, MapHash, KeyEqual>::lock(K key, bool lock)
{
	Entry* entry = cache.lock(key, lock);

	if (!entry) {
		// This can mean two things: Either the entry was not found in the internal cache, or it was unlocked
		// successfully in an overfilled cache, which means it is instantly deleted by cache.lock().

		if (lock) {
			// We must cache the entry now

			Entry* entry = doCache(key, true);
			return entry;
		} else {
			// Unlocked is the default state of an entry, so we won't have to cache it right now.
		}
	}

	return entry;
}


template <class K, class Compare, class MapHash, class KeyEqual>
bool ResourceCache<K, Compare, MapHash, KeyEqual>::uncacheEntry(K key)
{
	return cache.remove(key);
}


template <class K, class Compare, class MapHash, class KeyEqual>
bool ResourceCache<K, Compare, MapHash, KeyEqual>::clear()
{
	return cache.clear();
}






template<class K, class Compare, class MapHash, class KeyEqual>
typename ResourceCache<K, Compare, MapHash, KeyEqual>::Entry* ResourceCache<K, Compare, MapHash, KeyEqual>::Pointer::getEntry(bool lock)
{
	if (lock) {
		if (!locked) {
			locked = true;
			return sharedPtr->getEntry(true);
		}
	}

	return sharedPtr->getEntry();
}





template<class K, class Compare, class MapHash, class KeyEqual>
ResourceCache<K, Compare, MapHash, KeyEqual>::SharedPointer::~SharedPointer()
{
	cache->removeSharedPointer(key);

	if (entry)
		entry->unregisterPointer();
}


template<class K, class Compare, class MapHash, class KeyEqual>
typename ResourceCache<K, Compare, MapHash, KeyEqual>::Entry* ResourceCache<K, Compare, MapHash, KeyEqual>::SharedPointer::getEntry(bool lock)
{
	if (lock) {
		lockCount++;

		if (lockCount == 1) {
			// Entry was unlocked before, so it must be really locked now.
			Entry* oldEntry = entry;
			entry = cache->lock(key, true);

			if (!oldEntry  &&  entry)
				entry->registerPointer(this);

			return entry;
		}
	}

	if (entry) {
		return entry;
	}

	entry = cache->getEntry(key);

	if (entry) {
		entry->registerPointer(this);
	}

	return entry;
}


template<class K, class Compare, class MapHash, class KeyEqual>
void ResourceCache<K, Compare, MapHash, KeyEqual>::SharedPointer::release()
{
	lockCount--;

	if (lockCount == 0) {
		// Entry was locked before, so it must be really unlocked now.
		Entry* oldEntry = entry;
		cache->lock(key, false);
	}
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

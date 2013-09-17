/*
	Copyright 2010-2013 David "Alemarius Nexus" Lerch

	This file is part of gtaformats.

	gtaformats is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	gtaformats is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with gtaformats.  If not, see <http://www.gnu.org/licenses/>.

	Additional permissions are granted, which are listed in the file
	GPLADDITIONS.
 */

#ifndef CACHE_H_
#define CACHE_H_

#include <gtaformats/config.h>
#include <gtaformats/util/cxx0xhash.h>
#include "InvalidStateException.h"
#include <utility>

#ifdef CXX0X_AVAILABLE
#include <unordered_map>

using std::unordered_map;
#else
#include <map>
using std::map;
#endif

using std::pair;
using std::less;
using std::equal_to;





/**	\brief A general-purpose cache class.
 *
 * 	This class represents a general-purpose LRU cache. Basically, it represents a map of keys to values, with
 * 	each value having a cache size. A cache has a maximum size which is called its <i>capacity</i>. Also, it
 * 	has an <i>occupied size</i>, which is the sum of the sizes of all currently cached entries. When
 * 	inserting an entry into a cache would cause its occupied size to be greater than its capacity, the entry
 * 	is said not to fit into the cache.
 * 	When an entry does not fit into the cache, it tries to make space for it by removing enough of the least
 * 	recently used entries, until the new entry does fit in.
 *
 * 	Ownership of every value inserted into the cache is taken by this class, and when the entry corresponding
 * 	to this value is removed from the cache, the value pointer is deleted, too. Because of this, values stored
 * 	in the cache might become invalid with each writing access to the cache. An entry inserted into or
 * 	obtained from the cache is guaranteed to be valid only until the next non-read-only method is called on
 * 	the cache.
 * 	To guarantee validity of an entry even when non-read-only methods are called, it is possible to
 * 	<i>lock</i> cache entries. If a cache entry is locked, it will not removed by the cache until it is
 * 	unlocked again. Therefore, locked entries are skipped in the LRU cleaning phase. Also, if all entries
 * 	in the cache are locked, and another new locked entry is added to the cache that does not fit into it, it
 * 	is possible for the occupied size to become greater than the capacity, because locked entries are never
 * 	removed from the cache. In such situations, the cache is said to be <i>overfilled</i>. If a locked entry
 * 	is unlocked in an overfilled cache, it becomes deleted instantly.
 */
template<class K, class V, class Compare = less<K>,
		class MapHash = CXX0XHash<K>,
		class KeyEqual = equal_to<K> >
class Cache {
public:
	typedef unsigned int cachesize_t;

private:
	struct Entry
	{
		Entry(V* vPtr, cachesize_t cost, bool locked) : vPtr(vPtr), cost(cost), prev(NULL), next(NULL),
				locked(locked) {}
		const K* kPtr;
		V* vPtr;
		cachesize_t cost;
		Entry* prev;
		Entry* next;
		bool locked;
	};

#ifdef CXX0X_AVAILABLE
	typedef unordered_map<K, Entry, MapHash, KeyEqual> EntryMap;
#else
	typedef map<K, Entry, Compare> EntryMap;
#endif

public:
	/**	\brief Creates a new, empty cache.
	 *
	 * 	@param capacity The capacity (maximum size) of the cache.
	 */
	Cache(cachesize_t capacity = 0) : first(NULL), last(NULL), capacity(capacity), occupied(0) {}

	/**	\brief Deletes the cache and all it's content.
	 *
	 * 	This removes and therefore deletes all cache entries. Note that before destroying a cache, all it's
	 * 	entries must be unlocked by the user. If this destructor is called with entries still locked, it will
	 * 	throw an InvalidStateException.
	 *
	 * 	@throws InvalidStateException if entries are still locked.
	 */
	~Cache() { if (!clear()) { throw InvalidStateException("Cache entries are still locked upon destruction!", __FILE__, __LINE__); } }

	/**	\brief Inserts a new entry into the cache.
	 *
	 * 	If the entry does not fit into the cache, entries are removed until it does. If even after freeing
	 * 	the cache the entry does not fit in (if it is bigger than the whole cache or if too many entries are
	 * 	locked), and it was not inserted as locked, it is not inserted but deleted instantly. However, if in
	 * 	this same scenario, the entry was inserted as locked, it will be forcefully inserted, causing the
	 * 	cache to be overfilled.
	 *
	 * 	Ownership of the value pointer is taken by the cache, meaning it can be removed any time a
	 * 	non-read-only method is called on the cache, unless the entry is stored in locked mode.
	 *
	 * 	@param key The entry's key.
	 * 	@param A pointer to the entry's value. Ownership of it is taken.
	 * 	@param size The size of the new entry.
	 * 	@param locked Whether the entry should be stored in locked state.
	 * 	@return true when the entry was successfully inserted, false otherwise.
	 */
	bool insert(const K& key, V* value, cachesize_t size, bool locked = false);

	/**	\brief Removes an entry from the cache.
	 *
	 *	This method does nothing if the entry is locked. To remove a locked entry, you have to manually
	 *	unlock it first.
	 *
	 *	@param key The entry's key.
	 *	@return true if the entry was removed successfully, false otherwise.
	 */
	bool remove(const K& key);

	/**	\brief Returns the value of a cache entry.
	 *
	 * 	This also updates the last-used status of the accessed entry, so that it is the most recently used
	 * 	one afterwards.
	 *
	 * 	@param key The entry's key.
	 * 	@return The entry's value if it was found, or NULL otherwise.
	 */
	V* item(const K& key) { return access(key); }

	/**	\brief Removes entries from the cache until it has the specified size.
	 *
	 *	Entries are removed by calling remove() for as many entries as needed, starting with the least
	 *	recently used one. Freeing the cache stops when either enough entries were removed, or when only
	 *	locked entries are left inside the cache.
	 *
	 *	Note that because of the latter case, this method does <b>not</b> guarantee that the occupied size is
	 *	lower or equal to the one requested afterwards.
	 *
	 *	@param size The new maximum occupied size of the cache.
	 *	@return true if enough space was freed, false otherwise.
	 */
	bool free(cachesize_t size);

	/**	\brief Returns the value of a cache entry.
	 *
	 * 	This is a synonym for item().
	 *
	 * 	@see item()
	 */
	V* operator[](const K& key) { return item(key); }

	/**	\brief Returns the current capacity of the cache.
	 *
	 * 	@return The capacity.
	 */
	cachesize_t getCapacity() const { return capacity; }

	/**	\brief Returns the currently occupied size of the cache.
	 *
	 * 	This is the sum of the sizes of all cache entries.
	 *
	 * 	@return The occupied size.
	 */
	cachesize_t getOccupiedSize() const { return occupied; }

	/**	\brief Determines whether the cache is overfilled.
	 *
	 * 	@return true if occupied size > capacity, false otherwise.
	 */
	bool isOverfilled() const { return occupied > capacity; }

	/**	\brief Removes all cache entries.
	 *
	 * 	This is a synonym for free(0).
	 *
	 * 	@see free()
	 */
	bool clear();

	/**	\brief Sets the new capacity of the cache.
	 *
	 * 	When the new capacity is lower than the old, this might mean that entries are removed from the
	 * 	cache by a call to free().
	 *
	 * 	@param capacity The new capacity.
	 */
	void resize(cachesize_t capacity) { free(capacity); this->capacity = capacity; }

	/**	\brief Sets the locked status of an entry.
	 *
	 * 	This method is used both to lock and unlock entries. If an entry is unlocked in an overfilled cache,
	 * 	it is instantly removed by this method. Otherwise, a pointer to it's value is returned which may be
	 * 	useful to the caller.
	 *
	 * 	@param key The entry's key.
	 * 	@param locked The new locked status.
	 * 	@return A pointer to the locked or unlocked value. If an entry was unlocked in an overfilled cache,
	 * 		it will be removed by this method. In this case, NULL is returned.
	 */
	V* lock(const K& key, bool locked = true);

	/**	\brief Unlocks an entry in the cache.
	 *
	 * 	This is a synonym for lock(const K&, false).
	 *
	 * 	@see lock()
	 */
	V* unlock(const K& key) { return lock(key, false); }

private:
	/**	\brief Internal name of method item().
	 *
	 * 	@see item()
	 */
	V* access(const K& key);
	bool remove(Entry& entry);

private:
	EntryMap entries;

public:
	Entry* first;

private:
	Entry* last;
	cachesize_t capacity;
	cachesize_t occupied;
};


template<class K, class V, class Compare, class MapHash, class KeyEqual>
V* Cache<K, V, Compare, MapHash, KeyEqual>::access(const K& key)
{
	typename EntryMap::iterator it = entries.find(key);

	if (it == entries.end()) {
		return NULL;
	}

	Entry& entry = it->second;
	if (&entry != first) {
		if (entry.prev) {
			entry.prev->next = entry.next;
		}
		if (entry.next) {
			entry.next->prev = entry.prev;
		}
		if (&entry == last) {
			last = entry.prev;
		}
		entry.prev = NULL;
		entry.next = first;
		first->prev = &entry;
		first = &entry;
	}

	return entry.vPtr;
}


template<class K, class V, class Compare, class MapHash, class KeyEqual>
bool Cache<K, V, Compare, MapHash, KeyEqual>::remove(Entry& entry)
{
	if (entry.locked) {
		return false;
	}

	if (entry.prev) {
		entry.prev->next = entry.next;
	}
	if (entry.next) {
		entry.next->prev = entry.prev;
	}
	if (&entry == first) {
		first = entry.next;
	}
	if (&entry == last) {
		last = entry.prev;
	}
	occupied -= entry.cost;
	delete entry.vPtr;
	entries.erase(entries.find(*entry.kPtr));
	return true;
}


template<class K, class V, class Compare, class MapHash, class KeyEqual>
bool Cache<K, V, Compare, MapHash, KeyEqual>::insert(const K& key, V* value, cachesize_t size, bool locked)
{
	if (size > capacity  &&  !locked) {
		delete value;
		return false;
	}
	bool enoughLeft = free(size > capacity ? 0 : capacity-size);
	if (!enoughLeft  &&  !locked) {
		delete value;
		return false;
	}
	typename EntryMap::iterator it = entries
			.insert(pair<const K, Entry>(key, Entry(value, size, locked))).first;
	Entry& entry = it->second;
	if (first) {
		first->prev = &entry;
		entry.next = first;
	}
	entry.kPtr = &it->first;
	first = &entry;
	if (!last) {
		last = &entry;
	}
	occupied += size;
	return true;
}


template<class K, class V, class Compare, class MapHash, class KeyEqual>
bool Cache<K, V, Compare, MapHash, KeyEqual>::free(cachesize_t size)
{
	Entry* entry = last;
	while (entry  &&  occupied > size) {
		Entry* cur = entry;
		entry = entry->prev;
		remove(*cur);
	}

	return occupied <= size;
}


template<class K, class V, class Compare, class MapHash, class KeyEqual>
bool Cache<K, V, Compare, MapHash, KeyEqual>::clear()
{
	return free(0);
}


template<class K, class V, class Compare, class MapHash, class KeyEqual>
bool Cache<K, V, Compare, MapHash, KeyEqual>::remove(const K& key)
{
	typename EntryMap::iterator it = entries.find(key);
	return remove(it->second);
}


template<class K, class V, class Compare, class MapHash, class KeyEqual>
V* Cache<K, V, Compare, MapHash, KeyEqual>::lock(const K& key, bool locked)
{
	typename EntryMap::iterator it = entries.find(key);

	if (it == entries.end()) {
		return NULL;
	}

	bool wasOverfilled = occupied > capacity;

	Entry& entry = it->second;
	entry.locked = locked;
	free(capacity);

	if (!locked  &&  wasOverfilled) {
		// If an entry is unlocked in an overfilled cache, it will be deleted by the above call to free().
		return NULL;
	} else {
		return entry.vPtr;
	}
}

#endif /* CACHE_H_ */

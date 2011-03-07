/*
	Copyright 2010-2011 David "Alemarius Nexus" Lerch

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
 */

#ifndef CACHE_H_
#define CACHE_H_

#include "../config.h"
#include <map>
#include <utility>

using std::map;
using std::pair;



template<class K, class V>
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

public:
	Cache(cachesize_t capacity = 0) : first(NULL), last(NULL), capacity(capacity), occupied(0) {}
	~Cache() { clear(); }
	bool insert(const K& key, V* value, cachesize_t size, bool locked = false);
	bool remove(const K& key);
	V* item(const K& key) { return access(key); }
	bool free(cachesize_t size);
	V* operator[](const K& key) { return item(key); }
	cachesize_t getCapacity() const { return capacity; }
	cachesize_t getOccupiedSize() const { return occupied; }
	bool clear();
	void resize(cachesize_t capacity) { free(capacity); this->capacity = capacity; }
	V* lock(const K& key, bool locked = true);
	V* unlock(const K& key) { return lock(key, false); }

private:
	V* access(const K& key);
	bool remove(Entry& entry);

private:
	map<K, Entry> entries;
	Entry* first;
	Entry* last;
	cachesize_t capacity;
	cachesize_t occupied;
};


template<class K, class V>
V* Cache<K, V>::access(const K& key)
{
	typename map<K, Entry>::iterator it = entries.find(key);

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


template<class K, class V>
bool Cache<K, V>::remove(Entry& entry)
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


template<class K, class V>
bool Cache<K, V>::insert(const K& key, V* value, cachesize_t size, bool locked)
{
	if (size > capacity  &&  !locked) {
		delete value;
		return false;
	}
	free(capacity-size);
	typename map<K, Entry>::iterator it = entries
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


template<class K, class V>
bool Cache<K, V>::free(cachesize_t size)
{
	Entry* entry = last;
	while (entry  &&  occupied > size) {
		Entry* cur = entry;
		entry = entry->prev;
		remove(*cur);
	}

	return occupied <= size;
}


template<class K, class V>
bool Cache<K, V>::clear()
{
	/*while (first) {
		delete first->vPtr;
		first = first->next;
	}
	occupied = 0;
	entries.clear();
	last = NULL;*/

	return free(0);
}


template<class K, class V>
bool Cache<K, V>::remove(const K& key)
{
	typename map<K, Entry>::iterator it = entries.find(key);
	return remove(it->second);
}


template<class K, class V>
V* Cache<K, V>::lock(const K& key, bool locked)
{
	typename map<K, Entry>::iterator it = entries.find(key);

	if (it == entries.end()) {
		return NULL;
	}

	Entry& entry = it->second;
	entry.locked = locked;
	free(capacity);
	return entry.vPtr;
}

#endif /* CACHE_H_ */

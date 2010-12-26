/*
 * Cache.h
 *
 *  Created on: 23.10.2010
 *      Author: alemariusnexus
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
private:
	struct Entry
	{
		Entry(V* vPtr, int cost) : vPtr(vPtr), cost(cost), prev(NULL), next(NULL) {}
		const K* kPtr;
		V* vPtr;
		int cost;
		Entry* prev;
		Entry* next;
	};

public:
	Cache(int capacity = 0) : capacity(capacity), occupied(0), first(NULL), last(NULL) {}
	~Cache() { clear(); }
	bool insert(const K& key, V* value, int size);
	void remove(const K& key);
	V* item(const K& key) { return access(key); }
	void free(int size);
	V* operator[](const K& key) { return item(key); }
	int getCapacity() const { return capacity; }
	int getOccupiedSize() const { return occupied; }
	void clear();
	void resize(int capacity) { free(capacity); this->capacity = capacity; }

private:
	V* access(const K& key);
	void remove(Entry& entry);

private:
	map<K, Entry> entries;
	Entry* first;
	Entry* last;
	int capacity;
	int occupied;
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
void Cache<K, V>::remove(Entry& entry)
{
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
}


template<class K, class V>
bool Cache<K, V>::insert(const K& key, V* value, int size)
{
	if (size > capacity) {
		delete value;
		return false;
	}
	free(capacity-size);
	typename map<K, Entry>::iterator it = entries.insert(pair<const K, Entry>(key, Entry(value, size))).first;
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
void Cache<K, V>::free(int size)
{
	Entry* entry = last;
	while (entry  &&  occupied > size) {
		Entry* cur = entry;
		entry = entry->prev;
		remove(*cur);
	}
}


template<class K, class V>
void Cache<K, V>::clear()
{
	while (first) {
		delete first->vPtr;
		first = first->next;
	}
	occupied = 0;
	entries.clear();
	last = NULL;
}


template<class K, class V>
void Cache<K, V>::remove(const K& key)
{
	typename map<K, Entry>::iterator it = entries.find(key);
	remove(it->second);
}

#endif /* CACHE_H_ */

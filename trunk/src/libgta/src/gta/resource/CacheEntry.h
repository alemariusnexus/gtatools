/*
 * CacheEntry.h
 *
 *  Created on: 06.03.2011
 *      Author: alemariusnexus
 */

#ifndef CACHEENTRY_H_
#define CACHEENTRY_H_

class ResourceCache;


typedef unsigned int cachesize_t;


class CacheEntry {
public:
	virtual ~CacheEntry() {}
	virtual cachesize_t getSize() const = 0;
};

#endif /* CACHEENTRY_H_ */

/*
 * CacheEntryLoader.h
 *
 *  Created on: 06.03.2011
 *      Author: alemariusnexus
 */

#ifndef CACHEENTRYLOADER_H_
#define CACHEENTRYLOADER_H_

#include "../Engine.h"

class CacheEntry;


class CacheEntryLoader {
public:
	virtual CacheEntry* load(hash_t key) = 0;
};

#endif /* CACHEENTRYLOADER_H_ */

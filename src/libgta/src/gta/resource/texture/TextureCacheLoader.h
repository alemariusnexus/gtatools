/*
 * TextureCacheLoader.h
 *
 *  Created on: 07.03.2011
 *      Author: alemariusnexus
 */

#ifndef TEXTURECACHELOADER_H_
#define TEXTURECACHELOADER_H_

#include "../CacheEntryLoader.h"
#include "TextureIndexer.h"


class TextureCacheLoader : public CacheEntryLoader {
public:
	TextureCacheLoader(TextureIndexer* indexer);
	virtual CacheEntry* load(hash_t hash);
	TextureIndexer* getIndexer() { return indexer; }

private:
	TextureIndexer* indexer;
};

#endif /* TEXTURECACHELOADER_H_ */

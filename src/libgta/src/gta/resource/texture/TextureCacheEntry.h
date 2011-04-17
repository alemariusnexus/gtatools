/*
 * TextureCacheEntry.h
 *
 *  Created on: 06.03.2011
 *      Author: alemariusnexus
 */

#ifndef TEXTURECACHEENTRY_H_
#define TEXTURECACHEENTRY_H_

#include "../../config.h"
#include "../CacheEntry.h"
#include "TextureArchive.h"
#include <map>

#ifdef CXX0X_AVAILABLE
#include <unordered_map>
using std::unordered_map;
#else
#include <map>
using std::map;
#endif


class TextureCacheEntry : public CacheEntry {
private:
#ifdef CXX0X_AVAILABLE
	typedef unordered_map<hash_t, GLuint> TextureMap;
#else
	typedef map<hash_t, GLuint> TextureMap;
#endif

public:
	TextureCacheEntry() : size(0) {}
	virtual ~TextureCacheEntry();
	void addTexture(hash_t name, GLuint tex, cachesize_t size);
	virtual cachesize_t getSize() const { return size; }
	GLuint getTexture(hash_t texName) const;
	GLuint operator[](hash_t texName) const { return getTexture(texName); }

private:
	TextureMap texMap;
	cachesize_t size;
};

#endif /* TEXTURECACHEENTRY_H_ */

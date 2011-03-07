/*
 * TextureCacheEntry.h
 *
 *  Created on: 06.03.2011
 *      Author: alemariusnexus
 */

#ifndef TEXTURECACHEENTRY_H_
#define TEXTURECACHEENTRY_H_

#include "../CacheEntry.h"


class TextureCacheEntry : public CacheEntry {
public:
	TextureCacheEntry(GLuint texture, cachesize_t size) : texture(texture), size(size) {}
	virtual ~TextureCacheEntry() { glDeleteTextures(1, &texture); }
	virtual cachesize_t getSize() const { return size; }
	GLuint getTexture() { return texture; }
	GLuint operator*() { return texture; }

private:
	GLuint texture;
	cachesize_t size;
};

#endif /* TEXTURECACHEENTRY_H_ */

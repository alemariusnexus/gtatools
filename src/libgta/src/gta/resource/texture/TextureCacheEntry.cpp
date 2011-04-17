/*
 * TextureCacheEntry.cpp
 *
 *  Created on: 10.04.2011
 *      Author: alemariusnexus
 */

#include "TextureCacheEntry.h"
#include <utility>

using std::pair;




TextureCacheEntry::~TextureCacheEntry()
{
	TextureMap::iterator it;
	for (it = texMap.begin() ; it != texMap.end() ; it++) {
		glDeleteTextures(1, &it->second);
	}
}


void TextureCacheEntry::addTexture(hash_t name, GLuint tex, cachesize_t size)
{
	texMap.insert(pair<hash_t, GLuint>(name, tex));
	this->size += size;
}


GLuint TextureCacheEntry::getTexture(hash_t texName) const
{
	TextureMap::const_iterator it = texMap.find(texName);

	if (it == texMap.end()) {
		return 0;
	}

	return it->second;
}

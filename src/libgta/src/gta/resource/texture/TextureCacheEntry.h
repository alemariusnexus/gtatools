/*
	Copyright 2010-2011 David "Alemarius Nexus" Lerch

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

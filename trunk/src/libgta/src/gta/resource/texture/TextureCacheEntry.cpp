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

#include "TextureCacheEntry.h"
#include <utility>
#include "../../Engine.h"
#include "../ResourceCache.h"

using std::pair;



TextureCacheEntry::TextureCacheEntry(TextureArchive* archive)
		: size(0), parent(0)
{
	if (archive->getParent()) {
		parent = archive->getParent()->getName();
	}
}


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
		if (parent != 0) {
			TextureCacheEntry* entry = (TextureCacheEntry*)
					Engine::getInstance()->getTextureCache()->getEntry(parent);

			if (entry) {
				return entry->getTexture(texName);
			}
		}
		return 0;
	}

	return it->second;
}

/*
	Copyright 2010-2014 David "Alemarius Nexus" Lerch

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
#include <nxcommon/ResourceCache.h>
#include <functional>

using std::pair;



TextureCacheEntry::TextureCacheEntry(TextureArchive* archive)
#ifdef ADHOC_MAP
		: numTexes(0), size(0)
#else
		: size(0)
#endif
{
	if (archive->getParent()) {
		parentPtr = archive->getParent()->getCachePointer();
	}
}


TextureCacheEntry::~TextureCacheEntry()
{
#ifndef ADHOC_MAP
	TextureMap::iterator it;
	for (it = texMap.begin() ; it != texMap.end() ; it++) {
		delete it->second;
	}
#else
	for (size_t i = 0 ; i < numTexes ; i++) {
		delete texValues[i];
	}

	if (numTexes != 0)
		delete[] texValues;
#endif
}



void TextureCacheEntry::addTexture(const CString& name, Texture* tex)
{
	texMap.insert(pair<CString, Texture*>(name, tex));
	this->size += tex->getSize();
}


Texture* TextureCacheEntry::getTexture(const CString& texName)
{
	TextureMap::const_iterator it = texMap.find(texName);

	if (it == texMap.end()) {
		/*if (parentPtr.isValid()) {
			TextureCacheEntry* entry = (TextureCacheEntry*) parentPtr.getEntry();

			if (entry) {
				return entry->getTexture(texName);
			}
		}*/
		return NULL;
	}

	return it->second;
}

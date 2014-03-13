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

#ifndef TEXTURECACHEENTRY_H_
#define TEXTURECACHEENTRY_H_


#define ADHOC_MAP


#include <gta/config.h>
#include <nxcommon/cxx11hash.h>
#include <nxcommon/CString.h>
#include <nxcommon/strutil.h>
#include <nxcommon/ResourceCache.h>
#include "TextureArchive.h"
#include "Texture.h"
#include <map>
#include <unordered_map>

using std::unordered_map;


class TextureCacheEntry : public Engine::StringResourceCache::Entry {
private:
	typedef unordered_map<CString, Texture*, CXX11Hash<CString> > TextureMap;

public:
	TextureCacheEntry(TextureArchive* archive);
	virtual ~TextureCacheEntry();
	void addTexture(const CString&, Texture* tex);
	virtual cachesize_t getSize() const { return size; }
	Texture* getTexture(const CString& texName);
	Texture* operator[](const CString& texName) { return getTexture(texName); }

private:
#ifndef ADHOC_MAP
	TextureMap texMap;
#else
	hash_t* texKeys;
	Texture** texValues;
	size_t numTexes;
#endif

	cachesize_t size;
	Engine::StringResourceCache::Pointer parentPtr;
};

#endif /* TEXTURECACHEENTRY_H_ */

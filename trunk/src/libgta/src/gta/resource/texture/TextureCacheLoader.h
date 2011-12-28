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

#ifndef TEXTURECACHELOADER_H_
#define TEXTURECACHELOADER_H_

#include "../ResourceCache.h"
#include "TextureIndexer.h"


class TextureCacheLoader : public Engine::StringResourceCache::EntryLoader {
public:
	TextureCacheLoader(TextureIndexer* indexer);
	virtual Engine::StringResourceCache::Entry* load(CString name);
	TextureIndexer* getIndexer() { return indexer; }

private:
	TextureIndexer* indexer;
};

#endif /* TEXTURECACHELOADER_H_ */

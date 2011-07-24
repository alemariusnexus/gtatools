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

#include "TextureCacheLoader.h"
#include "TextureCacheEntry.h"
#include "TextureArchive.h"
#include <gtaformats/txd/TXDArchive.h>
#include <gtaformats/txd/TXDTextureHeader.h>
#include <gtaformats/txd/TXDConverter.h>
#include <gtaformats/util/strutil.h>
#include "../../gl.h"
#include "../../EngineException.h"
#include "../../GLException.h"




TextureCacheLoader::TextureCacheLoader(TextureIndexer* indexer)
		: indexer(indexer)
{
}


CacheEntry* TextureCacheLoader::load(hash_t hash)
{
	TextureArchive* indexArchive = indexer->findArchive(hash);

	if (!indexArchive) {
		return NULL;
	}

	TXDArchive txd(indexArchive->getFile());

	TextureCacheEntry* cacheEntry = new TextureCacheEntry(indexArchive);

	for (TXDArchive::TextureIterator it = txd.getHeaderBegin() ; it != txd.getHeaderEnd() ; it++) {
		TXDTextureHeader* txdTex = *it;
		Texture* tex = new Texture(&txd, txdTex);

		const char* name = txdTex->getDiffuseName();
		char* lname = new char[strlen(name)+1];
		strtolower(lname, name);
		hash_t texHash = Hash(lname);
		delete[] lname;

		cacheEntry->addTexture(texHash, tex);
	}

	return cacheEntry;
}

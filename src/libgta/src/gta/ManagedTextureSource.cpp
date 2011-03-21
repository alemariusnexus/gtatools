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
 */

#include "ManagedTextureSource.h"
#include "resource/ResourceCache.h"
#include "resource/texture/TextureCacheLoader.h"
#include "resource/texture/TextureIndexer.h"
#include "resource/texture/TextureCacheEntry.h"
#include "Engine.h"



GLuint ManagedTextureSource::getTexture(hash_t texHash)
{
	ResourceCache* cache = Engine::getInstance()->getTextureCache();
	TextureIndexer* indexer = Engine::getInstance()->getTextureIndexer();
	hash_t hash;

	if (!indexer->resolveCombinedHash(txdHash, texHash, hash)) {
		return 0;
	}

	TextureCacheEntry* entry = (TextureCacheEntry*) cache->getEntry(hash);

	return entry->getTexture();
	/*ResourceManager* resMgr = Engine::getInstance()->getResourceManager();
	GLuint tex = resMgr->getTexture(TextureIndex(txdHash, texHash));
	return tex;*/
}

/*
	Copyright 2010-2012 David "Alemarius Nexus" Lerch

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

#include "CollisionMeshCacheLoader.h"
#include "COLBulletConverter.h"
#include "CollisionMeshCacheEntry.h"




CollisionMeshCacheLoader::CollisionMeshCacheLoader(CollisionMeshIndexer* indexer)
		: indexer(indexer)
{
}


Engine::StringResourceCache::Entry* CollisionMeshCacheLoader::load(CString name)
{
	CollisionMeshIndexer::CollisionMeshIndexEntry* entry = indexer->getCollisionMesh(name);

	if (!entry) {
		return NULL;
	}

	COLLoader col;
	istream* stream = entry->file->openInputStream(istream::binary);
	col.skip(stream, entry->index);
	COLModel* model = col.loadModel(stream);
	COLBulletConverter conv;
	btCollisionShape* shape = conv.convert(*model);
	delete model;
	delete stream;

	CollisionMeshCacheEntry* cacheEntry = new CollisionMeshCacheEntry(shape, 10);
	return cacheEntry;
}

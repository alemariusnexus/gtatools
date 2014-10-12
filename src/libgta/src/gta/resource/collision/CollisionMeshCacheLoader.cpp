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

#include "CollisionMeshCacheLoader.h"
#include "CollisionModel.h"
#include "CollisionMeshCacheEntry.h"
#include "../smesh/ShadowMesh.h"
#include <gtaformats/col/COLLoader.h>
#include <gtaformats/dff/DFFLoader.h>




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

	DFFMesh* dffMesh = NULL;
	COLModel* model;

	if (entry->file->guessContentType() == CONTENT_TYPE_COL) {
		COLLoader col;
		istream* stream = entry->file->openInputStream(istream::binary);
		col.skip(stream, entry->index);
		model = col.loadModel(stream);
		delete stream;
	} else {
		DFFLoader dff;
		dffMesh = dff.loadMesh(*entry->file);
		model = dffMesh->getIntegratedCOLModel();
	}

	if (!model) {
		if (dffMesh) {
			delete dffMesh;
		}

		return NULL;
	}

	CollisionModel* mmodel = new CollisionModel(*model);
	cachesize_t size = mmodel->getCacheSize();

	ShadowMesh* smesh = NULL;

	if (model->getShadowMesh()) {
		COLShadowMesh* csmesh = model->getShadowMesh();
		smesh = new ShadowMesh(*csmesh);
	}

	if (dffMesh) {
		delete dffMesh;
	} else {
		delete model;
	}

	CollisionMeshCacheEntry* cacheEntry = new CollisionMeshCacheEntry(mmodel, smesh, size);
	return cacheEntry;
}

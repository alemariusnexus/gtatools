/*
 * MeshCacheLoader.cpp
 *
 *  Created on: 06.03.2011
 *      Author: alemariusnexus
 */

#include "MeshCacheLoader.h"
#include "MeshCacheEntry.h"
#include <gtaformats/dff/DFFMesh.h>
#include <gtaformats/dff/DFFLoader.h>
#include <gtaformats/dff/DFFGeometry.h>
#include "../../Mesh.h"



CacheEntry* MeshCacheLoader::load(hash_t key)
{
	const File* file = indexer->find(key);

	if (!file) {
		return NULL;
	}

	DFFLoader dff;
	DFFMesh* dffMesh = dff.loadMesh(*file);

	if (dffMesh->getGeometryCount() < 1) {
		printf("ERROR in MeshCacheLoader::load()\n");
		exit(1337);
	}

	Mesh* mesh = new Mesh(*dffMesh->getGeometry((uint32_t) 0));
	MeshCacheEntry* entry = new MeshCacheEntry(mesh);
	return entry;
}

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

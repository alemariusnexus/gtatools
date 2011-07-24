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
#include "Mesh.h"
#include "../../EngineException.h"



CacheEntry* MeshCacheLoader::load(hash_t key)
{
	const File* file = indexer->find(key);

	if (!file) {
		return NULL;
	}

	DFFLoader dff;
	DFFMesh* dffMesh;

	try {
		dffMesh = dff.loadMesh(*file);
	} catch (Exception ex) {
		char* errmsg = new char[64 + strlen(file->getPath()->toString())];
		sprintf(errmsg, "Exception thrown during loading of DFF mesh %s.", file->getPath()->toString());
		EngineException eex(errmsg, __FILE__, __LINE__, &ex);
		delete[] errmsg;
		throw eex;
	}

	Mesh* mesh = NULL;

	if (dffMesh->getGeometryCount() > 0) {
		mesh = new Mesh(*dffMesh->getGeometry(dffMesh->getGeometryCount() - 1));
	}

	delete dffMesh;
	MeshCacheEntry* entry = new MeshCacheEntry(mesh);
	return entry;
}

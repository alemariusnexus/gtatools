/*
	Copyright 2010-2013 David "Alemarius Nexus" Lerch

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
#include <gtaformats/col/COLLoader.h>
#include "../collision/COLMeshConverter.h"
#include "Mesh.h"
#include "../../EngineException.h"



Engine::StringResourceCache::Entry* MeshCacheLoader::load(CString key)
{
#if 1
	MeshIndexer::IndexEntry ientry = indexer->find(key);

	if (!ientry.file) {
		return NULL;
	}

	DFFLoader dff;
	DFFMesh* dffMesh;

	try {
		dffMesh = dff.loadMesh(*ientry.file);
	} catch (Exception ex) {
		char* errmsg = new char[64 + ientry.file->getPath()->toString().length()];
		sprintf(errmsg, "Exception thrown during loading of DFF mesh %s.", ientry.file->getPath()->toString().get());
		EngineException eex(errmsg, __FILE__, __LINE__, &ex);
		delete[] errmsg;
		throw eex;
	}

	MeshClump* clump = new MeshClump(dffMesh);

	delete dffMesh;

	MeshCacheEntry* entry = new MeshCacheEntry(clump);
	return entry;
#else
	/*if (key.lower() != CString("mtraffic1")) {
		return NULL;
	}*/

	CollisionMeshIndexer::CollisionMeshIndexEntry* ientry = colIndexer->getCollisionMesh(key);

	if (!ientry) {
		return NULL;
	}

	//printf("%s : %d\n", ientry->file->getPath()->toString().get(), ientry->index);

	COLLoader col;
	istream* stream = ientry->file->openInputStream(istream::binary);
	col.skip(stream, ientry->index);
	COLModel* model = col.loadModel(stream);

	COLBox* boxes = model->getBoxes();
	COLSphere* spheres = model->getSpheres();

	COLMeshConverter conv;
	Mesh* mesh = conv.convert(*model);

	MeshFrame* rootFrame = new MeshFrame;
	mesh->setFrame(rootFrame);

	MeshClump* clump = new MeshClump;
	clump->setRootFrame(rootFrame);
	clump->addMesh(mesh);


	delete model;

	MeshCacheEntry* entry = new MeshCacheEntry(clump);
	return entry;
#endif
}

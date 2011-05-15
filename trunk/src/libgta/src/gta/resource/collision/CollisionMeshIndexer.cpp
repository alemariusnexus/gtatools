/*
 * CollisionMeshIndexer.cpp
 *
 *  Created on: 30.04.2011
 *      Author: alemariusnexus
 */

#include "CollisionMeshIndexer.h"
#include <gtaformats/gtacol.h>
#include <gtaformats/util/strutil.h>
#include <utility>

using std::pair;



void CollisionMeshIndexer::resourceAdded(const File& file)
{
	if (file.guessContentType() == CONTENT_TYPE_COL) {
		COLLoader col;
		istream* stream = file.openInputStream(istream::binary);
		char name[20];
		int i = 0;

		while (col.loadModelName(stream, name)) {
			CollisionMeshIndexEntry* entry = new CollisionMeshIndexEntry;
			entry->file = new File(file);
			entry->index = i++;
			index.insert(pair<hash_t, CollisionMeshIndexEntry*>(LowerHash(name), entry));
		}

		delete stream;
	}
}


CollisionMeshIndexer::CollisionMeshIndexEntry* CollisionMeshIndexer::getCollisionMesh(hash_t name)
{
	IndexMap::iterator it = index.find(name);

	if (it == index.end()) {
		return NULL;
	}

	return it->second;
}

/*
 * MeshIndexer.cpp
 *
 *  Created on: 07.03.2011
 *      Author: alemariusnexus
 */

#include "MeshIndexer.h"
#include <cstring>
#include <gtaformats/util/strutil.h>
#include <utility>

using std::pair;



MeshIndexer::~MeshIndexer()
{
	IndexMap::iterator it;

	for (it = index.begin() ; it != index.end() ; it++) {
		delete it->second;
	}

	index.clear();
}


void MeshIndexer::resourceAdded(const File& file)
{
	if (file.guessContentType() == CONTENT_TYPE_DFF) {
		const char* fname = file.getPath()->getFileName();
		char* lMeshName = new char[strlen(fname)+1];
		strtolower(lMeshName, fname);
		*strrchr(lMeshName, '.') = '\0';
		index.insert(pair<hash_t, File*>(Hash(lMeshName), new File(file)));
		delete[] lMeshName;
	}
}


const File* MeshIndexer::find(hash_t name)
{
	IndexMap::iterator it = index.find(name);

	if (it == index.end()) {
		return NULL;
	}

	return it->second;
}

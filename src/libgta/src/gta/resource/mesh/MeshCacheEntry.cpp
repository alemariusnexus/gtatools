/*
 * MeshCacheEntry.cpp
 *
 *  Created on: 06.03.2011
 *      Author: alemariusnexus
 */

#include "MeshCacheEntry.h"



MeshCacheEntry::MeshCacheEntry(Mesh* mesh)
		: mesh(mesh)
{
}


MeshCacheEntry::~MeshCacheEntry()
{
	delete mesh;
}


cachesize_t MeshCacheEntry::getSize() const
{
	return mesh->guessSize();
}

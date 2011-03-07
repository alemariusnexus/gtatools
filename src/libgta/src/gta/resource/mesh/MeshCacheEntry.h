/*
 * MeshCacheEntry.h
 *
 *  Created on: 06.03.2011
 *      Author: alemariusnexus
 */

#ifndef MESHCACHEENTRY_H_
#define MESHCACHEENTRY_H_

#include "../CacheEntry.h"
#include "../../Mesh.h"


class MeshCacheEntry : public CacheEntry {
public:
	MeshCacheEntry(Mesh* mesh);
	virtual ~MeshCacheEntry();
	virtual cachesize_t getSize() const;
	Mesh* getMesh() { return mesh; }
	Mesh* operator*() { return mesh; }

private:
	Mesh* mesh;
};

#endif /* MESHCACHEENTRY_H_ */

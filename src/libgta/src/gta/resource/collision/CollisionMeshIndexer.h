/*
 * CollisionMeshIndexer.h
 *
 *  Created on: 30.04.2011
 *      Author: alemariusnexus
 */

#ifndef COLLISIONMESHINDEXER_H_
#define COLLISIONMESHINDEXER_H_

#include "../ResourceObserver.h"
#include <gta/config.h>
#include <gtaformats/util/strutil.h>

#ifdef CXX0X_AVAILABLE
#include <unordered_map>
using std::unordered_map;
#else
#include <map>
using std::map;
#endif



class CollisionMeshIndexer : public ResourceObserver {
public:
	struct CollisionMeshIndexEntry
	{
		File* file;
		int index;
	};

private:
#ifdef CXX0X_AVAILABLE
	typedef unordered_map<hash_t, CollisionMeshIndexEntry*> IndexMap;
#else
	typedef map<hash_t, CollisionMeshIndexEntry*> IndexMap;
#endif

public:
	virtual void resourceAdded(const File& file);
	CollisionMeshIndexEntry* getCollisionMesh(hash_t name);
	CollisionMeshIndexEntry* operator[](hash_t name) { return getCollisionMesh(name); }

private:
	IndexMap index;
};

#endif /* COLLISIONMESHINDEXER_H_ */

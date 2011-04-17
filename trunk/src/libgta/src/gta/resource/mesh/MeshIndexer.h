/*
 * MeshIndexer.h
 *
 *  Created on: 07.03.2011
 *      Author: alemariusnexus
 */

#ifndef MESHINDEXER_H_
#define MESHINDEXER_H_

#include "../../config.h"
#include <gtaformats/util/File.h>
#include "../../Engine.h"
#include "../ResourceObserver.h"

#ifdef CXX0X_AVAILABLE
#include <unordered_map>
using std::unordered_map;
#else
#include <map>
using std::map;
#endif


class MeshIndexer : public ResourceObserver {
private:
#ifdef CXX0X_AVAILABLE
	typedef unordered_map<hash_t, File*> IndexMap;
#else
	typedef map<hash_t, File*> IndexMap;
#endif

public:
	~MeshIndexer();
	virtual void resourceAdded(const File& file);
	const File* find(hash_t name);
	const File* operator[](hash_t name) { return find(name); }

private:
	IndexMap index;
};

#endif /* MESHINDEXER_H_ */

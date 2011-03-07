/*
 * MeshIndexer.h
 *
 *  Created on: 07.03.2011
 *      Author: alemariusnexus
 */

#ifndef MESHINDEXER_H_
#define MESHINDEXER_H_

#include <gtaformats/util/File.h>
#include "../../Engine.h"
#include "../ResourceObserver.h"
#include <map>

using std::map;


class MeshIndexer : public ResourceObserver {
private:
	typedef map<hash_t, File*> IndexMap;

public:
	~MeshIndexer();
	virtual void resourceAdded(const File& file);
	const File* find(hash_t name);
	const File* operator[](hash_t name) { return find(name); }

private:
	IndexMap index;
};

#endif /* MESHINDEXER_H_ */

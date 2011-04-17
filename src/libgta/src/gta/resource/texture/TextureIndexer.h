/*
 * TextureIndexer.h
 *
 *  Created on: 07.03.2011
 *      Author: alemariusnexus
 */

#ifndef TEXTUREINDEXER_H_
#define TEXTUREINDEXER_H_

#include "../../config.h"
#include "../ResourceObserver.h"
#include "TextureArchive.h"
#include "../../Engine.h"

#ifdef CXX0X_AVAILABLE
#include <unordered_map>
using std::unordered_map;
#else
#include <map>
using std::map;
#endif


struct TextureIndexEntry
{
	File* file;
	int16_t index;
};


class TextureIndexer : public ResourceObserver {
private:
#ifdef CXX0X_AVAILABLE
	typedef unordered_map<hash_t, TextureArchive*> ArchiveMap;
#else
	typedef map<hash_t, TextureArchive*> ArchiveMap;
#endif

public:
	~TextureIndexer();
	virtual void resourceAdded(const File& file);
	virtual void resourcesCleared();
	TextureArchive* findArchive(hash_t name);

private:
	ArchiveMap archives;
};

#endif /* TEXTUREINDEXER_H_ */

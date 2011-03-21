/*
 * TextureIndexer.h
 *
 *  Created on: 07.03.2011
 *      Author: alemariusnexus
 */

#ifndef TEXTUREINDEXER_H_
#define TEXTUREINDEXER_H_

#include "../ResourceObserver.h"
#include "TextureArchive.h"
#include <map>
#include "../../Engine.h"

using std::map;


struct TextureIndexEntry
{
	File* file;
	int16_t index;
};


class TextureIndexer : public ResourceObserver {
private:
	typedef map<hash_t, TextureIndexEntry*> IndexMap;
	typedef map<hash_t, TextureArchive*> ArchiveMap;

public:
	~TextureIndexer();
	virtual void resourceAdded(const File& file);
	virtual void resourcesCleared();
	const TextureIndexEntry* find(hash_t combinedHash);
	const TextureIndexEntry* operator[](hash_t combinedHash);
	hash_t createCombinedHash(const char* txdName, const char* texName);
	bool resolveCombinedHash(hash_t txdHash, hash_t texHash, hash_t& combinedHash);
	bool resolveCombinedHash(const char* txdName, const char* texName, hash_t& combinedHash)
			{ return resolveCombinedHash(Hash(txdName), Hash(texName), combinedHash); }
	const TextureIndexEntry* find(hash_t txdHash, hash_t texHash);
	const TextureIndexEntry* find(const char* txdName, const char* texName)
			{ return find(Hash(txdName), Hash(texName)); }

private:
	IndexMap txdTexCombinedIndex;
	ArchiveMap archives;
};

#endif /* TEXTUREINDEXER_H_ */

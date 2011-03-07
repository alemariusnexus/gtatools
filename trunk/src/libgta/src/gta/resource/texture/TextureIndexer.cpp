/*
 * TextureIndexer.cpp
 *
 *  Created on: 07.03.2011
 *      Author: alemariusnexus
 */

#include "TextureIndexer.h"
#include <gtaformats/txd/TXDArchive.h>
#include <gtaformats/txd/TXDTexture.h>
#include <gtaformats/util/strutil.h>
#include <cstring>
#include <cstdio>
#include <utility>

using std::pair;



TextureIndexer::~TextureIndexer()
{
	IndexMap::iterator it;

	for (it = txdTexCombinedIndex.begin() ; it != txdTexCombinedIndex.end() ; it++) {
		TextureIndexEntry* entry = it->second;
		delete entry->file;
		delete entry;
	}
}


void TextureIndexer::resourceAdded(const File& file)
{
	if (file.guessContentType() == CONTENT_TYPE_TXD) {
		const char* fname = file.getPath()->getFileName();
		char* txdName = new char[strlen(fname)+1];
		strtolower(txdName, fname);
		*strrchr(txdName, '.') = '\0';

		int txdLen = strlen(txdName);
		char* combinedName = new char[txdLen+33];
		strcpy(combinedName, txdName);
		*(combinedName+txdLen) = ':';
		char* texName = combinedName+txdLen+1;

		hash_t txdHash = Hash(txdName);

		TXDArchive txd(file);

		TextureArchive* archive = new TextureArchive(txdHash);

		for (int16_t i = 0 ; i < txd.getTextureCount() ; i++) {
			TXDTexture* tex = txd.nextTexture();
			TextureIndexEntry* entry = new TextureIndexEntry;
			entry->file = new File(file);
			entry->index = i;

			strtolower(texName, tex->getDiffuseName());
			hash_t combinedHash = Hash(combinedName);
			txdTexCombinedIndex.insert(pair<hash_t, TextureIndexEntry*>(combinedHash, entry));
			archive->addTexture(Hash(texName), combinedHash);

			delete tex;
		}

		archives.insert(pair<hash_t, TextureArchive*>(txdHash, archive));

		delete[] combinedName;
		delete[] txdName;
	}
}


const TextureIndexEntry* TextureIndexer::find(hash_t combinedHash)
{
	IndexMap::iterator it = txdTexCombinedIndex.find(combinedHash);

	if (it == txdTexCombinedIndex.end()) {
		return NULL;
	}

	return it->second;
}


bool TextureIndexer::resolveCombinedHash(hash_t txdHash, hash_t texHash, hash_t& combinedHash)
{
	ArchiveMap::iterator it = archives.find(txdHash);

	if (it == archives.end()) {
		return false;
	}

	TextureArchive* archive = it->second;

	return archive->getTextureCombinedHash(texHash, combinedHash);
}


hash_t TextureIndexer::createCombinedHash(const char* txdName, const char* texName)
{
	char* combinedName = new char[strlen(txdName)+strlen(texName)+2];
	sprintf(combinedName, "%s:%s", txdName, texName);
	hash_t hash = Hash(combinedName);
	delete[] combinedName;
	return hash;
}


const TextureIndexEntry* TextureIndexer::find(hash_t txdHash, hash_t texHash)
{
	hash_t hash;

	if (!resolveCombinedHash(txdHash, texHash, hash)) {
		return NULL;
	}

	return find(hash);
}

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
	resourcesCleared();
}


void TextureIndexer::resourceAdded(const File& file)
{
	if (file.guessContentType() == CONTENT_TYPE_TXD) {
		const char* fname = file.getPath()->getFileName();
		char* txdName = new char[strlen(fname)+1];
		strtolower(txdName, fname);
		*strchr(txdName, '.') = '\0';

		char texName[33];

		hash_t txdHash = Hash(txdName);

		TXDArchive txd(file);

		TextureArchive* archive = new TextureArchive(txdHash, file);

		for (int16_t i = 0 ; i < txd.getTextureCount() ; i++) {
			TXDTexture* tex = txd.nextTexture();

			strtolower(texName, tex->getDiffuseName());
			archive->addTexture(Hash(texName));

			delete tex;
		}

		archives.insert(pair<hash_t, TextureArchive*>(txdHash, archive));

		delete[] txdName;
	}
}


void TextureIndexer::resourcesCleared()
{
	ArchiveMap::iterator ait;

	for (ait = archives.begin() ; ait != archives.end() ; ait++) {
		delete ait->second;
	}

	archives.clear();
}


TextureArchive* TextureIndexer::findArchive(hash_t name)
{
	ArchiveMap::iterator it = archives.find(name);

	if (it == archives.end()) {
		return NULL;
	}

	return it->second;
}

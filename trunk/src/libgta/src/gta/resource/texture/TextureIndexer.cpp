/*
	Copyright 2010-2011 David "Alemarius Nexus" Lerch

	This file is part of libgta.

	libgta is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	libgta is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with libgta.  If not, see <http://www.gnu.org/licenses/>.

	Additional permissions are granted, which are listed in the file
	GPLADDITIONS.
 */

#include "TextureIndexer.h"
#include <gtaformats/txd/TXDArchive.h>
#include <gtaformats/txd/TXDTextureHeader.h>
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
			TXDTextureHeader* tex = txd.nextTexture();

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

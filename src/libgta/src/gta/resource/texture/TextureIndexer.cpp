/*
	Copyright 2010-2014 David "Alemarius Nexus" Lerch

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
#include <nxcommon/strutil.h>
#include <nxcommon/log.h>
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
		CString fname = file.getPath().getFileName();
		char* txdName = new char[fname.length() + 1];
		strtolower(txdName, fname.get());
		*strchr(txdName, '.') = '\0';

		char texName[33];

		CString lTxdName = CString::from(txdName);

		//hash_t txdHash = Hash(txdName);

		/*if (archives.find(txdName) != archives.end()) {
			char* oldPath = dbgArchivePaths.find(txdName)->second;
			LogWarning("Conflicting resources: A TXD archive with the same name as %s was "
					"already added! Previous resource: %s", file.getPath().toString().get(), oldPath);
		}*/

		TextureArchive* archive = new TextureArchive(lTxdName, file);

		auto res = archives.insert(pair<CString, TextureArchive*>(lTxdName, archive));

		if (res.second) {
#ifndef NDEBUG
			char* path = new char[file.getPath().toString().length() + 1];
			strcpy(path, file.getPath().toString().get());
			dbgArchivePaths.insert(pair<CString, char*>(lTxdName, path));
#endif
		} else {
			char* oldPath = dbgArchivePaths.find(txdName)->second;
			LogWarning("Conflicting resources: A TXD archive with the same name as %s was "
					"already added! Previous resource: %s", file.getPath().toString().get(), oldPath);

			delete res.first->second;
			res.first->second = archive;
		}
	}
}


void TextureIndexer::resourcesCleared()
{
	ArchiveMap::iterator ait;

	for (ait = archives.begin() ; ait != archives.end() ; ait++) {
		delete ait->second;
	}

	archives.clear();

#ifndef NDEBUG
	map<CString, char*>::iterator it;

	for (it = dbgArchivePaths.begin() ; it != dbgArchivePaths.end() ; it++) {
		delete[] it->second;
	}

	dbgArchivePaths.clear();
#endif
}


TextureArchive* TextureIndexer::findArchive(const CString& name)
{
	ArchiveMap::iterator it = archives.find(name);

	if (it == archives.end()) {
		return NULL;
	}

	return it->second;
}

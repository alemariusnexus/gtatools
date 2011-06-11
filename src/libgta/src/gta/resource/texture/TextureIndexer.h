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

#ifndef TEXTUREINDEXER_H_
#define TEXTUREINDEXER_H_

#include <gta/config.h>
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
	static TextureIndexer* getInstance()
	{
		static TextureIndexer inst;
		return &inst;
	}

public:
	~TextureIndexer();
	virtual void resourceAdded(const File& file);
	virtual void resourcesCleared();
	TextureArchive* findArchive(hash_t name);

private:
	TextureIndexer() {}

private:
	ArchiveMap archives;

#ifndef NDEBUG
	map<hash_t, char*> dbgArchivePaths;
#endif
};

#endif /* TEXTUREINDEXER_H_ */

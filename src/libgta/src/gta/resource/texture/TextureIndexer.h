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

#ifndef TEXTUREINDEXER_H_
#define TEXTUREINDEXER_H_

#include <gta/config.h>
#include <nxcommon/cxx11hash.h>
#include "../ResourceObserver.h"
#include "TextureArchive.h"
#include "../../Engine.h"
#include <unordered_map>

using std::unordered_map;


struct TextureIndexEntry
{
	File* file;
	int16_t index;
};


class TextureIndexer : public ResourceObserver {
private:
	typedef unordered_map<CString, TextureArchive*, CXX11Hash<CString> > ArchiveMap;

public:
	TextureIndexer() {}
	virtual ~TextureIndexer();
	virtual void resourceAdded(const File& file);
	virtual void resourcesCleared();
	TextureArchive* findArchive(const CString& name);

private:

private:
	ArchiveMap archives;

#ifndef NDEBUG
	map<CString, char*> dbgArchivePaths;
#endif
};

#endif /* TEXTUREINDEXER_H_ */

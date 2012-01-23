/*
	Copyright 2010-2012 David "Alemarius Nexus" Lerch

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

#include "CollisionMeshIndexer.h"
#include <gtaformats/gtacol.h>
#include <gtaformats/util/strutil.h>
#include <utility>

using std::pair;



void CollisionMeshIndexer::resourceAdded(const File& file)
{
	if (file.guessContentType() == CONTENT_TYPE_COL) {
		COLLoader col;
		istream* stream = file.openInputStream(istream::binary);
		char name[20];
		int i = 0;

		while (col.loadModelName(stream, name)) {
			CollisionMeshIndexEntry* entry = new CollisionMeshIndexEntry;
			entry->file = new File(file);
			entry->index = i++;
			index.insert(pair<CString, CollisionMeshIndexEntry*>(CString(name).lower(), entry));
		}

		delete stream;
	}
}


CollisionMeshIndexer::CollisionMeshIndexEntry* CollisionMeshIndexer::getCollisionMesh(CString name)
{
	IndexMap::iterator it = index.find(name);

	if (it == index.end()) {
		return NULL;
	}

	return it->second;
}
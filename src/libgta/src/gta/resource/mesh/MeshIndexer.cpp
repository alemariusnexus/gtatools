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

#include "MeshIndexer.h"
#include <cstring>
#include <gtaformats/util/strutil.h>
#include <utility>

using std::pair;



MeshIndexer::~MeshIndexer()
{
	IndexMap::iterator it;

	for (it = index.begin() ; it != index.end() ; it++) {
		delete it->second;
	}

	index.clear();
}


void MeshIndexer::resourceAdded(const File& file)
{
	if (file.guessContentType() == CONTENT_TYPE_DFF) {
		const char* fname = file.getPath()->getFileName();
		char* lMeshName = new char[strlen(fname)+1];
		strtolower(lMeshName, fname);
		*strrchr(lMeshName, '.') = '\0';
		index.insert(pair<CString, File*>(CString::from(lMeshName), new File(file)));
	}
}


const File* MeshIndexer::find(const CString& name)
{
	IndexMap::iterator it = index.find(name);

	if (it == index.end()) {
		return NULL;
	}

	return it->second;
}

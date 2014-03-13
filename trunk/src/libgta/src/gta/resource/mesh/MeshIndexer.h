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

#ifndef MESHINDEXER_H_
#define MESHINDEXER_H_

#include <gta/config.h>
#include <nxcommon/file/File.h>
#include <nxcommon/cxx11hash.h>
#include <nxcommon/CString.h>
#include "../../Engine.h"
#include "../ResourceObserver.h"
#include <unordered_map>

using std::unordered_map;


class MeshIndexer : public ResourceObserver {
public:
	struct IndexEntry
	{
		File* file;
	};

private:
	typedef unordered_map<CString, IndexEntry, CXX11Hash<CString> > IndexMap;

public:
	~MeshIndexer();
	virtual void resourceAdded(const File& file);
	virtual void resourcesCleared();
	IndexEntry find(const CString& name);
	IndexEntry operator[](const CString& name) { return find(name); }

private:
	IndexMap index;
};

#endif /* MESHINDEXER_H_ */

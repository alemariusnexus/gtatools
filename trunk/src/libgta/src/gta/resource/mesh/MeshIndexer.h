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

#ifndef MESHINDEXER_H_
#define MESHINDEXER_H_

#include <gta/config.h>
#include <gtaformats/util/File.h>
#include "../../Engine.h"
#include "../ResourceObserver.h"

#ifdef CXX0X_AVAILABLE
#include <unordered_map>
using std::unordered_map;
#else
#include <map>
using std::map;
#endif


class MeshIndexer : public ResourceObserver {
private:
#ifdef CXX0X_AVAILABLE
	typedef unordered_map<hash_t, File*> IndexMap;
#else
	typedef map<hash_t, File*> IndexMap;
#endif

public:
	~MeshIndexer();
	virtual void resourceAdded(const File& file);
	const File* find(hash_t name);
	const File* operator[](hash_t name) { return find(name); }

private:
	IndexMap index;
};

#endif /* MESHINDEXER_H_ */

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

#ifndef MESHINDEXER_H_
#define MESHINDEXER_H_

#include <gta/config.h>
#include <gtaformats/util/File.h>
#include <gtaformats/util/cxx0xhash.h>
#include <gtaformats/util/CString.h>
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
	typedef unordered_map<CString, File*, CXX0XHash<CString> > IndexMap;
#else
	typedef map<CString, File*> IndexMap;
#endif

public:
	~MeshIndexer();
	virtual void resourceAdded(const File& file);
	const File* find(const CString& name);
	const File* operator[](const CString& name) { return find(name); }

private:
	IndexMap index;
};

#endif /* MESHINDEXER_H_ */

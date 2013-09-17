/*
	Copyright 2010-2013 David "Alemarius Nexus" Lerch

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

#include "AnimationIndexer.h"
#include <utility>

using std::pair;




AnimationIndexer::~AnimationIndexer()
{
	for (AnimMap::iterator it = index.begin() ; it != index.end() ; it++) {
		delete it->second;
	}
}


void AnimationIndexer::resourceAdded(const File& file)
{
	if (file.guessContentType() == CONTENT_TYPE_IFP) {
		CString fname = file.getPath()->getFileName();
		size_t len = fname.length();
		CString lname(fname.get(), len-4);
		lname.lower();
		index.insert(pair<CString, File*>(lname, new File(file)));
	}
}


void AnimationIndexer::resourcesCleared()
{
	for (AnimMap::iterator it = index.begin() ; it != index.end() ; it++) {
		delete it->second;
	}

	index.clear();
}


File* AnimationIndexer::find(CString name)
{
	AnimMap::iterator it = index.find(name);

	if (it == index.end())
		return NULL;

	return it->second;
}

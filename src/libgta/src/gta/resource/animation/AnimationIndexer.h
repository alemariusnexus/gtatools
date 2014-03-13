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

#ifndef ANIMATIONINDEXER_H_
#define ANIMATIONINDEXER_H_

#include "../ResourceObserver.h"
#include <map>
#include <nxcommon/strutil.h>
#include <nxcommon/CString.h>
#include <nxcommon/file/File.h>

using std::map;



class AnimationIndexer : public ResourceObserver {
private:
	typedef map<CString, File*> AnimMap;

public:
	~AnimationIndexer();
	virtual void resourceAdded(const File& file);
	virtual void resourcesCleared();
	File* find(CString name);
	File* operator[](CString name) { return find(name); }

private:
	AnimMap index;
};

#endif /* ANIMATIONINDEXER_H_ */

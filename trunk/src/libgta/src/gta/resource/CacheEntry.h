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

#ifndef CACHEENTRY_H_
#define CACHEENTRY_H_

#include <gta/config.h>
#include <cstdlib>


class ResourceCache;
class SharedCachePointer;


typedef unsigned int cachesize_t;


class CacheEntry {
public:
	CacheEntry() : sharedPtr(NULL) {}
	virtual ~CacheEntry();
	virtual cachesize_t getSize() const = 0;

private:
	void registerPointer(SharedCachePointer* pointer) { sharedPtr = pointer; }
	void unregisterPointer() { sharedPtr = NULL; }

private:
	SharedCachePointer* sharedPtr;

	friend class ResourceCache;
	friend class SharedCachePointer;
};

#endif /* CACHEENTRY_H_ */

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

#include "SharedCachePointer.h"
#include "ResourceCache.h"



SharedCachePointer::~SharedCachePointer()
{
	cache->removeSharedPointer(key);

	if (entry)
		entry->unregisterPointer();
}


CacheEntry* SharedCachePointer::getEntry()
{
	if (entry)
		return entry;

	entry = cache->getEntry(key);

	if (entry) {
		entry->registerPointer(this);
	}

	return entry;
}


void SharedCachePointer::release()
{

}


void SharedCachePointer::entryDeleted()
{
	entry = NULL;
}

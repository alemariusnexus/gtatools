/*
	Copyright 2010-2013 David "Alemarius Nexus" Lerch

	This file is part of gtaformats.

	gtaformats is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	gtaformats is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with gtaformats.  If not, see <http://www.gnu.org/licenses/>.

	Additional permissions are granted, which are listed in the file
	GPLADDITIONS.
 */

#include "rwbs.h"
#include <cstring>
#include <cstdio>



bool RWGetSectionInfo(int32_t id, RWSectionInfo* info)
{
	for (unsigned int i = 0 ; i < RW_NUM_KNOWN_SECTIONS ; i++) {
		RWSectionInfo tmpInfo = RWKnownSections[i];

		if (tmpInfo.id == id) {
			*info = tmpInfo;
			return true;
		}
	}

	return false;
}


bool RWGetSectionName(int32_t id, char* dest)
{
	const char* prefix = "RW_SECTION_";

	if (RWGetSectionShortName(id, dest+strlen(prefix))) {
		memcpy(dest, prefix, strlen(prefix));
		return true;
	} else {
		memmove(dest, dest+strlen(prefix), strlen(dest+strlen(prefix))+1);
		return false;
	}
}

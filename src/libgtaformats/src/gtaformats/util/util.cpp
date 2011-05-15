/*
	Copyright 2010-2011 David "Alemarius Nexus" Lerch

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

#include "util.h"
#include <cctype>
#include <cstring>
#include <cstdlib>

#ifdef _POSIX_VERSION
#include <sys/time.h>
#else
#include <windows.h>
#endif


uint64_t GetTickcount()
{
#ifdef _POSIX_VERSION
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec*1000 + tv.tv_usec/1000;
#else
	return GetTickCount();
#endif
}


float RandomFloat(float min, float max)
{
	return (rand() / (float) RAND_MAX) * (max-min) + min;
}


bool RandomBool()
{
	return rand() <= RAND_MAX/2;
}

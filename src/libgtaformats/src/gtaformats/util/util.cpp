/*
	Copyright 2010-2012 David "Alemarius Nexus" Lerch

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
#include <unistd.h>
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


bool IsTimeBetween(int8_t timeH, int8_t timeM, int8_t startH, int8_t startM, int8_t endH, int8_t endM)
{
	int16_t timeAbsM = timeH*60 + timeM;
	int16_t startAbsM = startH*60 + startM;
	int16_t endAbsM = endH*60 + endM;

	if (endAbsM >= startAbsM) {
		return timeAbsM >= startAbsM  &&  timeAbsM < endAbsM;
	} else {
		return timeAbsM >= startAbsM  ||  timeAbsM < endAbsM;
	}
}


void SleepMilliseconds(unsigned int time)
{
#ifdef _POSIX_VERSION
	usleep(time * 1000);
#else
	Sleep(time);
#endif
}

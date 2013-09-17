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

#ifndef MUTEX_H_
#define MUTEX_H_

#include <gtaformats/config.h>

#ifdef _POSIX_VERSION
#include <pthread.h>
#else
#include <windows.h>
#endif



class Mutex {
public:
	Mutex();
	void lock();
	void unlock();

private:
#ifdef _POSIX_VERSION
	pthread_mutex_t posixMutex;
#else
	CRITICAL_SECTION winCriticalSection;
#endif
};

#endif /* MUTEX_H_ */

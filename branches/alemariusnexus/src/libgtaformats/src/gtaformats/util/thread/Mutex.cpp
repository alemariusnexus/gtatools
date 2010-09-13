/*
 * Mutex.cpp
 *
 *  Created on: 25.07.2010
 *      Author: alemariusnexus
 */

#include "Mutex.h"


Mutex::Mutex()
{
#ifdef linux
	pthread_mutex_init(&posixMutex, NULL);
#else
	InitializeCriticalSection(&winCriticalSection);
#endif
}


void Mutex::lock()
{
#ifdef linux
	pthread_mutex_lock(&posixMutex);
#else
	EnterCriticalSection(&winCriticalSection);
#endif
}


void Mutex::unlock()
{
#ifdef linux
	pthread_mutex_unlock(&posixMutex);
#else
	LeaveCriticalSection(&winCriticalSection);
#endif
}


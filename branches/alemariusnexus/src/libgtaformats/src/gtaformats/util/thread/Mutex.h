/*
 * Mutex.h
 *
 *  Created on: 25.07.2010
 *      Author: alemariusnexus
 */

#ifndef MUTEX_H_
#define MUTEX_H_

#ifdef linux
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
#ifdef linux
	pthread_mutex_t posixMutex;
#else
	CRITICAL_SECTION winCriticalSection;
#endif
};

#endif /* MUTEX_H_ */

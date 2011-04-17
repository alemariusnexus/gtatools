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
 */

#include "Thread.h"
#include <cstdio>
#include <map>
#include <utility>
#include "MainThread.h"

using std::map;
using std::pair;



MainThread mainThread;
map<unsigned long, Thread*> threads;


struct ThreadContainer
{
	Thread* thread;
};




#ifdef _POSIX_VERSION
void* __posixThreadStarter(void* param)
{
	ThreadContainer* tc = (ThreadContainer*) param;
	Thread* thread = tc->thread;
	thread->run();
	thread->terminated();
	delete tc;
	return NULL;
}
#else
DWORD WINAPI __winThreadStarter(LPVOID param)
{
	Thread* thread = (Thread*) param;
	thread->run();
	thread->terminated();
    return 0;
}
#endif




Thread::Thread(bool deleteOnTermination)
		: deleteOnTermination(deleteOnTermination), running(false), parentThread(currentThread()),
		  terminationHandler(NULL)
#ifdef _POSIX_VERSION
#else
		  , winHandle(NULL)
#endif
{
	setPriority(Inherit);
}


#ifdef _POSIX_VERSION
Thread::Thread(pthread_t posixThread)
		: deleteOnTermination(false), parentThread(NULL), terminationHandler(NULL), posixThread(posixThread)
{
}
#else
Thread::Thread(HANDLE winHandle)
		: winHandle(winHandle), parentThread(NULL), deleteOnTermination(false), terminationHandler(NULL)
{
}
#endif


Thread::~Thread()
{
#ifdef _POSIX_VERSION
#else
	CloseHandle(winHandle);
#endif
}


Thread* Thread::currentThread()
{
	unsigned long threadId;

#ifdef _POSIX_VERSION
	threadId = pthread_self();
#else
	threadId = GetCurrentThreadId();
#endif

	map<unsigned long, Thread*>::iterator it = threads.find(threadId);

	if (it == threads.end()) {
		threads.insert(pair<unsigned long, Thread*>(threadId, &mainThread));
		return &mainThread;
	} else {
		return it->second;
	}
}


void Thread::start()
{
	unsigned long id;

	ThreadContainer* tc = new ThreadContainer;
	tc->thread = this;

#ifdef _POSIX_VERSION
	pthread_create(&posixThread, NULL, __posixThreadStarter, tc);
	running = true;
	id = (unsigned long) posixThread;
#else
	winHandle = CreateThread(NULL, 0, __winThreadStarter, tc, 0, &id);
	running = true;

	int winPriority;

	switch (priority) {
	case Idle:
		winPriority = THREAD_PRIORITY_IDLE;
		break;
	case Lowest:
		winPriority = THREAD_PRIORITY_LOWEST;
		break;
	case BelowNormal:
		winPriority = THREAD_PRIORITY_BELOW_NORMAL;
		break;
	case Normal:
		winPriority = THREAD_PRIORITY_NORMAL;
		break;
	case AboveNormal:
		winPriority = THREAD_PRIORITY_ABOVE_NORMAL;
		break;
	case Highest:
		winPriority = THREAD_PRIORITY_HIGHEST;
		break;
	case TimeCritical:
		winPriority = THREAD_PRIORITY_TIME_CRITICAL;
		break;
	}

	SetThreadPriority(winHandle, winPriority);
#endif

	threads.insert(pair<unsigned int, Thread*>(id, this));
}


void Thread::terminated()
{
	running = false;

	if (terminationHandler != NULL) {
		terminationHandler();
	}
}


void Thread::join()
{
#ifdef _POSIX_VERSION
	pthread_join(posixThread, NULL);
#else
	WaitForSingleObject(winHandle, INFINITE);
#endif
}


void Thread::setPriority(Priority priority)
{
	if (priority == Inherit) {
		Thread* cur = currentThread();
		priority = cur->getPriority();
	}

	this->priority = priority;
}


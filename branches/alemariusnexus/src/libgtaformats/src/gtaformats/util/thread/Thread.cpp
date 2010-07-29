/*
 * Thread.cpp
 *
 *  Created on: 25.07.2010
 *      Author: alemariusnexus
 */

#include "Thread.h"
#include <cstdio>
#include <map>
#include <utility>
#include "MainThread.h"

using std::map;
using std::pair;



map<unsigned long, Thread*> threads;




#ifdef linux
void* __posixThreadStarter(void* param)
{
	Thread* thread = (Thread*) param;
	thread->run();
	thread->terminated();
}
#else
DWORD WINAPI __winThreadStarter(LPVOID param)
{
	Thread* thread = (Thread*) param;
	thread->run();
	thread->terminated();
}
#endif




Thread::Thread()
		: parentThread(currentThread())
#ifdef linux
#else
		  , winHandle(NULL)
#endif
{
	setPriority(Inherit);
}


#ifdef linux
Thread::Thread(pthread_t posixThread)
		: posixThread(posixThread), parentThread(NULL)
{
}
#else
Thread::Thread(HANDLE winHandle)
		: winHandle(winHandle)
{
}
#endif


Thread::~Thread()
{
#ifdef linux
#else
	CloseHandle(winHandle);
#endif
}


Thread* Thread::currentThread()
{
	unsigned long threadId;

#ifdef linux
	threadId = pthread_self();
#else
	threadId = GetCurrentThreadId();
#endif

	map<unsigned long, Thread*>::iterator it = threads.find(threadId);

	if (it == threads.end()) {
		Thread* mainThread = new MainThread();
		threads.insert(pair<unsigned long, Thread*>(threadId, mainThread));
		return mainThread;
	} else {
		return it->second;
	}
}


void Thread::start()
{
	unsigned long id;

#ifdef linux
	pthread_create(&posixThread, NULL, __posixThreadStarter, (void*) this);
	running = true;
	id = (unsigned long) posixThread;
#else
	winHandle = CreateThread(NULL, 0, __winThreadStarter, (void*) this, 0, &id);
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
}


void Thread::join()
{
#ifdef linux
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
		delete cur;
	}

	this->priority = priority;
}


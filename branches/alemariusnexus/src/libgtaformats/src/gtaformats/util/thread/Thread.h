/*
 * Thread.h
 *
 *  Created on: 25.07.2010
 *      Author: alemariusnexus
 */

#ifndef THREAD_H_
#define THREAD_H_


#ifdef linux
#include <pthread.h>
#else
#include <windows.h>
#endif

#define GTHREADS_MAX 2048


class MainThread;


class Thread {
public:
	enum Priority
	{
		Idle,
		Lowest,
		BelowNormal,
		Normal,
		AboveNormal,
		Highest,
		TimeCritical,
		Inherit
	};

public:
	static Thread* currentThread();

public:
	Thread();
	~Thread();
	void start();
	void join();
	void setPriority(Priority priority);
	Priority getPriority() { return priority; }

protected:
	virtual void run() = 0;

private:
#ifdef linux
	Thread(pthread_t posixThread);
#else
	Thread(HANDLE winHandle);
#endif
	void terminated();

private:
	bool running;
	Priority priority;
	Thread* parentThread;
#ifdef linux
	pthread_t posixThread;
#else
	HANDLE winHandle;
#endif

private:
	friend class MainThread;
#ifdef linux
	friend void* __posixThreadStarter(void* param);
#else
	friend DWORD WINAPI __winThreadStarter(LPVOID param);
#endif
};

#endif /* THREAD_H_ */

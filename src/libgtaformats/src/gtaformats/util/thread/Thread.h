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

#ifndef THREAD_H_
#define THREAD_H_

#include <gtaformats/config.h>

#ifdef _POSIX_VERSION
#include <pthread.h>
#else
#include <windows.h>
#endif

#include <cstdio>

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

#ifdef _POSIX_VERSION
	typedef pthread_t threadid_t;
#else
	typedef DWORD threadid_t;
#endif

public:
	static Thread* currentThread();

public:
	Thread(bool deleteOnTermination = false);
	~Thread();
	void start();
	void join();
	void setPriority(Priority priority);
	Priority getPriority() { return priority; }
	void setTerminationHandler(void (*terminationHandler)())
	{
		this->terminationHandler = terminationHandler;
	}

protected:
	virtual void run() = 0;

	void blubb(void* test);

private:
#ifdef _POSIX_VERSION
	Thread(pthread_t posixThread);
#else
	Thread(HANDLE winHandle);
#endif
	void terminated();

private:
	bool deleteOnTermination;
	bool running;
	Priority priority;
	Thread* parentThread;
	void (*terminationHandler)();
#ifdef _POSIX_VERSION
	pthread_t posixThread;
#else
	HANDLE winHandle;
#endif

private:
	friend class MainThread;
#ifdef _POSIX_VERSION
	friend void* __posixThreadStarter(void* param);
#else
	friend DWORD WINAPI __winThreadStarter(LPVOID param);
#endif
};

#endif /* THREAD_H_ */

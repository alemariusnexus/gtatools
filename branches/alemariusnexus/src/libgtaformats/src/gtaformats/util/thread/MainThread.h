/*
 * MainThread.h
 *
 *  Created on: 25.07.2010
 *      Author: alemariusnexus
 */

#ifndef MAINTHREAD_H_
#define MAINTHREAD_H_

#include "Thread.h"



class MainThread : public Thread {
protected:
	virtual void run() {}

private:
	MainThread();

private:
	friend class Thread;
};

#endif /* MAINTHREAD_H_ */

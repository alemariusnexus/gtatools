/*
 * MainThread.cpp
 *
 *  Created on: 25.07.2010
 *      Author: alemariusnexus
 */

#include "MainThread.h"



MainThread::MainThread()
#ifdef linux
		: Thread(pthread_self())
#else
		: Thread(GetCurrentThread())
#endif
{
}

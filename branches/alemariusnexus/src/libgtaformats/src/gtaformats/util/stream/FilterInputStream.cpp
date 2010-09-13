/*
 * FilterInputStream.cpp
 *
 *  Created on: 22.08.2010
 *      Author: alemariusnexus
 */

#include "FilterInputStream.h"


FilterInputStream::FilterInputStream(InputStream* backend, bool deleteBackend)
		: backend(backend), deleteBackend(deleteBackend)
{
}


FilterInputStream::~FilterInputStream()
{
	if (deleteBackend) {
		delete backend;
	}
}


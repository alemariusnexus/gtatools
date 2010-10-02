/*
 * OutOfBoundsException.cpp
 *
 *  Created on: 02.10.2010
 *      Author: alemariusnexus
 */

#include "OutOfBoundsException.h"
#include <cstdio>



OutOfBoundsException::OutOfBoundsException(int index, const char* srcFile, int srcLine,
			Exception* nestedException)
		: Exception(NULL, srcFile, srcLine, nestedException, "OutOfBoundsException")
{
	char* message = new char[64];
	sprintf(message, "Index %d out of bounds", index);
	setMessage(message);
	delete[] message;
}

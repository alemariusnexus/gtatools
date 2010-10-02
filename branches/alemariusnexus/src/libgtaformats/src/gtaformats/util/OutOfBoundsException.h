/*
 * OutOfBoundsException.h
 *
 *  Created on: 02.10.2010
 *      Author: alemariusnexus
 */

#ifndef OUTOFBOUNDSEXCEPTION_H_
#define OUTOFBOUNDSEXCEPTION_H_

#include "Exception.h"



class OutOfBoundsException : public Exception {
public:
	OutOfBoundsException(int index, const char* srcFile = NULL, int srcLine = -1,
			Exception* nestedException = NULL);
};

#endif /* OUTOFBOUNDSEXCEPTION_H_ */

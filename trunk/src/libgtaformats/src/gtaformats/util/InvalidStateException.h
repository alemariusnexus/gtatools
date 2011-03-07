/*
 * InvalidStateException.h
 *
 *  Created on: 06.03.2011
 *      Author: alemariusnexus
 */

#ifndef INVALIDSTATEEXCEPTION_H_
#define INVALIDSTATEEXCEPTION_H_

#include "Exception.h"


class InvalidStateException : public Exception {
public:
	InvalidStateException(const char* message, const char* srcFile = NULL, int srcLine = -1,
			Exception* nestedException = NULL)
			: Exception(message, srcFile, srcLine, nestedException, "InvalidStateException") {}
};

#endif /* INVALIDSTATEEXCEPTION_H_ */

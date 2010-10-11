/*
 * IDEException.h
 *
 *  Created on: 11.10.2010
 *      Author: alemariusnexus
 */

#ifndef IDEEXCEPTION_H_
#define IDEEXCEPTION_H_

#include "../util/Exception.h"



class IDEException : public Exception {
public:
	IDEException(const char* message, const char* srcFile = NULL, int srcLine = -1,
			Exception* nestedException = NULL)
			: Exception(message, srcFile, srcLine, nestedException, "IDEException") {}
};

#endif /* IDEEXCEPTION_H_ */

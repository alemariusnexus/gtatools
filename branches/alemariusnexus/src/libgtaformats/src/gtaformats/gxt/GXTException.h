/*
 * GXTException.h
 *
 *  Created on: 18.09.2010
 *      Author: alemariusnexus
 */

#ifndef GXTEXCEPTION_H_
#define GXTEXCEPTION_H_

#include "../util/Exception.h"


class GXTException : public Exception {
public:
	GXTException(const char* message, const char* srcFile = NULL, int srcLine = -1,
			Exception* nestedException = NULL)
	: Exception(message, srcFile, srcLine, nestedException, "GXTException") {}
};

#endif /* GXTEXCEPTION_H_ */

/*
 * COLException.h
 *
 *  Created on: 31.10.2010
 *      Author: alemariusnexus
 */

#ifndef COLEXCEPTION_H_
#define COLEXCEPTION_H_

#include "../util/Exception.h"


class COLException : public Exception {
public:
	COLException(const char* message, const char* srcFile = NULL, int srcLine = -1,
			Exception* nestedException = NULL)
			: Exception(message, srcFile, srcLine, nestedException, "COLException") {}
};

#endif /* COLEXCEPTION_H_ */

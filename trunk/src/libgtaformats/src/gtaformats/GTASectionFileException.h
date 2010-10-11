/*
 * GTASectionFileException.h
 *
 *  Created on: 11.10.2010
 *      Author: alemariusnexus
 */

#ifndef GTASECTIONFILEEXCEPTION_H_
#define GTASECTIONFILEEXCEPTION_H_

#include "util/Exception.h"



class GTASectionFileException : public Exception {
public:
	GTASectionFileException(const char* message, const char* srcFile = NULL, int srcLine = -1,
			Exception* nestedException = NULL)
			: Exception(message, srcFile, srcLine, nestedException, "GTASectionFileException") {}
};

#endif /* GTASECTIONFILEEXCEPTION_H_ */

/*
 * EngineException.h
 *
 *  Created on: 23.10.2010
 *      Author: alemariusnexus
 */

#ifndef ENGINEEXCEPTION_H_
#define ENGINEEXCEPTION_H_

#include <gtaformats/util/Exception.h>


class EngineException : public Exception {
public:
	EngineException(const char* message, const char* srcFile = NULL, int srcLine = -1,
			Exception* nestedException = NULL)
			: Exception(message, srcFile, srcLine, nestedException, "EngineException") {}
};

#endif /* ENGINEEXCEPTION_H_ */

/*
 * GLException.h
 *
 *  Created on: 23.09.2010
 *      Author: alemariusnexus
 */

#ifndef GLEXCEPTION_H_
#define GLEXCEPTION_H_

#include <gtaformats/util/Exception.h>



class GLException : public Exception {
public:
	static void checkError(const char* msg = NULL);

public:
	GLException(const char* message, const char* srcFile = NULL, int srcLine = -1,
			Exception* nestedException = NULL)
			: Exception(message, srcFile, srcLine, nestedException, "GLException") {}
};

#endif /* GLEXCEPTION_H_ */

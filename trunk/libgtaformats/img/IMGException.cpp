/*
 * IMGException.cpp
 *
 *  Created on: 29.01.2010
 *      Author: alemariusnexus
 */

#include "IMGException.h"
#include <cstring>

IMGException::IMGException(IMGException::ErrorCode errCode, const char* msg) throw()
		: errCode(errCode)
{
	message = new char[strlen(msg)+1];
	strncpy(message, msg, strlen(msg)+1);
}


IMGException::~IMGException() throw()
{
	delete[] message;
}

/*
 * DFFException.cpp
 *
 *  Created on: 10.02.2010
 *      Author: alemariusnexus
 */

#include "DFFException.h"

#include <cstring>


DFFException::DFFException(ErrorCode code, const char* message) throw()
		: message(NULL), errCode(code)
{
	this->message = new char[strlen(message)+1];
	strcpy(this->message, message);
}


DFFException::~DFFException() throw()
{
	//delete[] message;
}


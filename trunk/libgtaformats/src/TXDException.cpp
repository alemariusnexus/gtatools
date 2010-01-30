/*
 * TXDException.cpp
 *
 *  Created on: 23.01.2010
 *      Author: alemariusnexus
 */

#include "TXDException.h"
#include <cstring>

TXDException::TXDException(TXDException::ErrorCode errCode, const char* msg) throw()
		: errCode(errCode)
{
	message = new char[strlen(msg)+1];
	strncpy(message, msg, strlen(msg)+1);
}

/*
 * TXDException.cpp
 *
 *  Created on: 23.01.2010
 *      Author: alemariusnexus
 */

#include "TXDException.h"
#include <cstring>
#include <cstdio>

TXDException::TXDException(TXDException::ErrorCode errCode, const char* msg, long long offset) throw()
		: errCode(errCode), offset(offset)
{
	message = new char[strlen(msg)+64];
	sprintf(message, "%s (at offset %lld)", msg, offset);
	//strncpy(message, msg, strlen(msg)+1);
}

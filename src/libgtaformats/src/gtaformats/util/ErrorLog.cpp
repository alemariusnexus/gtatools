/*
 * ErrorLog.cpp
 *
 *  Created on: 11.10.2010
 *      Author: alemariusnexus
 */

#include "ErrorLog.h"
#include <cstring>



ErrorLog::~ErrorLog()
{
	while (data.size() != 0) {
		delete[] data.back();
		data.pop();
	}
}


void ErrorLog::log(const char* message)
{
	char* cpy = new char[strlen(message)+1];
	strcpy(cpy, message);
	data.push(cpy);
}


char* ErrorLog::nextMessage()
{
	if (data.size() == 0) {
		return NULL;
	}

	char* msg = data.back();
	data.pop();
	return msg;
}

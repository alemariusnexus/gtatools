/*
 * Exception.cpp
 *
 *  Created on: 15.05.2010
 *      Author: alemariusnexus
 */

#include "Exception.h"
#include <cstring>
#include <cstdio>



Exception::Exception(const char* message, const char* srcFile, int srcLine)
		: message(new char[strlen(message)+1]), srcFile(srcFile), srcLine(srcLine)
{
	strcpy(this->message, message);
}


Exception::Exception(const Exception& ex)
		: message(new char[strlen(ex.message)+1]), srcFile(ex.srcFile), srcLine(ex.srcLine)
{
	strcpy(message, ex.message);
}


Exception::~Exception() throw()
{
	delete[] message;
}


const char* Exception::getFullMessage() const throw()
{
	int len = strlen(message);

	if (srcFile != NULL) {
		len += strlen(srcFile)+3;
	}
	if (srcLine != -1) {
		len += 11 + 1;
	}

	char* formMsg = new char[len+1];

	if (srcFile != NULL) {
		strcat(formMsg, srcFile);

		if (srcLine != -1) {
			char appendix[13];
			sprintf(appendix, ":%d", srcLine);
			strcat(formMsg, appendix);
		}
	}

	strcat(formMsg, " - ");
	strcat(formMsg, message);

	return formMsg;
}


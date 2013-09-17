/*
	Copyright 2010-2013 David "Alemarius Nexus" Lerch

	This file is part of gtaformats.

	gtaformats is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	gtaformats is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with gtaformats.  If not, see <http://www.gnu.org/licenses/>.

	Additional permissions are granted, which are listed in the file
	GPLADDITIONS.
 */

#include "Exception.h"
#include <gtaformats/config.h>
#include <cstring>
#include <cstdio>



Exception::Exception(const char* message, const char* srcFile, int srcLine, const Exception* nestedException,
		const char* exceptionName)
		: exceptionName(exceptionName), message(NULL), srcFile(srcFile), srcLine(srcLine),
		  nestedException(nestedException ? nestedException->copy() : NULL)
{
	setMessage(message);

#ifdef linux
	void* buf[50];
	int backTraceSize = backtrace(buf, 50);
	char** btArr = backtrace_symbols(buf, backTraceSize);

	int len = 1;

	for (int i = 0 ; i < backTraceSize ; i++) {
		len += strlen(btArr[i])+1;
	}

	backTrace = new char[len];
	backTrace[0] = '\0';

	for (int i = 0 ; i < backTraceSize ; i++) {
		strcat(backTrace, btArr[i]);
		strcat(backTrace, "\n");
	}
#endif
}


Exception::Exception(const Exception& ex)
		: exceptionName(ex.exceptionName), message(NULL), srcFile(ex.srcFile), srcLine(ex.srcLine),
		  nestedException(ex.nestedException ? ex.nestedException->copy() : NULL)
{
	setMessage(ex.message);

#ifdef linux
	backTrace = new char[strlen(ex.backTrace)+1];
	memcpy(backTrace, ex.backTrace, strlen(ex.backTrace)+1);
#endif
}


Exception::~Exception() throw()
{
	delete[] message;
	delete[] fullMessage;

	if (nestedException)
		delete nestedException;

#ifdef linux
	delete[] backTrace;
#endif
}


const char* Exception::getBacktrace() const throw()
{
#ifdef linux
	return backTrace;
#else
	return NULL;
#endif
}


void Exception::setMessage(const char* message)
{
	if (this->message) {
		delete[] this->message;
		delete[] this->fullMessage;
	}

	if (message) {
		this->message = new char[strlen(message)+1];
		strcpy(this->message, message);
		fullMessage = buildFullMessage();
	} else {
		message = NULL;
	}
}


char* Exception::buildFullMessage() const throw()
{
	int messageLen = (message ? strlen(message) : 10);
	int len = messageLen + strlen(exceptionName) + 2;

#ifdef EXCEPTION_POSITION_INFO
	if (srcFile != NULL) {
		len += strlen(srcFile)+3;
	}
	if (srcLine != -1) {
		len += 11 + 1;
	}
#endif

	char* formMsg = new char[len+1];

	strcpy(formMsg, exceptionName);
	strcat(formMsg, ": ");

#ifdef EXCEPTION_POSITION_INFO
	if (srcFile != NULL) {
		strcat(formMsg, srcFile);

		if (srcLine != -1) {
			char appendix[13];
			sprintf(appendix, ":%d", srcLine);
			strcat(formMsg, appendix);
		}

		strcat(formMsg, " - ");
	}
#endif

	if (message) {
		strcat(formMsg, message);
	} else {
		strcat(formMsg, "No message");
	}

	if (nestedException) {
		const char* nestedMsg = nestedException->what();

		char* fullMsg = new char[strlen(formMsg) + strlen(nestedMsg) + 32];
		sprintf(fullMsg, "%s\n\tCaused by: %s", formMsg, nestedMsg);

		delete[] formMsg;

		return fullMsg;
	}

	return formMsg;
}


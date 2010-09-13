/*
	Copyright 2010 David "Alemarius Nexus" Lerch

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
 */

#include "Exception.h"
#include <gf_config.h>
#include <cstring>
#include <cstdio>



Exception::Exception(const char* message, const char* srcFile, int srcLine, Exception* nestedException,
		const char* exceptionName)
		: message(new char[strlen(message)+1]), srcFile(srcFile), srcLine(srcLine), nestedException(nestedException),
		  exceptionName(exceptionName)
{
	strcpy(this->message, message);
	fullMessage = buildFullMessage();

#ifdef linux
	void* buf[50];
	int backTraceSize = backtrace(buf, 50);
	char** btArr = backtrace_symbols(buf, backTraceSize);

	int len = 1;

	for (int i = 0 ; i < backTraceSize ; i++) {
		len += strlen(btArr[i])+1;
	}

	backTrace = new char[len];

	for (int i = 0 ; i < backTraceSize ; i++) {
		strcat(backTrace, btArr[i]);
		strcat(backTrace, "\n");
	}
#endif
}


Exception::Exception(const Exception& ex)
		: message(new char[strlen(ex.message)+1]), srcFile(ex.srcFile), srcLine(ex.srcLine), nestedException(ex.nestedException),
		  exceptionName(ex.exceptionName)
{
	strcpy(message, ex.message);
	fullMessage = buildFullMessage();

#ifdef linux
	backTrace = new char[strlen(ex.backTrace)+1];
	memcpy(backTrace, ex.backTrace, strlen(ex.backTrace)+1);
#endif
}


Exception::~Exception() throw()
{
	delete[] message;
	delete[] fullMessage;

#ifdef linux
	delete[] backTrace;
#endif
}


char* Exception::getBacktrace() const throw()
{
#ifdef linux
	return backTrace;
#else
	char* btMsg = new char[64];
	sprintf(btMsg, "[Backtrace can not be received on this platform]");
	return btMsg;
#endif
}


char* Exception::buildFullMessage() const throw()
{
	int len = strlen(message) + strlen(exceptionName) + 2;

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

	strcat(formMsg, message);

	if (nestedException) {
		const char* nestedMsg = nestedException->what();

		char* fullMsg = new char[strlen(formMsg) + strlen(nestedMsg) + 32];
		sprintf(fullMsg, "%s\n\tCaused by: %s", formMsg, nestedMsg);

		delete[] formMsg;

		return fullMsg;
	}

	return formMsg;
}


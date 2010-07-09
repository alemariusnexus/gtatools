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


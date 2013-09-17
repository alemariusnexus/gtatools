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

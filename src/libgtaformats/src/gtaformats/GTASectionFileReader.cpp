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

#include "GTASectionFileReader.h"
#include "GTASectionFileException.h"
#include <cstdio>
#include <cstdlib>

using std::streamoff;



GTASectionFileReader::GTASectionFileReader(istream* stream, bool deleteStream)
		: stream(stream), errorBehavior(Continue), deleteStream(deleteStream), lastReadLine(0), paramNo(0)
{
}


GTASectionFileReader::GTASectionFileReader(const File& file)
		: stream(file.openInputStream()), errorBehavior(Continue), deleteStream(true), lastReadLine(0),
		  paramNo(0)
{
	/*char* msg = new char[10];
	strcpy(msg, "");
	char* tmsg = trim(msg);
	printf("%s\n", tmsg);
	exit(0);*/
}


GTASectionFileReader::~GTASectionFileReader()
{
	if (deleteStream) {
		delete stream;
	}
}


bool GTASectionFileReader::readNextLine(char* buf, int len)
{
	while (true) {
		stream->getline(buf, len);
		streamoff numChars = stream->gcount();

		if (numChars >= 2  &&  buf[numChars-2] == '\r') {
			buf[numChars-2] = '\0';
		}

		lastReadLine++;
		paramNo = 0;

		if (stream->eof()) {
			return false;
		}

		/*if (buf[strlen(buf)-1] == '\r') {
			buf[strlen(buf)-1] = '\0';
		}*/

		char* cmtStart = strchr(buf, '#');

		if (cmtStart) {
			cmtStart[0] = '\0';
		}

		if (strlen(trim(buf)) == 0) {
			continue;
		}

		return true;
	}
}


char* GTASectionFileReader::trim(char* str)
{
	if (str == NULL) {
		return NULL;
	}

	char* strEnd = str + strlen(str);
	char* newStr = str;

	while (newStr != strEnd  &&  (*newStr == ' '  ||  *newStr == '\t')) newStr++;

	while (strEnd != newStr  &&  (*strEnd == ' '  ||  *strEnd == '\t'  ||  *strEnd == '\0')) *strEnd-- = '\0';

	return newStr;
}


char* GTASectionFileReader::nextString(bool* ok, char* str)
{
	if (!*ok) {
		return NULL;
	}

	char* cstr = strtok(str, ",");

	if (!cstr) {
		*ok = false;
		char* errmsg = new char[128];
		sprintf(errmsg, "Syntax error: Missing parameter no. %d at line %d", paramNo+1, lastReadLine);
		log.log(errmsg);

		if (errorBehavior == Continue) {
			delete[] errmsg;
			return NULL;
		} else {
			GTASectionFileException ex(errmsg, __FILE__, __LINE__);
			delete[] errmsg;
			throw ex;
		}
	}

	paramNo++;
	*ok = true;

	return trim(cstr);
}


int32_t GTASectionFileReader::nextInt(bool* ok, char* str)
{
	char* cstr = nextString(ok, str);

	if (!cstr) {
		*ok = false;
		return 0;
	}

	int32_t val;
	int numScanned = sscanf(cstr, "%d", &val);

	if (numScanned == 0) {
		*ok = false;
		char* errmsg = new char[128];
		sprintf(errmsg, "Syntax error: Parameter no. %d is not an integer at line %d",
				paramNo, lastReadLine);
		log.log(errmsg);

		if (errorBehavior == Continue) {
			delete[] errmsg;
			return 0;
		} else {
			GTASectionFileException ex(errmsg, __FILE__, __LINE__);
			delete[] errmsg;
			throw ex;
		}
	} else {
		*ok = true;
		return val;
	}
}


int32_t GTASectionFileReader::nextHexInt(bool* ok, char* str)
{
	char* cstr = nextString(ok, str);

	if (!cstr) {
		*ok = false;
		return 0;
	}

	int32_t val;
	int numScanned = sscanf(cstr, "%x", &val);

	if (numScanned == 0) {
		*ok = false;
		char* errmsg = new char[128];
		sprintf(errmsg, "Syntax error: Parameter no. %d is not a hexadecimal integer at line %d",
				paramNo, lastReadLine);
		log.log(errmsg);

		if (errorBehavior == Continue) {
			delete[] errmsg;
			return 0;
		} else {
			GTASectionFileException ex(errmsg, __FILE__, __LINE__);
			delete[] errmsg;
			throw ex;
		}
	} else {
		*ok = true;
		return val;
	}
}


float GTASectionFileReader::nextFloat(bool* ok, char* str)
{
	char* cstr = nextString(ok, str);

	if (!cstr) {
		*ok = false;
		return 0;
	}

	setlocale(LC_NUMERIC, "C");

	float val;
	int numScanned = sscanf(cstr, "%f", &val);

	if (numScanned == 0) {
		*ok = false;
		char* errmsg = new char[128];
		sprintf(errmsg, "Syntax error: Parameter no. %d is not a float at line %d",
				paramNo, lastReadLine);
		log.log(errmsg);

		if (errorBehavior == Continue) {
			delete[] errmsg;
			return 0;
		} else {
			GTASectionFileException ex(errmsg, __FILE__, __LINE__);
			delete[] errmsg;
			throw ex;
		}
	} else {
		*ok = true;
		return val;
	}
}



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

#include "GTASectionFileReader.h"
#include "util/stream/FileInputStream.h"



GTASectionFileReader::GTASectionFileReader(InputStream* stream, bool deleteStream)
		: stream(stream), deleteStream(deleteStream)
{
}


GTASectionFileReader::GTASectionFileReader(const File& file)
		: stream(file.openStream()), deleteStream(true)
{
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
		//stream->getline(buf, len);
		stream->readLine(buf, len);

		if (stream->hasReachedEnd()) {
			return false;
		}

		if (buf[strlen(buf)-1] == '\r') {
			buf[strlen(buf)-1] = '\0';
		}

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

	char* strEnd = str + strlen(str) - 1;
	char* newStr = str;

	while (*newStr == ' ') newStr++;

	while(*strEnd == ' ') *strEnd-- = '\0';

	return newStr;
}



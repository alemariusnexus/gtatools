/*
 * GTASectionFileReader.cpp
 *
 *  Created on: 18.04.2010
 *      Author: alemariusnexus
 */

#include "GTASectionFileReader.h"
#include <fstream>

using std::ifstream;



GTASectionFileReader::GTASectionFileReader(istream* stream, bool deleteStream)
		: stream(stream), deleteStream(deleteStream)
{
}


GTASectionFileReader::GTASectionFileReader(const char* filename)
		: stream(new ifstream(filename)), deleteStream(true)
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
		stream->getline(buf, len);

		if (stream->eof()) {
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



/*
 * GTASectionFileReader.h
 *
 *  Created on: 18.04.2010
 *      Author: alemariusnexus
 */

#ifndef GTASECTIONFILEREADER_H_
#define GTASECTIONFILEREADER_H_

#include "gf_config.h"
#include <cstring>
#include <cstdlib>
#include "util/stream/InputStream.h"



class GTASectionFileReader {
protected:
	GTASectionFileReader(InputStream* stream, bool deleteStream = true);
	GTASectionFileReader(const char* filename);
	virtual ~GTASectionFileReader();

	bool readNextLine(char* buf, int len);

	char* nextString(char* str = NULL) { return trim(strtok(str, ",")); }
	int32_t nextInt(char* str = NULL) { return atoi(nextString(str)); }
	float nextFloat(char* str = NULL) { return (float) atof(nextString(str)); }
	char* trim(char* str);

private:
	InputStream* stream;
	bool deleteStream;
};

#endif /* GTASECTIONFILEREADER_H_ */

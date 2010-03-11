/*
 * IDEReader.h
 *
 *  Created on: 09.02.2010
 *      Author: alemariusnexus
 */

#ifndef IDEREADER_H_
#define IDEREADER_H_

#include "../gf_config.h"
#include <istream>
#include <cstring>
#include <cstdlib>
#include "IDEEntity.h"

using std::istream;


class IDEReader
{
private:
	enum SectionType {
		NONE,
		OBJS
	};

public:
	IDEReader(istream* stream, bool deleteStream = true);
	IDEReader(const char* filename);
	~IDEReader();
	IDEEntity* readEntity();

private:
	char* nextString(char* str = NULL) { return strtok(str, ","); }
	int32_t nextInt(char* str = NULL) { return atoi(nextString(str)); }
	float nextFloat(char* str = NULL) { return (float) atof(nextString(str)); }

private:
	istream* stream;
	SectionType currentSection;
	bool deleteStream;
};

#endif /* IDEREADER_H_ */

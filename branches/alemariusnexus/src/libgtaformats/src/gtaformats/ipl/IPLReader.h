/*
 * IPLReader.h
 *
 *  Created on: 18.04.2010
 *      Author: alemariusnexus
 */

#ifndef IPLREADER_H_
#define IPLREADER_H_

#include <cstring>
#include "IPLStatement.h"
#include "../GTASectionFileReader.h"
#include "../util/stream/InputStream.h"



class IPLReader : public GTASectionFileReader
{
private:
	enum SectionType {
		NONE,
		INST
	};

public:
	IPLReader(InputStream* stream, bool deleteStream = true);
	IPLReader(const char* filename);
	~IPLReader();
	IPLStatement* readStatement();

private:
	SectionType currentSection;
};

#endif /* IPLREADER_H_ */

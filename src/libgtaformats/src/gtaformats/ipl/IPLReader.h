/*
 * IPLReader.h
 *
 *  Created on: 18.04.2010
 *      Author: alemariusnexus
 */

#ifndef IPLREADER_H_
#define IPLREADER_H_

#include <istream>
#include <cstring>
#include "IPLStatement.h"
#include "../GTASectionFileReader.h"

using std::istream;



class IPLReader : public GTASectionFileReader
{
private:
	enum SectionType {
		NONE,
		INST
	};

public:
	IPLReader(istream* stream, bool deleteStream = true);
	IPLReader(const char* filename);
	~IPLReader();
	IPLStatement* readStatement();

private:
	SectionType currentSection;
};

#endif /* IPLREADER_H_ */

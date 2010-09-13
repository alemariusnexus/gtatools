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

#ifndef IPLREADER_H_
#define IPLREADER_H_

#include <cstring>
#include "IPLStatement.h"
#include "../GTASectionFileReader.h"
#include "../util/stream/InputStream.h"
#include "../util/File.h"



class IPLReader : public GTASectionFileReader
{
private:
	enum SectionType {
		NONE,
		INST
	};

public:
	IPLReader(InputStream* stream, bool deleteStream = true);
	IPLReader(const File& file);
	~IPLReader();
	IPLStatement* readStatement();

private:
	void init();

private:
	SectionType currentSection;
	int32_t binaryInstanceCount;
	int32_t binaryInstanceOffset;
	int32_t binaryCarCount;
	int32_t binaryCarOffset;
	int32_t binaryReadCount;
};

#endif /* IPLREADER_H_ */

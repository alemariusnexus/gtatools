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

#ifndef IPLREADER_H_
#define IPLREADER_H_

#include <gtaformats/config.h>
#include <cstring>
#include "IPLStatement.h"
#include "../GTASectionFileReader.h"
#include "../util/File.h"
#include <istream>

using std::istream;



class IPLReader : public GTASectionFileReader
{
private:
	enum SectionType {
		NONE,
		INST,
		CARS
	};

public:
	IPLReader(istream* stream, bool deleteStream = true);
	IPLReader(const File& file);
	~IPLReader();
	IPLStatement* readStatement();
	bool isBinary() const { return binaryReadCount != -1; }

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

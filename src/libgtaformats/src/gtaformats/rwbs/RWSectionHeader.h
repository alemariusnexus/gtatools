/*
	Copyright 2010-2014 David "Alemarius Nexus" Lerch

	This file is part of gtatools-gui.

	gtatools-gui is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	gtatools-gui is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with gtatools-gui.  If not, see <http://www.gnu.org/licenses/>.

	Additional permissions are granted, which are listed in the file
	GPLADDITIONS.
 */

#ifndef RWSECTIONHEADER_H_
#define RWSECTIONHEADER_H_

#include "rwbs.h"
#include <istream>

using std::istream;



class RWSectionHeader
{
public:
	static bool readSectionHeader(RWSectionHeader* outHeader, istream* stream, const RWSectionHeader* lastRead = NULL);

	static bool isContainerSection(uint32_t id)
	{
		return (
			id == RW_SECTION_CLUMP  ||  id == RW_SECTION_FRAMELIST  ||  id == RW_SECTION_EXTENSION
			||  id == RW_SECTION_GEOMETRYLIST  ||  id == RW_SECTION_GEOMETRY
			||  id == RW_SECTION_MATERIALLIST  ||  id == RW_SECTION_MATERIAL  ||  id == RW_SECTION_TEXTURE
			||  id == RW_SECTION_ATOMIC ||  id == RW_SECTION_TEXTUREDICTIONARY
			||  id == RW_SECTION_TEXTURENATIVE
		);
	}

public:
	RWSectionHeader() : id(RW_SECTION_INVALID), size(0), version(0) {}
	RWSectionHeader(const RWSectionHeader& other) : id(other.id), size(other.size), version(other.version) {}
	RWSectionHeader(uint32_t id, uint32_t size, uint32_t version) : id(id), size(size), version(version) {}

	uint32_t getID() const { return id; }
	uint32_t getSize() const { return size; }
	uint32_t getVersion() const { return version; }
	bool isContainerSection() const { return isContainerSection(id); }

	void setID(uint32_t id) { this->id = id; }
	void setSize(uint32_t size) { this->size = size; }
	void setVersion(uint32_t version) { this->version = version; }

private:
	// These three values HAVE to be the first members, and remain in this order!
	uint32_t id;
	uint32_t size;
	uint32_t version;

private:
	friend class RWSection;
};

#endif /* RWSECTIONHEADER_H_ */

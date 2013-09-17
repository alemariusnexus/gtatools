/*
	Copyright 2010-2013 David "Alemarius Nexus" Lerch

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

#ifndef RWSECTION_H_
#define RWSECTION_H_

#include "rwbs.h"
#include <vector>
#include <istream>
#include <ostream>
#include "../util/File.h"
#include <algorithm>

using std::vector;
using std::find;
using std::istream;
using std::ostream;




class RWSection {
private:
	typedef vector<RWSection*> ChildList;

public:
	typedef ChildList::iterator ChildIterator;
	typedef ChildList::const_iterator ConstChildIterator;

public:
	static RWSection* readSection(istream* stream, RWSection* lastRead = NULL, int i = 0);

private:
	static bool isContainerSection(int32_t id)
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
	RWSection(int32_t id, int32_t version);
	RWSection(int32_t id, RWSection* parent);
	RWSection(uint8_t* rawData);
	~RWSection();

	int32_t getID() const { return id; }
	int32_t getSize() const { return size; }
	int32_t getVersion() const { return version; }
	uint64_t getAbsoluteOffset() const { return offset; }
	void setAbsoluteOffset(uint64_t offset) { this->offset = offset; }
	RWSection* getParent() { return parent; }
	uint8_t* getData() { ensureData(); return data; }
	bool isDataSection() const { return data != NULL; }

	void computeAbsoluteOffsets(uint64_t offset = 0);

	void setID(int32_t id) { this->id = id; }
	void setVersion(int32_t version) { this->version = version; }
	void setData(uint8_t* data, int32_t size);

	void addChild(RWSection* child);
	void insertChild(ChildIterator it, RWSection* sect);
	void removeChild(RWSection* child);

	ChildIterator getChildBegin() { ensureContainer(); return children.begin(); }
	ChildIterator getChildEnd() { ensureContainer(); return children.end(); }
	ChildIterator nextChild(int32_t id, ChildIterator it);
	ChildIterator getChildIterator(int32_t id);
	ChildIterator getLastChildIterator(int32_t id);
	int getChildCount() { ensureContainer(); return children.size(); }
	RWSection* getChild(int32_t id);
	RWSection* getLastChild(int32_t id);
	RWSection& operator[](int32_t id) { return *getChild(id); }
	int indexOf(const RWSection* child)
			{ ensureContainer(); return find(children.begin(), children.end(), child) - children.begin(); }

	void setDataSection(bool ds) { if (ds) ensureData(); else ensureContainer(); }

	void toRawData(uint8_t* data);
	uint8_t* toRawData() { uint8_t* data = new uint8_t[size+12]; toRawData(data); return data; }
	void write(ostream* stream);
	void write(const File& file);

	void printDebug(int ind = 0);

private:
	RWSection() : parent(NULL), data(NULL) {}
	void childResized(int32_t change) { size += change; if (parent) parent->childResized(change); }
	void ensureContainer();
	void ensureData();
	void clearChildren();

private:
	int32_t id;
	int32_t size;
	int32_t version;
	uint64_t offset;
	RWSection* parent;
	uint8_t* data;
	ChildList children;

	friend class TXDArchive;
};

#endif /* RWSECTION_H_ */

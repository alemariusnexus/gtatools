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

#ifndef RWSECTION_H_
#define RWSECTION_H_

#include "rwbs.h"
#include <vector>
#include <istream>
#include <ostream>
#include <nxcommon/file/File.h>
#include <algorithm>

using std::vector;
using std::find;
using std::istream;
using std::ostream;




class RWSection {
public:
	typedef vector<RWSection*> ChildList;
	typedef ChildList::iterator ChildIterator;
	typedef ChildList::const_iterator ConstChildIterator;

public:
	static RWSection* readSection(istream* stream, RWSection* lastRead = NULL);

private:
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
	RWSection(uint32_t id, uint32_t version);
	RWSection(uint32_t id, RWSection* parent);
	RWSection(uint8_t* rawData, size_t dataSize);
	RWSection(const RWSection& other);
	~RWSection();

	uint32_t getID() const { return id; }
	uint32_t getSize() const { return size; }
	uint32_t getVersion() const { return version; }
	uint64_t getAbsoluteOffset() const { return offset; }
	void setAbsoluteOffset(uint64_t offset) { this->offset = offset; }
	RWSection* getParent() { return parent; }
	const RWSection* getParent() const { return parent; }
	uint8_t* getData() { ensureData(); return data; }
	bool isDataSection() const { return data != NULL; }
	bool isContainerSection() const { return !isDataSection(); }
	bool canBeConvertedToContainerSection() const;

	void computeAbsoluteOffsets(uint64_t offset = 0);

	void setID(uint32_t id) { this->id = id; }
	void setVersion(uint32_t version) { this->version = version; }
	void setData(uint8_t* data, uint32_t size);

	void addChild(RWSection* child);
	void insertChild(ChildIterator it, RWSection* sect);
	void removeChild(RWSection* child);

	ChildList& getChildren() { ensureContainer(); return children; }
	ChildIterator getChildBegin() { ensureContainer(); return children.begin(); }
	ChildIterator getChildEnd() { ensureContainer(); return children.end(); }
	ChildIterator nextChild(uint32_t id, ChildIterator it);
	ChildIterator getChildIterator(uint32_t id);
	ChildIterator getLastChildIterator(uint32_t id);
	ChildIterator getChildIteratorByIndex(int idx)
			{ return (idx < 0  ||  idx > children.size()) ? children.end() : children.begin() + idx; }
	int getChildCount() { ensureContainer(); return children.size(); }
	RWSection* getChild(uint32_t id);
	RWSection* getLastChild(uint32_t id);
	RWSection& operator[](uint32_t id) { return *getChild(id); }
	int indexOf(const RWSection* child);
	RWSection* getChildByIndex(int idx) { return (idx < 0  ||  idx >= children.size()) ? NULL : children[idx]; }

	void setDataSection(bool ds) { if (ds) ensureData(); else ensureContainer(); }
	void setContainerSection(bool cs) { setDataSection(!cs); }

	void toRawData(uint8_t* data, size_t bufSize);
	uint8_t* toRawData() { uint8_t* data = new uint8_t[size+12]; toRawData(data, size+12); return data; }
	void write(ostream* stream);
	void write(const File& file);

	void printDebug(int ind = 0);

	CString getDescription() const
			{ return RWGetSectionName(id).append(" (").append(size).append(" bytes @ 0x").append(offset, 16).append(")"); }

private:
	RWSection() : parent(NULL), data(NULL) {}
	void childResized(uint32_t change) { size += change; if (parent) parent->childResized(change); }
	void ensureContainer();
	void ensureData();
	void clearChildren();

private:
	// These three values HAVE to be the first members, and remain in this order!
	uint32_t id;
	uint32_t size;
	uint32_t version;

	uint64_t offset;
	RWSection* parent;
	uint8_t* data;
	ChildList children;

	friend class TXDArchive;
};

#endif /* RWSECTION_H_ */

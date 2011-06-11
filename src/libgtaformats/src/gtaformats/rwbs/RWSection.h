/*
	Copyright 2010-2011 David "Alemarius Nexus" Lerch

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

#include <vector>
#include <istream>
#include <ostream>
#include "../util/File.h"
#include <algorithm>

using std::vector;
using std::find;
using std::istream;
using std::ostream;


#define RW_SECTION_INVALID 0
#define RW_SECTION_STRUCT 1				// Alias Data
#define RW_SECTION_STRING 2
#define RW_SECTION_EXTENSION 3
#define RW_SECTION_TEXTURE 6
#define RW_SECTION_MATERIAL 7
#define RW_SECTION_MATERIALLIST 8
#define RW_SECTION_FRAMELIST 14
#define RW_SECTION_GEOMETRY 15
#define RW_SECTION_CLUMP 16
#define RW_SECTION_ATOMIC 20
#define RW_SECTION_GEOMETRYLIST 26
#define RW_SECTION_MATERIALSPLIT 1294	// Alias Bin Mesh PLG
#define RW_SECTION_FRAME 39056126

#define RW_SECTION_HANIM_PLG 0x011E
#define RW_SECTION_SKY_MIPMAP_VAL 0x0110
#define RW_SECTION_MATERIAL_EFFECTS_PLG 0x0120
#define RW_SECTION_SPECULAR_MATERIAL 0x0253F2F6
#define RW_SECTION_NIGHT_VERTEX_COLORS 0x0253F2F9
#define RW_SECTION_REFLECTION_MATERIAL 0x0253F2FC
#define RW_SECTION_2DFX 0x0253F2F8
#define RW_SECTION_MESH_EXTENSION 0x0253F2FD
#define RW_SECTION_RIGHT_TO_RENDER 0x001F
#define RW_SECTION_COLLISION_MODEL 0x0253F2FA

#define RW_SECTION_TEXTUREDICTIONARY 0x16
#define RW_SECTION_TEXTURENATIVE 0x15


class RWSection {
private:
	typedef vector<RWSection*> ChildList;

public:
	typedef ChildList::iterator ChildIterator;
	typedef ChildList::const_iterator ConstChildIterator;

public:
	static RWSection* readSection(istream* stream, RWSection* lastRead = NULL);

public:
	RWSection(int32_t id, int32_t version);
	RWSection(int32_t id, RWSection* parent);
	~RWSection();

	int32_t getID() const { return id; }
	int32_t getSize() const { return size; }
	int32_t getVersion() const { return version; }
	uint64_t getAbsoluteOffset() const { return offset; }
	void setAbsoluteOffset(uint64_t offset) { this->offset = offset; }
	RWSection* getParent() { return parent; }
	uint8_t* getData() { return data; }
	bool isDataSection() const { return data != NULL; }

	void computeAbsoluteOffsets(uint64_t offset = 0);

	void setVersion(int32_t version) { this->version = version; }
	void setData(uint8_t* data, int32_t size);

	void addChild(RWSection* child);
	void insertChild(ChildIterator it, RWSection* sect);
	void removeChild(RWSection* child);

	ChildIterator getChildBegin() { return children.begin(); }
	ConstChildIterator getChildBegin() const { return children.begin(); }
	ChildIterator getChildEnd() { return children.end(); }
	ConstChildIterator getChildEnd() const { return children.end(); }
	ChildIterator nextChild(int32_t id, ChildIterator it);
	ChildIterator getChildIterator(int32_t id);
	ChildIterator getLastChildIterator(int32_t id);
	int getChildCount() const { return children.size(); }
	RWSection* getChild(int32_t id);
	RWSection* getLastChild(int32_t id);
	RWSection& operator[](int32_t id) { return *getChild(id); }
	int indexOf(const RWSection* child) const
			{ return find(children.begin(), children.end(), child) - children.begin(); }

	void write(ostream* stream);
	void write(const File& file);

	void printDebug(int ind = 0);

private:
	RWSection() : parent(NULL), data(NULL) {}
	void childResized(int32_t change) { size += change; if (parent) parent->childResized(change); }

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

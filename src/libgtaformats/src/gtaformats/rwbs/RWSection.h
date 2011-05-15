/*
 * RWSection.h
 *
 *  Created on: 23.04.2011
 *      Author: alemariusnexus
 */

#ifndef RWSECTION_H_
#define RWSECTION_H_

#include <vector>
#include <istream>
#include <ostream>
#include "../util/File.h"

using std::vector;
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
	RWSection(int32_t id, int32_t version);
	RWSection(int32_t id, RWSection* parent);
	RWSection(istream* stream);
	~RWSection();

	int32_t getID() const { return id; }
	int32_t getSize() const { return size; }
	int32_t getVersion() const { return version; }
	RWSection* getParent() { return parent; }
	uint8_t* getData() { return data; }
	bool isDataSection() const { return data != NULL; }

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
	RWSection* getChild(int32_t id);
	RWSection* getLastChild(int32_t id);
	RWSection& operator[](int32_t id) { return *getChild(id); }

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
	RWSection* parent;
	uint8_t* data;
	ChildList children;

	friend class TXDArchive;
};

#endif /* RWSECTION_H_ */

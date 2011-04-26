/*
 * RWSection.cpp
 *
 *  Created on: 23.04.2011
 *      Author: alemariusnexus
 */

#include "RWSection.h"
#include <algorithm>
#include "../gta.h"

using std::find;




RWSection::RWSection(int32_t id, int32_t version)
		: id(id), size(0), version(version), parent(NULL), data(NULL)
{
}


RWSection::RWSection(int32_t id, RWSection* parent)
		: id(id), size(0), version(parent->version), parent(parent), data(NULL)
{
	parent->addChild(this);
}


RWSection::RWSection(istream* stream)
		: parent(NULL)
{
	stream->read((char*) this, 12);

	bool dataSect;

	if (	id == RW_SECTION_CLUMP
		||  id == RW_SECTION_FRAMELIST
		||  id == RW_SECTION_EXTENSION
		||  id == RW_SECTION_GEOMETRYLIST
		||  id == RW_SECTION_GEOMETRY
		||  id == RW_SECTION_MATERIALLIST
		||  id == RW_SECTION_MATERIAL
		||  id == RW_SECTION_TEXTURE
		||  id == RW_SECTION_MATERIAL_EFFECTS_PLG
		||  id == RW_SECTION_ATOMIC
		||  id == RW_SECTION_TEXTUREDICTIONARY
		||  id == RW_SECTION_TEXTURENATIVE
	) {
		dataSect = false;
	} else {
		dataSect = true;
	}

	if (dataSect) {
		data = new uint8_t[size];
		stream->read((char*) data, size);
	} else {
		data = NULL;
		int32_t numRead = 0;

		while (numRead < size) {
			RWSection* child = new RWSection(stream);
			child->parent = this;
			numRead += child->size+12;
			children.push_back(child);
		}
	}
}


RWSection::~RWSection()
{
	if (data) {
		delete[] data;
	} else {
		for (ChildIterator it = children.begin() ; it != children.end() ; it++) {
			delete *it;
		}
	}
}


void RWSection::setData(uint8_t* data, int32_t size)
{
	this->data = data;

	if (parent)
		parent->childResized(size - this->size);

	this->size = size;
}


void RWSection::addChild(RWSection* child)
{
	children.push_back(child);
	size += child->size + 12;

	child->parent = this;

	if (parent)
		parent->childResized(child->size + 12);
}


void RWSection::insertChild(ChildIterator it, RWSection* sect)
{
	children.insert(it, sect);
	size += sect->size + 12;

	sect->parent = this;

	if (parent)
		parent->childResized(sect->size + 12);
}


void RWSection::removeChild(RWSection* child)
{
	children.erase(find(children.begin(), children.end(), child));
	size -= child->size + 12;

	if (parent)
		parent->childResized(-child->size - 12);
}


RWSection::ChildIterator RWSection::nextChild(int32_t id, ChildIterator it)
{
	while (it != children.end()) {
		RWSection* sect = *it;

		if (sect->id == id) {
			break;
		}

		it++;
	}

	return it;
}


RWSection::ChildIterator RWSection::getChildIterator(int32_t id)
{
	return nextChild(id, children.begin());
}


RWSection::ChildIterator RWSection::getLastChildIterator(int32_t id)
{
	ChildIterator it = children.end();

	do {
		it--;
		RWSection* sect = *it;

		if (sect->id == id) {
			return it;
		}
	} while (it != children.begin());

	return children.end();
}


RWSection* RWSection::getLastChild(int32_t id)
{
	ChildIterator it = getLastChildIterator(id);

	if (it == children.end()) {
		return NULL;
	} else {
		return *it;
	}
}


RWSection* RWSection::getChild(int32_t id)
{
	ChildIterator it = getChildIterator(id);

	if (it == children.end()) {
		return NULL;
	} else {
		return *it;
	}
}


void RWSection::write(ostream* stream)
{
	stream->write((char*) this, 12);

	if (data) {
		stream->write((char*) data, size);
	} else {
		for (ChildIterator it = children.begin() ; it != children.end() ; it++) {
			RWSection* child = *it;
			child->write(stream);
		}
	}
}


void RWSection::write(const File& file)
{
	ostream* stream = file.openOutputStream(ostream::binary);
	write(stream);
	delete stream;
}


void RWSection::printDebug(int ind)
{
	for (int i = 0 ; i < ind ; i++) {
		printf("  ");
	}

	char name[64];
	RwGetSectionName(id, name);
	printf("%s (%d Bytes)\n", name, size);

	if (!data) {
		for (ChildIterator it = children.begin() ; it != children.end() ; it++) {
			(*it)->printDebug(ind+1);
		}
	}
}



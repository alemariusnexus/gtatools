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

#include "RWSection.h"
#include <algorithm>
#include "../gta.h"
#include "RWBSException.h"
#include "../util/stream/StreamReader.h"
#include "../util/stream/EndianSwappingStreamReader.h"
#include "../util/stream/StreamWriter.h"
#include "../util/stream/EndianSwappingStreamWriter.h"

using std::find;




RWSection::RWSection(int32_t id, int32_t version)
		: id(id), size(0), version(version), offset(0), parent(NULL), data(NULL)
{
}


RWSection::RWSection(int32_t id, RWSection* parent)
		: id(id), size(0), version(parent->version), offset(0), parent(parent), data(NULL)
{
	parent->addChild(this);
}


RWSection::RWSection(uint8_t* rawData)
		: id(FromLittleEndian32(*((int32_t*) rawData))), size(FromLittleEndian32(*((int32_t*) (rawData+4)))),
		  version(FromLittleEndian32(*((int32_t*) (rawData+8)))), offset(0), parent(NULL), data(NULL)
{
	bool dataSect = !isContainerSection(id);

	if (dataSect) {
		data = new uint8_t[size];
		memcpy(data, rawData+12, size);
	} else {
		int32_t numRead = 0;
		rawData += 12;

		while (numRead < size) {
			RWSection* child = new RWSection(rawData);
			child->parent = this;
			children.push_back(child);

			numRead += child->size + 12;
			rawData += child->size + 12;
		}
	}
}


RWSection* RWSection::readSection(istream* stream, RWSection* lastRead, int i)
{
	// Check if there is data to read
	stream->peek();
	if (stream->eof()) {
		return NULL;
	}

	RWSection* sect = new RWSection;
	stream->read((char*) sect, 12);

#ifndef GTAFORMATS_LITTLE_ENDIAN
	sect->id = SwapEndianness32(sect->id);
	sect->size = SwapEndianness32(sect->size);
	sect->version = SwapEndianness32(sect->version);
#endif

	if (lastRead) {
		// Do some heuristic checks to see if the data we read is actually a section. For files in IMG
		// archives, there might be some junk after the actual section data (to fill the IMG section). If it
		// seems to be an invalid section, we assume this is the end of the RWBS file.
		if (	lastRead->version != sect->version
				||  sect->size < 0
		) {
			delete sect;
			return NULL;
		}
	}

	sect->offset = 0;

	bool dataSect = !isContainerSection(sect->id);

	if (dataSect) {
		sect->data = new uint8_t[sect->size];
		stream->read((char*) sect->data, sect->size);
	} else {
		sect->data = NULL;
		int32_t numRead = 0;

		while (numRead < sect->size) {
			// Using NULL disables the end-of-file check. We know that there has to be another section.
			RWSection* child = RWSection::readSection(stream, NULL, i+1);

			if (!child) {
				throw RWBSException("Premature end of RWBS file (wrong section size)", __FILE__, __LINE__);
			}

			child->parent = sect;
			numRead += child->size+12;
			sect->children.push_back(child);
		}
	}

	return sect;
}


void RWSection::ensureContainer()
{
	if (!data)
		return;

	int32_t numRead = 0;
	uint8_t* data = this->data;

	while (numRead < size) {
		RWSection* child = new RWSection(data);
		child->parent = this;
		children.push_back(child);

		numRead += 12 + child->size;
		data += 12 + child->size;
	}

	delete[] this->data;
	this->data = NULL;
}


void RWSection::ensureData()
{
	if (data)
		return;

	data = new uint8_t[size+12];
	uint8_t* offsetData = data;

	for (ChildIterator it = children.begin() ; it != children.end() ; it++) {
		RWSection* child = *it;
		child->toRawData(offsetData);
		offsetData += child->size + 12;
		delete child;
	}

	children.clear();
}


void RWSection::clearChildren()
{
	if (!data) {
		for (ChildIterator it = children.begin() ; it != children.end() ; it++) {
			RWSection* child = *it;
			delete child;
		}

		children.clear();
	}
}


void RWSection::computeAbsoluteOffsets(uint64_t offset)
{
	this->offset = offset;

	offset += 12;

	if (!isDataSection()) {
		for (ChildIterator it = children.begin() ; it != children.end() ; it++) {
			RWSection* child = *it;
			child->computeAbsoluteOffsets(offset);
			offset += child->size + 12;
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
	clearChildren();

	if (this->data)
		delete[] this->data;

	this->data = data;

	if (parent)
		parent->childResized(size - this->size);

	this->size = size;
}


void RWSection::addChild(RWSection* child)
{
	ensureContainer();

	children.push_back(child);
	size += child->size + 12;

	child->parent = this;

	if (parent)
		parent->childResized(child->size + 12);
}


void RWSection::insertChild(ChildIterator it, RWSection* sect)
{
	ensureContainer();

	children.insert(it, sect);
	size += sect->size + 12;

	sect->parent = this;

	if (parent)
		parent->childResized(sect->size + 12);
}


void RWSection::removeChild(RWSection* child)
{
	ensureContainer();

	children.erase(find(children.begin(), children.end(), child));
	size -= child->size + 12;

	if (parent)
		parent->childResized(-child->size - 12);

	child->parent = NULL;
}


RWSection::ChildIterator RWSection::nextChild(int32_t id, ChildIterator it)
{
	// Not needed, because if the caller has got a valid iterator, it must already be a container section:
	// ensureContainer();

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
	ensureContainer();
	return nextChild(id, children.begin());
}


RWSection::ChildIterator RWSection::getLastChildIterator(int32_t id)
{
	ensureContainer();
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
	ensureContainer();
	ChildIterator it = getLastChildIterator(id);

	if (it == children.end()) {
		return NULL;
	} else {
		return *it;
	}
}


RWSection* RWSection::getChild(int32_t id)
{
	ensureContainer();
	ChildIterator it = getChildIterator(id);

	if (it == children.end()) {
		return NULL;
	} else {
		return *it;
	}
}


void RWSection::toRawData(uint8_t* data)
{
#ifdef GTAFORMATS_LITTLE_ENDIAN
	memcpy(data, this, 12);
#else
	uint32_t* data32 = (uint32_t*) data;
	data32[0] = ToLittleEndian32(id);
	data32[1] = ToLittleEndian32(size);
	data32[2] = ToLittleEndian32(version);
#endif

	int32_t offset = 12;

	if (isDataSection()) {
		memcpy(data+offset, this->data, size);
	} else {
		for (ChildIterator it = children.begin() ; it != children.end() ; it++) {
			RWSection* child = *it;
			child->toRawData(data+offset);
			offset += child->size + 12;
		}
	}
}


void RWSection::write(ostream* stream)
{
#ifdef GTAFORMATS_LITTLE_ENDIAN
	StreamWriter writer(stream);
#else
	EndianSwappingStreamWriter writer(stream);
#endif

	writer.writeArrayCopyU32((uint32_t*) this, 3);

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

	char name[RW_MAX_SECTION_NAME];
	RWGetSectionName(id, name);
	printf("%s (%d Bytes)\n", name, size);

	if (!data) {
		for (ChildIterator it = children.begin() ; it != children.end() ; it++) {
			(*it)->printDebug(ind+1);
		}
	}
}



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

#include "RWSection.h"
#include <algorithm>
#include "../gta.h"
#include "RWBSException.h"
#include <nxcommon/stream/StreamReader.h>
#include <nxcommon/stream/EndianSwappingStreamReader.h>
#include <nxcommon/stream/StreamWriter.h>
#include <nxcommon/stream/EndianSwappingStreamWriter.h>

using std::find;
using std::min;
using std::max;




// Maximum number of bytes read in one block.
// This is currently 4 MBytes.
#define SINGLE_READ_BUFFER_SIZE 4194304U




RWSection::RWSection(uint32_t id, uint32_t version)
		: id(id), size(0), version(version), offset(0), parent(NULL), data(NULL)
{
}


RWSection::RWSection(uint32_t id, RWSection* parent)
		: id(id), size(0), version(parent->version), offset(0), parent(parent), data(NULL)
{
	parent->addChild(this);
}


RWSection::RWSection(uint8_t* rawData, size_t dataSize)
		: offset(0), parent(NULL), data(NULL)
{
	if (dataSize < 12) {
		throw RWBSException(CString("Invalid data size for RWSection(uint8_t*, size_t): Must be at least 12 bytes, but was ")
				<< dataSize << " bytes.");
	}

	id = FromLittleEndian32(*((uint32_t*) rawData));
	size = FromLittleEndian32(*((uint32_t*) (rawData+4)));
	version = FromLittleEndian32(*((uint32_t*) (rawData+8)));

	dataSize -= 12;
	rawData += 12;

	bool dataSect = !isContainerSection(id);

	if (dataSize < size) {
		throw RWBSException(CString("Premature end of RWBS data: Section ") << getDescription() << " ended after "
				<< dataSize << " bytes.", __FILE__, __LINE__);
	}

	if (dataSect) {
		data = new uint8_t[size];
		memcpy(data, rawData, size);
	} else {
		uint32_t numRead = 0;

		while (numRead < size) {
			RWSection* child;

			try {
				child = new RWSection(rawData, dataSize);
			} catch (Exception&) {
				clearChildren();
				throw;
			}
			child->parent = this;
			children.push_back(child);

			numRead += child->size + 12;
			rawData += child->size + 12;
			dataSize -= child->size + 12;
		}
	}
}


RWSection::RWSection(const RWSection& other)
		: id(other.id), size(other.size), version(other.version), offset(other.offset), parent(other.parent),
		  data(other.data ? new uint8_t[size] : NULL)
{
	if (data) {
		memcpy(data, other.data, size);
	}

	for (ConstChildIterator it = other.children.begin() ; it != other.children.end() ; it++) {
		const RWSection* child = *it;
		RWSection* childCpy = new RWSection(*child);
		childCpy->parent = this;
		children.push_back(childCpy);
	}
}


RWSection* RWSection::readSection(istream* stream, RWSection* lastRead)
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
		if (		lastRead->version != sect->version
				||	(sect->id == 0  &&  sect->size == 0  &&  sect->version == 0)
		) {
			delete sect;
			return NULL;
		}
	}

	sect->offset = 0;

	bool dataSect = !isContainerSection(sect->id);

	if (dataSect) {
		sect->data = NULL;
		uint32_t numRead = 0;

		// We read this blockwise to be able to detect an invalid section with an enormously high size without first
		// allocating a buffer of that size.

		do {
			uint32_t numToBeRead = min(sect->size-numRead, SINGLE_READ_BUFFER_SIZE);
			uint8_t* newData = new uint8_t[numRead + numToBeRead];

			if (sect->data) {
				memcpy(newData, sect->data, numRead);
				delete[] sect->data;
			}

			sect->data = newData;

			stream->read((char*) (newData + numRead), numToBeRead);
			numRead += stream->gcount();
		} while (numRead < sect->size  &&  !stream->eof());

		if (stream->eof()  &&  numRead != sect->size) {
			CString desc = sect->getDescription();
			delete sect;
			throw RWBSException(CString("Premature end of RWBS file: Data section ") << desc << " ended after "
					<< numRead << " bytes.", __FILE__, __LINE__);
		}
	} else {
		sect->data = NULL;
		uint32_t numRead = 0;

		lastRead = sect;

		while (numRead < sect->size) {
			RWSection* child;

			try {
				child = RWSection::readSection(stream, lastRead);
			} catch (exception&) {
				delete sect;
				throw;
			}

			if (!child) {
				CString desc = sect->getDescription();
				delete sect;
				throw RWBSException(CString("Premature end of RWBS file: Container section ") << desc
						<< " ended after " << numRead << " bytes.", __FILE__, __LINE__);
			}

			child->parent = sect;
			numRead += child->size+12;
			sect->children.push_back(child);

			lastRead = child;
		}
	}

	return sect;
}


void RWSection::ensureContainer()
{
	if (!data)
		return;

	uint32_t numRead = 0;
	uint8_t* data = this->data;

	while (numRead < size) {
		if (size-numRead < 12) {
			clearChildren();
			throw RWBSException(CString("Section ") << getDescription() << " is not a valid container section: " << size-numRead
					<< " dangling bytes at section end.", __FILE__, __LINE__);
		}

		RWSection* child;

		try {
			child = new RWSection(data, size - numRead);
		} catch (exception&) {
			clearChildren();
			throw;
		}

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

		try {
			child->toRawData(offsetData, size+12 - (offsetData-data));
		} catch (exception&) {
			delete[] data;
			throw;
		}

		offsetData += child->size + 12;
	}

	for (ChildIterator it = children.begin() ; it != children.end() ; it++) {
		delete *it;
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


bool RWSection::canBeConvertedToContainerSection() const
{
	if (isContainerSection())
		return true;

	int32_t numRead = 0;
	uint8_t* data = this->data;

	while (numRead < size) {
		if (size-numRead < 12) {
			return false;
		}

		RWSection* child = NULL;

		try {
			child = new RWSection(data, size - numRead);
		} catch (RWBSException& ex) {
			return false;
		}

		numRead += 12 + child->size;
		data += 12 + child->size;

		delete child;
	}

	return true;
}


void RWSection::setData(uint8_t* data, uint32_t size)
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

	insertChild(children.end(), child);
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


RWSection::ChildIterator RWSection::nextChild(uint32_t id, ChildIterator it)
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


RWSection::ChildIterator RWSection::getChildIterator(uint32_t id)
{
	ensureContainer();
	return nextChild(id, children.begin());
}


RWSection::ChildIterator RWSection::getLastChildIterator(uint32_t id)
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


RWSection* RWSection::getLastChild(uint32_t id)
{
	ensureContainer();
	ChildIterator it = getLastChildIterator(id);

	if (it == children.end()) {
		return NULL;
	} else {
		return *it;
	}
}


RWSection* RWSection::getChild(uint32_t id)
{
	ensureContainer();
	ChildIterator it = getChildIterator(id);

	if (it == children.end()) {
		return NULL;
	} else {
		return *it;
	}
}


int RWSection::indexOf(const RWSection* child)
{
	ensureContainer();
	ChildIterator it = find(children.begin(), children.end(), child);

	if (it == children.end()) {
		return -1;
	} else {
		return it - children.begin();
	}
}


void RWSection::toRawData(uint8_t* data, size_t bufSize)
{
	if (bufSize < 12+size) {
		throw RWBSException(CString("RWSection::toRawData() called with an undersized buffer. We need at least ")
				<< 12+size << " bytes but got only " << bufSize << " bytes.", __FILE__, __LINE__);
	}

#ifdef GTAFORMATS_LITTLE_ENDIAN
	memcpy(data, this, 12);
#else
	uint32_t* data32 = (uint32_t*) data;
	data32[0] = ToLittleEndian32(id);
	data32[1] = ToLittleEndian32(size);
	data32[2] = ToLittleEndian32(version);
#endif

	int32_t offset = 12;
	bufSize -= 12;

	if (isDataSection()) {
		memcpy(data+offset, this->data, size);
	} else {
		for (ChildIterator it = children.begin() ; it != children.end() ; it++) {
			RWSection* child = *it;
			child->toRawData(data+offset, bufSize);
			offset += child->size + 12;
			bufSize -= child->size + 12;
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

	printf("%s\n", getDescription().get());

	if (!data) {
		for (ChildIterator it = children.begin() ; it != children.end() ; it++) {
			(*it)->printDebug(ind+1);
		}
	}
}



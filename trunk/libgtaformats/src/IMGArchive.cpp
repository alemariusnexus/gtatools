/*
 * IMGArchive.cpp
 *
 *  Created on: 21.01.2010
 *      Author: alemariusnexus
 */

#include "IMGArchive.h"
#include <string>
#include <fstream>
#include <cstring>

using std::string;


IMGException::IMGException(const char* msg, int curOffs) throw()
		: message(msg), currentOffset(curOffs)
{
}


IMGArchive::IMGArchive(istream* stream, bool maySeek)
		: stream(stream), maySeek(maySeek)
{
	char skipBuffer[4];

	stream->read(skipBuffer, 4);
	stream->read((char*) &numEntries, 4);

	assertNoEOF();

	entries = new IMGEntry*[numEntries];

	for (int32_t i = 0 ; i < numEntries ; i++) {
		entries[i] = new IMGEntry;
		stream->read((char*) entries[i], sizeof(IMGEntry));
		assertNoEOF();
	}

	bytesRead = 8 + numEntries*sizeof(IMGEntry);
}

istream* IMGArchive::gotoEntry(IMGEntry* entry) {
	long long start = entry->offset*IMG_BLOCK_SIZE;

	if (start <= bytesRead) {
		char skipBuffer[4096];
		SkipBytes(stream, start - bytesRead, skipBuffer, sizeof(skipBuffer));
		assertNoEOF();
		bytesRead = start;
	} else {
		if (maySeek) {
			stream->seekg(start, istream::beg);

			if (stream->fail()) {
				throw IMGException("Seeking IMG entry failed", bytesRead);
			}
		} else {
			string msg = "Entry ";
			msg += entry->name;
			msg += " is before current position, but may not seek!";
			throw IMGException(msg.c_str(), bytesRead);
		}
	}

	return stream;
}

istream* IMGArchive::gotoEntry(const char* name) {
	IMGEntry* entry = getEntryByName(name);

	if (!entry) {
		return NULL;
	}

	return gotoEntry(entry);
}

void IMGArchive::visit(IMGVisitor* visitor, IMGEntry* entry) {
	void* udata = NULL;

	if (visitor->readHeader(entry, udata)) {
		gotoEntry(entry);
		visitor->readEntry(entry, stream, udata);
	}
}

void IMGArchive::visitAll(IMGVisitor* visitor) {
	for (int i = 0 ; i < numEntries ; i++) {
		visit(visitor, entries[i]);
	}
}

IMGEntry* IMGArchive::getEntryByName(const char* name) {
	for (int32_t i = 0 ; i < numEntries ; i++) {
		if (strcmp(entries[i]->name, name) == 0) {
			return entries[i];
		}
	}

	return NULL;
}

bool IMGArchive::isValidFile(const char* filename) {
	char ver[4];
	std::ifstream stream(filename, std::ifstream::in | std::ifstream::binary);
	stream.read(ver, 4);
	stream.close();
	return strcmp(ver, "VER2") == 0;
}

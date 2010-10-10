/*
	Copyright 2010 David "Alemarius Nexus" Lerch

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
 */

#include "IMGArchive.h"
#include "IMGException.h"
#include <string>
#include <cstring>
#include <vector>
#include "../util/stream/RangedInputStream.h"
#include "../util/stream/BufferedInputStream.h"

using std::string;
using std::vector;

#define IMG_BUFFER_SIZE 4096




IMGArchive::IMGArchive(InputStream* stream, bool randomAccess, bool deleteStream)
		: stream(stream), randomAccess(randomAccess), deleteStream(deleteStream)
{
	readHeader(stream);
}


IMGArchive::IMGArchive(const File& file, bool deleteStream)
		: stream(NULL), randomAccess(true), deleteStream(deleteStream)
{
	const FilePath* path = file.getPath();
	FileContentType type = file.guessContentType();

	if (type == CONTENT_TYPE_DIR) {
		char* imgFilePath = new char[strlen(path->toString())+1];
		int basePathLen = path->getExtension() - path->toString();
		strncpy(imgFilePath, path->toString(), basePathLen);
		strcpy(imgFilePath+basePathLen, "img");
		File imgFile(imgFilePath);
		delete[] imgFilePath;

		InputStream* ubDirStream = openStream(file);

		if (!ubDirStream) {
			throw IMGException("Unable to open DIR file", __FILE__, __LINE__);
		}

		BufferedInputStream dirStream(ubDirStream, IMG_BUFFER_SIZE);

		stream = openStream(imgFile);

		if (!stream) {
			char errmsg[2048];
			sprintf(errmsg, "Unable to open corresponding IMG file %s for the given DIR file",
					imgFile.getPath()->toString());
			throw IMGException(errmsg, __FILE__, __LINE__);
		}

		stream = new BufferedInputStream(stream, IMG_BUFFER_SIZE);

		readHeader(&dirStream);
	} else if (type == CONTENT_TYPE_IMG) {
		stream = openStream(file);

		if (guessIMGVersion(stream, true) == VER2) {
			if (!stream) {
				throw IMGException("Unable to open VER2 IMG file", __FILE__, __LINE__);
			}

			stream = new BufferedInputStream(stream, IMG_BUFFER_SIZE);

			readHeader(stream);
		} else {
			if (!stream) {
				throw IMGException("Unable to open VER1 IMG file", __FILE__, __LINE__);
			}

			stream = new BufferedInputStream(stream, IMG_BUFFER_SIZE);

			char* dirFilePath = new char[strlen(path->toString())+1];
			int basePathLen = path->getExtension() - path->toString();
			strncpy(dirFilePath, path->toString(), basePathLen);
			strcpy(dirFilePath+basePathLen, "dir");
			File dirFile(dirFilePath);
			delete[] dirFilePath;

			InputStream* ubDirStream = openStream(dirFile);

			if (!ubDirStream) {
				char errmsg[2048];
				sprintf(errmsg, "Unable to open corresponding DIR file %s for the given IMG file",
						dirFile.getPath()->toString());
				throw IMGException(errmsg, __FILE__, __LINE__);
			}

			BufferedInputStream dirStream(ubDirStream, IMG_BLOCK_SIZE);

			readHeader(&dirStream);
		}
	} else {
		throw IMGException("File name is neither an IMG nor a DIR file", __FILE__, __LINE__);
	}
}


IMGArchive::IMGArchive(InputStream* dirStream, InputStream* imgStream, bool randomAccess,
		bool deleteIMGStream)
		: stream(imgStream), randomAccess(randomAccess), deleteStream(deleteIMGStream)
{
	BufferedInputStream bDirStream(dirStream, IMG_BUFFER_SIZE, false);
	readHeader(&bDirStream);
}


IMGArchive::IMGArchive(const File& dirFile, const File& imgFile, bool deleteStream)
		: stream(NULL), randomAccess(true), deleteStream(deleteStream)
{
	stream = openStream(imgFile);
	stream = new BufferedInputStream(stream, IMG_BUFFER_SIZE);

	InputStream* dirStream = openStream(dirFile);
	BufferedInputStream bDirStream(dirStream, IMG_BUFFER_SIZE);

	readHeader(&bDirStream);
}


IMGArchive::~IMGArchive()
{
	for (int32_t i = 0 ; i < numEntries ; i++) {
		delete entries[i];
	}

	delete[] entries;

	if (deleteStream)
		delete stream;
}


IMGArchive::IMGVersion IMGArchive::guessIMGVersion(InputStream* stream, bool returnToStart)
{
	char fourCC[4];
	stream->read(fourCC, 4);

	if (stream->hasReachedEnd()) {
		return VER1;
	}

	if (returnToStart) {
		stream->seek(-4);
	}

	if (fourCC[0] == 'V'  &&  fourCC[1] == 'E'  &&  fourCC[2] == 'R'  &&  fourCC[3] == '2') {
		return VER2;
	} else {
		return VER1;
	}
}


IMGArchive::IMGVersion IMGArchive::guessIMGVersion(const File& file)
{
	FileInputStream stream(file, STREAM_BINARY);
	return guessIMGVersion(&stream, false);
}


InputStream* IMGArchive::gotoEntry(IMGEntry* entry, bool autoCloseStream) {
	long long start = entry->offset*IMG_BLOCK_SIZE;
	stream->seek(start - stream->tell());
	RangedInputStream* rstream = new RangedInputStream(stream, entry->size*IMG_BLOCK_SIZE, autoCloseStream);
	return rstream;
}

InputStream* IMGArchive::gotoEntry(const char* name, bool autoCloseStream) {
	IMGEntry* entry = getEntryByName(name);

	if (!entry) {
		return NULL;
	}

	return gotoEntry(entry, autoCloseStream);
}

void IMGArchive::visit(IMGVisitor* visitor, IMGEntry* entry) {
	void* udata = NULL;

	if (visitor->readHeader(entry, udata)) {
		InputStream* rstream = gotoEntry(entry);
		visitor->readEntry(entry, rstream, udata);
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


void IMGArchive::readHeader(InputStream* stream)
{
	int32_t firstBytes;

	stream->read((char*) &firstBytes, 4);
	int gcount = stream->getLastReadCount();

	if (stream->hasReachedEnd()) {
		if (gcount == 0) {
			// This is an empty file: VER1 DIR files can be completely empty, so we assume this one is
			// such a file.

			version = VER1;
			numEntries = 0;
			entries = new IMGEntry*[0];
			return;
		} else {
			throw IMGException("Premature end of file", __FILE__, __LINE__);
		}
	}

	char* fourCC = (char*) &firstBytes;

	if (fourCC[0] == 'V'  &&  fourCC[1] == 'E'  &&  fourCC[2] == 'R'  &&  fourCC[3] == '2') {
		version = VER2;
		stream->read((char*) &numEntries, 4);

		entries = new IMGEntry*[numEntries];

		for (int32_t i = 0 ; i < numEntries ; i++) {
			entries[i] = new IMGEntry;
			stream->read((char*) entries[i], sizeof(IMGEntry));
		}

		bytesRead = 8 + numEntries*sizeof(IMGEntry);
	} else {
		version = VER1;

		vector<IMGEntry*> entryVector;

		IMGEntry* firstEntry = new IMGEntry;
		firstEntry->offset = firstBytes;
		stream->read(((char*) firstEntry)+4, sizeof(IMGEntry)-4);
		entryVector.push_back(firstEntry);

		numEntries = 1;

		while (!stream->hasReachedEnd()) {
			IMGEntry* entry = new IMGEntry;
			stream->read((char*) entry, sizeof(IMGEntry));

			if (stream->getLastReadCount() == 0) {
				delete entry;
				break;
			}

			entryVector.push_back(entry);
			numEntries++;
		}

		entries = new IMGEntry*[numEntries];

		vector<IMGEntry*>::iterator it;
		int i = 0;
		for (it = entryVector.begin() ; it != entryVector.end() ; it++) {
			entries[i++] = *it;
		}

		bytesRead = 0;
	}

	for (int32_t i = 0 ; i < numEntries ; i++) {
		IMGEntry* entry = entries[i];

		if (entry->offset < 0) {
			printf("Last was %s\n", entries[i-1]->name);
			char* errmsg = new char[256];
			sprintf(errmsg, "Offset of entry #%d (%s) is < 0: %d!", i, entry->name, entry->offset);
			IMGException ex(errmsg, __FILE__, __LINE__);
			delete[] errmsg;
			throw ex;
		}
		if (entry->size < 0) {
			char* errmsg = new char[256];
			sprintf(errmsg, "Size of entry #%d (%s) is < 0: %d!", i, entry->name, entry->size);
			IMGException ex(errmsg, __FILE__, __LINE__);
			delete[] errmsg;
			throw ex;
		}
	}
}

InputStream* IMGArchive::openStream(const File& file)
{
	FileInputStream* stream = new FileInputStream(file, STREAM_BINARY);
	return stream;
}


void IMGArchive::reposition(int offset)
{
	bytesRead += offset;
}

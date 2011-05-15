/*
	Copyright 2010-2011 David "Alemarius Nexus" Lerch

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

	Additional permissions are granted, which are listed in the file
	GPLADDITIONS.
 */

#include "IMGArchive.h"
#include "IMGException.h"
#include <string>
#include <cstring>
#include <vector>
#include "../util/stream/RangedStream.h"
#include "../util/File.h"
#include <algorithm>
#include <cmath>

using std::iostream;
using std::string;
using std::vector;
using std::streamoff;
using std::sort;
using std::copy;
using std::min;

#define IMG_BUFFER_SIZE 4096


char _DummyBuffer[8192] = {0};

bool _EntrySortComparator(const IMGEntry* e1, const IMGEntry* e2)
{
	return e1->offset < e2->offset;
}





IMGArchive::IMGArchive(istream* stream, int mode)
		: imgStream(stream), dirStream(stream), mode(mode)
{
	readHeader();
}


IMGArchive::IMGArchive(const File& file, int mode)
		: mode(mode)
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

		if ((mode & ReadOnly)  !=  0) {
			dirStream = file.openInputStream(istream::binary);
			imgStream = imgFile.openInputStream(istream::binary);
		} else {
			dirStream = file.openInputOutputStream(iostream::binary);
			imgStream = file.openInputOutputStream(iostream::binary);
		}

		readHeader();
	} else if (type == CONTENT_TYPE_IMG) {
		if ((mode & ReadOnly)  !=  0) {
			imgStream = file.openInputStream(istream::binary);
		} else {
			imgStream = file.openInputOutputStream(iostream::binary);
		}

		if (guessIMGVersion(imgStream, true) == VER2) {
			dirStream = imgStream;
			readHeader();
		} else {
			char* dirFilePath = new char[strlen(path->toString())+1];
			int basePathLen = path->getExtension() - path->toString();
			strncpy(dirFilePath, path->toString(), basePathLen);
			strcpy(dirFilePath+basePathLen, "dir");
			File dirFile(dirFilePath);
			delete[] dirFilePath;

			if ((mode & ReadOnly)  !=  0) {
				dirStream = dirFile.openInputStream(istream::binary);
			} else {
				dirStream = dirFile.openInputOutputStream(istream::binary);
			}

			readHeader();
		}
	} else {
		throw IMGException("File name is neither an IMG nor a DIR file", __FILE__, __LINE__);
	}
}


IMGArchive::IMGArchive(istream* dirStream, istream* imgStream, int mode)
		: imgStream(imgStream), dirStream(dirStream), mode(mode)
{
	readHeader();
}


IMGArchive::IMGArchive(const File& dirFile, const File& imgFile, int mode)
		: mode(mode)
{
	if ((mode & ReadOnly)  !=  0) {
		imgStream = imgFile.openInputStream(istream::binary);
		dirStream = dirFile.openInputStream(istream::binary);
	} else {
		imgStream = imgFile.openInputOutputStream(iostream::binary);
		dirStream = dirFile.openInputOutputStream(iostream::binary);
	}

	readHeader();
}


IMGArchive::~IMGArchive()
{
	sync();

	EntryIterator it;
	for (it = entries.begin() ; it != entries.end() ; it++) {
		delete *it;
	}

	if ((mode & KeepStreamsOpen)  ==  0) {
		if (imgStream != dirStream) {
			delete imgStream;
			delete dirStream;
		} else {
			delete imgStream;
		}
	}
}


IMGArchive::IMGVersion IMGArchive::guessIMGVersion(istream* stream, bool returnToStart)
{
	char fourCC[4];
	stream->read(fourCC, 4);

	if (stream->eof()) {
		return VER1;
	}

	if (returnToStart) {
		stream->seekg(-4, istream::cur);
	}

	if (fourCC[0] == 'V'  &&  fourCC[1] == 'E'  &&  fourCC[2] == 'R'  &&  fourCC[3] == '2') {
		return VER2;
	} else {
		return VER1;
	}
}


IMGArchive::IMGVersion IMGArchive::guessIMGVersion(const File& file)
{
	//FileInputStream stream(file, STREAM_BINARY);
	istream* stream = file.openInputStream(istream::binary);
	IMGVersion ver = guessIMGVersion(stream, false);
	delete stream;
	return ver;
}


IMGArchive* IMGArchive::createArchive(iostream* imgStream, int mode)
{
	int32_t numEntries = 0;
	imgStream->write("VER2", 4);
	imgStream->write((char*) &numEntries, 4);
	imgStream->flush();
	imgStream->seekg(-8, iostream::cur);
	return new IMGArchive(imgStream, mode);
}


IMGArchive* IMGArchive::createArchive(iostream* dirStream, iostream* imgStream, int mode)
{
	// Nothing to be done. VER1 archives don't even have a header, so we don't have to write anything.
	return new IMGArchive(dirStream, imgStream, mode);
}


IMGArchive* IMGArchive::createArchive(const File& file, IMGVersion version, int mode)
{
	if (version == VER1) {
		if (file.guessContentType() == CONTENT_TYPE_DIR) {
			const char* dirPath = file.getPath()->toString();
			char* imgPath = new char[strlen(dirPath)+1];
			int baseLen = strlen(dirPath) - strlen(file.getPath()->getExtension());
			strncpy(imgPath, dirPath, baseLen);
			strcpy(imgPath+baseLen, "img");
			File imgFile(imgPath);
			delete[] imgPath;
			return createArchive(file.openInputOutputStream(iostream::binary),
					imgFile.openInputOutputStream(iostream::binary), mode);
		} else {
			const char* imgPath = file.getPath()->toString();
			char* dirPath = new char[strlen(imgPath)+1];
			int baseLen = strlen(imgPath) - strlen(file.getPath()->getExtension());
			strncpy(dirPath, imgPath, baseLen);
			strcpy(dirPath+baseLen, "dir");
			File dirFile(dirPath);
			delete[] dirPath;
			return createArchive(dirFile.openInputOutputStream(iostream::binary),
					file.openInputOutputStream(iostream::binary), mode);
		}
	} else {
		return createArchive(file.openInputOutputStream(iostream::binary), mode);
	}
}


IMGArchive* IMGArchive::createArchive(const File& dirFile, const File& imgFile, int mode)
{
	return createArchive(dirFile.openInputOutputStream(iostream::binary),
			imgFile.openInputOutputStream(iostream::binary), mode);
}


istream* IMGArchive::gotoEntry(ConstEntryIterator it, bool autoCloseStream)
{
	IMGEntry* entry = *it;
	long long start = entry->offset*IMG_BLOCK_SIZE;

	if ((mode & ReadOnly)  !=  0) {
		imgStream->seekg(start - imgStream->tellg(), istream::cur);
		RangedStream<istream>* rstream = new RangedStream<istream>(imgStream, entry->size*IMG_BLOCK_SIZE, autoCloseStream);
		return rstream;
	} else {
		printf("Going to write to %lld\n", start);
		iostream* outImgStream = static_cast<iostream*>(imgStream);
		imgStream->seekg(start - imgStream->tellg(), istream::cur);
		outImgStream->seekp(start - outImgStream->tellp(), fstream::cur);
		//printf("Frankophilia %lld\n", outImgStream->tellp());
		RangedStream<iostream>* rstream = new RangedStream<iostream>(outImgStream, entry->size*IMG_BLOCK_SIZE, autoCloseStream);
		return rstream;
	}
}


istream* IMGArchive::gotoEntry(const char* name, bool autoCloseStream)
{
	ConstEntryIterator it = getEntryByName(name);

	if (it == entries.end()) {
		return NULL;
	}

	return gotoEntry(it, autoCloseStream);
}


IMGArchive::ConstEntryIterator IMGArchive::getEntryByName(const char* name) const
{
	for (ConstEntryIterator it = entries.begin() ; it != entries.end() ; it++) {
		IMGEntry* entry = *it;
		if (strcmp(entry->name, name) == 0) {
			return it;
		}
	}

	return entries.end();
}


IMGArchive::EntryIterator IMGArchive::getEntryByName(const char* name)
{
	for (EntryIterator it = entries.begin() ; it != entries.end() ; it++) {
		IMGEntry* entry = *it;
		if (strcmp(entry->name, name) == 0) {
			return it;
		}
	}

	return entries.end();
}


void IMGArchive::readHeader()
{
	printf("READING IMG\n");

	istream* stream = dirStream;

	int32_t firstBytes;

	stream->read((char*) &firstBytes, 4);
	streamoff gcount = stream->gcount();

	if (stream->eof()) {
		if (gcount == 0) {
			// This is an empty file: VER1 DIR files can be completely empty, so we assume this one is
			// such a file.

			version = VER1;
			return;
		} else {
			throw IMGException("Premature end of file", __FILE__, __LINE__);
		}
	}

	char* fourCC = (char*) &firstBytes;

	vector<IMGEntry*> entryVector;

	if (fourCC[0] == 'V'  &&  fourCC[1] == 'E'  &&  fourCC[2] == 'R'  &&  fourCC[3] == '2') {
		version = VER2;
		int32_t numEntries;
		stream->read((char*) &numEntries, 4);

		for (int32_t i = 0 ; i < numEntries ; i++) {
			IMGEntry* entry = new IMGEntry;
			stream->read((char*) entry, sizeof(IMGEntry));
			entryVector.push_back(entry);
		}
	} else {
		version = VER1;

		// In VER1 'firstBytes' is actually part of the first entry, so we have to handle it manually.
		IMGEntry* firstEntry = new IMGEntry;
		firstEntry->offset = firstBytes;
		stream->read(((char*) firstEntry)+4, sizeof(IMGEntry)-4);
		entryVector.push_back(firstEntry);

		while (!stream->eof()) {
			IMGEntry* entry = new IMGEntry;
			stream->read((char*) entry, sizeof(IMGEntry));

			streamoff lrc = stream->gcount();

			if (lrc != sizeof(IMGEntry)) {
				if (lrc == 0) {
					delete entry;
					break;
				} else {
					char errmsg[128];
					sprintf(errmsg, "ERROR: Input isn't divided into %lu-byte blocks. Is this really a "
							"VER1 DIR file?", sizeof(IMGEntry));
					throw IMGException(errmsg, __FILE__, __LINE__);
				}
			}

			entryVector.push_back(entry);
		}
	}

	// TODO: All IMG files I've seen so far are already sorted by offset, but I'm not sure if this is
	// always true, so we don't take any risks here...
	sort(entryVector.begin(), entryVector.end(), _EntrySortComparator);
	entries = EntryList(entryVector.begin(), entryVector.end());

	if (entries.size() > 0) {
		headerReservedSpace = (*entries.begin())->offset;
	} else {
		headerReservedSpace = 1;
	}
}


int32_t IMGArchive::getHeaderReservedSize() const
{
	if (version == VER1) {
		// DIR files don't have something as a reserved size.
		return -1;
	} else {
		if (entries.size() > 0) {
			return (*entries.begin())->offset;
		} else {
			return -1;
		}
	}
}


bool IMGArchive::reserveHeaderSpace(int32_t numHeaders)
{
	if ((mode & ReadWrite)  ==  0) {
		throw IMGException("Attempt to modify a read-only IMG archive!", __FILE__, __LINE__);
	}

	int64_t bytesToReserve = numHeaders * sizeof(IMGEntry);

	if (version == VER2) {
		// Consider the IMG header for VER2
		bytesToReserve += 8;
	}

	int64_t reservedSize = getHeaderReservedSize();

	headerReservedSpace = ceil((float) bytesToReserve / IMG_BLOCK_SIZE);

	if (reservedSize == -1) {
		// VER1 or empty VER2 archive. In both cases, there's nothing to reserve.
		return true;
	}

	reservedSize *= IMG_BLOCK_SIZE; // reservedSize shall be in bytes

	if (bytesToReserve <= reservedSize) {
		return true;
	}

	// This has to be a VER2 archive, because reservedSize would be -1 for VER1 archives.

	EntryIterator moveBegin = entries.begin();
	EntryIterator moveEnd; // One PAST the last entry to be moved
	int32_t newOffset;

	// Find out how many entries we have to move
	for (moveEnd = entries.begin() ; moveEnd != entries.end() ; moveEnd++) {
		IMGEntry* entry = *moveEnd;
		if (entry->offset*IMG_BLOCK_SIZE >= bytesToReserve) {
			break;
		}
	}

	if (moveEnd == entries.end()) {
		// Handle the case where the new header size is greater than the size of the whole archive.
		newOffset = ceil((float) bytesToReserve / IMG_BLOCK_SIZE);
	} else {
		EntryIterator last = --entries.end();
		IMGEntry* lastEntry = *last;
		newOffset = lastEntry->offset + lastEntry->size;
	}

	EntryIterator lastToBeMoved = moveEnd;
	lastToBeMoved--;
	IMGEntry* moveBeginEntry = *moveBegin;
	IMGEntry* lastMoveEntry = *lastToBeMoved;

	iostream* outImgStream = static_cast<iostream*>(imgStream);

	// Move the contents of all entries between moveBegin and moveEnd to newOffset
	int32_t moveCount = (lastMoveEntry->offset + lastMoveEntry->size) - moveBeginEntry->offset;
	imgStream->seekg(moveBeginEntry->offset*IMG_BLOCK_SIZE, istream::beg);
	char* data = new char[moveCount * IMG_BLOCK_SIZE];
	imgStream->read(data, moveCount * IMG_BLOCK_SIZE);

	outImgStream->seekp(newOffset*IMG_BLOCK_SIZE, ostream::beg);
	outImgStream->write(data, moveCount*IMG_BLOCK_SIZE);

	// The offset by which all moved entries have been moved.
	int32_t relativeOffset = newOffset - moveBeginEntry->offset;

	// Update the offsets in the header
	for (EntryIterator it = moveBegin ; it != moveEnd ; it++) {
		IMGEntry* entry = *it;
		entry->offset += relativeOffset;
	}

	// And move them in the 'entries' list
	// Note that after calling entries.insert(), all iterators still point to the same elements they pointed
	// to before. However, when moveEnd points to entries.end(), it should afterwards still point to the
	// end of the moved-range, and not the end of the whole list, so we let oldMoveEnd point to the last moved
	// entry, and afterwards increment it again to point to the old location of moveEnd.
	EntryIterator oldMoveEnd = moveEnd;
	oldMoveEnd--;
	entries.insert(entries.end(), moveBegin, moveEnd);
	oldMoveEnd++;
	entries.erase(moveBegin, oldMoveEnd);

	expandSize();

	return true;
}


void IMGArchive::rewriteHeaderSection()
{
	if ((mode & ReadWrite)  ==  0) {
		throw IMGException("Attempt to modify a read-only IMG archive!", __FILE__, __LINE__);
	}

	iostream* outImgStream = static_cast<iostream*>(imgStream);

	if (version == VER1) {
		outImgStream->seekp(0 - outImgStream->tellp(), ostream::cur);
	} else {
		outImgStream->seekp(4 - outImgStream->tellp(), ostream::cur);
		int32_t numEntries = entries.size();
		outImgStream->write((char*) &numEntries, 4);
	}

	// Write the header section
	EntryIterator it;
	for (it = entries.begin() ; it != entries.end() ; it++) {
		outImgStream->write((char*) *it, sizeof(IMGEntry));
	}
}


bool IMGArchive::addEntries(IMGEntry** newEntries, int32_t num)
{
	if ((mode & ReadWrite)  ==  0) {
		throw IMGException("Attempt to modify a read-only IMG archive!", __FILE__, __LINE__);
	}

	int32_t left = getReservedHeadersLeft();
	if (left != -1  &&  left < num) {
		return false;
	}

	int32_t offset;

	if (entries.size() > 0) {
		EntryIterator last = --entries.end();
		IMGEntry* lastEntry = *last;
		offset = lastEntry->offset + lastEntry->size;
	} else {
		offset = headerReservedSpace;
	}

	for (int32_t i = 0 ; i < num ; i++) {
		IMGEntry* entry = newEntries[i];
		entry->offset = offset;
		offset += entry->size;
		entries.push_back(entry);
	}

	expandSize();

	return true;
}


IMGArchive::EntryIterator IMGArchive::addEntry(const char* name, int32_t size)
{
	if ((mode & ReadWrite)  ==  0) {
		throw IMGException("Attempt to modify a read-only IMG archive!", __FILE__, __LINE__);
	}
	if (strlen(name) > 23) {
		throw IMGException("Maximum length of 23 characters for IMG entry names exceeded!",
				__FILE__, __LINE__);
	}

	IMGEntry* entry = new IMGEntry;
	strcpy(entry->name, name);
	entry->size = size;
	bool result = addEntries(&entry, 1);

	if (!result) {
		delete entry;
		return entries.end();
	}

	return getEntryByName(name);
}


void IMGArchive::resizeEntry(EntryIterator it, int32_t size)
{
	if ((mode & ReadWrite)  ==  0) {
		throw IMGException("Attempt to modify a read-only IMG archive!", __FILE__, __LINE__);
	}

	iostream* outImgStream = static_cast<iostream*>(imgStream);

	IMGEntry* entry = *it;

	ConstEntryIterator next = it;
	next++;

	if (next == entries.end()) {
		// Nothing special has to be done to resize the last entry
		entry->size = size;
		expandSize();
		return;
	}

	IMGEntry* nextEntry = *next;

	if (nextEntry->offset-entry->offset < size) {
		// Too few space left, so we have to move the entry to the end
		int32_t previousEnd = getDataEndOffset();
		imgStream->seekg(entry->offset*IMG_BLOCK_SIZE - imgStream->tellg(), istream::cur);
		char* data = new char[entry->size*IMG_BLOCK_SIZE];
		imgStream->read(data, entry->size*IMG_BLOCK_SIZE);

		outImgStream->seekp(getDataEndOffset()*IMG_BLOCK_SIZE - outImgStream->tellp(), fstream::cur);
		outImgStream->write(data, entry->size*IMG_BLOCK_SIZE);
		entries.erase(it);
		entries.push_back(entry);

		entry->size = size;
		entry->offset = previousEnd;

		expandSize();
	} else {
		// Enough space left after the entry, so there's nothing special to be done
		entry->size = size;
	}
}


void IMGArchive::removeEntry(EntryIterator it)
{
	if ((mode & ReadWrite)  ==  0) {
		throw IMGException("Attempt to modify a read-only IMG archive!", __FILE__, __LINE__);
	}

	delete *it;
	entries.erase(it);
}


void IMGArchive::renameEntry(EntryIterator it, const char* name)
{
	if ((mode & ReadWrite)  ==  0) {
		throw IMGException("Attempt to modify a read-only IMG archive!", __FILE__, __LINE__);
	}
	if (strlen(name) > 23) {
		throw IMGException("Maximum length of 23 characters for IMG entry names exceeded!",
				__FILE__, __LINE__);
	}

	IMGEntry* entry = *it;
	strcpy(entry->name, name);
}


void IMGArchive::expandSize(int32_t size)
{
	// Internal method, so no need to check for ReadWrite
	iostream* outImgStream = static_cast<iostream*>(imgStream);
	outImgStream->seekp(0, iostream::end);
	streamoff curSize = outImgStream->tellp();
	int bytesToAdd = size*IMG_BLOCK_SIZE - curSize;

	while (bytesToAdd != 0) {
		int numBytes = min(bytesToAdd, (int) sizeof(_DummyBuffer));
		outImgStream->write(_DummyBuffer, numBytes);
		bytesToAdd -= numBytes;
	}
}


void IMGArchive::expandSize()
{
	// Internal method, so no need to check for ReadWrite
	if (entries.size() > 0) {
		EntryIterator last = --entries.end();
		IMGEntry* entry = *last;
		expandSize(entry->offset + entry->size);
	}
}


int32_t IMGArchive::pack()
{
	if ((mode & ReadWrite)  ==  0) {
		throw IMGException("Attempt to modify a read-only IMG archive!", __FILE__, __LINE__);
	}

	// First, we create a copy of this IMGArchive
	File copyFile = File::createTemporaryFile();
	imgStream->seekg(0, istream::beg);
	copyFile.copyFrom(imgStream);

	// And a copy of it's entries
	IMGEntry** oldEntries = new IMGEntry*[entries.size()];
	EntryIterator it;
	int i = 0;
	for (it = entries.begin() ; it != entries.end() ; it++, i++) {
		oldEntries[i] = new IMGEntry;
		memcpy(oldEntries[i], *it, sizeof(IMGEntry));
	}

	// We calculate the minimum size needed for the header section
	int32_t headerSize = ceil((float) (entries.size() * sizeof(IMGEntry)) / IMG_BLOCK_SIZE);

	// Then we rebuild the header section
	int32_t offset = headerSize;
	for (it = entries.begin() ; it != entries.end() ; it++) {
		IMGEntry* entry = *it;
		entry->offset = offset;
		offset += entry->size;
	}

	// And now we reassign the content to each entry, reading it from the copy file.
	iostream* outImgStream = static_cast<iostream*>(imgStream);
	outImgStream->seekp(headerSize*IMG_BLOCK_SIZE, ostream::beg);
	istream* oldImgStream = copyFile.openInputOutputStream(istream::binary);
	for (i = 0, it = entries.begin() ; it != entries.end() ; it++, i++) {
		// We don't have to reposition outImgStream, because the new entries are tightly packed.
		IMGEntry* oldEntry = oldEntries[i];
		oldImgStream->seekg(oldEntry->offset*IMG_BLOCK_SIZE, istream::beg);
		char buf[4096];
		int bytesToRead = oldEntry->size*IMG_BLOCK_SIZE;

		while (bytesToRead != 0) {
			int numBytes = min(bytesToRead, (int) sizeof(buf));
			oldImgStream->read(buf, numBytes);
			outImgStream->write(buf, numBytes);
			bytesToRead -= numBytes;
		}
	}
	delete oldImgStream;

	// The archive is now successfully packed. Clean up
	delete[] oldEntries;
	copyFile.remove();

	return getDataEndOffset();
}



/*
	Copyright 2010-2014 David "Alemarius Nexus" Lerch

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
#include <nxcommon/file/File.h>
#include <nxcommon/stream/RangedStream.h>
#include <nxcommon/util.h>
#include <algorithm>
#include <cmath>
#include <utility>
#include <iterator>
#include <nxcommon/stream/streamutil.h>
#include <nxcommon/stream/StreamReader.h>
#include <nxcommon/stream/EndianSwappingStreamReader.h>
#include <nxcommon/stream/StreamWriter.h>
#include <nxcommon/stream/EndianSwappingStreamWriter.h>

using std::iostream;
using std::string;
using std::vector;
using std::streamoff;
using std::sort;
using std::copy;
using std::min;
using std::pair;
using std::distance;

#define IMG_BUFFER_SIZE 4096

// Maximum number of blocks to be kept in memory
#define IMG_MAX_RAM_BLOCKS 25000 // ~ 51.2MB


shared_ptr<int> ptr;

char _DummyBuffer[8192] = {0};

bool _EntrySortComparator(const IMGEntry& e1, const IMGEntry& e2)
{
	return e1.offset < e2.offset;
}





IMGArchive::IMGArchive(istream* stream, int mode)
		: imgStream(stream), dirStream(stream), mode(mode)
{
	readHeader();
}


IMGArchive::IMGArchive(const File& file, int mode)
		: mode(mode)
{
	FilePath path = file.getPath();
	FileContentType type = file.guessContentType();

	if (type == CONTENT_TYPE_DIR) {
		File imgFile(path.getDirectoryPath(), path.getFullBaseFileName().append(".img"));

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
			File dirFile(path.getDirectoryPath(), path.getFullBaseFileName().append(".dir"));

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

	if (imgStream != dirStream) {
		delete imgStream;
		delete dirStream;
	} else {
		delete imgStream;
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
	istream* stream = file.openInputStream(istream::binary);
	IMGVersion ver = guessIMGVersion(stream, false);
	delete stream;
	return ver;
}


IMGArchive* IMGArchive::createArchive(iostream* imgStream, int mode)
{
#ifdef GTAFORMATS_LITTLE_ENDIAN
	StreamWriter writer(imgStream);
#else
	EndianSwappingStreamWriter writer(imgStream);
#endif

	uint32_t numEntries = 0;
	imgStream->write("VER2", 4);
	writer.writeU32(numEntries);
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
			File imgFile(file.getPath().getDirectoryPath(), file.getFullBaseFileName().append(".img"));

			return createArchive(file.openInputOutputStream(iostream::binary),
					imgFile.openInputOutputStream(iostream::binary), mode);
		} else {
			File dirFile(file.getPath().getDirectoryPath(), file.getFullBaseFileName().append(".dir"));

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


istream* IMGArchive::gotoEntry(const ConstEntryIterator& it)
{
	const IMGEntry& entry = *it;
	uint64_t start = IMG_BLOCKS2BYTES(entry.offset);

	if ((mode & ReadOnly)  !=  0) {
		imgStream->seekg(start - imgStream->tellg(), istream::cur);
		RangedStream<istream>* rstream = new RangedStream<istream>(imgStream,
				IMG_BLOCKS2BYTES(entry.size));
		return rstream;
	} else {
		iostream* outImgStream = static_cast<iostream*>(imgStream);
		imgStream->seekg(start - imgStream->tellg(), istream::cur);
		outImgStream->seekp(start - outImgStream->tellp(), fstream::cur);
		RangedStream<iostream>* rstream = new RangedStream<iostream>(outImgStream,
				IMG_BLOCKS2BYTES(entry.size));
		return rstream;
	}
}


istream* IMGArchive::gotoEntry(const CString& name)
{
	ConstEntryIterator it = getEntryByName(name);

	if (it == entries.end()) {
		return NULL;
	}

	return gotoEntry(it);
}


IMGArchive::ConstEntryIterator IMGArchive::getEntryByName(const CString& name) const
{
	EntryMap::const_iterator it = entryMap.find(CString(name).lower());

	if (it == entryMap.end()) {
		return entries.end();
	}

	return it->second;
}


IMGArchive::EntryIterator IMGArchive::getEntryByName(const CString& name)
{
	EntryMap::iterator it = entryMap.find(CString(name).lower());

	if (it == entryMap.end()) {
		return entries.end();
	}

	return it->second;
}


void IMGArchive::readHeader()
{
	istream* stream = dirStream;

#ifdef GTAFORMATS_LITTLE_ENDIAN
	StreamReader reader(stream);
#else
	EndianSwappingStreamReader reader(stream);
#endif

	uint32_t firstBytes;

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

	entries.clear();

	// Offset of the last entry that was read. This is kept to see if the archive is sorted.
	// Most of the time (always?), IMG archives are sorted. If we know that, we can save some time because we won't have to
	// sort later on.
	uint32_t lastOffset = 0;

	bool sorted = true;

	char* fourCC = (char*) &firstBytes;

	if (fourCC[0] == 'V'  &&  fourCC[1] == 'E'  &&  fourCC[2] == 'R'  &&  fourCC[3] == '2') {
		version = VER2;
		int32_t numEntries = reader.readU32();

		for (int32_t i = 0 ; i < numEntries ; i++) {
			IMGEntry entry;
			stream->read((char*) &entry, sizeof(IMGEntry));

#ifndef GTAFORMATS_LITTLE_ENDIAN
			entry.offset = SwapEndianness32(entry.offset);
			entry.size = SwapEndianness32(entry.size);
#endif

			if (entry.offset < lastOffset) {
				sorted = false;
			}

			lastOffset = entry.offset;

			entries.push_back(entry);

			entryMap.insert(pair<CString, EntryIterator>(CString(entry.name).lower(), --entries.end()));
		}
	} else {
		version = VER1;

		// In VER1 'firstBytes' is actually part of the first entry, so we have to handle it manually.
		IMGEntry firstEntry;

		firstEntry.offset = ToLittleEndian32(firstBytes);

		stream->read(((char*) &firstEntry)+4, sizeof(IMGEntry)-4);

#ifndef GTAFORMATS_LITTLE_ENDIAN
		firstEntry.size = SwapEndianness32(firstEntry.size);
#endif

		entries.push_back(firstEntry);

		while (!stream->eof()) {
			IMGEntry entry;
			stream->read((char*) &entry, sizeof(IMGEntry));

#ifndef GTAFORMATS_LITTLE_ENDIAN
			entry.offset = SwapEndianness32(entry.offset);
			entry.size = SwapEndianness32(entry.size);
#endif

			if (entry.offset < lastOffset) {
				sorted = false;
			}

			lastOffset = entry.offset;

			streamoff lrc = stream->gcount();

			if (lrc != sizeof(IMGEntry)) {
				if (lrc == 0) {
					break;
				} else {
					char errmsg[128];
					sprintf(errmsg, "ERROR: Input isn't divided into %u-byte blocks. Is this really a "
							"VER1 DIR file?", (unsigned int) sizeof(IMGEntry));
					throw IMGException(errmsg, __FILE__, __LINE__);
				}
			}

			entries.push_back(entry);

			entryMap.insert(pair<CString, EntryIterator>(CString(entry.name).lower(), --entries.end()));
		}
	}

	// TODO: All IMG files I've seen so far are already sorted by offset, but I'm not sure if this is
	// always true, so we don't take any risks here...
	if (!sorted) {
		entries.sort(_EntrySortComparator);
	}

	if (entries.size() > 0) {
		headerReservedSpace = entries.begin()->offset;
	} else {
		headerReservedSpace = 1;
	}
}


uint32_t IMGArchive::getHeaderReservedSize() const
{
	if (version == VER1) {
		// DIR files don't have something as a reserved size.
		return 0;
	} else {
		if (entries.size() > 0) {
			return entries.begin()->offset;
		} else {
			return 0;
		}
	}
}


void IMGArchive::forceMoveEntries(const EntryIterator& pos,
		const EntryIterator& moveBegin, const EntryIterator& moveEnd, uint32_t newOffset)
{
	if (moveBegin == moveEnd)
		return;

	EntryIterator lastToBeMoved = moveEnd;
	lastToBeMoved--;
	IMGEntry& moveBeginEntry = *moveBegin;
	IMGEntry& lastMoveEntry = *lastToBeMoved;

	iostream* outImgStream = static_cast<iostream*>(imgStream);

	// Move the contents of all entries between moveBegin and moveEnd to newOffset
	uint32_t moveCount = (lastMoveEntry.offset + lastMoveEntry.size) - moveBeginEntry.offset;

	imgStream->seekg(IMG_BLOCKS2BYTES(moveBeginEntry.offset), istream::beg);

	StreamMove(outImgStream, IMG_BLOCKS2BYTES(moveCount), IMG_BLOCKS2BYTES(newOffset));

	// The offset by which all moved entries have been moved.
	int64_t relativeOffset = newOffset - moveBeginEntry.offset;

	size_t numMoved = 0;

	// Update the offsets in the header
	for (IMGEntry& entry : ItRange(moveBegin, moveEnd)) {
		entry.offset += relativeOffset;
		numMoved++;
	}

	// And move them in the 'entries' list
	// Note that after calling entries.insert(), all iterators still point to the same elements they pointed
	// to before. However, when moveEnd points to entries.end(), it should afterwards still point to the
	// end of the moved-range, and not the end of the whole list, so we let oldMoveEnd point to the last moved
	// entry, and afterwards increment it again to point to the old location of moveEnd.
	EntryIterator oldMoveEnd = moveEnd;
	oldMoveEnd--;
	entries.insert(pos, moveBegin, moveEnd);
	oldMoveEnd++;

	entries.erase(moveBegin, oldMoveEnd);

	// Update the entry map. Note that the only valid iterator we have here is 'pos'.
	EntryIterator it = pos;
	for (size_t i = 0 ; i < numMoved ; i++) {
		it--;
		IMGEntry& entry = *it;
		entryMap[CString(entry.name).lower()] = it;
	}
}


void IMGArchive::moveEntries(const EntryIterator& ppos,
		const EntryIterator& moveBegin, const EntryIterator& moveEnd, MoveMode mmode)
{
	if (moveBegin == moveEnd)
		return;

	// Check if pos is between moveBegin and moveEnd. In this case, there's nothing to be moved.
	for (EntryIterator it = moveBegin ; it != moveEnd ; it++) {
		if (it == ppos)
			return;
	}
	if (ppos == moveEnd)
		return;

	// Now we know that pos is either before or after the move range.

	EntryIterator pos = ppos;

	if (mmode == MoveToEnd) {
		// We move enough entries following the insertion position to the end of the archive.

		uint32_t newStartOffset; // Offset of the insertion position

		if (pos == entries.begin()) {
			newStartOffset = pos->offset;
		} else {
			// There might be some holes before endMoveBegin. If so, we can fill them with this move
			// operation.
			EntryIterator beforePos = pos;
			beforePos--;
			newStartOffset = beforePos->offset + beforePos->size;
		}

		if (pos != entries.end()) {
			// If the entries are to be inserted somewhere in the middle of the archive, we first have to
			// make place for them...

			uint32_t moveBeginOffs = moveBegin->offset;

			EntryIterator last = moveEnd;
			last--;
			uint32_t moveEndOffs = last->offset + last->size;

			uint32_t moveSize = moveEndOffs - moveBeginOffs; // Number of blocks to be moved

			EntryIterator endMoveBegin = pos; // First entry to be moved to the end
			EntryIterator endMoveEnd; // One PAST the last entry to be moved to the end
			uint32_t endMoveBeginOffset = newStartOffset;

			for (	endMoveEnd = pos
					;
					endMoveEnd != entries.end() // Stop if the archive end is reached
					&&  endMoveEnd != moveBegin // Stop if the move range is reached
					&&  endMoveEnd->offset - endMoveBeginOffset < moveSize
							// Stop if enough space is available between endMoveBegin and endMoveEnd
					;
					endMoveEnd++
			) {}

			uint32_t endOffset = getDataEndOffset();

			// We might need to move the entries even futher than the current archive end
			if (endMoveEnd == entries.end()) {
				endOffset += moveSize - (endOffset - endMoveBeginOffset);
			}

			pos = endMoveEnd;

			// Now move the entries to the end (and maybe futher)
			forceMoveEntries(entries.end(), endMoveBegin, endMoveEnd, endOffset);
		}

		// Now we can be sure that there is enough space left after the insertion position.

		if (pos != moveBegin) {
			// And finally, perform the actual requested move
			forceMoveEntries(pos, moveBegin, moveEnd, newStartOffset);
		}

		expandSize();
	} else {
		// We create two consecutive ranges between moveBegin, moveEnd and pos, and swap them.

		bool moveForward = false;

		// Determine whether pos is before or after the move range (unfortunately, with bidi iterators,
		// there's no better way to do this)
		for (EntryIterator it = entries.begin() ;; it++) {
			if (it == pos) {
				moveForward = false;
				break;
			} else if (it == moveBegin) {
				moveForward = true;
				break;
			}
		}

		if (moveForward) {
			swapConsecutive(moveBegin, moveEnd, pos);
		} else {
			swapConsecutive(pos, moveBegin, moveEnd);
		}
	}
}


bool IMGArchive::reserveHeaderSpace(uint32_t numHeaders)
{
	if ((mode & ReadWrite)  ==  0) {
		throw IMGException("Attempt to modify a read-only IMG archive!", __FILE__, __LINE__);
	}

	uint64_t bytesToReserve = numHeaders * sizeof(IMGEntry);

	if (version == VER2) {
		// Consider the IMG header for VER2
		bytesToReserve += 8;
	}

	uint64_t reservedSize = IMG_BLOCKS2BYTES(getHeaderReservedSize()); // reservedSize shall be in bytes

	headerReservedSpace = IMG_BYTES2BLOCKS(bytesToReserve);

	if (reservedSize == 0  ||  bytesToReserve <= reservedSize) {
		return true;
	}

	// This has to be a VER2 archive, because reservedSize would be UINT32_MAX for VER1 archives.

	EntryIterator moveBegin = entries.begin();
	EntryIterator moveEnd; // One PAST the last entry to be moved

	// Find out how many entries we have to move
	for (moveEnd = entries.begin() ; moveEnd != entries.end() ; moveEnd++) {
		IMGEntry& entry = *moveEnd;
		if (IMG_BLOCKS2BYTES(entry.offset) >= bytesToReserve) {
			break;
		}
	}

	moveEntries(entries.end(), moveBegin, moveEnd);

	return true;
}


void IMGArchive::rewriteHeaderSection()
{
	if ((mode & ReadWrite)  ==  0) {
		throw IMGException("Attempt to modify a read-only IMG archive!", __FILE__, __LINE__);
	}

	iostream* outImgStream = static_cast<iostream*>(imgStream);

#ifdef GTAFORMATS_LITTLE_ENDIAN
	StreamWriter writer(outImgStream);
#else
	EndianSwappingStreamWriter writer(outImgStream);
#endif

	if (version == VER1) {
		outImgStream->seekp(0 - outImgStream->tellp(), ostream::cur);
	} else {
		outImgStream->seekp(4 - outImgStream->tellp(), ostream::cur);
		uint32_t numEntries = entries.size();
		writer.writeU32(numEntries);
	}

	// Write the header section
	EntryIterator it;
	for (it = entries.begin() ; it != entries.end() ; it++) {
#ifdef GTAFORMATS_LITTLE_ENDIAN
		outImgStream->write((char*) &(*it), sizeof(IMGEntry));
#else
		writer.writeArrayCopyU32((uint32_t*) &(*it), 2);
		outImgStream->write(((char*) &(*it)) + 8, sizeof(IMGEntry)-8);
#endif
	}
}


/*bool IMGArchive::addEntries(IMGEntry* newEntries, uint32_t num)
{
	if ((mode & ReadWrite)  ==  0) {
		throw IMGException("Attempt to modify a read-only IMG archive!", __FILE__, __LINE__);
	}

	uint32_t left = getReservedHeadersLeft();
	if (left != 0  &&  left < num) {
		return false;
	}

	uint32_t offset = getDataEndOffset();

	for (uint32_t i = 0 ; i < num ; i++) {
		IMGEntry& entry = newEntries[i];
		entry.offset = offset;
		offset += entry.size;

		entries.push_back(entry);

		if ((mode & DisableNameZeroFill) == 0) {
			size_t len = strlen(entry.name);
			memset(entry.name + len + 1, 0, 23 - len);
		}

		entryMap[CString(entry.name).lower()] = --entries.end();
	}

	expandSize();

	return true;
}*/


IMGArchive::EntryIterator IMGArchive::doAddEntry(const CString& name, uint32_t size)
{
	if ((mode & ReadWrite)  ==  0) {
		throw IMGException("Attempt to modify a read-only IMG archive!", __FILE__, __LINE__);
	}
	if (strlen(name.get()) > 23) {
		throw IMGException("Maximum length of 23 characters for IMG entry names exceeded!",
				__FILE__, __LINE__);
	}

	uint32_t offset = getDataEndOffset();

	IMGEntry entry;
	size_t nameLen = strlen(name.get());
	memcpy(entry.name, name.get(), nameLen+1);
	entry.name[nameLen] = '\0';

	if ((mode & DisableNameZeroFill) == 0) {
		memset(entry.name + nameLen + 1, 0, 23 - nameLen);
	}

	entry.size = size;
	entry.offset = offset;

	entries.push_back(entry);

	if ((mode & DisableNameZeroFill) == 0) {
		size_t len = strlen(entry.name);
		memset(entry.name + len + 1, 0, 23 - len);
	}

	EntryIterator it = --entries.end();
	entryMap[CString(entry.name).lower()] = it;

	expandSize();

	return it;
}


IMGArchive::EntryIterator IMGArchive::addEntry(const CString& name, uint32_t size)
{
	if (!reserveHeaderSpace(entries.size() + 1)) {
		return entries.end();
	}

	return doAddEntry(name, size);
}


void IMGArchive::resizeEntry(const EntryIterator& it, uint32_t size)
{
	if ((mode & ReadWrite)  ==  0) {
		throw IMGException("Attempt to modify a read-only IMG archive!", __FILE__, __LINE__);
	}
	if (it == entries.end()) {
		throw IMGException("resizeEntry() called with the end iterator!", __FILE__, __LINE__);
	}

	IMGEntry& entry = *it;

	EntryIterator next = it;
	next++;

	if (next == --entries.end()) {
		// Nothing special has to be done to resize the last entry
		entry.size = size;
		expandSize();
		return;
	}

	IMGEntry& nextEntry = *next;

	if (nextEntry.offset-entry.offset < size) {
		// Too few space left, so we have to move the entry to the end
		moveEntries(entries.end(), it, next);

		// The entry was moved, so we must get its new reference.
		IMGEntry& newEntry = *(--entries.end());

		newEntry.size = size;

		expandSize();
	} else {
		// Enough space left after the entry, so there's nothing special to be done
		entry.size = size;
	}
}


void IMGArchive::removeEntry(const EntryIterator& it)
{
	if ((mode & ReadWrite)  ==  0) {
		throw IMGException("Attempt to modify a read-only IMG archive!", __FILE__, __LINE__);
	}

	if (it == entries.end()) {
		return;
	}

	entryMap.erase(CString(it->name).lower());
	entries.erase(it);
}


void IMGArchive::renameEntry(const EntryIterator& it, const CString& name)
{
	if ((mode & ReadWrite)  ==  0) {
		throw IMGException("Attempt to modify a read-only IMG archive!", __FILE__, __LINE__);
	}
	if (strlen(name.get()) > 23) {
		throw IMGException("Maximum length of 23 characters for IMG entry names exceeded!",
				__FILE__, __LINE__);
	}
	if (it == entries.end()) {
		throw IMGException("renameEntry() called with the end iterator!", __FILE__, __LINE__);
	}

	IMGEntry& entry = *it;

	size_t numRem = entryMap.erase(CString(entry.name).lower());

	strcpy(entry.name, name.get());

	if ((mode & DisableNameZeroFill) == 0) {
		size_t len = strlen(entry.name);
		memset(entry.name + len + 1, 0, 23 - len);
	}

	entryMap[CString(name).lower()] = it;
}


void IMGArchive::renameEntry(const CString& oldName, const CString& newName)
{
	EntryIterator it = getEntryByName(oldName);

	if (it == entries.end()) {
		throw IMGException(CString("No entry found with name ") << oldName, __FILE__, __LINE__);
	}

	renameEntry(it, newName);
}


void IMGArchive::expandSize(uint32_t size)
{
	// Internal method, so no need to check for ReadWrite
	iostream* outImgStream = static_cast<iostream*>(imgStream);
	outImgStream->seekp(0, iostream::end);
	streamoff curSize = outImgStream->tellp();
	int64_t bytesToAdd = IMG_BLOCKS2BYTES(size) - curSize;

	while (bytesToAdd > 0) {
		int64_t numBytes = min(bytesToAdd, (int64_t) sizeof(_DummyBuffer));
		outImgStream->write(_DummyBuffer, numBytes);
		bytesToAdd -= numBytes;
	}
}


void IMGArchive::expandSize()
{
	// Internal method, so no need to check for ReadWrite
	if (entries.size() > 0) {
		EntryIterator last = --entries.end();
		IMGEntry& entry = *last;
		expandSize(entry.offset + entry.size);
	}
}


uint32_t IMGArchive::pack()
{
	if ((mode & ReadWrite)  ==  0) {
		throw IMGException("Attempt to modify a read-only IMG archive!", __FILE__, __LINE__);
	}

	// First, we create a copy of this IMGArchive
	File copyFile = File::createTemporaryFile();
	imgStream->seekg(0, istream::beg);
	copyFile.copyFrom(imgStream);

	// And a copy of it's entries
	IMGEntry* oldEntries = new IMGEntry[entries.size()];
	EntryIterator it;
	size_t i = 0;
	for (it = entries.begin() ; it != entries.end() ; it++, i++) {
		memcpy(oldEntries+i, &(*it), sizeof(IMGEntry));
	}

	// We calculate the minimum size needed for the header section
	uint32_t headerSize = IMG_BYTES2BLOCKS(entries.size() * sizeof(IMGEntry));

	// Then we rebuild the header section
	uint32_t offset = headerSize;
	for (it = entries.begin() ; it != entries.end() ; it++) {
		IMGEntry& entry = *it;
		entry.offset = offset;
		offset += entry.size;
	}

	// And now we reassign the content to each entry, reading it from the copy file.
	iostream* outImgStream = static_cast<iostream*>(imgStream);
	outImgStream->seekp(IMG_BLOCKS2BYTES(headerSize), ostream::beg);
	istream* oldImgStream = copyFile.openInputOutputStream(istream::binary);
	for (i = 0, it = entries.begin() ; it != entries.end() ; it++, i++) {
		// We don't have to reposition outImgStream, because the new entries are tightly packed.
		IMGEntry& oldEntry = oldEntries[i];
		oldImgStream->seekg(IMG_BLOCKS2BYTES(oldEntry.offset), istream::beg);
		char buf[4096];
		int64_t bytesToRead = (int) IMG_BLOCKS2BYTES(oldEntry.size);

		while (bytesToRead != 0) {
			int64_t numBytes = min(bytesToRead, (int64_t) sizeof(buf));
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


void IMGArchive::swapConsecutive (
		const EntryIterator& r1Begin, const EntryIterator& pr1End, const EntryIterator& r2End)
{
	if (r1Begin == pr1End  ||  pr1End == r2End)
		return;

	EntryIterator r1End = pr1End;

	iostream* outImgStream = static_cast<iostream*>(imgStream);

	EntryIterator r1BeforeBegin = r1Begin;
	bool r1BeginIsFirst;

	if (r1Begin != entries.begin()) {
		r1BeginIsFirst = false;
		r1BeforeBegin--;
	} else {
		r1BeginIsFirst = true;
	}

	EntryIterator r1Last = r1End;
	EntryIterator r2Last = r2End;
	r1Last--;
	r2Last--;

	uint32_t r1Size = r1Last->offset+r1Last->size - r1Begin->offset;
	uint32_t r2Size = r2Last->offset+r2Last->size - r1End->offset;

	uint32_t backupSize; // In IMG blocks

	// For swapping, we need to store a copy of the contents of one range. We choose the smaller one.
	if (r1Size < r2Size) {
		imgStream->seekg(IMG_BLOCKS2BYTES(r1Begin->offset) - imgStream->tellg(), istream::cur);
		backupSize = r1Size;
	} else {
		imgStream->seekg(IMG_BLOCKS2BYTES(r1End->offset) - imgStream->tellg(), istream::cur);
		backupSize = r2Size;
	}

	uint8_t* backup;
	File tmpFile;

	if (backupSize <= IMG_MAX_RAM_BLOCKS) {
		// Read the backup completely into memory.

		backup = new uint8_t[IMG_BLOCKS2BYTES(backupSize)];
		imgStream->read((char*) backup, IMG_BLOCKS2BYTES(backupSize));
	} else {
		// Write the backup to a temporary file.

		tmpFile = File::createTemporaryFile();
		ostream* out = tmpFile.openOutputStream(ostream::binary);

		char buf[IMG_BLOCK_SIZE];

		for (uint32_t i = 0 ; i < backupSize ; i++) {
			imgStream->read(buf, sizeof(buf));
			out->write(buf, sizeof(buf));

			if (out->fail()) {
				delete out;
				tmpFile.remove();

				IOException ex("IO error writing backup for IMGArchive::swapConsecutive()! Maybe you don't "
						"have enough space left on the hard disk.", __FILE__, __LINE__);
				throw ex;
			}
		}

		delete out;
	}

	if (r1Size < r2Size) {
		// Now move the contents of range 2 to the beginning of range 1
		forceMoveEntries(r1Begin, r1End, r2End, r1Begin->offset);

		r1End = r1Last;
		r1End++;

		// Update the offsets of all entries in range 1
		for (EntryIterator it = r1Begin ; it != r1End ; it++) {
			IMGEntry& entry = *it;
			entry.offset += r2Size;
		}

		// And rewrite the contents of range 1 from the backup
		outImgStream->seekp(IMG_BLOCKS2BYTES(r1Begin->offset) - outImgStream->tellp(), iostream::cur);
	} else {
		// Range 2 is smaller
		uint32_t oldR1BeginOffset = r1Begin->offset;

		// Update the offsets of all entries in range 2
		for (EntryIterator it = r1End ; it != r2End ; it++) {
			IMGEntry& entry = *it;
			entry.offset -= r1Size;
		}

		// Now move the contents of range 1 r2Size blocks further
		forceMoveEntries(r2End, r1Begin, r1End, r2Last->offset + r2Last->size);

		// And rewrite the contents of range 2 from the backup
		outImgStream->seekp(IMG_BLOCKS2BYTES(oldR1BeginOffset) - outImgStream->tellp(), iostream::cur);
	}

	if (backupSize <= IMG_MAX_RAM_BLOCKS) {
		// Restore the backup from memory

		outImgStream->write((char*) backup, IMG_BLOCKS2BYTES(backupSize));
		delete[] backup;
	} else {
		// Restore the backup from the temporary file

		istream* in = tmpFile.openInputStream(istream::binary);

		char buf[IMG_BLOCK_SIZE];

		for (uint32_t i = 0 ; i < backupSize ; i++) {
			in->read(buf, sizeof(buf));
			outImgStream->write(buf, sizeof(buf));
		}

		delete in;
		tmpFile.remove();
	}
}



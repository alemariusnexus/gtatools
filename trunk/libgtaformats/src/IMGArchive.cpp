/*
 * IMGArchive.cpp
 *
 *  Created on: 21.01.2010
 *      Author: alemariusnexus
 */

#include "IMGArchive.h"
#include "IMGException.h"
#include <string>
#include <cstring>
#include <vector>
#include "gf_filetype.h"

using std::string;
using std::vector;
using std::ifstream;




IMGArchive::IMGArchive(istream* stream, bool randomAccess)
		: stream(stream), randomAccess(randomAccess), autoGeneratedStream(false)
{
	readHeader(stream);
}


IMGArchive::IMGArchive(const char* filename)
		: stream(NULL), randomAccess(true), autoGeneratedStream(true)
{
	string fname(filename);

	if (isValidDIRFilename(fname)) {
		string imgFile(fname);
		size_t index = imgFile.find_last_of('.');
		imgFile = imgFile.substr(0, index).append(".img");

		printf("%s\n", imgFile.c_str());

		ifstream* dirStream = openStream(filename);
		stream = openStream(imgFile.c_str());

		readHeader(dirStream);

		dirStream->close();
		delete dirStream;
	} else if (isValidIMGFilename(fname)) {
		if (guessIMGVersion(filename) == VER2) {
			stream = openStream(filename);
			readHeader(stream);
		} else {
			stream = openStream(filename);

			string dirFile(fname);
			size_t index = dirFile.find_last_of('.');
			dirFile = dirFile.substr(0, index).append(".dir");

			ifstream* dirStream = openStream(dirFile.c_str());
			readHeader(dirStream);
			dirStream->close();
			delete dirStream;
		}
	} else {
		throw IMGException(IMGException::UnknownFileType, "File name is neither an IMG nor a DIR file");
	}
}


IMGArchive::IMGArchive(istream* dirStream, istream* imgStream, bool randomAccess)
		: stream(imgStream), randomAccess(randomAccess), autoGeneratedStream(false)
{
	readHeader(dirStream);
}


IMGArchive::IMGArchive(const char* dirName, const char* imgName)
		: stream(NULL), randomAccess(true), autoGeneratedStream(true)
{
	stream = openStream(imgName);
	ifstream* dirStream = openStream(dirName);

	readHeader(dirStream);

	dirStream->close();
	delete dirStream;
}


IMGArchive::~IMGArchive()
{
	for (int32_t i = 0 ; i < numEntries ; i++) {
		delete entries[i];
	}

	delete[] entries;

	if (autoGeneratedStream) {
		((ifstream*) stream)->close();
		delete stream;
	}
}


bool IMGArchive::isValidDIRFilename(const std::string& filename)
{
	return GFGuessFileType(filename) == GF_TYPE_DIR;
}


bool IMGArchive::isValidIMGFilename(const std::string& filename)
{
	return GFGuessFileType(filename) == GF_TYPE_IMG;
}


IMGArchive::IMGVersion IMGArchive::guessIMGVersion(istream* stream, bool returnToStart)
{
	char fourCC[4];
	stream->read(fourCC, 4);

	if (returnToStart) {
		stream->seekg(-4, istream::cur);
	}

	if (fourCC[0] == 'V'  &&  fourCC[1] == 'E'  &&  fourCC[2] == 'R'  &&  fourCC[3] == '2') {
		return VER2;
	} else {
		return VER1;
	}
}


IMGArchive::IMGVersion IMGArchive::guessIMGVersion(const char* filename)
{
	ifstream stream(filename, ifstream::in | ifstream::binary);
	return guessIMGVersion(&stream, false);
}


istream* IMGArchive::gotoEntry(IMGEntry* entry) {
	long long start = entry->offset*IMG_BLOCK_SIZE;

	long long cpos;

	if (randomAccess) {
		cpos = stream->tellg();
	} else {
		cpos = bytesRead;
	}

	if (start > bytesRead) {
		char skipBuffer[4096];
		SkipBytes(stream, start - cpos, skipBuffer, sizeof(skipBuffer));
		assertNoEOF();
		bytesRead = start;
	} else {
		if (randomAccess) {
			stream->seekg(start-cpos, istream::cur);
		} else {
			string msg = "Entry ";
			msg += entry->name;
			msg += " is before current position, but may not seek!";
			throw IMGException(IMGException::MayNotSeek, msg.c_str());
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

void IMGArchive::readHeader(istream* stream)
{
	int32_t firstBytes;

	stream->read((char*) &firstBytes, 4);

	char* fourCC = (char*) &firstBytes;

	if (fourCC[0] == 'V'  &&  fourCC[1] == 'E'  &&  fourCC[2] == 'R'  &&  fourCC[3] == '2') {
		version = VER2;
		stream->read((char*) &numEntries, 4);

		assertNoEOF();

		entries = new IMGEntry*[numEntries];

		for (int32_t i = 0 ; i < numEntries ; i++) {
			entries[i] = new IMGEntry;
			stream->read((char*) entries[i], sizeof(IMGEntry));
			assertNoEOF();
		}

		bytesRead = 8 + numEntries*sizeof(IMGEntry);
	} else {
		version = VER1;

		vector<IMGEntry*> entryVector;

		IMGEntry* firstEntry = new IMGEntry;
		firstEntry->offset = firstBytes;
		stream->read((char*) firstEntry+4, sizeof(IMGEntry)-4);
		entryVector.push_back(firstEntry);

		stream->exceptions(istream::goodbit);

		int j = 0;
		while (!stream->eof()) {
			IMGEntry* entry = new IMGEntry;
			stream->read((char*) entry, sizeof(IMGEntry));
			entryVector.push_back(entry);
			j++;
		}

		stream->clear();
		stream->exceptions(ifstream::failbit | ifstream::badbit | ifstream::eofbit);

		entries = new IMGEntry*[entryVector.size()];

		vector<IMGEntry*>::iterator it;
		int i = 0;
		for (it = entryVector.begin() ; it != entryVector.end() ; it++) {
			entries[i++] = *it;
		}

		bytesRead = 0;
		numEntries = entryVector.size();
	}
}

ifstream* IMGArchive::openStream(const char* filename)
{
	ifstream* stream = new ifstream;
	stream->exceptions(ifstream::failbit | ifstream::badbit | ifstream::eofbit);
	stream->open(filename, ifstream::in | ifstream::binary);
	return stream;
}

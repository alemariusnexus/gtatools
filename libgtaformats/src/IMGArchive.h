/*
 * IMGArchive.h
 *
 *  Created on: 21.01.2010
 *      Author: alemariusnexus
 */

#ifndef IMGARCHIVE_H_
#define IMGARCHIVE_H_

#include "config.h"
#include "IMGVisitor.h"
#include <istream>
#include <exception>
#include <string>
#include <algorithm>
#include <cctype>
#include <cstdio>

using std::istream;


#define IMG_BLOCK_SIZE 2048


class IMGVisitor;

struct IMGEntry {
	int32_t offset;
	int32_t size;
	char name[24];
};


class IMGException : std::exception {
public:
	IMGException(const char* message, int currentOffset) throw();
	virtual const char* what() const throw() { return message; };
	int getCurrentOffset() const throw() { return currentOffset; }

private:
	const char* message;
	int currentOffset;
};


class IMGArchive {
public:
	static bool isValidExtension(const std::string& extension) {
		std::string copy = extension;
		std::transform(extension.begin(), extension.end(), copy.begin(), tolower);
		return copy.compare("img") == 0;
	}
	static bool isValidFilename(const std::string& filename) {
		size_t pos = filename.find_last_of('.');
		return pos != filename.npos  &&  isValidExtension(filename.substr(pos+1));
	}
	static bool isValidFile(const char* filename);

public:
	IMGArchive(istream* stream, bool maySeek = true);
	istream* gotoEntry(IMGEntry* entry);
	istream* gotoEntry(const char* name);
	void visit(IMGVisitor* visitor, IMGEntry* entry);
	void visitAll(IMGVisitor* visitor);
	IMGEntry* getEntryByName(const char* name);

	IMGEntry** getEntries() {
		return entries;;
	}
	int32_t getEntryCount() {
		return numEntries;
	}

private:
	void assertNoEOF() {
		if (stream->eof())
			throw IMGException("Premature end of file", bytesRead);
	}

private:
	istream* stream;
	IMGEntry** entries;
	int32_t numEntries;

	long long bytesRead;
	bool maySeek;
};

#endif /* IMGARCHIVE_H_ */

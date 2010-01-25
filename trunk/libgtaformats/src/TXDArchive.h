/*
 * TXDArchive.h
 *
 *  Created on: 08.01.2010
 *      Author: alemariusnexus
 */

#ifndef TXDARCHIVE_H_
#define TXDARCHIVE_H_

#include "config.h"
#include "TXDException.h"
#include "TXDTexture.h"
#include "TXDVisitor.h"
#include <istream>
#include <string>
#include <algorithm>
#include <cctype>
#include <cstdio>

using std::istream;
using std::string;

#define TXD_COMPRESSION_NONE 0
#define TXD_COMPRESSION_DXT1 1
#define TXD_COMPRESSION_DXT3 3

class TXDVisitor;




class TXDArchive {
public:
	static bool isValidExtension(const string& extension) {
		string copy = extension;
		std::transform(extension.begin(), extension.end(), copy.begin(), tolower);
		return copy.compare("txd") == 0;
	}
	static bool isValidFilename(const std::string& filename) {
		size_t pos = filename.find_last_of('.');
		return pos != filename.npos  &&  isValidExtension(filename.substr(pos+1));
	}

public:
	TXDArchive(istream* stream);
	~TXDArchive();
	TXDTexture* nextTexture();
	void readTextureData(uint8_t* dest, TXDTexture* texture);
	uint8_t* readTextureData(TXDTexture* texture);
	void gotoTexture(TXDTexture* texture);
	void visit(TXDVisitor* visitor, TXDTexture* texture);
	void visitAll(TXDVisitor* visitor);

	int16_t getTextureCount() { return textureCount; }

private:
	void assertNoEOF() {
		if (stream->eof())
			throw TXDException("Premature end of file");
	}

private:
	istream* stream;
	long long bytesRead;
	TXDTexture** indexedTextures;
	long long* textureNativeStarts;
	int16_t readIndex;
	int16_t textureCount;
	TXDTexture* currentTexture;
	int32_t currentTextureNativeSize;
	long long currentTextureNativeStart;
};

#endif /* TXDARCHIVE_H_ */

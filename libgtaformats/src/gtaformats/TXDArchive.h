/*
 * TXDArchive.h
 *
 *  Created on: 08.01.2010
 *      Author: alemariusnexus
 */

#ifndef TXDARCHIVE_H_
#define TXDARCHIVE_H_

#include "gf_config.h"
#include "TXDException.h"
#include "TXDTexture.h"
#include "TXDVisitor.h"
#include "gta.h"
#include <istream>
#include <string>
#include <algorithm>
#include <cctype>
#include <cstdio>
#include "gf_filetype.h"

using std::istream;
using std::string;

#define TXD_COMPRESSION_NONE 0
#define TXD_COMPRESSION_DXT1 1
#define TXD_COMPRESSION_DXT3 3

class TXDVisitor;




class TXDArchive {
public:
	static bool isValidFilename(const std::string& filename) {
		return GFGuessFileType(filename) == GF_TYPE_TXD;
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
	void assertNoEOF() {}
	void readSectionHeaderWithID(istream* stream, RwSectionHeader& header, uint32_t id);

private:
	istream* stream;
	istream::streamoff bytesRead;
	TXDTexture** indexedTextures;
	istream::streamoff* textureNativeStarts;
	int16_t readIndex;
	int16_t textureCount;
	TXDTexture* currentTexture;
	int32_t currentTextureNativeSize;
	istream::streamoff currentTextureNativeStart;
};

#endif /* TXDARCHIVE_H_ */

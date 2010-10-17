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

#include "TXDArchive.h"
#include "../gta.h"
#include <cstring>
#include <cstdio>
#include "../util/stream/FileInputStream.h"


TXDArchive::TXDArchive(InputStream* stream, bool randomAccess)
		: randomAccess(randomAccess), stream(stream), bytesRead(0), readIndex(0),
		  currentTextureNativeSize(-1), currentTextureNativeStart(-1), deleteStream(false)
{
	/*if (stream->fail()) {
		throw TXDException(TXDException::IOError, "failbit was set in the given TXD stream");
	}*/

	init();
}


TXDArchive::TXDArchive(const File& file)
		: randomAccess(true), stream(file.openStream(STREAM_BINARY)),
		  bytesRead(0), readIndex(0), currentTextureNativeSize(-1), currentTextureNativeStart(-1),
		  deleteStream(true)
{
	/*if (stream->fail()) {
		throw TXDException(TXDException::UnableToOpen, "Unable to open TXD file");
	}*/

	init();
}


TXDArchive::~TXDArchive()
{
	delete[] textureNativeStarts;

	delete[] indexedTextures;

	if (deleteStream) {
		delete stream;
	}
}


void TXDArchive::init()
{
	char skipBuf[2];

	RwSectionHeader header;
	readSectionHeaderWithID(stream, header, RW_SECTION_TEXTUREDICTIONARY);
	readSectionHeaderWithID(stream, header, RW_SECTION_STRUCT);

	stream->read((char*) &textureCount, 2);
	stream->skip(2);

	if (textureCount < 0) {
		throw TXDException("Texture count is < 0", __FILE__, __LINE__);
	}

	bytesRead += 4;

	textureNativeStarts = new long long[textureCount];
	indexedTextures = new TXDTexture*[textureCount];

	for (int16_t i = 0 ; i < textureCount ; i++) {
		indexedTextures[i] = NULL;
	}
}


TXDTexture* TXDArchive::nextTexture()
{
	if (currentTextureNativeStart != -1) {
		long long len = currentTextureNativeStart + currentTextureNativeSize + 12 - bytesRead;
		stream->skip(len);
		bytesRead += len;
	}

	char skipBuf[12];

	long long texNativeStart = bytesRead;

	RwSectionHeader texNative;
    RwReadSectionHeader(stream, texNative);
    bytesRead += sizeof(RwSectionHeader);

    stream->skip(12);
    bytesRead += 12;

    TXDTexture* texture = new TXDTexture(stream, bytesRead);

    currentTextureNativeStart = texNativeStart;
    currentTextureNativeSize = texNative.size;
    currentTexture = texture;

    indexedTextures[readIndex] = texture;
    textureNativeStarts[readIndex] = texNativeStart;
    readIndex++;

    return texture;
}

void TXDArchive::readTextureData(uint8_t* dest, TXDTexture* texture)
{
	int32_t rasterSize;

	if ((texture->getRasterFormatExtension() & TXD_FORMAT_EXT_PAL4) != 0) {
		stream->read((char*) dest, 16*4);
		stream->read((char*) &rasterSize, 4);
		stream->read((char*) dest + 16*4, rasterSize);
		bytesRead += 16*4 + rasterSize + 4;
	} else if ((texture->getRasterFormatExtension() & TXD_FORMAT_EXT_PAL8) != 0) {
		stream->read((char*) dest, 256*4);
		stream->read((char*) &rasterSize, 4);
		stream->read((char*) (dest + 256*4), rasterSize);
		bytesRead += 256*4 + rasterSize + 4;
	} else {
		stream->read((char*) &rasterSize, 4);
		stream->read((char*) dest, rasterSize);
		bytesRead += rasterSize+4;
	}
}

uint8_t* TXDArchive::readTextureData(TXDTexture* texture)
{
	int size = texture->getWidth() * texture->getHeight() * texture->getBytesPerPixel();

	if (texture->getRasterFormatExtension() & TXD_FORMAT_EXT_PAL4) {
		size += 16*4;
	} else if (texture->getRasterFormatExtension() & TXD_FORMAT_EXT_PAL8) {
		size += 256*4;
	}

	uint8_t* raster = new uint8_t[size];
	readTextureData(raster, texture);
	return raster;
}

void TXDArchive::gotoTexture(TXDTexture* texture)
{
	for (int32_t i = 0 ; i < textureCount ; i++) {
		if (indexedTextures[i] == texture) {
			long long start = textureNativeStarts[i];
			stream->seek((start+112) - bytesRead);
			bytesRead = start+112;
			return;
		}
	}
}

void TXDArchive::visit(TXDVisitor* visitor, TXDTexture* texture)
{
	if (currentTexture != texture) {
		gotoTexture(texture);
	}

	visitor->handleTexture(this, texture);
}

void TXDArchive::visitAll(TXDVisitor* visitor)
{
	for (int16_t i = 0 ; i < textureCount ; i++) {
		TXDTexture* tex;

		if (indexedTextures[i] == NULL) {
			tex = nextTexture();
		} else {
			tex = indexedTextures[i];
			gotoTexture(tex);
		}

		visit(visitor, tex);
	}
}


void TXDArchive::destroyTexture(TXDTexture* tex)
{
	for (int16_t i = 0 ; i < textureCount ; i++) {
		if (indexedTextures[i] == tex) {
			indexedTextures[i] = NULL;
			break;
		}
	}

	delete tex;
}




void TXDArchive::readSectionHeaderWithID(InputStream* stream, RwSectionHeader& header, uint32_t id)
{
	RwReadSectionHeader(stream, header);

	/*if (stream->fail()) {
		throw TXDException(TXDException::SyntaxError, "Premature end of file");
	}*/

	bytesRead += sizeof(RwSectionHeader);

	if (header.id != id) {
		char expected[64];
		char found[64];
		RwGetSectionName(id, expected);
		RwGetSectionName(header.id, found);
		char errmsg[256];
		sprintf(errmsg, "Found section with type %s where %s was expected", found, expected);
		throw TXDException(errmsg, __FILE__, __LINE__);
	}
}


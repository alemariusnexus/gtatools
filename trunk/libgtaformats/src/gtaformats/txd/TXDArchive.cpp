/*
 * TXDArchive.cpp
 *
 *  Created on: 08.01.2010
 *      Author: alemariusnexus
 */

#include "TXDArchive.h"
#include "../gta.h"
#include <cstring>
#include <cstdio>


TXDArchive::TXDArchive(istream* stream)
		: stream(stream), bytesRead(0), readIndex(0), currentTextureNativeSize(-1),
		  currentTextureNativeStart(-1)
{
	char skipBuf[2];

	RwSectionHeader header;
	readSectionHeaderWithID(stream, header, RW_SECTION_TEXTUREDICTIONARY);
	readSectionHeaderWithID(stream, header, RW_SECTION_STRUCT);

	stream->read((char*) &textureCount, 2);
	stream->read(skipBuf, 2);

	bytesRead += 4;

	textureNativeStarts = new istream::streamoff[textureCount];
	indexedTextures = new TXDTexture*[textureCount];

	for (int16_t i = 0 ; i < textureCount ; i++) {
		indexedTextures[i] = NULL;
	}
}

TXDArchive::~TXDArchive()
{
	delete[] textureNativeStarts;

	/*for (int16_t i = 0 ; i < textureCount ; i++) {
		if (indexedTextures[i] != NULL) {
			delete indexedTextures[i];
		}
	}*/

	delete[] indexedTextures;
}

TXDTexture* TXDArchive::nextTexture()
{
	if (currentTextureNativeStart != -1) {
		char skipBuf[2048];
		istream::streamoff len = currentTextureNativeStart + currentTextureNativeSize + 12 - bytesRead;
		SkipBytes(stream, len, skipBuf, sizeof(skipBuf));
		bytesRead += len;
	}

	char skipBuf[12];

	istream::streamoff texNativeStart = bytesRead;

	RwSectionHeader texNative;
    RwReadSectionHeader(stream, texNative);
    bytesRead += sizeof(RwSectionHeader);

    stream->read(skipBuf, 12);
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
	for (int i = 0 ; i < textureCount ; i++) {
		if (indexedTextures[i] == texture) {
			istream::streamoff start = textureNativeStarts[i];
			stream->seekg((start+112) - bytesRead, istream::cur);

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




void TXDArchive::readSectionHeaderWithID(istream* stream, RwSectionHeader& header, uint32_t id)
{
	RwReadSectionHeader(stream, header);

	bytesRead += sizeof(RwSectionHeader);

	if (header.id != id) {
		char expected[64];
		char found[64];
		RwGetSectionName(id, expected);
		RwGetSectionName(header.id, found);
		char errmsg[256];
		printf("Throwing\n");
		sprintf(errmsg, "Found section with type %s where %s was expected", found, expected);
		throw TXDException(TXDException::SyntaxError, errmsg, bytesRead);
	}
}


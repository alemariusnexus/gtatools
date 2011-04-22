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

#include "TXDArchive.h"
#include "../gta.h"
#include <cstring>
#include <cstdio>


TXDArchive::TXDArchive(istream* stream, bool randomAccess)
		: randomAccess(randomAccess), stream(stream), bytesRead(0), readIndex(0),
		  currentTextureNativeSize(-1), currentTextureNativeStart(-1), deleteStream(false)
{
	init();
}


TXDArchive::TXDArchive(const File& file)
		: randomAccess(true), stream(file.openInputStream(istream::binary)),
		  bytesRead(0), readIndex(0), currentTextureNativeSize(-1), currentTextureNativeStart(-1),
		  deleteStream(true)
{
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
	stream->read(skipBuf, 2);

	if (textureCount < 0) {
		throw TXDException("Texture count is < 0", __FILE__, __LINE__);
	}

	bytesRead += 4;

	textureNativeStarts = new long long[textureCount];
	indexedTextures = new TXDTextureHeader*[textureCount];

	for (int16_t i = 0 ; i < textureCount ; i++) {
		indexedTextures[i] = NULL;
	}
}


TXDTextureHeader* TXDArchive::nextTexture()
{
	if (currentTextureNativeStart != -1) {
		unsigned int len = (unsigned int) (currentTextureNativeStart + currentTextureNativeSize + 12 - bytesRead);
		char* tmpSkipBuf = new char[len];
		stream->read(tmpSkipBuf, len);
		delete[] tmpSkipBuf;
		bytesRead += len;
	}

	char skipBuf[12];

	long long texNativeStart = bytesRead;

	RwSectionHeader texNative;
    RwReadSectionHeader(stream, texNative);
    bytesRead += sizeof(RwSectionHeader);

    stream->read(skipBuf, 12);
    bytesRead += 12;

    int32_t platform;
    int16_t filterFlags;
    int8_t vWrap, uWrap;
    char diffuseName[32];
    char alphaName[32];
    int32_t rasterFormat;
    int32_t alphaOrCompr;
    int16_t width, height;
    int8_t bpp;
    int8_t mipmapCount;
    //int8_t rasterType;
    int8_t comprTypeOrAlpha;

    stream->read((char*) &platform, 4);
    stream->read((char*) &filterFlags, 2);
    stream->read((char*) &vWrap, 1);
    stream->read((char*) &uWrap, 1);
    stream->read(diffuseName, 32);
    stream->read(alphaName, 32);
    stream->read((char*) &rasterFormat, 4);
    stream->read((char*) &alphaOrCompr, 4);
    stream->read((char*) &width, 2);
    stream->read((char*) &height, 2);
    stream->read((char*) &bpp, 1);
    stream->read((char*) &mipmapCount, 1);
    stream->ignore(1); // Raster Type
    stream->read((char*) &comprTypeOrAlpha, 1);

    bytesRead += 88;

    TXDCompression compr = NONE;
    bool alpha;

    if (platform == 9) {
    	char* fourCC = (char*) &alphaOrCompr;
    	if (fourCC[0] == 'D'  &&  fourCC[1] == 'X'  &&  fourCC[2] == 'T') {
    		if (fourCC[3] == '1') {
    			compr = DXT1;
    		} else if (fourCC[3] == '3') {
    			compr = DXT3;
    		}
    	} else if (fourCC[0] == 'P'  &&  fourCC[1] == 'V'  &&  fourCC[2] == 'R') {
    		if (fourCC[3] == '2') {
    			compr = PVRTC2;
    		} else if (fourCC[3] == '4') {
    			compr = PVRTC4;
    		}
    	}

    	alpha = (comprTypeOrAlpha == 9  ||  comprTypeOrAlpha == 1);
    } else {
    	if (comprTypeOrAlpha == 1) {
    		compr = DXT1;
    	} else if (comprTypeOrAlpha == 3) {
    		compr = DXT3;
    	} else if (comprTypeOrAlpha == 50) {
    		compr = PVRTC2;
    	} else if (comprTypeOrAlpha == 51) {
    		compr = PVRTC4;
    	}

    	alpha = (alphaOrCompr == 1);
    }

    TXDTextureHeader* texture = new TXDTextureHeader(diffuseName, rasterFormat, compr, width, height);
    texture->setAlphaChannel(alpha);
    texture->setAlphaName(alphaName);
    texture->setFilterFlags(filterFlags);
    texture->setMipmapCount(mipmapCount);
    texture->setWrappingFlags(uWrap, vWrap);
    texture->setBytesPerPixel(bpp/8);

    currentTextureNativeStart = texNativeStart;
    currentTextureNativeSize = texNative.size;
    currentTexture = texture;

    indexedTextures[readIndex] = texture;
    textureNativeStarts[readIndex] = texNativeStart;
    readIndex++;

    return texture;
}

int TXDArchive::readTextureData(uint8_t* dest, TXDTextureHeader* texture)
{
	int32_t rasterSize;
	uint8_t* destStart = dest;

	if ((texture->getRasterFormatExtension() & RasterFormatEXTPAL4) != 0) {
		stream->read((char*) dest, 16*4);
		dest += 16*4;
		bytesRead += 16*4;
	} else if ((texture->getRasterFormatExtension() & RasterFormatEXTPAL8) != 0) {
		stream->read((char*) dest, 256*4);
		dest += 256*4;
		bytesRead += 256*4;
	}

	for (int i = 0 ; i < texture->getMipmapCount() ; i++) {
		stream->read((char*) &rasterSize, 4);
		stream->read((char*) dest, rasterSize);
		bytesRead += rasterSize+4;
		dest += rasterSize;
	}

	return dest-destStart;
}


uint8_t* TXDArchive::readTextureData(TXDTextureHeader* texture)
{
	int size = texture->computeDataSize();
	uint8_t* raster = new uint8_t[size];
	readTextureData(raster, texture);
	return raster;
}

void TXDArchive::gotoTexture(TXDTextureHeader* texture)
{
	for (int32_t i = 0 ; i < textureCount ; i++) {
		if (indexedTextures[i] == texture) {
			long long start = textureNativeStarts[i];
			stream->seekg((start+112) - bytesRead, istream::cur);
			bytesRead = start+112;
			return;
		}
	}
}


void TXDArchive::destroyTexture(TXDTextureHeader* tex)
{
	for (int16_t i = 0 ; i < textureCount ; i++) {
		if (indexedTextures[i] == tex) {
			indexedTextures[i] = NULL;
			break;
		}
	}

	delete tex;
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
		sprintf(errmsg, "Found section with type %s where %s was expected (is it really a TXD file?)",
				found, expected);
		throw TXDException(errmsg, __FILE__, __LINE__);
	}
}


/*
 * TXDArchive.cpp
 *
 *  Created on: 08.01.2010
 *      Author: alemariusnexus
 */

#include "TXDArchive.h"
#include "../gta.h"
#include <squish.h>
#include <cstring>
#include <cstdio>

using namespace squish;


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

	assertNoEOF();

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

	for (int16_t i = 0 ; i < textureCount ; i++) {
		if (indexedTextures[i] != NULL) {
			delete indexedTextures[i];
		}
	}
	delete[] indexedTextures;
}

TXDTexture* TXDArchive::nextTexture()
{
	if (currentTextureNativeStart != -1) {
		char skipBuf[2048];
		istream::streamoff len = currentTextureNativeStart + currentTextureNativeSize + 12 - bytesRead;
		SkipBytes(stream, len, skipBuf, sizeof(skipBuf));
		bytesRead += len;
		assertNoEOF();
	}

	char skipBuf[12];

	istream::streamoff texNativeStart = bytesRead;

	RwSectionHeader texNative;
    RwReadSectionHeader(stream, texNative);
    bytesRead += sizeof(RwSectionHeader);

    stream->read(skipBuf, 12);
    bytesRead += 12;
    TXDTexture* texture = new TXDTexture(stream, bytesRead);

    assertNoEOF();

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

	assertNoEOF();
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


/*TXDArchive::TXDArchive(istream* stream)
		: stream(stream)
{
	bytesReadFromLastTextureNative = 0;
	lastTextureNativeSize = 0;
}

TXDArchive::~TXDArchive() {

}

void TXDArchive::getRasterFormatName(int rasterFormat, char* dest) {
	int format = rasterFormat & 0xF00;

    switch (format) {
        case TXD_FORMAT_A1R5G5B5:
            strcpy(dest, "A1R5G5B5");
            break;
        case TXD_FORMAT_B8G8R8:
            strcpy(dest, "B8G8R8");
            break;
        case TXD_FORMAT_B8G8R8A8:
            strcpy(dest, "B8G8R8A8");
            break;
        case TXD_FORMAT_R4G4B4A4:
            strcpy(dest, "R4G4B4A4");
            break;
        case TXD_FORMAT_R5G5B5:
            strcpy(dest, "R5G5B5");
            break;
        case TXD_FORMAT_R5G6B5:
            strcpy(dest, "R5G6B5");
            break;
        default:
            strcpy(dest, "UNKNOWN");
    }
}

void TXDArchive::visit(TXDVisitor* visitor) {
	parseStart();

	for (int i = 0 ; i < numTextures ; i++) {
		TXDHeader header;
		parseHeader(&header);
		void* udata;

		bool res = visitor->handleHeader(&header, udata);

		if (res) {
			FIBITMAP* bmp = parseTexture(&header);

			if (!visitor->handleTexture(&header, bmp, udata)) {
				break;
			}
		}

		nextTexture();
	}
}

void TXDArchive::parseStart() {
	char skipBuf[2];

	RwSectionHeader header;
	RwReadSectionHeaderWithID(stream, header, RW_SECTION_TEXTUREDICTIONARY);
	RwReadSectionHeaderWithID(stream, header, RW_SECTION_STRUCT);

	stream->read((char*) &numTextures, 2);
	stream->read(skipBuf, 2);
}

void TXDArchive::parseHeader(TXDHeader* header) {
	char skipBuf[12];

	RwSectionHeader texNative;
    bytesReadFromLastTextureNative += RwReadSectionHeader(stream, texNative);

    lastTextureNativeSize = texNative.size;

    stream->read(skipBuf, 12);
    stream->read((char*) header, sizeof(TXDHeader));
    bytesReadFromLastTextureNative += sizeof(TXDHeader);
}

FIBITMAP* TXDArchive::parseTexture(TXDHeader* header) {
	if (!isSupported(header)) {
		throw TXDException("Texture format is not supported!");
	}

	int rasterFormat = header->rasterFormat & 0xFFF;
    compress_t compression = getCompression(header);

    int rasterSize;
    stream->read((char*) &rasterSize, 4);
    bytesReadFromLastTextureNative += 4;

    BYTE* rawData = new BYTE[rasterSize];
    BYTE* data;

    //readCount += ReadBytes(archive->stream, rawData, rasterSize);
    stream->read((char*) rawData, rasterSize);
    bytesReadFromLastTextureNative += rasterSize;

    // The absolute size of a scanline, in bytes
    int pitch;

    // Masks that determine which bits to use for R, G, B and A
    unsigned int redMask, greenMask, blueMask, alphaMask;

    int bpp = header->bpp;
    int bytespp = bpp/8;

    pitch = header->width * bytespp;
    data = rawData;

    if (rasterFormat == TXD_FORMAT_B8G8R8) {
        blueMask = 0xFF;
        greenMask = 0xFF00;
        redMask = 0xFF0000;
        alphaMask = 0xFF000000;
    } else if (rasterFormat == TXD_FORMAT_B8G8R8A8) {
        blueMask = 0xFF;
        greenMask = 0xFF00;
        redMask = 0xFF0000;
        alphaMask = 0xFF000000;
    } else if (rasterFormat == TXD_FORMAT_R5G6B5) {
        redMask = 0x1F;
        greenMask = 0x7E0;
        blueMask = 0xF800;
        alphaMask = 0;
    } else if (rasterFormat == TXD_FORMAT_A1R5G5B5) {
        redMask = 0x1F;
        greenMask = 0x7E0;
        blueMask = 0xF800;
        alphaMask = 0;
    } else if (rasterFormat == TXD_FORMAT_R4G4B4A4) {
        redMask = 0xF;
        greenMask = 0xF0;
        blueMask = 0xF00;
        alphaMask = 0xF000;
    }

    // After DXT-images are decompressed by Squish, they always use an
    // R8G8B8A8 format, so we correct the values passed to FreeImage
    if (compression != TXD_COMPRESSION_NONE) {
        redMask = 0xFF;
        greenMask = 0xFF00;
        blueMask = 0xFF0000;
        alphaMask = 0xFF000000;
        bytespp = 4;
        bpp = 32;
    }

    // Decompress DXT
    if (compression == TXD_COMPRESSION_DXT1) {
    	data = new BYTE[header->width*header->height*bytespp];
        DecompressImage(data, header->width, header->height, rawData, kDxt1);
        delete[] rawData;
    } else if (compression == TXD_COMPRESSION_DXT3) {
    	data = new BYTE[header->width*header->height*bytespp];
        DecompressImage(data, header->width, header->height, rawData, kDxt3);
        delete[] rawData;
    }

    // Allocate an empty bitmap for the result image data
    FIBITMAP* bmp = FreeImage_Allocate(header->width, header->height, bpp);

    // The number of bits to shift the values for R, G, B, A after they
    // have been extracted from the pixel using the masks.
    int redShift, greenShift, blueShift, alphaShift;

    // Calculate the shifts from the masks
    for (int i = 0 ; i < 32 ; i++) {
        if ((redMask & (1 << i)) != 0) {
            redShift = i;
            break;
        }
    }
    for (int i = 0 ; i < 32 ; i++) {
        if ((greenMask & (1 << i)) != 0) {
            greenShift = i;
            break;
        }
    }
    for (int i = 0 ; i < 32 ; i++) {
        if ((blueMask & (1 << i)) != 0) {
            blueShift = i;
            break;
        }
    }
    for (int i = 0 ; i < 32 ; i++) {
        if ((alphaMask & (1 << i)) != 0) {
            alphaShift = i;
            break;
        }
    }


    int pixel;

    BYTE* dataIterator = data;
    for (int y = 0 ; y < header->height ; y++) {
        BYTE* line = FreeImage_GetScanLine(bmp, header->height-y-1);

        for (int x = 0 ; x < header->width ; x++) {
            // Read the pixel from the raster data
            memcpy(&pixel, dataIterator, bytespp);

            // Set the color values for the result pixel using the
            // masks
            line[FI_RGBA_RED] = (BYTE) ((pixel & redMask) >> redShift);
            line[FI_RGBA_GREEN] = (BYTE) ((pixel & greenMask) >> greenShift);
            line[FI_RGBA_BLUE] = (BYTE) ((pixel & blueMask) >> blueShift);
            line[FI_RGBA_ALPHA] = (BYTE) ((pixel & alphaMask) >> alphaShift);

            // Move to the next pixel
            line += bytespp;
            dataIterator += bytespp;
        }
    }

    delete[] data;

    return bmp;
}

void TXDArchive::nextTexture() {
	int skipSize = lastTextureNativeSize - bytesReadFromLastTextureNative;
	char* skipBuf = new char[skipSize];
	stream->read(skipBuf, skipSize);
	lastTextureNativeSize = 0;
	bytesReadFromLastTextureNative = 0;
}

TXDArchive::compress_t TXDArchive::getCompression(TXDHeader* header) {
	compress_t compression = TXD_COMPRESSION_NONE;

    if (    header->alphaFourCC[0] == 'D'  &&  header->alphaFourCC[1] == 'X'
            &&  header->alphaFourCC[2] == 'T'
    ) {
        if (header->alphaFourCC[3] == '1') {
            compression = TXD_COMPRESSION_DXT1;
        } else if (header->alphaFourCC[3] == '3') {
            compression = TXD_COMPRESSION_DXT3;
        }
    }

    return compression;
}

bool TXDArchive::isSupported(TXDHeader* header) {
	int rasterFormat = header->rasterFormat & 0xFFF;

	return	rasterFormat == TXD_FORMAT_A1R5G5B5
			||  rasterFormat == TXD_FORMAT_B8G8R8
			||  rasterFormat == TXD_FORMAT_B8G8R8A8
			||  rasterFormat == TXD_FORMAT_R4G4B4A4
			||  rasterFormat == TXD_FORMAT_R5G5B5
			||  rasterFormat == TXD_FORMAT_R5G6B5;
}*/


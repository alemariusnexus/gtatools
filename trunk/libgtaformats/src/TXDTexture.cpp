/*
 * TXDTexture.cpp
 *
 *  Created on: 23.01.2010
 *      Author: alemariusnexus
 */

#include "TXDTexture.h"
#include "TXDException.h"
#include <cstring>
#include <cstdio>
#include <cmath>
#include <squish.h>

using namespace squish;


void TxdGetRasterFormatName(char* dest, int32_t rasterFormat) {
	switch (rasterFormat & 0xF00) {
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
	}
}


TXDTexture::TXDTexture(const char* diffuseName, const char* alphaName, int32_t rasterFormat,
			TXDCompression compression, int16_t width, int16_t height, int8_t bpp, int8_t mipmapCount)
		: rasterFormat(rasterFormat), compression(compression), width(width), height(height),
		  bytesPerPixel(bpp), mipmapCount(mipmapCount)
{
	strncpy(this->diffuseName, diffuseName, 32);
	strncpy(this->alphaName, alphaName, 32);


}

TXDTexture::TXDTexture(istream* stream)
{
	char skipBuf[4];

	int32_t platformId;
	stream->read((char*) &platformId, 4);
	stream->read(skipBuf, 4);
	stream->read(diffuseName, 32);
	stream->read(alphaName, 32);
	stream->read((char*) &rasterFormat, 4);
	//char alphaFourCC[4];
	int32_t alphaFourCC;
	stream->read((char*) &alphaFourCC, 4);
	stream->read((char*) &width, 2);
	stream->read((char*) &height, 2);
	stream->read((char*) &bytesPerPixel, 1);
	bytesPerPixel /= 8;
	stream->read((char*) &mipmapCount, 1);
	stream->read(skipBuf, 1);
	int8_t dxtType;
	stream->read((char*) &dxtType, 1);

	if (stream->eof()) {
		throw TXDException("Premature end of file");
	}

	if (platformId == 9) {
		alphaChannel = (dxtType == 9);
		char* dxtFourCC = (char*) &alphaFourCC;

		if (dxtFourCC[0] == 'D'  &&  dxtFourCC[1] == 'X'  &&  dxtFourCC[2] == 'T') {
			if (dxtFourCC[3] == '1') {
				compression = DXT1;
			} else if (dxtFourCC[3] == '3') {
				compression = DXT3;
			} else {
				compression = NONE;
			}
		} else {
			compression = NONE;
		}
	} else {
		if (dxtType == 1) {
			compression = DXT1;
		} else if (dxtType == 3) {
			compression = DXT3;
		} else {
			compression = NONE;
		}

		alphaChannel = (alphaFourCC == 1);
	}
}

void TXDTexture::getColorMasks(int32_t& redMask, int32_t& greenMask, int32_t& blueMask,
		int32_t& alphaMask)
{
	switch (rasterFormat) {
	case TXD_FORMAT_A1R5G5B5:
		alphaMask = 1;
		redMask = 0x3E;
		greenMask = 0x7C0;
		blueMask = 0xF800;
		break;
	case TXD_FORMAT_B8G8R8:
		blueMask = 0xFF;
		greenMask = 0xFF00;
		redMask = 0xFF0000;
		alphaMask = 0;
		break;
	case TXD_FORMAT_B8G8R8A8:
		blueMask = 0xFF;
		greenMask = 0xFF00;
		redMask = 0xFF0000;
		alphaMask = 0xFF000000;
		break;
	case TXD_FORMAT_R4G4B4A4:
		redMask = 0xF;
		greenMask = 0xF0;
		blueMask = 0xF00;
		alphaMask = 0xF000;
		break;
	case TXD_FORMAT_R5G5B5:
		redMask = 0x1F;
		greenMask = 0x3E0;
		blueMask = 0x7C00;
		alphaMask = 0;
		break;
	case TXD_FORMAT_R5G6B5:
		redMask = 0x1F;
		greenMask = 0x7E0;
		blueMask = 0xF800;
		alphaMask = 0;
		break;
	}
}

void TXDTexture::getFormat(char* dest)
{
	char compStr[8];
	char formatStr[16];

	if (compression == NONE) {
		strcpy(compStr, "un");
	} else if (compression == DXT1) {
		strcpy(compStr, "DXT1-");
	} else {
		strcpy(compStr, "DXT3-");
	}

	TxdGetRasterFormatName(formatStr, rasterFormat & 0xF00);

	sprintf(dest, "%d BPP %scompressed %s image", bytesPerPixel, compStr, formatStr);
}

void TXDTexture::convert(uint8_t* dest, const uint8_t* src, TXDMirrorFlags mirror,
		int8_t bpp, int redOffset, int greenOffset, int blueOffset, int alphaOffset)
{
	int8_t srcBpp = bytesPerPixel;
	const uint8_t* data;

	int32_t redMask, greenMask, blueMask, alphaMask;

	if (getCompression() == NONE) {
		getColorMasks(redMask, greenMask, blueMask, alphaMask);
		data = src;
	} else {
		uint8_t* tmp = new uint8_t[width * height * 4];

		if (getCompression() == DXT1) {
			DecompressImage(tmp, width, height, src, kDxt1);
		} else {
			DecompressImage(tmp, width, height, src, kDxt3);
		}

		data = tmp;

		redMask = 0xFF;
		greenMask = 0xFF00;
		blueMask = 0xFF0000;
		alphaMask = 0xFF000000;
		srcBpp = 4;
	}

	int32_t rShift = 0, gShift = 0, bShift = 0, aShift = 0;

	for (int i = 0 ; i < 32 ; i++) {
		if ((redMask & (1 << i)) != 0) {
            rShift = i;
            break;
        }
	}
	for (int i = 0 ; i < 32 ; i++) {
		if ((greenMask & (1 << i)) != 0) {
            gShift = i;
            break;
        }
	}
	for (int i = 0 ; i < 32 ; i++) {
		if ((blueMask & (1 << i)) != 0) {
            bShift = i;
            break;
        }
	}
	for (int i = 0 ; i < 32 ; i++) {
		if ((alphaMask & (1 << i)) != 0) {
            aShift = i;
            break;
        }
	}

	for (int16_t y = 0 ; y < height ; y++) {
		for (int16_t x = 0 ; x < width ; x++) {
			int srcIdx = (y*width + x);
			int destIdx = (y*width + x) * bpp;

			if (mirror == MIRROR_BOTH) {
				srcIdx = (width*height) - srcIdx;
			} else if (mirror == MIRROR_HORIZONTAL) {
				srcIdx = (height - (int) floor((float) (srcIdx/width)) - 1) * width + srcIdx%width;
			} else if (mirror == MIRROR_VERTICAL) {
				srcIdx = ((int) floor((float) (srcIdx/width)) * width + (height - srcIdx%width) - 1);
			}

			srcIdx *= srcBpp;

			int32_t pixel;
			memcpy(&pixel, data+srcIdx, srcBpp);

			dest[destIdx + redOffset] = (pixel & redMask) >> rShift;
			dest[destIdx + greenOffset] = (pixel & greenMask) >> gShift;
			dest[destIdx + blueOffset] = (pixel & blueMask) >> bShift;

			if (alphaOffset != -1) {
				dest[destIdx + alphaOffset] = 255;
			}
		}
	}

	if (compression != NONE) {
		delete[] data;
	}
}

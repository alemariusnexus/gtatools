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
	case TXD_FORMAT_LUM8:
		strcpy(dest, "LUM8");
		break;
	}
}


TXDTexture::TXDTexture(const char* diffuseName, const char* alphaName, int32_t rasterFormat,
			TXDCompression compression, int16_t width, int16_t height, int8_t bpp, int8_t mipmapCount,
			bool alphaChannel, int8_t uWrap, int8_t vWrap, int16_t filterFlags)
		: rasterFormat(rasterFormat), compression(compression), width(width), height(height),
		  bytesPerPixel(bpp), mipmapCount(mipmapCount), alphaChannel(alphaChannel), uWrap(uWrap),
		  vWrap(vWrap), filterFlags(filterFlags)
{
	strncpy(this->diffuseName, diffuseName, 32);
	strncpy(this->alphaName, alphaName, 32);
}


TXDTexture::TXDTexture(istream* stream, long long& bytesRead)
{
	char skipBuf[4];

	int32_t platformId;
	stream->read((char*) &platformId, 4);
	stream->read((char*) &filterFlags, 2);
	stream->read((char*) &uWrap, 1);
	stream->read((char*) &vWrap, 1);
	stream->read(diffuseName, 32);
	stream->read(alphaName, 32);
	stream->read((char*) &rasterFormat, 4);
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

	bytesRead += 88;

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
		int32_t& alphaMask) const
{
	if (	getRasterFormatExtension() & TXD_FORMAT_EXT_PAL4
			||  getRasterFormatExtension() & TXD_FORMAT_EXT_PAL8
	) {
		redMask = 0xFF;
		greenMask = 0xFF00;
		blueMask = 0xFF0000;
		alphaMask = 0xFF000000;
	} else {
		switch (getRasterFormat()) {
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
		case TXD_FORMAT_LUM8:
			redMask = 0xFF;
			greenMask = 0xFF;
			blueMask = 0xFF;
			alphaMask = 0xFF;
			break;
		}
	}
}

void TXDTexture::getFormat(char* dest) const
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


bool TXDTexture::canConvert()
{
	if (	getRasterFormatExtension() & TXD_FORMAT_EXT_PAL4
			||  getRasterFormatExtension() & TXD_FORMAT_EXT_PAL8
	) {
		return true;
	}

	return true;
}


void TXDTexture::convert(uint8_t* dest, const uint8_t* src, TXDMirrorFlags mirror,
		int8_t bpp, int redOffset, int greenOffset, int blueOffset, int alphaOffset) const
{
	int8_t srcBpp = bytesPerPixel;

	// Will point to the actual, uncompressed raster data, without the palette (if any)
	const uint8_t* data;

	// The masks that extract the color channel of each pixel. For paletted images, these masks
	// can be applied to each palette entry.
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

		// After decompression with Squish, data is stored as R8G8B8A8
		redMask = 0xFF;
		greenMask = 0xFF00;
		blueMask = 0xFF0000;
		alphaMask = 0xFF000000;
		srcBpp = 4;
	}

	// How many times do we have to shift each color channel value to the right so that they
	// can be read as plain uint8_t values?
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

	bool pal4 = (getRasterFormatExtension() & TXD_FORMAT_EXT_PAL4) != 0;
	bool pal8 = (getRasterFormatExtension() & TXD_FORMAT_EXT_PAL8) != 0;

	const uint8_t* palette = NULL;

	// If the image is paletted, data begins with the palette. We'll let 'palette' point to the
	// beginning of the palette and reposition data to point to the beginning of the actual raster
	// data
	if (pal4) {
		palette = data;
		data += 16*4;
	} else if (pal8) {
		palette = data;
		data += 256*4;
	}


	// Now we will transform each pixel
	for (int16_t y = 0 ; y < height ; y++) {
		for (int16_t x = 0 ; x < width ; x++) {

			// The index of the current pixel inside the data array
			int srcIdx = (y*width + x);

			// The index of the new pixel inside the dest array
			int destIdx = (y*width + x) * bpp;

			// To mirror each pixel, we choose another index in the data array.
			if (mirror == MIRROR_BOTH) {
				srcIdx = (width*height) - srcIdx;
			} else if (mirror == MIRROR_HORIZONTAL) {
				srcIdx = (height - (int) floor((float) (srcIdx/width)) - 1) * width + srcIdx%width;
			} else if (mirror == MIRROR_VERTICAL) {
				srcIdx = ((int) floor((float) (srcIdx/width)) * width + (height - srcIdx%width) - 1);
			}

			// So far, srcIdx is the pixel index inside 'data', but each pixel consists of more than
			// one byte, so we multiply the pixel index by the number of bytes per pixel
			srcIdx *= srcBpp;

			// Extract the pixel from data
			int32_t pixel;
			memcpy(&pixel, data+srcIdx, srcBpp);

			// The RGBA values of the pixel
			uint8_t r, g, b, a;

			// For paletted images, pixel currently is the index into the palette, so we actually
			// need the color at that index
			if (pal4) {
				memcpy(&pixel, palette + ((pixel & 0xF) * 4), 4);
			} else if (pal8) {
				memcpy(&pixel, palette + ((pixel & 0xFF) * 4), 4);
			}

			// Extract RGBA using the previously calculated masks and shift values.
			r = (uint8_t) ((pixel & redMask) >> rShift);
			g = (uint8_t) ((pixel & greenMask) >> gShift);
			b = (uint8_t) ((pixel & blueMask) >> bShift);

			// If there's no alpha channel bit alphaOffset is != -1, assume 255 (opaque)
			if (alphaChannel) {
				a = (uint8_t) ((pixel & alphaMask) >> aShift);
			} else {
				a = 255;
			}

			// Write the RGBA data into dest
			if (redOffset != -1)
				dest[destIdx + redOffset] = r;
			if (greenOffset != -1)
				dest[destIdx + greenOffset] = g;
			if (blueOffset != -1)
				dest[destIdx + blueOffset] = b;
			if (alphaOffset != -1)
				dest[destIdx + alphaOffset] = a;
		}
	}

	// If the texture was compressed, data is an array temporarily created here. Delete it!
	if (compression != NONE) {
		delete[] data;
	}
}


TXDTexture* TXDTexture::generateMipmap() const
{
	return new TXDTexture(diffuseName, alphaName, rasterFormat, compression, width/2, height/2,
			bytesPerPixel, (int8_t) (mipmapCount-1), alphaChannel, uWrap, vWrap, filterFlags);
}

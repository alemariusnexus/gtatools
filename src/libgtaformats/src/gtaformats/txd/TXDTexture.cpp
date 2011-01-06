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

#include "TXDTexture.h"
#include "TXDException.h"
#include "../util/OutOfBoundsException.h"
#include <cstring>
#include <cstdio>
#include <cmath>

#ifdef GTAFORMATS_ENABLE_SQUISH
#include <squish.h>
using namespace squish;
#endif



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


/*TXDTexture::TXDTexture(const char* diffuseName, const char* alphaName, int32_t rasterFormat,
			TXDCompression compression, int16_t width, int16_t height, int8_t bpp, int8_t mipmapCount,
			bool alphaChannel, int8_t uWrap, int8_t vWrap, int16_t filterFlags)
		: rasterFormat(rasterFormat), compression(compression), width(width), height(height),
		  bytesPerPixel(bpp), mipmapCount(mipmapCount), alphaChannel(alphaChannel), uWrap(uWrap),
		  vWrap(vWrap), filterFlags(filterFlags)
{
	strncpy(this->diffuseName, diffuseName, 32);
	strncpy(this->alphaName, alphaName, 32);
}*/


TXDTexture::TXDTexture(InputStream* stream, long long& bytesRead)
{
	char skipBuf[4];

	int32_t platformId;
	int32_t alphaFourCC;
	int8_t dxtType;

	char buf[16];
	stream->read(buf, 8);

	platformId = *((int32_t*) buf);
	filterFlags = *((int16_t*) (buf+4));
	uWrap = *((int8_t*) (buf+6));
	vWrap = *((int8_t*) (buf+7));

	stream->read(diffuseName, 32);
	stream->read(alphaName, 32);

	stream->read(buf, 16);

	rasterFormat = *((int32_t*) buf);
	alphaFourCC = *((int32_t*) (buf+4));
	width = *((int16_t*) (buf+8));
	height = *((int16_t*) (buf+10));
	bytesPerPixel = *((int8_t*) (buf+12));
	mipmapCount = *((int8_t*) (buf+13));
	dxtType = *((int8_t*) (buf+15));

	bytesPerPixel /= 8;

	bytesRead += 88;

	if (platformId == 9) {
		alphaChannel = (dxtType == 9  ||  dxtType == 1);
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


bool TXDTexture::canConvert() const
{
	return true;
}


void TXDTexture::convert(uint8_t* dest, const uint8_t* src, int mipmap, TXDMirrorFlags mirror,
		int8_t bpp, int redOffset, int greenOffset, int blueOffset, int alphaOffset) const
{
	if (mipmap >= mipmapCount) {
		throw OutOfBoundsException(mipmap, __FILE__, __LINE__);
	}

	int w = width / pow(2, mipmap);
	int h = height / pow(2, mipmap);

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
#		ifdef GTAFORMATS_ENABLE_SQUISH
			uint8_t* squishDest;

			if (	bpp == 4
					&&  redOffset == 0
					&&  greenOffset == 1
					&&  blueOffset == 2
					&&  alphaOffset == 3
					&&  mirror == MIRROR_NONE
			) {
				squishDest = dest;
			} else {
				squishDest = new uint8_t[w*h*4];
			}

			if (getCompression() == DXT1) {
				DecompressImage(squishDest, w, h, src, kDxt1);
			} else {
				DecompressImage(squishDest, w, h, src, kDxt3);
			}

			if (squishDest == dest) {
				return;
			}

			data = squishDest;

			// After decompression with Squish, data is stored as R8G8B8A8
			redMask = 0xFF;
			greenMask = 0xFF00;
			blueMask = 0xFF0000;
			alphaMask = 0xFF000000;
			srcBpp = 4;
#		else
			throw TXDException("Attempt to call TXDTexture::convert() for a DXT-compressed texture, but DXT "
					"is not supported because GTAFORMATS_ENABLE_SQUISH was turned off during compilation");
#		endif
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
	for (int16_t y = 0 ; y < h ; y++) {
		for (int16_t x = 0 ; x < w ; x++) {
			// The index of the new pixel inside the dest array
			int destIdx = (y*w + x) * bpp;

			// The index of the current pixel inside the data array
			int srcIdx = (y*w + x);

			// To mirror each pixel, we choose another index in the data array.
			if (mirror == MIRROR_BOTH) {
				//srcIdx = (width*height) - srcIdx;
				srcIdx = INDEX_MIRROR_BOTH(srcIdx, w, h);
			} else if (mirror == MIRROR_HORIZONTAL) {
				//srcIdx = 2*width*(srcIdx/width) + width - srcIdx - 1;
				srcIdx = INDEX_MIRROR_HORIZONTAL(srcIdx, w, h);
			} else if (mirror == MIRROR_VERTICAL) {
				//srcIdx = width*height - 2*width*(srcIdx/width) - width + srcIdx;
				srcIdx = INDEX_MIRROR_VERTICAL(srcIdx, w, h);
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


int TXDTexture::computeDataSize() const
{
	int baseSize = width * height;

	if (compression == DXT1) {
		baseSize /= 2;
	} else if (compression == NONE) {
		baseSize *= bytesPerPixel;
	}

	int size = 0;

	for (uint8_t i = 0 ; i < mipmapCount ; i++) {
		size += baseSize;
		baseSize /= 4;
	}

	if ((rasterFormat & TXD_FORMAT_EXT_PAL4)  !=  0) {
		size += 16*4;
	}
	if ((rasterFormat & TXD_FORMAT_EXT_PAL8)  !=  0) {
		size += 256*4;
	}

	return size;
}


int TXDTexture::computeMipmapDataSize(int mipmap) const
{
	int baseSize = width * height;

	if (compression == DXT1) {
		baseSize /= 2;
	} else if (compression == NONE) {
		baseSize *= bytesPerPixel;
	}

	return baseSize / pow(4, mipmap);
}



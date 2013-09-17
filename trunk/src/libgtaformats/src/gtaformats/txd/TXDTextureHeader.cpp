/*
	Copyright 2010-2013 David "Alemarius Nexus" Lerch

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

#include "TXDTextureHeader.h"
#include "TXDException.h"
#include "../util/OutOfBoundsException.h"
#include <cstring>
#include <cstdio>
#include <cmath>
#include <algorithm>
#include <cassert>

using std::min;

#ifdef GTAFORMATS_ENABLE_SQUISH
#include <squish.h>
using namespace squish;
#endif



void TxdGetRasterFormatName(char* dest, int32_t rasterFormat) {
	switch (rasterFormat & 0xF00) {
	case RasterFormatA1R5G5B5:
		strcpy(dest, "A1R5G5B5");
		break;
	case RasterFormatB8G8R8:
		strcpy(dest, "B8G8R8");
		break;
	case RasterFormatB8G8R8A8:
		strcpy(dest, "B8G8R8A8");
		break;
	case RasterFormatR4G4B4A4:
		strcpy(dest, "R4G4B4A4");
		break;
	case RasterFormatR5G5B5:
		strcpy(dest, "R5G5B5");
		break;
	case RasterFormatR5G6B5:
		strcpy(dest, "R5G6B5");
		break;
	case RasterFormatLUM8:
		strcpy(dest, "LUM8");
		break;
	case RasterFormatR8G8B8A8:
		strcpy(dest, "R8G8B8A8");
		break;
	case RasterFormatDefault:
		strcpy(dest, "DEFAULT");
		break;
	default:
		assert(false);
		break;
	}
}


void TxdGetRasterFormatDescription(char* dest, int32_t rasterFormat)
{
	TxdGetRasterFormatName(dest, rasterFormat);

	if ((rasterFormat & RasterFormatEXTMipmap)  !=  0) {
		strcat(dest, ",M");
	}
	if ((rasterFormat & RasterFormatEXTAutoMipmap)  !=  0) {
		strcat(dest, ",AM");
	}
	if ((rasterFormat & RasterFormatEXTPAL4)  !=  0) {
		strcat(dest, ",P4");
	}
	if ((rasterFormat & RasterFormatEXTPAL8)  !=  0) {
		strcat(dest, ",P8");
	}
}


TXDTextureHeader::TXDTextureHeader(const CString& diffuseName, int32_t rasterFormat,
		TXDCompression compression, int16_t w, int16_t h)
		: width(w), height(h), mipmapCount(1), alphaChannelUsed(false), uWrap(WrapNone),
		  vWrap(WrapNone), filterFlags(FilterNone)
{
	setRasterFormat(rasterFormat, compression);
	setDiffuseName(diffuseName);
	alphaName = CString("");
}


TXDTextureHeader::TXDTextureHeader(const TXDTextureHeader& other)
		: rasterFormat(other.rasterFormat), compression(other.compression), width(other.width),
		  height(other.height), mipmapCount(other.mipmapCount),
		  alphaChannelUsed(other.alphaChannelUsed), uWrap(other.uWrap), vWrap(other.vWrap),
		  filterFlags(other.filterFlags)
{
	diffuseName = other.diffuseName;
	alphaName = other.alphaName;
}


void TXDTextureHeader::getFormat(char* dest) const
{
	char compStr[8];
	char formatStr[32];

	switch (compression) {
	case NONE:
		strcpy(compStr, "un");
		break;
	case DXT1:
		strcpy(compStr, "DXT1-");
		break;
	case DXT3:
		strcpy(compStr, "DXT3-");
		break;
	case PVRTC2:
		strcpy(compStr, "PVRTC2-");
		break;
	case PVRTC4:
		strcpy(compStr, "PVRTC4-");
		break;
	default:
		assert(false);
	}

	char palStr[16];

	if ((rasterFormat & RasterFormatEXTPAL4)  !=  0) {
		strcpy(palStr, "4bit-paletted");
	} else if ((rasterFormat & RasterFormatEXTPAL8)  !=  0) {
		strcpy(palStr, "8bit-paletted");
	} else {
		strcpy(palStr, "unpaletted");
	}

	TxdGetRasterFormatDescription(formatStr, rasterFormat);

	sprintf(dest, "%dx%d@%d %scompressed %s containing %d mipmaps with%s alpha in format %s",
			width, height, getBytesPerPixel(), compStr, palStr, mipmapCount, alphaChannelUsed ? "" : "out",
			formatStr);
}


int TXDTextureHeader::computeDataSize() const
{
	int size = 0;

	for (uint8_t i = 0 ; i < mipmapCount ; i++) {
		size += computeMipmapDataSize(i);
	}

	if ((rasterFormat & RasterFormatEXTPAL4)  !=  0) {
		size += 16*4;
	}
	if ((rasterFormat & RasterFormatEXTPAL8)  !=  0) {
		size += 256*4;
	}

	return size;
}


int TXDTextureHeader::computeMipmapDataSize(int mipmap) const
{
	int8_t bpp = getBytesPerPixel();

	float scale = ceil(pow((float) 2, (float) mipmap));
	int mipW = (int) (width / scale);
	int mipH = (int) (height / scale);

	if (compression == DXT1  ||  compression == DXT3) {
		if (mipW < 4)
			mipW = 4;
		if (mipH < 4)
			mipH = 4;
	}

	int mipSize = mipW * mipH;

	switch (compression) {
	case NONE:
		mipSize *= bpp;
		break;
	case DXT1:
	case PVRTC4:
		mipSize /= 2;
		break;
	case DXT3:
		// Size already correct
		break;
	case PVRTC2:
		mipSize /= 4;
		break;
	}

	return mipSize;
}


int8_t TXDTextureHeader::getBytesPerPixel() const
{
	if ((rasterFormat & RasterFormatEXTPAL4) != 0  ||  (rasterFormat & RasterFormatEXTPAL8) != 0) {
		return 1;
	} else {
		switch (getRasterFormat()) {
		case RasterFormatB8G8R8A8:
		case RasterFormatR8G8B8A8:
		case RasterFormatB8G8R8:
			return 4;

		case RasterFormatA1R5G5B5:
		case RasterFormatR4G4B4A4:
		case RasterFormatR5G5B5:
		case RasterFormatR5G6B5:
			return 2;

		case RasterFormatLUM8:
			return 1;

		default:
			// TODO What's the default BPP?
			return 0;
		}
	}
}


void TXDTextureHeader::setRasterFormat(int32_t rasterFormat, TXDCompression compression)
{
	if (rasterFormat == RasterFormatDefault) {
		switch (compression) {
		case DXT1:
			rasterFormat = RasterFormatR5G6B5;
			break;
		case DXT3:
			rasterFormat = RasterFormatR4G4B4A4;
			break;
		case PVRTC2:
		case PVRTC4:
			break;
		case NONE:
			throw TXDException("Raster format DEFAULT is invalid for uncompressed textures!",
					__FILE__, __LINE__);
		}
	}

	this->rasterFormat = rasterFormat;
	this->compression = compression;

	if (calculateMaximumMipmapLevel() < 0) {
		throw TXDException("Invalid texture dimensions for this format!");
	}
}


void TXDTextureHeader::setDiffuseName(const CString& name)
{
	if (name.length() > 31) {
		throw TXDException("Texture diffuse name too long. Maximum length is 31 characters plus null "
				"terminator.", __FILE__, __LINE__);
	}

	diffuseName = name;
}


void TXDTextureHeader::setAlphaName(const CString& name)
{
	if (name.length() > 31) {
		throw TXDException("Texture alpha name too long. Maximum length is 31 characters plus null "
				"terminator.", __FILE__, __LINE__);
	}

	alphaName = name;
}


int8_t TXDTextureHeader::calculateMaximumMipmapLevel()
{
	int minW, minH;

	switch (compression) {
	case DXT1:
	case DXT3:
		minW = 4;
		minH = 4;
		break;
	case PVRTC2:
		minW = 16;
		minH = 8;
		break;
	case PVRTC4:
		minW = 8;
		minH = 8;
		break;
	default:
		minW = 1;
		minH = 1;
	}

	return min(-(log10((float) minW/width)/log10(2.0f)), -(log10((float) minH/height)/log10(2.0f)));
}


void TXDTextureHeader::fixMipmapCount()
{
	mipmapCount = min<int8_t>((int8_t) calculateMaximumMipmapLevel()+1, mipmapCount);
}



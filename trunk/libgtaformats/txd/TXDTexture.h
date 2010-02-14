/*
 * TXDTexture.h
 *
 *  Created on: 23.01.2010
 *      Author: alemariusnexus
 */

#ifndef TXDTEXTURE_H_
#define TXDTEXTURE_H_

#include "../config.h"
#include <istream>

using std::istream;

#define TXD_FORMAT_A1R5G5B5 0x100
#define TXD_FORMAT_R5G6B5 0x200
#define TXD_FORMAT_R4G4B4A4 0x300
#define TXD_FORMAT_LUM8 0x400
#define TXD_FORMAT_B8G8R8A8 0x500
#define TXD_FORMAT_B8G8R8 0x600
#define TXD_FORMAT_R5G5B5 0xA00

#define TXD_FORMAT_EXT_AUTO_MIPMAP 0x1000
#define TXD_FORMAT_EXT_PAL8 0x2000
#define TXD_FORMAT_EXT_PAL4 0x4000
#define TXD_FORMAT_EXT_MIPMAP 0x8000

#define TXD_FILTER_NONE 0x0
#define TXD_FILTER_NEAREST 0x1
#define TXD_FILTER_LINEAR 0x2
#define TXD_FILTER_MIP_NEAREST 0x3
#define TXD_FILTER_MIP_LINEAR 0x4
#define TXD_FILTER_LINEAR_MIP_NEAREST 0x5
#define TXD_FILTER_LINEAR_MIP_LINEAR 0x6

#define TXD_WRAP_NONE 0x0
#define TXD_WRAP_WRAP 0x0
#define TXD_WRAP_MIRROR 0x0
#define TXD_WRAP_CLAMP 0x0


void TxdGetRasterFormatName(char* dest, int32_t rasterFormat);


enum TXDCompression {
	NONE = 0, DXT1 = 1, DXT3 = 3
};
enum TXDMirrorFlags {
	MIRROR_NONE, MIRROR_HORIZONTAL, MIRROR_VERTICAL, MIRROR_BOTH
};


class TXDTexture {
public:
	TXDTexture(istream* stream, istream::streamoff& bytesRead);
	void getColorMasks(int32_t& redMask, int32_t& greenMask, int32_t& blueMask, int32_t& alphaMask) const;
	void getFormat(char* dest) const;

	const char* getDiffuseName() const { return diffuseName; }
	const char* getAlphaName() const { return alphaName; }
	int32_t getRasterFormat() const { return rasterFormat & 0x0F00; }
	int32_t getFullRasterFormat() const { return rasterFormat; }
	int32_t getRasterFormatExtension() const { return rasterFormat & 0xF000; }
	TXDCompression getCompression() const { return compression; }
	int16_t getWidth() const { return width; }
	int16_t getHeight() const { return height; }
	int8_t getBytesPerPixel() const { return bytesPerPixel; }
	int8_t getMipmapCount() const { return mipmapCount; }
	bool hasAlphaChannel() const { return alphaChannel; }
	int8_t getUWrapFlags() const { return uWrap; }
	int8_t getVWrapFlags() const { return vWrap; }
	int16_t getFilterFlags() const { return filterFlags; }

	bool canConvert();
	void convert(uint8_t* dest, const uint8_t* src, TXDMirrorFlags mirror = MIRROR_HORIZONTAL,
			int8_t bpp = 4, int redOffset = 0, int greenOffset = 1, int blueOffset = 2,
			int alphaOffset = 3) const;
	TXDTexture* generateMipmap() const;

private:
	TXDTexture(const char* diffuseName, const char* alphaName, int32_t rasterFormat,
			TXDCompression compression, int16_t width, int16_t height, int8_t bpp, int8_t mipmapCount,
			bool alphaChannel, int8_t uWrap, int8_t vWrap, int16_t filterFlags);

private:
	char diffuseName[32];
	char alphaName[32];
	int32_t rasterFormat;
	TXDCompression compression;
	int16_t width;
	int16_t height;
	int8_t bytesPerPixel;
	int8_t mipmapCount;
	bool alphaChannel;
	int8_t uWrap;
	int8_t vWrap;
	int16_t filterFlags;
};


#endif /* TXDTEXTURE_H_ */

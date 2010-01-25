/*
 * TXDTexture.h
 *
 *  Created on: 23.01.2010
 *      Author: alemariusnexus
 */

#ifndef TXDTEXTURE_H_
#define TXDTEXTURE_H_

#include "config.h"
#include <istream>

using std::istream;

#define TXD_FORMAT_A1R5G5B5 0x100
#define TXD_FORMAT_R5G6B5 0x200
#define TXD_FORMAT_R4G4B4A4 0x300
#define TXD_FORMAT_B8G8R8A8 0x500
#define TXD_FORMAT_B8G8R8 0x600
#define TXD_FORMAT_R5G5B5 0xA00


enum TXDCompression {
	NONE = 0, DXT1 = 1, DXT3 = 3
};

enum TXDMirrorFlags {
	MIRROR_NONE, MIRROR_HORIZONTAL, MIRROR_VERTICAL, MIRROR_BOTH
};


void TxdGetRasterFormatName(char* dest, int32_t rasterFormat);


class TXDTexture {
public:
	TXDTexture(const char* diffuseName, const char* alphaName, int32_t rasterFormat,
			TXDCompression compression, int16_t width, int16_t height, int8_t bpp, int8_t mipmapCount);
	TXDTexture(istream* stream);
	void getColorMasks(int32_t& redMask, int32_t& greenMask, int32_t& blueMask, int32_t& alphaMask);
	void getFormat(char* dest);

	const char* getDiffuseName() { return diffuseName; }
	const char* getAlphaName() { return alphaName; }
	int32_t getRasterFormat() { return rasterFormat; }
	TXDCompression getCompression() { return compression; }
	int16_t getWidth() { return width; }
	int16_t getHeight() { return height; }
	int8_t getBytesPerPixel() { return bytesPerPixel; }
	int8_t getMipmapCount() { return mipmapCount; }
	bool hasAlphaChannel() { return alphaChannel; }

	void convert(uint8_t* dest, const uint8_t* src, TXDMirrorFlags mirror = MIRROR_HORIZONTAL,
			int8_t bpp = 4, int redOffset = 0, int greenOffset = 1, int blueOffset = 2,
			int alphaOffset = 3);

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
};


#endif /* TXDTEXTURE_H_ */

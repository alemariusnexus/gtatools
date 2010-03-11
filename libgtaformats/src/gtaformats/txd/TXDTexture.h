/*
 * TXDTexture.h
 *
 *  Created on: 23.01.2010
 *      Author: alemariusnexus
 */

#ifndef TXDTEXTURE_H_
#define TXDTEXTURE_H_

#include "../gf_config.h"
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
#define TXD_WRAP_WRAP 0x1
#define TXD_WRAP_MIRROR 0x2
#define TXD_WRAP_CLAMP 0x3


void TxdGetRasterFormatName(char* dest, int32_t rasterFormat);


enum TXDCompression {
	NONE = 0, DXT1 = 1, DXT3 = 3
};
enum TXDMirrorFlags {
	MIRROR_NONE, MIRROR_HORIZONTAL, MIRROR_VERTICAL, MIRROR_BOTH
};



/**	\brief This class represents the general information of a TXD texture.
 *
 * 	It is read by TXDArchive and can be used to fetch information about the texture (the header data is
 * 	kept here) and to convert the raw data to a more convenient format.
 */
class TXDTexture {
public:
	/**	\brief This method reads a texture header from the given stream.
	 *
	 * 	This may be used internally only by TXDArchive.
	 *
	 *	@param stream The stream to read from.
	 *	@param bytesRead This will be increased by the number of bytes read by this constructor.
	 */
	TXDTexture(istream* stream, long long& bytesRead);

	/**	\brief Returns the RGBA masks of the raw data of this texture.
	 *
	 * 	For plain textures, these masks can be used directly to extract the color values from each pixel
	 * 	(You shouldn't do that if you don't really know what you do, though). For compressed images, this
	 * 	method does the same as for plain textures (and the values are therefore not really useful). For
	 * 	paletted images, this returns the masks used for the palette color entries.
	 *
	 *	@param redMask The red mask.
	 *	@param greenMask The green mask.
	 *	@param blueMask The blue mask.
	 *	@param alphaMask The alpha (transparency) mask.
	 */
	void getColorMasks(int32_t& redMask, int32_t& greenMask, int32_t& blueMask, int32_t& alphaMask) const;

	/**	\brief Returns a small describing string of the raster format of this texture.
	 *
	 * 	This can be used for debugging or whatever.
	 *
	 *	@param dest Where to store the information.
	 */
	void getFormat(char* dest) const;

	/**	\brief Returns the diffuse name.
	 *
	 *	@return The diffuse name.
	 */
	const char* getDiffuseName() const { return diffuseName; }

	/**	\brief Returns the alpha texture name.
	 *
	 *	@return The alpha name.
	 */
	const char* getAlphaName() const { return alphaName; }

	/**	\brief Returns the plain raster format (without extension) of this textures.
	 *
	 * 	This is the value of one of the TXD_FORMAT_* constants.
	 *
	 *	@return The raster format without extensions.
	 */
	int32_t getRasterFormat() const { return rasterFormat & 0x0F00; }

	/**	\brief Returns the full raster format (extension included) of this texture.
	 *
	 * 	This is a bitwise-ORed combination of one of the TXD_FORMAT_* constants and several of the
	 * 	TXD_FORMAT_EXT_* constants.
	 *
	 *	@return The full raster format (extension included).
	 */
	int32_t getFullRasterFormat() const { return rasterFormat; }

	/**	\brief Returns the raster format extension of this texture.
	 *
	 * 	This is a bitwise-ORed combination of the TXD_FORMAT_EXT_* constants.
	 *
	 *	@return The raster format extension.
	 */
	int32_t getRasterFormatExtension() const { return rasterFormat & 0xF000; }

	/**	\brief Returns the type of compression used for this texture.
	 *
	 *	@return The compression type.
	 */
	TXDCompression getCompression() const { return compression; }

	/**	\brief Returns the texture's width in pixels.
	 *
	 *	@return The texture witdth in pixels.
	 */
	int16_t getWidth() const { return width; }

	/**	\brief Returns the texture's height in pixels.
	 *
	 *	@return The texture's height in pixels.
	 */
	int16_t getHeight() const { return height; }


	int8_t getBytesPerPixel() const { return bytesPerPixel; }

	/**	\brief Returns the number of mipmaps of this texture.
	 *
	 *	@return The mipmap count.
	 */
	int8_t getMipmapCount() const { return mipmapCount; }

	/**	\brief Returns whether the alpha channel of this texture is used.
	 *
	 * 	This is determined by just looking at a value in the header, so no guarantee can be given that
	 * 	there really is no alpha channel.
	 *
	 *	@return Whether the header says that alpha channels should be used.
	 */
	bool hasAlphaChannel() const { return alphaChannel; }

	/**	\brief Returns flags for texture coordinate U-wrapping.
	 *
	 *	This is one of the TXD_WRAP_* constants.
	 *
	 *	@return The U-wrapping flags.
	 */
	int8_t getUWrapFlags() const { return uWrap; }

	/**	\brief Returns flags for texture coordinate V-wrapping.
	 *
	 *	This is one of the TXD_WRAP_* constants.
	 *
	 *	@return The V-wrapping flags.
	 */
	int8_t getVWrapFlags() const { return vWrap; }

	/**	\brief Returns this texture's filter flags.
	 *
	 *	@return The filter flags.
	 */
	int16_t getFilterFlags() const { return filterFlags; }

	/**	\brief Returns whether this texture can be converted using convert().
	 *
	 * 	This currently returns true for all textures. This just means that there is currently no TXD
	 * 	construct known which can not be converted, though. It is possible that there is something that
	 * 	can not be handeled and for which this method returns true.
	 *
	 *	@return Whether this texture can be converted using convert().
	 */
	bool canConvert();

	/**	\brief This converts the raw data of this texture to a more suitable form.
	 *
	 * 	This method can be used to do several transformations at once, which yields to better performance.
	 * 	If the texture is compressed, it will automatically be uncompressed. If it was paletted, it will
	 * 	be automatically converted to a non-paletted image. You can give some parameters to change the
	 * 	output format and you can mirror the texture horizontally, vertically or in both directions.
	 * 	Be sure to choose a correct size for the dest parameter to not cause buffer overflow. Generally,
	 * 	it should be width*height*bpp bytes in size.
	 * 	The offset parameters give the location in the output buffer for the color components, relative to
	 * 	the offset of the current pixel. Be sure not to choose it bigger or equal to bpp!
	 *
	 *	@param dest Where the new data shall be written to.
	 *	@param src The raw data of this texture.
	 *	@param mirror In which direction to mirror the texture. This can be one of MIRROR_NONE,
	 *		MIRROR_HORIZONTAL, MIRROR_VERTICAL or MIRROR_BOTH. MIRROR_HORIZONTAL is the default.
	 *	@param bpp The number of bytes per pixel in the output buffer.
	 *	@param redOffset The offset in the output buffer to store the red component.
	 *	@param greenOffset The offset in the output buffer to store the green component.
	 *	@param blueOffset The offset in the output buffer to store the blue component.
	 *	@param alphaOffset The offset in the output buffer to store the alpha component.
	 */
	void convert(uint8_t* dest, const uint8_t* src, TXDMirrorFlags mirror = MIRROR_HORIZONTAL,
			int8_t bpp = 4, int redOffset = 0, int greenOffset = 1, int blueOffset = 2,
			int alphaOffset = 3) const;

	/**	\brief Generates a new TXDTexture object which is suitable to read the next bitmap of this texture.
	 *
	 *	@return A TXDTexture representing the data of the next bitmap of this texture.
	 */
	TXDTexture* generateMipmap() const;

private:
	/**	\brief Creates a TXDTexture from already available information.
	 *
	 * 	This is used internally only (by generateMipmap()).
	 *
	 *	@param diffuseName The diffuse name.
	 *	@param alphaName The alpha name.
	 *	@param rasterFormat The raster format.
	 *	@param compression The compression type.
	 *	@param width The width.
	 *	@param height The height.
	 *	@param bpp The number of bytes per pixel.
	 *	@param mipmapCount The number of mipmaps.
	 *	@param alphaChannel Whether the alpha channel is used.
	 *	@param uWrap The texture U-wrapping flags.
	 *	@param vWrap The texture V-wrapping flags.
	 *	@param filterFlags The filter flags.
	 */
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

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

#ifndef TXDTEXTUREHEADER_H_
#define TXDTEXTUREHEADER_H_

#include <gtaformats/config.h>
#include "../util/CString.h"
#include <istream>

using std::istream;


#define INDEX_MIRROR_HORIZONTAL(i,w,h) (2*(w)*((i)/(w)) + (w) - (i) - 1)
#define INDEX_MIRROR_VERTICAL(i,w,h) ((w)*(h) - 2*(w)*((i)/(w)) - (w) + (i))
#define INDEX_MIRROR_BOTH(i,w,h) ((w)*(h) - (i))


void TxdGetRasterFormatName(char* dest, int32_t rasterFormat);
void TxdGetRasterFormatDescription(char* dest, int32_t rasterFormat);


/**	\brief The different compression methods which are supported.
 *
 *	Note that not all compressions listed here are supported by the original TXD format. In fact, only DXT1
 *	and DXT3 are supported, all other compressions will not work with the original game or tools. The
 *	PVRTC2 and PVRTC4 compressions are supported as extensions and will only work in gtatools.
 */
enum TXDCompression {
	NONE = 0,		//!< No compression
	DXT1 = 1,		//!< DXT1 compression (S3TC)
	DXT3 = 3,		//!< DXT3 compression (S3TC)
	PVRTC2 = 50,	//!< PVRTC compression with 2 bits per pixel.
	PVRTC4 = 51		//!< PVRTC compression with 4 bits per pixel.
};

/**	\brief The different raster formats which are supported.
 *
 * 	The raster format consists of the base format and one or more extension flags. Extensions start with
 * 	RasterFormatEXT.
 *
 * 	Note that not all raster format listed here are supported by the original TXD format: R8G8B8A8 is not
 * 	supported by the original game or tools designed for it, but is used as an extension of gtatools, and as
 * 	an intermediate format for conversion (but can still be written to TXD files).
 */
enum TXDRasterFormat {
	RasterFormatDefault = 0x000,	/*!< Default raster format. What this format means in TXD files is
	 	 	 	 	 	 	 	 	 	 currently not known. When reading TXD files, this format may only
	 	 	 	 	 	 	 	 	 	 appear for PVRTC-compressed textures. */
	RasterFormatA1R5G5B5 = 0x100,	//!< 1 bit alpha and 5 bit for each red, green and blue (DXT1/3).
	RasterFormatR5G6B5 = 0x200,		//!< 5 bits red, 6 bits green and 5 bits blue (DXT1/3).
	RasterFormatR4G4B4A4 = 0x300,	//!< 4 bits red, green, blue and alpha each (DXT1/3).
	RasterFormatLUM8 = 0x400,		//!< 8 bit luminance (for black-white).
	RasterFormatB8G8R8A8 = 0x500,	//!< 8 bits blue, green, red and alpha each (in this order).
	RasterFormatB8G8R8 = 0x600,		//!< 8 bits blue, green and red each (in this order).
	RasterFormatR5G5B5 = 0xA00,		//!< 5 bits red, green and blue each.

	RasterFormatR8G8B8A8 = 0xF00,	//!< 8 bits red, green, blue and alpha each (in this order).

	RasterFormatEXTAutoMipmap = 0x1000,	//!< Mipmaps shall be automatically generated.
	RasterFormatEXTPAL8 = 0x2000,		//!< 8 bit palette with 256 entries included.
	RasterFormatEXTPAL4 = 0x4000,		//!< 4 bit palette with 16 entries included.
	RasterFormatEXTMipmap = 0x8000,		//!< Mipmaps included.

	RasterFormatMask = 0xF00,		//!< Mask to strip out the base raster format.
	RasterFormatEXTMask = 0xF000	//!< Mask to strip out the raster format extension.
};

enum TXDFilterFlags {
	FilterNone = 0,
	FilterNearest = 1,
	FilterLinear = 2,
	FilterMipNearest = 3,
	FilterMipLinear = 4,
	FilterLinearMipNearest = 5,
	FilterLinearMipLinear = 6
};

enum TXDWrappingMode {
	WrapNone = 0,
	WrapWrap = 1,
	WrapMirror = 2,
	WrapClamp = 3
};



/**	\brief This class represents the header information of a TXD texture.
 *
 *	A TXD texture header by default has the following properties:
 *		- Raster format: The format used for storing the pixel data. See TXDRasterFormat for possible values
 *		- Compression: A texture can be compressed in several ways, see TXDCompression
 *		- Diffuse and alpha name
 *		- Width and height in pixels
 *		- A number of bytes per pixel, usually given by the raster format, but can be set manually
 *		- The number of mipmaps, where 1 means only a base image
 *		- Whether it has an alpha channel
 *		- The wrapping flags for the U and V texture coordinates
 *		- The filter flags
 */
class TXDTextureHeader {
public:
	/**	\brief This method reads a texture header from the given stream.
	 *
	 * 	This may be used internally only by TXDArchive.
	 *
	 *	@param stream The stream to read from.
	 *	@param bytesRead This will be increased by the number of bytes read by this constructor.
	 */
	//TXDTextureHeader(istream* stream, long long& bytesRead);

	TXDTextureHeader(const CString& diffuseName, int32_t rasterFormat, TXDCompression compression,
			int16_t w, int16_t h);

	TXDTextureHeader(const TXDTextureHeader& other);

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
	CString getDiffuseName() const { return diffuseName; }

	/**	\brief Returns the alpha texture name.
	 *
	 *	@return The alpha name.
	 */
	CString getAlphaName() const { return alphaName; }

	/**	\brief Returns the plain raster format (without extension) of this textures.
	 *
	 * 	This is the value of one of the TXD_FORMAT_* constants.
	 *
	 *	@return The raster format without extensions.
	 */
	int32_t getRasterFormat() const { return rasterFormat & RasterFormatMask; }

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
	int32_t getRasterFormatExtension() const { return rasterFormat & RasterFormatEXTMask; }

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


	int8_t getBytesPerPixel() const;

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
	bool isAlphaChannelUsed() const { return alphaChannelUsed; }

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

	/**	\brief Updates the raster format of this texture header.
	 *
	 * 	Note that this method does not prevent you from using invalid combinations of formats.
	 *
	 *	If you use a compression, you may use RasterFormatDefault as a raster format. It will then be
	 *	replaced by a raster format which fits to the compression used. Any other use of RasterFormatDefault
	 *	is an error.
	 *
	 *	@param rasterFormat The raster format of the texture. May be RasterFormatDefault for compressed
	 *		textures.
	 *	@param compression The compression to be used.
	 */
	void setRasterFormat(int32_t rasterFormat, TXDCompression compression = NONE);

	/**	\brief Sets the diffuse name of this texture.
	 *
	 *	@param name The new diffuse name. It may not be longer than 31 characters + null terminator, or an
	 *		exception will be thrown.
	 */
	void setDiffuseName(const CString& name);

	/**	\brief Sets the alpha name of this texture.
	 *
	 *	@param name The new alpha name. It may not be longer than 31 characters + null terminator, or an
	 *		exception will be thrown.
	 */
	void setAlphaName(const CString& name);

	/**	\brief Sets the base dimensions of the texture.
	 *
	 * 	@param w The width in pixels.
	 * 	@param h The height in pixels.
	 */
	void setRasterSize(int32_t w, int32_t h) { width = w; height = h; }

	/**	\brief Sets the mipmap count of this texture.
	 *
	 * 	This method does not set the appropriate raster format extension, so you have to do it manually. Also,
	 * 	it does not prevent you from using an invalid mipmap count. You can use fixMipmapCount() to do this.
	 *
	 * 	@param mmc The mipmap count. 1 means no mipmaps (a single image).
	 */
	void setMipmapCount(int8_t mmc) { mipmapCount = mmc; }

	/**	\brief Enables or disables the alpha channel.
	 *
	 * 	@param alpha Whether to enable the alpha channel.
	 */
	void setAlphaChannelUsed(bool alpha) { alphaChannelUsed = alpha; }

	/**	\brief Sets the wrapping flags of the texture.
	 *
	 * 	@param uWrap The wrapping flags for the U-coordinate.
	 * 	@param vWrap The wrapping flags for the V-coordinate.
	 */
	void setWrappingFlags(int8_t uWrap, int8_t vWrap) { this->uWrap = uWrap; this->vWrap = vWrap; }

	/**	\brief Sets the filter flags of this texture.
	 *
	 * 	@param flags The filter flags.
	 */
	void setFilterFlags(int16_t flags) { filterFlags = flags; }

	/**	\brief Compute the size the raw data of this texture would have.
	 *
	 * 	This is the sum of the sizes of all mipmaps, and the palette if included.
	 *
	 * 	@return The size of the raster data in bytes.
	 */
	int computeDataSize() const;

	/**	\brief Compute the size a single mipmap of this texture would have.
	 *
	 *	@param mipmap The mipmap for which you want to get the size. 0 is the base image.
	 *	@return The number of bytes for the mipmap.
	 */
	int computeMipmapDataSize(int mipmap) const;

	/**	\brief Computes the maximum possible mipmap level.
	 *
	 *	Each mipmap is a scaled version of the original image, with side lengths halved. This means that there
	 *	is a point where the image can't be scaled down any more (which is 1x1 pixels). Also, the compression
	 *	algorithms used require a certain minimum raster size to work properly. This method computes the
	 *	maximum mipmap level which still works and isn't just a duplicate of the ones before.
	 *
	 *	@return The maximum mipmap level. 0 means that the image will have only the base image.
	 */
	int8_t calculateMaximumMipmapLevel();

	/**	\brief Sets the mipmap count to the maximum possible value.
	 *
	 * 	This sets the mipmap count to the value returned by calculateMaximumMipmapLevel() plus one.
	 *
	 * 	@see calculateMaximumMipmapLevel()
	 */
	void fixMipmapCount();

private:
	CString diffuseName;
	CString alphaName;
	int32_t rasterFormat;
	TXDCompression compression;
	int16_t width;
	int16_t height;
	int8_t mipmapCount;
	bool alphaChannelUsed;
	int8_t uWrap;
	int8_t vWrap;
	int16_t filterFlags;
};


#endif /* TXDTEXTUREHEADER_H_ */

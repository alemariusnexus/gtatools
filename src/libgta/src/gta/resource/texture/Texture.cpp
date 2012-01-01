/*
	Copyright 2010-2012 David "Alemarius Nexus" Lerch

	This file is part of libgta.

	libgta is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	libgta is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with libgta.  If not, see <http://www.gnu.org/licenses/>.

	Additional permissions are granted, which are listed in the file
	GPLADDITIONS.
 */

#include "Texture.h"
#include "../../GLException.h"
#include <gtaformats/txd/TXDConverter.h>


Texture::Texture(TXDArchive* txd, TXDTextureHeader* tex)
		: size(0)
{
	uint8_t* data = txd->getTextureData(tex);

	GLException::checkError("Fehler bei 5");

	glGenTextures(1, &glTex);
	glBindTexture(GL_TEXTURE_2D, glTex);

	GLException::checkError("Fehler bei 4");

	GLint uWrap = GL_REPEAT;
	GLint vWrap = GL_REPEAT;

	switch (tex->getUWrapFlags()) {
	case WrapNone:
		uWrap = GL_REPEAT;
		break;
	case WrapWrap:
		uWrap = GL_REPEAT;
		break;
	case WrapMirror:
		uWrap = GL_MIRRORED_REPEAT;
		break;
	case WrapClamp:
		uWrap = GL_CLAMP_TO_EDGE;
		break;
	}

	switch (tex->getVWrapFlags()) {
	case WrapNone:
		vWrap = GL_REPEAT;
		break;
	case WrapWrap:
		vWrap = GL_REPEAT;
		break;
	case WrapMirror:
		vWrap = GL_MIRRORED_REPEAT;
		break;
	case WrapClamp:
		vWrap = GL_CLAMP_TO_EDGE;
		break;
	}

	GLint magFilter, minFilter;

	switch (tex->getFilterFlags()) {
	case FilterLinear:
		magFilter = GL_LINEAR;
		minFilter = GL_LINEAR;
		break;

	case FilterNearest:
		magFilter = GL_NEAREST;
		minFilter = GL_NEAREST;
		break;

	case FilterMipNearest:
		magFilter = GL_NEAREST;
		minFilter = GL_NEAREST_MIPMAP_NEAREST;
		break;

	case FilterMipLinear:
		magFilter = GL_NEAREST;
		minFilter = GL_NEAREST_MIPMAP_LINEAR;
		break;

	case FilterLinearMipNearest:
		magFilter = GL_LINEAR;
		minFilter = GL_LINEAR_MIPMAP_NEAREST;
		break;

	case FilterLinearMipLinear:
		magFilter = GL_LINEAR;
		minFilter = GL_LINEAR_MIPMAP_LINEAR;
		break;

	case FilterNone:
		// Don't know what it is
		magFilter = GL_LINEAR;
		minFilter = GL_LINEAR;
		break;

	default:
		// Unknown filter, use linear
		magFilter = GL_LINEAR;
		minFilter = GL_LINEAR;
	}

	GLException::checkError("Fehler bei 3");

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, uWrap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, vWrap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);

	GLException::checkError("Fehler bei 2");

	// TODO Can we really disable this?
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, tex->getMipmapCount()-1);

	bool mipmapsIncluded = (tex->getRasterFormatExtension() & RasterFormatEXTMipmap) != 0;
	bool mipmapsAuto = (tex->getRasterFormatExtension() & RasterFormatEXTAutoMipmap) != 0;
	int numIncludedMipmaps = mipmapsIncluded ? tex->getMipmapCount() : 1;

#ifndef GTA_USE_OPENGL_ES
	if (mipmapsAuto  &&  !gtaglIsVersionSupported(3, 0)) {
		glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
	}
#endif

	GLException::checkError("Fehler bei 1");

	int16_t w = tex->getWidth();
	int16_t h = tex->getHeight();

	TXDCompression compr = tex->getCompression();

#ifndef GTA_USE_OPENGL_ES
	if (	(compr == DXT1  ||  compr == DXT3)
			&&  gtaglIsExtensionSupported("GL_EXT_texture_compression_s3tc")
	) {
		if (compr == DXT1) {
			uint8_t* dataStart = data;

			int16_t mipW = w;
			int16_t mipH = h;

			for (int i = 0 ; i < numIncludedMipmaps ; i++) {
				if (mipW < 4  ||  mipH < 4) {
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, i-1);
					break;
				}

				glCompressedTexImage2D(GL_TEXTURE_2D, i, GL_COMPRESSED_RGBA_S3TC_DXT1_EXT,
						mipW, mipH, 0, (mipW*mipH)/2, data);
				size += (mipW*mipH)/2;
				data += (mipW*mipH)/2;
				mipW /= 2;
				mipH /= 2;
			}

			delete[] dataStart;
		} else if (compr == DXT3) {
			uint8_t* dataStart = data;

			int16_t mipW = w;
			int16_t mipH = h;

			for (int i = 0 ; i < numIncludedMipmaps ; i++) {
				if (mipW < 4  ||  mipH < 4) {
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, i-1);
					break;
				}

				glCompressedTexImage2D(GL_TEXTURE_2D, i, GL_COMPRESSED_RGBA_S3TC_DXT3_EXT,
						mipW, mipH, 0, mipW*mipH, data);
				size += mipW*mipH;
				data += mipW*mipH;
				mipW /= 2;
				mipH /= 2;
			}

			delete[] dataStart;
		}
	} else
#endif
	if (	(compr == PVRTC2  ||  compr == PVRTC4)
			&&  gtaglIsExtensionSupported("GL_IMG_texture_compression_pvrtc")
	) {
#ifdef GTA_USE_OPENGL_ES
		if (compr == PVRTC2) {
			uint8_t* dataStart = data;

			int16_t mipW = w;
			int16_t mipH = h;

			for (int i = 0 ; i < numIncludedMipmaps ; i++) {
				glCompressedTexImage2D(GL_TEXTURE_2D, i, GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG,
						mipW, mipH, 0, mipW*mipH, data);
				size += (mipW*mipH)/4;
				data += (mipW*mipH)/4;
				mipW /= 2;
				mipH /= 2;
			}

			delete[] dataStart;
		} else if (compr == PVRTC4) {
			//printf("PVRTC?!\n");
			uint8_t* dataStart = data;

			int16_t mipW = w;
			int16_t mipH = h;

			GLException::checkError("Fehler bei 2");

			for (int i = 0 ; i < numIncludedMipmaps ; i++) {
				glCompressedTexImage2D(GL_TEXTURE_2D, i, GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG,
						mipW, mipH, 0, mipW*mipH/2, data);
				size += (mipW*mipH)/2;
				data += (mipW*mipH)/2;
				mipW /= 2;
				mipH /= 2;
			}

			GLException::checkError("Fehler bei 3");

			delete[] dataStart;
		}
#endif
	} else {
		uint8_t* dataStart = data;

		int16_t mipW = w;
		int16_t mipH = h;

		for (int i = 0 ; i < numIncludedMipmaps ; i++) {
			if (	(mipW < 4  ||  mipH < 4)
					&&  (compr == DXT1  ||  compr == DXT3)
			) {
				// TODO See above...
				//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, i-1);
				break;
			}

			uint8_t* pixels = new uint8_t[mipW*mipH*4];

			TXDTextureHeader to = *tex;
			to.setRasterFormat(RasterFormatR8G8B8A8);

			TXDConverter conv;
			conv.convert(*tex, to, data, pixels, i, i);

			glTexImage2D(GL_TEXTURE_2D, i, GL_RGBA, mipW, mipH, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
			size += mipW*mipH*4;
			data += tex->computeMipmapDataSize(i);
			//data += mipW*mipH/bppFraction;

			delete[] pixels;

			mipW /= 2;
			mipH /= 2;
		}

		delete[] dataStart;
	}

	GLException::checkError("Fehler bei 4");

	if (	(tex->getRasterFormatExtension() & RasterFormatEXTAutoMipmap) != 0
#ifndef GTA_USE_OPENGL_ES
			&&  gtaglIsVersionSupported(3, 0)
#endif
	) {
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	GLException::checkError("Fehler bei 5");
}

/*
 * TextureCacheLoader.cpp
 *
 *  Created on: 07.03.2011
 *      Author: alemariusnexus
 */

#include "TextureCacheLoader.h"
#include "TextureCacheEntry.h"
#include <gtaformats/txd/TXDArchive.h>
#include <gtaformats/txd/TXDTexture.h>
#include "../../gl.h"




TextureCacheLoader::TextureCacheLoader(TextureIndexer* indexer)
		: indexer(indexer)
{
}


CacheEntry* TextureCacheLoader::load(hash_t hash)
{
	const TextureIndexEntry* entry = indexer->find(hash);

	TXDArchive txd(*entry->file);

	for (int16_t i = 0 ; i < entry->index ; i++) delete txd.nextTexture();

	TXDTexture* tex = txd.nextTexture();
	uint8_t* data = txd.readTextureData(tex);

	GLuint glTex;
	glGenTextures(1, &glTex);
	glBindTexture(GL_TEXTURE_2D, glTex);

	GLint uWrap = GL_REPEAT;
	GLint vWrap = GL_REPEAT;

	switch (tex->getUWrapFlags()) {
	case TXD_WRAP_NONE:
		uWrap = GL_REPEAT;
		break;
	case TXD_WRAP_WRAP:
		uWrap = GL_REPEAT;
		break;
	case TXD_WRAP_MIRROR:
		uWrap = GL_MIRRORED_REPEAT;
		break;
	case TXD_WRAP_CLAMP:
		uWrap = GL_CLAMP_TO_EDGE;
		break;
	}

	switch (tex->getVWrapFlags()) {
	case TXD_WRAP_NONE:
		vWrap = GL_REPEAT;
		break;
	case TXD_WRAP_WRAP:
		vWrap = GL_REPEAT;
		break;
	case TXD_WRAP_MIRROR:
		vWrap = GL_MIRRORED_REPEAT;
		break;
	case TXD_WRAP_CLAMP:
		vWrap = GL_CLAMP_TO_EDGE;
		break;
	}

	GLint magFilter, minFilter;

	switch (tex->getFilterFlags()) {
	case TXD_FILTER_LINEAR:
		magFilter = GL_LINEAR;
		minFilter = GL_LINEAR;
		break;

	case TXD_FILTER_NEAREST:
		magFilter = GL_NEAREST;
		minFilter = GL_NEAREST;
		break;

	case TXD_FILTER_MIP_NEAREST:
		magFilter = GL_NEAREST;
		minFilter = GL_NEAREST_MIPMAP_NEAREST;
		break;

	case TXD_FILTER_MIP_LINEAR:
		magFilter = GL_NEAREST;
		minFilter = GL_NEAREST_MIPMAP_LINEAR;
		break;

	case TXD_FILTER_LINEAR_MIP_NEAREST:
		magFilter = GL_LINEAR;
		minFilter = GL_LINEAR_MIPMAP_NEAREST;
		break;

	case TXD_FILTER_LINEAR_MIP_LINEAR:
		magFilter = GL_LINEAR;
		minFilter = GL_LINEAR_MIPMAP_LINEAR;
		break;

	case TXD_FILTER_NONE:
		// Don't know what it is
		magFilter = GL_LINEAR;
		minFilter = GL_LINEAR;
		break;

	default:
		// Unknown filter, use linear
		magFilter = GL_LINEAR;
		minFilter = GL_LINEAR;
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, uWrap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, vWrap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);

	// TODO Can we really disable this?
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, tex->getMipmapCount()-1);

	bool mipmapsIncluded = (tex->getRasterFormatExtension() & TXD_FORMAT_EXT_MIPMAP) != 0;
	bool mipmapsAuto = (tex->getRasterFormatExtension() & TXD_FORMAT_EXT_AUTO_MIPMAP) != 0;
	int numIncludedMipmaps = mipmapsIncluded ? tex->getMipmapCount() : 1;

#ifndef GTA_USE_OPENGL_ES
	if (mipmapsAuto  &&  !gtaglIsVersionSupported(3, 0)) {
		glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
	}
#endif

	int16_t w = tex->getWidth();
	int16_t h = tex->getHeight();

	TXDCompression compr = tex->getCompression();
	int size = 0;

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
		} else {
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
	} else {
#endif
		uint8_t* dataStart = data;

		int16_t mipW = w;
		int16_t mipH = h;

		for (int i = 0 ; i < numIncludedMipmaps ; i++) {
			if (mipW < 1  ||  mipH < 1) {
				// TODO See above...
				//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, i-1);
				break;
			}

			uint8_t* pixels = new uint8_t[mipW*mipH*4];
			tex->convert(pixels, data, i, MIRROR_NONE, 4, 0, 1, 2, 3);

			glTexImage2D(GL_TEXTURE_2D, i, GL_RGBA, mipW, mipH, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
			size += mipW*mipH*4;
			data += mipW*mipH*tex->getBytesPerPixel();

			delete[] pixels;

			mipW /= 2;
			mipH /= 2;
		}

		delete[] dataStart;
#ifndef GTA_USE_OPENGL_ES
	}
#endif

	if (	(tex->getRasterFormatExtension() & TXD_FORMAT_EXT_AUTO_MIPMAP) != 0
#ifndef GTA_USE_OPENGL_ES
			&&  gtaglIsVersionSupported(3, 0)
#endif
	) {
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	delete tex;

	TextureCacheEntry* cacheEntry = new TextureCacheEntry(glTex, size);

	return cacheEntry;
}

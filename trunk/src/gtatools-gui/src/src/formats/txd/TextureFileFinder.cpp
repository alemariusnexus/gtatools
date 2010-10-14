/*
 * TextureFileFinder.cpp
 *
 *  Created on: 14.10.2010
 *      Author: alemariusnexus
 */

#include "TextureFileFinder.h"
#include <gtaformats/txd/TXDArchive.h>
#include <gtaformats/txd/TXDTexture.h>



TextureFileFinder::TextureFileFinder(FileFinder* textureBackend, FileFinder* txdBackend)
		: textureBackend(textureBackend), txdBackend(txdBackend)
{
}


bool TextureFileFinder::matches(const File& file)
{
	if (file.guessContentType() != CONTENT_TYPE_TXD) {
		return false;
	}

	if (txdBackend) {
		if (!txdBackend->matches(file)) {
			return false;
		}
	}

	TXDArchive txd(file);

	for (int32_t i = 0 ; i < txd.getTextureCount() ; i++) {
		TXDTexture* tex = txd.nextTexture();
		FilePath pseudoPath(*file.getPath(), tex->getDiffuseName());
		File pseudoFile(&pseudoPath, false);
		bool matches = textureBackend->matches(pseudoFile);
		char* texNameCpy = new char[strlen(tex->getDiffuseName())+1];
		strcpy(texNameCpy, tex->getDiffuseName());
		textureMap[file] = texNameCpy;
		delete tex;

		if (matches) {
			return true;
		}
	}

	return false;
}


bool TextureFileFinder::isInterrupted()
{
	return textureBackend->isInterrupted()  ||  (txdBackend  &&  txdBackend->isInterrupted());
}

/*
	Copyright 2010 David "Alemarius Nexus" Lerch

	This file is part of gtatools-gui.

	gtatools-gui is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	gtatools-gui is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with gtatools-gui.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "TextureFileFinder.h"
#include <gtaformats/txd/TXDArchive.h>
#include <gtaformats/txd/TXDTexture.h>



TextureFileFinder::TextureFileFinder(FileFinder* textureBackend, FileFinder* txdBackend)
		: textureBackend(textureBackend), txdBackend(txdBackend)
{
}


TextureFileFinder::~TextureFileFinder()
{
	QMap<File, char*>::iterator it;

	for (it = textureMap.begin() ; it != textureMap.end() ; it++) {
		delete[] it.value();
	}
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

		if (matches) {
			char* texNameCpy = new char[strlen(tex->getDiffuseName())+1];
			strcpy(texNameCpy, tex->getDiffuseName());
			textureMap[file] = texNameCpy;
			delete tex;
			return true;
		}
	}

	return false;
}


bool TextureFileFinder::isInterrupted()
{
	return textureBackend->isInterrupted()  ||  (txdBackend  &&  txdBackend->isInterrupted());
}

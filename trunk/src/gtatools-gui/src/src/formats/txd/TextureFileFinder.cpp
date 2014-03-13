/*
	Copyright 2010-2014 David "Alemarius Nexus" Lerch

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

	Additional permissions are granted, which are listed in the file
	GPLADDITIONS.
 */

#include "TextureFileFinder.h"
#include <gtaformats/txd/TXDArchive.h>
#include <gtaformats/txd/TXDTextureHeader.h>
#include <nxcommon/exception/Exception.h>



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

	try {
		TXDArchive txd(file);

		for (TXDArchive::TextureIterator it = txd.getHeaderBegin() ; it != txd.getHeaderEnd() ; it++) {
			TXDTextureHeader* tex = *it;
			FilePath pseudoPath(file.getPath(), tex->getDiffuseName().get());
			File pseudoFile(pseudoPath);
			bool matches = textureBackend->matches(pseudoFile);

			if (matches) {
				char* texNameCpy = new char[strlen(tex->getDiffuseName().get())+1];
				strcpy(texNameCpy, tex->getDiffuseName().get());
				textureMap[file] = texNameCpy;
				return true;
			}
		}
	} catch (Exception ex) {
	}

	return false;
}


bool TextureFileFinder::isInterrupted()
{
	return textureBackend->isInterrupted()  ||  (txdBackend  &&  txdBackend->isInterrupted());
}

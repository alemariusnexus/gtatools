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

#ifndef TEXTUREFILEFINDER_H_
#define TEXTUREFILEFINDER_H_

#include <gtaformats/util/FileFinder.h>
#include <QtCore/QMap>



class TextureFileFinder : public FileFinder {
public:
	TextureFileFinder(FileFinder* textureBackend, FileFinder* txdBackend = NULL);
	~TextureFileFinder();
	virtual bool matches(const File& file);
	virtual void interrupt() { textureBackend->interrupt(); if (txdBackend) txdBackend->interrupt(); }
	virtual bool isInterrupted();
	const char* getMatchedTexture(const File& file) { return textureMap[file]; }

private:
	FileFinder* textureBackend;
	FileFinder* txdBackend;
	QMap<File, char*> textureMap;

};

#endif /* TEXTUREFILEFINDER_H_ */

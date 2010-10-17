/*
 * TextureFileFinder.h
 *
 *  Created on: 14.10.2010
 *      Author: alemariusnexus
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

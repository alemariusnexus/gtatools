/*
 * TextureSource.h
 *
 *  Created on: 06.11.2010
 *      Author: alemariusnexus
 */

#ifndef TEXTURESOURCE_H_
#define TEXTURESOURCE_H_

#include "config.h"
#include "Engine.h"


class TextureSource {
public:
	virtual GLuint getTexture(hash_t texHash) = 0;
	GLuint getTexture(const char* txdName) { return getTexture(Hash(txdName)); }
};

#endif /* TEXTURESOURCE_H_ */

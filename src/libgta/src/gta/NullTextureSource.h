/*
 * NullTextureSource.h
 *
 *  Created on: 07.11.2010
 *      Author: alemariusnexus
 */

#ifndef NULLTEXTURESOURCE_H_
#define NULLTEXTURESOURCE_H_

#include "config.h"
#include "TextureSource.h"


class NullTextureSource : public TextureSource {
public:
	virtual GLuint getTexture(hash_t texHash) { return 0; }
};

#endif /* NULLTEXTURESOURCE_H_ */

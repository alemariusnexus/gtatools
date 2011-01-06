/*
 * TextureFilter.h
 *
 *  Created on: 28.12.2010
 *      Author: alemariusnexus
 */

#ifndef TEXTUREFILTER_H_
#define TEXTUREFILTER_H_

#include <gtaformats/txd/TXDTexture.h>


class TextureFilter {
public:
	virtual bool process(const TXDTexture& tex) = 0;
};

#endif /* TEXTUREFILTER_H_ */

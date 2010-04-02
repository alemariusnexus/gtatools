/*
 * TextureProvider.h
 *
 *  Created on: 14.03.2010
 *      Author: alemariusnexus
 */

#ifndef TEXTUREPROVIDER_H_
#define TEXTUREPROVIDER_H_

#include "../gf_config.h"
#include "../gtatxd.h"


class TextureProvider
{
public:
	virtual bool findTexture(const char* texName, TXDTexture*& texture, uint8_t*& data,
			int16_t& width, int16_t& height) = 0;
};

#endif /* TEXTUREPROVIDER_H_ */

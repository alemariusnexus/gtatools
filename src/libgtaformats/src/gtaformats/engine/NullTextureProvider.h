/*
 * NullTextureProvider.h
 *
 *  Created on: 14.03.2010
 *      Author: alemariusnexus
 */

#ifndef NULLTEXTUREPROVIDER_H_
#define NULLTEXTUREPROVIDER_H_

//#include "TextureProvider.h"
#include "DefaultTextureProvider.h"


class NullTextureProvider : public TextureProvider
{
public:
	virtual bool findTexture(const char* texName, TXDTexture*& texture, uint8_t*& data,
			int16_t& width, int16_t& height)
	{
		return false;
	}
	void machWas();
};

#endif /* NULLTEXTUREPROVIDER_H_ */

/*
 * DefaultTextureProvider.h
 *
 *  Created on: 14.03.2010
 *      Author: alemariusnexus
 */

#ifndef DEFAULTTEXTUREPROVIDER_H_
#define DEFAULTTEXTUREPROVIDER_H_

#include "TextureProvider.h"
#include "../gf_config.h"
#include "../internal/StringComparator.h"
#include "../gtatxd.h"
#include <map>
#include <vector>

using std::map;
using std::vector;


struct TextureEntry
{
	const char* file;
	int32_t texIndex;
	int32_t imgEntryIndex;
};


class DefaultTextureProvider : public TextureProvider
{
public:
	typedef map<const char*, const TextureEntry*, StringComparator> TextureMap;

public:
	virtual bool findTexture(const char* texName, TXDTexture*& texture, uint8_t*& data,
			int16_t& width, int16_t& height);
	void addResource(const char* file);

private:
	TextureMap textureMap;
};

#endif /* DEFAULTTEXTUREPROVIDER_H_ */

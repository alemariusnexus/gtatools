/*
 * TextureArchive.cpp
 *
 *  Created on: 07.03.2011
 *      Author: alemariusnexus
 */

#include "TextureArchive.h"
#include <utility>

using std::pair;



void TextureArchive::addTexture(hash_t tex, hash_t combinedHash)
{
	combinedHashMap.insert(pair<hash_t, hash_t>(tex, combinedHash));
}


bool TextureArchive::getTextureCombinedHash(hash_t tex, hash_t& combinedHash)
{
	map<hash_t, hash_t>::iterator it = combinedHashMap.find(tex);

	if (it == combinedHashMap.end()) {
		return parent  &&  parent->getTextureCombinedHash(tex, combinedHash);
	}

	combinedHash = it->second;
	return true;
}

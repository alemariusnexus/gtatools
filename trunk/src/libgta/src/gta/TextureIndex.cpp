/*
 * TextureIndex.cpp
 *
 *  Created on: 22.10.2010
 *      Author: alemariusnexus
 */

#include "TextureIndex.h"



TextureIndex::TextureIndex(hash_t txdHash, hash_t texHash)
		: txdHash(txdHash), texHash(texHash)
{
}


TextureIndex::TextureIndex(const char* txdName, const char* texName)
		: txdHash(Hash(txdName)), texHash(Hash(texName))
{
}


TextureIndex::TextureIndex(const TextureIndex& other)
		: txdHash(other.txdHash), texHash(other.texHash)
{
}

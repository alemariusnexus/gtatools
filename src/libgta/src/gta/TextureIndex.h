/*
 * TextureIndex.h
 *
 *  Created on: 22.10.2010
 *      Author: alemariusnexus
 */

#ifndef TEXTUREINDEX_H_
#define TEXTUREINDEX_H_

#include "config.h"
#include <locale>
#include "Engine.h"

using std::collate;
using std::locale;
using std::use_facet;



class TextureIndex {
public:
	TextureIndex(hash_t txdHash, hash_t texHash);
	TextureIndex(const char* txdName, const char* texName);
	TextureIndex(const TextureIndex& other);
	hash_t getTextureHash() const { return texHash; }
	hash_t getTXDHash() const { return txdHash; }

private:
	hash_t txdHash;
	hash_t texHash;
};

#endif /* TEXTUREINDEX_H_ */

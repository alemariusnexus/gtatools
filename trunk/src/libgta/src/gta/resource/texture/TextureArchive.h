/*
 * TextureArchive.h
 *
 *  Created on: 07.03.2011
 *      Author: alemariusnexus
 */

#ifndef TEXTUREARCHIVE_H_
#define TEXTUREARCHIVE_H_

#include <map>
#include "../../Engine.h"

using std::map;


class TextureArchive {
public:
	TextureArchive(hash_t name, TextureArchive* parent = NULL) : name(name), parent(parent) {}
	void addTexture(hash_t tex, hash_t combinedHash);
	void setParent(TextureArchive* parent) { this->parent = parent; }
	TextureArchive* getParent() { return parent; }
	bool getTextureCombinedHash(hash_t tex, hash_t& combinedHash);

private:
	hash_t name;
	TextureArchive* parent;
	map<hash_t, hash_t> combinedHashMap;
};

#endif /* TEXTUREARCHIVE_H_ */

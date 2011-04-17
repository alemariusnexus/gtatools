/*
 * TextureArchive.h
 *
 *  Created on: 07.03.2011
 *      Author: alemariusnexus
 */

#ifndef TEXTUREARCHIVE_H_
#define TEXTUREARCHIVE_H_

#include <set>
#include "../../Engine.h"

using std::set;


class TextureArchive {
public:
	TextureArchive(hash_t name, const File& file, TextureArchive* parent = NULL)
			: name(name), parent(parent), file(file) {}
	void addTexture(hash_t name) { textures.insert(name); }
	const File& getFile() const { return file; }
	hash_t getName() const { return name; }
	void setParent(TextureArchive* parent) { this->parent = parent; }
	TextureArchive* getParent() { return parent; }
	bool contains(hash_t tex) const { return textures.find(tex) != textures.end(); }
	TextureArchive* findTextureArchive(hash_t tex);

private:
	hash_t name;
	TextureArchive* parent;
	set<hash_t> textures;
	File file;
};

#endif /* TEXTUREARCHIVE_H_ */

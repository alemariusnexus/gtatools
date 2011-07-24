/*
	Copyright 2010-2011 David "Alemarius Nexus" Lerch

	This file is part of libgta.

	libgta is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	libgta is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with libgta.  If not, see <http://www.gnu.org/licenses/>.

	Additional permissions are granted, which are listed in the file
	GPLADDITIONS.
 */

#ifndef TEXTUREARCHIVE_H_
#define TEXTUREARCHIVE_H_

#include <set>
#include "../../Engine.h"
#include "../CachePointer.h"

using std::set;


class TextureArchive {
public:
	TextureArchive(hash_t name, const File& file, TextureArchive* parent = NULL)
			: cachePtr(Engine::getInstance()->getTextureCache()->getEntryPointer(name)), parent(parent),
			  file(file) {}
	void addTexture(hash_t name) { textures.insert(name); }
	CachePointer getCachePointer() const { return cachePtr; }
	const File& getFile() const { return file; }
	void setParent(TextureArchive* parent) { this->parent = parent; }
	TextureArchive* getParent() { return parent; }
	bool contains(hash_t tex) const { return textures.find(tex) != textures.end(); }
	TextureArchive* findTextureArchive(hash_t tex);

private:
	CachePointer cachePtr;
	TextureArchive* parent;
	set<hash_t> textures;
	File file;
};

#endif /* TEXTUREARCHIVE_H_ */

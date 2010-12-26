/*
	Copyright 2010 David "Alemarius Nexus" Lerch

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

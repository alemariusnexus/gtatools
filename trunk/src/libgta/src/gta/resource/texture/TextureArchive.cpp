/*
 * TextureArchive.cpp
 *
 *  Created on: 07.03.2011
 *      Author: alemariusnexus
 */

#include "TextureArchive.h"
#include <utility>

using std::pair;



TextureArchive* TextureArchive::findTextureArchive(hash_t tex)
{
	if (contains(tex)) {
		return this;
	} else if (parent) {
		return parent->findTextureArchive(tex);
	}

	return NULL;
}

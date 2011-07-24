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

#ifndef MANAGEDTEXTURESOURCE_H_
#define MANAGEDTEXTURESOURCE_H_

#include <gta/config.h>
#include "TextureSource.h"
#include "../../Engine.h"
#include "../CachePointer.h"


class ManagedTextureSource : public TextureSource {
public:
	ManagedTextureSource(hash_t txdHash)
			: pointer(Engine::getInstance()->getTextureCache()->getEntryPointer(txdHash)) {}
	ManagedTextureSource(const char* txdName)
			: pointer(Engine::getInstance()->getTextureCache()->getEntryPointer(Hash(txdName))) {}
	ManagedTextureSource(const ManagedTextureSource& other) : pointer(other.pointer) {}
	virtual Texture* getTexture(hash_t txdHash);
	virtual TextureSource* clone() const { return new ManagedTextureSource(*this); }
	virtual void release() {}

private:
	CachePointer pointer;
};

#endif /* MANAGEDTEXTURESOURCE_H_ */

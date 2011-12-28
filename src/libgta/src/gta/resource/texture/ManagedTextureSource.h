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
#include <gtaformats/util/CString.h>
#include "TextureSource.h"
#include "../../Engine.h"
#include "../ResourceCache.h"


class ManagedTextureSource : public TextureSource {
public:
	ManagedTextureSource(const CString& txdName)
			: pointer(Engine::getInstance()->getTextureCache()->getEntryPointer(txdName)) {}
	ManagedTextureSource(const ManagedTextureSource& other) : pointer(other.pointer) {}
	virtual Texture* getTexture(const CString& txdName);
	virtual TextureSource* clone() const { return new ManagedTextureSource(*this); }
	virtual void release() {}

private:
	Engine::StringResourceCache::Pointer pointer;
};

#endif /* MANAGEDTEXTURESOURCE_H_ */

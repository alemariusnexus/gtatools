/*
	Copyright 2010 David "Alemarius Nexus" Lerch

	This file is part of gtaformats.

	gtaformats is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	gtaformats is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with gtaformats.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "DFFMaterial.h"
#include <cstring>


DFFMaterial::DFFMaterial(const DFFMaterial& other)
{
	memcpy(color, other.color, sizeof(color));

	ConstTextureIterator it;
	for (it = other.getTextureBegin() ; it != other.getTextureEnd() ; it++) {
		textures.push_back(new DFFTexture(**it));
	}
}


DFFMaterial::~DFFMaterial()
{
	removeTextures();
}


void DFFMaterial::removeTexture(DFFTexture* texture)
{
	TextureIterator it;

	for (it = textures.begin() ; it != textures.end() ; it++) {
		if (*it == texture) {
			delete *it;
			textures.erase(it);
		}
	}
}


void DFFMaterial::removeTextures()
{
	TextureIterator it;

	for (it = textures.begin() ; it != textures.end() ; it++) {
		delete *it;
	}

	textures.clear();
}

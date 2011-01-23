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

#include "Material.h"
#include <gtaformats/util/strutil.h>
#include <cstring>



Material::Material(hash_t texture)
		: textured(true), texture(texture)
{
}


Material::Material()
		: textured(false)
{
}


Material::Material(const DFFMaterial& dffMat)
		: textured(dffMat.getTextureCount() > 0)
{
	dffMat.getColor(color[0], color[1], color[2], color[3]);

	if (textured) {
		const char* texName = dffMat.getTexture(0)->getDiffuseName();
		char* lTexName = new char[strlen(texName)+1];
		strtolower(lTexName, texName);
		texture = Hash(lTexName);
		delete[] lTexName;
	}
}
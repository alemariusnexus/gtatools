/*
 * Material.cpp
 *
 *  Created on: 22.10.2010
 *      Author: alemariusnexus
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

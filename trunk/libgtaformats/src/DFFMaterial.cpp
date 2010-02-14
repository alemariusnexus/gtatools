/*
 * DFFMaterial.cpp
 *
 *  Created on: 12.02.2010
 *      Author: alemariusnexus
 */

#include "DFFMaterial.h"


DFFMaterial::~DFFMaterial()
{
	for (int32_t i = 0 ; i < textureCount ; i++) {
		delete textures[i];
	}

	delete[] textures;
}

/*
 * DFFTexture.cpp
 *
 *  Created on: 12.02.2010
 *      Author: alemariusnexus
 */

#include "DFFTexture.h"


DFFTexture::~DFFTexture()
{
	if (diffuseName != NULL) {
		delete[] diffuseName;
	}

	if (alphaName != NULL) {
		delete[] alphaName;
	}
}

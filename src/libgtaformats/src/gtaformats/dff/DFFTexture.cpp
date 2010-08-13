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

#include "DFFTexture.h"
#include <cstring>



DFFTexture::DFFTexture(const DFFTexture& other)
		: diffuseName(new char[strlen(other.diffuseName)+1]), alphaName(new char[strlen(other.alphaName)+1])
{
	strcpy(diffuseName, other.diffuseName);
	strcpy(alphaName, other.alphaName);
}


DFFTexture::~DFFTexture()
{
	if (diffuseName != NULL) {
		delete[] diffuseName;
	}

	if (alphaName != NULL) {
		delete[] alphaName;
	}
}


void DFFTexture::setAlphaName(const char* name)
{
	alphaName = new char[strlen(name)+1];
	strcpy(alphaName, name);
}


void DFFTexture::setDiffuseName(const char* name)
{
	diffuseName = new char[strlen(name)+1];
	strcpy(diffuseName, name);
}

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

#ifndef DFFMATERIAL_H_
#define DFFMATERIAL_H_

#include <gf_config.h>
#include "DFFTexture.h"


class DFFMaterial {
private:
	friend class DFFLoader;

public:
	~DFFMaterial();
	void getColor(uint8_t& r, uint8_t& g, uint8_t& b, uint8_t& a) { r=color[0];g=color[1];b=color[2];a=color[3]; }
	int32_t getTextureCount() { return textureCount; }
	DFFTexture** getTextures() { return textures; }

private:
	uint8_t color[4];
	int32_t textureCount;
	DFFTexture** textures;
};

#endif /* DFFMATERIAL_H_ */

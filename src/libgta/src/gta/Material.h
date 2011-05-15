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

#ifndef MATERIAL_H_
#define MATERIAL_H_

#include <gta/config.h>
#include "Engine.h"
#include <gtaformats/dff/DFFMaterial.h>



class Material {
public:
	Material(hash_t texture);
	Material();
	Material(const DFFMaterial& dffMat);
	hash_t getTextureHash() const { return texture; }
	void setTextureHash(hash_t texHash) { texture = texHash; }
	bool isTextured() const { return textured; }
	void setTextured(bool texed) { textured = texed; }

	void getColor(uint8_t& r, uint8_t& g, uint8_t& b, uint8_t& a) const
	{
		r = color[0]; g = color[1]; b = color[2]; a = color[3];
	}
	void setColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
	{
		color[0] = r; color[1] = g; color[2] = b; color[3] = a;
	}

	int guessSize() const { return sizeof(Material); }

private:
	bool textured;
	hash_t texture;
	uint8_t color[4];
};

#endif /* MATERIAL_H_ */

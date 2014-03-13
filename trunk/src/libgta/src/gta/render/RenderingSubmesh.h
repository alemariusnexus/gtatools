/*
	Copyright 2010-2014 David "Alemarius Nexus" Lerch

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

#ifndef RENDERINGSUBMESH_H_
#define RENDERINGSUBMESH_H_

#include <gta/config.h>
#include <GL/glew.h>
#include "../resource/texture/TextureSource.h"


class RenderingMesh;


class RenderingSubmesh
{
public:
	RenderingSubmesh(RenderingMesh* mesh, int indexCount, GLuint indexOffset, GLuint texture);
	int getIndexCount() const { return indexCount; }
	GLuint getIndexOffset() const { return indexOffset; }
	GLuint getTexture() const { return texture; }
	void setMaterialColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
			{ materialColor[0] = r; materialColor[1] = g; materialColor[2] = b; materialColor[3] = a; }
	void getMaterialColor(uint8_t& r, uint8_t& g, uint8_t& b, uint8_t& a)
			{ r = materialColor[0]; g = materialColor[1]; b = materialColor[2]; a = materialColor[3]; }

	~RenderingSubmesh()
	{
		if (texSrc) {
			texSrc->release();
			delete texSrc;
		}
	}

private:
	int indexCount;
	GLuint indexOffset;
	GLuint texture;
	uint8_t materialColor[4];

	TextureSource* texSrc;


	friend class RenderingEntityGenerator;
};

#endif /* RENDERINGSUBMESH_H_ */

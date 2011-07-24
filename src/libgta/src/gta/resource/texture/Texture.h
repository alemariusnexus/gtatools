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

#ifndef TEXTURE_H_
#define TEXTURE_H_

#include <gtaformats/txd/TXDArchive.h>
#include "../ResourceCache.h"
#include "../../gl.h"


class Texture {
public:
	Texture(GLuint tex, cachesize_t size) : glTex(tex), size(size) {}
	Texture(TXDArchive* txd, TXDTextureHeader* tex);
	~Texture() { glDeleteTextures(1, &glTex); }
	GLuint getGLTexture() const { return glTex; }
	cachesize_t getSize() const { return size; }

private:
	GLuint glTex;
	cachesize_t size;
};

#endif /* TEXTURE_H_ */

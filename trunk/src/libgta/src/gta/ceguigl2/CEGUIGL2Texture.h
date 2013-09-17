/*
	Copyright 2010-2013 David "Alemarius Nexus" Lerch

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

#ifndef CEGUIGL2TEXTURE_H_
#define CEGUIGL2TEXTURE_H_

#include <CEGUI/CEGUI.h>
#include "../gl.h"

using CEGUI::Size;
using CEGUI::Vector2;
using CEGUI::String;

class CEGUIGL2Renderer;


class CEGUIGL2Texture : public CEGUI::Texture {
public:
	virtual const Size& getSize() const { return size; }
	virtual const Size& getOriginalDataSize() const { return origSize; }
	virtual const Vector2& getTexelScaling() const { return scale; }
	virtual void loadFromFile(const String& fileName, const String& resGroup);
	virtual void loadFromMemory(const void* data, const Size& size, PixelFormat format);
	virtual void saveToMemory(void* data);
	GLuint getGLTexture() const { return oglTex; }

protected:
	CEGUIGL2Texture(CEGUIGL2Renderer* renderer);
	void generateGLTexture();
	void resizeGLTexture(const Size& nsize);

private:
	CEGUIGL2Renderer* renderer;
	GLuint oglTex;
	Size origSize;
	Size size;
	Vector2 scale;

private:
	friend class CEGUIGL2FBOTextureTarget;
	friend class CEGUIGL2Renderer;
};

#endif /* CEGUIGL2TEXTURE_H_ */

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

#include "CEGUIGL2Texture.h"
#include <gtaformats/util/util.h>
#include "CEGUIGL2Renderer.h"
#include "CEGUIGL2TextureTarget.h"

using CEGUI::RawDataContainer;
using CEGUI::GeometryBuffer;



CEGUIGL2Texture::CEGUIGL2Texture(CEGUIGL2Renderer* renderer)
		: renderer(renderer), oglTex(0), size(-1.0f, -1.0f), origSize(-1.0f, -1.0f)
{
	generateGLTexture();
}


void CEGUIGL2Texture::generateGLTexture()
{
	if (oglTex == 0) {
		glDeleteTextures(1, &oglTex);
	}

	glGenTextures(1, &oglTex);
	glBindTexture(GL_TEXTURE_2D, oglTex);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}


void CEGUIGL2Texture::resizeGLTexture(const Size& nsize)
{
	Size adjustedSize(GetNextPowerOfTwo(nsize.d_width), GetNextPowerOfTwo(nsize.d_height));

	if (adjustedSize != size) {
		size = adjustedSize;
		glBindTexture(GL_TEXTURE_2D, oglTex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size.d_width, size.d_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		scale = Vector2(1.0f / size.d_width, 1.0f / size.d_height);
	}

	origSize = nsize;
}


void CEGUIGL2Texture::loadFromFile(const String& fileName, const String& resGroup)
{
	RawDataContainer rawData;
	CEGUI::System& sys = CEGUI::System::getSingleton();
	CEGUI::ResourceProvider* res = sys.getResourceProvider();
	res->loadRawDataContainer(fileName, rawData, resGroup);

	CEGUI::Texture* result = sys.getImageCodec().load(rawData, this);
}


void CEGUIGL2Texture::loadFromMemory(const void* data, const Size& size, PixelFormat format)
{
	resizeGLTexture(size);

	GLenum oglFormat;

	switch (format) {
	case PF_RGB:
		oglFormat = GL_RGB;
		break;
	case PF_RGBA:
		oglFormat = GL_RGBA;
		break;
	}

	glBindTexture(GL_TEXTURE_2D, oglTex);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, size.d_width, size.d_height, oglFormat, GL_UNSIGNED_BYTE, data);
}


void CEGUIGL2Texture::saveToMemory(void* data)
{
#ifdef GTA_USE_OPENGL_ES

	// In OpenGL ES 2.0 there is no direct way to copy texture image data back to system memory. We'll get
	// our data by rendering a plane with the same pixel size as the texture and applying the texture to
	// this plane. We do this in an FBO and read it back afterwards.

	CEGUIGL2TextureTarget* target = (CEGUIGL2TextureTarget*) renderer->createTextureTarget();

	GeometryBuffer& geom = renderer->createGeometryBuffer();

	GLint w = (GLint) size.d_width;
	GLint h = (GLint) size.d_height;

	CEGUI::Vertex vertices[] = {
			{ CEGUI::Vector3(0.0f, 0.0f, 0.0f), Vector2(0.0f, 1.0f), CEGUI::colour(0xFFFFFFFF) },
			{ CEGUI::Vector3(0.0f, h, 0.0f), Vector2(0.0f, 0.0f), CEGUI::colour(0xFFFFFFFF) },
			{ CEGUI::Vector3(w, 0.0f, 0.0f), Vector2(1.0f, 1.0f), CEGUI::colour(0xFFFFFFFF) },
			{ CEGUI::Vector3(w, 0.0f, 0.0f), Vector2(1.0f, 1.0f), CEGUI::colour(0xFFFFFFFF) },
			{ CEGUI::Vector3(0.0f, h, 0.0f), Vector2(0.0f, 0.0f), CEGUI::colour(0xFFFFFFFF) },
			{ CEGUI::Vector3(w, h, 0.0f), Vector2(1.0f, 0.0f), CEGUI::colour(0xFFFFFFFF) }
	};

	geom.setActiveTexture(this);
	geom.appendGeometry(vertices, 6);
	geom.setClippingRegion(Rect(0, 0, w, h));

	target->declareRenderSize(size);

	renderer->beginRendering();

	target->activate();

	geom.draw();
	target->saveToMemory(data);

	target->deactivate();

	renderer->endRendering();

	renderer->destroyTextureTarget(target);
	renderer->destroyGeometryBuffer(geom);

#else

	glBindTexture(GL_TEXTURE_2D, oglTex);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

#endif
}

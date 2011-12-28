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

#include "CEGUIGL2FBOTextureTarget.h"
#include "CEGUIGL2Renderer.h"
#include <gtaformats/util/util.h>



CEGUIGL2FBOTextureTarget::CEGUIGL2FBOTextureTarget(CEGUIGL2Renderer* renderer)
		: CEGUIGL2TextureTarget(renderer)
{
	tex = new CEGUIGL2Texture(renderer);
	generateFBO();
}


void CEGUIGL2FBOTextureTarget::generateFBO()
{
	Size defSize(128, 128);

#ifndef GTA_USE_OPENGL_ES
	if (gtaglIsVersionSupported(3, 0)) {
#endif
		glGenFramebuffers(1, &fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);

		tex->resizeGLTexture(defSize);

		GLuint oglTex = tex->getGLTexture();
		glBindTexture(GL_TEXTURE_2D, oglTex);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, oglTex, 0);
#ifndef GTA_USE_OPENGL_ES
	} else if (GLEW_EXT_framebuffer_object) {
		glGenFramebuffersEXT(1, &fbo);
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);

		tex->resizeGLTexture(defSize);

		GLuint oglTex = tex->getGLTexture();
		glBindTexture(GL_TEXTURE_2D, oglTex);

		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, oglTex, 0);
	}
#endif

	declareRenderSize(defSize);
}


void CEGUIGL2FBOTextureTarget::activate()
{
#ifndef GTA_USE_OPENGL_ES
	if (gtaglIsVersionSupported(3, 0)) {
#endif
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
#ifndef GTA_USE_OPENGL_ES
	} else if (GLEW_EXT_framebuffer_object) {
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);
	}
#endif

	CEGUIGL2RenderTarget::activate();
}


void CEGUIGL2FBOTextureTarget::deactivate()
{
#ifndef GTA_USE_OPENGL_ES
	if (gtaglIsVersionSupported(3, 0)) {
#endif
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
#ifndef GTA_USE_OPENGL_ES
	} else if (GLEW_EXT_framebuffer_object) {
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	}
#endif

	CEGUIGL2RenderTarget::deactivate();
}


void CEGUIGL2FBOTextureTarget::clear()
{
#ifndef GTA_USE_OPENGL_ES
	if (gtaglIsVersionSupported(3, 0)) {
#endif
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
#ifndef GTA_USE_OPENGL_ES
	} else if (GLEW_EXT_framebuffer_object) {
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);
	}
#endif

	glDisable(GL_SCISSOR_TEST);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glEnable(GL_SCISSOR_TEST);

#ifndef GTA_USE_OPENGL_ES
	if (gtaglIsVersionSupported(3, 0)) {
#endif
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
#ifndef GTA_USE_OPENGL_ES
	} else if (GLEW_EXT_framebuffer_object) {
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	}
#endif
}


void CEGUIGL2FBOTextureTarget::declareRenderSize(const Size& size)
{
	Size adjustedSize(GetNextPowerOfTwo(size.d_width), GetNextPowerOfTwo(size.d_height));
	setArea(Rect(getArea().getPosition(), adjustedSize));
	tex->resizeGLTexture(size);
	clear();
}


void CEGUIGL2FBOTextureTarget::saveToMemory(void* data)
{
#ifndef GTA_USE_OPENGL_ES
	if (gtaglIsVersionSupported(3, 0)) {
#endif
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
#ifndef GTA_USE_OPENGL_ES
	} else if (GLEW_EXT_framebuffer_object) {
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);
	}
#endif

	Rect area = getArea();
	glReadPixels(0, 0, (GLint) area.getWidth(), (GLint) area.getHeight(), GL_RGBA, GL_UNSIGNED_BYTE, data);

#ifndef GTA_USE_OPENGL_ES
	if (gtaglIsVersionSupported(3, 0)) {
#endif
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
#ifndef GTA_USE_OPENGL_ES
	} else if (GLEW_EXT_framebuffer_object) {
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	}
#endif
}

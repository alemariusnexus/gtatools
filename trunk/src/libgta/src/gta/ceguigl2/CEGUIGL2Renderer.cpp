/*
	Copyright 2010-2012 David "Alemarius Nexus" Lerch

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

#include "CEGUIGL2Renderer.h"
#include "CEGUIGL2GeometryBuffer.h"
#include "CEGUIGL2TextureTarget.h"
#include "CEGUIGL2ViewportTarget.h"
#include "CEGUIGL2FBOTextureTarget.h"
#include "../gl.h"
#include <algorithm>

#ifdef GTA_USE_OPENGL_ES
#include <res_glsles2_cegui_vertex_shader.h>
#include <res_glsles2_cegui_fragment_shader.h>
#else
#include <res_glsl110_cegui_vertex_shader.h>
#include <res_glsl110_cegui_fragment_shader.h>

#include <res_glsl140_cegui_vertex_shader.h>
#include <res_glsl140_cegui_fragment_shader.h>
#endif

using std::find;
using CEGUI::ResourceProvider;
using CEGUI::DefaultResourceProvider;



CEGUIGL2Renderer& CEGUIGL2Renderer::bootstrapSystem()
{
	CEGUIGL2Renderer* renderer = new CEGUIGL2Renderer;
	ResourceProvider* resProv = new DefaultResourceProvider;
	CEGUI::System::create(*renderer, resProv);
	return *renderer;
}


CEGUIGL2Renderer::CEGUIGL2Renderer()
		: program(new ShaderProgram), rootTarget(new CEGUIGL2ViewportTarget(this)),
		  root(new RenderingRoot(*rootTarget)),
		  displayDPI(96, 96)
{
	GLint vp[4];
	glGetIntegerv(GL_VIEWPORT, vp);
	displaySize = Size(vp[2], vp[3]);

	idStr.append("OpenGL(ES) 2.0 Renderer running on ")
#ifdef GTA_USE_OPENGL_ES
			.append(glGetString(GL_VERSION))
#else
			.append("OpenGL ").append(glGetString(GL_VERSION))
#endif
			.append(" (GL vendor: ").append(glGetString(GL_VENDOR))
			.append(", GL renderer: ").append(glGetString(GL_RENDERER))
			.append(")");

	Shader* vshader;
	Shader* fshader;

#ifdef GTA_USE_OPENGL_ES

	vshader = new Shader(GL_VERTEX_SHADER);
	vshader->loadSourceCode((const char*) res_glsles2_cegui_vertex_shader_data,
			sizeof(res_glsles2_cegui_vertex_shader_data));
	vshader->compile();

	fshader = new Shader(GL_FRAGMENT_SHADER);
	fshader->loadSourceCode((const char*) res_glsles2_cegui_fragment_shader_data,
			sizeof(res_glsles2_cegui_fragment_shader_data));
	fshader->compile();

#else

	if (gtaglIsVersionSupported(3, 1)) {
		vshader = new Shader(GL_VERTEX_SHADER);
		vshader->loadSourceCode((const char*) res_glsl140_cegui_vertex_shader_data,
				sizeof(res_glsl140_cegui_vertex_shader_data));
		vshader->compile();

		fshader = new Shader(GL_FRAGMENT_SHADER);
		fshader->loadSourceCode((const char*) res_glsl140_cegui_fragment_shader_data,
				sizeof(res_glsl140_cegui_fragment_shader_data));
		fshader->compile();
	} else {
		vshader = new Shader(GL_VERTEX_SHADER);
		vshader->loadSourceCode((const char*) res_glsl110_cegui_vertex_shader_data,
				sizeof(res_glsl110_cegui_vertex_shader_data));
		vshader->compile();

		fshader = new Shader(GL_FRAGMENT_SHADER);
		fshader->loadSourceCode((const char*) res_glsl110_cegui_fragment_shader_data,
				sizeof(res_glsl110_cegui_fragment_shader_data));
		fshader->compile();
	}

#endif

	program->attachShader(vshader);
	program->attachShader(fshader);
	program->link();

	mvpMatrixUniform = program->getUniformLocation("MVPMatrix");
	texturedUniform = program->getUniformLocation("Textured");
	textureUniform = program->getUniformLocation("Texture");

	vertexAttrib = program->getAttributeLocation("Vertex");
	texCoordAttrib = program->getAttributeLocation("TexCoord");
	colorAttrib = program->getAttributeLocation("Color");

	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTexSize);
}


RenderingRoot& CEGUIGL2Renderer::getDefaultRenderingRoot()
{
	return *root;
}


GeometryBuffer& CEGUIGL2Renderer::createGeometryBuffer()
{
	CEGUIGL2GeometryBuffer* buf = new CEGUIGL2GeometryBuffer(this);
	geomBufs.push_back(buf);
	return *buf;
}


void CEGUIGL2Renderer::destroyGeometryBuffer(GeomBufList::iterator it)
{
	CEGUIGL2GeometryBuffer* buf = *it;
	geomBufs.erase(it);
	delete buf;
}


void CEGUIGL2Renderer::destroyGeometryBuffer(const GeometryBuffer& buf)
{
	GeomBufList::iterator it = find(geomBufs.begin(), geomBufs.end(), &buf);

	if (it != geomBufs.end())
		destroyGeometryBuffer(it);
}


void CEGUIGL2Renderer::destroyAllGeometryBuffers()
{
	size_t size = geomBufs.size();
	for (size_t i = 0 ; i < size ; i++) {
		destroyGeometryBuffer(geomBufs.begin());
	}
}


TextureTarget* CEGUIGL2Renderer::createTextureTarget()
{
	CEGUIGL2FBOTextureTarget* target = new CEGUIGL2FBOTextureTarget(this);
	texTargets.push_back(target);
	return target;
}


void CEGUIGL2Renderer::destroyTextureTarget(TexTargetList::iterator it)
{
	CEGUIGL2TextureTarget* target = *it;
	texTargets.erase(it);
	delete target;
}


void CEGUIGL2Renderer::destroyTextureTarget(TextureTarget* target)
{
	TexTargetList::iterator it = find(texTargets.begin(), texTargets.end(), target);

	if (it != texTargets.end()) {
		destroyTextureTarget(it);
	}
}


void CEGUIGL2Renderer::destroyAllTextureTargets()
{
	size_t size = texTargets.size();
	for (size_t i = 0 ; i < size ; i++) {
		destroyTextureTarget(texTargets.begin());
	}
}


CEGUI::Texture& CEGUIGL2Renderer::createTexture()
{
	CEGUIGL2Texture* tex = new CEGUIGL2Texture(this);
	texes.push_back(tex);
	return *tex;
}


CEGUI::Texture& CEGUIGL2Renderer::createTexture(const String& fileName, const String& resGroup)
{
	CEGUIGL2Texture* tex = (CEGUIGL2Texture*) &createTexture();
	tex->loadFromFile(fileName, resGroup);
	return *tex;
}


CEGUI::Texture& CEGUIGL2Renderer::createTexture(const Size& size)
{
	CEGUIGL2Texture* tex = (CEGUIGL2Texture*) &createTexture();
	tex->resizeGLTexture(size);
	return *tex;
}


void CEGUIGL2Renderer::destroyTexture(TexList::iterator it)
{
	CEGUIGL2Texture* tex = *it;
	texes.erase(it);
	delete tex;
}


void CEGUIGL2Renderer::destroyTexture(CEGUI::Texture& tex)
{
	TexList::iterator it = find(texes.begin(), texes.end(), &tex);

	if (it != texes.end())
		destroyTexture(it);
}


void CEGUIGL2Renderer::destroyAllTextures()
{
	size_t size = texes.size();
	for (size_t i = 0 ; i < size ; i++) {
		destroyTexture(texes.begin());
	}
}


void CEGUIGL2Renderer::beginRendering()
{
	program->makeCurrent();

	glEnableVertexAttribArray(vertexAttrib);
	glEnableVertexAttribArray(texCoordAttrib);
	glEnableVertexAttribArray(colorAttrib);

	setupBlendMode(CEGUI::BM_NORMAL, true);

	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_SCISSOR_TEST);
	glEnable(GL_BLEND);
}


void CEGUIGL2Renderer::endRendering()
{
	glDisableVertexAttribArray(vertexAttrib);
	glDisableVertexAttribArray(texCoordAttrib);
	glDisableVertexAttribArray(colorAttrib);

	glDisable(GL_SCISSOR_TEST);

	glUseProgram(0);
}


void CEGUIGL2Renderer::setDisplaySize(const Size& size)
{
	displaySize = size;

	RenderTarget& target = root->getRenderTarget();
	target.setArea(Rect(target.getArea().getPosition(), size));
}


const Size& CEGUIGL2Renderer::getDisplaySize() const
{
	return displaySize;
}


const Vector2& CEGUIGL2Renderer::getDisplayDPI() const
{
	return displayDPI;
}


uint CEGUIGL2Renderer::getMaxTextureSize() const
{
	return maxTexSize;
}


const String& CEGUIGL2Renderer::getIdentifierString() const
{
	return idStr;
}


void CEGUIGL2Renderer::setActiveRenderTarget(CEGUIGL2RenderTarget* target)
{
	if (!target)
		target = rootTarget;

	activeTarget = target;
}


void CEGUIGL2Renderer::setupBlendMode(BlendMode mode, bool force)
{
	if (!force  &&  blendMode == mode) {
		return;
	}

	blendMode = mode;

	if (mode == CEGUI::BM_NORMAL) {
		glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE_MINUS_DST_ALPHA, GL_ONE);
	} else if (mode == CEGUI::BM_RTT_PREMULTIPLIED) {
		glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	}
}

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

#include "gldebug.h"
#include "Engine.h"
#include "Shader.h"
#include "ShaderProgram.h"
#include "GLException.h"

#include <res_glsl140_render_depth_2d_vertex_shader.h>
#include <res_glsl140_render_depth_2d_fragment_shader.h>
#include <res_glsl140_render_depth_rect_vertex_shader.h>
#include <res_glsl140_render_depth_rect_fragment_shader.h>

#include <res_glsl110_render_depth_2d_vertex_shader.h>
#include <res_glsl110_render_depth_2d_fragment_shader.h>
#include <res_glsl110_render_depth_rect_vertex_shader.h>
#include <res_glsl110_render_depth_rect_fragment_shader.h>

#include <res_glsles2_render_depth_2d_vertex_shader.h>
#include <res_glsles2_render_depth_2d_fragment_shader.h>
#include <res_glsles2_render_depth_rect_vertex_shader.h>
#include <res_glsles2_render_depth_rect_fragment_shader.h>


bool debugInited = false;

GLuint rdFBO;
GLuint rdColorTex;

GLuint planeDataBuf;

ShaderProgram* rd2dProgram;
ShaderProgram* rdRectProgram;

GLint rd2dDepthTexUniform;
GLint rd2dVertexAttrib;

GLint rdRectDepthTexUniform;
GLint rdRectVertexAttrib;



void GLDebugInit()
{
	if (debugInited)
		return;

	GLint oldProgram;
	glGetIntegerv(GL_CURRENT_PROGRAM, &oldProgram);

	GLint oldTex;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &oldTex);

	GLint oldRectTex;

#ifndef GTA_USE_OPENGL_ES
	if (gtaglIsVersionSupported(3, 1))
		glGetIntegerv(GL_TEXTURE_BINDING_RECTANGLE, &oldRectTex);
#endif

	gtaglGenFramebuffers(1, &rdFBO);
	gtaglBindFramebuffer(GTAGL_FRAMEBUFFER, rdFBO);

	glGenTextures(1, &rdColorTex);

	Engine* engine = Engine::getInstance();
	int viewW = engine->getViewportWidth();
	int viewH = engine->getViewportHeight();

	glBindTexture(GL_TEXTURE_2D, rdColorTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, viewW, viewH, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	gtaglFramebufferTexture2D(GTAGL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, rdColorTex, 0);

	GLException::checkFramebufferStatus(GTAGL_FRAMEBUFFER,
			"During initialization of depth render FBO");

	const char* rdVertex2dShaderData;
	const char* rdFragment2dShaderData;
	const char* rdVertexRectShaderData;
	const char* rdFragmentRectShaderData;

	int rdVertex2dShaderDataLen;
	int rdFragment2dShaderDataLen;
	int rdVertexRectShaderDataLen;
	int rdFragmentRectShaderDataLen;


#ifndef GTA_USE_OPENGL_ES
	if (gtaglIsVersionSupported(3, 1)) {
		rdVertex2dShaderData = (const char*) res_glsl140_render_depth_2d_vertex_shader_data;
		rdFragment2dShaderData = (const char*) res_glsl140_render_depth_2d_fragment_shader_data;
		rdVertexRectShaderData = (const char*) res_glsl140_render_depth_rect_vertex_shader_data;
		rdFragmentRectShaderData = (const char*) res_glsl140_render_depth_rect_fragment_shader_data;

		rdVertex2dShaderDataLen = sizeof(res_glsl140_render_depth_2d_vertex_shader_data);
		rdFragment2dShaderDataLen = sizeof(res_glsl140_render_depth_2d_fragment_shader_data);
		rdVertexRectShaderDataLen = sizeof(res_glsl140_render_depth_rect_vertex_shader_data);
		rdFragmentRectShaderDataLen = sizeof(res_glsl140_render_depth_rect_fragment_shader_data);
	} else {
		rdVertex2dShaderData = (const char*) res_glsl110_render_depth_2d_vertex_shader_data;
		rdFragment2dShaderData = (const char*) res_glsl110_render_depth_2d_fragment_shader_data;
		rdVertexRectShaderData = (const char*) res_glsl110_render_depth_rect_vertex_shader_data;
		rdFragmentRectShaderData = (const char*) res_glsl110_render_depth_rect_fragment_shader_data;

		rdVertex2dShaderDataLen = sizeof(res_glsl110_render_depth_2d_vertex_shader_data);
		rdFragment2dShaderDataLen = sizeof(res_glsl110_render_depth_2d_fragment_shader_data);
		rdVertexRectShaderDataLen = sizeof(res_glsl110_render_depth_rect_vertex_shader_data);
		rdFragmentRectShaderDataLen = sizeof(res_glsl110_render_depth_rect_fragment_shader_data);
	}
#else
	rdVertex2dShaderData = (const char*) res_glsles2_render_depth_2d_vertex_shader_data;
	rdFragment2dShaderData = (const char*) res_glsles2_render_depth_2d_fragment_shader_data;
	rdVertexRectShaderData = (const char*) res_glsles2_render_depth_rect_vertex_shader_data;
	rdFragmentRectShaderData = (const char*) res_glsles2_render_depth_rect_fragment_shader_data;

	rdVertex2dShaderDataLen = sizeof(res_glsles2_render_depth_2d_vertex_shader_data);
	rdFragment2dShaderDataLen = sizeof(res_glsles2_render_depth_2d_fragment_shader_data);
	rdVertexRectShaderDataLen = sizeof(res_glsles2_render_depth_rect_vertex_shader_data);
	rdFragmentRectShaderDataLen = sizeof(res_glsles2_render_depth_rect_fragment_shader_data);
#endif


	Shader* rdVertex2dShader = new Shader(GL_VERTEX_SHADER, "Render Depth Buffer 2D Vertex Shader");
	rdVertex2dShader->loadSourceCode(rdVertex2dShaderData, rdVertex2dShaderDataLen);
	rdVertex2dShader->compile();

	Shader* rdFragment2dShader = new Shader(GL_FRAGMENT_SHADER, "Render Depth Buffer 2D Fragment Shader");
	rdFragment2dShader->loadSourceCode(rdFragment2dShaderData, rdFragment2dShaderDataLen);
	rdFragment2dShader->compile();

	rd2dProgram = new ShaderProgram("Render Depth Buffer 2D Shader Program");
	rd2dProgram->attachShader(rdVertex2dShader);
	rd2dProgram->attachShader(rdFragment2dShader);
	rd2dProgram->link();

	rd2dDepthTexUniform = rd2dProgram->getUniformLocation("DepthTex");
	rd2dVertexAttrib = rd2dProgram->getAttributeLocation("Vertex");


#ifndef GTA_USE_OPENGL_ES
	if (gtaglIsVersionSupported(3, 1)) {
		Shader* rdVertexRectShader = new Shader(GL_VERTEX_SHADER, "Render Depth Buffer RECTANGLE Vertex Shader");
		rdVertexRectShader->loadSourceCode(rdVertexRectShaderData, rdVertexRectShaderDataLen);
		rdVertexRectShader->compile();

		Shader* rdFragmentRectShader = new Shader(GL_FRAGMENT_SHADER, "Render Depth Buffer RECTANGLE Fragment Shader");
		rdFragmentRectShader->loadSourceCode(rdFragmentRectShaderData, rdFragmentRectShaderDataLen);
		rdFragmentRectShader->compile();

		rdRectProgram = new ShaderProgram("Render Depth Buffer RECTANGLE Shader Program");
		rdRectProgram->attachShader(rdVertexRectShader);
		rdRectProgram->attachShader(rdFragmentRectShader);
		rdRectProgram->link();

		rdRectDepthTexUniform = rdRectProgram->getUniformLocation("DepthTex");
		rdRectVertexAttrib = rdRectProgram->getAttributeLocation("Vertex");
	}
#endif


	float planeData[] = {
			-1.0f, 1.0f,	// Upper left
			-1.0f, -1.0f,	// Lower left
			1.0f, 1.0f,		// Upper right
			1.0f, -1.0f		// Lower right
	};

	glGenBuffers(1, &planeDataBuf);
	glBindBuffer(GL_ARRAY_BUFFER, planeDataBuf);
	glBufferData(GL_ARRAY_BUFFER, 4*2*sizeof(float), planeData, GL_STATIC_DRAW);

	glBindTexture(GL_TEXTURE_2D, oldTex);

#ifndef GTA_USE_OPENGL_ES
	if (gtaglIsVersionSupported(3, 1))
		glBindTexture(GL_TEXTURE_RECTANGLE, oldRectTex);
#endif

	glUseProgram(oldProgram);
}


void GLDebugReadDepthTexture(float* buf, GLenum target)
{
	GLboolean oldDepth = glIsEnabled(GL_DEPTH_TEST);
	GLboolean oldBlend = glIsEnabled(GL_BLEND);

	GLint oldProgram;
	glGetIntegerv(GL_CURRENT_PROGRAM, &oldProgram);

	GLint oldArrayBuf;
	glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &oldArrayBuf);

	GLint oldFBO;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &oldFBO);

	GLDebugInit();

	if (target == GL_TEXTURE_2D)
		rd2dProgram->makeCurrent();
	else
		rdRectProgram->makeCurrent();

	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);

	GLint oldTexUnit;
	glGetIntegerv(GL_ACTIVE_TEXTURE, &oldTexUnit);

	if (target == GL_TEXTURE_2D)
		glUniform1i(rd2dDepthTexUniform, oldTexUnit - GL_TEXTURE0);
	else
		glUniform1i(rdRectDepthTexUniform, oldTexUnit - GL_TEXTURE0);

	gtaglBindFramebuffer(GTAGL_FRAMEBUFFER, rdFBO);

	glBindBuffer(GL_ARRAY_BUFFER, planeDataBuf);

	if (target == GL_TEXTURE_2D) {
		glEnableVertexAttribArray(rd2dVertexAttrib);
		glVertexAttribPointer(rd2dVertexAttrib, 2, GL_FLOAT, GL_FALSE, 0, (void*) 0);
	} else {
		glEnableVertexAttribArray(rdRectVertexAttrib);
		glVertexAttribPointer(rdRectVertexAttrib, 2, GL_FLOAT, GL_FALSE, 0, (void*) 0);
	}

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);


	Engine* engine = Engine::getInstance();
	int viewW = engine->getViewportWidth();
	int viewH = engine->getViewportHeight();

	uint8_t* rgbaData = new uint8_t[viewW*viewH*4];
	glReadPixels(0, 0, viewW, viewH, GL_RGBA, GL_UNSIGNED_BYTE, rgbaData);

	for (unsigned int i = 0 ; i < viewW*viewH ; i++) {
		buf[i] = (rgbaData[i*4] / 255.0f)
				+ (rgbaData[i*4 + 1] / 65025.0f)
				+ (rgbaData[i*4 + 2] / 16581375.0f)
				+ (rgbaData[i*4 + 3] / 4228250625.0f);
	}

	delete[] rgbaData;

	glUseProgram(oldProgram);

	glBindBuffer(GL_ARRAY_BUFFER, oldArrayBuf);

	gtaglBindFramebuffer(GTAGL_FRAMEBUFFER, oldFBO);

	if (oldDepth == GL_TRUE)
		glEnable(GL_DEPTH_TEST);
	if (oldBlend == GL_TRUE)
		glEnable(GL_BLEND);
}


float* GLDebugReadDepthTexture(GLenum target)
{
	Engine* engine = Engine::getInstance();
	int viewW = engine->getViewportWidth();
	int viewH = engine->getViewportHeight();

	float* buf = new float[viewW*viewH];

	GLDebugReadDepthTexture(buf, target);

	return buf;
}


void GLDebugSaveDepthTexture(ostream* out, GLenum target)
{
	Engine* engine = Engine::getInstance();
	int viewW = engine->getViewportWidth();
	int viewH = engine->getViewportHeight();

	float* dbuf = GLDebugReadDepthTexture(target);
	uint32_t* data = new uint32_t[viewW*viewH];

	for (unsigned int i = 0 ; i < viewW*viewH ; i++) {
		data[i] = dbuf[i] * 4294967295;
	}

	delete[] dbuf;

	out->write((char*) data, viewW*viewH*sizeof(uint32_t));

	delete[] data;
}


void GLDebugSaveDepthTexture(const File& file, GLenum target)
{
	ostream* out = file.openOutputStream(ostream::out | ostream::binary);
	GLDebugSaveDepthTexture(out, target);
	delete out;
}


void GLDebugSavePixels(ostream* out, int x, int y, int w, int h, PixelSaveFormat format)
{
	GLenum oglFormat;
	int bpp;

#ifndef GTA_USE_OPENGL_ES
	if (format == PixelSaveFormatRGB) {
		oglFormat = GL_RGB;
		bpp = 3;
	} else {
#endif
		oglFormat = GL_RGBA;
		bpp = 4;
#ifndef GTA_USE_OPENGL_ES
	}
#endif

	uint8_t* data = new uint8_t[w*h*bpp];
	glReadPixels(x, y, w, h, oglFormat, GL_UNSIGNED_BYTE, data);

#ifdef GTA_USE_OPENGL_ES
	if (format == PixelSaveFormatRGB) {
		for (unsigned int i = 0 ; i < w*h ; i++) {
			out->write((char*) (data+i*4), 3);
		}
	} else {
#endif
		out->write((char*) data, w*h*bpp);
#ifdef GTA_USE_OPENGL_ES
	}
#endif

	delete[] data;
}


void GLDebugSavePixels(ostream* out, PixelSaveFormat format)
{
	Engine* engine = Engine::getInstance();

	int w = engine->getViewportWidth();
	int h = engine->getViewportHeight();

	GLDebugSavePixels(out, 0, 0, w, h, format);
}


void GLDebugSavePixels(const File& file, int x, int y, int w, int h, PixelSaveFormat format)
{
	ostream* out = file.openOutputStream(ostream::out | ostream::binary);
	GLDebugSavePixels(out, x, y, w, h, format);
	delete out;
}


void GLDebugSavePixels(const File& file, PixelSaveFormat format)
{
	ostream* out = file.openOutputStream(ostream::out | ostream::binary);
	GLDebugSavePixels(out, format);
	delete out;
}

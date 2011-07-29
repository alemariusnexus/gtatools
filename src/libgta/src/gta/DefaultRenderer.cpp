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

#include "DefaultRenderer.h"
#include "resource/mesh/Mesh.h"
#include "gl.h"
#include "GLException.h"

#include <res_glsl110_vertex_default_shader.h>
#include <res_glsl110_fragment_default_shader.h>
#include <res_glsl110_shade_vertex_shader.h>
#include <res_glsl110_shade_fragment_shader.h>
#include <res_glsl110_dp_peel_layer_vertex_shader.h>
#include <res_glsl110_dp_peel_layer_fragment_shader.h>
#include <res_glsl110_dp_blend_layer_vertex_shader.h>
#include <res_glsl110_dp_blend_layer_fragment_shader.h>
#include <res_glsl110_dp_blend_final_vertex_shader.h>
#include <res_glsl110_dp_blend_final_fragment_shader.h>
#include <res_glsl110_wavg_vertex_shader.h>
#include <res_glsl110_wavg_fragment_shader.h>
#include <res_glsl110_wavg_final_vertex_shader.h>
#include <res_glsl110_wavg_final_fragment_shader.h>

#include <res_glsl140_vertex_default_shader.h>
#include <res_glsl140_fragment_default_shader.h>
#include <res_glsl140_shade_vertex_shader.h>
#include <res_glsl140_shade_fragment_shader.h>
#include <res_glsl140_dp_peel_layer_vertex_shader.h>
#include <res_glsl140_dp_peel_layer_fragment_shader.h>
#include <res_glsl140_dp_blend_layer_vertex_shader.h>
#include <res_glsl140_dp_blend_layer_fragment_shader.h>
#include <res_glsl140_dp_blend_final_vertex_shader.h>
#include <res_glsl140_dp_blend_final_fragment_shader.h>
#include <res_glsl140_wavg_vertex_shader.h>
#include <res_glsl140_wavg_fragment_shader.h>
#include <res_glsl140_wavg_final_vertex_shader.h>
#include <res_glsl140_wavg_final_fragment_shader.h>




inline void gtaglGenFramebuffersCORE(GLsizei p1, GLuint* p2) { glGenFramebuffers(p1, p2); }
inline void gtaglBindFramebufferCORE(GLenum p1, GLuint p2) { glBindFramebuffer(p1, p2); }
inline void gtaglFramebufferTexture2DCORE(GLenum p1, GLenum p2, GLenum p3, GLuint p4, GLint p5)
		{ glFramebufferTexture2D(p1, p2, p3, p4, p5); }
inline void gtaglBlitFramebufferCORE(GLint p1, GLint p2, GLint p3, GLint p4, GLint p5, GLint p6, GLint p7,
		GLint p8, GLbitfield p9, GLenum p10)
		{ glBlitFramebuffer(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10); }

inline void gtaglGenFramebuffersEXT(GLsizei p1, GLuint* p2) { glGenFramebuffersEXT(p1, p2); }
inline void gtaglBindFramebufferEXT(GLenum p1, GLuint p2) { glBindFramebufferEXT(p1, p2); }
inline void gtaglFramebufferTexture2DEXT(GLenum p1, GLenum p2, GLenum p3, GLuint p4, GLint p5)
		{ glFramebufferTexture2DEXT(p1, p2, p3, p4, p5); }
inline void gtaglBlitFramebufferEXT(GLint p1, GLint p2, GLint p3, GLint p4, GLint p5, GLint p6, GLint p7,
		GLint p8, GLbitfield p9, GLenum p10)
		{ glBlitFramebufferEXT(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10); }


void (*gtaglGenFramebuffers)(GLsizei, GLuint*);
void (*gtaglBindFramebuffer)(GLenum, GLuint);
void (*gtaglFramebufferTexture2D)(GLenum, GLenum, GLenum, GLuint, GLint);
void (*gtaglBlitFramebuffer)(GLint, GLint, GLint, GLint, GLint, GLint, GLint, GLint, GLbitfield, GLenum);
void (*gtaglGetFramebufferAttachmentParameter)(GLenum, GLenum, GLenum, GLint*);

GLenum GTAGL_FRAMEBUFFER;
GLenum GTAGL_READ_FRAMEBUFFER;
GLenum GTAGL_DRAW_FRAMEBUFFER;
GLenum GTAGL_COLOR_ATTACHMENT0;
GLenum GTAGL_COLOR_ATTACHMENT1;
GLenum GTAGL_COLOR_ATTACHMENT2;
GLenum GTAGL_COLOR_ATTACHMENT3;
GLenum GTAGL_DEPTH_ATTACHMENT;




DefaultRenderer::DefaultRenderer()
		: transMode(DepthPeeling), transPassCount(5)
{
	Engine* engine = Engine::getInstance();

	// Insert a dummy. This makes handling of the alphaBegin iterator easier
	objs.push_back(NULL);

	alphaBegin = objs.begin();

	const char* vertexDefaultShaderData;
	const char* fragmentDefaultShaderData;
	const char* shadeVertexShaderData;
	const char* shadeFragmentShaderData;
	const char* dpPeelLayerVertexShaderData;
	const char* dpPeelLayerFragmentShaderData;
	const char* dpBlendLayerVertexShaderData;
	const char* dpBlendLayerFragmentShaderData;
	const char* dpBlendFinalVertexShaderData;
	const char* dpBlendFinalFragmentShaderData;
	const char* wavgVertexShaderData;
	const char* wavgFragmentShaderData;
	const char* wavgFinalVertexShaderData;
	const char* wavgFinalFragmentShaderData;

	int vertexDefaultShaderDataLen;
	int fragmentDefaultShaderDataLen;
	int shadeVertexShaderDataLen;
	int shadeFragmentShaderDataLen;
	int dpPeelLayerVertexShaderDataLen;
	int dpPeelLayerFragmentShaderDataLen;
	int dpBlendLayerVertexShaderDataLen;
	int dpBlendLayerFragmentShaderDataLen;
	int dpBlendFinalVertexShaderDataLen;
	int dpBlendFinalFragmentShaderDataLen;
	int wavgVertexShaderDataLen;
	int wavgFragmentShaderDataLen;
	int wavgFinalVertexShaderDataLen;
	int wavgFinalFragmentShaderDataLen;

	if (gtaglIsVersionSupported(3, 1)) {
		vertexDefaultShaderData				= (const char*) res_glsl140_vertex_default_shader_data;
		fragmentDefaultShaderData			= (const char*) res_glsl140_fragment_default_shader_data;
		shadeVertexShaderData				= (const char*) res_glsl140_shade_vertex_shader_data;
		shadeFragmentShaderData				= (const char*) res_glsl140_shade_fragment_shader_data;
		dpPeelLayerVertexShaderData			= (const char*) res_glsl140_dp_peel_layer_vertex_shader_data;
		dpPeelLayerFragmentShaderData		= (const char*) res_glsl140_dp_peel_layer_fragment_shader_data;
		dpBlendLayerVertexShaderData		= (const char*) res_glsl140_dp_blend_layer_vertex_shader_data;
		dpBlendLayerFragmentShaderData		= (const char*) res_glsl140_dp_blend_layer_fragment_shader_data;
		dpBlendFinalVertexShaderData		= (const char*) res_glsl140_dp_blend_final_vertex_shader_data;
		dpBlendFinalFragmentShaderData		= (const char*) res_glsl140_dp_blend_final_fragment_shader_data;
		wavgVertexShaderData				= (const char*) res_glsl140_wavg_vertex_shader_data;
		wavgFragmentShaderData				= (const char*) res_glsl140_wavg_fragment_shader_data;
		wavgFinalVertexShaderData			= (const char*) res_glsl140_wavg_final_vertex_shader_data;
		wavgFinalFragmentShaderData			= (const char*) res_glsl140_wavg_final_fragment_shader_data;

		vertexDefaultShaderDataLen				= sizeof(res_glsl140_vertex_default_shader_data);
		fragmentDefaultShaderDataLen			= sizeof(res_glsl140_fragment_default_shader_data);
		shadeVertexShaderDataLen				= sizeof(res_glsl140_shade_vertex_shader_data);
		shadeFragmentShaderDataLen				= sizeof(res_glsl140_shade_fragment_shader_data);
		dpPeelLayerVertexShaderDataLen			= sizeof(res_glsl140_dp_peel_layer_vertex_shader_data);
		dpPeelLayerFragmentShaderDataLen		= sizeof(res_glsl140_dp_peel_layer_fragment_shader_data);
		dpBlendLayerVertexShaderDataLen			= sizeof(res_glsl140_dp_blend_layer_vertex_shader_data);
		dpBlendLayerFragmentShaderDataLen		= sizeof(res_glsl140_dp_blend_layer_fragment_shader_data);
		dpBlendFinalVertexShaderDataLen			= sizeof(res_glsl140_dp_blend_final_vertex_shader_data);
		dpBlendFinalFragmentShaderDataLen		= sizeof(res_glsl140_dp_blend_final_fragment_shader_data);
		wavgVertexShaderDataLen					= sizeof(res_glsl140_wavg_vertex_shader_data);
		wavgFragmentShaderDataLen				= sizeof(res_glsl140_wavg_fragment_shader_data);
		wavgFinalVertexShaderDataLen			= sizeof(res_glsl140_wavg_final_vertex_shader_data);
		wavgFinalFragmentShaderDataLen			= sizeof(res_glsl140_wavg_final_fragment_shader_data);

		gtaglGenFramebuffers = &gtaglGenFramebuffersCORE;
		gtaglBindFramebuffer = &gtaglBindFramebufferCORE;
		gtaglFramebufferTexture2D = &gtaglFramebufferTexture2DCORE;
		gtaglBlitFramebuffer = &gtaglBlitFramebufferCORE;

		GTAGL_FRAMEBUFFER = GL_FRAMEBUFFER;
		GTAGL_READ_FRAMEBUFFER = GL_READ_FRAMEBUFFER;
		GTAGL_DRAW_FRAMEBUFFER = GL_DRAW_FRAMEBUFFER;
		GTAGL_COLOR_ATTACHMENT0 = GL_COLOR_ATTACHMENT0;
		GTAGL_COLOR_ATTACHMENT1 = GL_COLOR_ATTACHMENT1;
		GTAGL_COLOR_ATTACHMENT2 = GL_COLOR_ATTACHMENT2;
		GTAGL_COLOR_ATTACHMENT3 = GL_COLOR_ATTACHMENT3;
		GTAGL_DEPTH_ATTACHMENT = GL_DEPTH_ATTACHMENT;
	} else {
		vertexDefaultShaderData				= (const char*) res_glsl110_vertex_default_shader_data;
		fragmentDefaultShaderData			= (const char*) res_glsl110_fragment_default_shader_data;
		shadeVertexShaderData				= (const char*) res_glsl110_shade_vertex_shader_data;
		shadeFragmentShaderData				= (const char*) res_glsl110_shade_fragment_shader_data;
		dpPeelLayerVertexShaderData			= (const char*) res_glsl110_dp_peel_layer_vertex_shader_data;
		dpPeelLayerFragmentShaderData		= (const char*) res_glsl110_dp_peel_layer_fragment_shader_data;
		dpBlendLayerVertexShaderData		= (const char*) res_glsl110_dp_blend_layer_vertex_shader_data;
		dpBlendLayerFragmentShaderData		= (const char*) res_glsl110_dp_blend_layer_fragment_shader_data;
		dpBlendFinalVertexShaderData		= (const char*) res_glsl110_dp_blend_final_vertex_shader_data;
		dpBlendFinalFragmentShaderData		= (const char*) res_glsl110_dp_blend_final_fragment_shader_data;
		wavgVertexShaderData				= (const char*) res_glsl110_wavg_vertex_shader_data;
		wavgFragmentShaderData				= (const char*) res_glsl110_wavg_fragment_shader_data;
		wavgFinalVertexShaderData			= (const char*) res_glsl110_wavg_final_vertex_shader_data;
		wavgFinalFragmentShaderData			= (const char*) res_glsl110_wavg_final_fragment_shader_data;

		vertexDefaultShaderDataLen				= sizeof(res_glsl110_vertex_default_shader_data);
		fragmentDefaultShaderDataLen			= sizeof(res_glsl110_fragment_default_shader_data);
		shadeVertexShaderDataLen				= sizeof(res_glsl110_shade_vertex_shader_data);
		shadeFragmentShaderDataLen				= sizeof(res_glsl110_shade_fragment_shader_data);
		dpPeelLayerVertexShaderDataLen			= sizeof(res_glsl110_dp_peel_layer_vertex_shader_data);
		dpPeelLayerFragmentShaderDataLen		= sizeof(res_glsl110_dp_peel_layer_fragment_shader_data);
		dpBlendLayerVertexShaderDataLen			= sizeof(res_glsl110_dp_blend_layer_vertex_shader_data);
		dpBlendLayerFragmentShaderDataLen		= sizeof(res_glsl110_dp_blend_layer_fragment_shader_data);
		dpBlendFinalVertexShaderDataLen			= sizeof(res_glsl110_dp_blend_final_vertex_shader_data);
		dpBlendFinalFragmentShaderDataLen		= sizeof(res_glsl110_dp_blend_final_fragment_shader_data);
		wavgVertexShaderDataLen					= sizeof(res_glsl110_wavg_vertex_shader_data);
		wavgFragmentShaderDataLen				= sizeof(res_glsl110_wavg_fragment_shader_data);
		wavgFinalVertexShaderDataLen			= sizeof(res_glsl110_wavg_final_vertex_shader_data);
		wavgFinalFragmentShaderDataLen			= sizeof(res_glsl110_wavg_final_fragment_shader_data);

		gtaglGenFramebuffers = &gtaglGenFramebuffersEXT;
		gtaglBindFramebuffer = &gtaglBindFramebufferEXT;
		gtaglFramebufferTexture2D = &gtaglFramebufferTexture2DEXT;
		gtaglBlitFramebuffer = &gtaglBlitFramebufferEXT;

		GTAGL_FRAMEBUFFER = GL_FRAMEBUFFER_EXT;
		GTAGL_READ_FRAMEBUFFER = GL_READ_FRAMEBUFFER_EXT;
		GTAGL_DRAW_FRAMEBUFFER = GL_DRAW_FRAMEBUFFER_EXT;
		GTAGL_COLOR_ATTACHMENT0 = GL_COLOR_ATTACHMENT0_EXT;
		GTAGL_COLOR_ATTACHMENT1 = GL_COLOR_ATTACHMENT1_EXT;
		GTAGL_COLOR_ATTACHMENT2 = GL_COLOR_ATTACHMENT2_EXT;
		GTAGL_COLOR_ATTACHMENT3 = GL_COLOR_ATTACHMENT3_EXT;
		GTAGL_DEPTH_ATTACHMENT = GL_DEPTH_ATTACHMENT_EXT;
	}

	Shader* vertexDefaultShader = new Shader(GL_VERTEX_SHADER);
	vertexDefaultShader->loadSourceCode((const char*) vertexDefaultShaderData,
			vertexDefaultShaderDataLen);
	vertexDefaultShader->compile();

	Shader* fragmentDefaultShader = new Shader(GL_FRAGMENT_SHADER);
	fragmentDefaultShader->loadSourceCode((const char*) fragmentDefaultShaderData,
			fragmentDefaultShaderDataLen);
	fragmentDefaultShader->compile();

	Shader* shadeVertexShader = new Shader(GL_VERTEX_SHADER);
	shadeVertexShader->loadSourceCode(shadeVertexShaderData, shadeVertexShaderDataLen);
	shadeVertexShader->compile();

	Shader* shadeFragmentShader = new Shader(GL_FRAGMENT_SHADER);
	shadeFragmentShader->loadSourceCode(shadeFragmentShaderData, shadeFragmentShaderDataLen);
	shadeFragmentShader->compile();

	Shader* dpPeelLayerVertexShader = new Shader(GL_VERTEX_SHADER);
	dpPeelLayerVertexShader->loadSourceCode(dpPeelLayerVertexShaderData, dpPeelLayerVertexShaderDataLen);
	dpPeelLayerVertexShader->compile();

	Shader* dpPeelLayerFragmentShader = new Shader(GL_FRAGMENT_SHADER);
	dpPeelLayerFragmentShader->loadSourceCode(dpPeelLayerFragmentShaderData,
			dpPeelLayerFragmentShaderDataLen);
	dpPeelLayerFragmentShader->compile();

	Shader* dpBlendLayerVertexShader = new Shader(GL_VERTEX_SHADER);
	dpBlendLayerVertexShader->loadSourceCode(dpBlendLayerVertexShaderData,
			dpBlendLayerVertexShaderDataLen);
	dpBlendLayerVertexShader->compile();

	Shader* dpBlendLayerFragmentShader = new Shader(GL_FRAGMENT_SHADER);
	dpBlendLayerFragmentShader->loadSourceCode(dpBlendLayerFragmentShaderData,
			dpBlendLayerFragmentShaderDataLen);
	dpBlendLayerFragmentShader->compile();

	Shader* dpBlendFinalVertexShader = new Shader(GL_VERTEX_SHADER);
	dpBlendFinalVertexShader->loadSourceCode(dpBlendFinalVertexShaderData,
			dpBlendFinalVertexShaderDataLen);
	dpBlendFinalVertexShader->compile();

	Shader* dpBlendFinalFragmentShader = new Shader(GL_FRAGMENT_SHADER);
	dpBlendFinalFragmentShader->loadSourceCode(dpBlendFinalFragmentShaderData,
			dpBlendFinalFragmentShaderDataLen);
	dpBlendFinalFragmentShader->compile();

	Shader* wavgVertexShader = new Shader(GL_VERTEX_SHADER);
	wavgVertexShader->loadSourceCode(wavgVertexShaderData, wavgVertexShaderDataLen);
	wavgVertexShader->compile();

	Shader* wavgFragmentShader = new Shader(GL_FRAGMENT_SHADER);
	wavgFragmentShader->loadSourceCode(wavgFragmentShaderData, wavgFragmentShaderDataLen);
	wavgFragmentShader->compile();

	Shader* wavgFinalVertexShader = new Shader(GL_VERTEX_SHADER);
	wavgFinalVertexShader->loadSourceCode(wavgFinalVertexShaderData, wavgFinalVertexShaderDataLen);
	wavgFinalVertexShader->compile();

	Shader* wavgFinalFragmentShader = new Shader(GL_FRAGMENT_SHADER);
	wavgFinalFragmentShader->loadSourceCode(wavgFinalFragmentShaderData, wavgFinalFragmentShaderDataLen);
	wavgFinalFragmentShader->compile();

	defaultProgram = new ShaderProgram;
	defaultProgram->attachShader(vertexDefaultShader);
	defaultProgram->attachShader(fragmentDefaultShader);
	defaultProgram->attachShader(shadeVertexShader);
	defaultProgram->attachShader(shadeFragmentShader);
	defaultProgram->link();

	dpProgram = new ShaderProgram;
	dpProgram->attachShader(dpPeelLayerVertexShader);
	dpProgram->attachShader(dpPeelLayerFragmentShader);
	dpProgram->attachShader(shadeVertexShader);
	dpProgram->attachShader(shadeFragmentShader);
	dpProgram->link();

	dpBlendLayerProgram = new ShaderProgram;
	dpBlendLayerProgram->attachShader(dpBlendLayerVertexShader);
	dpBlendLayerProgram->attachShader(dpBlendLayerFragmentShader);
	dpBlendLayerProgram->link();

	dpBlendFinalProgram = new ShaderProgram;
	dpBlendFinalProgram->attachShader(dpBlendFinalVertexShader);
	dpBlendFinalProgram->attachShader(dpBlendFinalFragmentShader);
	dpBlendFinalProgram->link();

	wavgProgram = new ShaderProgram;
	wavgProgram->attachShader(wavgVertexShader);
	wavgProgram->attachShader(wavgFragmentShader);
	wavgProgram->attachShader(shadeVertexShader);
	wavgProgram->attachShader(shadeFragmentShader);
	wavgProgram->link();

	wavgBlendFinalProgram = new ShaderProgram;
	wavgBlendFinalProgram->attachShader(wavgFinalVertexShader);
	wavgBlendFinalProgram->attachShader(wavgFinalFragmentShader);
	wavgBlendFinalProgram->link();

	gtaglGenFramebuffers(2, dpFBOs);
	gtaglGenFramebuffers(1, &dpBlendFBO);
	gtaglGenFramebuffers(1, &wavgFBO);

	glGenTextures(2, dpDepthTexes);
	glGenTextures(2, dpColorTexes);
	glGenTextures(1, &dpBlendTex);
	glGenTextures(1, &wavgColorTex);
	glGenTextures(1, &wavgDepthComplexityTex);
	glGenTextures(1, &wavgDepthBufferTex);


	int viewW = engine->getViewportWidth();
	int viewH = engine->getViewportHeight();


	if (gtaglIsVersionSupported(3, 1)) {
		gtaglBindFramebuffer(GTAGL_DRAW_FRAMEBUFFER, dpFBOs[0]);

		// Initialize the depth texture for FBO 0
		glBindTexture(GL_TEXTURE_RECTANGLE, dpDepthTexes[0]);
		glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_DEPTH_COMPONENT32, viewW, viewH, 0, GL_DEPTH_COMPONENT, GL_FLOAT,
				NULL);
		gtaglFramebufferTexture2D(GTAGL_DRAW_FRAMEBUFFER, GTAGL_DEPTH_ATTACHMENT, GL_TEXTURE_RECTANGLE,
				dpDepthTexes[0], 0);

		// Initialize the color texture for FBO 0
		glBindTexture(GL_TEXTURE_RECTANGLE, dpColorTexes[0]);
		glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGBA, viewW, viewH, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		gtaglFramebufferTexture2D(GTAGL_DRAW_FRAMEBUFFER, GTAGL_COLOR_ATTACHMENT0, GL_TEXTURE_RECTANGLE, dpColorTexes[0], 0);



		gtaglBindFramebuffer(GTAGL_DRAW_FRAMEBUFFER, dpFBOs[1]);

		// Initialize the depth texture for FBO 1
		glBindTexture(GL_TEXTURE_RECTANGLE, dpDepthTexes[1]);
		glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_DEPTH_COMPONENT32, viewW, viewH, 0, GL_DEPTH_COMPONENT, GL_FLOAT,
				NULL);
		gtaglFramebufferTexture2D(GTAGL_DRAW_FRAMEBUFFER, GTAGL_DEPTH_ATTACHMENT, GL_TEXTURE_RECTANGLE,
				dpDepthTexes[1], 0);

		// Initialize the color texture for FBO 1
		glBindTexture(GL_TEXTURE_RECTANGLE, dpColorTexes[1]);
		glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGBA, viewW, viewH, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		gtaglFramebufferTexture2D(GTAGL_DRAW_FRAMEBUFFER, GTAGL_COLOR_ATTACHMENT0, GL_TEXTURE_RECTANGLE, dpColorTexes[1], 0);



		gtaglBindFramebuffer(GTAGL_DRAW_FRAMEBUFFER, dpBlendFBO);

		// Initialize the color texture for the blending FBO
		glBindTexture(GL_TEXTURE_RECTANGLE, dpBlendTex);
		glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGBA, viewW, viewH, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		gtaglFramebufferTexture2D(GTAGL_DRAW_FRAMEBUFFER, GTAGL_COLOR_ATTACHMENT0, GL_TEXTURE_RECTANGLE, dpBlendTex, 0);
	} else {
		// Query the size of the window-system depth buffer and try to choose the same format for the FBO
		// depth buffers. This is needed because glBlitFramebufferEXT generates an error if the source and
		// destination buffers aren't of the same format.
		GLint depthSize;
		glGetIntegerv(GL_DEPTH_BITS, &depthSize);

		GLint depthFormat;

		switch (depthSize) {
		case 16:
			depthFormat = GL_DEPTH_COMPONENT16;
			break;
		case 24:
			depthFormat = GL_DEPTH_COMPONENT24;
			break;
		case 32:
			depthFormat = GL_DEPTH_COMPONENT32;
			break;
		default:
			fprintf(stderr, "WARNING: Window-system depth buffer size is unsupported for depth peeling: %d",
					depthSize);
			depthFormat = GL_DEPTH_COMPONENT;
		}


		gtaglBindFramebuffer(GTAGL_DRAW_FRAMEBUFFER, dpFBOs[0]);

		// Initialize the depth texture for FBO 0
		glBindTexture(GL_TEXTURE_2D, dpDepthTexes[0]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, depthFormat, viewW, viewH, 0, GL_DEPTH_COMPONENT, GL_FLOAT,
				NULL);
		gtaglFramebufferTexture2D(GTAGL_DRAW_FRAMEBUFFER, GTAGL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
				dpDepthTexes[0], 0);

		// Initialize the color texture for FBO 0
		glBindTexture(GL_TEXTURE_2D, dpColorTexes[0]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, viewW, viewH, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		gtaglFramebufferTexture2D(GTAGL_DRAW_FRAMEBUFFER, GTAGL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, dpColorTexes[0], 0);



		gtaglBindFramebuffer(GTAGL_DRAW_FRAMEBUFFER, dpFBOs[1]);

		// Initialize the depth texture for FBO 1
		glBindTexture(GL_TEXTURE_2D, dpDepthTexes[1]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, depthFormat, viewW, viewH, 0, GL_DEPTH_COMPONENT, GL_FLOAT,
				NULL);
		gtaglFramebufferTexture2D(GTAGL_DRAW_FRAMEBUFFER, GTAGL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
				dpDepthTexes[1], 0);

		// Initialize the color texture for FBO 1
		glBindTexture(GL_TEXTURE_2D, dpColorTexes[1]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, viewW, viewH, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		gtaglFramebufferTexture2D(GTAGL_DRAW_FRAMEBUFFER, GTAGL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, dpColorTexes[1], 0);



		gtaglBindFramebuffer(GTAGL_DRAW_FRAMEBUFFER, dpBlendFBO);

		// Initialize the color texture for the blending FBO
		glBindTexture(GL_TEXTURE_2D, dpBlendTex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, viewW, viewH, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		gtaglFramebufferTexture2D(GTAGL_DRAW_FRAMEBUFFER, GTAGL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, dpBlendTex, 0);
	}



	if (gtaglIsVersionSupported(3, 1)) {
		gtaglBindFramebuffer(GTAGL_DRAW_FRAMEBUFFER, wavgFBO);

		glBindTexture(GL_TEXTURE_RECTANGLE, wavgColorTex);
		glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGBA16F, viewW, viewH, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		gtaglFramebufferTexture2D(GTAGL_DRAW_FRAMEBUFFER, GTAGL_COLOR_ATTACHMENT0, GL_TEXTURE_RECTANGLE, wavgColorTex, 0);

		glBindTexture(GL_TEXTURE_RECTANGLE, wavgDepthComplexityTex);
		glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGBA16F, viewW, viewH, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		gtaglFramebufferTexture2D(GTAGL_DRAW_FRAMEBUFFER, GTAGL_COLOR_ATTACHMENT1, GL_TEXTURE_RECTANGLE,
				wavgDepthComplexityTex, 0);

		glBindTexture(GL_TEXTURE_RECTANGLE, wavgDepthBufferTex);
		glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_DEPTH_COMPONENT32, viewW, viewH, 0, GL_DEPTH_COMPONENT, GL_FLOAT,
				NULL);
		gtaglFramebufferTexture2D(GTAGL_DRAW_FRAMEBUFFER, GTAGL_DEPTH_ATTACHMENT, GL_TEXTURE_RECTANGLE,
				wavgDepthBufferTex, 0);
	} else {
		gtaglBindFramebuffer(GTAGL_FRAMEBUFFER, 0);

		// Query the size of the window-system depth buffer and try to choose the same format for the FBO
		// depth buffers. This is needed because glBlitFramebufferEXT generates an error if the source and
		// destination buffers aren't of the same format.
		GLint depthSize;
		glGetIntegerv(GL_DEPTH_BITS, &depthSize);

		GLint depthFormat;

		switch (depthSize) {
		case 16:
			depthFormat = GL_DEPTH_COMPONENT16;
			break;
		case 24:
			depthFormat = GL_DEPTH_COMPONENT24;
			break;
		case 32:
			depthFormat = GL_DEPTH_COMPONENT32;
			break;
		default:
			fprintf(stderr, "WARNING: Window-system depth buffer size is unsupported for weighted average: %d",
					depthSize);
			depthFormat = GL_DEPTH_COMPONENT;
		}


		gtaglBindFramebuffer(GTAGL_DRAW_FRAMEBUFFER, wavgFBO);

		glBindTexture(GL_TEXTURE_2D, wavgDepthBufferTex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, depthFormat, viewW, viewH, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT,
				NULL);
		gtaglFramebufferTexture2D(GTAGL_DRAW_FRAMEBUFFER, GTAGL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
				wavgDepthBufferTex, 0);

		glBindTexture(GL_TEXTURE_2D, wavgColorTex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, viewW, viewH, 0, GL_RGBA, GL_FLOAT, NULL);
		gtaglFramebufferTexture2D(GTAGL_DRAW_FRAMEBUFFER, GTAGL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, wavgColorTex, 0);

		glBindTexture(GL_TEXTURE_2D, wavgDepthComplexityTex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, viewW, viewH, 0, GL_RGBA, GL_FLOAT, NULL);
		gtaglFramebufferTexture2D(GTAGL_DRAW_FRAMEBUFFER, GTAGL_COLOR_ATTACHMENT1, GL_TEXTURE_2D,
				wavgDepthComplexityTex, 0);
	}


	gtaglBindFramebuffer(GTAGL_FRAMEBUFFER, 0);



	float planeData[] = {
			-1.0f, 1.0f,	// Upper left
			-1.0f, -1.0f,	// Lower left
			1.0f, 1.0f,		// Upper right
			1.0f, -1.0f		// Lower right
	};

	glGenBuffers(1, &planeDataBuf);
	glBindBuffer(GL_ARRAY_BUFFER, planeDataBuf);
	glBufferData(GL_ARRAY_BUFFER, 4*2*sizeof(float), planeData, GL_STATIC_DRAW);
}


void DefaultRenderer::enqueueForRendering(SceneObject* obj)
{
	int type = obj->getType();

	if (type == SceneObjectStatic) {
		if (obj->hasAlphaTransparency())
			objs.push_back(obj);
		else
			objs.insert(alphaBegin, obj);
	}
}


void DefaultRenderer::render()
{
	// Erase the dummy
	alphaBegin = objs.erase(alphaBegin);

	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);

	defaultProgram->makeCurrent();

	mvpMatrixUniform = defaultProgram->getUniformLocation("MVPMatrix");
	texturedUniform = defaultProgram->getUniformLocation("Textured");
	materialColorUniform = defaultProgram->getUniformLocation("MaterialColor");
	vertexColorsUniform = defaultProgram->getUniformLocation("VertexColors");
	textureUniform = defaultProgram->getUniformLocation("Texture");

	vertexAttrib = defaultProgram->getAttributeLocation("Vertex");
	normalAttrib = defaultProgram->getAttributeLocation("Normal");
	colorAttrib = defaultProgram->getAttributeLocation("Color");
	texCoordAttrib = defaultProgram->getAttributeLocation("TexCoord");

	glEnableVertexAttribArray(vertexAttrib);

	Engine* engine = Engine::getInstance();

	int viewW = engine->getViewportWidth();
	int viewH = engine->getViewportHeight();

	Camera* camera = engine->getCamera();

	Matrix4 mvpMatrix = engine->getProjectionMatrix();
	mvpMatrix *= Matrix4::lookAt(camera->getTarget(), camera->getUp());
	const Vector3& cpos = camera->getPosition();
	mvpMatrix.translate(-cpos);

	gtaglBindFramebuffer(GTAGL_FRAMEBUFFER, 0);

	ObjectList::iterator it;
	ObjectList::iterator beg = objs.begin();
	ObjectList::iterator end = objs.end();

	for (it = beg ; it != alphaBegin ; it++) {
		SceneObject* obj = *it;

		int type = obj->getType();

		if (type == SceneObjectStatic) {
			renderStaticSceneObject((StaticSceneObject*) obj, mvpMatrix);
		}
	}


	if (transMode == DepthPeeling) {
		mvpMatrixUniform = dpProgram->getUniformLocation("MVPMatrix");
		texturedUniform = dpProgram->getUniformLocation("Textured");
		materialColorUniform = dpProgram->getUniformLocation("MaterialColor");
		vertexColorsUniform = dpProgram->getUniformLocation("VertexColors");
		textureUniform = dpProgram->getUniformLocation("Texture");

		vertexAttrib = dpProgram->getAttributeLocation("Vertex");
		normalAttrib = dpProgram->getAttributeLocation("Normal");
		colorAttrib = dpProgram->getAttributeLocation("Color");
		texCoordAttrib = dpProgram->getAttributeLocation("TexCoord");

		GLint dpBlendLayerTexUniform = dpBlendLayerProgram->getUniformLocation("LayerTex");
		GLint dpBlendLayerVertexAttrib = dpBlendLayerProgram->getAttributeLocation("Vertex");

		GLint dpBlendFinalTexUniform = dpBlendFinalProgram->getUniformLocation("CombinedLayerTex");
		GLint dpBlendFinalVertexAttrib = dpBlendFinalProgram->getAttributeLocation("Vertex");

		GLint depthTexUniform = dpProgram->getUniformLocation("DepthTex");
		GLint texDimensionsUniform = dpProgram->getUniformLocation("TexDimensions");

		// Clear the blending FBO to (0, 0, 0, 1). Exactly this value is needed for the blending equations
		gtaglBindFramebuffer(GTAGL_DRAW_FRAMEBUFFER, dpBlendFBO);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// Clear input depth buffer to 0 (so that no fragment will be initially peeled away)
		gtaglBindFramebuffer(GTAGL_DRAW_FRAMEBUFFER, dpFBOs[1]);
		glClearDepth(0.0);
		glClear(GL_DEPTH_BUFFER_BIT);
		glClearDepth(1.0);

		// Our front-to-back blending equation. The source alpha is premultiplied in the fragment shader
		glBlendFuncSeparate(GL_DST_ALPHA, GL_ONE, GL_ZERO, GL_ONE_MINUS_SRC_ALPHA);

		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

		GLenum texTarget = gtaglIsVersionSupported(3, 1) ? GL_TEXTURE_RECTANGLE : GL_TEXTURE_2D;

		// Do the render passes...
		for (int i = 0 ; i < transPassCount ; i++) {
			int inputIdx = (i+1) % 2;
			int outputIdx = i % 2;

			glDisable(GL_BLEND);
			glEnable(GL_DEPTH_TEST);

			// ********** Peel a single layer **********

			dpProgram->makeCurrent();
			glEnableVertexAttribArray(vertexAttrib);

			// For OpenGL prior to 3.1 we use simple 2D textures instead of RECTANGLE ones, so we need the
			// dimensions of the depth buffer texture in our fragment shader.
			if (!gtaglIsVersionSupported(3, 1))
				glUniform2i(texDimensionsUniform, viewW, viewH);

			gtaglBindFramebuffer(GTAGL_READ_FRAMEBUFFER, 0);
			gtaglBindFramebuffer(GTAGL_DRAW_FRAMEBUFFER, dpFBOs[outputIdx]);

			glBindTexture(texTarget, dpDepthTexes[outputIdx]);

			// Clear the output color buffer and copy the opaque depth buffer to the output depth buffer
			glClear(GL_COLOR_BUFFER_BIT);
			glCopyTexImage2D(texTarget, 0, GL_DEPTH_COMPONENT24, 0, 0, viewW, viewH, 0);
			//gtaglBlitFramebuffer(0, 0, viewW, viewH, 0, 0, viewW, viewH, GL_DEPTH_BUFFER_BIT, GL_NEAREST);

			// Bind the input depth texture to texture unit 1 (unit 0 is used by the render* methods)
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(texTarget, dpDepthTexes[inputIdx]);
			glUniform1i(depthTexUniform, 1);

			// Back to texture unit 0 (the render* methods rely on it to be active)
			glActiveTexture(GL_TEXTURE0);

			// Render all alpha objects
			for (it = alphaBegin ; it != end ; it++) {
				SceneObject* obj = *it;

				int type = obj->getType();

				if (type == SceneObjectStatic) {
					renderStaticSceneObject((StaticSceneObject*) obj, mvpMatrix);
				}
			}


			// ********** Blend the peeled layer into dpBlendFBO **********

			dpBlendLayerProgram->makeCurrent();

			glEnable(GL_BLEND);
			glDisable(GL_DEPTH_TEST);

			gtaglBindFramebuffer(GTAGL_DRAW_FRAMEBUFFER, dpBlendFBO);

			// Render a screen-size plane with the contents of the output color buffer
			glBindBuffer(GL_ARRAY_BUFFER, planeDataBuf);
			glEnableVertexAttribArray(dpBlendLayerVertexAttrib);
			glVertexAttribPointer(dpBlendLayerVertexAttrib, 2, GL_FLOAT, GL_FALSE, 0, (void*) 0);

			glBindTexture(texTarget, dpColorTexes[outputIdx]);
			glUniform1i(dpBlendLayerTexUniform, 0);

			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		}


		// ********** Blend the contents of dpBlendFBO back into the window-system framebuffer **********

		dpBlendFinalProgram->makeCurrent();

		// We use GL_ONE as source factor because the values in dpBlendFBO are already premultiplied by alpha
		glBlendFunc(GL_ONE, GL_SRC_ALPHA);

		gtaglBindFramebuffer(GTAGL_DRAW_FRAMEBUFFER, 0);

		glBindBuffer(GL_ARRAY_BUFFER, planeDataBuf);
		glEnableVertexAttribArray(dpBlendFinalVertexAttrib);
		glVertexAttribPointer(dpBlendFinalVertexAttrib, 2, GL_FLOAT, GL_FALSE, 0, (void*) 0);

		glBindTexture(texTarget, dpBlendTex);
		glUniform1i(dpBlendFinalTexUniform, 0);

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

		gtaglBindFramebuffer(GTAGL_FRAMEBUFFER, 0);
	} else if (transMode == WeightedAverage) {
		GLenum texTarget = gtaglIsVersionSupported(3, 1) ? GL_TEXTURE_RECTANGLE : GL_TEXTURE_2D;

		gtaglBindFramebuffer(GTAGL_DRAW_FRAMEBUFFER, wavgFBO);

		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE);

		glEnable(GL_DEPTH_TEST);

		glBindTexture(texTarget, wavgDepthBufferTex);

		glCopyTexImage2D(texTarget, 0, GL_DEPTH_COMPONENT24, 0, 0, viewW, viewH, 0);

		glDepthMask(GL_FALSE);

		GLenum drawBufs[] = {GTAGL_COLOR_ATTACHMENT0, GTAGL_COLOR_ATTACHMENT1};
		glDrawBuffers(2, drawBufs);

		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		wavgProgram->makeCurrent();

		mvpMatrixUniform = wavgProgram->getUniformLocation("MVPMatrix");
		texturedUniform = wavgProgram->getUniformLocation("Textured");
		materialColorUniform = wavgProgram->getUniformLocation("MaterialColor");
		vertexColorsUniform = wavgProgram->getUniformLocation("VertexColors");
		textureUniform = wavgProgram->getUniformLocation("Texture");

		vertexAttrib = wavgProgram->getAttributeLocation("Vertex");
		normalAttrib = wavgProgram->getAttributeLocation("Normal");
		colorAttrib = wavgProgram->getAttributeLocation("Color");
		texCoordAttrib = wavgProgram->getAttributeLocation("TexCoord");

		GLint colorTexUniform = wavgBlendFinalProgram->getUniformLocation("ColorTex");
		GLint dcTexUniform = wavgBlendFinalProgram->getUniformLocation("DepthComplexityTex");
		GLint wavgFinalVertexAttrib = wavgBlendFinalProgram->getAttributeLocation("Vertex");

		for (it = alphaBegin ; it != end ; it++) {
			SceneObject* obj = *it;

			int type = obj->getType();

			if (type == SceneObjectStatic) {
				renderStaticSceneObject((StaticSceneObject*) obj, mvpMatrix);
			}
		}

		wavgBlendFinalProgram->makeCurrent();

		gtaglBindFramebuffer(GTAGL_FRAMEBUFFER, 0);

		glDisable(GL_DEPTH_TEST);

		glBlendFunc(GL_ONE, GL_SRC_ALPHA);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(texTarget, wavgColorTex);
		glUniform1i(colorTexUniform, 0);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(texTarget, wavgDepthComplexityTex);
		glUniform1i(dcTexUniform, 1);

		glBindBuffer(GL_ARRAY_BUFFER, planeDataBuf);
		glEnableVertexAttribArray(wavgFinalVertexAttrib);
		glVertexAttribPointer(wavgFinalVertexAttrib, 2, GL_FLOAT, GL_FALSE, 0, (void*) 0);

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		GLenum drawBuf = GL_BACK_LEFT;
		glDrawBuffers(1, &drawBuf);

		glActiveTexture(GL_TEXTURE0);

		glDepthMask(GL_TRUE);
		glEnable(GL_DEPTH_TEST);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	}

	objs.clear();

	// Push the dummy back in.
	objs.push_back(NULL);

	alphaBegin = objs.begin();
}


void DefaultRenderer::renderStaticSceneObject(StaticSceneObject* obj, const Matrix4& mvpMatrix)
{
	Matrix4 mat = mvpMatrix * obj->getModelMatrix();
	const float* matData = mat.toArray();
	glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, matData);

	Mesh* mesh = **obj->getDefinition()->getMeshPointer();
	TextureSource* texSrc = obj->getDefinition()->getTextureSource();

	if (!mesh)
		return;

	mesh->bindDataBuffer();

	int vertexOffs = mesh->getVertexOffset();
	int normalOffs = mesh->getNormalOffset();
	int colorOffs = mesh->getVertexColorOffset();
	int texCoordOffs = mesh->getTexCoordOffset();

	glVertexAttribPointer(vertexAttrib, 3, GL_FLOAT, GL_FALSE, 0, (void*) vertexOffs);

	if (normalAttrib != -1) {
		if (normalOffs != -1) {
			glEnableVertexAttribArray(normalAttrib);
			glVertexAttribPointer(normalAttrib, 3, GL_FLOAT, GL_FALSE, 0, (void*) normalOffs);
		} else {
			glDisableVertexAttribArray(normalAttrib);
		}
	}

	if (colorAttrib != -1) {
		if (colorOffs != -1) {
			glEnableVertexAttribArray(colorAttrib);
			glVertexAttribPointer(colorAttrib, 4, GL_UNSIGNED_BYTE, GL_FALSE, 0, (void*) colorOffs);
		} else {
			glDisableVertexAttribArray(colorAttrib);
		}
	}

	if (texCoordAttrib != -1) {
		if (texCoordOffs != -1) {
			glEnableVertexAttribArray(texCoordAttrib);
			glVertexAttribPointer(texCoordAttrib, 2, GL_FLOAT, GL_FALSE, 0, (void*) texCoordOffs);
		}
	}

	GLenum mode;

	switch (mesh->getVertexFormat()) {
	case VertexFormatTriangles:
		mode = GL_TRIANGLES;
		break;
	case VertexFormatTriangleStrips:
		mode = GL_TRIANGLE_STRIP;
		break;
	case VertexFormatLines:
		mode = GL_LINES;
		break;
	case VertexFormatPoints:
		mode = GL_POINTS;
		break;
	}

	glUniform1i(vertexColorsUniform, (colorOffs != -1) ? 1 : 0);

	Mesh::SubmeshIterator it;
	Mesh::SubmeshIterator beg = mesh->getSubmeshBegin();
	Mesh::SubmeshIterator end = mesh->getSubmeshEnd();

	for (it = beg ; it != end ; it++) {
		Submesh* submesh = *it;

		Material* mat = submesh->getMaterial();

		if (mat) {
			if (mat->isTextured()) {
				if (texSrc) {
					Texture* tex = texSrc->getTexture(mat->getTextureHash());

					if (tex) {
						GLuint texGL = tex->getGLTexture();
						glBindTexture(GL_TEXTURE_2D, texGL);
						glUniform1i(texturedUniform, 1);
						glUniform1i(textureUniform, 0);
					} else {
						glUniform1i(texturedUniform, 0);
					}
				} else {
					glUniform1i(texturedUniform, 0);
				}
			} else {
				glUniform1i(texturedUniform, 0);
			}

			uint8_t r, g, b, a;
			mat->getColor(r, g, b, a);
			glUniform4f(materialColorUniform, 0.0039215686f*r, 0.0039215686f*g, 0.0039215686f*b,
					0.0039215686f*a);
		} else {
			glUniform4f(materialColorUniform, 1.0f, 1.0f, 1.0f, 0.0f);
		}

		submesh->bindIndexBuffer();

		glDrawElements(mode, submesh->getIndexCount(), GL_UNSIGNED_INT, (void*) 0);
	}
}

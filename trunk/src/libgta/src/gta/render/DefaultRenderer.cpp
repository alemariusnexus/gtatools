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

#include "DefaultRenderer.h"

#include "../Engine.h"
#include "../GLException.h"

#include "../scene/objects/DirectionalLightSource.h"
#include "../scene/objects/PointLightSource.h"
#include "../scene/objects/AmbientLightSource.h"
#include "../scene/objects/SpotLightSource.h"


#include <res_vertex_default_shader.h>
#include <res_fragment_default_shader.h>
#include <res_shade_vertex_shader.h>
#include <res_shade_fragment_shader.h>
#include <res_anim_shade_vertex_shader.h>
#include <res_anim_shade_fragment_shader.h>
#include <res_lighting_vertex_shader.h>

#include <res_dp_peel_layer_vertex_shader.h>
#include <res_dp_peel_layer_fragment_shader.h>
#include <res_dp_blend_layer_vertex_shader.h>
#include <res_dp_blend_layer_fragment_shader.h>
#include <res_dp_blend_final_vertex_shader.h>
#include <res_dp_blend_final_fragment_shader.h>




DefaultRenderer::DefaultRenderer()
		: globalAmbientLightEnabled(true), dpNumPasses(6)
{
	setupShaders();
	setupFBOs();
	setupBuffers();
}


void DefaultRenderer::setupShaders()
{
	const char* vertexDefaultData;
	const char* fragmentDefaultData;
	const char* shadeVertexShaderData;
	const char* shadeFragmentShaderData;
	const char* animShadeVertexShaderData;
	const char* animShadeFragmentShaderData;
	const char* lightingVertexShaderData;

	const char* dpPeelLayerVertexShaderData;
	const char* dpPeelLayerFragmentShaderData;
	const char* dpBlendLayerVertexShaderData;
	const char* dpBlendLayerFragmentShaderData;
	const char* dpBlendFinalVertexShaderData;
	const char* dpBlendFinalFragmentShaderData;

	int vertexDefaultDataLen;
	int fragmentDefaultDataLen;
	int shadeVertexShaderDataLen;
	int shadeFragmentShaderDataLen;
	int animShadeVertexShaderDataLen;
	int animShadeFragmentShaderDataLen;
	int lightingVertexShaderDataLen;

	int dpPeelLayerVertexShaderDataLen;
	int dpPeelLayerFragmentShaderDataLen;
	int dpBlendLayerVertexShaderDataLen;
	int dpBlendLayerFragmentShaderDataLen;
	int dpBlendFinalVertexShaderDataLen;
	int dpBlendFinalFragmentShaderDataLen;

	vertexDefaultData					= (const char*) res_vertex_default_shader_data;
	fragmentDefaultData					= (const char*) res_fragment_default_shader_data;
	shadeVertexShaderData				= (const char*) res_shade_vertex_shader_data;
	shadeFragmentShaderData				= (const char*) res_shade_fragment_shader_data;
	animShadeVertexShaderData			= (const char*) res_anim_shade_vertex_shader_data;
	animShadeFragmentShaderData			= (const char*) res_anim_shade_fragment_shader_data;
	lightingVertexShaderData			= (const char*) res_lighting_vertex_shader_data;

	dpPeelLayerVertexShaderData			= (const char*) res_dp_peel_layer_vertex_shader_data;
	dpPeelLayerFragmentShaderData		= (const char*) res_dp_peel_layer_fragment_shader_data;
	dpBlendLayerVertexShaderData		= (const char*) res_dp_blend_layer_vertex_shader_data;
	dpBlendLayerFragmentShaderData		= (const char*) res_dp_blend_layer_fragment_shader_data;
	dpBlendFinalVertexShaderData		= (const char*) res_dp_blend_final_vertex_shader_data;
	dpBlendFinalFragmentShaderData		= (const char*) res_dp_blend_final_fragment_shader_data;

	vertexDefaultDataLen					= sizeof(res_vertex_default_shader_data);
	fragmentDefaultDataLen					= sizeof(res_fragment_default_shader_data);
	shadeVertexShaderDataLen				= sizeof(res_shade_vertex_shader_data);
	shadeFragmentShaderDataLen				= sizeof(res_shade_fragment_shader_data);
	animShadeVertexShaderDataLen			= sizeof(res_anim_shade_vertex_shader_data);
	animShadeFragmentShaderDataLen			= sizeof(res_anim_shade_fragment_shader_data);
	lightingVertexShaderDataLen				= sizeof(res_lighting_vertex_shader_data);

	dpPeelLayerVertexShaderDataLen			= sizeof(res_dp_peel_layer_vertex_shader_data);
	dpPeelLayerFragmentShaderDataLen		= sizeof(res_dp_peel_layer_fragment_shader_data);
	dpBlendLayerVertexShaderDataLen			= sizeof(res_dp_blend_layer_vertex_shader_data);
	dpBlendLayerFragmentShaderDataLen		= sizeof(res_dp_blend_layer_fragment_shader_data);
	dpBlendFinalVertexShaderDataLen			= sizeof(res_dp_blend_final_vertex_shader_data);
	dpBlendFinalFragmentShaderDataLen		= sizeof(res_dp_blend_final_fragment_shader_data);


#ifdef GTA_USE_OPENGL_ES
	glGetIntegerv(GL_MAX_VERTEX_UNIFORM_VECTORS, &maxVertexUniformComponents);
	maxVertexUniformComponents *= 4;
#else
	glGetIntegerv(GL_MAX_VERTEX_UNIFORM_COMPONENTS, &maxVertexUniformComponents);
#endif


	shaders.dpPeelLayerVertexShader = new Shader(GL_VERTEX_SHADER,
			"Depth Peeling Transparency Algorithm Peel Layer Vertex Shader");
	shaders.dpPeelLayerVertexShader->loadSourceCode(CString(dpPeelLayerVertexShaderData,
			dpPeelLayerVertexShaderDataLen));
	shaders.dpPeelLayerVertexShader->compile();

	shaders.dpPeelLayerFragmentShader = new Shader(GL_FRAGMENT_SHADER,
			"Depth Peeling Transparency Algorithm Peel Layer Fragment Shader");
	shaders.dpPeelLayerFragmentShader->loadSourceCode(CString(dpPeelLayerFragmentShaderData,
			dpPeelLayerFragmentShaderDataLen));
	shaders.dpPeelLayerFragmentShader->compile();

	shaders.dpBlendLayerVertexShader = new Shader(GL_VERTEX_SHADER,
			"Depth Peeling Transparency Algorithm Blend Layer Vertex Shader");
	shaders.dpBlendLayerVertexShader->loadSourceCode(CString(dpBlendLayerVertexShaderData,
			dpBlendLayerVertexShaderDataLen));
	shaders.dpBlendLayerVertexShader->compile();

	shaders.dpBlendLayerFragmentShader = new Shader(GL_FRAGMENT_SHADER,
			"Depth Peeling Transparency Algorithm Blend Layer Fragment Shader");
	shaders.dpBlendLayerFragmentShader->loadSourceCode(CString(dpBlendLayerFragmentShaderData,
			dpBlendLayerFragmentShaderDataLen));
	shaders.dpBlendLayerFragmentShader->compile();

	shaders.dpBlendFinalVertexShader = new Shader(GL_VERTEX_SHADER,
			"Depth Peeling Transparency Algorithm Blend Final Vertex Shader");
	shaders.dpBlendFinalVertexShader->loadSourceCode(CString(dpBlendFinalVertexShaderData,
			dpBlendFinalVertexShaderDataLen));
	shaders.dpBlendFinalVertexShader->compile();

	shaders.dpBlendFinalFragmentShader = new Shader(GL_FRAGMENT_SHADER,
			"Depth Peeling Transparency Algorithm Blend Final Fragment Shader");
	shaders.dpBlendFinalFragmentShader->loadSourceCode(CString(dpBlendFinalFragmentShaderData,
			dpBlendFinalFragmentShaderDataLen));
	shaders.dpBlendFinalFragmentShader->compile();

	programs.dpBlendLayerProgram = new ShaderProgram(
			"Depth Peeling Transparency Algorithm Blend Layer Shader Program");
	programs.dpBlendLayerProgram->attachShader(shaders.dpBlendLayerVertexShader);
	programs.dpBlendLayerProgram->attachShader(shaders.dpBlendLayerFragmentShader);
	programs.dpBlendLayerProgram->link();

	programs.dpBlendFinalProgram = new ShaderProgram(
			"Depth Peeling Transparency Algorithm Blend Final Shader Program");
	programs.dpBlendFinalProgram->attachShader(shaders.dpBlendFinalVertexShader);
	programs.dpBlendFinalProgram->attachShader(shaders.dpBlendFinalFragmentShader);
	programs.dpBlendFinalProgram->link();


	shaders.shadeVertexShader = new Shader(GL_VERTEX_SHADER, "Static Vertex Shader");
	shaders.shadeVertexShader->loadSourceCode(CString(shadeVertexShaderData, shadeVertexShaderDataLen));
	shaders.shadeVertexShader->compile();

	shaders.shadeFragmentShader = new Shader(GL_FRAGMENT_SHADER, "Static Fragment Shader");
	shaders.shadeFragmentShader->loadSourceCode(CString(shadeFragmentShaderData, shadeFragmentShaderDataLen));
	shaders.shadeFragmentShader->compile();

	shaders.vertexDefaultShader = new Shader(GL_VERTEX_SHADER, "Default Opaque Vertex Shader");
	shaders.vertexDefaultShader->loadSourceCode(CString(vertexDefaultData, vertexDefaultDataLen));
	shaders.vertexDefaultShader->compile();

	shaders.fragmentDefaultShader = new Shader(GL_FRAGMENT_SHADER, "Default Opaque Fragment Shader");
	shaders.fragmentDefaultShader->loadSourceCode(CString(fragmentDefaultData, fragmentDefaultDataLen));
	shaders.fragmentDefaultShader->compile();

	CString animShadeVertexShaderCode = CString("");
	if (maxVertexUniformComponents >= 2048) {
		animShadeVertexShaderCode.append("#define GTAGL_BONE_MATRIX_UNIFORM_ARRAY_100\n#line 0\n");
	}
	animShadeVertexShaderCode.append(CString(animShadeVertexShaderData, animShadeVertexShaderDataLen));

	/*svVertexShader = new Shader(GL_VERTEX_SHADER, "Test Vertex Shader");
	svVertexShader->loadSourceCode(File("/home/alemariusnexus/test_vertex.glsl"));
	svVertexShader->compile();

	svFragmentShader = new Shader(GL_FRAGMENT_SHADER, "Test Fragment Shader");
	svFragmentShader->loadSourceCode(File("/home/alemariusnexus/test_fragment.glsl"));
	svFragmentShader->compile();

	svProgram = new ShaderProgram("Test Program");
	svProgram->attachShader(svVertexShader);
	svProgram->attachShader(svFragmentShader);
	svProgram->link();

	svBlendVertexShader = new Shader(GL_VERTEX_SHADER, "Shadow Volume Layer Blend Vertex Shader");
	svBlendVertexShader->loadSourceCode(File("/home/alemariusnexus/blend_vertex.glsl"));
	svBlendVertexShader->compile();

	svBlendFragmentShader = new Shader(GL_FRAGMENT_SHADER, "Shadow Volume Layer Blend Fragment Shader");
	svBlendFragmentShader->loadSourceCode(File("/home/alemariusnexus/blend_fragment.glsl"));
	svBlendFragmentShader->compile();

	svBlendProgram = new ShaderProgram("Shadow Volume Layer Blend Program");
	svBlendProgram->attachShader(svBlendVertexShader);
	svBlendProgram->attachShader(svBlendFragmentShader);
	svBlendProgram->link();*/

	shaders.animShadeVertexShader = new Shader(GL_VERTEX_SHADER, "Animated Vertex Shader");
	shaders.animShadeVertexShader->loadSourceCode(animShadeVertexShaderCode);
	shaders.animShadeVertexShader->compile();

	shaders.animShadeFragmentShader = new Shader(GL_FRAGMENT_SHADER, "Animated Fragment Shader");
	shaders.animShadeFragmentShader->loadSourceCode(CString(animShadeFragmentShaderData, animShadeFragmentShaderDataLen));
	shaders.animShadeFragmentShader->compile();

	shaders.lightingVertexShader = new Shader(GL_VERTEX_SHADER, "Lighting Vertex Shader");
	shaders.lightingVertexShader->loadSourceCode(CString(lightingVertexShaderData, lightingVertexShaderDataLen));
	shaders.lightingVertexShader->compile();

	programs.opaqueStaticProgram = new ShaderProgram("Opaque Static Shader Program");
	programs.opaqueStaticProgram->attachShader(shaders.shadeVertexShader);
	programs.opaqueStaticProgram->attachShader(shaders.shadeFragmentShader);
	programs.opaqueStaticProgram->attachShader(shaders.vertexDefaultShader);
	programs.opaqueStaticProgram->attachShader(shaders.fragmentDefaultShader);
	programs.opaqueStaticProgram->attachShader(shaders.lightingVertexShader);
	programs.opaqueStaticProgram->link();

	programs.transStaticProgram = new ShaderProgram("Transparent Static Shader Program");
	programs.transStaticProgram->attachShader(shaders.shadeVertexShader);
	programs.transStaticProgram->attachShader(shaders.shadeFragmentShader);
	programs.transStaticProgram->attachShader(shaders.lightingVertexShader);
	programs.transStaticProgram->attachShader(shaders.dpPeelLayerVertexShader);
	programs.transStaticProgram->attachShader(shaders.dpPeelLayerFragmentShader);
	programs.transStaticProgram->link();


	programs.opaqueAnimProgram = new ShaderProgram("Opaque Animated Shader Program");
	programs.opaqueAnimProgram->attachShader(shaders.animShadeVertexShader);
	programs.opaqueAnimProgram->attachShader(shaders.animShadeFragmentShader);
	programs.opaqueAnimProgram->attachShader(shaders.vertexDefaultShader);
	programs.opaqueAnimProgram->attachShader(shaders.fragmentDefaultShader);
	programs.opaqueAnimProgram->attachShader(shaders.lightingVertexShader);
	programs.opaqueAnimProgram->link();

	programs.transAnimProgram = new ShaderProgram("Transparent Animated Shader Program");
	programs.transAnimProgram->attachShader(shaders.animShadeVertexShader);
	programs.transAnimProgram->attachShader(shaders.animShadeFragmentShader);
	programs.transAnimProgram->attachShader(shaders.lightingVertexShader);
	programs.transAnimProgram->attachShader(shaders.dpPeelLayerVertexShader);
	programs.transAnimProgram->attachShader(shaders.dpPeelLayerFragmentShader);
	programs.transAnimProgram->link();


	dpShaderLocations.dpBlendLayerTexUniform = programs.dpBlendLayerProgram->getUniformLocation("LayerTex");
	dpShaderLocations.dpBlendLayerVertexAttrib = programs.dpBlendLayerProgram->getAttributeLocation("Vertex");

	dpShaderLocations.dpBlendFinalTexUniform = programs.dpBlendFinalProgram->getUniformLocation("CombinedLayerTex");
	dpShaderLocations.dpBlendFinalVertexAttrib = programs.dpBlendFinalProgram->getAttributeLocation("Vertex");
}


void DefaultRenderer::setupFBOs()
{
	Engine* engine = Engine::getInstance();

	gtaglGenFramebuffers(1, &fbos.dpFBO);
	glGenTextures(1, &fbos.dpColorTex);

	gtaglGenFramebuffers(2, fbos.dpPingPongFBOs);
	gtaglGenFramebuffers(1, &fbos.dpOpaqueFBO);

	glGenTextures(2, fbos.dpPingPongDepthStencilTexes);
	glGenTextures(2, fbos.dpPingPongColorTexes);
	glGenTextures(2, fbos.dpOpaqueTexes);


	int viewW = engine->getViewportWidth();
	int viewH = engine->getViewportHeight();

#ifndef GTA_USE_OPENGL_ES
	if (gtaglIsVersionSupported(3, 1)) {
		gtaglBindFramebuffer(GTAGL_FRAMEBUFFER, fbos.dpFBO);

		// Initialize the color texture for the main FBO
		glBindTexture(GL_TEXTURE_RECTANGLE, fbos.dpColorTex);
		glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGBA, viewW, viewH, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		gtaglFramebufferTexture2D(GTAGL_FRAMEBUFFER, GTAGL_COLOR_ATTACHMENT0, GL_TEXTURE_RECTANGLE, fbos.dpColorTex, 0);

		GLException::checkFramebufferStatus(GTAGL_FRAMEBUFFER,
				"During initialization of depth peeling result FBO");
	} else {
#endif
		gtaglBindFramebuffer(GTAGL_FRAMEBUFFER, fbos.dpFBO);

		// Initialize the color texture for the main FBO
		glBindTexture(GL_TEXTURE_2D, fbos.dpColorTex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, viewW, viewH, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		gtaglFramebufferTexture2D(GTAGL_FRAMEBUFFER, GTAGL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbos.dpColorTex, 0);

		GLException::checkFramebufferStatus(GTAGL_FRAMEBUFFER,
				"During initialization of depth peeling result FBO");
#ifndef GTA_USE_OPENGL_ES
	}
#endif

	printf("%d, %d\n", viewW, viewH);

	GLException::checkError("After FBO texes");

	GLException::checkFramebufferStatus(GTAGL_FRAMEBUFFER,
				"GALLALA X");



#ifndef GTA_USE_OPENGL_ES
	if (gtaglIsVersionSupported(3, 1)) {
		gtaglBindFramebuffer(GTAGL_FRAMEBUFFER, fbos.dpPingPongFBOs[0]);

		// Initialize the depth-stencil texture for ping-pong FBO 0
		glBindTexture(GL_TEXTURE_RECTANGLE, fbos.dpPingPongDepthStencilTexes[0]);
		glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_DEPTH24_STENCIL8, viewW, viewH, 0, GL_DEPTH_STENCIL,
				GL_UNSIGNED_INT_24_8, NULL);
		gtaglFramebufferTexture2D(GTAGL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_RECTANGLE,
				fbos.dpPingPongDepthStencilTexes[0], 0);

		// Initialize the color texture for ping-pong FBO 0
		glBindTexture(GL_TEXTURE_RECTANGLE, fbos.dpPingPongColorTexes[0]);
		glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGBA, viewW, viewH, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		gtaglFramebufferTexture2D(GTAGL_FRAMEBUFFER, GTAGL_COLOR_ATTACHMENT0, GL_TEXTURE_RECTANGLE,
				fbos.dpPingPongColorTexes[0], 0);

		GLException::checkFramebufferStatus(GTAGL_FRAMEBUFFER,
				"During initialization of depth peeling FBO 0");


		gtaglBindFramebuffer(GTAGL_FRAMEBUFFER, fbos.dpPingPongFBOs[1]);

		// Initialize the depth-stencil texture for ping-pong FBO 1
		glBindTexture(GL_TEXTURE_RECTANGLE, fbos.dpPingPongDepthStencilTexes[1]);
		glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_DEPTH24_STENCIL8, viewW, viewH, 0, GL_DEPTH_STENCIL,
				GL_UNSIGNED_INT_24_8, NULL);
		gtaglFramebufferTexture2D(GTAGL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_RECTANGLE,
				fbos.dpPingPongDepthStencilTexes[1], 0);

		// Initialize the color texture for ping-pong FBO 1
		glBindTexture(GL_TEXTURE_RECTANGLE, fbos.dpPingPongColorTexes[1]);
		glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGBA, viewW, viewH, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		gtaglFramebufferTexture2D(GTAGL_FRAMEBUFFER, GTAGL_COLOR_ATTACHMENT0, GL_TEXTURE_RECTANGLE,
				fbos.dpPingPongColorTexes[1], 0);

		GLException::checkFramebufferStatus(GTAGL_FRAMEBUFFER,
				"During initialization of depth peeling FBO 1");



		gtaglBindFramebuffer(GTAGL_FRAMEBUFFER, fbos.dpOpaqueFBO);

		// Initialize the color texture for the opaque FBO
		glBindTexture(GL_TEXTURE_RECTANGLE, fbos.dpOpaqueTexes[0]);
		glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGBA, viewW, viewH, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		gtaglFramebufferTexture2D(GTAGL_FRAMEBUFFER, GTAGL_COLOR_ATTACHMENT0, GL_TEXTURE_RECTANGLE,
				fbos.dpOpaqueTexes[0], 0);

		// Initialize the depth-stencil texture for the opaque FBO
		glBindTexture(GL_TEXTURE_RECTANGLE, fbos.dpOpaqueTexes[1]);
		glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_DEPTH24_STENCIL8, viewW, viewH, 0, GL_DEPTH_STENCIL,
				GL_UNSIGNED_INT_24_8, NULL);
		gtaglFramebufferTexture2D(GTAGL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_RECTANGLE,
				fbos.dpOpaqueTexes[1], 0);

		GLException::checkFramebufferStatus(GTAGL_FRAMEBUFFER,
				"During initialization of depth peeling blend FBO");
	} else {
#endif
		/*gtaglBindFramebuffer(GTAGL_FRAMEBUFFER, fbos[0]);

		// Initialize the depth texture for FBO 0
		glBindTexture(GL_TEXTURE_2D, depthTexes[0]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, viewW, viewH, 0, GL_DEPTH_COMPONENT,
				GL_UNSIGNED_INT, NULL);
		gtaglFramebufferTexture2D(GTAGL_FRAMEBUFFER, GTAGL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
				depthTexes[0], 0);

		// Initialize the color texture for FBO 0
		glBindTexture(GL_TEXTURE_2D, colorTexes[0]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, viewW, viewH, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		gtaglFramebufferTexture2D(GTAGL_FRAMEBUFFER, GTAGL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTexes[0], 0);

		GLException::checkFramebufferStatus(GTAGL_FRAMEBUFFER,
				"During initialization of depth peeling FBO 0");



		gtaglBindFramebuffer(GTAGL_FRAMEBUFFER, fbos[1]);

		// Initialize the depth texture for FBO 1
		glBindTexture(GL_TEXTURE_2D, depthTexes[1]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, viewW, viewH, 0, GL_DEPTH_COMPONENT,
				GL_UNSIGNED_INT, NULL);
		gtaglFramebufferTexture2D(GTAGL_FRAMEBUFFER, GTAGL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
				depthTexes[1], 0);

		// Initialize the color texture for FBO 1
		glBindTexture(GL_TEXTURE_2D, colorTexes[1]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, viewW, viewH, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		gtaglFramebufferTexture2D(GTAGL_FRAMEBUFFER, GTAGL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTexes[1], 0);

		GLException::checkFramebufferStatus(GTAGL_FRAMEBUFFER,
				"During initialization of depth peeling FBO 1");



		gtaglBindFramebuffer(GTAGL_FRAMEBUFFER, mainFBO);

		// Initialize the color texture for the main FBO
		glBindTexture(GL_TEXTURE_2D, mainTexes[0]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, viewW, viewH, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		gtaglFramebufferTexture2D(GTAGL_FRAMEBUFFER, GTAGL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mainTexes[0], 0);

		// Initialize the depth texture for the main FBO
		glBindTexture(GL_TEXTURE_2D, mainTexes[1]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, viewW, viewH, 0, GL_DEPTH_COMPONENT,
				GL_UNSIGNED_INT, NULL);
		gtaglFramebufferTexture2D(GTAGL_FRAMEBUFFER, GTAGL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, mainTexes[1], 0);


		GLException::checkFramebufferStatus(GTAGL_FRAMEBUFFER,
				"During initialization of depth peeling blend FBO");*/
#ifndef GTA_USE_OPENGL_ES
	}
#endif



	gtaglBindFramebuffer(GTAGL_FRAMEBUFFER, 0);
}


void DefaultRenderer::setupBuffers()
{
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


void DefaultRenderer::enqueueForRendering(list<RenderingEntity*>::iterator beg, list<RenderingEntity*>::iterator end)
{
	for (list<RenderingEntity*>::iterator it = beg ; it != end ; it++) {
		RenderingEntity* entity = *it;

		AnimatedRenderingMesh* amesh = dynamic_cast<AnimatedRenderingMesh*>(entity);

		if (amesh) {
			if (amesh->hasTransparency()) {
				transAnimMeshes.push_back(amesh);
			} else {
				opaqueAnimMeshes.push_back(amesh);
			}
			continue;
		}

		StaticRenderingMesh* smesh = dynamic_cast<StaticRenderingMesh*>(entity);

		if (smesh->hasTransparency()) {
			transStaticMeshes.push_back(smesh);
		} else {
			opaqueStaticMeshes.push_back(smesh);
		}
	}
}


void DefaultRenderer::enqueueForRendering(list<LightSource*>::iterator beg, list<LightSource*>::iterator end)
{
	for (list<LightSource*>::iterator it = beg ; it != end ; it++) {
		lightSources.push_back(*it);
	}
}


void DefaultRenderer::initializeUniforms(ShaderProgram* program)
{
	map<ShaderProgram*, ShaderProgramLocations>::iterator it = spLocs.find(program);

	ShaderProgramLocations locs;

	if (it == spLocs.end()) {
		ShaderProgramLocations loc;

		loc.camPosUniform = program->getUniformLocation("CameraPosition");
		loc.mvMatrixUniform = program->getUniformLocation("MVMatrix");
		loc.mvpMatrixUniform = program->getUniformLocation("MVPMatrix");
		loc.normalMvMatrixUniform = program->getUniformLocation("NormalMVMatrix");
		loc.texturedUniform = program->getUniformLocation("Textured");
		loc.materialColorUniform = program->getUniformLocation("MaterialColor");
		loc.vertexColorsUniform = program->getUniformLocation("VertexColors");
		loc.textureUniform = program->getUniformLocation("Texture");

		loc.matAmbientReflectionUniform = program->getUniformLocation("MaterialAmbientReflection");
		loc.matDiffuseReflectionUniform = program->getUniformLocation("MaterialDiffuseReflection");
		loc.matSpecularReflectionUniform = program->getUniformLocation("MaterialSpecularReflection");
		loc.globalAmbientLightUniform = program->getUniformLocation("GlobalAmbientLight");
		loc.dynamicLightingEnabledUniform = program->getUniformLocation("DynamicLightingEnabled");

		loc.vertexAttrib = program->getAttributeLocation("Vertex");
		loc.normalAttrib = program->getAttributeLocation("Normal");
		loc.colorAttrib = program->getAttributeLocation("Color");
		loc.texCoordAttrib = program->getAttributeLocation("TexCoord");

		loc.boneMatUniform = program->getUniformLocation("BoneMatrices");
		loc.boneUniform = program->getUniformLocation("Bone");
		loc.boneWeightAttrib = program->getAttributeLocation("BoneWeights");
		loc.boneIndexAttrib = program->getAttributeLocation("BoneIndices");

		if (!gtaglIsVersionSupported(3, 1)) {
			loc.boneMatSizeUniform = program->getUniformLocation("TexSize");
		} else {
			loc.boneMatSizeUniform = -1;
		}

		for (int i = 0 ; i < 10 ; i++) {
			char baseName[32];
			sprintf(baseName, "LightSources[%d]", i);

			LightSourceUniformLocations& lloc = loc.lightSources[i];

			lloc.ambient = program->getUniformLocation(CString(baseName).append(".ambient"));
			lloc.diffuse = program->getUniformLocation(CString(baseName).append(".diffuse"));
			lloc.specular = program->getUniformLocation(CString(baseName).append(".specular"));
			lloc.position = program->getUniformLocation(CString(baseName).append(".position"));
			lloc.direction = program->getUniformLocation(CString(baseName).append(".direction"));
			lloc.cutoffAngleCos = program->getUniformLocation(CString(baseName).append(".cutoffAngleCos"));
			lloc.shininess = program->getUniformLocation(CString(baseName).append(".shininess"));
			lloc.spotlightExponent = program->getUniformLocation(CString(baseName).append(".spotlightExponent"));
			lloc.constAttenuation = program->getUniformLocation(CString(baseName).append(".constAttenuation"));
			lloc.linearAttenuation = program->getUniformLocation(CString(baseName).append(".linearAttenuation"));
			lloc.quadAttenuation = program->getUniformLocation(CString(baseName).append(".quadAttenuation"));
		}

		locs = loc;

		it = spLocs.insert(pair<ShaderProgram*, ShaderProgramLocations>(program, loc)).first;
	}

	renderProgramLocations = &it->second;
}


void DefaultRenderer::setupDynamicLighting(bool enabled)
{
	if (globalAmbientLightEnabled) {
		Vector3 ambientLight = Vector3::Zero;

		for (LightSourceIterator it = lightSources.begin() ; it != lightSources.end() ; it++) {
			LightSource* light = *it;

			if (light->getLightType() == LightSource::AmbientLight  &&  light->isEnabled()) {
				ambientLight += light->getAmbientColor();
			}
		}

		glUniform4f(renderProgramLocations->globalAmbientLightUniform,
				ambientLight.getX(), ambientLight.getY(), ambientLight.getZ(), 0.0f);
	} else {
		glUniform4f(renderProgramLocations->globalAmbientLightUniform, 0.0f, 0.0f, 0.0f, 0.0f);
	}

	if (enabled) {
		glUniform1i(renderProgramLocations->dynamicLightingEnabledUniform, 1);

		int i = 0;
		for (LightSourceIterator it = enabledLightBegin ; it != enabledLightEnd ; it++) {
			LightSource* light = *it;

			if (light->getLightType() == LightSource::AmbientLight  ||  !light->isEnabled())
				continue;

			Vector3 a = light->getAmbientColor();
			Vector3 d = light->getDiffuseColor();
			Vector3 s = light->getSpecularColor();

			glUniform4f(renderProgramLocations->lightSources[i].ambient, a.getX(), a.getY(), a.getZ(), 1.0f);
			glUniform4f(renderProgramLocations->lightSources[i].diffuse, d.getX(), d.getY(), d.getZ(), 1.0f);
			glUniform4f(renderProgramLocations->lightSources[i].specular, s.getX(), s.getY(), s.getZ(), 1.0f);

			if (light->getLightType() == LightSource::DirectionalLight) {
				DirectionalLightSource* dlight = (DirectionalLightSource*) light;

				Vector3 dir = dlight->getDirection();
				Vector4 dir4 = vRotationMatrix * Vector4(dir);
				dir = Vector3(dir4.getX(), dir4.getY(), dir4.getZ());
				dir.normalize();

				glUniform3f(renderProgramLocations->lightSources[i].direction, dir.getX(), dir.getY(), dir.getZ());
				glUniform1f(renderProgramLocations->lightSources[i].shininess, dlight->getShininess());

				glUniform1f(renderProgramLocations->lightSources[i].constAttenuation, dlight->getConstantAttenuation());
				glUniform1f(renderProgramLocations->lightSources[i].linearAttenuation, dlight->getLinearAttenuation());
				glUniform1f(renderProgramLocations->lightSources[i].quadAttenuation, dlight->getQuadraticAttenuation());

				glUniform1f(renderProgramLocations->lightSources[i].cutoffAngleCos, -1.0f);
			} else if (light->getLightType() == LightSource::PointLight) {
				PointLightSource* plight = (PointLightSource*) light;

				Vector3 pos = plight->getPosition();
				Vector4 pos4 = vMatrix * Vector4(pos);
				pos = Vector3(pos4.getX(), pos4.getY(), pos4.getZ());

				glUniform3f(renderProgramLocations->lightSources[i].position, pos.getX(), pos.getY(), pos.getZ());
				glUniform1f(renderProgramLocations->lightSources[i].shininess, plight->getShininess());

				glUniform1f(renderProgramLocations->lightSources[i].constAttenuation, plight->getConstantAttenuation());
				glUniform1f(renderProgramLocations->lightSources[i].linearAttenuation, plight->getLinearAttenuation());
				glUniform1f(renderProgramLocations->lightSources[i].quadAttenuation, plight->getQuadraticAttenuation());

				glUniform1f(renderProgramLocations->lightSources[i].cutoffAngleCos, -2.0f);
			} else if (light->getLightType() == LightSource::SpotLight) {
				SpotLightSource* slight = (SpotLightSource*) light;

				Vector3 pos = slight->getPosition();
				Vector4 pos4 = vMatrix * Vector4(pos);
				pos = Vector3(pos4.getX(), pos4.getY(), pos4.getZ());

				Vector3 dir = slight->getDirection();
				Vector4 dir4 = vRotationMatrix * Vector4(dir);
				dir = Vector3(dir4.getX(), dir4.getY(), dir4.getZ());
				dir.normalize();

				glUniform3f(renderProgramLocations->lightSources[i].position, pos.getX(), pos.getY(), pos.getZ());
				glUniform3f(renderProgramLocations->lightSources[i].direction, dir.getX(), dir.getY(), dir.getZ());
				glUniform1f(renderProgramLocations->lightSources[i].shininess, slight->getShininess());
				glUniform1f(renderProgramLocations->lightSources[i].cutoffAngleCos, slight->getCutoffAngleCosine());
				glUniform1f(renderProgramLocations->lightSources[i].spotlightExponent, slight->getSpotlightExponent());

				glUniform1f(renderProgramLocations->lightSources[i].constAttenuation, slight->getConstantAttenuation());
				glUniform1f(renderProgramLocations->lightSources[i].linearAttenuation, slight->getLinearAttenuation());
				glUniform1f(renderProgramLocations->lightSources[i].quadAttenuation, slight->getQuadraticAttenuation());
			}

			i++;
		}

		if (i <= 9) {
			glUniform1f(renderProgramLocations->lightSources[i].cutoffAngleCos, -3.0f);
		}
	} else {
		glUniform1i(renderProgramLocations->dynamicLightingEnabledUniform, 0);
	}
}


void DefaultRenderer::dpSetupShaderUniforms(ShaderProgram* program)
{
	int viewW = Engine::getInstance()->getViewportWidth();
	int viewH = Engine::getInstance()->getViewportHeight();

	GLint peelDepthTexUniform = program->getUniformLocation("PeelDepthTex");
	GLint opaqueDepthTexUniform = program->getUniformLocation("OpaqueDepthTex");
	GLint texDimensionsUniform = program->getUniformLocation("TexDimensions");

	glUniform1i(peelDepthTexUniform, 1);
	glUniform1i(opaqueDepthTexUniform, 2);

	// For OpenGL prior to 3.1 we use simple 2D textures instead of RECTANGLE ones, so we need the
	// dimensions of the depth buffer texture in our fragment shader.
	if (!gtaglIsVersionSupported(3, 1))
		glUniform2i(texDimensionsUniform, viewW, viewH);
}


void DefaultRenderer::render()
{
	Engine* engine = Engine::getInstance();

	Camera* cam = engine->getCamera();

	Vector3 cpos = cam->getPosition();

	pMatrix = engine->getProjectionMatrix();
	vMatrix = Matrix4::lookAt(cam->getTarget(), cam->getUp())
			* Matrix4::translation(-cpos);
	vpMatrix = pMatrix * vMatrix;

	vRotationMatrix = vMatrix;
	vRotationMatrix.invert();
	vRotationMatrix.transpose();





	// **********************************************
	// *											*
	// *				RENDER SCENE				*
	// *											*
	// **********************************************

	glEnable(GL_DEPTH_TEST);
	//glDisable(GL_DEPTH_TEST);


	// ********** Render opaque objects **********

	// Ambient lighting pass

	gtaglBindFramebuffer(GTAGL_FRAMEBUFFER, fbos.dpOpaqueFBO);
	//gtaglBindFramebuffer(GTAGL_FRAMEBUFFER, 0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	globalAmbientLightEnabled = true;
	enabledLightBegin = lightSources.end();
	enabledLightEnd = lightSources.end();


	programs.opaqueStaticProgram->makeCurrent();
	initializeUniforms(programs.opaqueStaticProgram);

	glEnableVertexAttribArray(renderProgramLocations->vertexAttrib);

	glUniform3f(renderProgramLocations->camPosUniform, cpos.getX(), cpos.getY(), cpos.getZ());

	setupDynamicLighting(true);

	renderStaticMeshes(opaqueStaticMeshes.begin(), opaqueStaticMeshes.end());


#ifndef GTA_CPU_ANIMATION
	programs.opaqueAnimProgram->makeCurrent();
	initializeUniforms(programs.opaqueAnimProgram);

	glEnableVertexAttribArray(renderProgramLocations->vertexAttrib);

	glUniform3f(renderProgramLocations->camPosUniform, cpos.getX(), cpos.getY(), cpos.getZ());

	setupDynamicLighting(true);
#endif

	renderAnimatedMeshes(opaqueAnimMeshes.begin(), opaqueAnimMeshes.end());

	GLException::checkError("Blablabla 5");


	// Dynamic lighting passes

	glEnable(GL_BLEND);
	glBlendFuncSeparate(GL_ONE, GL_ONE, GL_ZERO, GL_ONE);

	glEnable(GL_STENCIL_TEST);
	glDepthMask(GL_FALSE);

	for (LightSourceList::iterator lit = lightSources.begin() ; lit != lightSources.end() ; lit++) {
		// Render the shadow volumes of light i
		LightSource* light = *lit;

		if (light->getLightType() == LightSource::AmbientLight  ||  !light->isEnabled())
			continue;

		if (light->isShadowCastingEnabled())
			renderShadowVolumes(light);
		else
			glDisable(GL_STENCIL_TEST);

		globalAmbientLightEnabled = false;
		enabledLightBegin = lit;
		enabledLightEnd = lit;
		enabledLightEnd++;

		programs.opaqueStaticProgram->makeCurrent();

		initializeUniforms(programs.opaqueStaticProgram);

		glEnableVertexAttribArray(renderProgramLocations->vertexAttrib);

		glUniform3f(renderProgramLocations->camPosUniform, cpos.getX(), cpos.getY(), cpos.getZ());

		glUniform4f(renderProgramLocations->materialColorUniform, 0.0f, 0.0f, 0.0f, 0.0f);

		setupDynamicLighting(true);

		renderStaticMeshes(opaqueStaticMeshes.begin(), opaqueStaticMeshes.end());

#ifndef GTA_CPU_ANIMATION
		programs.opaqueAnimProgram->makeCurrent();
		initializeUniforms(programs.opaqueAnimProgram);

		glEnableVertexAttribArray(renderProgramLocations->vertexAttrib);

		glUniform3f(renderProgramLocations->camPosUniform, cpos.getX(), cpos.getY(), cpos.getZ());

		glUniform4f(renderProgramLocations->materialColorUniform, 0.0f, 0.0f, 0.0f, 0.0f);

		setupDynamicLighting(true);
#endif

		renderAnimatedMeshes(opaqueAnimMeshes.begin(), opaqueAnimMeshes.end());

		if (!light->isShadowCastingEnabled())
			glEnable(GL_STENCIL_TEST);
	}

	glDisable(GL_STENCIL_TEST);



	// ********** Render transparent objects **********

	glDepthMask(GL_TRUE);

	// Clear input ping-pong depth buffer to 0 (so that no fragment will be initially peeled away)
	gtaglBindFramebuffer(GTAGL_FRAMEBUFFER, fbos.dpPingPongFBOs[1]);

#ifdef GTA_USE_OPENGL_ES
	glClearDepthf(0.0f);
#else
	glClearDepth(0.0);
#endif

	glClear(GL_DEPTH_BUFFER_BIT);

#ifdef GTA_USE_OPENGL_ES
	glClearDepthf(1.0f);
#else
	glClearDepth(1.0);
#endif

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

#ifdef GTA_USE_OPENGL_ES
	GLenum dpTexTarget = GL_TEXTURE_2D;
#else
	GLenum dpTexTarget = gtaglIsVersionSupported(3, 1) ? GL_TEXTURE_RECTANGLE : GL_TEXTURE_2D;
#endif


	// Do the depth-peeling passes
	for (int currentPass = 0 ; currentPass < dpNumPasses ; currentPass++) {
		int inputIdx = (currentPass+1) % 2;
		int outputIdx = currentPass % 2;

		glDisable(GL_BLEND);

		// ********** Peel a single layer **********
		gtaglBindFramebuffer(GTAGL_FRAMEBUFFER, fbos.dpPingPongFBOs[outputIdx]);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Bind the input depth texture to texture unit 1 (unit 0 is used by the render* methods)
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(dpTexTarget, fbos.dpPingPongDepthStencilTexes[inputIdx]);

		// Bind the opaque depth texture to texture unit 2
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(dpTexTarget, fbos.dpOpaqueTexes[1]);

		// Back to texture unit 0 (the render* methods rely on it to be active)
		glActiveTexture(GL_TEXTURE0);



		// Ambient lighting pass

		globalAmbientLightEnabled = true;
		enabledLightBegin = lightSources.end();
		enabledLightEnd = lightSources.end();

		programs.transStaticProgram->makeCurrent();
		dpSetupShaderUniforms(programs.transStaticProgram);

		glEnableVertexAttribArray(renderProgramLocations->vertexAttrib);

		initializeUniforms(programs.transStaticProgram);

		glUniform3f(renderProgramLocations->camPosUniform, cpos.getX(), cpos.getY(), cpos.getZ());

		setupDynamicLighting(true);

		renderStaticMeshes(transStaticMeshes.begin(), transStaticMeshes.end());


		programs.transAnimProgram->makeCurrent();
		dpSetupShaderUniforms(programs.transAnimProgram);

		glEnableVertexAttribArray(renderProgramLocations->vertexAttrib);

		initializeUniforms(programs.transAnimProgram);

		glUniform3f(renderProgramLocations->camPosUniform, cpos.getX(), cpos.getY(), cpos.getZ());

		setupDynamicLighting(true);

		renderAnimatedMeshes(transAnimMeshes.begin(), transAnimMeshes.end());

		//glFrameTerminatorGREMEDY();

		// Dynamic lighting passes

		glEnable(GL_STENCIL_TEST);
		glEnable(GL_BLEND);
		glBlendFuncSeparate(GL_ONE, GL_ONE, GL_ZERO, GL_ONE);
		glDepthMask(GL_FALSE);

		for (LightSourceList::iterator lit = lightSources.begin() ; lit != lightSources.end() ; lit++) {
			// Render shadow volumes of light i
			LightSource* light = *lit;

			if (light->getLightType() == LightSource::AmbientLight  ||  !light->isEnabled())
				continue;

			if (light->isShadowCastingEnabled())
				renderShadowVolumes(light);
			else
				glDisable(GL_STENCIL_TEST);

			globalAmbientLightEnabled = false;
			enabledLightBegin = lit;
			enabledLightEnd = lit;
			enabledLightEnd++;

			programs.transStaticProgram->makeCurrent();
			dpSetupShaderUniforms(programs.transStaticProgram);

			initializeUniforms(programs.transStaticProgram);

			glUniform3f(renderProgramLocations->camPosUniform, cpos.getX(), cpos.getY(), cpos.getZ());

			glUniform4f(renderProgramLocations->materialColorUniform, 0.0f, 0.0f, 0.0f, 0.0f);

			setupDynamicLighting(true);

			renderStaticMeshes(transStaticMeshes.begin(), transStaticMeshes.end());


			programs.transAnimProgram->makeCurrent();
			dpSetupShaderUniforms(programs.transAnimProgram);

			initializeUniforms(programs.transAnimProgram);

			glUniform3f(renderProgramLocations->camPosUniform, cpos.getX(), cpos.getY(), cpos.getZ());

			glUniform4f(renderProgramLocations->materialColorUniform, 0.0f, 0.0f, 0.0f, 0.0f);

			setupDynamicLighting(true);

			renderAnimatedMeshes(transAnimMeshes.begin(), transAnimMeshes.end());

			if (!light->isShadowCastingEnabled())
				glEnable(GL_STENCIL_TEST);
		}

		glDisable(GL_STENCIL_TEST);
		glDepthMask(GL_TRUE);



		// Our front-to-back blending equation. The source alpha is premultiplied in the fragment shader
		glBlendFuncSeparate(GL_DST_ALPHA, GL_ONE, GL_ZERO, GL_ONE_MINUS_SRC_ALPHA);

		programs.dpBlendLayerProgram->makeCurrent();

		glDisable(GL_DEPTH_TEST);

		gtaglBindFramebuffer(GTAGL_FRAMEBUFFER, 0);

		// Render a screen-size plane with the contents of the output color buffer
		glBindBuffer(GL_ARRAY_BUFFER, planeDataBuf);
		glEnableVertexAttribArray(dpShaderLocations.dpBlendLayerVertexAttrib);
		glVertexAttribPointer(dpShaderLocations.dpBlendLayerVertexAttrib, 2, GL_FLOAT, GL_FALSE, 0, (void*) 0);

		glBindTexture(dpTexTarget, fbos.dpPingPongColorTexes[outputIdx]);
		glUniform1i(dpShaderLocations.dpBlendLayerTexUniform, 0);

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		glDisableVertexAttribArray(dpShaderLocations.dpBlendLayerVertexAttrib);

		glEnable(GL_DEPTH_TEST);
	}


	// ********** Blend opaque and transparent layers together **********

	glDisable(GL_DEPTH_TEST);

	programs.dpBlendFinalProgram->makeCurrent();

	// We still blend front-to-back here, so we'll still use the old blend functions
	glBlendFuncSeparate(GL_DST_ALPHA, GL_ONE, GL_ZERO, GL_ONE_MINUS_SRC_ALPHA);

	gtaglBindFramebuffer(GTAGL_FRAMEBUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER, planeDataBuf);
	glEnableVertexAttribArray(dpShaderLocations.dpBlendFinalVertexAttrib);
	glVertexAttribPointer(dpShaderLocations.dpBlendFinalVertexAttrib, 2, GL_FLOAT, GL_FALSE, 0, (void*) 0);

	glBindTexture(dpTexTarget, fbos.dpOpaqueTexes[0]);
	glUniform1i(dpShaderLocations.dpBlendFinalTexUniform, 0);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glDisableVertexAttribArray(dpShaderLocations.dpBlendFinalVertexAttrib);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glDisable(GL_BLEND);



	glDisableVertexAttribArray(renderProgramLocations->vertexAttrib);


	if (renderProgramLocations->normalAttrib != -1)
		glDisableVertexAttribArray(renderProgramLocations->normalAttrib);
	if (renderProgramLocations->colorAttrib != -1)
		glDisableVertexAttribArray(renderProgramLocations->colorAttrib);
	if (renderProgramLocations->texCoordAttrib != -1)
		glDisableVertexAttribArray(renderProgramLocations->texCoordAttrib);



	gtaglBindFramebuffer(GTAGL_FRAMEBUFFER, 0);


	for (StaticMeshIterator it = opaqueStaticMeshes.begin() ; it != opaqueStaticMeshes.end() ; it++) {
		delete *it;
	}
	for (StaticMeshIterator it = transStaticMeshes.begin() ; it != transStaticMeshes.end() ; it++) {
		delete *it;
	}
	for (AnimatedMeshIterator it = opaqueAnimMeshes.begin() ; it != opaqueAnimMeshes.end() ; it++) {
		delete *it;
	}
	for (AnimatedMeshIterator it = transAnimMeshes.begin() ; it != transAnimMeshes.end() ; it++) {
		delete *it;
	}


	opaqueStaticMeshes.clear();
	transStaticMeshes.clear();
	opaqueAnimMeshes.clear();
	transAnimMeshes.clear();

	lightSources.clear();
}


void DefaultRenderer::renderStaticMeshes(StaticMeshIterator beg, StaticMeshIterator end)
{
	glUniform4f(renderProgramLocations->matAmbientReflectionUniform, 1.0f, 1.0f, 1.0f, 1.0f);
	glUniform4f(renderProgramLocations->matDiffuseReflectionUniform, 1.0f, 1.0f, 1.0f, 1.0f);
	glUniform4f(renderProgramLocations->matSpecularReflectionUniform, 1.0f, 1.0f, 1.0f, 1.0f);

	GLuint curDataBuf = 0;
	int curNOffs = -1;
	int curTCOffs = -1;
	int curVCOffs = -1;

	for (StaticMeshIterator it = beg ; it != end ; it++) {
		StaticRenderingMesh* smesh = *it;


		int nOffs = smesh->getNormalOffset();
		int tcOffs = smesh->getTexCoordOffset();
		int vcOffs = smesh->getVertexColorOffset();

		GLuint dataBuf = smesh->getDataBuffer();

		bool sameBuf = true;

		if (curDataBuf == 0  ||  dataBuf != curDataBuf) {
			glBindBuffer(GL_ARRAY_BUFFER, dataBuf);
			glVertexAttribPointer(renderProgramLocations->vertexAttrib, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0);

			curDataBuf = dataBuf;
			sameBuf = false;
		}

		if (!sameBuf  ||  nOffs != curNOffs) {
			if (renderProgramLocations->normalAttrib != -1) {
				if (nOffs != -1) {
					glEnableVertexAttribArray(renderProgramLocations->normalAttrib);
					glVertexAttribPointer(renderProgramLocations->normalAttrib, 3, GL_FLOAT, GL_FALSE, 0, (void*) nOffs);
				} else {
					glDisableVertexAttribArray(renderProgramLocations->normalAttrib);
				}
			}

			curNOffs = nOffs;
		}

		if (!sameBuf  ||  tcOffs != curTCOffs) {
			if (renderProgramLocations->texCoordAttrib != -1) {
				if (tcOffs != -1) {
					glEnableVertexAttribArray(renderProgramLocations->texCoordAttrib);
					glVertexAttribPointer(renderProgramLocations->texCoordAttrib, 2, GL_FLOAT, GL_FALSE, 0, (void*) tcOffs);
				} else {
					glDisableVertexAttribArray(renderProgramLocations->texCoordAttrib);
				}
			}

			curTCOffs = tcOffs;
		}

		if (!sameBuf  ||  vcOffs != curVCOffs) {
			if (renderProgramLocations->colorAttrib != -1) {
				if (vcOffs != -1) {
					glEnableVertexAttribArray(renderProgramLocations->colorAttrib);
					glVertexAttribPointer(renderProgramLocations->colorAttrib, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, (void*) vcOffs);
				} else {
					glDisableVertexAttribArray(renderProgramLocations->colorAttrib);
				}
			}

			curVCOffs = vcOffs;
		}


		const Matrix4& mm = smesh->getModelMatrix();

		Matrix4 mvMatrix = vMatrix * mm;
		Matrix4 mvpMatrix = vpMatrix * mm;

		glUniformMatrix4fv(renderProgramLocations->mvMatrixUniform, 1, GL_FALSE, (float*) &mvMatrix);
		glUniformMatrix4fv(renderProgramLocations->mvpMatrixUniform, 1, GL_FALSE, (float*) &mvpMatrix);


		glUniform1i(renderProgramLocations->vertexColorsUniform, (vcOffs != -1) ? 1 : 0);


		GLuint tex = smesh->getTexture();

		//glActiveTexture(GL_TEXTURE0);

		if (tex != 0) {
			glBindTexture(GL_TEXTURE_2D, tex);
			glUniform1i(renderProgramLocations->texturedUniform, 1);
			glUniform1i(renderProgramLocations->textureUniform, 0);
		} else {
			glUniform1i(renderProgramLocations->texturedUniform, 0);
		}

		uint8_t r, g, b, a;
		smesh->getMaterialColor(r, g, b, a);

		if (globalAmbientLightEnabled) {
			glUniform4f(renderProgramLocations->materialColorUniform,
					r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
		}


		GLenum polyMode;

		switch (smesh->getPrimitiveFormat()) {
		case RenderingPrimitiveTriangles:
			polyMode = GL_TRIANGLES;
			break;
		case RenderingPrimitiveTriangleStrip:
			polyMode = GL_TRIANGLE_STRIP;
			break;
		case RenderingPrimitiveLines:
			polyMode = GL_LINES;
			break;
		case RenderingPrimitivePoints:
			polyMode = GL_POINTS;
			break;
		}


		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, smesh->getIndexBuffer());

		glDrawRangeElements(polyMode, 0, smesh->getVertexCount()-1, smesh->getIndexCount(), GL_UNSIGNED_INT, (void*) 0);
	}
}


void DefaultRenderer::renderAnimatedMeshes(AnimatedMeshIterator beg, AnimatedMeshIterator end)
{
	for (AnimatedMeshIterator it = beg ; it != end ; it++) {
		AnimatedRenderingMesh* amesh = *it;


		glUniform4f(renderProgramLocations->matAmbientReflectionUniform, 1.0f, 1.0f, 1.0f, 1.0f);
		glUniform4f(renderProgramLocations->matDiffuseReflectionUniform, 1.0f, 1.0f, 1.0f, 1.0f);
		glUniform4f(renderProgramLocations->matSpecularReflectionUniform, 1.0f, 1.0f, 1.0f, 1.0f);


		glBindBuffer(GL_ARRAY_BUFFER, amesh->getDataBuffer());

		glVertexAttribPointer(renderProgramLocations->vertexAttrib, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0);


		int nOffs = amesh->getNormalOffset();
		int tcOffs = amesh->getTexCoordOffset();
		int vcOffs = amesh->getVertexColorOffset();
		int biOffs = amesh->getBoneIndexOffset();
		int bwOffs = amesh->getBoneWeightOffset();

		if (renderProgramLocations->normalAttrib != -1) {
			if (nOffs != -1) {
				glEnableVertexAttribArray(renderProgramLocations->normalAttrib);
				glVertexAttribPointer(renderProgramLocations->normalAttrib, 3, GL_FLOAT, GL_FALSE, 0, (void*) nOffs);
			} else {
				glDisableVertexAttribArray(renderProgramLocations->normalAttrib);
			}
		}

		if (renderProgramLocations->texCoordAttrib != -1) {
			if (tcOffs != -1) {
				glEnableVertexAttribArray(renderProgramLocations->texCoordAttrib);
				glVertexAttribPointer(renderProgramLocations->texCoordAttrib, 2, GL_FLOAT, GL_FALSE, 0, (void*) tcOffs);
			} else {
				glDisableVertexAttribArray(renderProgramLocations->texCoordAttrib);
			}
		}

		if (renderProgramLocations->colorAttrib != -1) {
			if (vcOffs != -1) {
				glEnableVertexAttribArray(renderProgramLocations->colorAttrib);
				glVertexAttribPointer(renderProgramLocations->colorAttrib, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, (void*) vcOffs);
			} else {
				glDisableVertexAttribArray(renderProgramLocations->colorAttrib);
			}
		}

		if (renderProgramLocations->boneIndexAttrib != -1) {
			if (biOffs != -1) {
				glEnableVertexAttribArray(renderProgramLocations->boneIndexAttrib);
				glVertexAttribPointer(renderProgramLocations->boneIndexAttrib, 4, GL_UNSIGNED_BYTE, GL_FALSE, 0, (void*) biOffs);
			} else {
				glDisableVertexAttribArray(renderProgramLocations->boneIndexAttrib);
			}
		}

		if (renderProgramLocations->boneWeightAttrib != -1) {
			if (bwOffs != -1) {
				glEnableVertexAttribArray(renderProgramLocations->boneWeightAttrib);
				glVertexAttribPointer(renderProgramLocations->boneWeightAttrib, 4, GL_FLOAT, GL_FALSE, 0, (void*) bwOffs);
			} else {
				glDisableVertexAttribArray(renderProgramLocations->boneWeightAttrib);
			}
		}


		int16_t boneIdx = amesh->getBoneIndex();

		if (boneIdx == -1) {
			// Skinned mesh
			glUniform1i(renderProgramLocations->boneUniform, -1);
		} else {
			// Unskinned mesh
			glUniform1i(renderProgramLocations->boneUniform, boneIdx);
		}

		glUniformMatrix4fv(renderProgramLocations->boneMatUniform, amesh->getBoneCount(), GL_FALSE, (float*) amesh->getBoneMatrices());


		Matrix4 mvMatrix = vMatrix * amesh->getModelMatrix();
		Matrix4 mvpMatrix = vpMatrix * amesh->getModelMatrix();

		const float* matData = mvMatrix.toArray();
		glUniformMatrix4fv(renderProgramLocations->mvMatrixUniform, 1, GL_FALSE, matData);

		matData = mvpMatrix.toArray();
		glUniformMatrix4fv(renderProgramLocations->mvpMatrixUniform, 1, GL_FALSE, matData);


		glUniform1i(renderProgramLocations->vertexColorsUniform, (vcOffs != -1) ? 1 : 0);


		GLuint tex = amesh->getTexture();

		glActiveTexture(GL_TEXTURE0);

		if (tex != 0) {
			glBindTexture(GL_TEXTURE_2D, tex);
			glUniform1i(renderProgramLocations->texturedUniform, 1);
			glUniform1i(renderProgramLocations->textureUniform, 0);
		} else {
			glUniform1i(renderProgramLocations->texturedUniform, 0);
		}

		uint8_t r, g, b, a;
		amesh->getMaterialColor(r, g, b, a);

		if (globalAmbientLightEnabled) {
			glUniform4f(renderProgramLocations->materialColorUniform,
					r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
		} else {
			//glUniform4f(renderProgramLocations->materialColorUniform, 0.0f, 0.0f, 0.0f, 0.0f);
		}


		GLenum polyMode;

		switch (amesh->getPrimitiveFormat()) {
		case RenderingPrimitivePoints:
			polyMode = GL_POINTS;
			break;
		case RenderingPrimitiveLines:
			polyMode = GL_LINES;
			break;
		case RenderingPrimitiveTriangles:
			polyMode = GL_TRIANGLES;
			break;
		case RenderingPrimitiveTriangleStrip:
			polyMode = GL_TRIANGLE_STRIP;
			break;
		}


		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, amesh->getIndexBuffer());

		GLException::checkError("Before drawing (NR)");

		glDrawRangeElements(polyMode, 0, amesh->getVertexCount()-1, amesh->getIndexCount(), GL_UNSIGNED_INT, (void*) 0);
	}
}

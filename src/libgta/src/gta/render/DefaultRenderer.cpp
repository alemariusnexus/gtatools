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

#include "DefaultRenderer.h"

#include "TestShaderPlugin.h"

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



//#define TIME_RENDERING

#define NUM_SUBMESHES 16





DefaultRenderer::DefaultRenderer()
		: globalAmbientLightEnabled(true), numObjects(0), currentMatrixAllocSize(5000),
		  mvMatrices(new Matrix4[currentMatrixAllocSize]), mvpMatrices(new Matrix4[currentMatrixAllocSize]),
		  normalMatrices(new Matrix4[currentMatrixAllocSize]), dpNumPasses(6)
{
	programCache = new AdvancedShaderProgramCache(new AdvancedShaderProgramCacheLoader(this), 50);

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


	shaders.dpPeelLayerVertexShader = new AdvancedShader(GL_VERTEX_SHADER,
			"Depth Peeling Transparency Algorithm Peel Layer Vertex Shader");
	shaders.dpPeelLayerVertexShader->setSourceCode(CString(dpPeelLayerVertexShaderData,
			dpPeelLayerVertexShaderDataLen));

	shaders.dpPeelLayerFragmentShader = new AdvancedShader(GL_FRAGMENT_SHADER,
			"Depth Peeling Transparency Algorithm Peel Layer Fragment Shader");
	shaders.dpPeelLayerFragmentShader->setSourceCode(CString(dpPeelLayerFragmentShaderData,
			dpPeelLayerFragmentShaderDataLen));

	shaders.dpBlendLayerVertexShader = new AdvancedShader(GL_VERTEX_SHADER,
			"Depth Peeling Transparency Algorithm Blend Layer Vertex Shader");
	shaders.dpBlendLayerVertexShader->setSourceCode(CString(dpBlendLayerVertexShaderData,
			dpBlendLayerVertexShaderDataLen));

	shaders.dpBlendLayerFragmentShader = new AdvancedShader(GL_FRAGMENT_SHADER,
			"Depth Peeling Transparency Algorithm Blend Layer Fragment Shader");
	shaders.dpBlendLayerFragmentShader->setSourceCode(CString(dpBlendLayerFragmentShaderData,
			dpBlendLayerFragmentShaderDataLen));

	shaders.dpBlendFinalVertexShader = new AdvancedShader(GL_VERTEX_SHADER,
			"Depth Peeling Transparency Algorithm Blend Final Vertex Shader");
	shaders.dpBlendFinalVertexShader->setSourceCode(CString(dpBlendFinalVertexShaderData,
			dpBlendFinalVertexShaderDataLen));

	shaders.dpBlendFinalFragmentShader = new AdvancedShader(GL_FRAGMENT_SHADER,
			"Depth Peeling Transparency Algorithm Blend Final Fragment Shader");
	shaders.dpBlendFinalFragmentShader->setSourceCode(CString(dpBlendFinalFragmentShaderData,
			dpBlendFinalFragmentShaderDataLen));


	shaders.shadeVertexShader = new AdvancedShader(GL_VERTEX_SHADER, "Static Vertex Shader");
	shaders.shadeVertexShader->setSourceCode(CString(shadeVertexShaderData, shadeVertexShaderDataLen));

	shaders.shadeFragmentShader = new AdvancedShader(GL_FRAGMENT_SHADER, "Static Fragment Shader");
	shaders.shadeFragmentShader->setSourceCode(CString(shadeFragmentShaderData, shadeFragmentShaderDataLen));

	shaders.vertexDefaultShader = new AdvancedShader(GL_VERTEX_SHADER, "Default Opaque Vertex Shader");
	shaders.vertexDefaultShader->setSourceCode(CString(vertexDefaultData, vertexDefaultDataLen));

	shaders.fragmentDefaultShader = new AdvancedShader(GL_FRAGMENT_SHADER, "Default Opaque Fragment Shader");
	shaders.fragmentDefaultShader->setSourceCode(CString(fragmentDefaultData, fragmentDefaultDataLen));

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

	shaders.animShadeVertexShader = new AdvancedShader(GL_VERTEX_SHADER, "Animated Vertex Shader");
	shaders.animShadeVertexShader->setSourceCode(animShadeVertexShaderCode);

	shaders.animShadeFragmentShader = new AdvancedShader(GL_FRAGMENT_SHADER, "Animated Fragment Shader");
	shaders.animShadeFragmentShader->setSourceCode(CString(animShadeFragmentShaderData, animShadeFragmentShaderDataLen));

	shaders.lightingVertexShader = new AdvancedShader(GL_VERTEX_SHADER, "Lighting Vertex Shader");
	shaders.lightingVertexShader->setSourceCode(CString(lightingVertexShaderData, lightingVertexShaderDataLen));


	programs.dpBlendLayerProgram = NULL;
	programs.dpBlendFinalProgram = NULL;
}


void DefaultRenderer::bindFixedShaderProgramLocations(AdvancedShaderProgram* program)
{
	program->bindAttributeLocation(VertexAttributeLocationVertex, "Vertex");
	program->bindAttributeLocation(VertexAttributeLocationNormal, "Normal");
	program->bindAttributeLocation(VertexAttributeLocationVertexColor, "Color");
	program->bindAttributeLocation(VertexAttributeLocationTexCoord, "TexCoord");
	program->bindAttributeLocation(VertexAttributeLocationBoneWeight, "BoneWeights");
	program->bindAttributeLocation(VertexAttributeLocationBoneIndex, "BoneIndices");
	program->bindAttributeLocation(VertexAttributeLocationSubmeshIndex, "SubmeshIndex");
}


DefaultRenderer::AdvancedShaderProgramCacheEntry* DefaultRenderer::buildShaderPrograms(const ShaderPluginRegistry& pluginReg)
{
	list<ShaderPlugin*> plugins;
	pluginReg.getPlugins(plugins);

	AdvancedShaderProgramCacheEntry* entry = new AdvancedShaderProgramCacheEntry(this);


	AdvancedShaderProgram* opaqueStaticProgram = new AdvancedShaderProgram("Opaque Static Shader Program");
	opaqueStaticProgram->attachShader(shaders.shadeVertexShader);
	opaqueStaticProgram->attachShader(shaders.shadeFragmentShader);
	opaqueStaticProgram->attachShader(shaders.vertexDefaultShader);
	opaqueStaticProgram->attachShader(shaders.fragmentDefaultShader);
	opaqueStaticProgram->attachShader(shaders.lightingVertexShader);
	opaqueStaticProgram->addShaderPlugins(plugins.begin(), plugins.end());
	bindFixedShaderProgramLocations(opaqueStaticProgram);
	opaqueStaticProgram->build();
	entry->programs[OpaqueStaticShaderProgram] = opaqueStaticProgram;

	AdvancedShaderProgram* transStaticProgram = new AdvancedShaderProgram("Transparent Static Shader Program");
	transStaticProgram->attachShader(shaders.shadeVertexShader);
	transStaticProgram->attachShader(shaders.shadeFragmentShader);
	transStaticProgram->attachShader(shaders.lightingVertexShader);
	transStaticProgram->attachShader(shaders.dpPeelLayerVertexShader);
	transStaticProgram->attachShader(shaders.dpPeelLayerFragmentShader);
	transStaticProgram->addShaderPlugins(plugins.begin(), plugins.end());
	bindFixedShaderProgramLocations(transStaticProgram);
	transStaticProgram->build();
	entry->programs[TransparentStaticShaderProgram] = transStaticProgram;


	AdvancedShaderProgram* opaqueAnimProgram = new AdvancedShaderProgram("Opaque Animated Shader Program");
	opaqueAnimProgram->attachShader(shaders.animShadeVertexShader);
	opaqueAnimProgram->attachShader(shaders.animShadeFragmentShader);
	opaqueAnimProgram->attachShader(shaders.vertexDefaultShader);
	opaqueAnimProgram->attachShader(shaders.fragmentDefaultShader);
	opaqueAnimProgram->attachShader(shaders.lightingVertexShader);
	opaqueAnimProgram->addShaderPlugins(plugins.begin(), plugins.end());
	bindFixedShaderProgramLocations(opaqueAnimProgram);
	opaqueAnimProgram->build();
	entry->programs[OpaqueAnimatedShaderProgram] = opaqueAnimProgram;

	AdvancedShaderProgram* transAnimProgram = new AdvancedShaderProgram("Transparent Animated Shader Program");
	transAnimProgram->attachShader(shaders.animShadeVertexShader);
	transAnimProgram->attachShader(shaders.animShadeFragmentShader);
	transAnimProgram->attachShader(shaders.lightingVertexShader);
	transAnimProgram->attachShader(shaders.dpPeelLayerVertexShader);
	transAnimProgram->attachShader(shaders.dpPeelLayerFragmentShader);
	transAnimProgram->addShaderPlugins(plugins.begin(), plugins.end());
	bindFixedShaderProgramLocations(transAnimProgram);
	transAnimProgram->build();
	entry->programs[TransparentAnimatedShaderProgram] = transAnimProgram;

	return entry;


	/*dpShaderLocations.dpBlendLayerTexUniform = programs.dpBlendLayerProgram->getUniformLocation("LayerTex");
	dpShaderLocations.dpBlendLayerVertexAttrib = programs.dpBlendLayerProgram->getAttributeLocation("Vertex");

	dpShaderLocations.dpBlendFinalTexUniform = programs.dpBlendFinalProgram->getUniformLocation("CombinedLayerTex");
	dpShaderLocations.dpBlendFinalVertexAttrib = programs.dpBlendFinalProgram->getAttributeLocation("Vertex");*/
}


void DefaultRenderer::updateMiscShaderPrograms()
{
	if (!programs.dpBlendLayerProgram  ||  dpBlendLayerPluginRegistry != dpBlendLayerLinkedPluginRegistry) {
		list<ShaderPlugin*> plugins;
		dpBlendLayerPluginRegistry.getPlugins(plugins);

		if (programs.dpBlendLayerProgram)
			delete programs.dpBlendLayerProgram;

		programs.dpBlendLayerProgram = new AdvancedShaderProgram (
				"Depth Peeling Transparency Algorithm Blend Layer Shader Program");
		programs.dpBlendLayerProgram->attachShader(shaders.dpBlendLayerVertexShader);
		programs.dpBlendLayerProgram->attachShader(shaders.dpBlendLayerFragmentShader);
		programs.dpBlendLayerProgram->addShaderPlugins(plugins.begin(), plugins.end());
		programs.dpBlendLayerProgram->build();

		dpShaderLocations.dpBlendLayerTexUniform = programs.dpBlendLayerProgram->getUniformLocation("LayerTex");
		dpShaderLocations.dpBlendLayerVertexAttrib = programs.dpBlendLayerProgram->getAttributeLocation("Vertex");

		dpBlendLayerLinkedPluginRegistry = dpBlendLayerPluginRegistry;
	}

	if (!programs.dpBlendFinalProgram  ||  dpBlendFinalPluginRegistry != dpBlendFinalLinkedPluginRegistry) {
		list<ShaderPlugin*> plugins;
		dpBlendFinalPluginRegistry.getPlugins(plugins);

		if (programs.dpBlendFinalProgram)
			delete programs.dpBlendFinalProgram;

		programs.dpBlendFinalProgram = new AdvancedShaderProgram (
				"Depth Peeling Transparency Algorithm Blend Final Shader Program");
		programs.dpBlendFinalProgram->attachShader(shaders.dpBlendFinalVertexShader);
		programs.dpBlendFinalProgram->attachShader(shaders.dpBlendFinalFragmentShader);
		programs.dpBlendFinalProgram->addShaderPlugins(plugins.begin(), plugins.end());
		programs.dpBlendFinalProgram->build();

		dpShaderLocations.dpBlendFinalTexUniform = programs.dpBlendFinalProgram->getUniformLocation("CombinedLayerTex");
		dpShaderLocations.dpBlendFinalVertexAttrib = programs.dpBlendFinalProgram->getAttributeLocation("Vertex");

		dpBlendFinalLinkedPluginRegistry = dpBlendFinalPluginRegistry;
	}
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

	glGenTextures(1, &dummyTex);
	glBindTexture(GL_TEXTURE_2D, dummyTex);
	uint8_t dummyTexData[] = {
			255, 255, 255, 255
	};
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, dummyTexData);
}


void DefaultRenderer::enqueueForRendering(list<RenderingEntity*>::iterator beg, list<RenderingEntity*>::iterator end)
{
	for (list<RenderingEntity*>::iterator it = beg ; it != end ; it++) {
		RenderingEntity* entity = *it;

		renderingEntityDisposalList.push_back(entity);

		RenderingMesh* mesh = dynamic_cast<RenderingMesh*>(entity);
		uint32_t flags = mesh->getFlags();

		AdvancedShaderProgramCache::Pointer ptr = programCache->getEntryPointer(mesh->getPluginRegistry());

		if ((flags & RenderingMesh::IsAnimated)  !=  0) {
			AnimatedRenderingMeshEntry entry;
			entry.mesh = mesh;
			entry.id = numObjects;

			if ((flags & RenderingMesh::HasTransparency)  !=  0) {
				transAnimMeshes[ptr].push_back(entry);
			} else {
				opaqueAnimMeshes[ptr].push_back(entry);
			}
		} else {
			StaticRenderingMeshEntry entry;
			entry.mesh = mesh;
			entry.id = numObjects;

			if ((flags & RenderingMesh::HasTransparency)  !=  0) {
				transStaticMeshes[ptr].push_back(entry);
			} else {
				opaqueStaticMeshes[ptr].push_back(entry);
			}
		}

		numObjects++;
	}
}


void DefaultRenderer::enqueueForRendering(list<LightSource*>::iterator beg, list<LightSource*>::iterator end)
{
	for (list<LightSource*>::iterator it = beg ; it != end ; it++) {
		lightSources.push_back(*it);
	}
}


void DefaultRenderer::initializeUniforms(AdvancedShaderProgram* program)
{
	map<AdvancedShaderProgram*, ShaderProgramLocations>::iterator it = spLocs.find(program);

	ShaderProgramLocations locs;

	if (it == spLocs.end()) {
		ShaderProgramLocations loc;

		loc.matrixUBIndex = program->getUniformBlockIndex("Matrices");

		loc.camPosUniform = program->getUniformLocation("CameraPosition");
		loc.mMatrixUniform = program->getUniformLocation("MMatrix");
		loc.vMatrixUniform = program->getUniformLocation("VMatrix");
		loc.vpMatrixUniform = program->getUniformLocation("VPMatrix");
		loc.mvMatrixUniform = program->getUniformLocation("MVMatrix");
		loc.mvpMatrixUniform = program->getUniformLocation("MVPMatrix");
		loc.normalMvMatrixUniform = program->getUniformLocation("NormalMVMatrix");
		loc.texturedUniform = program->getUniformLocation("IsTextured");
		loc.materialColorUniform = program->getUniformLocation("MaterialColor");
		loc.vertexColorsUniform = program->getUniformLocation("HasVertexColors");
		loc.textureUniform = program->getUniformLocation("Texture");

		loc.matAmbientReflectionUniform = program->getUniformLocation("MaterialAmbientReflection");
		loc.matDiffuseReflectionUniform = program->getUniformLocation("MaterialDiffuseReflection");
		loc.matSpecularReflectionUniform = program->getUniformLocation("MaterialSpecularReflection");
		loc.globalAmbientLightUniform = program->getUniformLocation("GlobalAmbientLight");
		loc.dynamicLightingEnabledUniform = program->getUniformLocation("DynamicLightingEnabled");

		loc.submeshOffsetUniform = program->getUniformLocation("SubmeshOffset");

		loc.boneMatUniform = program->getUniformLocation("BoneMatrices");
		loc.boneUniform = program->getUniformLocation("Bone");

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

		it = spLocs.insert(pair<AdvancedShaderProgram*, ShaderProgramLocations>(program, loc)).first;
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


void DefaultRenderer::dpSetupShaderUniforms(AdvancedShaderProgram* program)
{
	int viewW = Engine::getInstance()->getViewportWidth();
	int viewH = Engine::getInstance()->getViewportHeight();

	GLint peelDepthTexUniform = program->getUniformLocation("PeelDepthTex");
	GLint opaqueDepthTexUniform = program->getUniformLocation("OpaqueDepthTex");
	GLint texDimensionsUniform = program->getUniformLocation("TexDimensions");

	glUniform1i(peelDepthTexUniform, 0);
	glUniform1i(opaqueDepthTexUniform, 1);

	// For OpenGL prior to 3.1 we use simple 2D textures instead of RECTANGLE ones, so we need the
	// dimensions of the depth buffer texture in our fragment shader.
	if (!gtaglIsVersionSupported(3, 1))
		glUniform2i(texDimensionsUniform, viewW, viewH);
}


void DefaultRenderer::applyShaderPluginUniformBuffers(AdvancedShaderProgram* program)
{
	AdvancedShaderProgram::PluginIterator beg = program->getShaderPluginBegin();
	AdvancedShaderProgram::PluginIterator end = program->getShaderPluginEnd();

	for (AdvancedShaderProgram::PluginIterator it = beg ; it != end ; it++) {
		ShaderPlugin* plugin = *it;
		plugin->applyUniformBuffers(program);
	}
}


void DefaultRenderer::render()
{
#ifdef TIME_RENDERING
	uint64_t absS, absE, s, e;

	absS = GetTickcountMicroseconds();

	s = GetTickcountMicroseconds();
#endif

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


#ifdef TIME_RENDERING
	e = GetTickcountMicroseconds();
	uint64_t t1 = e-s;
	s = GetTickcountMicroseconds();
#endif

#if 1

	if (numObjects > currentMatrixAllocSize  ||  !mvMatrices) {
		currentMatrixAllocSize = numObjects + 100;

		mvMatrices = new Matrix4[currentMatrixAllocSize];
		mvpMatrices = new Matrix4[currentMatrixAllocSize];
		normalMatrices = new Matrix4[currentMatrixAllocSize];
	}

	for (StaticMeshIterator it = opaqueStaticMeshes.begin() ; it != opaqueStaticMeshes.end() ; it++) {
		for (list<StaticRenderingMeshEntry>::iterator iit = it->second.begin() ; iit != it->second.end() ; iit++) {
			StaticRenderingMeshEntry& entry = *iit;

			const Matrix4& mm = entry.mesh->getModelMatrix();
			Matrix4 mvMatrix = vMatrix * mm;
			Matrix4 mvpMatrix = vpMatrix * mm;
			Matrix4 nm = Matrix4::Identity;

			mvMatrices[entry.id] = mvMatrix;
			mvpMatrices[entry.id] = mvpMatrix;
			normalMatrices[entry.id] = nm;
		}
	}

	for (StaticMeshIterator it = transStaticMeshes.begin() ; it != transStaticMeshes.end() ; it++) {
		for (list<StaticRenderingMeshEntry>::iterator iit = it->second.begin() ; iit != it->second.end() ; iit++) {
			StaticRenderingMeshEntry& entry = *iit;

			const Matrix4& mm = entry.mesh->getModelMatrix();
			Matrix4 mvMatrix = vMatrix * mm;
			Matrix4 mvpMatrix = vpMatrix * mm;
			Matrix4 nm = Matrix4::Identity;

			mvMatrices[entry.id] = mvMatrix;
			mvpMatrices[entry.id] = mvpMatrix;
			normalMatrices[entry.id] = nm;
		}
	}

	for (AnimatedMeshIterator it = opaqueAnimMeshes.begin() ; it != opaqueAnimMeshes.end() ; it++) {
		for (list<AnimatedRenderingMeshEntry>::iterator iit = it->second.begin() ; iit != it->second.end() ; iit++) {
			AnimatedRenderingMeshEntry& entry = *iit;

			const Matrix4& mm = entry.mesh->getModelMatrix();
			Matrix4 mvMatrix = vMatrix * mm;
			Matrix4 mvpMatrix = vpMatrix * mm;
			Matrix4 nm = Matrix4::Identity;

			mvMatrices[entry.id] = mvMatrix;
			mvpMatrices[entry.id] = mvpMatrix;
			normalMatrices[entry.id] = nm;
		}
	}

	for (AnimatedMeshIterator it = transAnimMeshes.begin() ; it != transAnimMeshes.end() ; it++) {
		for (list<AnimatedRenderingMeshEntry>::iterator iit = it->second.begin() ; iit != it->second.end() ; iit++) {
			AnimatedRenderingMeshEntry& entry = *iit;

			const Matrix4& mm = entry.mesh->getModelMatrix();
			Matrix4 mvMatrix = vMatrix * mm;
			Matrix4 mvpMatrix = vpMatrix * mm;
			Matrix4 nm = Matrix4::Identity;

			mvMatrices[entry.id] = mvMatrix;
			mvpMatrices[entry.id] = mvpMatrix;
			normalMatrices[entry.id] = nm;
		}
	}


	updateMiscShaderPrograms();

	if (wireframeRendering)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

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

#ifdef TIME_RENDERING
	e = GetTickcountMicroseconds();
	uint64_t t2 = e-s;
	s = GetTickcountMicroseconds();
#endif

	for (StaticMeshIterator it = opaqueStaticMeshes.begin() ; it != opaqueStaticMeshes.end() ; it++) {
		AdvancedShaderProgramCache::Pointer ptr = it->first;
		AdvancedShaderProgramCacheEntry* entry = (AdvancedShaderProgramCacheEntry*) ptr.getEntry(true);
		AdvancedShaderProgram* prog = entry->programs[OpaqueStaticShaderProgram];
		prog->makeCurrent();
		currentProgram = prog;
		initializeUniforms(prog);

		applyShaderPluginUniformBuffers(prog);
		glUniform3f(renderProgramLocations->camPosUniform, cpos.getX(), cpos.getY(), cpos.getZ());
		glUniformMatrix4fv(renderProgramLocations->vMatrixUniform, 1, GL_FALSE, (float*) &vMatrix);
		glUniformMatrix4fv(renderProgramLocations->vpMatrixUniform, 1, GL_FALSE, (float*) &vpMatrix);
		setupDynamicLighting(true);

		renderStaticMeshes(it->second.begin(), it->second.end());

		ptr.release();
	}

	for (AnimatedMeshIterator it = opaqueAnimMeshes.begin() ; it != opaqueAnimMeshes.end() ; it++) {
		AdvancedShaderProgramCache::Pointer ptr = it->first;
		AdvancedShaderProgramCacheEntry* entry = (AdvancedShaderProgramCacheEntry*) ptr.getEntry(true);
		AdvancedShaderProgram* prog = entry->programs[OpaqueAnimatedShaderProgram];
		prog->makeCurrent();
		currentProgram = prog;
		initializeUniforms(prog);

		applyShaderPluginUniformBuffers(prog);
		glUniform3f(renderProgramLocations->camPosUniform, cpos.getX(), cpos.getY(), cpos.getZ());
		glUniformMatrix4fv(renderProgramLocations->vMatrixUniform, 1, GL_FALSE, (float*) &vMatrix);
		glUniformMatrix4fv(renderProgramLocations->vpMatrixUniform, 1, GL_FALSE, (float*) &vpMatrix);
		setupDynamicLighting(true);

		renderAnimatedMeshes(it->second.begin(), it->second.end());

		ptr.release();
	}

#if 1

#ifdef TIME_RENDERING
	e = GetTickcountMicroseconds();
	uint64_t t3 = e-s;
	s = GetTickcountMicroseconds();
#endif

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

		for (StaticMeshIterator it = opaqueStaticMeshes.begin() ; it != opaqueStaticMeshes.end() ; it++) {
			AdvancedShaderProgramCache::Pointer ptr = it->first;
			AdvancedShaderProgramCacheEntry* entry = (AdvancedShaderProgramCacheEntry*) ptr.getEntry(true);
			AdvancedShaderProgram* prog = entry->programs[OpaqueStaticShaderProgram];
			prog->makeCurrent();
			currentProgram = prog;
			initializeUniforms(prog);

			applyShaderPluginUniformBuffers(prog);
			glUniform3f(renderProgramLocations->camPosUniform, cpos.getX(), cpos.getY(), cpos.getZ());
			glUniformMatrix4fv(renderProgramLocations->vMatrixUniform, 1, GL_FALSE, (float*) &vMatrix);
			glUniformMatrix4fv(renderProgramLocations->vpMatrixUniform, 1, GL_FALSE, (float*) &vpMatrix);
			glUniform4f(renderProgramLocations->materialColorUniform, 0.0f, 0.0f, 0.0f, 0.0f);
			setupDynamicLighting(true);

			renderStaticMeshes(it->second.begin(), it->second.end());

			ptr.release();
		}

		for (AnimatedMeshIterator it = opaqueAnimMeshes.begin() ; it != opaqueAnimMeshes.end() ; it++) {
			AdvancedShaderProgramCache::Pointer ptr = it->first;
			AdvancedShaderProgramCacheEntry* entry = (AdvancedShaderProgramCacheEntry*) ptr.getEntry(true);
			AdvancedShaderProgram* prog = entry->programs[OpaqueAnimatedShaderProgram];
			prog->makeCurrent();
			currentProgram = prog;
			initializeUniforms(prog);

			applyShaderPluginUniformBuffers(prog);
			glUniform3f(renderProgramLocations->camPosUniform, cpos.getX(), cpos.getY(), cpos.getZ());
			glUniformMatrix4fv(renderProgramLocations->vMatrixUniform, 1, GL_FALSE, (float*) &vMatrix);
			glUniformMatrix4fv(renderProgramLocations->vpMatrixUniform, 1, GL_FALSE, (float*) &vpMatrix);
			glUniform4f(renderProgramLocations->materialColorUniform, 0.0f, 0.0f, 0.0f, 0.0f);
			setupDynamicLighting(true);

			renderAnimatedMeshes(it->second.begin(), it->second.end());

			ptr.release();
		}

		if (!light->isShadowCastingEnabled())
			glEnable(GL_STENCIL_TEST);
	}

	glDisable(GL_STENCIL_TEST);

#ifdef TIME_RENDERING
	e = GetTickcountMicroseconds();
	uint64_t t4 = e-s;
	s = GetTickcountMicroseconds();
#endif


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


#ifdef TIME_RENDERING
	e = GetTickcountMicroseconds();
	uint64_t t5 = e-s;
	s = GetTickcountMicroseconds();
#endif

	// Do the depth-peeling passes
	for (int currentPass = 0 ; currentPass < dpNumPasses ; currentPass++) {
		int inputIdx = (currentPass+1) % 2;
		int outputIdx = currentPass % 2;

		glDisable(GL_BLEND);

		// ********** Peel a single layer **********
		gtaglBindFramebuffer(GTAGL_FRAMEBUFFER, fbos.dpPingPongFBOs[outputIdx]);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Bind the input depth texture to texture unit 0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(dpTexTarget, fbos.dpPingPongDepthStencilTexes[inputIdx]);

		// Bind the opaque depth texture to texture unit 1
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(dpTexTarget, fbos.dpOpaqueTexes[1]);

		// Back to texture unit 2 (the render* methods rely on it to be active)
		glActiveTexture(GL_TEXTURE2);



		// Ambient lighting pass

		globalAmbientLightEnabled = true;
		enabledLightBegin = lightSources.end();
		enabledLightEnd = lightSources.end();

		for (StaticMeshIterator it = transStaticMeshes.begin() ; it != transStaticMeshes.end() ; it++) {
			AdvancedShaderProgramCache::Pointer ptr = it->first;
			AdvancedShaderProgramCacheEntry* entry = (AdvancedShaderProgramCacheEntry*) ptr.getEntry(true);
			AdvancedShaderProgram* prog = entry->programs[TransparentStaticShaderProgram];
			prog->makeCurrent();
			currentProgram = prog;
			dpSetupShaderUniforms(prog);
			initializeUniforms(prog);

			applyShaderPluginUniformBuffers(prog);
			glUniform3f(renderProgramLocations->camPosUniform, cpos.getX(), cpos.getY(), cpos.getZ());
			glUniformMatrix4fv(renderProgramLocations->vMatrixUniform, 1, GL_FALSE, (float*) &vMatrix);
			glUniformMatrix4fv(renderProgramLocations->vpMatrixUniform, 1, GL_FALSE, (float*) &vpMatrix);
			setupDynamicLighting(true);

			renderStaticMeshes(it->second.begin(), it->second.end());

			ptr.release();
		}

		for (AnimatedMeshIterator it = transAnimMeshes.begin() ; it != transAnimMeshes.end() ; it++) {
			AdvancedShaderProgramCache::Pointer ptr = it->first;
			AdvancedShaderProgramCacheEntry* entry = (AdvancedShaderProgramCacheEntry*) ptr.getEntry(true);
			AdvancedShaderProgram* prog = entry->programs[TransparentAnimatedShaderProgram];
			prog->makeCurrent();
			currentProgram = prog;
			dpSetupShaderUniforms(prog);
			initializeUniforms(prog);

			applyShaderPluginUniformBuffers(prog);
			glUniform3f(renderProgramLocations->camPosUniform, cpos.getX(), cpos.getY(), cpos.getZ());
			glUniformMatrix4fv(renderProgramLocations->vMatrixUniform, 1, GL_FALSE, (float*) &vMatrix);
			glUniformMatrix4fv(renderProgramLocations->vpMatrixUniform, 1, GL_FALSE, (float*) &vpMatrix);
			setupDynamicLighting(true);

			renderAnimatedMeshes(it->second.begin(), it->second.end());

			ptr.release();
		}

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

			for (StaticMeshIterator it = transStaticMeshes.begin() ; it != transStaticMeshes.end() ; it++) {
				AdvancedShaderProgramCache::Pointer ptr = it->first;
				AdvancedShaderProgramCacheEntry* entry = (AdvancedShaderProgramCacheEntry*) ptr.getEntry(true);
				AdvancedShaderProgram* prog = entry->programs[TransparentStaticShaderProgram];
				prog->makeCurrent();
				currentProgram = prog;
				dpSetupShaderUniforms(prog);
				initializeUniforms(prog);

				applyShaderPluginUniformBuffers(prog);
				glUniform3f(renderProgramLocations->camPosUniform, cpos.getX(), cpos.getY(), cpos.getZ());
				glUniformMatrix4fv(renderProgramLocations->vMatrixUniform, 1, GL_FALSE, (float*) &vMatrix);
				glUniformMatrix4fv(renderProgramLocations->vpMatrixUniform, 1, GL_FALSE, (float*) &vpMatrix);
				glUniform4f(renderProgramLocations->materialColorUniform, 0.0f, 0.0f, 0.0f, 0.0f);
				setupDynamicLighting(true);

				renderStaticMeshes(it->second.begin(), it->second.end());

				ptr.release();
			}

			for (AnimatedMeshIterator it = transAnimMeshes.begin() ; it != transAnimMeshes.end() ; it++) {
				AdvancedShaderProgramCache::Pointer ptr = it->first;
				AdvancedShaderProgramCacheEntry* entry = (AdvancedShaderProgramCacheEntry*) ptr.getEntry(true);
				AdvancedShaderProgram* prog = entry->programs[TransparentAnimatedShaderProgram];
				prog->makeCurrent();
				currentProgram = prog;
				dpSetupShaderUniforms(prog);
				initializeUniforms(prog);

				applyShaderPluginUniformBuffers(prog);
				glUniform3f(renderProgramLocations->camPosUniform, cpos.getX(), cpos.getY(), cpos.getZ());
				glUniformMatrix4fv(renderProgramLocations->vMatrixUniform, 1, GL_FALSE, (float*) &vMatrix);
				glUniformMatrix4fv(renderProgramLocations->vpMatrixUniform, 1, GL_FALSE, (float*) &vpMatrix);
				glUniform4f(renderProgramLocations->materialColorUniform, 0.0f, 0.0f, 0.0f, 0.0f);
				setupDynamicLighting(true);

				renderAnimatedMeshes(it->second.begin(), it->second.end());

				ptr.release();
			}

			if (!light->isShadowCastingEnabled())
				glEnable(GL_STENCIL_TEST);
		}

		glDisable(GL_STENCIL_TEST);
		glDepthMask(GL_TRUE);


		glDisableVertexAttribArray(VertexAttributeLocationVertex);
		glDisableVertexAttribArray(VertexAttributeLocationNormal);
		glDisableVertexAttribArray(VertexAttributeLocationTexCoord);
		glDisableVertexAttribArray(VertexAttributeLocationVertexColor);
		glDisableVertexAttribArray(VertexAttributeLocationBoneIndex);
		glDisableVertexAttribArray(VertexAttributeLocationBoneWeight);
		glDisableVertexAttribArray(VertexAttributeLocationSubmeshIndex);

		// Our front-to-back blending equation. The source alpha is premultiplied in the fragment shader
		glBlendFuncSeparate(GL_DST_ALPHA, GL_ONE, GL_ZERO, GL_ONE_MINUS_SRC_ALPHA);

		programs.dpBlendLayerProgram->makeCurrent();

		glDisable(GL_DEPTH_TEST);

		gtaglBindFramebuffer(GTAGL_FRAMEBUFFER, 0);

		if (wireframeRendering)
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		// Render a screen-size plane with the contents of the output color buffer
		glBindBuffer(GL_ARRAY_BUFFER, planeDataBuf);
		glEnableVertexAttribArray(dpShaderLocations.dpBlendLayerVertexAttrib);
		glVertexAttribPointer(dpShaderLocations.dpBlendLayerVertexAttrib, 2, GL_FLOAT, GL_FALSE, 0, (void*) 0);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(dpTexTarget, fbos.dpPingPongColorTexes[outputIdx]);
		glUniform1i(dpShaderLocations.dpBlendLayerTexUniform, 0);

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		glDisableVertexAttribArray(dpShaderLocations.dpBlendLayerVertexAttrib);

		glEnable(GL_DEPTH_TEST);

		if (wireframeRendering)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}


#ifdef TIME_RENDERING
	e = GetTickcountMicroseconds();
	uint64_t t6 = e-s;
	s = GetTickcountMicroseconds();
#endif


	// ********** Blend opaque and transparent layers together **********

	if (wireframeRendering)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glDisable(GL_DEPTH_TEST);

	programs.dpBlendFinalProgram->makeCurrent();

	// We still blend front-to-back here, so we'll still use the old blend functions
	glBlendFuncSeparate(GL_DST_ALPHA, GL_ONE, GL_ZERO, GL_ONE_MINUS_SRC_ALPHA);

	gtaglBindFramebuffer(GTAGL_FRAMEBUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER, planeDataBuf);
	glEnableVertexAttribArray(dpShaderLocations.dpBlendFinalVertexAttrib);
	glVertexAttribPointer(dpShaderLocations.dpBlendFinalVertexAttrib, 2, GL_FLOAT, GL_FALSE, 0, (void*) 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(dpTexTarget, fbos.dpOpaqueTexes[0]);
	glUniform1i(dpShaderLocations.dpBlendFinalTexUniform, 0);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glDisableVertexAttribArray(dpShaderLocations.dpBlendFinalVertexAttrib);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glDisable(GL_BLEND);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


	gtaglBindFramebuffer(GTAGL_FRAMEBUFFER, 0);

#ifdef TIME_RENDERING
	e = GetTickcountMicroseconds();
	uint64_t t7 = e-s;
	s = GetTickcountMicroseconds();
#endif

#endif

#endif

	for (list<RenderingEntity*>::iterator it = renderingEntityDisposalList.begin() ; it != renderingEntityDisposalList.end() ; it++) {
		delete *it;
	}

#ifdef TIME_RENDERING
	e = GetTickcountMicroseconds();
	uint64_t t8 = e-s;
	s = GetTickcountMicroseconds();
#endif

	renderingEntityDisposalList.clear();

#ifdef TIME_RENDERING
	e = GetTickcountMicroseconds();
	uint64_t t9 = e-s;
	s = GetTickcountMicroseconds();
#endif


	opaqueStaticMeshes.clear();
	transStaticMeshes.clear();
	opaqueAnimMeshes.clear();
	transAnimMeshes.clear();

	lightSources.clear();

	numObjects = 0;

#ifdef TIME_RENDERING
	e = GetTickcountMicroseconds();
	uint64_t t10 = e-s;

	absE = GetTickcountMicroseconds();

	fprintf(stderr, "=== RENDER TIMING ===\n");
	fprintf(stderr, "Absolute:        %uus\n", (unsigned int) (absE-absS));
	fprintf(stderr, "T1:              %uus\n", (unsigned int) t1);
	fprintf(stderr, "T2:              %uus\n", (unsigned int) t2);
	fprintf(stderr, "T3:              %uus\n", (unsigned int) t3);
	fprintf(stderr, "T4:              %uus\n", (unsigned int) t4);
	fprintf(stderr, "T5:              %uus\n", (unsigned int) t5);
	fprintf(stderr, "T6:              %uus\n", (unsigned int) t6);
	fprintf(stderr, "T7:              %uus\n", (unsigned int) t7);
	fprintf(stderr, "T8:              %uus\n", (unsigned int) t8);
	fprintf(stderr, "T9:              %uus\n", (unsigned int) t9);
	fprintf(stderr, "T10:             %uus\n", (unsigned int) t10);

	fprintf(stderr, "\n");
#endif
}


void DefaultRenderer::renderStaticMeshes(list<StaticRenderingMeshEntry>::iterator beg, list<StaticRenderingMeshEntry>::iterator end)
{
	GLuint currentBuf = 0;

	glUniform4f(renderProgramLocations->matAmbientReflectionUniform, 1.0f, 1.0f, 1.0f, 1.0f);
	glUniform4f(renderProgramLocations->matDiffuseReflectionUniform, 1.0f, 1.0f, 1.0f, 1.0f);
	glUniform4f(renderProgramLocations->matSpecularReflectionUniform, 1.0f, 1.0f, 1.0f, 1.0f);

	for (list<StaticRenderingMeshEntry>::iterator it = beg ; it != end ; it++) {
		StaticRenderingMeshEntry& entry = *it;
		RenderingMesh* smesh = entry.mesh;

		uint32_t flags = smesh->getFlags();

		int vcOffs = smesh->getVertexColorOffset();

		GLuint dbuf = smesh->getDataBuffer();

		if (dbuf != currentBuf) {
			glBindBuffer(GL_ARRAY_BUFFER, dbuf);

			glEnableVertexAttribArray(Renderer::VertexAttributeLocationVertex);
			glVertexAttribPointer(Renderer::VertexAttributeLocationVertex, 3, GL_FLOAT, GL_FALSE,
					smesh->getVertexStride(), (void*) (intptr_t) smesh->getVertexOffset());

			int nOffs = smesh->getNormalOffset();
			int tcOffs = smesh->getTexCoordOffset();
			int smOffs = smesh->getSubmeshIDOffset();

			if (nOffs != -1) {
				glEnableVertexAttribArray(Renderer::VertexAttributeLocationNormal);
				glVertexAttribPointer(Renderer::VertexAttributeLocationNormal, 3, GL_FLOAT, GL_FALSE,
						smesh->getNormalStride(), (void*) (intptr_t) nOffs);
			} else {
				glDisableVertexAttribArray(Renderer::VertexAttributeLocationNormal);
			}

			if (tcOffs != -1) {
				glEnableVertexAttribArray(Renderer::VertexAttributeLocationTexCoord);
				glVertexAttribPointer(Renderer::VertexAttributeLocationTexCoord, 2, GL_FLOAT, GL_FALSE,
						smesh->getTexCoordStride(), (void*) (intptr_t) tcOffs);
			} else {
				glDisableVertexAttribArray(Renderer::VertexAttributeLocationTexCoord);
			}

			if (vcOffs != -1) {
				glEnableVertexAttribArray(Renderer::VertexAttributeLocationVertexColor);
				glVertexAttribPointer(Renderer::VertexAttributeLocationVertexColor, 4, GL_UNSIGNED_BYTE, GL_TRUE,
						smesh->getVertexColorStride(), (void*) (intptr_t) vcOffs);
			} else {
				glDisableVertexAttribArray(Renderer::VertexAttributeLocationVertexColor);
			}

			if (smOffs != -1) {
				glEnableVertexAttribArray(Renderer::VertexAttributeLocationSubmeshIndex);
				glVertexAttribIPointer(Renderer::VertexAttributeLocationSubmeshIndex, 1, GL_UNSIGNED_BYTE,
						smesh->getSubmeshIDStride(), (void*) (intptr_t) smOffs);
			} else {
				glDisableVertexAttribArray(Renderer::VertexAttributeLocationSubmeshIndex);
			}

			currentBuf = dbuf;
		}

		Matrix4 mm = smesh->getModelMatrix();

		glUniformMatrix4fv(renderProgramLocations->mMatrixUniform, 1, GL_FALSE, (float*) &mm);
		glUniformMatrix4fv(renderProgramLocations->mvMatrixUniform, 1, GL_FALSE, (float*) (mvMatrices + entry.id));
		glUniformMatrix4fv(renderProgramLocations->mvpMatrixUniform, 1, GL_FALSE, (float*) (mvpMatrices + entry.id));
		glUniformMatrix4fv(renderProgramLocations->normalMvMatrixUniform, 1, GL_FALSE, (float*) (normalMatrices + entry.id));

		glUniform1i(renderProgramLocations->vertexColorsUniform, (vcOffs != -1) ? 1 : 0);

		if ((flags & RenderingMesh::EnableShaderPluginUniformBuffers)  !=  0) {
			smesh->getPluginRegistry().applyUniformBuffers(currentProgram);
		}

		GLenum polyMode = smesh->getPrimitiveFormat();

		size_t numSubmeshes = smesh->getSubmeshCount();
		size_t submeshOffs = 0;

		RenderingMesh::SubmeshIterator sbeg, send;
		sbeg = smesh->getSubmeshBegin();
		send = smesh->getSubmeshEnd();

		GLuint indexOffs = 0;

		while (submeshOffs < numSubmeshes) {
			GLint texturedVals[NUM_SUBMESHES];
			GLint textureVals[NUM_SUBMESHES];
			float matColors[NUM_SUBMESHES*4];

			GLuint numIndices = 0;
			GLsizei indexCounts[NUM_SUBMESHES];
			GLvoid* indexOffsets[NUM_SUBMESHES];
			GLint baseVertices[NUM_SUBMESHES];

			uint8_t numPassSubmeshes = numSubmeshes-submeshOffs;

			if (numPassSubmeshes > NUM_SUBMESHES)
				numPassSubmeshes = NUM_SUBMESHES;

			GLint dummyTexUnit = -1;

			for (uint8_t j = 0 ; j < numPassSubmeshes ; j++, sbeg++) {
				RenderingSubmesh* submesh = *sbeg;

				GLuint tex = submesh->getTexture();

				if (tex != 0) {
					glActiveTexture(GL_TEXTURE2 + j);
					glBindTexture(GL_TEXTURE_2D, tex);
					texturedVals[j] = 1;
					textureVals[j] = j+2;
					dummyTexUnit = j+2;
				} else {
					texturedVals[j] = 0;
				}

				uint8_t r, g, b, a;
				submesh->getMaterialColor(r, g, b, a);

				if (!globalAmbientLightEnabled) {
					r = 0;
					g = 0;
					b = 0;
					a = 0;
				}

				matColors[j*4] = r / 255.0f;
				matColors[j*4 + 1] = g / 255.0f;
				matColors[j*4 + 2] = b / 255.0f;
				matColors[j*4 + 3] = a / 255.0f;

				indexCounts[j] = submesh->getIndexCount();
				indexOffsets[j] = (GLvoid*) (intptr_t) indexOffs;
				baseVertices[j] = smesh->getVertexCount() * (submeshOffs + j);

				indexOffs += submesh->getIndexCount() * 4;

				numIndices += submesh->getIndexCount();
			}

			// This is necessary, because OpenGL performs some sanity checks on all samplers in the current shader when
			// the draw functions are called, and if some samplers are uninitialized, these checks will very likely fail
			// (as uninitialized samplers seem to default to GL_TEXTURE0, which is used by depth peeling code with a
			// GL_TEXTURE_RECTANGLE format that is incompatible to the sampler2Ds used in the core shaders).
			if (dummyTexUnit == -1) {
				dummyTexUnit = 2;
				glActiveTexture(GL_TEXTURE2);
				glBindTexture(GL_TEXTURE_2D, dummyTex);
			}

			for (uint8_t j = 0 ; j < NUM_SUBMESHES ; j++) {
				if (j >= numPassSubmeshes  ||  texturedVals[j] == 0) {
					textureVals[j] = dummyTexUnit;
				}
			}

			glUniform1i(renderProgramLocations->submeshOffsetUniform, submeshOffs);
			glUniform1iv(renderProgramLocations->texturedUniform, numPassSubmeshes, texturedVals);
			glUniform1iv(renderProgramLocations->textureUniform, NUM_SUBMESHES, textureVals);
			glUniform4fv(renderProgramLocations->materialColorUniform, numPassSubmeshes, matColors);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, smesh->getIndexBuffer());

			glMultiDrawElementsBaseVertex(polyMode, indexCounts, GL_UNSIGNED_INT, indexOffsets, numPassSubmeshes, baseVertices);

			submeshOffs += numPassSubmeshes;
		}
	}
}


void DefaultRenderer::renderAnimatedMeshes(list<AnimatedRenderingMeshEntry>::iterator beg, list<AnimatedRenderingMeshEntry>::iterator end)
{
	GLuint currentBuf = 0;

	for (list<AnimatedRenderingMeshEntry>::iterator it = beg ; it != end ; it++) {
		AnimatedRenderingMeshEntry& entry = *it;
		RenderingMesh* amesh = entry.mesh;


		glUniform4f(renderProgramLocations->matAmbientReflectionUniform, 1.0f, 1.0f, 1.0f, 1.0f);
		glUniform4f(renderProgramLocations->matDiffuseReflectionUniform, 1.0f, 1.0f, 1.0f, 1.0f);
		glUniform4f(renderProgramLocations->matSpecularReflectionUniform, 1.0f, 1.0f, 1.0f, 1.0f);


		int vcOffs = amesh->getVertexColorOffset();

		GLuint dbuf = amesh->getDataBuffer();

		if (dbuf != currentBuf) {
			glBindBuffer(GL_ARRAY_BUFFER, dbuf);

			glEnableVertexAttribArray(Renderer::VertexAttributeLocationVertex);
			glVertexAttribPointer(Renderer::VertexAttributeLocationVertex, 3, GL_FLOAT, GL_FALSE,
					amesh->getVertexStride(), (void*) (intptr_t) amesh->getVertexOffset());

			int nOffs = amesh->getNormalOffset();
			int tcOffs = amesh->getTexCoordOffset();
			int biOffs = amesh->getBoneIndexOffset();
			int bwOffs = amesh->getBoneWeightOffset();
			int smOffs = amesh->getSubmeshIDOffset();

			if (nOffs != -1) {
				glEnableVertexAttribArray(Renderer::VertexAttributeLocationNormal);
				glVertexAttribPointer(Renderer::VertexAttributeLocationNormal, 3, GL_FLOAT, GL_FALSE,
						amesh->getNormalStride(), (void*) (intptr_t) nOffs);
			} else {
				glDisableVertexAttribArray(Renderer::VertexAttributeLocationNormal);
			}

			if (tcOffs != -1) {
				glEnableVertexAttribArray(Renderer::VertexAttributeLocationTexCoord);
				glVertexAttribPointer(Renderer::VertexAttributeLocationTexCoord, 2, GL_FLOAT, GL_FALSE,
						amesh->getTexCoordStride(), (void*) (intptr_t) tcOffs);
			} else {
				glDisableVertexAttribArray(Renderer::VertexAttributeLocationTexCoord);
			}

			if (vcOffs != -1) {
				glEnableVertexAttribArray(Renderer::VertexAttributeLocationVertexColor);
				glVertexAttribPointer(Renderer::VertexAttributeLocationVertexColor, 4, GL_UNSIGNED_BYTE, GL_TRUE,
						amesh->getVertexColorStride(), (void*) (intptr_t) vcOffs);
			} else {
				glDisableVertexAttribArray(Renderer::VertexAttributeLocationVertexColor);
			}

			if (biOffs != -1) {
				glEnableVertexAttribArray(Renderer::VertexAttributeLocationBoneIndex);
				glVertexAttribPointer(Renderer::VertexAttributeLocationBoneIndex, 4, GL_UNSIGNED_BYTE, GL_FALSE,
						amesh->getBoneIndexStride(), (void*) (intptr_t) biOffs);
			} else {
				glDisableVertexAttribArray(Renderer::VertexAttributeLocationBoneIndex);
			}

			if (bwOffs != -1) {
				glEnableVertexAttribArray(Renderer::VertexAttributeLocationBoneWeight);
				glVertexAttribPointer(Renderer::VertexAttributeLocationBoneWeight, 4, GL_FLOAT, GL_FALSE,
						amesh->getBoneWeightStride(), (void*) (intptr_t) bwOffs);
			} else {
				glDisableVertexAttribArray(Renderer::VertexAttributeLocationBoneWeight);
			}

			if (smOffs != -1) {
				glEnableVertexAttribArray(Renderer::VertexAttributeLocationSubmeshIndex);
				glVertexAttribIPointer(Renderer::VertexAttributeLocationSubmeshIndex, 1, GL_UNSIGNED_BYTE,
						amesh->getSubmeshIDStride(), (void*) (intptr_t) smOffs);
			} else {
				glDisableVertexAttribArray(Renderer::VertexAttributeLocationSubmeshIndex);
			}

			currentBuf = dbuf;
		}

		int16_t boneIdx = amesh->getBoneIndex();

		if (boneIdx == -1) {
			// Skinned mesh
			glUniform1i(renderProgramLocations->boneUniform, -1);
		} else {
			// Unskinned mesh
			glUniform1i(renderProgramLocations->boneUniform, boneIdx);
		}

		Matrix4 mm = amesh->getModelMatrix();

		glUniformMatrix4fv(renderProgramLocations->boneMatUniform, amesh->getBoneCount(), GL_FALSE, (float*) amesh->getBoneMatrices());
		glUniformMatrix4fv(renderProgramLocations->mMatrixUniform, 1, GL_FALSE, (float*) &mm);
		glUniformMatrix4fv(renderProgramLocations->mvMatrixUniform, 1, GL_FALSE, (float*) (mvMatrices + entry.id));
		glUniformMatrix4fv(renderProgramLocations->mvpMatrixUniform, 1, GL_FALSE, (float*) (mvpMatrices + entry.id));
		glUniformMatrix4fv(renderProgramLocations->normalMvMatrixUniform, 1, GL_FALSE, (float*) (normalMatrices + entry.id));

		glUniform1i(renderProgramLocations->vertexColorsUniform, (vcOffs != -1) ? 1 : 0);


		GLenum polyMode = amesh->getPrimitiveFormat();

		size_t numSubmeshes = amesh->getSubmeshCount();
		size_t submeshOffs = 0;

		RenderingMesh::SubmeshIterator sbeg, send;
		sbeg = amesh->getSubmeshBegin();
		send = amesh->getSubmeshEnd();

		GLuint indexOffs = 0;

		while (submeshOffs < numSubmeshes) {
			GLint texturedVals[NUM_SUBMESHES];
			GLint textureVals[NUM_SUBMESHES];
			float matColors[NUM_SUBMESHES*4];

			GLuint numIndices = 0;
			GLsizei indexCounts[NUM_SUBMESHES];
			GLvoid* indexOffsets[NUM_SUBMESHES];
			GLint baseVertices[NUM_SUBMESHES];

			uint8_t numPassSubmeshes = numSubmeshes-submeshOffs;

			if (numPassSubmeshes > NUM_SUBMESHES)
				numPassSubmeshes = NUM_SUBMESHES;

			GLint dummyTexUnit = -1;

			for (uint8_t j = 0 ; j < numPassSubmeshes ; j++, sbeg++) {
				RenderingSubmesh* submesh = *sbeg;

				GLuint tex = submesh->getTexture();

				if (tex != 0) {
					glActiveTexture(GL_TEXTURE2 + j);
					glBindTexture(GL_TEXTURE_2D, tex);
					texturedVals[j] = 1;
					textureVals[j] = j+2;
					dummyTexUnit = j+2;
				} else {
					texturedVals[j] = 0;
				}

				uint8_t r, g, b, a;
				submesh->getMaterialColor(r, g, b, a);

				if (!globalAmbientLightEnabled) {
					r = 0;
					g = 0;
					b = 0;
					a = 0;
				}

				matColors[j*4] = r / 255.0f;
				matColors[j*4 + 1] = g / 255.0f;
				matColors[j*4 + 2] = b / 255.0f;
				matColors[j*4 + 3] = a / 255.0f;

				indexCounts[j] = submesh->getIndexCount();
				indexOffsets[j] = (GLvoid*) (intptr_t) indexOffs;
				baseVertices[j] = amesh->getVertexCount() * (submeshOffs + j);

				indexOffs += submesh->getIndexCount() * 4;

				numIndices += submesh->getIndexCount();
			}

			// This is necessary, because OpenGL performs some sanity checks on all samplers in the current shader when
			// the draw functions are called, and if some samplers are uninitialized, these checks will very likely fail
			// (as uninitialized samplers seem to default to GL_TEXTURE0, which is used by depth peeling code with a
			// GL_TEXTURE_RECTANGLE format that is incompatible to the sampler2Ds used in the core shaders).
			if (dummyTexUnit == -1) {
				dummyTexUnit = 2;
				glActiveTexture(GL_TEXTURE2);
				glBindTexture(GL_TEXTURE_2D, dummyTex);
			}

			for (uint8_t j = 0 ; j < NUM_SUBMESHES ; j++) {
				if (j >= numPassSubmeshes  ||  texturedVals[j] == 0) {
					textureVals[j] = dummyTexUnit;
				}
			}

			glUniform1i(renderProgramLocations->submeshOffsetUniform, submeshOffs);
			glUniform1iv(renderProgramLocations->texturedUniform, numPassSubmeshes, texturedVals);
			glUniform1iv(renderProgramLocations->textureUniform, NUM_SUBMESHES, textureVals);
			glUniform4fv(renderProgramLocations->materialColorUniform, numPassSubmeshes, matColors);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, amesh->getIndexBuffer());

			glMultiDrawElementsBaseVertex(polyMode, indexCounts, GL_UNSIGNED_INT, indexOffsets, numPassSubmeshes, baseVertices);

			submeshOffs += numPassSubmeshes;
		}
	}
}

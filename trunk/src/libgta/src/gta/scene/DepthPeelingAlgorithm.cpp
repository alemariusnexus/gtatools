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

#include "DepthPeelingAlgorithm.h"
#include "../Engine.h"
#include "../GLException.h"
#include "../gldebug.h"

#include <res_dp_peel_layer_vertex_shader.h>
#include <res_dp_peel_layer_fragment_shader.h>
#include <res_dp_blend_layer_vertex_shader.h>
#include <res_dp_blend_layer_fragment_shader.h>
#include <res_dp_blend_final_vertex_shader.h>
#include <res_dp_blend_final_fragment_shader.h>





void DepthPeelingAlgorithm::install()
{
	if (!inited) {
		Engine* engine = Engine::getInstance();

		const char* peelLayerVertexShaderData;
		const char* peelLayerFragmentShaderData;
		const char* blendLayerVertexShaderData;
		const char* blendLayerFragmentShaderData;
		const char* blendFinalVertexShaderData;
		const char* blendFinalFragmentShaderData;

		int peelLayerVertexShaderDataLen;
		int peelLayerFragmentShaderDataLen;
		int blendLayerVertexShaderDataLen;
		int blendLayerFragmentShaderDataLen;
		int blendFinalVertexShaderDataLen;
		int blendFinalFragmentShaderDataLen;

		peelLayerVertexShaderData			= (const char*) res_dp_peel_layer_vertex_shader_data;
		peelLayerFragmentShaderData		= (const char*) res_dp_peel_layer_fragment_shader_data;
		blendLayerVertexShaderData		= (const char*) res_dp_blend_layer_vertex_shader_data;
		blendLayerFragmentShaderData		= (const char*) res_dp_blend_layer_fragment_shader_data;
		blendFinalVertexShaderData		= (const char*) res_dp_blend_final_vertex_shader_data;
		blendFinalFragmentShaderData		= (const char*) res_dp_blend_final_fragment_shader_data;

		peelLayerVertexShaderDataLen			= sizeof(res_dp_peel_layer_vertex_shader_data);
		peelLayerFragmentShaderDataLen		= sizeof(res_dp_peel_layer_fragment_shader_data);
		blendLayerVertexShaderDataLen			= sizeof(res_dp_blend_layer_vertex_shader_data);
		blendLayerFragmentShaderDataLen		= sizeof(res_dp_blend_layer_fragment_shader_data);
		blendFinalVertexShaderDataLen			= sizeof(res_dp_blend_final_vertex_shader_data);
		blendFinalFragmentShaderDataLen		= sizeof(res_dp_blend_final_fragment_shader_data);

		peelLayerVertexShader = new Shader(GL_VERTEX_SHADER,
				"Depth Peeling Transparency Algorithm Peel Layer Vertex Shader");
		peelLayerVertexShader->loadSourceCode(CString(peelLayerVertexShaderData,
				peelLayerVertexShaderDataLen));
		peelLayerVertexShader->compile();

		peelLayerFragmentShader = new Shader(GL_FRAGMENT_SHADER,
				"Depth Peeling Transparency Algorithm Peel Layer Fragment Shader");
		peelLayerFragmentShader->loadSourceCode(CString(peelLayerFragmentShaderData,
				peelLayerFragmentShaderDataLen));
		peelLayerFragmentShader->compile();

		blendLayerVertexShader = new Shader(GL_VERTEX_SHADER,
				"Depth Peeling Transparency Algorithm Blend Layer Vertex Shader");
		blendLayerVertexShader->loadSourceCode(CString(blendLayerVertexShaderData,
				blendLayerVertexShaderDataLen));
		blendLayerVertexShader->compile();

		blendLayerFragmentShader = new Shader(GL_FRAGMENT_SHADER,
				"Depth Peeling Transparency Algorithm Blend Layer Fragment Shader");
		blendLayerFragmentShader->loadSourceCode(CString(blendLayerFragmentShaderData,
				blendLayerFragmentShaderDataLen));
		blendLayerFragmentShader->compile();

		blendFinalVertexShader = new Shader(GL_VERTEX_SHADER,
				"Depth Peeling Transparency Algorithm Blend Final Vertex Shader");
		blendFinalVertexShader->loadSourceCode(CString(blendFinalVertexShaderData,
				blendFinalVertexShaderDataLen));
		blendFinalVertexShader->compile();

		blendFinalFragmentShader = new Shader(GL_FRAGMENT_SHADER,
				"Depth Peeling Transparency Algorithm Blend Final Fragment Shader");
		blendFinalFragmentShader->loadSourceCode(CString(blendFinalFragmentShaderData,
				blendFinalFragmentShaderDataLen));
		blendFinalFragmentShader->compile();

		blendLayerProgram = new ShaderProgram(
				"Depth Peeling Transparency Algorithm Blend Layer Shader Program");
		blendLayerProgram->attachShader(blendLayerVertexShader);
		blendLayerProgram->attachShader(blendLayerFragmentShader);
		blendLayerProgram->link();

		blendFinalProgram = new ShaderProgram(
				"Depth Peeling Transparency Algorithm Blend Final Shader Program");
		blendFinalProgram->attachShader(blendFinalVertexShader);
		blendFinalProgram->attachShader(blendFinalFragmentShader);
		blendFinalProgram->link();


		gtaglGenFramebuffers(2, fbos);
		gtaglGenFramebuffers(1, &mainFBO);

		glGenTextures(2, depthTexes);
		glGenTextures(2, colorTexes);
		glGenTextures(2, mainTexes);


		viewW = engine->getViewportWidth();
		viewH = engine->getViewportHeight();


#ifndef GTA_USE_OPENGL_ES
		if (gtaglIsVersionSupported(3, 1)) {
			gtaglBindFramebuffer(GTAGL_FRAMEBUFFER, fbos[0]);

			depthFormat = GL_DEPTH_COMPONENT32;

			// Initialize the depth texture for FBO 0
			glBindTexture(GL_TEXTURE_RECTANGLE, depthTexes[0]);
			glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_DEPTH_COMPONENT32, viewW, viewH, 0, GL_DEPTH_COMPONENT, GL_FLOAT,
					NULL);
			gtaglFramebufferTexture2D(GTAGL_FRAMEBUFFER, GTAGL_DEPTH_ATTACHMENT, GL_TEXTURE_RECTANGLE,
					depthTexes[0], 0);

			// Initialize the color texture for FBO 0
			glBindTexture(GL_TEXTURE_RECTANGLE, colorTexes[0]);
			glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGBA, viewW, viewH, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
			gtaglFramebufferTexture2D(GTAGL_FRAMEBUFFER, GTAGL_COLOR_ATTACHMENT0, GL_TEXTURE_RECTANGLE, colorTexes[0], 0);

			GLException::checkFramebufferStatus(GTAGL_FRAMEBUFFER,
					"During initialization of depth peeling FBO 0");


			gtaglBindFramebuffer(GTAGL_FRAMEBUFFER, fbos[1]);

			// Initialize the depth texture for FBO 1
			glBindTexture(GL_TEXTURE_RECTANGLE, depthTexes[1]);
			glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_DEPTH_COMPONENT32, viewW, viewH, 0, GL_DEPTH_COMPONENT, GL_FLOAT,
					NULL);
			gtaglFramebufferTexture2D(GTAGL_FRAMEBUFFER, GTAGL_DEPTH_ATTACHMENT, GL_TEXTURE_RECTANGLE,
					depthTexes[1], 0);

			// Initialize the color texture for FBO 1
			glBindTexture(GL_TEXTURE_RECTANGLE, colorTexes[1]);
			glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGBA, viewW, viewH, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
			gtaglFramebufferTexture2D(GTAGL_FRAMEBUFFER, GTAGL_COLOR_ATTACHMENT0, GL_TEXTURE_RECTANGLE, colorTexes[1], 0);

			GLException::checkFramebufferStatus(GTAGL_FRAMEBUFFER,
					"During initialization of depth peeling FBO 1");



			gtaglBindFramebuffer(GTAGL_FRAMEBUFFER, mainFBO);

			// Initialize the color texture for the main FBO
			glBindTexture(GL_TEXTURE_RECTANGLE, mainTexes[0]);
			glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGBA, viewW, viewH, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
			gtaglFramebufferTexture2D(GTAGL_FRAMEBUFFER, GTAGL_COLOR_ATTACHMENT0, GL_TEXTURE_RECTANGLE, mainTexes[0], 0);

			// Initialize the depth texture for the main FBO
			glBindTexture(GL_TEXTURE_RECTANGLE, mainTexes[1]);
			glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_DEPTH_COMPONENT, viewW, viewH, 0, GL_DEPTH_COMPONENT,
					GL_UNSIGNED_INT, NULL);
			gtaglFramebufferTexture2D(GTAGL_FRAMEBUFFER, GTAGL_DEPTH_ATTACHMENT, GL_TEXTURE_RECTANGLE, mainTexes[1], 0);

			GLException::checkFramebufferStatus(GTAGL_FRAMEBUFFER,
					"During initialization of depth peeling blend FBO");
		} else {
#endif
			gtaglBindFramebuffer(GTAGL_FRAMEBUFFER, fbos[0]);

			// Initialize the depth texture for FBO 0
			glBindTexture(GL_TEXTURE_2D, depthTexes[0]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, viewW, viewH, 0, GL_DEPTH_COMPONENT,
					GL_UNSIGNED_INT, NULL);
			gtaglFramebufferTexture2D(GTAGL_FRAMEBUFFER, GTAGL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
					depthTexes[0], 0);

			GLException::checkFramebufferStatus(GTAGL_FRAMEBUFFER,
					"During initialization of depth peeling FBO 0 [1]");

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
					"During initialization of depth peeling blend FBO");
#ifndef GTA_USE_OPENGL_ES
		}
#endif

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

		inited = true;

		GLException::checkError("After DepthPeelingAlgorithm::install()");
	}
}


void DepthPeelingAlgorithm::installShaders(ShaderProgram* program)
{
	program->attachShader(peelLayerVertexShader);
	program->attachShader(peelLayerFragmentShader);
}


void DepthPeelingAlgorithm::uninstallShaders(ShaderProgram* program)
{
	program->detachShader(peelLayerVertexShader);
	program->detachShader(peelLayerFragmentShader);
}


void DepthPeelingAlgorithm::performPostLinkOperations()
{
	blendLayerTexUniform = blendLayerProgram->getUniformLocation("LayerTex");
	blendLayerVertexAttrib = blendLayerProgram->getAttributeLocation("Vertex");

	blendFinalTexUniform = blendFinalProgram->getUniformLocation("CombinedLayerTex");
	blendFinalVertexAttrib = blendFinalProgram->getAttributeLocation("Vertex");
}


void DepthPeelingAlgorithm::performOpaqueRenderInit()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	gtaglBindFramebuffer(GTAGL_FRAMEBUFFER, mainFBO);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}


bool DepthPeelingAlgorithm::performTransparentRenderInit()
{
	// Clear the blending FBO to (0, 0, 0, 1). Exactly this value is needed for the blending equations
	/*gtaglBindFramebuffer(GTAGL_FRAMEBUFFER, mainFBO);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);*/

	// Clear input depth buffer to 0 (so that no fragment will be initially peeled away)
	gtaglBindFramebuffer(GTAGL_FRAMEBUFFER, fbos[1]);

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

	// Our front-to-back blending equation. The source alpha is premultiplied in the fragment shader
	glBlendFuncSeparate(GL_DST_ALPHA, GL_ONE, GL_ZERO, GL_ONE_MINUS_SRC_ALPHA);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	currentPass = 0;

	return numPasses != 0;
}


void DepthPeelingAlgorithm::setupShaderUniforms(ShaderProgram* program)
{
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


void DepthPeelingAlgorithm::performPreRenderPass()
{
#ifdef GTA_USE_OPENGL_ES
	GLenum texTarget = GL_TEXTURE_2D;
#else
	GLenum texTarget = gtaglIsVersionSupported(3, 1) ? GL_TEXTURE_RECTANGLE : GL_TEXTURE_2D;
#endif

	int inputIdx = (currentPass+1) % 2;
	int outputIdx = currentPass % 2;

	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);

	// ********** Peel a single layer **********
	gtaglBindFramebuffer(GTAGL_FRAMEBUFFER, fbos[outputIdx]);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Bind the input depth texture to texture unit 1 (unit 0 is used by the render* methods)
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(texTarget, depthTexes[inputIdx]);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(texTarget, mainTexes[1]);

	// Back to texture unit 0 (the render* methods rely on it to be active)
	glActiveTexture(GL_TEXTURE0);
}


bool DepthPeelingAlgorithm::performPostRenderPass()
{
	// ********** Blend the peeled layer into dpBlendFBO **********

	int inputIdx = (currentPass+1) % 2;
	int outputIdx = currentPass % 2;

#ifdef GTA_USE_OPENGL_ES
	GLenum texTarget = GL_TEXTURE_2D;
#else
	GLenum texTarget = gtaglIsVersionSupported(3, 1) ? GL_TEXTURE_RECTANGLE : GL_TEXTURE_2D;
#endif

	blendLayerProgram->makeCurrent();

	glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);

	gtaglBindFramebuffer(GTAGL_FRAMEBUFFER, 0);
	//gtaglBindFramebuffer(GTAGL_FRAMEBUFFER, mainFBO);

	// Render a screen-size plane with the contents of the output color buffer
	glBindBuffer(GL_ARRAY_BUFFER, planeDataBuf);
	glEnableVertexAttribArray(blendLayerVertexAttrib);
	glVertexAttribPointer(blendLayerVertexAttrib, 2, GL_FLOAT, GL_FALSE, 0, (void*) 0);

	glBindTexture(texTarget, colorTexes[outputIdx]);
	glUniform1i(blendLayerTexUniform, 0);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	return ++currentPass < numPasses;
}


void DepthPeelingAlgorithm::performTransparentRenderFinalization()
{
	// ********** Blend the contents of dpBlendFBO back into the window-system framebuffer **********

#ifdef GTA_USE_OPENGL_ES
	GLenum texTarget = GL_TEXTURE_2D;
#else
	GLenum texTarget = gtaglIsVersionSupported(3, 1) ? GL_TEXTURE_RECTANGLE : GL_TEXTURE_2D;
#endif

	blendFinalProgram->makeCurrent();

	// We still blend front-to-back here, so we'll still use the old blend functions

	gtaglBindFramebuffer(GTAGL_FRAMEBUFFER, 0);

	//glClear(GL_COLOR_BUFFER_BIT);

	glBindBuffer(GL_ARRAY_BUFFER, planeDataBuf);
	glEnableVertexAttribArray(blendFinalVertexAttrib);
	glVertexAttribPointer(blendFinalVertexAttrib, 2, GL_FLOAT, GL_FALSE, 0, (void*) 0);

	glBindTexture(texTarget, mainTexes[0]);
	glUniform1i(blendFinalTexUniform, 0);

	//glDisable(GL_DEPTH_TEST);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	//glEnable(GL_DEPTH_TEST);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	gtaglBindFramebuffer(GTAGL_FRAMEBUFFER, 0);

	//gtaglBindFramebuffer(GTAGL_FRAMEBUFFER, mainFBO);
	//GLDebugSavePixels("/home/alemariusnexus/fbo.bin", 0, 0, 512, 512);
	//exit(0);
}

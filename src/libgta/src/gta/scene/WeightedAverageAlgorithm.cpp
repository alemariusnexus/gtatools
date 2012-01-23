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

#include "WeightedAverageAlgorithm.h"
#include "../Engine.h"

#include <res_wavg_vertex_shader.h>
#include <res_wavg_fragment_shader.h>
#include <res_wavg_final_vertex_shader.h>
#include <res_wavg_final_fragment_shader.h>




#ifndef GTA_USE_OPENGL_ES

inline void wavgglGenFramebuffersCORE(GLsizei p1, GLuint* p2) { glGenFramebuffers(p1, p2); }
inline void wavgglBindFramebufferCORE(GLenum p1, GLuint p2) { glBindFramebuffer(p1, p2); }
inline void wavgglFramebufferTexture2DCORE(GLenum p1, GLenum p2, GLenum p3, GLuint p4, GLint p5)
		{ glFramebufferTexture2D(p1, p2, p3, p4, p5); }

inline void wavgglGenFramebuffersEXT(GLsizei p1, GLuint* p2) { glGenFramebuffersEXT(p1, p2); }
inline void wavgglBindFramebufferEXT(GLenum p1, GLuint p2) { glBindFramebufferEXT(p1, p2); }
inline void wavgglFramebufferTexture2DEXT(GLenum p1, GLenum p2, GLenum p3, GLuint p4, GLint p5)
		{ glFramebufferTexture2DEXT(p1, p2, p3, p4, p5); }

#endif


void (*wavgglGenFramebuffers)(GLsizei, GLuint*);
void (*wavgglBindFramebuffer)(GLenum, GLuint);
void (*wavgglFramebufferTexture2D)(GLenum, GLenum, GLenum, GLuint, GLint);

GLenum WAVGGL_FRAMEBUFFER;
GLenum WAVGGL_COLOR_ATTACHMENT0;
GLenum WAVGGL_COLOR_ATTACHMENT1;
GLenum WAVGGL_COLOR_ATTACHMENT2;
GLenum WAVGGL_COLOR_ATTACHMENT3;
GLenum WAVGGL_DEPTH_ATTACHMENT;



void WeightedAverageAlgorithm::install()
{
	if (!inited) {
		Engine* engine = Engine::getInstance();

		const char* vertexShaderData;
		const char* fragmentShaderData;
		const char* finalVertexShaderData;
		const char* finalFragmentShaderData;

		int vertexShaderDataLen;
		int fragmentShaderDataLen;
		int finalVertexShaderDataLen;
		int finalFragmentShaderDataLen;

		vertexShaderData				= (const char*) res_wavg_vertex_shader_data;
		fragmentShaderData				= (const char*) res_wavg_fragment_shader_data;
		finalVertexShaderData			= (const char*) res_wavg_final_vertex_shader_data;
		finalFragmentShaderData			= (const char*) res_wavg_final_fragment_shader_data;

		vertexShaderDataLen					= sizeof(res_wavg_vertex_shader_data);
		fragmentShaderDataLen				= sizeof(res_wavg_fragment_shader_data);
		finalVertexShaderDataLen			= sizeof(res_wavg_final_vertex_shader_data);
		finalFragmentShaderDataLen			= sizeof(res_wavg_final_fragment_shader_data);

		wavgglGenFramebuffers = &wavgglGenFramebuffersCORE;
		wavgglBindFramebuffer = &wavgglBindFramebufferCORE;
		wavgglFramebufferTexture2D = &wavgglFramebufferTexture2DCORE;

		WAVGGL_FRAMEBUFFER = GL_FRAMEBUFFER;
		WAVGGL_COLOR_ATTACHMENT0 = GL_COLOR_ATTACHMENT0;
		WAVGGL_COLOR_ATTACHMENT1 = GL_COLOR_ATTACHMENT1;
		WAVGGL_COLOR_ATTACHMENT2 = GL_COLOR_ATTACHMENT2;
		WAVGGL_COLOR_ATTACHMENT3 = GL_COLOR_ATTACHMENT3;
		WAVGGL_DEPTH_ATTACHMENT = GL_DEPTH_ATTACHMENT;


		vertexShader = new Shader(GL_VERTEX_SHADER,
				"Weighted Average Transparency Algorithm Main Vertex Shader");
		vertexShader->loadSourceCode(vertexShaderData, vertexShaderDataLen);
		vertexShader->compile();

		fragmentShader = new Shader(GL_FRAGMENT_SHADER,
				"Weighted Average Transparency Algorithm Main Fragment Shader");
		fragmentShader->loadSourceCode(fragmentShaderData, fragmentShaderDataLen);
		fragmentShader->compile();

		finalVertexShader = new Shader(GL_VERTEX_SHADER,
				"Weighted Average Transparency Algorithm Final Vertex Shader");
		finalVertexShader->loadSourceCode(finalVertexShaderData, finalVertexShaderDataLen);
		finalVertexShader->compile();

		finalFragmentShader = new Shader(GL_FRAGMENT_SHADER,
				"Weighted Average Transparency Algorithm Final Fragment Shader");
		finalFragmentShader->loadSourceCode(finalFragmentShaderData, finalFragmentShaderDataLen);
		finalFragmentShader->compile();

		/*wavgProgram = new ShaderProgram;
		wavgProgram->attachShader(wavgVertexShader);
		wavgProgram->attachShader(wavgFragmentShader);
		wavgProgram->attachShader(shadeVertexShader);
		wavgProgram->attachShader(shadeFragmentShader);
		wavgProgram->link();*/

		blendFinalProgram = new ShaderProgram("Weighted Average Transparency Algorithm Final Shader Program");
		blendFinalProgram->attachShader(finalVertexShader);
		blendFinalProgram->attachShader(finalFragmentShader);
		blendFinalProgram->link();


		wavgglGenFramebuffers(1, &fbo);

		glGenTextures(1, &colorTex);
		glGenTextures(1, &depthComplexityTex);
		glGenTextures(1, &depthBufferTex);



		viewW = engine->getViewportWidth();
		viewH = engine->getViewportHeight();

#ifndef GTA_USE_OPENGL_ES
		if (gtaglIsVersionSupported(3, 1)) {
			wavgglBindFramebuffer(WAVGGL_FRAMEBUFFER, fbo);

			glBindTexture(GL_TEXTURE_RECTANGLE, colorTex);
			glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGBA16F, viewW, viewH, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
			wavgglFramebufferTexture2D(WAVGGL_FRAMEBUFFER, WAVGGL_COLOR_ATTACHMENT0, GL_TEXTURE_RECTANGLE, colorTex, 0);

			glBindTexture(GL_TEXTURE_RECTANGLE, depthComplexityTex);
			glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGBA16F, viewW, viewH, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
			wavgglFramebufferTexture2D(WAVGGL_FRAMEBUFFER, WAVGGL_COLOR_ATTACHMENT1, GL_TEXTURE_RECTANGLE,
					depthComplexityTex, 0);

			glBindTexture(GL_TEXTURE_RECTANGLE, depthBufferTex);
			glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_DEPTH_COMPONENT32, viewW, viewH, 0, GL_DEPTH_COMPONENT, GL_FLOAT,
					NULL);
			wavgglFramebufferTexture2D(WAVGGL_FRAMEBUFFER, WAVGGL_DEPTH_ATTACHMENT, GL_TEXTURE_RECTANGLE,
					depthBufferTex, 0);
		} else {
			wavgglBindFramebuffer(WAVGGL_FRAMEBUFFER, 0);

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


			wavgglBindFramebuffer(WAVGGL_FRAMEBUFFER, fbo);

			glBindTexture(GL_TEXTURE_2D, depthBufferTex);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexImage2D(GL_TEXTURE_2D, 0, depthFormat, viewW, viewH, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT,
					NULL);
			wavgglFramebufferTexture2D(WAVGGL_FRAMEBUFFER, WAVGGL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
					depthBufferTex, 0);

			glBindTexture(GL_TEXTURE_2D, colorTex);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, viewW, viewH, 0, GL_RGBA, GL_FLOAT, NULL);
			wavgglFramebufferTexture2D(WAVGGL_FRAMEBUFFER, WAVGGL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTex, 0);

			glBindTexture(GL_TEXTURE_2D, depthComplexityTex);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, viewW, viewH, 0, GL_RGBA, GL_FLOAT, NULL);
			wavgglFramebufferTexture2D(WAVGGL_FRAMEBUFFER, WAVGGL_COLOR_ATTACHMENT1, GL_TEXTURE_2D,
					depthComplexityTex, 0);
		}
#else
		wavgglBindFramebuffer(WAVGGL_FRAMEBUFFER, 0);

		// Query the size of the window-system depth buffer and try to choose the same format for the FBO
		// depth buffers. This is needed because glBlitFramebufferEXT generates an error if the source and
		// destination buffers aren't of the same format.
		GLint depthSize;
		glGetIntegerv(GL_DEPTH_BITS, &depthSize);

		GLint depthFormat;
		GLint dataType;

		switch (depthSize) {
		case 16:
			depthFormat = GL_DEPTH_COMPONENT;
			dataType = GL_UNSIGNED_SHORT;
			break;
		case 24:
			depthFormat = GL_DEPTH_COMPONENT24_OES;
			dataType = GL_UNSIGNED_INT;
			break;
		case 32:
			depthFormat = GL_DEPTH_COMPONENT32_OES;
			dataType = GL_UNSIGNED_INT;
			break;
		default:
			fprintf(stderr, "WARNING: Window-system depth buffer size is unsupported for depth peeling: %d",
					depthSize);
			depthFormat = GL_DEPTH_COMPONENT;
		}


		wavgglBindFramebuffer(WAVGGL_FRAMEBUFFER, fbo);

		glBindTexture(GL_TEXTURE_2D, depthBufferTex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, depthFormat, viewW, viewH, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT,
				NULL);

		glBindTexture(GL_TEXTURE_2D, depthComplexityTex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, viewW, viewH, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT,
				NULL);
		wavgglFramebufferTexture2D(WAVGGL_FRAMEBUFFER, WAVGGL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
				depthComplexityTex, 0);

		glBindTexture(GL_TEXTURE_2D, colorTex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, viewW, viewH, 0, GL_RGBA, GL_FLOAT, NULL);
		wavgglFramebufferTexture2D(WAVGGL_FRAMEBUFFER, WAVGGL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTex, 0);
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

		inited = true;
	}
}


void WeightedAverageAlgorithm::installShaders(ShaderProgram* program)
{
	program->attachShader(vertexShader);
	program->attachShader(fragmentShader);
}


void WeightedAverageAlgorithm::uninstallShaders(ShaderProgram* program)
{
	program->detachShader(vertexShader);
	program->detachShader(fragmentShader);
}


void WeightedAverageAlgorithm::performPostLinkOperations()
{
	colorTexUniform = blendFinalProgram->getUniformLocation("ColorTex");
	dcTexUniform = blendFinalProgram->getUniformLocation("DepthComplexityTex");
	finalVertexAttrib = blendFinalProgram->getAttributeLocation("Vertex");
}


bool WeightedAverageAlgorithm::performPerFrameInit()
{
	//glCopyTexImage2D(texTarget, 0, GL_DEPTH_COMPONENT24_OES, 0, 0, viewW, viewH, 0);

#ifndef GTA_USE_OPENGL_ES
	GLenum texTarget = gtaglIsVersionSupported(3, 1) ? GL_TEXTURE_RECTANGLE : GL_TEXTURE_2D;

	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);

	glEnable(GL_DEPTH_TEST);

	glBindTexture(texTarget, depthBufferTex);

	glCopyTexImage2D(texTarget, 0, GL_DEPTH_COMPONENT24, 0, 0, viewW, viewH, 0);

	wavgglBindFramebuffer(WAVGGL_FRAMEBUFFER, fbo);

	glDepthMask(GL_FALSE);

	GLenum drawBufs[] = {WAVGGL_COLOR_ATTACHMENT0, WAVGGL_COLOR_ATTACHMENT1};
	glDrawBuffers(2, drawBufs);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);
#else
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);

	glDisable(GL_DEPTH_TEST);

	glActiveTexture(GL_TEXTURE0);

	glBindTexture(GL_TEXTURE_2D, depthBufferTex);
	glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 0, 0, viewW, viewH, 0); // TODO Does this work without a depth size?

	glActiveTexture(GL_TEXTURE0);

	wavgglBindFramebuffer(WAVGGL_FRAMEBUFFER, fbo);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepthf(0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearDepthf(1.0f);
#endif

	return true;
}


void WeightedAverageAlgorithm::setupShaderUniforms(ShaderProgram* program)
{
#ifdef GTA_USE_OPENGL_ES
	GLint depthTexUniform = program->getUniformLocation("DepthTex");
	GLint texDimensionsUniform = program->getUniformLocation("TexDimensions");

	glUniform1i(depthTexUniform, 1);
	glUniform2i(texDimensionsUniform, viewW, viewH);
#endif
}


void WeightedAverageAlgorithm::performPerFrameFinalization()
{
#ifdef GTA_USE_OPENGL_ES
	GLenum texTarget = GL_TEXTURE_2D;
#else
	GLenum texTarget = gtaglIsVersionSupported(3, 1) ? GL_TEXTURE_RECTANGLE : GL_TEXTURE_2D;
#endif

	blendFinalProgram->makeCurrent();

	wavgglBindFramebuffer(WAVGGL_FRAMEBUFFER, 0);

	glDisable(GL_DEPTH_TEST);

	glBlendFunc(GL_ONE, GL_SRC_ALPHA);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(texTarget, colorTex);
	glUniform1i(colorTexUniform, 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(texTarget, depthComplexityTex);
	glUniform1i(dcTexUniform, 1);

	glBindBuffer(GL_ARRAY_BUFFER, planeDataBuf);
	glEnableVertexAttribArray(finalVertexAttrib);
	glVertexAttribPointer(finalVertexAttrib, 2, GL_FLOAT, GL_FALSE, 0, (void*) 0);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

#ifndef GTA_USE_OPENGL_ES
	GLenum drawBuf = GL_BACK_LEFT;
	glDrawBuffers(1, &drawBuf);
#endif

	glActiveTexture(GL_TEXTURE0);

	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

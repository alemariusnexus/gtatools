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
#include <res_vertex_default_shader.h>
#include <res_fragment_default_shader.h>
#include <res_shade_vertex_shader.h>
#include <res_shade_fragment_shader.h>
#include <res_vertex_dc_shader.h>
#include <res_fragment_dc_shader.h>
#include <res_dp_blend_layer_vertex_shader.h>
#include <res_dp_blend_layer_fragment_shader.h>
#include <res_dp_blend_final_vertex_shader.h>
#include <res_dp_blend_final_fragment_shader.h>
#include <res_wavg_vertex_shader.h>
#include <res_wavg_fragment_shader.h>
#include <res_wavg_final_vertex_shader.h>
#include <res_wavg_final_fragment_shader.h>



DefaultRenderer::DefaultRenderer()
		: transMode(WeightedAverage), transPassCount(5)
{
	Engine* engine = Engine::getInstance();

	// Insert a dummy. This makes handling of the alphaBegin iterator easier
	objs.push_back(NULL);

	alphaBegin = objs.begin();

	Shader* vertexDefaultShader = new Shader(GL_VERTEX_SHADER);
	vertexDefaultShader->loadSourceCode((const char*) res_vertex_default_shader_data,
			sizeof(res_vertex_default_shader_data));
	vertexDefaultShader->compile();

	Shader* fragmentDefaultShader = new Shader(GL_FRAGMENT_SHADER);
	fragmentDefaultShader->loadSourceCode((const char*) res_fragment_default_shader_data,
			sizeof(res_fragment_default_shader_data));
	fragmentDefaultShader->compile();

	Shader* shadeVertexShader = new Shader(GL_VERTEX_SHADER);
	shadeVertexShader->loadSourceCode((const char*) res_shade_vertex_shader_data,
			sizeof(res_shade_vertex_shader_data));
	shadeVertexShader->compile();

	Shader* shadeFragmentShader = new Shader(GL_FRAGMENT_SHADER);
	shadeFragmentShader->loadSourceCode((const char*) res_shade_fragment_shader_data,
			sizeof(res_shade_fragment_shader_data));
	shadeFragmentShader->compile();

	Shader* vertexDcShader = new Shader(GL_VERTEX_SHADER);
	vertexDcShader->loadSourceCode((const char*) res_vertex_dc_shader_data,
			sizeof(res_vertex_dc_shader_data));
	vertexDcShader->compile();

	Shader* fragmentDcShader = new Shader(GL_FRAGMENT_SHADER);
	fragmentDcShader->loadSourceCode((const char*) res_fragment_dc_shader_data,
			sizeof(res_fragment_dc_shader_data));
	fragmentDcShader->compile();

	Shader* dpBlendLayerVertexShader = new Shader(GL_VERTEX_SHADER);
	dpBlendLayerVertexShader->loadSourceCode((const char*) res_dp_blend_layer_vertex_shader_data,
			sizeof(res_dp_blend_layer_vertex_shader_data));
	dpBlendLayerVertexShader->compile();

	Shader* dpBlendLayerFragmentShader = new Shader(GL_FRAGMENT_SHADER);
	dpBlendLayerFragmentShader->loadSourceCode((const char*) res_dp_blend_layer_fragment_shader_data,
			sizeof(res_dp_blend_layer_fragment_shader_data));
	dpBlendLayerFragmentShader->compile();

	Shader* dpBlendFinalVertexShader = new Shader(GL_VERTEX_SHADER);
	dpBlendFinalVertexShader->loadSourceCode((const char*) res_dp_blend_final_vertex_shader_data,
			sizeof(res_dp_blend_final_vertex_shader_data));
	dpBlendFinalVertexShader->compile();

	Shader* dpBlendFinalFragmentShader = new Shader(GL_FRAGMENT_SHADER);
	dpBlendFinalFragmentShader->loadSourceCode((const char*) res_dp_blend_final_fragment_shader_data,
			sizeof(res_dp_blend_final_fragment_shader_data));
	dpBlendFinalFragmentShader->compile();

	Shader* wavgVertexShader = new Shader(GL_VERTEX_SHADER);
	wavgVertexShader->loadSourceCode((const char*) res_wavg_vertex_shader_data,
			sizeof(res_wavg_vertex_shader_data));
	wavgVertexShader->compile();

	Shader* wavgFragmentShader = new Shader(GL_FRAGMENT_SHADER);
	wavgFragmentShader->loadSourceCode((const char*) res_wavg_fragment_shader_data,
			sizeof(res_wavg_fragment_shader_data));
	wavgFragmentShader->compile();

	Shader* wavgFinalVertexShader = new Shader(GL_VERTEX_SHADER);
	wavgFinalVertexShader->loadSourceCode((const char*) res_wavg_final_vertex_shader_data,
			sizeof(res_wavg_final_vertex_shader_data));
	wavgFinalVertexShader->compile();

	Shader* wavgFinalFragmentShader = new Shader(GL_FRAGMENT_SHADER);
	wavgFinalFragmentShader->loadSourceCode((const char*) res_wavg_final_fragment_shader_data,
			sizeof(res_wavg_final_fragment_shader_data));
	wavgFinalFragmentShader->compile();

	defaultProgram = new ShaderProgram;
	defaultProgram->attachShader(vertexDefaultShader);
	defaultProgram->attachShader(fragmentDefaultShader);
	defaultProgram->attachShader(shadeVertexShader);
	defaultProgram->attachShader(shadeFragmentShader);
	defaultProgram->link();

	dpProgram = new ShaderProgram;
	dpProgram->attachShader(vertexDcShader);
	dpProgram->attachShader(fragmentDcShader);
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

	glGenFramebuffers(2, dpFBOs);
	glGenFramebuffers(1, &dpBlendFBO);
	glGenFramebuffers(1, &wavgFBO);

	glGenTextures(2, dpDepthTexes);
	glGenTextures(2, dpColorTexes);
	glGenTextures(1, &dpBlendTex);
	glGenTextures(1, &wavgColorTex);
	glGenTextures(1, &wavgDepthComplexityTex);
	glGenTextures(1, &wavgDepthBufferTex);


	int viewW = engine->getViewportWidth();
	int viewH = engine->getViewportHeight();

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dpFBOs[0]);

	// Initialize the depth texture for FBO 0
	glBindTexture(GL_TEXTURE_RECTANGLE, dpDepthTexes[0]);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_DEPTH_COMPONENT32, viewW, viewH, 0, GL_DEPTH_COMPONENT, GL_FLOAT,
			NULL);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_RECTANGLE,
			dpDepthTexes[0], 0);

	// Initialize the color texture for FBO 0
	glBindTexture(GL_TEXTURE_RECTANGLE, dpColorTexes[0]);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGBA, viewW, viewH, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_RECTANGLE, dpColorTexes[0], 0);



	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dpFBOs[1]);

	// Initialize the depth texture for FBO 1
	glBindTexture(GL_TEXTURE_RECTANGLE, dpDepthTexes[1]);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_DEPTH_COMPONENT32, viewW, viewH, 0, GL_DEPTH_COMPONENT, GL_FLOAT,
			NULL);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_RECTANGLE,
			dpDepthTexes[1], 0);

	// Initialize the color texture for FBO 1
	glBindTexture(GL_TEXTURE_RECTANGLE, dpColorTexes[1]);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGBA, viewW, viewH, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_RECTANGLE, dpColorTexes[1], 0);



	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dpBlendFBO);

	// Initialize the color texture for the blending FBO
	glBindTexture(GL_TEXTURE_RECTANGLE, dpBlendTex);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGBA, viewW, viewH, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_RECTANGLE, dpBlendTex, 0);



	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, wavgFBO);

	GLException::checkError("Frankophobia castle 1");

	glBindTexture(GL_TEXTURE_RECTANGLE, wavgColorTex);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGBA16F, viewW, viewH, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_RECTANGLE, wavgColorTex, 0);

	GLException::checkError("Frankophobia castle 2");

	glBindTexture(GL_TEXTURE_RECTANGLE, wavgDepthComplexityTex);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGBA16F, viewW, viewH, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_RECTANGLE,
			wavgDepthComplexityTex, 0);

	glBindTexture(GL_TEXTURE_RECTANGLE, wavgDepthBufferTex);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_DEPTH_COMPONENT32, viewW, viewH, 0, GL_DEPTH_COMPONENT, GL_FLOAT,
			NULL);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_RECTANGLE,
			wavgDepthBufferTex, 0);

	GLException::checkError("Frankophobia castle 3");



	glBindFramebuffer(GL_FRAMEBUFFER, 0);



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

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	ObjectList::iterator it;
	ObjectList::iterator beg = objs.begin();
	ObjectList::iterator end = objs.end();

	GLException::checkError("Frank -2");

	for (it = beg ; it != alphaBegin ; it++) {
		SceneObject* obj = *it;

		int type = obj->getType();

		if (type == SceneObjectStatic) {
			renderStaticSceneObject((StaticSceneObject*) obj, mvpMatrix);
		}
	}

	GLException::checkError("Frank -1");


	if (transMode == DepthPeeling) {
		// ******************************************************
		// *					DEPTH PEELING					*
		// ******************************************************

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

		GLuint depthTexUniform = dpProgram->getUniformLocation("DepthTex");

		// Clear the blending FBO to (0, 0, 0, 1). Exactly this value is needed for the blending equations
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dpBlendFBO);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// Clear input depth buffer to 0 (so that no fragment will be initially peeled away)
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dpFBOs[1]);
		glClearDepth(0.0);
		glClear(GL_DEPTH_BUFFER_BIT);
		glClearDepth(1.0);

		glBlendFuncSeparate(GL_DST_ALPHA, GL_ONE, GL_ZERO, GL_ONE_MINUS_SRC_ALPHA);

		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

		GLException::checkError("Frank 0");

		// Do the render passes...
		for (int i = 0 ; i < transPassCount ; i++) {
			int inputIdx = (i+1) % 2; // In the first pass, this will be 1
			int outputIdx = i % 2; // In the first pass, this will be 0

			glDisable(GL_BLEND);
			glEnable(GL_DEPTH_TEST);

			GLException::checkError("Frank 0.1");

			// ********** Peel a single layer **********

			dpProgram->makeCurrent();
			glEnableVertexAttribArray(vertexAttrib);

			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dpFBOs[outputIdx]);

			glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

			GLException::checkError("Frank 0.2.1");

			// Clear the output color buffer and copy the opaque depth buffer to the output depth buffer
			glClear(GL_COLOR_BUFFER_BIT);
			GLException::checkError("Frank 0.2.2");
			glBlitFramebuffer(0, 0, viewW, viewH, 0, 0, viewW, viewH, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
			GLException::checkError("Frank 0.2.3");

			// Bind the input depth texture to texture unit 1 (unit 0 is used by the render* methods)
			glActiveTexture(GL_TEXTURE1);
			GLException::checkError("Frank 0.2.4");
			glBindTexture(GL_TEXTURE_RECTANGLE, dpDepthTexes[inputIdx]);
			GLException::checkError("Frank 0.2.5");
			glUniform1i(depthTexUniform, 1);

			GLException::checkError("Frank 0.3");

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

			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dpBlendFBO);

			glBindBuffer(GL_ARRAY_BUFFER, planeDataBuf);
			glEnableVertexAttribArray(dpBlendLayerVertexAttrib);
			glVertexAttribPointer(dpBlendLayerVertexAttrib, 2, GL_FLOAT, GL_FALSE, 0, (void*) 0);

			glBindTexture(GL_TEXTURE_RECTANGLE, dpColorTexes[outputIdx]);
			glUniform1i(dpBlendLayerTexUniform, 0);

			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		}

		GLException::checkError("Frank 1");

		dpBlendFinalProgram->makeCurrent();

		//glDisable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_SRC_ALPHA);

		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

		glBindBuffer(GL_ARRAY_BUFFER, planeDataBuf);
		glEnableVertexAttribArray(dpBlendFinalVertexAttrib);
		glVertexAttribPointer(dpBlendFinalVertexAttrib, 2, GL_FLOAT, GL_FALSE, 0, (void*) 0);

		glBindTexture(GL_TEXTURE_RECTANGLE, dpBlendTex);
		glUniform1i(dpBlendFinalTexUniform, 0);

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	} else if (transMode == WeightedAverage) {
		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE);

		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_FALSE);

		GLException::checkError("Frankophilia castle 1");

		glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, wavgFBO);
		glBlitFramebuffer(0, 0, viewW, viewH, 0, 0, viewW, viewH, GL_DEPTH_BUFFER_BIT, GL_NEAREST);

		//glFrameTerminatorGREMEDY();

		GLException::checkError("Frankophilia castle 1.1");

		GLenum drawBufs[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
		glDrawBuffers(2, drawBufs);

		GLException::checkError("Frankophilia castle 1.2");

		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		GLException::checkError("Frankophilia castle 1.3");

		wavgProgram->makeCurrent();

		GLException::checkError("Frankophilia castle 2");

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

		GLException::checkError("Frankophilia castle 3");

		for (it = alphaBegin ; it != end ; it++) {
			SceneObject* obj = *it;

			int type = obj->getType();

			if (type == SceneObjectStatic) {
				renderStaticSceneObject((StaticSceneObject*) obj, mvpMatrix);
			}
		}

		glDisable(GL_DEPTH_TEST);

		//glFrameTerminatorGREMEDY();

		GLException::checkError("Frankophilia castle 4");

		wavgBlendFinalProgram->makeCurrent();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glBlendFunc(GL_ONE, GL_SRC_ALPHA);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_RECTANGLE, wavgColorTex);
		glUniform1i(colorTexUniform, 0);

		GLException::checkError("Frankophilia castle 5");

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_RECTANGLE, wavgDepthComplexityTex);
		glUniform1i(dcTexUniform, 1);

		GLException::checkError("Frankophilia castle 6");

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

	GLException::checkError("Frank 2");

	objs.clear();

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

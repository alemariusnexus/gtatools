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

#include "DefaultRenderer.h"
#include "../resource/mesh/Mesh.h"
#include <gtaformats/util/util.h>
#include "DepthPeelingAlgorithm.h"
#include "WeightedAverageAlgorithm.h"
#include "BasicTransparencyAlgorithm.h"
#include "../Animator.h"

#include <res_glsl110_vertex_default_shader.h>
#include <res_glsl110_fragment_default_shader.h>
#include <res_glsl110_shade_vertex_shader.h>
#include <res_glsl110_shade_fragment_shader.h>

#include <res_glsl140_vertex_default_shader.h>
#include <res_glsl140_fragment_default_shader.h>
#include <res_glsl140_shade_vertex_shader.h>
#include <res_glsl140_shade_fragment_shader.h>
#include <res_glsl140_anim_shade_vertex_shader.h>
#include <res_glsl140_anim_shade_fragment_shader.h>

#include <res_glsles2_vertex_default_shader.h>
#include <res_glsles2_fragment_default_shader.h>
#include <res_glsles2_shade_vertex_shader.h>
#include <res_glsles2_shade_fragment_shader.h>





DefaultRenderer::DefaultRenderer()
		: transAlgo(NULL)
{
	Engine* engine = Engine::getInstance();

	// Insert a dummy. This makes handling of the alphaBegin iterator easier
	staticObjs.push_back(NULL);
	animObjs.push_back(NULL);

	staticAlphaBegin = staticObjs.begin();
	animAlphaBegin = animObjs.begin();

	const char* vertexDefaultData;
	const char* fragmentDefaultData;
	const char* shadeVertexShaderData;
	const char* shadeFragmentShaderData;
	const char* animShadeVertexShaderData;
	const char* animShadeFragmentShaderData;

	int vertexDefaultDataLen;
	int fragmentDefaultDataLen;
	int shadeVertexShaderDataLen;
	int shadeFragmentShaderDataLen;
	int animShadeVertexShaderDataLen;
	int animShadeFragmentShaderDataLen;

#ifdef GTA_USE_OPENGL_ES
	vertexDefaultData					= (const char*) res_glsles2_vertex_default_shader_data;
	fragmentDefaultData					= (const char*) res_glsles2_fragment_default_shader_data;
	shadeVertexShaderData				= (const char*) res_glsles2_shade_vertex_shader_data;
	shadeFragmentShaderData				= (const char*) res_glsles2_shade_fragment_shader_data;

	vertexDefaultDataLen					= sizeof(res_glsles2_vertex_default_shader_data);
	fragmentDefaultDataLen					= sizeof(res_glsles2_fragment_default_shader_data);
	shadeVertexShaderDataLen				= sizeof(res_glsles2_shade_vertex_shader_data);
	shadeFragmentShaderDataLen				= sizeof(res_glsles2_shade_fragment_shader_data);
#else
	if (gtaglIsVersionSupported(3, 1)) {
		vertexDefaultData					= (const char*) res_glsl140_vertex_default_shader_data;
		fragmentDefaultData					= (const char*) res_glsl140_fragment_default_shader_data;
		shadeVertexShaderData				= (const char*) res_glsl140_shade_vertex_shader_data;
		shadeFragmentShaderData				= (const char*) res_glsl140_shade_fragment_shader_data;
		animShadeVertexShaderData			= (const char*) res_glsl140_anim_shade_vertex_shader_data;
		animShadeFragmentShaderData			= (const char*) res_glsl140_anim_shade_fragment_shader_data;

		vertexDefaultDataLen					= sizeof(res_glsl140_vertex_default_shader_data);
		fragmentDefaultDataLen					= sizeof(res_glsl140_fragment_default_shader_data);
		shadeVertexShaderDataLen				= sizeof(res_glsl140_shade_vertex_shader_data);
		shadeFragmentShaderDataLen				= sizeof(res_glsl140_shade_fragment_shader_data);
		animShadeVertexShaderDataLen			= sizeof(res_glsl140_anim_shade_vertex_shader_data);
		animShadeFragmentShaderDataLen			= sizeof(res_glsl140_anim_shade_fragment_shader_data);
	} else {
		vertexDefaultData					= (const char*) res_glsl110_vertex_default_shader_data;
		fragmentDefaultData					= (const char*) res_glsl110_fragment_default_shader_data;
		shadeVertexShaderData				= (const char*) res_glsl110_shade_vertex_shader_data;
		shadeFragmentShaderData				= (const char*) res_glsl110_shade_fragment_shader_data;

		vertexDefaultDataLen					= sizeof(res_glsl110_vertex_default_shader_data);
		fragmentDefaultDataLen					= sizeof(res_glsl110_fragment_default_shader_data);
		shadeVertexShaderDataLen				= sizeof(res_glsl110_shade_vertex_shader_data);
		shadeFragmentShaderDataLen				= sizeof(res_glsl110_shade_fragment_shader_data);
	}
#endif

	shadeVertexShader = new Shader(GL_VERTEX_SHADER, "Static Vertex Shader");
	shadeVertexShader->loadSourceCode(shadeVertexShaderData, shadeVertexShaderDataLen);
	shadeVertexShader->compile();

	shadeFragmentShader = new Shader(GL_FRAGMENT_SHADER, "Static Fragment Shader");
	shadeFragmentShader->loadSourceCode(shadeFragmentShaderData, shadeFragmentShaderDataLen);
	shadeFragmentShader->compile();

	vertexDefaultShader = new Shader(GL_VERTEX_SHADER, "Default Opaque Vertex Shader");
	vertexDefaultShader->loadSourceCode(vertexDefaultData, vertexDefaultDataLen);
	vertexDefaultShader->compile();

	fragmentDefaultShader = new Shader(GL_FRAGMENT_SHADER, "Default Opaque Fragment Shader");
	fragmentDefaultShader->loadSourceCode(fragmentDefaultData, fragmentDefaultDataLen);
	fragmentDefaultShader->compile();

	animShadeVertexShader = new Shader(GL_VERTEX_SHADER, "Animated Vertex Shader");
	animShadeVertexShader->loadSourceCode(animShadeVertexShaderData, animShadeVertexShaderDataLen);
	animShadeVertexShader->compile();

	animShadeFragmentShader = new Shader(GL_FRAGMENT_SHADER, "Animated Fragment Shader");
	animShadeFragmentShader->loadSourceCode(animShadeFragmentShaderData, animShadeFragmentShaderDataLen);
	animShadeFragmentShader->compile();

	opaqueStaticProgram = new ShaderProgram("Opaque Static Shader Program");
	opaqueStaticProgram->attachShader(shadeVertexShader);
	opaqueStaticProgram->attachShader(shadeFragmentShader);
	opaqueStaticProgram->attachShader(vertexDefaultShader);
	opaqueStaticProgram->attachShader(fragmentDefaultShader);
	opaqueStaticProgram->link();

	transStaticProgram = new ShaderProgram("Transparent Static Shader Program");
	transStaticProgram->attachShader(shadeVertexShader);
	transStaticProgram->attachShader(shadeFragmentShader);


	opaqueAnimProgram = new ShaderProgram("Opaque Animated Shader Program");
	opaqueAnimProgram->attachShader(animShadeVertexShader);
	opaqueAnimProgram->attachShader(animShadeFragmentShader);
	opaqueAnimProgram->attachShader(vertexDefaultShader);
	opaqueAnimProgram->attachShader(fragmentDefaultShader);
	opaqueAnimProgram->link();

	transAnimProgram = new ShaderProgram("Transparent Animated Shader Program");
	transAnimProgram->attachShader(animShadeVertexShader);
	transAnimProgram->attachShader(animShadeFragmentShader);


	GLException::checkError("Error Frank 1");


	gtaglBindFramebuffer(GTAGL_FRAMEBUFFER, 0);
}


void DefaultRenderer::setTransparencyAlgorithm(TransparencyAlgorithm* algo)
{
	if (transAlgo) {
		transAlgo->uninstallShaders(transStaticProgram);
		transAlgo->uninstallShaders(transAnimProgram);

		transAlgo->uninstall();
	}

	algo->install();

	algo->installShaders(transStaticProgram);
	algo->installShaders(transAnimProgram);
	transStaticProgram->link();
	transAnimProgram->link();

	algo->performPostLinkOperations();

	transAlgo = algo;

	gtaglBindFramebuffer(GTAGL_FRAMEBUFFER, 0);
}


void DefaultRenderer::enqueueForRendering(VisualSceneObject* obj)
{
	int type = obj->getType();

	if (type == SceneObjectStatic) {
		if (obj->hasAlphaTransparency())
			staticObjs.push_back((StaticSceneObject*) obj);
		else
			staticObjs.insert(staticAlphaBegin, (StaticSceneObject*) obj);
	} else if (type == SceneObjectAnimated) {
		if (obj->hasAlphaTransparency())
			animObjs.push_back((AnimatedSceneObject*) obj);
		else
			animObjs.insert(animAlphaBegin, (AnimatedSceneObject*) obj);
	}
}


void DefaultRenderer::render()
{
	// Erase the dummy
	staticAlphaBegin = staticObjs.erase(staticAlphaBegin);
	animAlphaBegin = animObjs.erase(animAlphaBegin);

	Engine* engine = Engine::getInstance();

	int viewW = engine->getViewportWidth();
	int viewH = engine->getViewportHeight();

	Camera* camera = engine->getCamera();

	Matrix4 mvpMatrix = engine->getProjectionMatrix();
	mvpMatrix *= Matrix4::lookAt(camera->getTarget(), camera->getUp());
	const Vector3& cpos = camera->getPosition();
	mvpMatrix.translate(-cpos);

	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);

	gtaglBindFramebuffer(GTAGL_FRAMEBUFFER, 0);

	transAlgo->performOpaqueRenderInit();

	opaqueStaticProgram->makeCurrent();

	mvpMatrixUniform = opaqueStaticProgram->getUniformLocation("MVPMatrix");
	texturedUniform = opaqueStaticProgram->getUniformLocation("Textured");
	materialColorUniform = opaqueStaticProgram->getUniformLocation("MaterialColor");
	vertexColorsUniform = opaqueStaticProgram->getUniformLocation("VertexColors");
	textureUniform = opaqueStaticProgram->getUniformLocation("Texture");

	vertexAttrib = opaqueStaticProgram->getAttributeLocation("Vertex");
	normalAttrib = opaqueStaticProgram->getAttributeLocation("Normal");
	colorAttrib = opaqueStaticProgram->getAttributeLocation("Color");
	texCoordAttrib = opaqueStaticProgram->getAttributeLocation("TexCoord");

	transAlgo->setupOpaqueShaderUniforms(opaqueStaticProgram);

	glEnableVertexAttribArray(vertexAttrib);

	StaticObjectList::iterator sit, sbeg, send;
	sbeg = staticObjs.begin();
	send = staticObjs.end();

	for (sit = sbeg ; sit != staticAlphaBegin ; sit++) {
	//for (sit = staticAlphaBegin ; sit != send ; sit++) {
		renderStaticSceneObject(*sit, mvpMatrix);
	}

	transAlgo->performOpaqueRenderFinalization();


	opaqueAnimProgram->makeCurrent();

	mvpMatrixUniform = opaqueAnimProgram->getUniformLocation("MVPMatrix");
	texturedUniform = opaqueAnimProgram->getUniformLocation("Textured");
	materialColorUniform = opaqueAnimProgram->getUniformLocation("MaterialColor");
	vertexColorsUniform = opaqueAnimProgram->getUniformLocation("VertexColors");
	textureUniform = opaqueAnimProgram->getUniformLocation("Texture");

	vertexAttrib = opaqueAnimProgram->getAttributeLocation("Vertex");
	normalAttrib = opaqueAnimProgram->getAttributeLocation("Normal");
	colorAttrib = opaqueAnimProgram->getAttributeLocation("Color");
	texCoordAttrib = opaqueAnimProgram->getAttributeLocation("TexCoord");

	glEnableVertexAttribArray(vertexAttrib);

	boneMatUniform = opaqueAnimProgram->getUniformLocation("BoneMatrices");
	boneUniform = opaqueAnimProgram->getUniformLocation("Bone");
	boneWeightAttrib = opaqueAnimProgram->getAttributeLocation("BoneWeights");
	boneIndexAttrib = opaqueAnimProgram->getAttributeLocation("BoneIndices");

	AnimObjectList::iterator ait, abeg, aend;
	abeg = animObjs.begin();
	aend = animObjs.end();

	for (ait = abeg ; ait != animAlphaBegin ; ait++) {
		renderAnimatedSceneObject(*ait, mvpMatrix);
	}


	if (transAlgo->performTransparentRenderInit()) {
		bool running = true;

		int lastType = SceneObjectInvalid;

		while (running) {
			transAlgo->performPreRenderPass();

			// ********** Static Objects **********
			transStaticProgram->makeCurrent();
			transAlgo->setupShaderUniforms(transStaticProgram);

			mvpMatrixUniform = transStaticProgram->getUniformLocation("MVPMatrix");
			texturedUniform = transStaticProgram->getUniformLocation("Textured");
			materialColorUniform = transStaticProgram->getUniformLocation("MaterialColor");
			vertexColorsUniform = transStaticProgram->getUniformLocation("VertexColors");
			textureUniform = transStaticProgram->getUniformLocation("Texture");

			vertexAttrib = transStaticProgram->getAttributeLocation("Vertex");
			normalAttrib = transStaticProgram->getAttributeLocation("Normal");
			colorAttrib = transStaticProgram->getAttributeLocation("Color");
			texCoordAttrib = transStaticProgram->getAttributeLocation("TexCoord");

			for (sit = staticAlphaBegin ; sit != send ; sit++) {
				renderStaticSceneObject(*sit, mvpMatrix);
			}


			// ********** Animated Objects **********

			transAnimProgram->makeCurrent();
			transAlgo->setupShaderUniforms(transAnimProgram);

			mvpMatrixUniform = transAnimProgram->getUniformLocation("MVPMatrix");
			texturedUniform = transAnimProgram->getUniformLocation("Textured");
			materialColorUniform = transAnimProgram->getUniformLocation("MaterialColor");
			vertexColorsUniform = transAnimProgram->getUniformLocation("VertexColors");
			textureUniform = transAnimProgram->getUniformLocation("Texture");

			vertexAttrib = transAnimProgram->getAttributeLocation("Vertex");
			normalAttrib = transAnimProgram->getAttributeLocation("Normal");
			colorAttrib = transAnimProgram->getAttributeLocation("Color");
			texCoordAttrib = transAnimProgram->getAttributeLocation("TexCoord");

			boneMatUniform = transAnimProgram->getUniformLocation("BoneMatrices");
			boneUniform = transAnimProgram->getUniformLocation("Bone");
			boneWeightAttrib = transAnimProgram->getAttributeLocation("BoneWeights");
			boneIndexAttrib = transAnimProgram->getAttributeLocation("BoneIndices");

			for (ait = animAlphaBegin ; ait != aend ; ait++) {
				renderAnimatedSceneObject(*ait, mvpMatrix);
			}


			running = transAlgo->performPostRenderPass();
		}
	}

	transAlgo->performTransparentRenderFinalization();

	glDisableVertexAttribArray(vertexAttrib);

	if (normalAttrib != -1)
		glDisableVertexAttribArray(normalAttrib);
	if (colorAttrib != -1)
		glDisableVertexAttribArray(colorAttrib);
	if (texCoordAttrib != -1)
		glDisableVertexAttribArray(texCoordAttrib);

	gtaglBindFramebuffer(GTAGL_FRAMEBUFFER, 0);

	staticObjs.clear();
	animObjs.clear();

	// Push the dummy back in.
	staticObjs.push_back(NULL);
	animObjs.push_back(NULL);


	staticAlphaBegin = staticObjs.begin();
	animAlphaBegin = animObjs.begin();
}


void DefaultRenderer::renderStaticSceneObject(StaticSceneObject* obj, const Matrix4& mvpMatrix)
{
	Matrix4 mat = mvpMatrix * obj->getModelMatrix();
	const float* matData = mat.toArray();
	glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, matData);

	MeshClump* meshClump = **obj->getDefinition()->getMeshPointer();
	TextureSource* texSrc = obj->getDefinition()->getTextureSource();

	if (!meshClump  ||  meshClump->getMeshCount() == 0)
		return;

	Mesh* mesh = *meshClump->getMeshBegin();

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
					Texture* tex = texSrc->getTexture(mat->getTexture());

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


void DefaultRenderer::renderAnimatedSceneObject(AnimatedSceneObject* obj, const Matrix4& mvpMatrix)
{
	Matrix4 mat = mvpMatrix * obj->getModelMatrix();

	AnimationPackage* apkg = **obj->getDefinition()->getAnimationPackagePointer();
	MeshClump* meshClump = **obj->getDefinition()->getMeshPointer();
	TextureSource* texSrc = obj->getDefinition()->getTextureSource();

	if (!meshClump  ||  meshClump->getMeshCount() == 0)
		return;

	Animation* anim = apkg->find(obj->getCurrentAnimation());

	int32_t boneCount = anim->getBoneCount();
	unsigned int npotBoneCount = GetNextPowerOfTwo(boneCount+1);
	Matrix4* boneMats = new Matrix4[npotBoneCount];

	Animator animator(meshClump, anim);
	animator.setTime(obj->getAnimationTime());

	boneMats[boneCount] = Matrix4::Identity;
	memcpy(boneMats, animator.getBoneMatrices(), boneCount*sizeof(Matrix4));

	int i = 0;

	glActiveTexture(GL_TEXTURE1);

	// TODO Reactivate, was deactivated because it doesn't work on GLES2.
	GLuint boneTex;
	glGenTextures(1, &boneTex);
	glBindTexture(GL_TEXTURE_1D, boneTex);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA32F, npotBoneCount*4, 0, GL_RGBA, GL_FLOAT, boneMats);

	delete[] boneMats;

	glUniform1i(boneMatUniform, 1);

	glActiveTexture(GL_TEXTURE0);

	MeshClump::MeshIterator mit;
	i = 0;

	for (mit = meshClump->getMeshBegin() ; mit != meshClump->getMeshEnd() ; mit++, i++) {
		Mesh* mesh = *mit;
		MeshFrame* frame = mesh->getFrame();

		Matrix4 mvpmMatrix = mat;

		/*if (anim->getBone(frame->getName()) == NULL) {
			mvpmMatrix *= frame->getAbsoluteModelMatrix();
		}*/

		const float* matData = mvpmMatrix.toArray();
		glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, matData);

		mesh->bindDataBuffer();

		int vertexOffs = mesh->getVertexOffset();
		int normalOffs = mesh->getNormalOffset();
		int colorOffs = mesh->getVertexColorOffset();
		int texCoordOffs = mesh->getTexCoordOffset();
		int boneWeightOffs = mesh->getBoneWeightOffset();
		int boneIndexOffs = mesh->getBoneIndexOffset();

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

		glEnableVertexAttribArray(boneIndexAttrib);
		//glEnableVertexAttribArray(boneWeightAttrib);

		glVertexAttribPointer(boneIndexAttrib, 4, GL_UNSIGNED_BYTE, GL_FALSE, 0, (void*) boneIndexOffs);
		//glVertexAttribPointer(boneWeightAttrib, 4, GL_FLOAT, GL_FALSE, 0, (void*) boneWeightOffs);

		if (boneIndexOffs == -1) {
			AnimationBone* bone = anim->getBone(frame->getName());
			size_t idx = anim->indexOf(bone);
			glUniform1i(boneUniform, idx);
		} else {
			glUniform1i(boneUniform, -1);
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
						Texture* tex = texSrc->getTexture(mat->getTexture());

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

	glDeleteTextures(1, &boneTex);
}

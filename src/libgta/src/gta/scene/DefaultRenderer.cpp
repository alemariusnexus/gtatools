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
#include "../EngineException.h"

#include <res_vertex_default_shader.h>
#include <res_fragment_default_shader.h>
#include <res_shade_vertex_shader.h>
#include <res_shade_fragment_shader.h>
#include <res_anim_shade_vertex_shader.h>
#include <res_anim_shade_fragment_shader.h>





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

	vertexDefaultData					= (const char*) res_vertex_default_shader_data;
	fragmentDefaultData					= (const char*) res_fragment_default_shader_data;
	shadeVertexShaderData				= (const char*) res_shade_vertex_shader_data;
	shadeFragmentShaderData				= (const char*) res_shade_fragment_shader_data;
	animShadeVertexShaderData			= (const char*) res_anim_shade_vertex_shader_data;
	animShadeFragmentShaderData			= (const char*) res_anim_shade_fragment_shader_data;

	vertexDefaultDataLen					= sizeof(res_vertex_default_shader_data);
	fragmentDefaultDataLen					= sizeof(res_fragment_default_shader_data);
	shadeVertexShaderDataLen				= sizeof(res_shade_vertex_shader_data);
	shadeFragmentShaderDataLen				= sizeof(res_shade_fragment_shader_data);
	animShadeVertexShaderDataLen			= sizeof(res_anim_shade_vertex_shader_data);
	animShadeFragmentShaderDataLen			= sizeof(res_anim_shade_fragment_shader_data);


#ifdef GTA_USE_OPENGL_ES
	glGetIntegerv(GL_MAX_VERTEX_UNIFORM_VECTORS, &maxVertexUniformComponents);
	maxVertexUniformComponents *= 4;
#else
	glGetIntegerv(GL_MAX_VERTEX_UNIFORM_COMPONENTS, &maxVertexUniformComponents);
#endif


	shadeVertexShader = new Shader(GL_VERTEX_SHADER, "Static Vertex Shader");
	shadeVertexShader->loadSourceCode(CString(shadeVertexShaderData, shadeVertexShaderDataLen));
	shadeVertexShader->compile();

	shadeFragmentShader = new Shader(GL_FRAGMENT_SHADER, "Static Fragment Shader");
	shadeFragmentShader->loadSourceCode(CString(shadeFragmentShaderData, shadeFragmentShaderDataLen));
	shadeFragmentShader->compile();

	vertexDefaultShader = new Shader(GL_VERTEX_SHADER, "Default Opaque Vertex Shader");
	vertexDefaultShader->loadSourceCode(CString(vertexDefaultData, vertexDefaultDataLen));
	vertexDefaultShader->compile();

	fragmentDefaultShader = new Shader(GL_FRAGMENT_SHADER, "Default Opaque Fragment Shader");
	fragmentDefaultShader->loadSourceCode(CString(fragmentDefaultData, fragmentDefaultDataLen));
	fragmentDefaultShader->compile();

	CString animShadeVertexShaderCode = CString("");
	if (maxVertexUniformComponents >= 2048) {
		animShadeVertexShaderCode.append("#define GTAGL_BONE_MATRIX_UNIFORM_ARRAY_100\n#line 0\n");
	}
	animShadeVertexShaderCode.append(CString(animShadeVertexShaderData, animShadeVertexShaderDataLen));

	animShadeVertexShader = new Shader(GL_VERTEX_SHADER, "Animated Vertex Shader");
	animShadeVertexShader->loadSourceCode(animShadeVertexShaderCode);
	animShadeVertexShader->compile();

	animShadeFragmentShader = new Shader(GL_FRAGMENT_SHADER, "Animated Fragment Shader");
	animShadeFragmentShader->loadSourceCode(CString(animShadeFragmentShaderData, animShadeFragmentShaderDataLen));
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



	if (maxVertexUniformComponents < 2048) {
#ifdef GTA_USE_OPENGL_ES
		if (!GL_OES_texture_float) {
#else
		if (!gtaglIsVersionSupported(3, 2)  &&  !GLEW_ARB_texture_float) {
#endif
			throw EngineException("ERROR: This OpenGL implementation provides neither enough (2048) vertex "
					"uniform components, nor does it support float textures. One of these features is needed "
					"for animation.", __FILE__, __LINE__);
		}

		glActiveTexture(GL_TEXTURE1);

		glGenTextures(1, &animBoneMatrixTex);
		glBindTexture(GL_TEXTURE_2D, animBoneMatrixTex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glActiveTexture(GL_TEXTURE0);
	}


	gtaglBindFramebuffer(GTAGL_FRAMEBUFFER, 0);

	GLException::checkError("After DefaultRenderer initialization");
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

	if (!gtaglIsVersionSupported(3, 1)) {
		boneMatSizeUniform = opaqueAnimProgram->getUniformLocation("TexSize");
	}

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

			if (!gtaglIsVersionSupported(3, 1)) {
				boneMatSizeUniform = transAnimProgram->getUniformLocation("TexSize");
			}

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

	GLException::checkError("After DefaultRenderer::render()");
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

	// Find the current animation
	CString canimName = obj->getCurrentAnimation();
	Animation* anim = NULL;

	if (canimName.get()) {
		anim = apkg->find(canimName);
	}

	// Take a snapshot of the bone matrices for animation
	int32_t boneCount;
	Matrix4* boneMats;
	unsigned int npotBoneCount;

	// We use an extra identity bone matrix which is used when a bone can't be found
	boneCount = meshClump->getBoneCount();
	npotBoneCount = GetNextPowerOfTwo(boneCount+1);
	boneMats = new Matrix4[npotBoneCount];

	Animator animator(meshClump, anim);
	animator.setTime(obj->getAnimationTime());

	boneMats[boneCount] = Matrix4::Identity;
	memcpy(boneMats, animator.getBoneMatrices(), boneCount*sizeof(Matrix4));

	if (maxVertexUniformComponents < 2048) {
		// We store the bone matrices in a texture to access them from the vertex shader
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, animBoneMatrixTex);

#ifdef GTA_USE_OPENGL_ES
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, npotBoneCount*4, 1, 0, GL_RGBA, GL_FLOAT, boneMats);
#else
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, npotBoneCount*4, 1, 0, GL_RGBA, GL_FLOAT, boneMats);
#endif

		glUniform1i(boneMatUniform, 1);

		if (!gtaglIsVersionSupported(3, 1)) {
			glUniform1i(boneMatSizeUniform, npotBoneCount*4);
		}

		glActiveTexture(GL_TEXTURE0);
	} else {
		int32_t maxBoneCount = 100;

		if (boneCount+1 > maxBoneCount) {
			char errmsg[128];
			sprintf(errmsg, "ERROR: Too many bones in mesh: %d. Only %d are supported.", boneCount+1,
					maxBoneCount);
			throw EngineException(errmsg, __FILE__, __LINE__);
		}

		glUniformMatrix4fv(boneMatUniform, boneCount+1, GL_FALSE, (float*) boneMats);
	}

	for (MeshClump::MeshIterator mit = meshClump->getMeshBegin() ; mit != meshClump->getMeshEnd() ; mit++) {
		Mesh* mesh = *mit;
		MeshFrame* frame = mesh->getFrame();

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
		glEnableVertexAttribArray(boneWeightAttrib);

		glVertexAttribPointer(boneIndexAttrib, 4, GL_UNSIGNED_BYTE, GL_FALSE, 0, (void*) boneIndexOffs);
		glVertexAttribPointer(boneWeightAttrib, 4, GL_FLOAT, GL_FALSE, 0, (void*) boneWeightOffs);

		Matrix4 mvpmMatrix = mat;

		if (boneIndexOffs == -1) {
			// This is an unskinned animation, so all vertices of this frame belong to a single bone.
			AnimationBone* bone = anim->getBoneForFrame(frame);
			int32_t idx = frame->getBoneNumber();

			if (idx == -1)
				idx = boneCount;

			glUniform1i(boneUniform, idx);
		} else {
			// This animation uses skinning, so the bone index is stored on a per-vertex basis.
			glUniform1i(boneUniform, -1);
		}

		const float* matData = mvpmMatrix.toArray();
		glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, matData);

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

	delete[] boneMats;
}

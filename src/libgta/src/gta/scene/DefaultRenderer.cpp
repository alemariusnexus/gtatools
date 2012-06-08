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
#include "objects/LightSource.h"
#include "objects/DirectionalLightSource.h"
#include "objects/PointLightSource.h"
#include "objects/SpotLightSource.h"

#include <res_vertex_default_shader.h>
#include <res_fragment_default_shader.h>
#include <res_shade_vertex_shader.h>
#include <res_shade_fragment_shader.h>
#include <res_anim_shade_vertex_shader.h>
#include <res_anim_shade_fragment_shader.h>
#include <res_lighting_vertex_shader.h>





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
	const char* lightingVertexShaderData;

	int vertexDefaultDataLen;
	int fragmentDefaultDataLen;
	int shadeVertexShaderDataLen;
	int shadeFragmentShaderDataLen;
	int animShadeVertexShaderDataLen;
	int animShadeFragmentShaderDataLen;
	int lightingVertexShaderDataLen;

	vertexDefaultData					= (const char*) res_vertex_default_shader_data;
	fragmentDefaultData					= (const char*) res_fragment_default_shader_data;
	shadeVertexShaderData				= (const char*) res_shade_vertex_shader_data;
	shadeFragmentShaderData				= (const char*) res_shade_fragment_shader_data;
	animShadeVertexShaderData			= (const char*) res_anim_shade_vertex_shader_data;
	animShadeFragmentShaderData			= (const char*) res_anim_shade_fragment_shader_data;
	lightingVertexShaderData			= (const char*) res_lighting_vertex_shader_data;

	vertexDefaultDataLen					= sizeof(res_vertex_default_shader_data);
	fragmentDefaultDataLen					= sizeof(res_fragment_default_shader_data);
	shadeVertexShaderDataLen				= sizeof(res_shade_vertex_shader_data);
	shadeFragmentShaderDataLen				= sizeof(res_shade_fragment_shader_data);
	animShadeVertexShaderDataLen			= sizeof(res_anim_shade_vertex_shader_data);
	animShadeFragmentShaderDataLen			= sizeof(res_anim_shade_fragment_shader_data);
	lightingVertexShaderDataLen				= sizeof(res_lighting_vertex_shader_data);


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

	lightingVertexShader = new Shader(GL_VERTEX_SHADER, "Lighting Vertex Shader");
	lightingVertexShader->loadSourceCode(CString(lightingVertexShaderData, lightingVertexShaderDataLen));
	lightingVertexShader->compile();

	opaqueStaticProgram = new ShaderProgram("Opaque Static Shader Program");
	opaqueStaticProgram->attachShader(shadeVertexShader);
	opaqueStaticProgram->attachShader(shadeFragmentShader);
	opaqueStaticProgram->attachShader(vertexDefaultShader);
	opaqueStaticProgram->attachShader(fragmentDefaultShader);
	opaqueStaticProgram->attachShader(lightingVertexShader);
	opaqueStaticProgram->link();

	transStaticProgram = new ShaderProgram("Transparent Static Shader Program");
	transStaticProgram->attachShader(shadeVertexShader);
	transStaticProgram->attachShader(shadeFragmentShader);
	transStaticProgram->attachShader(lightingVertexShader);


	opaqueAnimProgram = new ShaderProgram("Opaque Animated Shader Program");
	opaqueAnimProgram->attachShader(animShadeVertexShader);
	opaqueAnimProgram->attachShader(animShadeFragmentShader);
	opaqueAnimProgram->attachShader(vertexDefaultShader);
	opaqueAnimProgram->attachShader(fragmentDefaultShader);
	opaqueAnimProgram->attachShader(lightingVertexShader);
	opaqueAnimProgram->link();

	transAnimProgram = new ShaderProgram("Transparent Animated Shader Program");
	transAnimProgram->attachShader(animShadeVertexShader);
	transAnimProgram->attachShader(animShadeFragmentShader);
	transAnimProgram->attachShader(lightingVertexShader);



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
	SceneObject::typeflags_t tf = obj->getTypeFlags();

	if ((tf & SceneObject::TypeFlagAnimated)  !=  0) {
		if (obj->hasAlphaTransparency())
			animObjs.push_back(dynamic_cast<AnimatedMapSceneObject*>(obj));
		else
			animObjs.insert(animAlphaBegin, dynamic_cast<AnimatedMapSceneObject*>(obj));
	} else {
		if (obj->hasAlphaTransparency())
			staticObjs.push_back(dynamic_cast<MapSceneObject*>(obj));
		else
			staticObjs.insert(staticAlphaBegin, dynamic_cast<MapSceneObject*>(obj));
	}
}


void DefaultRenderer::enqueueForRendering(LightSource* ls)
{
	lightSources.push_back(ls);
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

	Matrix4 vpMatrix = engine->getProjectionMatrix();
	//mvpMatrix *= Matrix4::lookAt(camera->getTarget(), camera->getUp());
	Matrix4 vMatrix = Matrix4::lookAt(camera->getTarget(), camera->getUp());
	const Vector3& cpos = camera->getPosition();
	vMatrix.translate(-cpos);
	vpMatrix *= vMatrix;

	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);

	gtaglBindFramebuffer(GTAGL_FRAMEBUFFER, 0);

	transAlgo->performOpaqueRenderInit();

	opaqueStaticProgram->makeCurrent();

	initializeUniforms(opaqueStaticProgram);

	transAlgo->setupOpaqueShaderUniforms(opaqueStaticProgram);

	glEnableVertexAttribArray(vertexAttrib);

	StaticObjectList::iterator sit, sbeg, send;
	sbeg = staticObjs.begin();
	send = staticObjs.end();

	glUniform3f(camPosUniform, cpos.getX(), cpos.getY(), cpos.getZ());

	for (sit = sbeg ; sit != staticAlphaBegin ; sit++) {
		renderStaticSceneObject(*sit, vMatrix, vpMatrix);
	}

	transAlgo->performOpaqueRenderFinalization();


	opaqueAnimProgram->makeCurrent();

	initializeUniforms(opaqueAnimProgram);

	glEnableVertexAttribArray(vertexAttrib);

	AnimObjectList::iterator ait, abeg, aend;
	abeg = animObjs.begin();
	aend = animObjs.end();

	glUniform3f(camPosUniform, cpos.getX(), cpos.getY(), cpos.getZ());

	for (ait = abeg ; ait != animAlphaBegin ; ait++) {
		renderAnimatedSceneObject(*ait, vMatrix, vpMatrix);
	}


	if (transAlgo->performTransparentRenderInit()) {
		bool running = true;

		while (running) {
			transAlgo->performPreRenderPass();

			// ********** Static Objects **********
			transStaticProgram->makeCurrent();
			transAlgo->setupShaderUniforms(transStaticProgram);

			initializeUniforms(transStaticProgram);

			glUniform3f(camPosUniform, cpos.getX(), cpos.getY(), cpos.getZ());

			for (sit = staticAlphaBegin ; sit != send ; sit++) {
				renderStaticSceneObject(*sit, vMatrix, vpMatrix);
			}


			// ********** Animated Objects **********

			transAnimProgram->makeCurrent();
			transAlgo->setupShaderUniforms(transAnimProgram);

			initializeUniforms(transAnimProgram);

			glUniform3f(camPosUniform, cpos.getX(), cpos.getY(), cpos.getZ());

			for (ait = animAlphaBegin ; ait != aend ; ait++) {
				renderAnimatedSceneObject(*ait, vMatrix, vpMatrix);
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

	lightSources.clear();

	staticObjs.clear();
	animObjs.clear();

	// Push the dummy back in.
	staticObjs.push_back(NULL);
	animObjs.push_back(NULL);


	staticAlphaBegin = staticObjs.begin();
	animAlphaBegin = animObjs.begin();

	GLException::checkError("After DefaultRenderer::render()");
}


void DefaultRenderer::initializeUniforms(ShaderProgram* program)
{
	camPosUniform = program->getUniformLocation("CameraPosition");
	mvMatrixUniform = program->getUniformLocation("MVMatrix");
	mvpMatrixUniform = program->getUniformLocation("MVPMatrix");
	normalMvMatrixUniform = program->getUniformLocation("NormalMVMatrix");
	texturedUniform = program->getUniformLocation("Textured");
	materialColorUniform = program->getUniformLocation("MaterialColor");
	vertexColorsUniform = program->getUniformLocation("VertexColors");
	textureUniform = program->getUniformLocation("Texture");

	matAmbientReflectionUniform = program->getUniformLocation("MaterialAmbientReflection");
	matDiffuseReflectionUniform = program->getUniformLocation("MaterialDiffuseReflection");
	matSpecularReflectionUniform = program->getUniformLocation("MaterialSpecularReflection");
	globalAmbientLightUniform = program->getUniformLocation("GlobalAmbientLight");
	dynamicLightingEnabledUniform = program->getUniformLocation("DynamicLightingEnabled");

	vertexAttrib = program->getAttributeLocation("Vertex");
	normalAttrib = program->getAttributeLocation("Normal");
	colorAttrib = program->getAttributeLocation("Color");
	texCoordAttrib = program->getAttributeLocation("TexCoord");

	boneMatUniform = program->getUniformLocation("BoneMatrices");
	boneUniform = program->getUniformLocation("Bone");
	boneWeightAttrib = program->getAttributeLocation("BoneWeights");
	boneIndexAttrib = program->getAttributeLocation("BoneIndices");

	if (!gtaglIsVersionSupported(3, 1)) {
		boneMatSizeUniform = program->getUniformLocation("TexSize");
	}

	for (int i = 0 ; i < 10 ; i++) {
		char baseName[32];
		sprintf(baseName, "LightSources[%d]", i);

		lightSourceUniforms[i].ambientUniform
				= program->getUniformLocation(CString(baseName).append(".ambient"));
		lightSourceUniforms[i].diffuseUniform
				= program->getUniformLocation(CString(baseName).append(".diffuse"));
		lightSourceUniforms[i].specularUniform
				= program->getUniformLocation(CString(baseName).append(".specular"));
		lightSourceUniforms[i].positionUniform
				= program->getUniformLocation(CString(baseName).append(".position"));
		lightSourceUniforms[i].directionUniform
				= program->getUniformLocation(CString(baseName).append(".direction"));
		lightSourceUniforms[i].cutoffAngleCosUniform
				= program->getUniformLocation(CString(baseName).append(".cutoffAngleCos"));
		lightSourceUniforms[i].shininessUniform
				= program->getUniformLocation(CString(baseName).append(".shininess"));
		lightSourceUniforms[i].spotlightExponentUniform
				= program->getUniformLocation(CString(baseName).append(".spotlightExponent"));
		lightSourceUniforms[i].constAttenuationUniform
				= program->getUniformLocation(CString(baseName).append(".constAttenuation"));
		lightSourceUniforms[i].linearAttenuationUniform
				= program->getUniformLocation(CString(baseName).append(".linearAttenuation"));
		lightSourceUniforms[i].quadAttenuationUniform
				= program->getUniformLocation(CString(baseName).append(".quadAttenuation"));
	}
}


void DefaultRenderer::setupDynamicLighting(bool enabled, const Matrix4& vMatrix, const Matrix4& lightMatrix)
{
	if (enabled) {
		glUniform1i(dynamicLightingEnabledUniform, 1);

		int i = 0;
		for (LightSourceList::iterator it = lightSources.begin() ; it != lightSources.end() ; it++) {
			LightSource* light = *it;

			Vector4 a = light->getAmbientColor();
			Vector4 d = light->getDiffuseColor();
			Vector4 s = light->getSpecularColor();

			glUniform4f(lightSourceUniforms[i].ambientUniform, a.getX(), a.getY(), a.getZ(), a.getW());
			glUniform4f(lightSourceUniforms[i].diffuseUniform, d.getX(), d.getY(), d.getZ(), d.getW());
			glUniform4f(lightSourceUniforms[i].specularUniform, s.getX(), s.getY(), s.getZ(), s.getW());

			if (light->getLightType() == LightSource::DirectionalLight) {
				DirectionalLightSource* dlight = (DirectionalLightSource*) light;

				Vector3 dir = dlight->getDirection();
				Vector4 dir4 = lightMatrix * Vector4(dir);
				dir = Vector3(dir4.getX(), dir4.getY(), dir4.getZ());
				dir.normalize();

				glUniform3f(lightSourceUniforms[i].directionUniform, dir.getX(), dir.getY(), dir.getZ());
				glUniform1f(lightSourceUniforms[i].shininessUniform, dlight->getShininess());

				glUniform1f(lightSourceUniforms[i].constAttenuationUniform, dlight->getConstantAttenuation());
				glUniform1f(lightSourceUniforms[i].linearAttenuationUniform, dlight->getLinearAttenuation());
				glUniform1f(lightSourceUniforms[i].quadAttenuationUniform, dlight->getQuadraticAttenuation());

				//printf("Setting uniform %d\n", lightSourceUniforms[i].cutoffAngleCosUniform);
				glUniform1f(lightSourceUniforms[i].cutoffAngleCosUniform, -1.0f);
			} else if (light->getLightType() == LightSource::PointLight) {
				PointLightSource* plight = (PointLightSource*) light;

				Vector3 pos = plight->getPosition();
				Vector4 pos4 = vMatrix * Vector4(pos);
				pos = Vector3(pos4.getX(), pos4.getY(), pos4.getZ());

				glUniform3f(lightSourceUniforms[i].positionUniform, pos.getX(), pos.getY(), pos.getZ());
				glUniform1f(lightSourceUniforms[i].shininessUniform, plight->getShininess());

				glUniform1f(lightSourceUniforms[i].constAttenuationUniform, plight->getConstantAttenuation());
				glUniform1f(lightSourceUniforms[i].linearAttenuationUniform, plight->getLinearAttenuation());
				glUniform1f(lightSourceUniforms[i].quadAttenuationUniform, plight->getQuadraticAttenuation());

				glUniform1f(lightSourceUniforms[i].cutoffAngleCosUniform, -2.0f);
			} else if (light->getLightType() == LightSource::SpotLight) {
				SpotLightSource* slight = (SpotLightSource*) light;

				Vector3 pos = slight->getPosition();
				Vector4 pos4 = vMatrix * Vector4(pos);
				pos = Vector3(pos4.getX(), pos4.getY(), pos4.getZ());

				Vector3 dir = slight->getDirection();
				Vector4 dir4 = lightMatrix * Vector4(dir);
				dir = Vector3(dir4.getX(), dir4.getY(), dir4.getZ());
				dir.normalize();

				glUniform3f(lightSourceUniforms[i].positionUniform, pos.getX(), pos.getY(), pos.getZ());
				glUniform3f(lightSourceUniforms[i].directionUniform, dir.getX(), dir.getY(), dir.getZ());
				glUniform1f(lightSourceUniforms[i].shininessUniform, slight->getShininess());
				glUniform1f(lightSourceUniforms[i].cutoffAngleCosUniform, slight->getCutoffAngleCosine());
				glUniform1f(lightSourceUniforms[i].spotlightExponentUniform, slight->getSpotlightExponent());

				glUniform1f(lightSourceUniforms[i].constAttenuationUniform, slight->getConstantAttenuation());
				glUniform1f(lightSourceUniforms[i].linearAttenuationUniform, slight->getLinearAttenuation());
				glUniform1f(lightSourceUniforms[i].quadAttenuationUniform, slight->getQuadraticAttenuation());
			}

			i++;
		}

		if (i <= 9) {
			glUniform1f(lightSourceUniforms[i].cutoffAngleCosUniform, -3.0f);
		}
	} else {
		glUniform1i(dynamicLightingEnabledUniform, 0);
	}
}


void DefaultRenderer::renderStaticSceneObject(MapSceneObject* obj, const Matrix4& vMatrix, const Matrix4& vpMatrix)
{

	Matrix4 mvpMatrix = vpMatrix * obj->getModelMatrix();
	const float* matData = mvpMatrix.toArray();
	glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, matData);

	Matrix4 mvMatrix = vMatrix * obj->getModelMatrix();
	matData = mvMatrix.toArray();
	glUniformMatrix4fv(mvMatrixUniform, 1, GL_FALSE, matData);

	Matrix4 normalMvMatrix = mvMatrix;
	normalMvMatrix.invert();
	normalMvMatrix.transpose();
	matData = normalMvMatrix.toArray();
	glUniformMatrix4fv(normalMvMatrixUniform, 1, GL_FALSE, matData);

	Matrix4 lightMatrix = vMatrix;
	lightMatrix.invert();
	lightMatrix.transpose();

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


	glUniform4f(matAmbientReflectionUniform, 1.0f, 1.0f, 1.0f, 1.0f);
	glUniform4f(matDiffuseReflectionUniform, 1.0f, 1.0f, 1.0f, 1.0f);
	glUniform4f(matSpecularReflectionUniform, 1.0f, 1.0f, 1.0f, 1.0f);
	glUniform4f(globalAmbientLightUniform, 0.2f, 0.2f, 0.2f, 1.0f);

	setupDynamicLighting((mesh->getFlags() & MeshDynamicLighting)  !=  0, vMatrix, lightMatrix);

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
			glVertexAttribPointer(colorAttrib, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, (void*) colorOffs);
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


void DefaultRenderer::renderAnimatedSceneObject(AnimatedMapSceneObject* obj, const Matrix4& vMatrix, const Matrix4& vpMatrix)
{
	Matrix4 mvpMatrix = vpMatrix * obj->getModelMatrix();
	const float* matData = mvpMatrix.toArray();
	glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, matData);

	Matrix4 mvMatrix = vMatrix * obj->getModelMatrix();
	matData = mvMatrix.toArray();
	glUniformMatrix4fv(mvMatrixUniform, 1, GL_FALSE, matData);

	Matrix4 normalMvMatrix = mvMatrix;
	normalMvMatrix.invert();
	normalMvMatrix.transpose();
	matData = normalMvMatrix.toArray();
	glUniformMatrix4fv(normalMvMatrixUniform, 1, GL_FALSE, matData);

	Matrix4 lightMatrix = vMatrix;
	lightMatrix.invert();
	lightMatrix.transpose();

	MeshClump* meshClump = **obj->getDefinition()->getMeshPointer();
	TextureSource* texSrc = obj->getDefinition()->getTextureSource();

	if (!meshClump  ||  meshClump->getMeshCount() == 0)
		return;

	AnimationPackage* apkg = NULL;

	if (obj->getDefinition()->getAnimationPackagePointer()) {
		apkg = **obj->getDefinition()->getAnimationPackagePointer();
	}

	// Find the current animation
	CString canimName = obj->getCurrentAnimation();
	Animation* anim = NULL;

	if (canimName.get()  &&  apkg) {
		anim = apkg->find(canimName);
	}

	// Take a snapshot of the bone matrices for animation
	int32_t boneCount;
	Matrix4* boneMats;
	unsigned int npotBoneCount;

	Animator* animator = NULL;

	if (anim) {
		animator = new Animator(meshClump, anim);
		boneCount = animator->getBoneCount();
	} else {
		boneCount = meshClump->getBoneCount();
	}

	// We use an extra identity bone matrix which is used when a bone can't be found
	npotBoneCount = GetNextPowerOfTwo(boneCount+1);
	boneMats = new Matrix4[npotBoneCount];

	if (anim) {
		animator->setTime(obj->getAnimationTime());

		boneMats[boneCount] = Matrix4::Identity;
		memcpy(boneMats, animator->getBoneMatrices(), boneCount*sizeof(Matrix4));
	} else {
		for (unsigned int i = 0 ; i <= boneCount ; i++) {
			boneMats[i] = Matrix4::Identity;
		}
	}

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

	glUniform4f(matAmbientReflectionUniform, 1.0f, 1.0f, 1.0f, 1.0f);
	glUniform4f(matDiffuseReflectionUniform, 1.0f, 1.0f, 1.0f, 1.0f);
	glUniform4f(matSpecularReflectionUniform, 1.0f, 1.0f, 1.0f, 1.0f);
	glUniform4f(globalAmbientLightUniform, 0.2f, 0.2f, 0.2f, 1.0f);

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
				glVertexAttribPointer(colorAttrib, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, (void*) colorOffs);
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

		if (boneIndexOffs == -1) {
			// This is an unskinned animation, so all vertices of this frame belong to a single bone.

			int32_t num;

			if (!animator  ||  !animator->hasPseudoBoneNumbers())
				num = frame->getBoneNumber();
			else
				num = animator->getPseudoBoneNumber(frame);

			if (num == -1)
				num = boneCount;

			glUniform1i(boneUniform, num);
		} else {
			// This animation uses skinning, so the bone index is stored on a per-vertex basis.
			glUniform1i(boneUniform, -1);
		}

		setupDynamicLighting((mesh->getFlags() & MeshDynamicLighting) != 0, vMatrix, lightMatrix);

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

	delete animator;

	delete[] boneMats;
}

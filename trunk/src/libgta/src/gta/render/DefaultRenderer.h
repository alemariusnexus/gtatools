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

#ifndef DEFAULTRENDERER_H_
#define DEFAULTRENDERER_H_

#include "Renderer.h"
#include "StaticRenderingMesh.h"
#include "AnimatedRenderingMesh.h"
#include "../Shader.h"
#include "../ShaderProgram.h"
#include "../scene/objects/LightSource.h"
#include <list>

using std::list;


class DefaultRenderer : public Renderer
{
private:
	typedef list<StaticRenderingMesh*> StaticMeshList;
	typedef StaticMeshList::iterator StaticMeshIterator;
	typedef StaticMeshList::const_iterator ConstStaticMeshIterator;

	typedef list<AnimatedRenderingMesh*> AnimatedMeshList;
	typedef AnimatedMeshList::iterator AnimatedMeshIterator;
	typedef AnimatedMeshList::const_iterator ConstAnimatedMeshIterator;

	typedef list<LightSource*> LightSourceList;
	typedef LightSourceList::iterator LightSourceIterator;
	typedef LightSourceList::const_iterator ConstLightSourceIterator;

private:
	struct LightSourceUniformLocations
	{
		GLint ambient;
		GLint diffuse;
		GLint specular;
		GLint position;
		GLint direction;
		GLint cutoffAngleCos;
		GLint shininess;
		GLint spotlightExponent;
		GLint constAttenuation;
		GLint linearAttenuation;
		GLint quadAttenuation;
	};

	struct ShaderProgramLocations
	{
		GLint camPosUniform;
		GLint mvMatrixUniform;
		GLint mvpMatrixUniform;
		GLint normalMvMatrixUniform;
		GLint texturedUniform;
		GLint materialColorUniform;
		GLint vertexColorsUniform;
		GLint textureUniform;

		GLint matAmbientReflectionUniform;
		GLint matDiffuseReflectionUniform;
		GLint matSpecularReflectionUniform;
		GLint globalAmbientLightUniform;
		GLint dynamicLightingEnabledUniform;

		GLint vertexAttrib;
		GLint normalAttrib;
		GLint colorAttrib;
		GLint texCoordAttrib;

		GLint boneMatUniform;
		GLint boneUniform;
		GLint boneWeightAttrib;
		GLint boneIndexAttrib;

		GLint boneMatSizeUniform;

		LightSourceUniformLocations lightSources[10];
	};

public:
	DefaultRenderer();

	virtual void enqueueForRendering(RenderingEntity* entity);
	virtual void enqueueForRendering(LightSource* ls);
	virtual void render();

private:
	void setupShaders();
	void setupFBOs();
	void setupBuffers();

	void initializeUniforms(ShaderProgram* program);
	void setupDynamicLighting(bool enabled);
	void dpSetupShaderUniforms(ShaderProgram* program);

	void renderShadowVolumes(LightSource* light) {}

	void renderStaticMeshes(StaticMeshIterator beg, StaticMeshIterator end);
	void renderAnimatedMeshes(AnimatedMeshIterator beg, AnimatedMeshIterator end);

private:
	StaticMeshList opaqueStaticMeshes;
	StaticMeshList transStaticMeshes;
	AnimatedMeshList opaqueAnimMeshes;
	AnimatedMeshList transAnimMeshes;

	LightSourceList lightSources;

	struct {
		Shader* shadeVertexShader;
		Shader* shadeFragmentShader;
		Shader* vertexDefaultShader;
		Shader* fragmentDefaultShader;
		Shader* animShadeVertexShader;
		Shader* animShadeFragmentShader;
		Shader* lightingVertexShader;
		Shader* svVertexShader;
		Shader* svFragmentShader;
		Shader* svBlendVertexShader;
		Shader* svBlendFragmentShader;

		Shader* dpPeelLayerVertexShader;
		Shader* dpPeelLayerFragmentShader;
		Shader* dpBlendLayerVertexShader;
		Shader* dpBlendLayerFragmentShader;
		Shader* dpBlendFinalVertexShader;
		Shader* dpBlendFinalFragmentShader;
	} shaders;

	struct {
		ShaderProgram* opaqueStaticProgram;
		ShaderProgram* transStaticProgram;

		ShaderProgram* opaqueAnimProgram;
		ShaderProgram* transAnimProgram;

		ShaderProgram* svProgram;
		ShaderProgram* svBlendProgram;

		ShaderProgram* dpBlendLayerProgram;
		ShaderProgram* dpBlendFinalProgram;
	} programs;

	struct {
		GLuint dpFBO;
		GLuint dpPingPongFBOs[2];
		GLuint dpOpaqueFBO;

		GLuint dpColorTex;
		GLuint dpPingPongDepthStencilTexes[2];
		GLuint dpPingPongColorTexes[2];
		GLuint dpOpaqueTexes[2];

		GLuint dpRBO;
		GLuint dpStencilRBO;
	} fbos;

	struct {
		GLint dpBlendLayerTexUniform, dpBlendFinalTexUniform;
		GLint dpBlendLayerVertexAttrib, dpBlendFinalVertexAttrib;
	} dpShaderLocations;


	GLuint planeDataBuf;

	GLint maxVertexUniformComponents;

	map<ShaderProgram*, ShaderProgramLocations> spLocs;
	ShaderProgramLocations* renderProgramLocations;

	bool globalAmbientLightEnabled;
	LightSourceIterator enabledLightBegin, enabledLightEnd;

	Matrix4 vMatrix, pMatrix, vpMatrix, vRotationMatrix;

public:
	int dpNumPasses;
};

#endif /* DEFAULTRENDERER_H_ */

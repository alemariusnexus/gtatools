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
#include "../Shader.h"
#include "../ShaderProgram.h"
#include "parts/VisualSceneObject.h"
#include "parts/AnimatedSceneObject.h"
#include "objects/MapSceneObject.h"
#include "objects/AnimatedMapSceneObject.h"
#include "../GLException.h"
#include "../gl.h"
#include "TransparencyAlgorithm.h"
#include <list>
#include "LightSource.h"

using std::list;







class DefaultRenderer : public Renderer {
public:
	enum TransparencyMode
	{
		DepthPeeling,
		WeightedAverage
	};

private:
	typedef list<MapSceneObject*> StaticObjectList;
	typedef list<AnimatedMapSceneObject*> AnimObjectList;
	typedef list<LightSource*> LightSourceList;

public:
	DefaultRenderer();
	virtual void enqueueForRendering(VisualSceneObject* obj);
	virtual void render();
	void setTransparencyAlgorithm(TransparencyAlgorithm* algo);
	TransparencyAlgorithm* getTransparencyAlgorithm() { return transAlgo; }
	void addLightSource(LightSource* light) { lightSources.push_back(light); }

private:
	void initializeUniforms(ShaderProgram* program);
	void setupDynamicLighting(bool enabled, const Matrix4& vMatrix, const Matrix4& lightMatrix);
	void renderStaticSceneObject(MapSceneObject* obj, const Matrix4& mvMatrix, const Matrix4& mvpMatrix);
	void renderAnimatedSceneObject(AnimatedMapSceneObject* obj, const Matrix4& vMatrix, const Matrix4& vpMatrix);

private:
	TransparencyAlgorithm* transAlgo;

	StaticObjectList staticObjs;
	StaticObjectList::iterator staticAlphaBegin;

	AnimObjectList animObjs;
	AnimObjectList::iterator animAlphaBegin;

	LightSourceList lightSources;

	Shader* shadeVertexShader;
	Shader* shadeFragmentShader;
	Shader* vertexDefaultShader;
	Shader* fragmentDefaultShader;
	Shader* animShadeVertexShader;
	Shader* animShadeFragmentShader;
	Shader* lightingVertexShader;

	ShaderProgram* opaqueStaticProgram;
	ShaderProgram* transStaticProgram;

	ShaderProgram* opaqueAnimProgram;
	ShaderProgram* transAnimProgram;

	GLint mvMatrixUniform, mvpMatrixUniform, normalMvMatrixUniform, texturedUniform, materialColorUniform,
			vertexColorsUniform, textureUniform, camPosUniform;
	GLint vertexAttrib, normalAttrib, colorAttrib, texCoordAttrib;

	struct {
		GLint ambientUniform, diffuseUniform, specularUniform;
		GLint positionUniform, directionUniform;
		GLint cutoffAngleCosUniform;
		GLint shininessUniform;
		GLint spotlightExponentUniform;
		GLint constAttenuationUniform, linearAttenuationUniform, quadAttenuationUniform;
	} lightSourceUniforms[10];

	GLint matAmbientReflectionUniform, matDiffuseReflectionUniform, matSpecularReflectionUniform;
	GLint globalAmbientLightUniform;
	GLint dynamicLightingEnabledUniform;

	GLint boneMatUniform, boneUniform, boneMatSizeUniform;
	GLint boneIndexAttrib, boneWeightAttrib;

	GLuint planeDataBuf;

	GLuint animBoneMatrixTex;

	GLint maxVertexUniformComponents;
};

#endif /* DEFAULTRENDERER_H_ */

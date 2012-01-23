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
#include "VisualSceneObject.h"
#include "StaticSceneObject.h"
#include "AnimatedSceneObject.h"
#include "../GLException.h"
#include "../gl.h"
#include "TransparencyAlgorithm.h"
#include <list>

using std::list;







class DefaultRenderer : public Renderer {
public:
	enum TransparencyMode
	{
		DepthPeeling,
		WeightedAverage
	};

private:
	typedef list<StaticSceneObject*> StaticObjectList;
	typedef list<AnimatedSceneObject*> AnimObjectList;

public:
	DefaultRenderer();
	virtual void enqueueForRendering(VisualSceneObject* obj);
	virtual void render();
	void setTransparencyAlgorithm(TransparencyAlgorithm* algo);
	TransparencyAlgorithm* getTransparencyAlgorithm() { return transAlgo; }

private:
	void renderStaticSceneObject(StaticSceneObject* obj, const Matrix4& mvpMatrix);
	void renderAnimatedSceneObject(AnimatedSceneObject* obj, const Matrix4& mvpMatrix);

private:
	TransparencyAlgorithm* transAlgo;

	StaticObjectList staticObjs;
	StaticObjectList::iterator staticAlphaBegin;

	AnimObjectList animObjs;
	AnimObjectList::iterator animAlphaBegin;

	Shader* shadeVertexShader;
	Shader* shadeFragmentShader;
	Shader* vertexDefaultShader;
	Shader* fragmentDefaultShader;
	Shader* animShadeVertexShader;
	Shader* animShadeFragmentShader;

	ShaderProgram* opaqueStaticProgram;
	ShaderProgram* transStaticProgram;

	ShaderProgram* opaqueAnimProgram;
	ShaderProgram* transAnimProgram;

	GLint mvpMatrixUniform, texturedUniform, materialColorUniform, vertexColorsUniform,
			textureUniform;
	GLint vertexAttrib, normalAttrib, colorAttrib, texCoordAttrib;

	GLint boneMatUniform, boneUniform, boneMatSizeUniform;
	GLint boneIndexAttrib, boneWeightAttrib;

	GLuint planeDataBuf;

	GLuint animBoneMatrixTex;

	GLint maxVertexUniformComponents;
};

#endif /* DEFAULTRENDERER_H_ */

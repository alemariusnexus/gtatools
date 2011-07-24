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

#ifndef DEFAULTRENDERER_H_
#define DEFAULTRENDERER_H_

#include "Renderer.h"
#include "Shader.h"
#include "ShaderProgram.h"
#include "scene/StaticSceneObject.h"
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
	typedef list<SceneObject*> ObjectList;

public:
	DefaultRenderer();
	virtual void enqueueForRendering(SceneObject* obj);
	virtual void render();
	TransparencyMode getTransparencyMode() const { return transMode; }
	void setTransparencyMode(TransparencyMode mode) { transMode = mode; }
	int getTransparencyPassCount() const { return transPassCount; }
	void setTransparencyPassCount(int count) { transPassCount = count; }

private:
	void renderStaticSceneObject(StaticSceneObject* obj, const Matrix4& mvpMatrix);

private:
	ObjectList objs;
	ObjectList::iterator alphaBegin;

	TransparencyMode transMode;
	int transPassCount;

	ShaderProgram* defaultProgram;

	ShaderProgram* dpProgram;
	ShaderProgram* dpBlendLayerProgram;
	ShaderProgram* dpBlendFinalProgram;

	ShaderProgram* wavgProgram;
	ShaderProgram* wavgBlendFinalProgram;

	GLint mvpMatrixUniform, texturedUniform, materialColorUniform, vertexColorsUniform,
			textureUniform;
	GLint vertexAttrib, normalAttrib, colorAttrib, texCoordAttrib;

	GLuint dpFBOs[2];
	GLuint dpBlendFBO;
	GLuint dpDepthTexes[2];
	GLuint dpColorTexes[2];
	GLuint dpBlendTex;

	GLuint wavgFBO;
	GLuint wavgColorTex;
	GLuint wavgDepthComplexityTex;
	GLuint wavgDepthBufferTex;

	GLuint planeDataBuf;
};

#endif /* DEFAULTRENDERER_H_ */

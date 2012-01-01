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

#ifndef WEIGHTEDAVERAGEALGORITHM_H_
#define WEIGHTEDAVERAGEALGORITHM_H_

#include "TransparencyAlgorithm.h"
#include "../Shader.h"
#include <vector>

using std::vector;




class WeightedAverageAlgorithm : public TransparencyAlgorithm {
public:
	WeightedAverageAlgorithm() : inited(false) {}
	virtual void install();
	virtual void uninstall() {}
	virtual void installShaders(ShaderProgram* program);
	virtual void uninstallShaders(ShaderProgram* program);
	virtual void performPostLinkOperations();
	virtual bool performPerFrameInit();
	virtual void setupShaderUniforms(ShaderProgram* program);
	virtual void performPreRenderPass() {}
	virtual bool performPostRenderPass() { return false; }
	virtual void performPerFrameFinalization();

private:
	Shader* vertexShader;
	Shader* fragmentShader;
	Shader* finalVertexShader;
	Shader* finalFragmentShader;

	ShaderProgram* blendFinalProgram;

	GLuint fbo;
	GLuint colorTex;
	GLuint depthComplexityTex;
	GLuint depthBufferTex;

	GLint colorTexUniform, dcTexUniform;
	GLint finalVertexAttrib;

	GLuint planeDataBuf;

	int viewW, viewH;

	bool inited;
};

#endif /* WEIGHTEDAVERAGEALGORITHM_H_ */

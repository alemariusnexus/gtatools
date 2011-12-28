/*
 * WeightedAverageAlgorithm.h
 *
 *  Created on: 11.09.2011
 *      Author: alemariusnexus
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

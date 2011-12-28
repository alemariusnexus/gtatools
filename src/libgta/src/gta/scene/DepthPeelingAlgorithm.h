/*
 * DepthPeelingAlgorithm.h
 *
 *  Created on: 11.09.2011
 *      Author: alemariusnexus
 */

#ifndef DEPTHPEELINGALGORITHM_H_
#define DEPTHPEELINGALGORITHM_H_

#include "TransparencyAlgorithm.h"
#include "../Shader.h"
#include <vector>

using std::vector;



class DepthPeelingAlgorithm : public TransparencyAlgorithm {
public:
	DepthPeelingAlgorithm() : numPasses(6), inited(false) {}
	virtual void install();
	virtual void uninstall() {}
	virtual void installShaders(ShaderProgram* program);
	virtual void uninstallShaders(ShaderProgram* program);
	virtual void performPostLinkOperations();
	virtual void performOpaqueRenderInit();
	virtual void setupOpaqueShaderUniforms(ShaderProgram* program) {}
	virtual void performOpaqueRenderFinalization() {}
	virtual bool performTransparentRenderInit();
	virtual void setupShaderUniforms(ShaderProgram* program);
	virtual void performPreRenderPass();
	virtual bool performPostRenderPass();
	virtual void performTransparentRenderFinalization();
	void setPassCount(int np) { numPasses = np; }
	int getPassCount() const { return numPasses; }

private:
	Shader* peelLayerVertexShader;
	Shader* peelLayerFragmentShader;
	Shader* blendLayerVertexShader;
	Shader* blendLayerFragmentShader;
	Shader* blendFinalVertexShader;
	Shader* blendFinalFragmentShader;

	ShaderProgram* blendLayerProgram;
	ShaderProgram* blendFinalProgram;

	GLint blendLayerTexUniform, blendFinalTexUniform;
	GLint blendLayerVertexAttrib, blendFinalVertexAttrib;

	GLint depthFormat;

	GLuint fbos[2];
	GLuint mainFBO;
	GLuint depthTexes[2];
	GLuint colorTexes[2];
	GLuint mainTexes[2];

	GLuint planeDataBuf;

	int numPasses;
	int currentPass;

	int viewW, viewH;

	bool inited;
};

#endif /* DEPTHPEELINGALGORITHM_H_ */

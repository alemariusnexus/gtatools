/*
 * BasicTransparencyAlgorithm.h
 *
 *  Created on: 22.09.2011
 *      Author: alemariusnexus
 */

#ifndef BASICTRANSPARENCYALGORITHM_H_
#define BASICTRANSPARENCYALGORITHM_H_

#include "TransparencyAlgorithm.h"


class BasicTransparencyAlgorithm : public TransparencyAlgorithm {
public:
	BasicTransparencyAlgorithm() : display(true), inited(false) {}
	virtual void install();
	virtual void uninstall() {}
	virtual void installShaders(ShaderProgram* program);
	virtual void uninstallShaders(ShaderProgram* program);
	virtual void performPostLinkOperations() {}
	virtual bool isOpaqueProcessedFirst() const { return true; }
	virtual void performOpaqueRenderInit() {}
	virtual void setupOpaqueShaderUniforms(ShaderProgram* program) {}
	virtual void performOpaqueRenderFinalization() {}
	virtual bool performTransparentRenderInit() { return display; }
	virtual void setupShaderUniforms(ShaderProgram* program) {}
	virtual void performPreRenderPass() {}
	virtual bool performPostRenderPass() { return false; }
	virtual void performTransparentRenderFinalization() {}
	void setDisplay(bool d) { display = d; }

private:
	Shader* vertexDefaultShader;
	Shader* fragmentDefaultShader;
	bool display;
	bool inited;
};

#endif /* BASICTRANSPARENCYALGORITHM_H_ */

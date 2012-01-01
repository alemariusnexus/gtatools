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

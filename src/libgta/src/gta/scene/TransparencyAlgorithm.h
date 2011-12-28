/*
 * TransparencyAlgorithm.h
 *
 *  Created on: 11.09.2011
 *      Author: alemariusnexus
 */

#ifndef TRANSPARENCYALGORITHM_H_
#define TRANSPARENCYALGORITHM_H_

#include <gta/config.h>
#include "../ShaderProgram.h"



/**	\brief Represents a common interface for certain order independent transparency (OIT) algorithms.
 *
 * 	An algorithm implemented using this interface is used as in the following pseudocode:
 *
 * 	\code
 * 	TransparencyAlgorithm* algo;
 *
 * 	// ***** Initialization (done only once during initialization of the renderer) *****
 * 	algo->init();
 * 	foreach (ShaderProgram* program : programs) {
 * 		algo->addShaders(program);
 * 		program->link();
 * 	}
 *
 * 	algo->performPostLinkOperations();
 *
 *	// ...
 *
 *
 *
 * 	// ***** Per-frame rendering code *****
 *
 * 	if (algo->isOpaqueProcessedFirst()) {
 * 		RenderOpaqueObjects();
 * 	}
 *
 *	if (algo->performPerFrameInit()) {
 * 		bool running = true;
 *
 * 		while (running) {
 * 			algo->performPreRenderPass();
 *
 *	 		foreach (SceneObject* obj : objects) {
 * 				ShaderProgram* program = GetProgramForObject(obj);
 *				if (!program->isCurrent()) {
 *					program->makeCurrent();
 *					algo->setupShaderUniforms(program);
 *				}
 *
 *				RenderTransparentObject(obj);
 * 			}
 *
 * 			running = algo->performPostRenderPass();
 * 		}
 * 	}
 *
 * 	algo->performPerFrameFinalization();
 *
 * 	if (!algo->isOpaqueProcessedFirst()) {
 * 		RenderOpaqueObjects();
 * 	}
 * 	\endcode
 */
class TransparencyAlgorithm {
public:
	virtual void install() = 0;
	virtual void uninstall() = 0;
	virtual void installShaders(ShaderProgram* program) = 0;
	virtual void uninstallShaders(ShaderProgram* program) = 0;
	virtual void performPostLinkOperations() = 0;
	virtual bool isOpaqueProcessedFirst() const { return true; }
	virtual void performOpaqueRenderInit() = 0;
	virtual void setupOpaqueShaderUniforms(ShaderProgram* program) = 0;
	virtual void performOpaqueRenderFinalization() = 0;
	virtual bool performTransparentRenderInit() = 0;
	virtual void setupShaderUniforms(ShaderProgram* program) = 0;
	virtual void performPreRenderPass() = 0;
	virtual bool performPostRenderPass() = 0;
	virtual void performTransparentRenderFinalization() = 0;
};

#endif /* TRANSPARENCYALGORITHM_H_ */

/*
 * Renderer.h
 *
 *  Created on: 10.09.2011
 *      Author: alemariusnexus
 */

#ifndef RENDERER_H_
#define RENDERER_H_

#include "VisualSceneObject.h"



class Renderer {
public:
	virtual void enqueueForRendering(VisualSceneObject* obj) = 0;
	virtual void render() = 0;
};

#endif /* RENDERER_H_ */

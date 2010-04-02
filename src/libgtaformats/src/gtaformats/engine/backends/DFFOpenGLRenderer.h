/*
 * DFFOpenGLRenderer.h
 *
 *  Created on: 17.03.2010
 *      Author: alemariusnexus
 */

#ifndef DFFOPENGLRENDERER_H_
#define DFFOPENGLRENDERER_H_

#include "../DFFRendererBackend.h"


class DFFOpenGLRenderer : public DFFRendererBackend
{
public:
	virtual void applyTexture(TXDTexture* texture, uint8_t* data);
	virtual void renderGeometryPart(DFFGeometry* geometry, DFFGeometryPart* part);
	virtual void applyFrameTransformation(DFFFrame* frame);
	virtual void endFrameTransformation(DFFFrame* frame);
};

#endif /* DFFOPENGLRENDERER_H_ */

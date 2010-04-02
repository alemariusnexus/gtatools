/*
 * DFFRendererBackend.h
 *
 *  Created on: 17.03.2010
 *      Author: alemariusnexus
 */

#ifndef DFFRENDERERBACKEND_H_
#define DFFRENDERERBACKEND_H_

#include "../gtatxd.h"
#include "../gtadff.h"


class DFFRendererBackend
{
public:
	virtual void applyTexture(TXDTexture* texture, uint8_t* data) = 0;
	virtual void renderGeometryPart(DFFGeometry* geometry, DFFGeometryPart* part) = 0;
	virtual void applyFrameTransformation(DFFFrame* frame) = 0;
	virtual void endFrameTransformation(DFFFrame* frame) = 0;
};

#endif /* DFFRENDERERBACKEND_H_ */

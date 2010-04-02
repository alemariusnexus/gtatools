/*
 * OpenGLDFFRenderer.h
 *
 *  Created on: 14.03.2010
 *      Author: alemariusnexus
 */

#ifndef OPENGLDFFRENDERER_H_
#define OPENGLDFFRENDERER_H_

//#include "TextureProvider.h"
#include "DefaultTextureProvider.h"
#include "DFFRendererBackend.h"
#include "../gtadff.h"


class DFFRenderer
{
public:
	DFFRenderer(TextureProvider* texProv, DFFRendererBackend* backend);
	void renderGeometry(DFFGeometry* geom, bool transform = true);
	void renderGeometryPart(DFFGeometry* geom, DFFGeometryPart* part);
	void renderStaticMesh(DFFMesh* mesh);
	void renderDummy(DFFMesh* mesh, const char* dummyName, const char* geomName);
	void applyFrameTranslation(DFFFrame* frame);

private:
	TextureProvider* textureProvider;
	DFFRendererBackend* backend;
};

#endif /* OPENGLDFFRENDERER_H_ */

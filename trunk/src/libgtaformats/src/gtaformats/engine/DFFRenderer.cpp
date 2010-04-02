/*
 * OpenGLDFFRenderer.cpp
 *
 *  Created on: 14.03.2010
 *      Author: alemariusnexus
 */

#include "DFFRenderer.h"
#include <gtaformats/gtatxd.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <cstring>




DFFRenderer::DFFRenderer(TextureProvider* texProv, DFFRendererBackend* backend)
		: textureProvider(texProv), backend(backend)
{
}


void DFFRenderer::renderStaticMesh(DFFMesh* mesh)
{
	DFFGeometry** geoms = mesh->getGeometries();

	for (int32_t i = 0 ; i < mesh->getGeometryCount() ; i++) {
		DFFGeometry* geom = geoms[i];
		DFFFrame* frame = geom->getAssociatedFrame();

		if (	strstr(frame->getName(), "_dummy") == NULL
				&&  strstr(frame->getName(), "_vlo") == NULL
		) {
			renderGeometry(geom);
		}
	}
}


void DFFRenderer::renderGeometry(DFFGeometry* geom, bool transform)
{
	DFFGeometryPart** parts = geom->getParts();

	if (transform) {
		backend->applyFrameTransformation(geom->getAssociatedFrame());
	}

	for (int32_t j = 0 ; j < geom->getPartCount() ; j++) {
		DFFGeometryPart* part = parts[j];
		renderGeometryPart(geom, part);
	}

	if (transform) {
		backend->endFrameTransformation(geom->getAssociatedFrame());
	}
}


void DFFRenderer::renderGeometryPart(DFFGeometry* geom, DFFGeometryPart* part)
{
	DFFMaterial* mat = part->getMaterial();

	if (mat) {
		if (mat->getTextureCount() > 0) {
			DFFTexture* tex = mat->getTextures()[0];

			TXDTexture* txdTex;
			uint8_t* rawData = NULL;
			int16_t width = 0, height = 0;
			bool texFound = textureProvider->findTexture(tex->getDiffuseName(), txdTex, rawData,
					width, height);

			if (texFound) {
				backend->applyTexture(txdTex, rawData);
			} else {
				printf("WARNING: Texture not found: %s\n", tex->getDiffuseName());
			}
		}
	}

	backend->renderGeometryPart(geom, part);
}


void DFFRenderer::renderDummy(DFFMesh* mesh, const char* dummyName, const char* geomName)
{
	DFFFrame* dummy = mesh->getFrame(dummyName);
	backend->applyFrameTransformation(dummy);
	renderGeometry(mesh->getGeometry(geomName));
	backend->endFrameTransformation(dummy);
}

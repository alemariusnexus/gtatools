/*
	Copyright 2010 David Lerch

	This file is part of gtaformats.

	gtaformats is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	gtaformats is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with gtaformats.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "DFFOpenGLRenderer.h"

#ifdef _WIN32
	#include <windows.h>
#endif

#include "../util/util.h"
#include <GL/gl.h>
#include <GL/glu.h>


DFFOpenGLRenderer::DFFOpenGLRenderer(OpenGLResourceManager* resourceManager)
		: resourceManager(resourceManager), texturing(true)
{
}


void DFFOpenGLRenderer::applyTexture(const char* name)
{

	resourceManager->bindTexture(name);
}


void DFFOpenGLRenderer::applyFrameTransformation(DFFFrame* frame)
{
	Vector3 trans = frame->getTranslation();
	Matrix3 rot = frame->getRotation();

	float transMatrix[] = {
			rot[0], rot[3], rot[6], 0,
			rot[1], rot[4], rot[7], 0,
			rot[2], rot[5], rot[8], 0,
			trans[0], trans[2], trans[1], 1
	};

	glMultMatrixf(transMatrix);
}


void DFFOpenGLRenderer::renderStaticMesh(DFFMesh* mesh)
{
	DFFMesh::GeometryIterator it;

	for (it = mesh->getGeometryBegin() ; it != mesh->getGeometryEnd() ; it++) {
		DFFGeometry* geom = *it;
		DFFFrame* frame = geom->getAssociatedFrame();

		renderGeometry(geom);

		const char* fname = frame->getName();
		size_t len = strlen(fname);

		if (	(len < 4  ||  strcmp((fname+len-4), "_vlo") != 0)
				&&  (len < 6  ||  strcmp((fname+len-6), "_dummy") != 0)
		) {
			renderGeometry(geom);
		}
	}
}


void DFFOpenGLRenderer::renderGeometry(DFFGeometry* geom, bool transform)
{
	if (transform) {
		glPushMatrix();
		applyFrameTransformation(geom->getAssociatedFrame());
	}

	DFFGeometry::PartIterator it;

	for (it = geom->getPartBegin() ; it != geom->getPartEnd() ; it++) {
		DFFGeometryPart* part = *it;
		renderGeometryPart(geom, part);
	}

	if (transform) {
		glPopMatrix();
	}
}


void DFFOpenGLRenderer::renderGeometryPart(DFFGeometry* geom, DFFGeometryPart* part)
{
	DFFMaterial* mat = part->getMaterial();

	if (mat) {
		if (mat->getTextureCount() > 0) {
			DFFTexture* tex = mat->getTexture(0);

			if (texturing) {
				char* lName = new char[strlen(tex->getDiffuseName())+1];
				strtolower(lName, tex->getDiffuseName());

				applyTexture(lName);

				delete[] lName;
			}
		}
	}

	float* verts = geom->getVertices();
	float* normals = geom->getNormals();
	uint8_t* colors = geom->getVertexColors();
	float* uvSets = geom->getUVCoordSets();

	int32_t* indices = part->getIndices();

	glBegin(geom->isTriangleStripFormat() ? GL_TRIANGLE_STRIP : GL_TRIANGLES);

	for (int32_t k = 0 ; k < part->getIndexCount() ; k++) {
		int32_t idx = indices[k];

		if (normals) {
			glNormal3f(normals[idx*3], normals[idx*3 + 1], normals[idx*3 + 2]);
		}
		if (colors) {
			glColor4f(colors[idx*4], colors[idx*4 + 1], colors[idx*4 + 2], colors[idx*4 + 3]);
		}
		if (uvSets) {
			glTexCoord2f(uvSets[idx*2], uvSets[idx*2 + 1]);
		}

		glVertex3f(verts[idx*3], verts[idx*3 + 1], verts[idx*3 + 2]);
	}

	glEnd();
}


void DFFOpenGLRenderer::renderDummy(DFFMesh* mesh, const char* dummyName, const char* geomName)
{
	DFFFrame* dummy = mesh->getFrame(dummyName);
	glPushMatrix();
	applyFrameTransformation(dummy);
	renderGeometry(mesh->getGeometry(geomName));
	glPopMatrix();
}

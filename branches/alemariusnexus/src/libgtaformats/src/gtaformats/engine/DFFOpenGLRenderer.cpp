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
		: resourceManager(resourceManager)
{
}


//void DFFOpenGLRenderer::applyTexture(TXDTexture* texture, uint8_t* data)
void DFFOpenGLRenderer::applyTexture(const char* name)
{

	resourceManager->bindTexture(name);


	/*GLenum texFormat;
	uint8_t* data = rawData;

	bool nativeFormat = false;
	int8_t bpp = txdTex->getBytesPerPixel();

	if (	txdTex->getCompression() == TXD_COMPRESSION_NONE
			&&  (txdTex->getRasterFormatExtension() & TXD_FORMAT_EXT_PAL4) == 0
			&&  (txdTex->getRasterFormatExtension() & TXD_FORMAT_EXT_PAL8) == 0
	) {
		nativeFormat = true;

		switch (txdTex->getRasterFormat()) {
		case TXD_FORMAT_B8G8R8:
			texFormat = GL_BGR;
			if (bpp != 3)
				nativeFormat = false;
			break;
		case TXD_FORMAT_B8G8R8A8:
			texFormat = GL_BGRA;
			if (bpp != 4)
				nativeFormat = false;
			break;
		case TXD_FORMAT_LUM8:
			texFormat = GL_LUMINANCE;
			if (bpp != 1)
				nativeFormat = false;
			break;
		default:
			nativeFormat = false;
		}
	}

	if (!nativeFormat) {
		texFormat = GL_RGBA;
		data = new uint8_t[width * height * 4];
		txdTex->convert(data, rawData, MIRROR_NONE);
		delete[] rawData;
	}*/

	//cache->applyCachedTexture();
}


void DFFOpenGLRenderer::applyFrameTransformation(DFFFrame* frame)
{
	const float* trans = frame->getTranslation();
	const float* rot = frame->getRotation();

	/*float transMatrix[] = {
			1,		1,		1,		trans[0],
			1,		1,		1,		trans[2],
			1,		1,		1,		trans[1],
			1,		1,		1,		1
	};*/

	float transMatrix[] = {
			rot[0], rot[3], rot[6], 0,
			rot[1], rot[4], rot[7], 0,
			rot[2], rot[5], rot[8], 0,
			trans[0], trans[2], trans[1], 1
	};

	/*float transMatrix[] = {
			rot[0], rot[2], rot[1], 0,
			rot[3], rot[5], rot[4], 0,
			rot[6], rot[8], rot[7], 0,
			trans[0], trans[2], trans[1], 1
	};*/

	/*float transMatrix[] = {
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			trans[0], trans[2], trans[1], 1
	};*/

	/*float transMatrix[] = {
			rot[0],	rot[2],	rot[1],	trans[0],
			rot[3],	rot[5],	rot[4],	trans[2],
			rot[6],	rot[8],	rot[7],	trans[1],
			0,		0,		0,		1
	};*/

	glMultMatrixf(transMatrix);
}


void DFFOpenGLRenderer::renderStaticMesh(DFFMesh* mesh)
{
	DFFGeometry** geoms = mesh->getGeometries();

	for (int32_t i = 0 ; i < mesh->getGeometryCount() ; i++) {
		DFFGeometry* geom = geoms[i];
		DFFFrame* frame = geom->getAssociatedFrame();

		renderGeometry(geom);

		char* fname = frame->getName();
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
	DFFGeometryPart** parts = geom->getParts();

	if (transform) {
		glPushMatrix();
		applyFrameTransformation(geom->getAssociatedFrame());
	}

	for (int32_t j = 0 ; j < geom->getPartCount() ; j++) {
		DFFGeometryPart* part = parts[j];
		renderGeometryPart(geom, part);
	}

	if (transform) {
		//backend->endFrameTransformation(geom->getAssociatedFrame());
		glPopMatrix();
	}
}


void DFFOpenGLRenderer::renderGeometryPart(DFFGeometry* geom, DFFGeometryPart* part)
{
	DFFMaterial* mat = part->getMaterial();

	if (mat) {
		if (mat->getTextureCount() > 0) {
			DFFTexture* tex = mat->getTextures()[0];

			/*if (!cache->applyCachedTexture(tex->getDiffuseName())) {
				printf("WARNING: Texture not found: %s\n", tex->getDiffuseName());
			}*/

			char* lName = new char[strlen(tex->getDiffuseName())+1];
			strtolower(lName, tex->getDiffuseName());

			applyTexture(lName);

			delete[] lName;

			/*TXDTexture* txdTex;
			uint8_t* rawData = NULL;
			bool texFound = textureProvider->findTexture(tex->getDiffuseName(), txdTex, rawData);

			if (texFound) {
				applyTexture(txdTex, rawData);
			} else {
				printf("WARNING: Texture not found: %s\n", tex->getDiffuseName());
			}*/
		}
	}

	//backend->renderGeometryPart(geom, part);

	float* verts = geom->getVertices();
	float* normals = geom->getNormals();
	uint8_t* colors = geom->getVertexColors();
	float** uvSets = geom->getUVCoordSets();

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
			glTexCoord2f(uvSets[0][idx*2], uvSets[0][idx*2 + 1]);
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

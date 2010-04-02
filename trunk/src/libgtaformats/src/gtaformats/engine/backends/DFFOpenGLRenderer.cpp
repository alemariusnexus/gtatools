/*
 * DFFOpenGLRenderer.cpp
 *
 *  Created on: 17.03.2010
 *      Author: alemariusnexus
 */

#include "DFFOpenGLRenderer.h"
#include <GL/gl.h>
#include <GL/glu.h>



void DFFOpenGLRenderer::applyTexture(TXDTexture* texture, uint8_t* rawData)
{
	GLenum texFormat;

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

	int16_t width = texture->getWidth();
	int16_t height = texture->getHeight();

	texFormat = GL_RGBA;
	uint8_t* data = new uint8_t[width * height * 4];
	texture->convert(data, rawData, MIRROR_NONE);
	delete[] rawData;

	GLuint texName = 0;
	glGenTextures(1, &texName);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	glBindTexture(GL_TEXTURE_2D, texName);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height,
			0, GL_RGBA, GL_UNSIGNED_BYTE, data);

	glBindTexture(GL_TEXTURE_2D, texName);
}


void DFFOpenGLRenderer::renderGeometryPart(DFFGeometry* geom, DFFGeometryPart* part)
{
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


void DFFOpenGLRenderer::applyFrameTransformation(DFFFrame* frame)
{
	const float* trans = frame->getTranslation();
	glPushMatrix();
	glTranslatef(trans[0], trans[1], trans[2]);
}


void DFFOpenGLRenderer::endFrameTransformation(DFFFrame* frame)
{
	glPopMatrix();
}

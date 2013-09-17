/*
	Copyright 2010-2013 David "Alemarius Nexus" Lerch

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

#include "CEGUIGL2GeometryBuffer.h"
#include "CEGUIGL2Renderer.h"
#include "../gl.h"
#include "../GLException.h"



CEGUIGL2GeometryBuffer::CEGUIGL2GeometryBuffer(CEGUIGL2Renderer* renderer)
		: renderer(renderer), trans(0, 0, 0), rot(0, 0, 0), pivot(0, 0, 0), activeTex(NULL),
		  mvMatrixValid(false), clipRegion(-1.0f, -1.0f, -1.0f, -1.0f), effect(NULL), vertexCount(0)
{
}


CEGUIGL2GeometryBuffer::~CEGUIGL2GeometryBuffer()
{
	reset();
}


void CEGUIGL2GeometryBuffer::draw() const
{
	updateMVMatrix();

	Rect targetArea = renderer->getActiveRenderTarget()->getArea();

	GLint x, y, w, h;

	x = (GLint) clipRegion.d_left;
	y = (GLint) (targetArea.getHeight() - clipRegion.d_bottom);
	w = (GLint) clipRegion.getWidth();
	h = (GLint) clipRegion.getHeight();

	glScissor(x, y, w, h);

	Matrix4 mvpMatrix = renderer->getActiveRenderTarget()->getProjectionMatrix()
			* mvMatrix;

	glUniformMatrix4fv(renderer->mvpMatrixUniform, 1, GL_FALSE, mvpMatrix.toArray());

	renderer->setupBlendMode(d_blendMode);

	int passes = effect ? effect->getPassCount() : 1;

	for (int i = 0 ; i < passes ; i++) {
		if (effect)
			effect->performPreRenderFunctions(i);

		BatchList::const_iterator it;
		for (it = batches.begin() ; it != batches.end() ; it++) {
			CEGUIGL2Batch batch = *it;

			if (batch.tex) {
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, batch.tex->getGLTexture());
				glUniform1i(renderer->texturedUniform, 1);
				glUniform1i(renderer->textureUniform, 0);
			} else {
				glUniform1i(renderer->texturedUniform, 0);
			}

			glBindBuffer(GL_ARRAY_BUFFER, batch.dataBuf);

			glVertexAttribPointer(renderer->vertexAttrib, 3, GL_FLOAT, GL_FALSE, 24, (void*) 0);
			glVertexAttribPointer(renderer->texCoordAttrib, 2, GL_FLOAT, GL_FALSE, 24, (void*) 12);
			glVertexAttribPointer(renderer->colorAttrib, 4, GL_UNSIGNED_BYTE, GL_TRUE, 24, (void*) 20);

			glDrawArrays(GL_TRIANGLES, 0, batch.vertices.size());
		}
	}

	if (effect)
		effect->performPostRenderFunctions();
}


void CEGUIGL2GeometryBuffer::setTranslation(const CEGUI::Vector3& t)
{
	trans = t;
	mvMatrixValid = false;
}


void CEGUIGL2GeometryBuffer::setRotation(const CEGUI::Vector3& r)
{
	if (r.d_x != 0.0f  ||  r.d_y != 0.0f  ||  r.d_z != 0.0f) {
		fprintf(stderr, "WARNING: A CEGUIGL2GeometryBuffer was rotated. Rotation is not fully supported yet and "
				"will break hit testing!\n");
	}

	rot = r;
	mvMatrixValid = false;
}


void CEGUIGL2GeometryBuffer::setPivot(const CEGUI::Vector3& p)
{
	pivot = p;
	mvMatrixValid = false;
}


void CEGUIGL2GeometryBuffer::updateMVMatrix() const
{
	if (!mvMatrixValid) {
		Vector3 transPlusPivot(trans.d_x+pivot.d_x, trans.d_y+pivot.d_y, trans.d_z+pivot.d_z);
		mvMatrix = Matrix4::translation(transPlusPivot)
				* Matrix4::rotationZ(rot.d_z * 0.0174532925f)
				* Matrix4::rotationY(rot.d_y * 0.0174532925f)
				* Matrix4::rotationX(rot.d_x * 0.0174532925f)
				* Matrix4::translation(Vector3(-pivot.d_x, -pivot.d_y, -pivot.d_z));
		mvMatrixValid = true;
	}
}


void CEGUIGL2GeometryBuffer::setClippingRegion(const Rect& r)
{
	clipRegion = r;
}


void CEGUIGL2GeometryBuffer::appendVertex(const Vertex& v)
{
	appendGeometry(&v, 1);
}


void CEGUIGL2GeometryBuffer::appendGeometry(const Vertex* const vbuf, uint vcount)
{
	updateBatches(vcount);

	CEGUIGL2Batch& batch = batches.back();

	glBindBuffer(GL_ARRAY_BUFFER, batch.dataBuf);

	for (uint i = 0 ; i < vcount ; i++) {
		const Vertex& v = vbuf[i];
		CEGUIGL2Vertex gv;
		memcpy(&gv, &v, 20);
		gv.c[0] = (uint8_t) (v.colour_val.getRed() * 255);
		gv.c[1] = (uint8_t) (v.colour_val.getGreen() * 255);
		gv.c[2] = (uint8_t) (v.colour_val.getBlue() * 255);
		gv.c[3] = (uint8_t) (v.colour_val.getAlpha() * 255);
		batch.vertices.push_back(gv);
	}

#ifndef GTA_USE_OPENGL_ES
	glBufferData(GL_ARRAY_BUFFER, batch.vertices.size()*24, &batch.vertices.front(), GL_DYNAMIC_READ);
#else
	// There is no GL_DYNAMIC_READ in GLES2
	glBufferData(GL_ARRAY_BUFFER, batch.vertices.size()*24, &batch.vertices.front(), GL_DYNAMIC_DRAW);
#endif
}


void CEGUIGL2GeometryBuffer::setActiveTexture(CEGUI::Texture* tex)
{
	activeTex = (CEGUIGL2Texture*) tex;
}


void CEGUIGL2GeometryBuffer::reset()
{
	BatchList::iterator it;
	for (it = batches.begin() ; it != batches.end() ; it++) {
		CEGUIGL2Batch& batch = *it;
		glDeleteBuffers(1, &batch.dataBuf);
	}

	batches.clear();
	activeTex = NULL;
	vertexCount = 0;
}


void CEGUIGL2GeometryBuffer::updateBatches(uint vcountAdd)
{
	if (batches.size() == 0  ||  batches.back().tex != activeTex) {
		GLuint dataBuf;
		glGenBuffers(1, &dataBuf);
		batches.push_back(CEGUIGL2Batch(dataBuf, activeTex));
	}

	vertexCount += vcountAdd;
}

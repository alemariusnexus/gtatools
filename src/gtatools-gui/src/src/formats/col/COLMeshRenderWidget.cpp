/*
	Copyright 2010-2011 David "Alemarius Nexus" Lerch

	This file is part of gtatools-gui.

	gtatools-gui is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	gtatools-gui is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with gtatools-gui.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "COLMeshRenderWidget.h"
#include <gta/COLMeshConverter.h>
#include <gta/StaticMeshPointer.h>
#include <gta/NullTextureSource.h>
#include <gta/Submesh.h>
#include <QtGui/QMouseEvent>
#include <QtGui/QImage>
#include <QtGui/QImageWriter>
#include <fstream>
#include <gta/GLException.h>
#include "../../System.h"

using std::ofstream;



COLMeshRenderWidget::COLMeshRenderWidget(QWidget* parent)
		: GLBaseWidget(parent), item(NULL), pickItem(NULL), pickedFace(-1), wireframe(false)
{

}


void COLMeshRenderWidget::render(const float* vertices, int32_t vertexCount, const COLFace* faces,
		int32_t faceCount)
{
	try {
		GLBaseWidget::initializeGL();

		makeCurrent();

		COLMeshConverter conv;

		// Build visible mesh
		int modelVertexCount, modelIndexCount;
		float* modelVertices;
		uint8_t* modelColors;
		int32_t* modelIndices;
		conv.convertVertexModel(vertices, vertexCount, faces, faceCount, modelVertexCount, modelVertices, modelColors,
				modelIndices, modelIndexCount);

		Mesh* mesh = new Mesh(modelVertexCount, VertexFormatTriangles, MeshVertexColors, modelVertices, NULL,
				NULL, modelColors);
		Submesh* submesh = new Submesh(mesh, modelIndexCount, modelIndices);
		mesh->addSubmesh(submesh);
		colors = modelColors;

		if (item)
			delete item;
		item = new ItemDefinition(new StaticMeshPointer(mesh), new NullTextureSource, 5000.0f);


		// Build picking mesh
		Mesh* pickMesh = conv.convert(vertices, vertexCount, faces, faceCount);
		pickMesh->bindDataBuffer();
		int colorOffs = pickMesh->getVertexColorOffset();
		int vcount = pickMesh->getVertexCount();

		uint32_t* pickColors = new uint32_t[vcount];
		for (uint32_t i = 0 ; i < faceCount ; i++) {
			pickColors[i*3] = i+1;
			pickColors[i*3+1] = i+1;
			pickColors[i*3+2] = i+1;
		}

		glBufferSubData(GL_ARRAY_BUFFER, colorOffs, vcount*4, pickColors);

		if (pickItem)
			delete pickItem;

		pickItem = new ItemDefinition(new StaticMeshPointer(pickMesh), new NullTextureSource, 5000.0f);

		updateGL();
	} catch (Exception& ex) {
		System::getInstance()->unhandeledException(ex);
	}
}


void COLMeshRenderWidget::initializeGL()
{
	GLBaseWidget::initializeGL();

#ifdef GTATOOLS_GUI_USE_OPENGL_ES
	QFile vfile(":/src/shader/vertex_col_es.glsl");
	QFile ffile(":/src/shader/fragment_col_es.glsl");
#else
	QFile vfile(":/src/shader/vertex_col.glsl");
	QFile ffile(":/src/shader/fragment_col.glsl");
#endif

	initializeShaders(vfile, ffile);

	glEnable(GL_DEPTH_TEST);
}


void COLMeshRenderWidget::resizeGL(int w, int h)
{
	GLBaseWidget::resizeGL(w, h);
}


void COLMeshRenderWidget::paintGL()
{
	GLBaseWidget::paintGL();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

#ifndef GTATOOLS_GUI_USE_OPENGL_ES
	glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);
#endif

	if (item) {
		item->render();
	}
}


void COLMeshRenderWidget::mouseDoubleClickEvent(QMouseEvent* evt)
{
	if (pickItem) {
		makeCurrent();

		int x = evt->x();
		int y = height() - evt->y();

#ifndef GTATOOLS_GUI_USE_OPENGL_ES
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#endif

		GLBaseWidget::paintGL();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

#ifndef GTATOOLS_GUI_USE_OPENGL_ES
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#endif

		pickItem->render();

		int32_t pixel = 0;
		glReadPixels(x, y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &pixel);
		pixel &= 0xFFFFFF;
		pixel--;

		setSelectedFace(pixel);

		updateGL();
	}
}


void COLMeshRenderWidget::setSelectedFace(int index)
{
	if (item) {
		int prevIdx = pickedFace;

		Mesh* mesh = **item->getMeshPointer();

		if (mesh->getVertexCount() != 0) {
			mesh->bindDataBuffer();
			
			int vcount = mesh->getVertexCount();
			int colorOffs = mesh->getVertexColorOffset();

			if (pickedFace != -1) {
				// Reset the previously picked face's color
				glBufferSubData(GL_ARRAY_BUFFER, colorOffs + pickedFace*3*4, 4, pickedFaceRealColor);
				glBufferSubData(GL_ARRAY_BUFFER, colorOffs + pickedFace*3*4 + 4, 4, pickedFaceRealColor);
				glBufferSubData(GL_ARRAY_BUFFER, colorOffs + pickedFace*3*4 + 8, 4, pickedFaceRealColor);
			}

			if (index != -1) {
				// Save the picked face's real color
				memcpy(pickedFaceRealColor, colors + index*3*4, 4);

				// And set the picked face's color to the 'picked-color'
				uint8_t pickColor[] = {255, 255, 255, 255};
				glBufferSubData(GL_ARRAY_BUFFER, colorOffs + index*3*4, 4, pickColor);
				glBufferSubData(GL_ARRAY_BUFFER, colorOffs + index*3*4 + 4, 4, pickColor);
				glBufferSubData(GL_ARRAY_BUFFER, colorOffs + index*3*4 + 8, 4, pickColor);
			}

			pickedFace = index;

			emit faceSelectionChanged(prevIdx, index);
		}
	}
}

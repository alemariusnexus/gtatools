/*
	Copyright 2010-2014 David "Alemarius Nexus" Lerch

	This file is part of gtatools-test.

	gtatools-test is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	gtatools-test is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with gtatools-test.  If not, see <http://www.gnu.org/licenses/>.

	Additional permissions are granted, which are listed in the file
	GPLADDITIONS.
 */

#include "BulletGLDebugDraw.h"
#include <gta/gl.h>
#include <res_btgldebugdraw_vertex_shader.h>
#include <res_btgldebugdraw_fragment_shader.h>
#include <iostream>
#include <gta/Engine.h>


#define BT_GL_DEBUG_DRAW_REALLOC_SIZE 65536




BulletGLDebugDraw::BulletGLDebugDraw()
		: lineVerticesAllocCount(BT_GL_DEBUG_DRAW_REALLOC_SIZE),
		  lineVerticesOffset(0),
		  lineVertices(new float[BT_GL_DEBUG_DRAW_REALLOC_SIZE*3]),
		  lineColors(new float[BT_GL_DEBUG_DRAW_REALLOC_SIZE*3])
{
	vertexShader = new Shader(GL_VERTEX_SHADER);
	vertexShader->loadSourceCode(CString((const char*) res_btgldebugdraw_vertex_shader_data,
			sizeof(res_btgldebugdraw_vertex_shader_data)));
	vertexShader->compile();

	fragmentShader = new Shader(GL_FRAGMENT_SHADER);
	fragmentShader->loadSourceCode(CString((const char*) res_btgldebugdraw_fragment_shader_data,
			sizeof(res_btgldebugdraw_fragment_shader_data)));
	fragmentShader->compile();

	program = new ShaderProgram;
	program->attachShader(vertexShader);
	program->attachShader(fragmentShader);
	program->link();

	program->makeCurrent();

	vertexAttrib = program->getAttributeLocation("Vertex");
	colorAttrib = program->getAttributeLocation("Color");
	vertexColorsUniform = program->getUniformLocation("VertexColors");
	texturedUniform = program->getUniformLocation("Textured");
	mvpMatrixUniform = program->getUniformLocation("MVPMatrix");

	glEnableVertexAttribArray(vertexAttrib);
	glEnableVertexAttribArray(colorAttrib);
}


BulletGLDebugDraw::~BulletGLDebugDraw()
{
	delete[] lineVertices;
	delete[] lineColors;

	delete program;
	delete vertexShader;
	delete fragmentShader;
}


void BulletGLDebugDraw::drawLine(const btVector3& from,const btVector3& to,const btVector3& fromColor, const btVector3& toColor)
{
	glBegin(GL_LINES);
		glColor3f(fromColor.getX(), fromColor.getY(), fromColor.getZ());
		glVertex3d(from.getX(), from.getY(), from.getZ());
		glColor3f(toColor.getX(), toColor.getY(), toColor.getZ());
		glVertex3d(to.getX(), to.getY(), to.getZ());
	glEnd();
}


void BulletGLDebugDraw::drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
{
	/*float vertices[] = {
			from.getX(), from.getY(), from.getZ(),
			to.getX(), to.getY(), to.getZ()
	};
	float colors[] = {
			color.getX(), color.getY(), color.getZ(),
			color.getX(), color.getY(), color.getZ()
	};

	reallocLineVertices(2);

	memcpy(lineVertices + lineVerticesOffset*3, vertices, 2*3*sizeof(float));
	memcpy(lineColors + lineVerticesOffset*3, colors, 2*3*sizeof(float));

	lineVerticesOffset += 2;*/

	drawLine(from, to, color, color);
}


void BulletGLDebugDraw::drawTriangle(const btVector3& a,const btVector3& b,const btVector3& c,const btVector3& color,btScalar alpha)
{
	{
		const btVector3	n=btCross(b-a,c-a).normalized();
		glBegin(GL_TRIANGLES);
		glColor4f(color.getX(), color.getY(), color.getZ(),alpha);
		glNormal3d(n.getX(),n.getY(),n.getZ());
		glVertex3d(a.getX(),a.getY(),a.getZ());
		glVertex3d(b.getX(),b.getY(),b.getZ());
		glVertex3d(c.getX(),c.getY(),c.getZ());
		glEnd();
	}
}


void BulletGLDebugDraw::drawContactPoint(const btVector3& pointOnB, const btVector3& normalOnB,
		btScalar distance, int lifeTime, const btVector3& color)
{
	const btVector3& from = pointOnB;
	btVector3 to = pointOnB + normalOnB*distance;

	drawLine(from, to, color);
}


void BulletGLDebugDraw::reportErrorWarning(const char* msg)
{
	std::cerr << msg << std::endl;
}


void BulletGLDebugDraw::draw3dText(const btVector3& location, const char* textString)
{
	// TODO
}


void BulletGLDebugDraw::drawTransform(const btTransform& trans, btScalar orthoLen)
{
	/*btVector3 pos = trans.getOrigin();
	printf("Draw Transform: %f, %f, %f\n", pos.getX(), pos.getY(), pos.getZ());*/
}


void BulletGLDebugDraw::reallocLineVertices(int numAddVertices)
{
	if (lineVerticesOffset + numAddVertices > lineVerticesAllocCount) {
		float* newVertices = new float[lineVerticesAllocCount*3 + BT_GL_DEBUG_DRAW_REALLOC_SIZE*3];
		float* newColors = new float[lineVerticesAllocCount*3 + BT_GL_DEBUG_DRAW_REALLOC_SIZE*3];
		memcpy(newVertices, lineVertices, lineVerticesOffset*3*sizeof(float));
		memcpy(newColors, lineColors, lineVerticesOffset*3*sizeof(float));
		delete[] lineVertices;
		delete[] lineColors;
		lineVertices = newVertices;
		lineColors = newColors;
		lineVerticesAllocCount += BT_GL_DEBUG_DRAW_REALLOC_SIZE;
	}
}


void BulletGLDebugDraw::flush()
{
	/*// TODO Using glBufferData with zero size seems to cause problems at least with the PowerVR X11 Linux
	// driver in GLES2
	if (lineVerticesOffset != 0) {
		uint32_t* indices = new uint32_t[lineVerticesOffset];

		for (int i = 0 ; i < lineVerticesOffset ; i++) {
			indices[i] = i;
		}

		//printf("%f, %f, %f\n", lineVertices[0], lineVertices[1], lineVertices[2]);

		GLuint dataBuffer, indexBuffer;
		glGenBuffers(1, &dataBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, dataBuffer);

		glBufferData(GL_ARRAY_BUFFER, lineVerticesOffset*3*sizeof(float) * 2, NULL, GL_STATIC_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, lineVerticesOffset*3*sizeof(float), lineVertices);
		glBufferSubData(GL_ARRAY_BUFFER, lineVerticesOffset*3*sizeof(float), lineVerticesOffset*3*sizeof(float),
				lineColors);

		glGenBuffers(1, &indexBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);

		glBufferData(GL_ELEMENT_ARRAY_BUFFER, lineVerticesOffset*sizeof(uint32_t), indices, GL_STATIC_DRAW);

		delete[] indices;

		glVertexAttribPointer(vertexAttrib, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0);
		glVertexAttribPointer(colorAttrib, 3, GL_FLOAT, GL_FALSE, 0,
				(void*) (lineVerticesOffset*3*sizeof(float)));

		glUniform1i(vertexColorsUniform, 1);
		glUniform1i(texturedUniform, 0);

		glDrawElements(GL_LINES, lineVerticesOffset, GL_UNSIGNED_INT, (void*) 0);

		glDeleteBuffers(1, &dataBuffer);
		glDeleteBuffers(1, &indexBuffer);
	}

	lineVerticesOffset = 0;*/
}


void BulletGLDebugDraw::update()
{
	//program->makeCurrent();

	glUseProgram(0);

	Engine* engine = Engine::getInstance();
	Camera* cam = engine->getCamera();

	Matrix4 pMatrix = engine->getProjectionMatrix();
	Matrix4 vMatrix = Matrix4::lookAt(cam->getTarget(), cam->getUp())
			* Matrix4::translation(-cam->getPosition());
	Matrix4 mvpMatrix = pMatrix * vMatrix;

	//glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, (float*) (&mvpMatrix));

	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(pMatrix.toArray());

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(vMatrix.toArray());
}

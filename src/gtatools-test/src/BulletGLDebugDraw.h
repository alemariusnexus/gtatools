/*
	Copyright 2010-2013 David "Alemarius Nexus" Lerch

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

#ifndef BULLETGLDEBUGDRAW_H_
#define BULLETGLDEBUGDRAW_H_

#include <btBulletDynamicsCommon.h>
#include <gta/ShaderProgram.h>


class BulletGLDebugDraw : public btIDebugDraw {
public:
	BulletGLDebugDraw();
	virtual void drawLine(const btVector3& from, const btVector3& to, const btVector3& color);
	virtual void drawContactPoint(const btVector3& pointOnB, const btVector3& normalOnB, btScalar distance,
			int lifeTime, const btVector3& color);
	virtual void reportErrorWarning(const char* msg);
	virtual void draw3dText(const btVector3& location, const char* textString);
	virtual void drawTransform(const btTransform& trans, btScalar orthoLen);
	virtual void setDebugMode(int debugMode) { this->debugMode = debugMode; }
	virtual int getDebugMode() const { return debugMode; }
	ShaderProgram* getShaderProgram() { return program; }
	void reallocLineVertices(int numAddVertices);
	void flush();

private:
	ShaderProgram* program;
	GLint vertexAttrib, colorAttrib;
	GLuint vertexColorsUniform, texturedUniform;
	int debugMode;

	int lineVerticesAllocCount, lineVerticesOffset;
	float* lineVertices;
	float* lineColors;
};

#endif /* BULLETGLDEBUGDRAW_H_ */

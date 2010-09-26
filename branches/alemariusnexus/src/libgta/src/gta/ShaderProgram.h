/*
 * ShaderProgram.h
 *
 *  Created on: 23.09.2010
 *      Author: alemariusnexus
 */

#ifndef SHADERPROGRAM_H_
#define SHADERPROGRAM_H_

#include <GL/glew.h>
#include "Shader.h"


class ShaderProgram {
public:
	static ShaderProgram* getCurrent();

public:
	ShaderProgram();
	void link();
	void attachShader(Shader* shader);
	void detachShader(Shader* shader);
	void makeCurrent();
	GLint getAttributeLocation(const char* name);
	GLint getUniformLocation(const char* name);

private:
	GLuint program;
};

#endif /* SHADERPROGRAM_H_ */
